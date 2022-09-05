/*
 *  PikeOS interrupt enable / disable abstraction layer
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 * 
 *  Copyright (C) 2011 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 *  The functions here are inlined or marked as __lockfunc to put them in their
 *  own section to support in_sched_functions() used by get_wchan().
 */

#include <linux/threads.h>
#include <linux/module.h>
#include <asm/irqflags.h>
#include <asm/kernthreads.h>
#include <asm/cache.h>
#include <asm/locking.h>

/*
 * per CPU "irq disabled" mutex
 *
 * the "IRQ flag state" is mutexes' data word
 * intialized to unlocked state (zero)
 */
struct mutex_table {
	struct uk_mutex m;
} ____cacheline_aligned_in_smp;

static struct mutex_table mutex_table[NR_CPUS];

static unsigned long __save_flags(void)
{
	unsigned int uid, cpu;
	struct uk_mutex *m;

	uid = kthread_uid();
	cpu = kthread_cpu();
	BUG_ON(cpu >= NR_CPUS);
	m = &mutex_table[cpu].m;

	return uk_mutex_locked(m, uid);
}

static unsigned long __irq_save(void)
{
	unsigned int uid, cpu;
	struct uk_mutex *m;
	int err;

	uid = kthread_uid();
	cpu = kthread_cpu();
	BUG_ON(cpu >= NR_CPUS);
	m = &mutex_table[cpu].m;

	if (uk_mutex_locked(m, uid))
		return 1;	/* already locked */

	/* try to get the mutex  */
	do {
		kthread_set_lock(1);
		err = uk_mutex_lock(m, uid);
		BUG_ON(err < 0);
		kthread_set_lock(0);
	} while (err != 0);

	return 0;
}

static void __irq_enable(void)
{
	unsigned int uid, cpu;
	struct uk_mutex *m;
	int err;

	uid = kthread_uid();
	cpu = kthread_cpu();
	BUG_ON(cpu >= NR_CPUS);
	m = &mutex_table[cpu].m;

	if (!uk_mutex_locked(m, uid))
		return;		/* not locked by us */

	err = uk_mutex_unlock(m, uid);
	BUG_ON(err != 0);
}

static void __irq_restore(unsigned long flags)
{
	unsigned int uid, cpu;
	struct uk_mutex *m;
	int err;

	uid = kthread_uid();
	cpu = kthread_cpu();
	BUG_ON(cpu >= NR_CPUS);
	m = &mutex_table[cpu].m;

	/* interrupts must be locked by us */
	BUG_ON(!uk_mutex_locked(m, uid));
	if (flags != 0)
		return;

	err = uk_mutex_unlock(m, uid);
	BUG_ON(err != 0);
}

static int __irq_lock(void)
{
	unsigned int uid, cpu;
	struct uk_mutex *m;
	int err;

	uid = kthread_uid();
	cpu = kthread_cpu();
	BUG_ON(cpu >= NR_CPUS);
	m = &mutex_table[cpu].m;

	BUG_ON(uk_mutex_locked(m, uid));

#if 0
	do {
		kthread_set_lock(1);
		err = uk_mutex_lock(m, uid);
		BUG_ON(err < 0);
		kthread_set_lock(0);
	} while (err != 0);
#else
	/* no support for preemption of IRQ threads */
	err = uk_mutex_lock(m, uid);
	BUG_ON(err < 0);
	/* UK_ERR_CANCEL must be forwarded to do_IRQ */
	return err;
#endif
}

static void __irq_unlock(void)
{
	unsigned int uid, cpu;
	struct uk_mutex *m;
	int err;

	uid = kthread_uid();
	cpu = kthread_cpu();
	BUG_ON(cpu >= NR_CPUS);
	m = &mutex_table[cpu].m;

	BUG_ON(!uk_mutex_locked(m, uid));

	err = uk_mutex_unlock(m, uid);
	BUG_ON(err != 0);
}

