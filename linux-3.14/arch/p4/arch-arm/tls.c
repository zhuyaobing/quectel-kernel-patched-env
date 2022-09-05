/*
 * linux/arch/p4/arch-arm/tls.c - ARM TLS support
 *
 * 2009-09-22, David Engraf <den@sysgo.com>
 */

#include <linux/sched.h>
#include <asm/ptrace.h>

int arch_copy_tls(struct task_struct *new, struct pt_regs *regs)
{
	new->thread.regs.context[HOST_TLS] = regs->context[HOST_R3];
	return 0;
}

void arch_clear_tls(struct task_struct *tsk)
{
	tsk->thread.regs.context[HOST_TLS] = 0;
}
