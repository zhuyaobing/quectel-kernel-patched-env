/*
 * linux/arch/p4/arch-ppc/ptrace.c - PowerPC 32-bit ptrace implementation,
 * taken from linux/arch/powerpc/kernel/ptrace.c
 */
/*
 *  PowerPC version
 *    Copyright (C) 1995-1996 Gary Thomas (gdt@linuxppc.org)
 *
 *  Derived from "arch/m68k/kernel/ptrace.c"
 *  Copyright (C) 1994 by Hamish Macdonald
 *  Taken from linux/kernel/ptrace.c and modified for M680x0.
 *  linux/kernel/ptrace.c is by Ross Biro 1/23/92, edited by Linus Torvalds
 *
 * Modified by Cort Dougan (cort@hq.fsmlabs.com)
 * and Paul Mackerras (paulus@samba.org).
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file README.legal in the main directory of
 * this archive for more details.
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/errno.h>
#include <linux/ptrace.h>
#include <linux/user.h>
#include <linux/security.h>
#include <linux/signal.h>
#include <linux/seccomp.h>
#include <linux/audit.h>
#include <linux/module.h>

#include <asm/uaccess.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/reg.h>

/*
 * does not yet catch signals sent when the child dies.
 * in exit.c or in signal.c.
 */

#ifdef CONFIG_PPC32
/*
 * Get contents of register REGNO in task TASK.
 */
static inline unsigned long get_reg(struct task_struct *task, int regno)
{
	if (regno < PT_NIP) /* r0 .. r31 */
		return task->thread.regs.context[regno];

	switch (regno) {
	case PT_NIP:
		return task->thread.regs.context[HOST_IP];
	case PT_MSR:
		return task->thread.regs.context[HOST_MSR];
	case PT_ORIG_R3:
		return task->thread.regs.orig_r3;
	case PT_CTR:
		return task->thread.regs.context[HOST_CTR];
	case PT_LNK:
		return task->thread.regs.context[HOST_LR];
	case PT_XER:
		return task->thread.regs.context[HOST_XER];
	case PT_CCR:
		return task->thread.regs.context[HOST_CR];
	case 40: /* PT_TRAP */
		return task->thread.regs.trap;
	case 41: /* PT_DAR */
		return task->thread.regs.context[HOST_DAR];
	case 42: /* PT_DSISR */
		return task->thread.regs.context[HOST_DSISR];
	case 43: /* PT_RESULT */
		return task->thread.regs.result;
	}

	/* default */
	return 0;
}


/*
 * Write contents of register REGNO in task TASK.
 */
static inline int put_reg(struct task_struct *task, int regno,
			  unsigned long data)
{
	if (regno > PT_MQ)
		return -EIO;

	if (regno < PT_NIP) /* r0 .. r31 */
		task->thread.regs.context[regno] = data;

	switch (regno) {
	case PT_NIP:
		task->thread.regs.context[HOST_IP] = data;
		break;
	case PT_MSR:
		task->thread.regs.context[HOST_MSR] = data;
		break;
	case PT_CTR:
		task->thread.regs.context[HOST_CTR] = data;
		break;
	case PT_LNK:
		task->thread.regs.context[HOST_LR] = data;
		break;
	case PT_XER:
		task->thread.regs.context[HOST_XER] = data;
		break;
	case PT_CCR:
		task->thread.regs.context[HOST_CR] = data;
		break;
	case PT_MQ:
		/* do nothing */
		break;
	}
	return 0;
}

#ifdef CONFIG_ALTIVEC
/* FIXME: not done yet */
/*
 * Get contents of AltiVec register state in task TASK
 */
