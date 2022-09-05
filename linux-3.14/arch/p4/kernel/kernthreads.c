/*
 *  PikeOS thread abstraction layer
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 * 
 *  Copyright (C) 2003-2011 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#include <linux/types.h>
#include <linux/module.h>
#include <linux/bootmem.h>
#include <linux/spinlock.h>
#include <linux/cpumask.h>
#include <asm/kernthreads.h>
#include <asm/cache.h>
#include <asm/irq.h>
#include <asm/internals.h>
#include <asm/host_threads.h>
#include <asm/host_exception.h>
#include <asm/host_tasks.h>
#include <asm/debugout.h>
#include <asm/locking.h>
#include <asm/pikeos_version.h>

/* the maximum number of threads PikeOS can handle */
#define THREAD_MAX 511
/* the maximum number of CPUs PikeOS can handle */
#define NATIVE_CPU_MAX 32

/*
 * internal thread table
 */
struct kthread_table {
	struct kthread_tls tls;	/* TLS accessible parts (UID, CPU ID) */

	bool used;
	char *stack;		/* allocated stack */
	int irq;			/* associated IRQ */

#if PIKEOS_API_VERSION < 0x40003
	/* PikeOS robust mutex associated with that thread */
	uk_rulist_t rulist;
#endif
} ____cacheline_aligned_in_smp;

static unsigned int native_max_threads;
static struct kthread_table kthread_table[THREAD_MAX];

static spinlock_t kthread_table_lock = __SPIN_LOCK_UNLOCKED(kthread_table_lock);

/* native CPU ID management */
static unsigned int native_cpu_num;	/* != 0 to activate VMAPI_CALL check */
static unsigned int native_cpu_ids[NATIVE_CPU_MAX];

unsigned int kthread_native_cpu_num(void)
{
	return native_cpu_num;
}

unsigned int kthread_native_cpu_id(unsigned int cpu)
{
	BUG_ON(cpu > native_cpu_num);
	return native_cpu_ids[cpu];
}

/* IRQ to thread relationship */
static int irq2thr[NR_IRQS];

static int __kthread_get_irq_thr(unsigned int irq)
{
	BUG_ON(irq > NR_IRQS);
	return irq2thr[irq];
}

static unsigned int kthread_get_irq_thr(unsigned int irq)
{
	int thr;

	thr = __kthread_get_irq_thr(irq);
	BUG_ON(thr < 0);

	return thr;
}

#define THREAD_STACK_SIZE 4096

