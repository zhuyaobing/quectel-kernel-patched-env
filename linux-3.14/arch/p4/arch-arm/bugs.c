/*
 * arch/p4/arch-arm/bugs.c - ARM exception handling
 *
 * 2009-09-22, David Engraf <den@sysgo.com>
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <asm/signal.h>
#include <asm/current.h>
#include <asm/uaccess.h>
#include <asm/processor.h>
#include <asm/internals.h>
#include <asm/host_context.h>
#include <asm/host_tasks.h>
#include <asm/vfp.h>

extern int (*alignment_handler)(unsigned long addr, unsigned int fsr,
				struct pt_regs *regs);

void __init check_bugs(void)
{
	/* NOTE: skipping CPU capability checks for now */
}

/*
 * exception vector to signal mapping
 *
 * return: 0  : handled
 *         >0 : unhandled, force signal (return val)....
 */
int arch_handle_exception(struct pt_regs *regs)
{
	int sig;
	unsigned int instr;
	
	switch (pt_regs_get_ex_type(regs)) {
	case HOST_TRAP_FP_UNAVAIL:
		if (uk_task_kinfo_has_fpu()) {
			/* enable FPU */
			regs->context[HOST_FPEXC] = FPEXC_EN;
			regs->context[HOST_FPSCR] = FPSCR_ROUND_NEAREST;
			sig = 0;
			break;
		}
		/* fall through */

	case HOST_TRAP_FP:
		/* FPU exception */
		sig = SIGFPE;
		break;

	case HOST_TRAP_ILL:
		get_user(instr, (u32 __user *)regs->context[HOST_EPC]);
		if ((instr & 0x0fffffff) == 0x07f001f0) {
			extern int break_trap(struct pt_regs *regs, unsigned int instr);
			break_trap(regs, instr);
			sig = 0;
		}
		else {
			/*printk(KERN_ERR "TRAP_ILL: pid: %d (%s), pc:0x%X, op:0x%X\n", 
				   task_pid_nr(current), current->comm, regs->context[HOST_EPC], instr);
			 */
			sig = SIGILL;
		}
		break;

	case HOST_TRAP_BRK:
		sig = SIGTRAP;
		break;

	default:
		if (pt_regs_is_alignment_fault(regs)) {
			sig = SIGBUS;
#ifdef CONFIG_ALIGNMENT_TRAP
			/* handle alignment exception */
			if (alignment_handler &&
				alignment_handler(pt_regs_get_fault_addr(regs),
						  1, regs) == 0) {
				sig = 0;
			}
#endif
		} else {
			printk("FIXME: %s unknown exception\n", __FUNCTION__);
			sig = SIGBUS;	/* FIXME: default behaviour */
		}
		break;
	}

	return sig;
}

#ifdef CONFIG_GENERIC_BUG

int is_valid_bugaddr(unsigned long pc)
{
#ifdef CONFIG_THUMB2_KERNEL
	unsigned short bkpt;
#else
	unsigned long bkpt;
#endif

	if (probe_kernel_address((unsigned *)pc, bkpt))
		return 0;

	return bkpt == BUG_INSTR_VALUE;
}

#endif
