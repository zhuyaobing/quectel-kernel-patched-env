/*
 * Copyright (C) 2000 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 *
 * adapted for P4/Linux by Alex Zuepke <azu at sysgo dot de>
 */

#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/errno.h>
#include <linux/ptrace.h>
#include <asm/ptrace.h>
#include <asm/uaccess.h>
#include <asm/internals.h>

#ifdef CONFIG_X86_64
long do_arch_prctl(struct task_struct *task, int code, unsigned long addr);
#endif

long arch_ptrace(struct task_struct *child, long request,
				 unsigned long addr, unsigned long data)
{
	int ret;

	switch (request) {
	/* read the word at location addr in the USER area. */
	case PTRACE_PEEKUSR: {
#ifdef CONFIG_P4_ARM
		extern int ptrace_read_user(struct task_struct *tsk, unsigned long off,
			    unsigned long __user *ret);

		ret = ptrace_read_user(child, addr, (unsigned long __user *)data);
#else
		unsigned long tmp;

		tmp = 0;  /* Default return condition */
		ret = getreg(child, addr, &tmp);
		if(ret == 0)
			ret = put_user(tmp, (unsigned long *) data);
#endif
		break;
	}

	case PTRACE_POKEUSR: {
		/* write the word at location addr in the USER area */
#ifdef CONFIG_P4_ARM
		extern int ptrace_write_user(struct task_struct *tsk, unsigned long off,
			     unsigned long val);

		ret = ptrace_write_user(child, addr, data);
#else
		ret = putreg(child, addr, data);
#endif
		break;
	}

#ifdef PTRACE_GETREGS
	case PTRACE_GETREGS: /* Get all gp regs from the child. */
		ret = get_gpregs(data, child);
		break;
#endif
#ifdef PTRACE_SETREGS
	case PTRACE_SETREGS: /* Set all gp regs in the child. */
		ret = set_gpregs(data, child);
		break;
#endif
#ifdef PTRACE_GETFPREGS
	case PTRACE_GETFPREGS: /* Get the child FPU state. */
		ret = get_fpregs(data, child);
		break;
#endif
#ifdef PTRACE_SETFPREGS
	case PTRACE_SETFPREGS: /* Set the child FPU state. */
	        ret = set_fpregs(data, child);
		break;
#endif
#ifdef PTRACE_GETFPXREGS
	case PTRACE_GETFPXREGS: /* Get the child FPU state. */
		ret = get_fpxregs(data, child);
		break;
#endif
#ifdef PTRACE_SETFPXREGS
	case PTRACE_SETFPXREGS: /* Set the child FPU state. */
		ret = set_fpxregs(data, child);
		break;
#endif
#ifdef CONFIG_VFP
#if defined PTRACE_GETVFPREGS
	case  PTRACE_GETVFPREGS: /* Get the child VFP state. */
		ret = get_vfpregs(data, child);
		break;
#endif
#if defined PTRACE_SETVFPREGS
	case  PTRACE_SETVFPREGS: /* Set the child VFP state. */
		ret = set_vfpregs(data, child);
		break;
#endif
#endif
#ifdef PTRACE_GET_THREAD_AREA
	case PTRACE_GET_THREAD_AREA:
		ret = ptrace_get_thread_area(child, addr, (void*)data);
		break;
#endif
#ifdef PTRACE_SET_THREAD_AREA
	case PTRACE_SET_THREAD_AREA:
		ret = ptrace_set_thread_area(child, addr, (void*)data);
		break;
#endif
#ifdef CONFIG_X86_64
		/* normal 64bit interface to access TLS data.
		   Works just like arch_prctl, except that the arguments
		   are reversed. */
	case PTRACE_ARCH_PRCTL:
		ret = do_arch_prctl(child, data, addr);
		break;
#endif
	default:
		ret = ptrace_request(child, request, addr, data);
		break;
	}

	return ret;
}

void send_sigtrap(struct task_struct *tsk, struct pt_regs *regs,
		  int error_code)
{
	struct siginfo info;

	memset(&info, 0, sizeof(info));
	info.si_signo = SIGTRAP;
	info.si_code = TRAP_BRKPT;

	/* User-mode eip? */
	info.si_addr = (void __user *) PT_REGS_IP(regs);

	/* Send us the fakey SIGTRAP */
	force_sig_info(SIGTRAP, &info, tsk);
}

/* XXX Check PT_DTRACE vs TIF_SINGLESTEP for singlestepping check */
void syscall_trace(struct pt_regs *regs, int entryexit)
{
	int is_singlestep = (current->ptrace & PT_DTRACE) && entryexit;
#ifdef CONFIG_P4_ARM
	unsigned long ip;
#endif

#if 0
	if (unlikely(current->audit_context)) {
		if (!entryexit)
			audit_syscall_entry(current,
                                            HOST_AUDIT_ARCH,
					    UPT_SYSCALL_NR(regs),
					    UPT_SYSCALL_ARG1(regs),
					    UPT_SYSCALL_ARG2(regs),
					    UPT_SYSCALL_ARG3(regs),
					    UPT_SYSCALL_ARG4(regs));
		else audit_syscall_exit(current,
                                        AUDITSC_RESULT(UPT_SYSCALL_RET(regs)),
                                        UPT_SYSCALL_RET(regs));
	}
#endif

	/* Fake a debug trap */
	if (is_singlestep)
		send_sigtrap(current, regs, 0);

	if (!test_thread_flag(TIF_SYSCALL_TRACE))
		return;

#ifdef CONFIG_P4_ARM
	/*
	 * Save IP.  IP is used to denote syscall entry/exit:
	 *  IP = 0 -> entry, = 1 -> exit
	 */
	ip = regs->context[HOST_IP];
	regs->context[HOST_IP] = entryexit;
#endif

	/* the 0x80 provides a way for the tracing parent to distinguish
	   between a syscall stop and SIGTRAP delivery */
	ptrace_notify(SIGTRAP | ((current->ptrace & PT_TRACESYSGOOD)
							? 0x80 : 0));

	if (entryexit) /* force do_signal() --> is_syscall() */
		set_thread_flag(TIF_SIGPENDING);

	/* this isn't the same as continuing with a signal, but it will do
	 * for normal use.  strace only continues with a signal if the
	 * stopping signal is not SIGTRAP.  -brl
	 */
	if (current->exit_code) {
		send_sig(current->exit_code, current, 1);
		current->exit_code = 0;
	}

#ifdef CONFIG_P4_ARM
	regs->context[HOST_IP] = ip;
#endif
}