void __init kthreads_init(void)
{
	struct kthread_table *kt;
	unsigned long cpu_mask;
	unsigned int cpu;
	unsigned int i;

	/* +1 for prio 0 being unusable */
	if (uk_task_mcp() < PRIO_RANGE + 1)
		die_early("kthreads_init: MCP priority too low!");

	native_max_threads = uk_task_max_threads();
	if (native_max_threads < THREAD_IRQ + 1)
		die_early("kthreads_init: number of threads too low!");

	/* CPU detection code */
	cpu_mask = uk_task_cpu_mask();
	native_cpu_num = 0;
	for (cpu = 0; cpu < NATIVE_CPU_MAX; cpu++) {
		if (cpu_mask & (1UL << cpu)) {
			native_cpu_ids[native_cpu_num] = cpu;
			native_cpu_num++;
			
		}
	}
	BUG_ON(native_cpu_num == 0);
	/* restrict CPUs if we have less */
	if (native_cpu_num > NR_CPUS)
		native_cpu_num = NR_CPUS;

	/* register our TLS area ... */
	uk_set_tls(&kthread_table[0]);

	/* ... and assign us (KPAGER) to all CPUs */
	uk_thread_setaffinity(0, cpu_mask);

	/* set the correct priority for this thread */
	uk_thread_set_prio(0, uk_task_mcp() + PRIO_KPAGER);

	for (i = 0; i < THREAD_MAX; i++) {
		kt = &kthread_table[i];

		kt->tls.tls_ptr = &kt->tls;
		kt->tls.uid = uk_uid_of_thread(i);
#if PIKEOS_API_VERSION < 0x40003
		kt->tls.rulist = &kt->rulist;
#else
		kt->tls.rulock_trying = NULL;
		kt->tls.rulock_list_next = &kt->tls.rulock_list_next;
		kt->tls.rulock_list_prev = &kt->tls.rulock_list_next;
#endif
		kt->tls.errno = 0;

		kt->tls.thread_id = i;
#ifdef CONFIG_SMP
		kt->tls.cpu = 0;
		kt->tls.new_cpu = 0;
#endif
		kt->tls.grab_lock = 0;

#if PIKEOS_API_VERSION >= 0x40003
		kt->tls.preempt_handler = 0;
		kt->tls.preempt_stack = 0;
		kt->tls.preempt_regs = 0;
		kt->tls.preempt_flags = 0;
		kt->tls.sysemu_handler = 0;
		kt->tls.sysemu_stack = 0;
		kt->tls.sysemu_regs = 0;
		kt->tls.sysemu_flags = 0;
		kt->tls.except_handler = 0;
		kt->tls.except_stack = 0;
		kt->tls.except_regs = 0;
		kt->tls.except_flags = 0;
#endif

		/* only the first (KPAGER) + NR_CPUS (MAIN) threads are used */
		kt->used = 0;
		kt->stack = NULL;
		kt->irq = -1;

#if PIKEOS_API_VERSION < 0x40003
		/* initialize robust mutexes for that thread */
		uk_thread_register_rulock(&kt->rulist);
#endif
	}

	/* KPAGER */
	kthread_table[0].used = 1;

	/* per CPU MAINs */
	for (cpu = 0; cpu < NR_CPUS; cpu++) {
		kt = &kthread_table[THREAD_MAIN + cpu];

#ifdef CONFIG_SMP
		kt->tls.cpu = cpu;
		kt->tls.new_cpu = cpu;
#endif
		kt->used = 1;
	}

	/* clear IRQ assignment */
	for (i = 0; i < FIRST_ASYNC_IRQ; i++)
		irq2thr[i] = -1;
	for (; i < NR_IRQS; i++)
		irq2thr[i] = -2;

	/* register available CPUs early, nr_cpu_ids depends on this */
	for (cpu = 0; cpu < native_cpu_num; cpu++)
		set_cpu_possible(cpu, true);
}

/*
 * called by the first thread (KPAGER) to disable its FPU
 */
void __init kthreads_call_on_new_stack(void *func)
{
	struct kthread_table *kt;

	kt = &kthread_table[THREAD_KPAGER];

	kt->stack = alloc_bootmem_pages(THREAD_STACK_SIZE);
	if (kt->stack == NULL)
		die_early("kthreads_call_on_new_stack: could not allocate stack");

	/* invoke func on new stack with correct TLS setting */
	uk_thread_call_on_stack(func, &kt->stack[THREAD_STACK_SIZE], kt);
	/* does not return */
}

/*
 * Starts a MAIN-thread on given CPU
 */
void __cpuinit kthreads_boot_cpu(unsigned int cpu, void *func, void *stack)
{
	struct kthread_table *kt;
	unsigned long cpu_mask;
	char name[16];
	int err;

	/* FIXME: SMP: better return an error here? */
	if (cpu >= native_cpu_num)
		panic("tried to boot CPU not available!");

	BUG_ON(cpu >= NR_CPUS);
	sprintf(name, "kernel CPU#%d", cpu);

	kt = &kthread_table[THREAD_MAIN + cpu];
#if PIKEOS_API_VERSION >= 0x40003
	/* set dummy handler to BUG on unwanted preemption */
	kt->tls.preempt_handler = (void *) dummy_preempt_handler;
#endif

	cpu_mask = 1UL << kthread_native_cpu_id(cpu);
	err = uk_thread_create(THREAD_MAIN + cpu, name, func, stack, kt,
						uk_task_mcp() + PRIO_MAIN, THREAD_KPAGER,
						cpu_mask, 0, NULL);
	if (err)
		die_early("kthreads_boot_cpu: could not start idle thread for CPU %d", cpu);
}

