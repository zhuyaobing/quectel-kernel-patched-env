/*
 *  Helper-Library for P4-threads
 *
 *  Author: Alex Zuepke <azu@sysgo.de>
 *
 *  Copyright (C) 2003-2012 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#include "p4.h"
#include <linux/module.h>
#include <linux/compiler.h>
#include <asm/host_threads.h>
#include <asm/host_tasks.h>
#include <asm/host_exception.h>
#include <asm/debugout.h>
#include <asm/wrapper.h>
#include <asm/archtls.h>
#include <asm/pikeos_version.h>

/*
 * uk_thread_create() this creates threads
 *
 * return 0 on ok, !=0 on error
 */
int uk_thread_create(
	unsigned int threadno,
	const char *name,
	void *func,
	void *stack,
	void *tls,
	unsigned int prio,
	unsigned int exhandler_thread,
	unsigned long affinity,
	unsigned int arg1,
	void *arg2)
{
	P4_thread_create_t tc;
	P4_uid_t exhandler;
	void *argvect[2];
	P4_regs_t regs;
	P4_e_t err;

	/* prepare registers */
	argvect[0] = (void*)(uintptr_t)arg1;
	argvect[1] = arg2;
	p4_thread_arg(&regs, func, stack, 0, 2, argvect);
#if PIKEOS_API_VERSION < 0x40004
	__tls_set_ptr_in_regs(&regs, tls);
#endif

	/* determine exhandler */
	if (exhandler_thread == -1)
		exhandler = P4_UID_INHERIT;
	else
		exhandler = uk_uid_of_thread(exhandler_thread);

	/* delete possible remains of previous thread */
	p4_thread_delete(threadno);

	/* KMEM: need kernel memory */
	uk_kmem_try_thread();

	tc.name     = name;
	tc.context  = &regs;
	tc.prio     = prio;
	tc.tp_id    = P4_TIMEPART_INHERIT;
	tc.shortexh = P4_UID_INVALID;
	tc.fullexh  = exhandler;
	/* Threads should be able to receive from other e.g., the PSSW */
	tc.ipc_mask = P4_UID_ALL;
	/* Events are restricted to threads in the Linux Kernel */
	tc.ev_mask  = uk_uid_of_all_threads();

#if PIKEOS_API_VERSION < 0x40004
	err = p4_thread_create(threadno, &tc, P4_THREAD_CREATE_STOPPED);
#else
	err = p4_thread_create_syscall(threadno, &tc, P4_THREAD_CREATE_STOPPED, tls);
#endif
	while (err == P4_E_STATE) {
		/* Thread is still active, this may happen on SMP systems
		 * because p4_thread_delete doesn't work immediate. Getting
		 * P4_E_STATE means the thread number is already active, so we
		 * have to wait.
		 */
		cpu_relax();
#if PIKEOS_API_VERSION < 0x40004
		err = p4_thread_create(threadno, &tc, P4_THREAD_CREATE_STOPPED);
#else
		err = p4_thread_create_syscall(threadno, &tc, P4_THREAD_CREATE_STOPPED, tls);
#endif
	};

	if (err != P4_E_OK) {
		debugout("uk_thread_create: could not start thread %d, err = 0x%x", threadno, err);
		return -err;
	}

	p4_thread_ex_affinity(threadno, NULL, affinity);
	p4_thread_resume(threadno);

	return 0;
}

/*
 * uk_thread_delete() destroys a thread
 *
 * NOTE: This function may sleep.
 */
void uk_thread_delete(unsigned int threadno)
{
	p4_thread_delete(threadno);
}

void uk_set_tls(void *tls)
{
	p4_tls_register(tls);
}


void uk_thread_cancel(unsigned int threadno)
{
	p4_thread_ex_regs(threadno, NULL, NULL, NULL, P4_PRIO_KEEP, 0);
	/* NOTE: error code not checked */
}

/*
 * uk_thread_ev_signal(unsigned int threadno)
 *
 * sends an event to a linux kernel thread
 */
void uk_thread_ev_signal(unsigned int threadno)
{
	P4_e_t err;

	err = p4_ev_signal(uk_uid_of_thread(threadno));
	/* NOTE: error code not checked */

	if (unlikely(err != P4_E_OK))
		debugout("uk_thread_ev_signal(thr %d): failed with err = 0x%x", threadno, err);
}

