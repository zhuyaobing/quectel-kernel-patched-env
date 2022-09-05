/*
 * Copyright (C) 2000 - 2007 Jeff Dike (jdike@{addtoit,linux.intel}.com)
 * Licensed under the GPL
 *
 * 2015-01-23, david.engraf@sysgo.com: adapted from UM
 *
 * NOTE: Keep in sync with arch/um/kernel/signal.c
 */

#include <linux/module.h>
#include <linux/ptrace.h>
#include <linux/sched.h>
#include <asm/siginfo.h>
#include <asm/signal.h>
#include <asm/unistd.h>

extern int setup_signal_stack_sc(unsigned long stack_top, int sig,
				 struct k_sigaction *ka,
				 struct pt_regs *regs,
				 sigset_t *mask);
extern int setup_signal_stack_si(unsigned long stack_top, int sig,
				 struct k_sigaction *ka,
				 struct pt_regs *regs, struct siginfo *info,
				 sigset_t *mask);

/*
 * OK, we're invoking a handler
 */
static void handle_signal(struct pt_regs *regs, unsigned long signr,
			 struct k_sigaction *ka, struct siginfo *info)
{
	sigset_t *oldset = sigmask_to_save();
	int singlestep = 0;
	unsigned long sp;
	int err;

	if ((current->ptrace & PT_DTRACE) && (current->ptrace & PT_PTRACED))
		singlestep = 1;

	/* Did we come from a system call? */
	if (PT_REGS_SYSCALL_NR(regs) >= 0) {
		/* If so, check system call restarting.. */
		switch (PT_REGS_SYSCALL_RET(regs)) {
		case -ERESTART_RESTARTBLOCK:
		case -ERESTARTNOHAND:
			PT_REGS_SYSCALL_RET(regs) = -EINTR;
			break;

		case -ERESTARTSYS:
			if (!(ka->sa.sa_flags & SA_RESTART)) {
				PT_REGS_SYSCALL_RET(regs) = -EINTR;
				break;
			}
		/* fallthrough */
		case -ERESTARTNOINTR:
			PT_REGS_RESTART_SYSCALL(regs);
			PT_REGS_ORIG_SYSCALL(regs) = PT_REGS_SYSCALL_NR(regs);
			break;
		}
	}

	sp = PT_REGS_SP(regs);
	if ((ka->sa.sa_flags & SA_ONSTACK) && (sas_ss_flags(sp) == 0))
		sp = current->sas_ss_sp + current->sas_ss_size;

#ifdef CONFIG_ARCH_HAS_SC_SIGNALS
	if (!(ka->sa.sa_flags & SA_SIGINFO))
		err = setup_signal_stack_sc(sp, signr, ka, regs, oldset);
	else
#endif
		err = setup_signal_stack_si(sp, signr, ka, regs, info, oldset);

	if (err)
		force_sigsegv(signr, current);
	else
		signal_delivered(signr, info, ka, regs, singlestep);
}

static int kern_do_signal(struct pt_regs *regs)
{
	struct k_sigaction ka_copy;
	struct siginfo info;
	int sig, handled_sig = 0;

	while ((sig = get_signal_to_deliver(&info, &ka_copy, regs, NULL)) > 0) {
		handled_sig = 1;
		/* Whee!  Actually deliver the signal.  */
		handle_signal(regs, sig, &ka_copy, &info);
	}

	/* Did we come from a system call? */
	if (!handled_sig && (PT_REGS_SYSCALL_NR(regs) >= 0)) {
		/* Restart the system call - no handlers present */
		switch (PT_REGS_SYSCALL_RET(regs)) {
		case -ERESTARTNOHAND:
		case -ERESTARTSYS:
		case -ERESTARTNOINTR:
			PT_REGS_ORIG_SYSCALL(regs) = PT_REGS_SYSCALL_NR(regs);
			PT_REGS_RESTART_SYSCALL(regs);
			break;
		case -ERESTART_RESTARTBLOCK:
			PT_REGS_ORIG_SYSCALL(regs) = __NR_restart_syscall;
			PT_REGS_RESTART_SYSCALL(regs);
			break;
		}
	}

	/*
	 * if there's no signal to deliver, we just put the saved sigmask
	 * back
	 */
	if (!handled_sig)
		restore_saved_sigmask();
	return handled_sig;
}

int do_signal(void)
{
	return kern_do_signal(&current->thread.regs);
}
