/*
 *  Userspace Exhandler and Pager wrapper
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

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sys.h>
#include <linux/tracehook.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,12,0)
#include <linux/hardirq.h>
#endif
#include <asm/signal.h>
#include <asm/host_context.h>
#include <asm/internals.h>
#include <asm/host_tasks.h>
#include <asm/host_exception.h>
#include <asm/host_threads.h>
#include <asm/kernthreads.h>
#include <asm/syscalls.h>
#include <asm/smp.h>
#include <asm/pikeos_version.h>



/*
 * Usermode vs Kernelmode State machine
 *
 * Keep the whole system in sync and
 * tells us whether we are 'in kernel' or 'in userspace'
 */
#define MAIN_STATE_KERNEL		0
#define MAIN_STATE_USER			1
#define MAIN_STATE_IDLE			2

static volatile int main_state[NR_CPUS] = { [0 ... NR_CPUS - 1] = MAIN_STATE_KERNEL };


int __is_user_mode(unsigned int cpu)
{
	return main_state[cpu] == MAIN_STATE_USER;
}

/*
 * wakeup_userspace()
 *
 * this may be called from interrupt-context to wakeup userspace or idle
 */
void test_and_wakeup_userspace(unsigned int cpu)
{
	/* check if a wakeup is needed */
	if (main_state[cpu] == MAIN_STATE_KERNEL)
		return;

	if (need_resched() || test_tsk_thread_flag(current, TIF_SIGPENDING)) {
		/* wakeup: userspace or idle task? */
		if (main_state[cpu] == MAIN_STATE_USER) {
			uk_sysemu_preempt(kthread_main_thread(cpu));
		} else { // (main_state[cpu] == MAIN_STATE_IDLE)
			uk_thread_ev_signal(kthread_main_thread(cpu));
		}
		main_state[cpu] = MAIN_STATE_KERNEL;
	}
}

/*
 * kern_idle()
 *
 * Special wait condition called by the idle thread and woken by
 * test_and_wakeup_userspace()
 */
void kern_idle(unsigned int cpu)
{
	/*
	 * we go to sleep and indicate by "2" that we are the idle task.
	 * we wait for interrupts to wake us
	 */
	main_state[cpu] = MAIN_STATE_IDLE;
	uk_thread_ev_wait();
	main_state[cpu] = MAIN_STATE_KERNEL;
}

static void dispatch(struct pt_regs *regs)
{
	int sig;

	/*
	 * Dispatch the exception message
	 */
	switch (pt_regs_get_ex_type(regs)) {
	case HOST_TRAP_SYS:
		/* Syscalls */
		arch_handle_syscall(regs);
		break;

	case HOST_TRAP_SEG:
		/* Pagefaults */
		user_segv(regs);
		break;

	case HOST_TRAP_NOT:
		/* caught running in userspace (forced exception) */
		/* -- do nothing, maybe send a signal */
		break;

	default:
		/* Architecture specific exceptions */
		sig = arch_handle_exception(regs);
#ifdef  CONFIG_SHOW_USER_EXCEPTIONS
#ifndef CONFIG_SHOW_USER_EXCEPTIONS_ALL
		if (sig)
#endif
		{
			printk("#### TRAP_%d pid:%d,%s, ip:0x%lx, vector:%lu, signal:%d\n",
			       pt_regs_get_ex_type(regs), task_pid_nr(current),
			       current->comm, PT_REGS_IP(regs),
			       regs->context[HOST_VECTOR], sig);
			//show_regs(regs);
		}
#endif /* CONFIG_SHOW_USER_EXCEPTIONS */
		if (sig)
			force_sig(sig, current);
		break;
	}

	/*
	 * We have all required registers to resume so we can safely call the
	 * scheduler, send signals or resume pending work
	 */
	if (need_resched())
		schedule();
	if (test_thread_flag(TIF_SIGPENDING))
		do_signal(regs);
	if (test_and_clear_thread_flag(TIF_NOTIFY_RESUME))
		tracehook_notify_resume(&current->thread.regs);
}

static void sysemu_handler(void)
{
	struct pt_regs *regs;
	unsigned int taskid;
	unsigned int cpu;

	regs = &current->thread.regs;
	cpu = hard_smp_processor_id();

	while (1) {
		/* we should arrive here with:
		   prio==MCP && main_state[cpu] = MAIN_STATE_USER
		   - in case we arrive from sysemu preempt
		   prio==MAIN && main_state[cpu] = MAIN_STATE_KERNEL
		   - in case we are called from userspace()
		*/
		main_state[cpu] = MAIN_STATE_KERNEL;
#if PIKEOS_API_VERSION >= 0x40004
		leave_critical_prio();
#endif

		BUG_ON(in_atomic());
		pt_regs_sysemu_convert(regs);
		dispatch(regs);
		BUG_ON(in_atomic());

#if PIKEOS_API_VERSION >= 0x40004
		enter_critical_prio();
#endif
		/* CPU ID may change when we're migrated in the kernel */
		cpu = hard_smp_processor_id();
		main_state[cpu] = MAIN_STATE_USER;

		/* SMP: may change when rescheduled and migrated */
		BUG_ON(current->mm == NULL);
		taskid = current->mm->context.taskid;
		BUG_ON(taskid == 0);

#if PIKEOS_API_VERSION < 0x40004
		uk_sysemu(taskid, regs, sysemu_handler, current->thread.kernel_stack);
#else
		__tls_set_ptr(offsetof(struct kthread_tls, sysemu_handler), sysemu_handler);
		__tls_set_ptr(offsetof(struct kthread_tls, sysemu_stack), (void *) current->thread.kernel_stack);
		__tls_set_ptr(offsetof(struct kthread_tls, sysemu_regs), &regs->context);

		uk_sysemu(taskid, regs, uk_task_mcp() + PRIO_MAIN);
#endif
		/* fall-thru due to preemption: fake HOST_TRAP_NOT */
		regs->context[HOST_EX_CODE] = 0;
	}
}

void userspace(struct pt_regs *regs)
{
	BUG_ON(in_atomic());

	/* first run: fake HOST_TRAP_NOT */
	regs->context[HOST_EX_CODE] = 0;

	sysemu_handler();
}
