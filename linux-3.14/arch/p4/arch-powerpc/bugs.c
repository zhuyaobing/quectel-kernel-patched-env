/*
 * Copyright (C) 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include <linux/module.h>
#include <linux/uaccess.h>
#include <asm/signal.h>
#include <asm/current.h>
#include <asm/internals.h>
#include <asm/host_context.h>
#include <asm/host_tasks.h>

/* alignment trap */
#ifdef CONFIG_P4_PPC_OEA
#define ARCH_ALIGNMENT_TRAP 0x600
#else
#define ARCH_ALIGNMENT_TRAP 0x500
#endif

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
	int err;

	/* special handling for alignment exceptions */
	if (regs->trap == ARCH_ALIGNMENT_TRAP) {
		err = fix_alignment(regs);
		if (err == 1) {
			/* skip over emulated instruction */
			regs->context[HOST_IP] += 4;
			return 0;
		}
		/* fall through: take normal path */
	}

	//printk("**** exception 0x%x, ip:0x%08x\n", regs->trap, regs->context[HOST_IP]);

	switch (pt_regs_get_ex_type(regs)) {
	case HOST_TRAP_FP_UNAVAIL:
		regs->context[HOST_MSR] |= (1<<13); /* enable FPU */
#ifdef CONFIG_P4_PPC_SPE
		regs->context[HOST_MSR] |= (1<<25); /* also enable SPE APU */
#endif
		err = 0;
		break;

	case HOST_TRAP_ILL:
	{
		u32 instword;
		u32 rd;

		if (get_user(instword, (u32 __user *)(PT_REGS_NIP(regs)))) {
			err = SIGILL;
			break;
		}

		/* Emulate the mfspr rD, PVR. */
		if ((instword & PPC_INST_MFSPR_PVR_MASK) == PPC_INST_MFSPR_PVR) {
			rd = (instword >> 21) & 0x1f;
			regs->context[rd] = uk_task_kinfo_pvr();
			PT_REGS_NIP(regs) += 4;
			err = 0;
			break;
		}
		/* fall through: take normal path */
	}

	case HOST_TRAP_ARI:
		err = SIGILL;
		break;

	case HOST_TRAP_FP:
		err = SIGFPE;
		break;

	case HOST_TRAP_BRK:
	case HOST_TRAP_TRP:
		err = SIGTRAP;
		break;

	default:
		err = SIGBUS;	/* FIXME: default behaviour */
		break;
	}

	return err;
}

int is_valid_bugaddr(unsigned long addr)
{
	/* check must be done before calling report_bug (see kmapper.c) */
	return 1;
}