/*
 * Get a free thread
 * Search the thread table for free threads and lock them
 */
static inline unsigned int kthreads_alloc_thread(void)
{
	unsigned int thr;

	for (thr = 0; thr < native_max_threads; thr++) {
		if (kthread_table[thr].used == 0) {
			kthread_table[thr].used = 1;
			break;
		}
	}

	if (thr < native_max_threads)
		return thr;
	return 0;	/* no free found */
}

/*
 * Release thread in the thread table
 */
static inline void kthreads_free_thread(unsigned int thr)
{
	kthread_table[thr].used = 0;
}

/*
 * Link kthread with IRQ (and vice versa)
 */
static inline void kthreads_link_irq(unsigned int thr, unsigned int irq)
{
	BUG_ON(kthread_table[thr].irq != -1);
	BUG_ON(irq2thr[irq] >= 0);

	kthread_table[thr].irq = irq;
	irq2thr[irq] = thr;
}

/*
 * Unlink kthread with IRQ (and vice versa)
 */
static inline void kthreads_unlink_irq(unsigned int thr, unsigned int irq)
{
	BUG_ON(kthread_table[thr].irq != irq);
	BUG_ON(irq2thr[irq] != thr);

	kthread_table[thr].irq = -1;
	irq2thr[irq] = -1;
}

/*
 * Starts an IRQ-thread
 *
 * returns -ENOMEM on error
 */
int enable_irq_thread(unsigned int irq, const char *name, void (*func)(unsigned int, void*), void *arg)
{
	struct kthread_table *kt;
	unsigned long cpu_mask;
	unsigned long flags;
	unsigned long stack;
	unsigned int thr;
	int err;

	BUG_ON(irq >= NR_IRQS);

	/* allocate stack for thread */
	/* this function may be called in interrupt context, thus we cannot go
	 * to sleep
	 */
	stack = __get_free_page(GFP_ATOMIC);
	if (stack == 0)
		return -ENOMEM;


	spin_lock_irqsave(&kthread_table_lock, flags);

	err = -ENOMEM;
	thr = kthreads_alloc_thread();
	if (thr == 0)
		goto out;

	kthreads_link_irq(thr, irq);

	kt = &kthread_table[thr];
	kt->stack = (char*) stack;

	/* FIXME: SMP: interrupts are assigned to CPU #0 for now */
	cpu_mask = 1UL << kthread_native_cpu_id(0);
#ifdef CONFIG_SMP
	kt->tls.cpu = 0;
	kt->tls.new_cpu = 0;
#endif

	err = uk_thread_create(thr, name, func,
			(void *) &kt->stack[THREAD_STACK_SIZE], kt,
			uk_task_mcp() + PRIO_IRQ, THREAD_KPAGER,
			cpu_mask, irq, arg);

	if (err == 0)
		goto out;

	kt->stack = NULL;

	kthreads_unlink_irq(thr, irq);
	kthreads_free_thread(thr);

out:
	spin_unlock_irqrestore(&kthread_table_lock, flags);

	if (err != 0)
		free_page(stack);

	return err;
}
EXPORT_SYMBOL(enable_irq_thread);

/*
 * Starts an IRQ-thread for a console with the highest priority.
 */
int enable_irq_console_thread(unsigned int irq, const char *name, void (*func)(unsigned int, void*), void *arg)
{
	int ret;

	ret = enable_irq_thread(irq, name, func, arg);
	if (ret == 0) {
		uk_thread_set_prio(get_irq_threadid(irq),
				   uk_task_mcp() + PRIO_IRQ_CONSOLE);
	}

	return ret;
}
EXPORT_SYMBOL(enable_irq_console_thread);