static inline int get_vrregs(unsigned long __user *data, struct task_struct *task)
{
	int i, j;

	if (!access_ok(VERIFY_WRITE, data, 133 * sizeof(unsigned long)))
		return -EFAULT;

	/* copy AltiVec registers VR[0] .. VR[31] */
	for (i = 0; i < 32; i++)
		for (j = 0; j < 4; j++, data++)
			if (__put_user(task->thread.vr[i].u[j], data))
				return -EFAULT;

	/* copy VSCR */
	for (i = 0; i < 4; i++, data++)
		if (__put_user(task->thread.vscr.u[i], data))
			return -EFAULT;

        /* copy VRSAVE */
	if (__put_user(task->thread.vrsave, data))
		return -EFAULT;

	return 0;
}

/*
 * Write contents of AltiVec register state into task TASK.
 */
/* FIXME: not done yet */
static inline int set_vrregs(struct task_struct *task, unsigned long __user *data)
{
	int i, j;

	if (!access_ok(VERIFY_READ, data, 133 * sizeof(unsigned long)))
		return -EFAULT;

	/* copy AltiVec registers VR[0] .. VR[31] */
	for (i = 0; i < 32; i++)
		for (j = 0; j < 4; j++, data++)
			if (__get_user(task->thread.vr[i].u[j], data))
				return -EFAULT;

	/* copy VSCR */
	for (i = 0; i < 4; i++, data++)
		if (__get_user(task->thread.vscr.u[i], data))
			return -EFAULT;

	/* copy VRSAVE */
	if (__get_user(task->thread.vrsave, data))
		return -EFAULT;

	return 0;
}
#endif

#ifdef CONFIG_SPE

/*
 * For get_evrregs/set_evrregs functions 'data' has the following layout:
 *
 * struct {
 *   u32 evr[32];
 *   u64 acc;
 *   u32 spefscr;
 * }
 */

/*
 * Get contents of SPE register state in task TASK.
 */
/* FIXME: not done yet */
static inline int get_evrregs(unsigned long *data, struct task_struct *task)
{
	int i;

	if (!access_ok(VERIFY_WRITE, data, 35 * sizeof(unsigned long)))
		return -EFAULT;

	/* copy SPEFSCR */
	if (__put_user(task->thread.spefscr, &data[34]))
		return -EFAULT;

	/* copy SPE registers EVR[0] .. EVR[31] */
	for (i = 0; i < 32; i++, data++)
		if (__put_user(task->thread.evr[i], data))
			return -EFAULT;

	/* copy ACC */
	if (__put_user64(task->thread.acc, (unsigned long long *)data))
		return -EFAULT;

	return 0;
}

/*
 * Write contents of SPE register state into task TASK.
 */
/* FIXME: not done yet */
static inline int set_evrregs(struct task_struct *task, unsigned long *data)
{
	int i;

	if (!access_ok(VERIFY_READ, data, 35 * sizeof(unsigned long)))
		return -EFAULT;

	/* copy SPEFSCR */
	if (__get_user(task->thread.spefscr, &data[34]))
		return -EFAULT;

	/* copy SPE registers EVR[0] .. EVR[31] */
	for (i = 0; i < 32; i++, data++)
		if (__get_user(task->thread.evr[i], data))
			return -EFAULT;
	/* copy ACC */
	if (__get_user64(task->thread.acc, (unsigned long long*)data))
		return -EFAULT;

	return 0;
}
#endif /* CONFIG_SPE */

static inline void
set_single_step(struct task_struct *task)
{
	struct pt_regs *regs = &task->thread.regs;

#if defined(CONFIG_4xx) || defined(CONFIG_BOOKE)
		//task->thread.dbcr0 = DBCR0_IDM | DBCR0_IC;
		regs->context[HOST_MSR] |= MSR_DE;
#else
		regs->context[HOST_MSR] |= MSR_SE;
#endif
}

static inline void
clear_single_step(struct task_struct *task)
{
	struct pt_regs *regs = &task->thread.regs;

#if defined(CONFIG_4xx) || defined(CONFIG_BOOKE)
		//task->thread.dbcr0 = 0;
		regs->context[HOST_MSR] &= ~MSR_DE;
#else
		regs->context[HOST_MSR] &= ~MSR_SE;
#endif
}
#endif /* CONFIG_PPC32 */