/*
 * uk_thread_ev_wait(void)
 *
 * Wait for events
 */
void uk_thread_ev_wait(void)
{
	P4_e_t err;

	err = p4_ev_wait(P4_TIMEOUT_INFINITE, P4_EV_CONSUME_ALL, NULL);
	/* NOTE: error code not checked */

	if (unlikely(err != P4_E_OK))
		debugout("uk_thread_ev_wait: failed with err = 0x%x", err);
}

/*
 * uk_thread_ev_clear(void)
 *
 * Consumes all pending events without waiting
 */
void uk_thread_ev_clear(void)
{
	p4_ev_wait(P4_TIMEOUT_NULL, P4_EV_CONSUME_ALL, NULL);
	/* NOTE: error code not checked */
}

/*
 * uk_thread_sleep(nanoseconds)
 *
 * Sleep time in ns
 */
void uk_thread_sleep(unsigned long long nanosecs)
{
	p4_sleep(nanosecs);
}

/*
 * uk_thread_sleep_ev(nanoseconds)
 *
 * Sleep time in ns AND wait for events
 */
void uk_thread_sleep_ev(unsigned long long nanosecs)
{
	P4_e_t err;

	err = p4_ev_wait(nanosecs, P4_EV_CONSUME_ALL, NULL);

	if (unlikely((err != P4_E_OK) && (err != P4_E_TIMEOUT)))
		debugout("uk_thread_sleep_ev: failed with err = 0x%x", err);
}

/*
 * uk_thread_suspend(void)
 *
 * Sleep forever
 */
void uk_thread_suspend(void)
{
	while(1)
		p4_sleep(P4_TIMEOUT_INFINITE);
}

/*
 * uk_thread_irq_attach(irqno)
 *
 * Attach thread to an interrupts
 */
int uk_thread_irq_attach(unsigned int irqno)
{
	P4_e_t err;

	err = p4_int_attach(irqno);

	return -err;
}

/*
 * uk_thread_irq_detach()
 *
 * Detach thread to an interrupts
 */
void uk_thread_irq_detach(void)
{
	p4_int_detach();
}

/*
 * uk_thread_irq_wait(chain)
 *
 * Wait for interrupt to happen
 */
int uk_thread_irq_wait(int chain)
{
	P4_e_t err;

	err = p4_int_wait(P4_TIMEOUT_INFINITE, chain ? P4_INT_CHAIN : 0);

	if (err == P4_E_OK)
		return UK_ERR_OK;
	else if (err == P4_E_CANCEL)
		return UK_ERR_CANCEL;
	else
		return -err;
}
EXPORT_SYMBOL(uk_thread_irq_wait);

/*
 * uk_thread_event_mask(unsigned int uid)
 *
 * Attach thread to an event
 */
int uk_thread_event_mask(unsigned int uid)
{
	p4_ev_mask(uid);

	return 0;
}
EXPORT_SYMBOL(uk_thread_event_mask);

/*
 * uk_thread_event_wait(void)
 *
 * Wait for event to happen
 */
int uk_thread_event_wait(void)
{
	return p4_ev_wait(P4_TIMEOUT_INFINITE, P4_EV_CONSUME_ALL, 0);
}
EXPORT_SYMBOL(uk_thread_event_wait);

/*
 * uk_thread_event_signal(unsigned int uid)
 *
 * Wait for interrupt to happen
 */
int uk_thread_event_signal(unsigned int uid)
{
	return p4_ev_signal(uid);
}

/*
 * uk_get_time()
 *
 * Returns the current time in nanoseconds since system boot,
 * granularity depends on the target
 */
unsigned long long uk_get_time_ns(void)
{
	unsigned long long ns;

#if 0
	static DEFINE_SPINLOCK(timer_check_lock);
	static DEFINE_PER_CPU(unsigned long long, last_read);
	int cpu;

	/* check last reads over all CPUs */
	spin_lock(&timer_check_lock);
	ns =  p4_get_time();
	for_each_possible_cpu(cpu) {
		if (ns < per_cpu(last_read, cpu) || ns > per_cpu(last_read, cpu) + (unsigned long long)60*NSEC_PER_SEC) {
			printk(KERN_ERR "WARNING: time jump detected, current on CPU#%d: %llu, last read on CPU#%d: %llu\n",
				hard_smp_processor_id(), ns, cpu, per_cpu(last_read, cpu));

			show_stack(NULL, NULL);
		}
	}
	per_cpu(last_read, hard_smp_processor_id()) = ns;
	spin_unlock(&timer_check_lock);
#else
	ns = p4_get_time();
#endif
	return ns;
}