/*
 * Stops an IRQ-thread
 *
 * NOTE: Mustn't be called from IRQ context. Also make sure that the interrupt
 *       thread is in a known state (e.g. blocking). If it's executing do_IRQ
 *       calling uk_thread_delete may break the locking mechanism.
 */
void disable_irq_thread(unsigned int irq)
{
	struct kthread_table *kt;
	unsigned long flags;
	unsigned int thr;
	unsigned int ret;
	unsigned int thr_state;
   unsigned int thr_pre_stop_state;
   unsigned int thr_stopping;
	unsigned int thr_cpu;
	unsigned int thr_uid;
   volatile unsigned int *grab_lock_ptr;
   unsigned int outer_loop_count;
   unsigned int outer_loop_valid;
   unsigned int wait_count;
   unsigned int wait_loop_valid;
   unsigned int stop_count;
   unsigned int pre_grab_lock;

	if (kthread_is_irq()) {
		/* uk_thread_delete may sleep */
		die_runtime("disable_irq_thread(): tried to disable IRQ in IRQ handler");
	}

	spin_lock_irqsave(&kthread_table_lock, flags);

	thr = kthread_get_irq_thr(irq);
	kt = &kthread_table[thr];

	/* Stop the thread to delete first before deleting.
	 *
	 * This is done to ensure that the thread to delete is not executing on
	 * the other CPU when it is deleted.
	 * The thread to delete could be in the middle of a mutex protected code
	 * section. If it is deleted there then it does not clean up the mutex flags.
	 *
	 * For a single cpu system this problem does not occur since the irq thread to delete
	 * has a higher priority than the "boss" thread that is deleting it.
	 * i.e. The irq thread will have finished executing and be in a waiting state before
	 * the "boss" thread deletes it.
	 *
	 * For a multiple cpu system this is not the case.
	 * For TBM3 the affinity (which cpus that a thread can run on) causes this problem
	 * to occur more often. The csp is dedicated to CPU0 and the Linux irq threads are
	 * dedicated to CPU0. This means that the Linux "boss" thread (even though it has a
	 * lower priority than the irq thread) can be running on CPU1 while the Linux irq
	 * thread on CPU0 could be swapped out by the csp.
	 *
	 * This has sometimes caused a TPA kernel panic in do_IRQ() in
	 * arch/p4/kernel/irq.c when TPA is starting.
	 * The grab_lock in the tls (thread local storage) was not
	 * always cleared when a thread was deleted.
	 * The muxa client on TPA gets cpu starved and then repeatedly
	 * calls vmfile_tty.c .startup and .shutdown functions.
	 * The .startup function creates two threads one for rx and one for tx.
	 * The .shutdown function deletes these threads.
	 * When .startup function is called next time it uses the same area for
	 * the tls as previously used for the two threads that have just been
	 * deleted since their tls is again available in the array of the
	 * kthread_table[].
	 * The problem was that this tls data was not reinitialised
	 * and sometimes the grab_lock was still set which caused the kernel panic.
	 *
	 * GOTCHA. Fixing grab_lock cured that problem but there was still the cpu
	 * mutex_table in irqflags.c that also had its mutex locked.
	 *
	 *
	 * The PikeOS function to stop a thread does not occur immediately in a
	 * multiple cpu system. The thread to be stopped will ONLY change to the
	 * stopped state when the thread changes state. However the thread when not
	 * in the stopped state will also have its stopping flag set.
	 * It makes sense when you think that a stopped thread can be resumed.
	 * In which case you would want the thread to continue what it was doing
	 * before it was stopped.
	 * E.g. If the thread was waiting for an interrupt or waiting for an event
	 * then you would want it to keep on waiting for an interrupt or an event.
	 * The stopping flag implementation makes it simple for PikeOS to resume
	 * where it was.
	 * During testing the stopping thread has been observed in the states
	 * 1 (ready), 2 (current), 5 (wait_rx), 12 (wait_ulock) with the stopping
	 * flag set. After some time the stopping thread states 1 (ready) and
	 * 2 (current) will change to state 3 (stopped) and the stopping flag is cleared.
	 *
	 * For multiple cpus need to repeatedly check that the stopping
	 * thread is not in the state of "current" AND its stopping flag set.
	 * i.e. that it is still not executing on the other cpu.
	 * The stopping thread if executing will eventually change state due to
	 * being swapped out by time partitioning or some higher prio thread.
	 * The stopping thread will instead of going to the ready state will go
	 * to the stopped state and its stopping flag will be cleared.
	 *
	 *
	 * GOTCHA.
	 * Clearing the thread grab_lock and the cpu mutex_table mutex solved many problems
	 * however there was still another problem in do_IRQ() in arch/p4/kernel/irq.c.
	 * In  do_IRQ() there are functions irq_enter(); and irq_exit(); which save other flags.
	 * In particular the preempt_count() in the thread_info of the CPU that the irq thread
	 * is running on.
	 * Sometimes when the irq thread is deleted it is inside irq_enter() and irq_exit().
	 * This means that the preempt_count of the CPU the irq thread is running on is not
	 * cleared properly.
	 * i.e. In our case we have a "boss" thread on CPU1 deleting an irq thread on CPU0 which
	 * causes the normal task on CPU0 thread_info to have a corrupt preempt_count.
	 * This causes kernel panic messages mainly like "BUG: scheduling while atomic:".
	 * Due to the scheduling, the task that then runs on CPU0 then seems to be the cause of
	 * the error. i.e. A wild goose chase looking for a bug in the wrong code.
	 *
	 * Solution
	 * Want to stop and delete the irq thread but only after the irq thread has exited its critical
	 * section. i.e. The irq thread has correctly restored the preempt_count(), cpu mutex_table,
	 * thread grab_lock, resumed the stopped CPU main thread, rcu_flags.
	 * Note that the critical section inside the kernel do_IRQ of the irq thread "should be" well
	 * behaved and not hang. After all the irq thread is behaving like an isr (interrupt service routine).

	 * NOTE: Beware of priority inversions between threads running on different CPUs. In particular
	 * TPA Linux with low priority can easily have either or both CPUs swapped out by Telematics Linux,
	 * csp, vbd, ssm.
	 * For example during the time between reading the grab_lock of the irq thread on the other cpu
	 * and then sending the thread_stop command, the grab_lock of the irq thread can have changed.
	 *
	 * To get around this I will use a series of thread_stop and thread_resume commands.
	 * Once the irq thread is stopped then its grab_lock can be read since it will not get changed.
	 * If the grab_lock is locked then resume the irq thread so it can finish and exit its critical
	 * section.
	 *
	 * GOTCHA. The uk_thread_sleep() for 100us waiting for the irq thread to stop causes problems.
	 * The "boss" thread stops the irq thread but while it is in uk_thread_sleep() Linux can
	 * swap out the task on CPU1 before the "boss" thread can resume() the irq thread if the
	 * grab_lock is set.
	 * (Tested by waiting for 900us)
	 * uk_thread_sleep() documentation states the resolution is based on
	 * p4_kinfo_get_timeout_resolution which is the PikeOS tick of 1.000 ms.
	 * In practice I have measured a resolution of approx 1/3 of a PikeOS tick being approx 333 us.
	 *
	 * GOTCHA. By putting the irq thread in the stopped state it gets rescheduled behind other
	 * Linux irq threads. Usually it works but sometimes another Linux irq thread activates and then
	 * we get kernel problems. By ensuring the irq thread is in a blocking state before stopping
	 * the irq thread fixes this problem.
	 *
	 * GOTCHA.
	 * The stopping bit can be set in the irq thread on the other cpu with its thread state "current".
	 * The irq thread state will get changed when it gets scheduled. Now when uk_thread_get_thread_attr()
	 * is called from the "boss" thread on CPU1 to get the state of the irq thread on CPU0 there is a
	 * PikeOS task switch (prio 255) on CPU0 while the PikeOS gets the irq threads attributes. Then
	 * there is the restore task switch back again which causes the scheduling or the irq thread which
	 * now changes state from "current" with stopping bit set to "stopped" with stopping bit cleared.
	 *
	 * GOTCHA.
	 *  pr_info() takes a long time. Approx 115 bytes per 10 ms. Printing data before fixing problem
	 *  often means the problem disappears. e.g. the grab_lock is now cleared
	 *
	 * Solution Implementation
	 *
	 * 1. Start Outer loop. Allow up to 3 attempts;
	 *
	 * 2. Loop and wait and test until the grab_lock on the irq thread running
	 *	 on the other CPU becomes not locked. (usually it is not locked) Also test in the loop
	 *    until the irq thread goes to its blocking state.
	 *
	 * 3. Stop the irq thread. Loop and check that the irq thread has stopped. (thread_stop is
	 *    not a synchronous function when having 2 cpus.)
	 *
	 * 4. Check that the grab_lock of the irq thread is still not locked !!!!
	 *    (The irq thread before the stop thread took effect could have locked the grab_lock.)
	 *
	 * 5. If the grab_lock is not locked then we can exit the Outer loop.
	 *
	 * 6. If the grab_lock is locked then resume the irq thread so that it can continue and
	 *    exit its critical section. Go back to the start of the outer loop.
	 *
	 * 7. The irq thread is stopped and its grab_lock is not locked.
	 *
	 * Testing.
	 * Testing was done by adding a function that does a cpu spin loop inside the
	 * critical section of do_IRQ(). To stop Linux from crashing due to slowness, the spin loop
	 * was only called for irq = 525 which was the fufu_tx thread for TPA.
	 *
	 */

	grab_lock_ptr = &kt->tls.grab_lock;
	pre_grab_lock = *grab_lock_ptr;
	uk_thread_get_thread_attr(thr, &thr_pre_stop_state, 0, 0, 0, 0);
	stop_count = 0;
	wait_count = 0;
	outer_loop_valid = 1;

	for (outer_loop_count = 0; ((outer_loop_count < 3) && outer_loop_valid); ++outer_loop_count) {
		wait_loop_valid = 1;
		for (wait_count = 0; ((wait_count < 1000000) && wait_loop_valid); ++wait_count ) {
			ret = uk_thread_get_thread_attr(thr, &thr_state, &thr_stopping, &thr_cpu, &thr_uid, 0);
			if (ret == 0) {
				if (*grab_lock_ptr != 0) {
					/* grab_lock is locked. Let irq cpu run to exit its critical section */
					switch (thr_state)
					{
						case 2:	 /*  2 = irq thread state "current" */
						case 10:  /* 10 = irq thread state "helping". Also non blocking thread.
									  * irq thread is executing so do not wait, loop and test again
									  */
							break;
						case 1:	 /*  1 = irq thread state "ready"
									  *  irq thread has been swapped out so wait for it to swap back
									  *  before testing again.
									  */
							uk_thread_sleep(1000ULL * 100ULL * 3ULL);  /* 300 us sleep */
							break;
						case 3:	 /*  3 = thread state "stopped" */
						default:  /*  Blocking thread state
									  *  Should not occur with grab_lock locked and thread state blocking.
									  *  Exit wait loop
									  */
							wait_loop_valid = 0;
					}
				}
				else {
					/* grab_lock is not locked. */
					switch (thr_state)
					{
						case 2:	 /*  2 = thread state "current" */
						case 10:  /* 10 = thread state "helping". Also non blocking thread.
									  *  irq thread is executing, it could be just about to enter the
									  *  critical section so do not wait, loop and test again
									  */
							break;
						case 1:	 /*  1 = thread state "ready"	*/
						case 3:	 /*  3 = thread state "stopped" */
						default:  /*  Blocking thread state
									  *  Exit wait loop
									  */
							wait_loop_valid = 0;
					}
				}
			}
			else {
				/* Only get here if thr is wrong */
				break;
			}
		}

		uk_thread_stop(thr);
		ret = uk_thread_get_thread_attr(thr, &thr_state, &thr_stopping, &thr_cpu, &thr_uid, 0);

		if (ret == 0) {
			if ((thr_state == 2) && thr_stopping) { /* 2 = thread state "current"
																  * i.e. irq thread is executing
																  */
				for (; stop_count < 1000000; ++stop_count ) {
					if (ret == 0) {
						if ((thr_stopping == 0) || (thr_state != 2)) {
							break;
						}
					}
					ret = uk_thread_get_thread_attr(thr, &thr_state, &thr_stopping, &thr_cpu, &thr_uid, 0);
				}
			}
		}

		/* irq thread is stopped so can read grab_lock without the risk of the irq thread starting
		 * to execute and changing the grab_lock
		 */
		if ((*grab_lock_ptr) == 0) {
			outer_loop_valid = 0;
			break;  /* Exit outer loop */
		}
		else {
			/* grab_lock is locked. Can only occur in "ready", "current" or "stopped" states since
			 * we can never be blocking inside the do_IRQ() routine.
			 * Resume the irq thread.
			 */
			uk_thread_resume(thr);
		}
	}

	/* just delete the thread */
	uk_thread_delete(thr);

	if (pre_grab_lock || (wait_count > 1)) {

		unsigned int this_thr_cpu;
		unsigned int del_thread_cpu;

		unsigned int this_thr;

#ifdef CONFIG_SMP
		del_thread_cpu = kt->tls.cpu;
#else
		del_thread_cpu = 0;
#endif
		this_thr_cpu = uk_thread_cpu_id();

		this_thr = uk_thread_nbr();

		pr_info("Clear grab_lock %d, this_cpu %d, this_thr %d, irq %d, thr %d, del_cpu %d, thr_state %d, stopping %d, pre_state %d, wait %d. outer %d\n",
					kt->tls.grab_lock, this_thr_cpu, this_thr, irq, thr, del_thread_cpu, thr_state, thr_stopping, thr_pre_stop_state, wait_count, outer_loop_count);
	}


	/* Clear the mutex locks */
	kt->tls.grab_lock = 0;
#ifdef CONFIG_SMP
	/* If the deleting thread has locked the mutex. Need to unlock it */
	arch_local_unlock_owned_mutex(kt->tls.cpu, kt->tls.uid);
#endif

	BUG_ON(kt->stack == NULL);
	free_page((unsigned long)kt->stack);
	kt->stack = NULL;

	kthreads_unlink_irq(thr, irq);
	kthreads_free_thread(thr);

	spin_unlock_irqrestore(&kthread_table_lock, flags);
}
EXPORT_SYMBOL(disable_irq_thread);