/*
 * Called by kernel/ptrace.c when detaching..
 *
 * Make sure single step bits etc are not set.
 */
void ptrace_disable(struct task_struct *child)
{
	/* make sure the single step bit is not set. */
	clear_single_step(child);
}

long arch_ptrace(struct task_struct *child, long request,
				 unsigned long addr, unsigned long data)
{
	int ret = -EPERM;

	switch (request) {
	/* read the word at location addr in the USER area. */
	case PTRACE_PEEKUSR: {
		unsigned long index, tmp;

		ret = -EIO;
		/* convert to index and check */
		index = (unsigned long) addr >> 2;
		if ((addr & 3) || (index > PT_FPSCR))
			break;

		if (index < PT_FPR0) {
			tmp = get_reg(child, (int) index);
		} else {
			tmp = child->thread.regs.context[HOST_FR0 + index - PT_FPR0];
		}
		ret = put_user(tmp,(unsigned long __user *) data);
		break;
	}

	/* write the word at location addr in the USER area */
	case PTRACE_POKEUSR: {
		unsigned long index;

		ret = -EIO;
		/* convert to index and check */
		index = (unsigned long) addr >> 2;
		if ((addr & 3) || (index > PT_FPSCR))
			break;

		if (index == PT_ORIG_R3)
			break;
		if (index < PT_FPR0) {
			ret = put_reg(child, index, data);
		} else {
			child->thread.regs.context[HOST_FR0 + index - PT_FPR0] = data;
			ret = 0;
		}
		break;
	}
#ifdef CONFIG_ALTIVEC
	case PTRACE_GETVRREGS:
		/* Get the child altivec register state. */
		flush_altivec_to_thread(child);
		ret = get_vrregs((unsigned long __user *)data, child);
		break;

	case PTRACE_SETVRREGS:
		/* Set the child altivec register state. */
		flush_altivec_to_thread(child);
		ret = set_vrregs(child, (unsigned long __user *)data);
		break;
#endif
#ifdef CONFIG_SPE
	case PTRACE_GETEVRREGS:
		/* Get the child spe register state. */
		if (child->thread.regs->msr & MSR_SPE)
			giveup_spe(child);
		ret = get_evrregs((unsigned long __user *)data, child);
		break;

	case PTRACE_SETEVRREGS:
		/* Set the child spe register state. */
		/* this is to clear the MSR_SPE bit to force a reload
		 * of register state from memory */
		if (child->thread.regs->msr & MSR_SPE)
			giveup_spe(child);
		ret = set_evrregs(child, (unsigned long __user *)data);
		break;
#endif

	default:
		ret = ptrace_request(child, request, addr, data);
		break;
	}

	return ret;
}

void syscall_trace(struct pt_regs *regs, int unused)
{
	/* the 0x80 provides a way for the tracing parent to distinguish
	   between a syscall stop and SIGTRAP delivery */
	ptrace_notify(SIGTRAP | ((current->ptrace & PT_TRACESYSGOOD)
				 ? 0x80 : 0));

	/*
	 * this isn't the same as continuing with a signal, but it will do
	 * for normal use.  strace only continues with a signal if the
	 * stopping signal is not SIGTRAP.  -brl
	 */
	if (current->exit_code) {
		send_sig(current->exit_code, current, 1);
		current->exit_code = 0;
	}
}

/*
 * doesn't belong to ptrace, but fits quite well here
 * used to save fpregs in ELF core dumps
 */
int dump_fpu(struct pt_regs *regs, elf_fpregset_t *fpu )
{
	return 1;
}

void user_enable_single_step(struct task_struct *task)
{
	set_single_step(task);
	set_tsk_thread_flag(task, TIF_SINGLESTEP);
}

void user_disable_single_step(struct task_struct *task)
{
	clear_single_step(task);
	clear_tsk_thread_flag(task, TIF_SINGLESTEP);
}
