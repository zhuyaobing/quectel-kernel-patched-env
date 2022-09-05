/*
 *  PikeOS thread abstraction layer
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

#ifndef __LOCKING_H__
#define __LOCKING_H__

/* NOTE: this must match PikeOS P4_ulock_t */
struct uk_mutex {
	volatile unsigned int lock;
	unsigned int wait_uid;
};

#define UK_MUTEX_MASK 0xfffff
#define UK_MUTEX_WAITERS (1U << 31)

/*
 * check mutex ownership:
 * returns true if \a uid is the current owner of \a m
 */
static inline int uk_mutex_locked(struct uk_mutex *m, unsigned int uid)
{
	return (m->lock & UK_MUTEX_MASK) == (uid & UK_MUTEX_MASK);
}

/*
 * mutex primitives
 * returns 0 on success
 * returns 1 when waiting is cancelled (uk_mutex_lock() only)
 * returns -P4_e_t on error
 */
int uk_mutex_lock(struct uk_mutex *mutex, unsigned int caller_uid);
int uk_mutex_unlock(struct uk_mutex *mutex, unsigned int caller_uid);
int uk_mutex_wait_cancel(unsigned int target_uid);

/*
 * set new mutex owner, used for lock stealing in exception handling
 */
void uk_mutex_set_owner(struct uk_mutex *mutex, unsigned int owner_uid);


/* user space events */

/* NOTE: this must match PikeOS P4_ulock_t */
struct uk_uevent {
	volatile unsigned int lock;
	unsigned int wait_uid;
};

#define UK_UEVENT_PENDING (1U << 0)
#define UK_UEVENT_WAITERS (1U << 31)

static inline void uk_uevent_init(struct uk_uevent *uevent)
{
	uevent->lock = 0;
	uevent->wait_uid = 0;
}

/*
 * user events (mimic p4_ev_*() behaviour)
 *
 * wait()
 *    If there are pending events, wait() consumes them immediately
 *    or wait() blocks until an event arrives.
 *    When wait() returns, all previous events are consumed.
 *    The wait() function must be used by a single thread only!
 *
 * signal()
 *    Signal marks a pending event, and wakes a possibly waiting thread.
 *    Every thread may use signal() for notification.
 *
 * returns 0 on success
 * returns 1 when waiting is cancelled
 * return -P4_e_t on error
 */
int uk_uevent_is_waiting(struct uk_uevent *e);
void uk_uevent_clear_pending(struct uk_uevent *uevent);
int uk_uevent_wait(struct uk_uevent *uevent);
int uk_uevent_wait_timeout(struct uk_uevent *e, unsigned long long timeout);
int uk_uevent_wait_timeout_abs(struct uk_uevent *e, unsigned long long timeout);
void uk_uevent_signal(struct uk_uevent *uevent);

#endif