/*
 * Cancel an IRQ-thread blocking on p4_int_wait()
 *
 * NOTE: mustn't be called from IRQ context!
 */
void cancel_irq_thread(unsigned int irq)
{
	unsigned int thr;
	thr = __kthread_get_irq_thr(irq);
	if (thr != -1)
		uk_thread_cancel(thr);
}
EXPORT_SYMBOL(cancel_irq_thread);

/*
 * sleep in IRQ cb
 */
void sleep_in_irq_thread(void *event)
{
	uk_uevent_wait(event);
}
EXPORT_SYMBOL(sleep_in_irq_thread);

/*
 * wake up IRQ cb
 */
void wake_irq_thread(void *event)
{
	uk_uevent_signal(event);
}
EXPORT_SYMBOL(wake_irq_thread);

#ifdef CONFIG_SMP
/*
 * bind an IRQ's thread to a given CPU
 */
void bind_irq_thread(unsigned int irq, unsigned int cpu)
{
	unsigned long cpu_mask;
	unsigned long flags;
	unsigned int thr;

	spin_lock_irqsave(&kthread_table_lock, flags);

	thr = kthread_get_irq_thr(irq);

	kthread_table[thr].tls.cpu = cpu;
	kthread_table[thr].tls.new_cpu = cpu;
	cpu_mask = 1UL << kthread_native_cpu_id(cpu);
	uk_thread_setaffinity(thr, cpu_mask);

	spin_unlock_irqrestore(&kthread_table_lock, flags);
}