#if 0
void __irq_save_cancel(unsigned int target_uid)
{
	int err;

	if (...grab_lock) {
		grab_lock = 0;

		err = uk_mutex_wait_cancel(target_uid);
		BUG_ON(err != 0);
	}
}
#endif

unsigned long __lockfunc arch_local_save_flags(void)
{
	if (!kthread_is_irq())
		return __save_flags();

	return kthread_get_lock();
}
EXPORT_SYMBOL(arch_local_save_flags);

unsigned long __lockfunc arch_local_irq_save(void)
{
	int old;

	if (!kthread_is_irq())
		return __irq_save();

	old = kthread_get_lock();
	kthread_set_lock(1);
	return old;
}
EXPORT_SYMBOL(arch_local_irq_save);

void __lockfunc arch_local_irq_enable(void)
{
	if (!kthread_is_irq())
		return __irq_enable();

	kthread_set_lock(0);
}
EXPORT_SYMBOL(arch_local_irq_enable);

void __lockfunc arch_local_irq_restore(unsigned long flags)
{
	if (!kthread_is_irq())
		return __irq_restore(flags);

	kthread_set_lock(flags);
}
EXPORT_SYMBOL(arch_local_irq_restore);

/* called from do_IRQ to serialize further interrupts */
int __lockfunc arch_local_irq_lock(void)
{
	int ret;

	BUG_ON(!kthread_is_irq());
	BUG_ON(kthread_get_lock() != 0);
	ret = __irq_lock();
	if (ret) return ret;

	kthread_set_lock(1);
	return 0;
}

void __lockfunc arch_local_irq_unlock(void)
{
	BUG_ON(!kthread_is_irq());
	BUG_ON(kthread_get_lock() != 1);
	kthread_set_lock(0);
	__irq_unlock();
}

/* Called by THREAD_KPAGER to steal a CPU's IRQ lock, otherwise the KPAGER
 * will deadlock on trying to obtain the IRQ lock on printk()s.
 *
 * The KPAGER thread runs on CPU #0 always.
 *
 * Stealing means: we just register us (the KPAGER) as lock owner and set
 * the waiters flag to prevent any problems.
 * The unsteal operation restores the original lock owner, but also sets the
 * waiters flag.
 */
unsigned int __lockfunc arch_local_irq_steal(unsigned int faulting_uid)
{
	struct uk_mutex *m;

	BUG_ON(kthread_thread_id() != THREAD_KPAGER);
	BUG_ON(kthread_cpu() != 0);
	BUG_ON((faulting_uid & UK_MUTEX_MASK) == 0);
	m = &mutex_table[0].m;

	if (!uk_mutex_locked(m, faulting_uid))
		return 0;

	/* become new mutex owner */
	uk_mutex_set_owner(m, kthread_uid());

	return faulting_uid;
}

void __lockfunc arch_local_irq_unsteal(unsigned int old_owner)
{
	struct uk_mutex *m;

	BUG_ON(kthread_thread_id() != THREAD_KPAGER);

	if (old_owner) {
		BUG_ON(kthread_cpu() != 0);
		m = &mutex_table[0].m;
		BUG_ON(!uk_mutex_locked(m, kthread_uid()));

		uk_mutex_set_owner(m, old_owner);
	}
}

/* Unlocks the mutex if the mutex is "owned" by the mutex_lock_uid
 * otherwise leave the mutex as is.
 * The mutex_lock_uid is the thread uid that is locking the cpu mutex.
 */
void __lockfunc arch_local_unlock_owned_mutex(int cpu_idx, unsigned int mutex_lock_uid)
{
	struct uk_mutex *m;
	int err;

	BUG_ON(cpu_idx >= NR_CPUS);
	m = &mutex_table[cpu_idx].m;

	if (uk_mutex_locked(m, mutex_lock_uid)) {
	   err = uk_mutex_unlock(m, mutex_lock_uid);
	   BUG_ON(err != 0);
	}
}