/*
 * uk_thread_call_on_stack()
 *
 * In the current thread, invoke "func" on given "stack" with FPU disabled
 */
void uk_thread_call_on_stack(void *func, void *stack, void *tls)
{
	P4_regs_t regs;

	/* PR#8391-p4: for the exception handler, only manage exception
	 * from threads of this task
	*/
	p4_ipc_mask(uk_uid_of_all_threads());

	/* invoke func with FPU disabled */
	p4_thread_arg(&regs, func, stack, 0, 0, NULL);
	__tls_set_ptr_in_regs(&regs, tls);
#if PIKEOS_API_VERSION >= 0x40004
	p4_tls_register(tls);
#endif
	p4_thread_set_regs(&regs, P4_PRIO_KEEP);
}

/*
 * uk_thread_cpu_id(void)
 *
 * return PikeOS CPUID of the current thread
 */
unsigned int uk_thread_cpu_id(void)
{
	return p4_my_cpuid();
}

/*
 * uk_thread_setaffinity() set a thread's PikeOS affinity
 */
void uk_thread_setaffinity(unsigned int thr, unsigned long affinity)
{
	p4_thread_ex_affinity(thr, NULL, affinity);
}

#if PIKEOS_API_VERSION < 0x40003
/*
 * uk_thread_register_rulock() register a thread's robust ulock lists
 */
void uk_thread_register_rulock(uk_rulist_t *rulist)
{
	P4_rulock_register_t *p = (P4_rulock_register_t *)rulist;

	p4_rulock_register_init(p);
	p4_ulock_register(p);
}
#endif

/*
 * uk_thread_set_prio() set a PikeOS thread's priority
 */
void uk_thread_set_prio(unsigned int thr, int prio)
{
	p4_thread_set_priority(thr, prio);
}


/*
 * uk_thread_set_my_prio() set priority of current thread
 */
void uk_thread_set_my_prio(unsigned int prio)
{
#if PIKEOS_API_VERSION < 0x40004
	p4_fast_set_prio(prio);
#else
	p4_tls_change_prio(prio);
#endif
}
/*
 * uk_thread_get_thread_attr() get some thread attributes.
 */
unsigned int uk_thread_get_thread_attr(unsigned int thr_num, unsigned int *thr_state_ptr,
       unsigned int *stopping_ptr, unsigned int *cpuid_ptr, unsigned int *uid_ptr, unsigned int *priority_ptr)
{
	P4_e_t ret;
	P4_thread_attr_t thr_attr;

	ret = p4_thread_get_attr(thr_num, &thr_attr);

	if (ret == P4_E_OK) {
		if (cpuid_ptr != 0) {
			*cpuid_ptr = thr_attr.cpuid;
		}
		if (thr_state_ptr != 0) {
			*thr_state_ptr = thr_attr.state;
		}
		if (uid_ptr != 0) {
			*uid_ptr = thr_attr.uid;
		}
		if (stopping_ptr != 0) {
			*stopping_ptr = thr_attr.stopping;
		}
		if (priority_ptr != 0) {
			*priority_ptr = thr_attr.priority;
		}
	}
	return ret;
}


/*
 * uk_thread_stop() stops the current thread
 */
unsigned int uk_thread_stop(unsigned int threadno)
{
	P4_e_t ret;

	ret = p4_thread_stop(threadno);
	return (unsigned int) ret;
}

/*
 * uk_thread_resume() resumes the current thread
 */
unsigned int uk_thread_resume(unsigned int threadno)
{
	P4_e_t ret;

	ret = p4_thread_resume(threadno);
	return (unsigned int) ret;
}

/*
 * uk_thread_nbr(void)
 *
 * return PikeOS thread number of the current thread
 */
unsigned int uk_thread_nbr(void)
{
	return p4_my_thread();
}

EXPORT_SYMBOL(uk_get_time_ns);