/*
 * lazily migrate an IRQ's thread to a given CPU
 */
void migrate_irq_thread(unsigned int irq, unsigned int cpu)
{
	unsigned long flags;
	unsigned int thr;

	spin_lock_irqsave(&kthread_table_lock, flags);

	thr = kthread_get_irq_thr(irq);
	kthread_table[thr].tls.new_cpu = cpu;

	spin_unlock_irqrestore(&kthread_table_lock, flags);
}
EXPORT_SYMBOL(migrate_irq_thread);

/*
 * do lazily migration of an IRQ thread now (while in the IRQ thread)
 */
void do_migrate_irq_thread(void)
{
	unsigned long cpu_mask;
	unsigned long flags;
	unsigned int thr;
	unsigned int cpu;

	spin_lock_irqsave(&kthread_table_lock, flags);

	thr = kthread_thread_id();
	cpu = __tls_get_uint(offsetof(struct kthread_tls, new_cpu));
	__tls_set_uint(offsetof(struct kthread_tls, cpu), cpu);

	cpu_mask = 1UL << kthread_native_cpu_id(cpu);
	uk_thread_setaffinity(thr, cpu_mask);

	spin_unlock_irqrestore(&kthread_table_lock, flags);
}
#endif

/*
 * Get a free ASYNC irq
 * Search the thread table for a free ASYNC IRQ (-2)
 * and mark it as "used" (-1)
 */
