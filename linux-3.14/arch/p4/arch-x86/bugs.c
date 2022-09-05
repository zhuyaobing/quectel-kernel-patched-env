/*
 * Copyright (C) 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include <linux/module.h>
#include <linux/uaccess.h>
#include <asm/signal.h>
#include <asm/current.h>
#include <asm/uaccess.h>
#include <asm/processor.h>
#include <asm/internals.h>
#include <asm/host_context.h>
#include <asm/alternative.h>

void __init check_bugs(void)
{
	alternative_instructions();
}

extern int emulate_user(struct pt_regs *regs);
extern void activate_fpu(struct pt_regs *regs);


/*
 * exception vector to signal mapping
 *
 * return: 0  : handled
 *         >0 : unhandled, force signal (return val)....
 */
int arch_handle_exception(struct pt_regs *regs)
{
	int sig;

	switch (regs->context[HOST_VECTOR]) {
	case 0:	/* div0 */
	case 9:	/* coproc segment overrun */
	case 16:	/* fpu error */
		sig = SIGFPE;
		break;

	case 1:	/* breakpoint - debug */
	case 3:	/* int3 - debug */
		sig = SIGTRAP;
		break;

	case 4:	/* overflow */
	case 5:	/* bounds */
	case 8:	/* double fault */
	case 10:	/* invalid tss */
	case 14:	/* pagefault */
		sig = SIGSEGV;
		break;

	case 13:	/* general protection fault */
		if (emulate_user(regs))	/* cli & sti + ioport emulation */
			return 0;
		sig = SIGSEGV;
		break;

	case 6:	/* invalid opcode */
		sig = SIGILL;
		break;

	case 11:	/* segment not present */
	case 12:	/* stack segment */
	case 17:	/* alignment */
		sig = SIGBUS;
		break;

	case 7:	/* device not available */
	case 19:	/* SIMD not available */
		/* enable FPU / SSE extension */
		activate_fpu(regs);
		return 0;

	default:
		sig = SIGBUS;
		break;
	}

	return sig;
}

int is_valid_bugaddr(unsigned long ip)
{
	unsigned short ud2;

	if (ip < PAGE_OFFSET)
		return 0;
	if (probe_kernel_address((unsigned short *)ip, ud2))
		return 0;

	return ud2 == 0x0b0f;
}
