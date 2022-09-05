/*
 *  PikeOS locking (mutex) abstraction layer
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 * 
 *  Copyright (C) 2011 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#include <p4.h>
#include <asm/locking.h>
#include <asm/host_threads.h>
#include <asm/host_exception.h>
#include <asm/errno.h>

/* instead of including <linux/spinlock.h> just take the __lockfunc define to
 * support skipping of locking function entries in wchan
 */
#define __lockfunc __attribute__((section(".spinlock.text")))

#define CAST(m) ((P4_atomic_t*)(m))
#define CAS(m, old, new) p4_atomic_cas(CAST(m), (old), (new))

#undef LOCK_DEBUG


/*
 * Lock mutex
 */
int __lockfunc uk_mutex_lock(
	struct uk_mutex *m,
	unsigned int caller_uid)
{
	P4_ulock_t *mutex = (P4_ulock_t *)m;
	P4_uid_t state;
	P4_e_t err;

#ifdef LOCK_DEBUG
	if (!p4_uid_eq(p4_my_uid(), caller_uid))
		BREAKPOINT();
#endif

	caller_uid &= UK_MUTEX_MASK;

	/* try to acquire mutex */
again:
	state = mutex->lock;
	if (state == P4_ULOCK_FREE) {
		if (CAS(&mutex->lock, P4_ULOCK_FREE, caller_uid))
			goto out_ok;
	}

	/* let kernel handle lock contention */
	err = p4_ulock_wait(P4_TIMEOUT_INFINITE, &mutex->lock, P4_ULOCK_MUTEX, 0, NULL);
	switch (err) {
		case P4_E_STATE:
			goto again;
		case P4_E_CANCEL:
			return UK_ERR_CANCEL;
		case P4_E_OK:
			break;
		default:
			return -err;
	}

out_ok:
	p4_atomic_acquire_barrier();
	return UK_ERR_OK;
}

/*
 * Unlock mutex
 */
int __lockfunc uk_mutex_unlock(
	struct uk_mutex *m,
	unsigned int caller_uid)
{
	P4_ulock_t *mutex = (P4_ulock_t *)m;
	P4_uid_t state;
	P4_e_t err;

#ifdef LOCK_DEBUG
	if (!p4_uid_eq(p4_my_uid(), caller_uid))
		BREAKPOINT();
#endif

	caller_uid &= UK_MUTEX_MASK;

	p4_atomic_release_barrier();

	/* try uncontended unlock */
	state = mutex->lock;
	if (state == caller_uid) {
		if (CAS(&mutex->lock, caller_uid, P4_ULOCK_FREE)) {
			return UK_ERR_OK;
		}
	}

	/* contended unlock */
	err = p4_ulock_wake(&mutex->lock, P4_ULOCK_MUTEX, 0, NULL);

	return -err;
}

/*
 * Cancel blocking on mutex
 */
int uk_mutex_wait_cancel(
	unsigned int target_uid)
{
	P4_thr_t thr;
	P4_e_t err;

	thr = P4_UID_GET_THREAD(target_uid);
	err = p4_thread_ex_regs(thr, NULL, NULL, NULL, P4_PRIO_KEEP, 0);

	return -err;
}

/*
 * Set new mutex owner (used during lock stealing in exception handling)
 */
void uk_mutex_set_owner(
	struct uk_mutex *m,
	unsigned int owner_uid)
{
	P4_ulock_t *mutex = (P4_ulock_t *)m;
	
	owner_uid &= UK_MUTEX_MASK;

	/* always set waiters flags */
	owner_uid |= P4_ULOCK_WAITERS;

	p4_atomic_release_barrier();
	mutex->lock = owner_uid;
	p4_atomic_acquire_barrier();
}

/* --------- user events --------- */

/*
 * Wait for user event and consume all pending events
 */
void uk_uevent_clear_pending(struct uk_uevent *e)
{
	P4_ulock_t *uevent = (P4_ulock_t *)e;

	if (uevent->lock == UK_UEVENT_PENDING) {
		// 0|1 * signal pending -> 0|0
		uevent->lock = 0;
	}
}

int uk_uevent_is_waiting(struct uk_uevent *e)
{
	P4_ulock_t *uevent = (P4_ulock_t *)e;

	return (uevent->lock == UK_UEVENT_WAITERS);
}

int uk_uevent_wait(struct uk_uevent *e)
{
	return uk_uevent_wait_timeout(e, P4_TIMEOUT_INFINITE);
}

int uk_uevent_wait_timeout(struct uk_uevent *e, unsigned long long timeout)
{
	P4_ulock_t *uevent = (P4_ulock_t *)e;
	P4_uint32_t state;
	P4_e_t err;

again:
	state = uevent->lock;
	if (state == 0) {
		// 0|0 -> 1|0 * block on empty event
		if (CAS(&uevent->lock, state, UK_UEVENT_WAITERS) == 0)
			goto again;	/* CAS failed */

		err = p4_ulock_wait(timeout, &uevent->lock, 0, UK_UEVENT_WAITERS, NULL);
		switch (err) {
			case P4_E_STATE:
				goto again;
			case P4_E_CANCEL:
				err = -UK_ERR_CANCEL;	/* becomes positive */
				break;
			case P4_E_TIMEOUT:
				err = -ETIME;
				break;
			case P4_E_OK:
			default:
				break;
		}
		// the partner already re-set the lock state to 0|0
	} else {
		// 0|1 -> 0|0 * consume single event
		// 1|0 or 1|1 -> lock is bad -> ignored -> 0|0
		err = P4_E_OK;
	}

	uevent->lock = 0;
	p4_atomic_acquire_barrier();
	return -err;
}

int uk_uevent_wait_timeout_abs(struct uk_uevent *e, unsigned long long timeout)
{
	return uk_uevent_wait_timeout(e, timeout | P4_TIMEOUT_ABSOLUTE);
}

void uk_uevent_signal(struct uk_uevent *e)
{
	P4_ulock_t *uevent = (P4_ulock_t *)e;
	P4_uint32_t state;
	P4_e_t err;

	p4_atomic_release_barrier();

again:
	state = uevent->lock;
	if (state == 0) {
		// 0|0 -> 0|1 * signal single event
		if (CAS(&uevent->lock, state, UK_UEVENT_PENDING) == 0)
			goto again;	/* CAS failed */
	} else if (state == UK_UEVENT_WAITERS) {
		// 1|0 -> 1|1 * wake sleeper
		if (CAS(&uevent->lock, state, UK_UEVENT_WAITERS | UK_UEVENT_PENDING) == 0)
			goto again;	/* CAS failed */

		err = p4_ulock_wake(&uevent->lock, 0, 0, NULL);
	} else {
		// 0|1 * signal already pending
		// 1|1 * someone else is waking up
	}
}