unsigned int get_free_async_irq(void)
{
	unsigned long flags;
	unsigned int irq;

	spin_lock_irqsave(&kthread_table_lock, flags);

	for (irq = FIRST_ASYNC_IRQ; irq < NR_IRQS; irq++) {
		if (irq2thr[irq] == -2) {
			irq2thr[irq] = -1;
			break;
		}
	}
	spin_unlock_irqrestore(&kthread_table_lock, flags);

	if (irq < NR_IRQS)
		return irq;
	return 0;	/* no free found */
}
EXPORT_SYMBOL(get_free_async_irq);

/*
 * Release ASYNC irq ID again
 */
void release_async_irq(unsigned int irq)
{
	unsigned long flags;

	BUG_ON(irq < FIRST_ASYNC_IRQ || irq >= NR_IRQS);

	spin_lock_irqsave(&kthread_table_lock, flags);

	BUG_ON(irq2thr[irq] != -1);
	irq2thr[irq] = -2;

	spin_unlock_irqrestore(&kthread_table_lock, flags);
}
EXPORT_SYMBOL(release_async_irq);

/*
 * Get thread ID of ASYNC irq thread
 */
unsigned int get_irq_threadid(unsigned int irq)
{
	return kthread_get_irq_thr(irq);
}
EXPORT_SYMBOL(get_irq_threadid);

/*
 * Get UID of ASYNC irq thread
 */
unsigned int get_irq_uid(unsigned int irq)
{
	return uk_uid_of_thread(kthread_get_irq_thr(irq));
}
EXPORT_SYMBOL(get_irq_uid);
