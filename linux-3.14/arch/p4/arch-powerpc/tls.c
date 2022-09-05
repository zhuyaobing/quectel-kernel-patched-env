/*
 * tls.c - TLS support for P4/Linux
 */

#include <linux/sched.h>
#include <asm/ptrace.h>

int arch_copy_tls(struct task_struct *new, struct pt_regs *regs)
{
	new->thread.regs.context[2] = new->thread.regs.context[6];
	return 0;
}

void arch_clear_tls(struct task_struct *tsk)
{
	/* nothing to do ... */
}
