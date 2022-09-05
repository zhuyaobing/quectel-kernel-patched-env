/*
 * IO-Port Emulation
 * Copyright (c) 2003-2009, Alex Zuepke, SYSGO AG <azu@sysgo.de>
 */

#include <linux/sched.h>
#include <linux/module.h>
#include <asm/signal.h>
#include <asm/uaccess.h>
#include <asm/sections.h>
#include <asm/host_context.h>
#include <asm/mmu_context.h>

/* from ioport.c */
extern int sys_iopl(int level);


/*
 * IO Port emulator for the kernel
 *
 * This is evil x86 magic, I suggest reading an x86 manual to understand it.
 */
static int emulate_ioport(struct pt_regs *regs, const unsigned char *opcode, unsigned long *bitmap, int fullaccess, int *pport)
{
	int ptr, found, width, port, string, rep;
	int direction;
	int diff;
	unsigned int mask;

	if (pport)
		*pport = -1;

	/* I/O port faults are GPFs */
	if (regs->context[HOST_VECTOR] != 13)
		return 0;

	/* And must be accessible. */
	if (opcode == NULL)
		return 0;

	ptr = 0;
	found = 0;	/* true if found, type: 0=not found, 1=in, 2=out */
	width = 2;	/* 32 bit wide op */
	port = 0;	/* keep GCC happy */
	string = 0;
	rep = 0;
	direction = 0;
	diff = 0;
	mask = 0;

	/* dispatch opcode with all valid prefixes and operands */
	/* maximum opcode lenght is 15 */
	while (!found && ptr < 15)
	{
		switch (opcode[ptr])
		{
			/* prefixes */
			case 0x66:		/* operand size */
				width = 1;
				break;
			case 0xf3:		/* REP */
				rep = 1;
				break;
			case 0x67:		/* address size (affects string operatios memory pointer) */
			case 0x26:		/* ES: */
			case 0x2e:		/* CS: */
			case 0x36:		/* SS: */
			case 0x3e:		/* DS: */
			case 0x64:		/* FS: */
			case 0x65:		/* GS: */
			case 0xf0:		/* LOCK */
				break;

			/* in / out opcodes */
			case 0x6c:		/* insb   (%dx),... */
			case 0x6d:		/* ins{w|l} (%dx),... */
			case 0x6e:		/* outsb   (%dx),... */
			case 0x6f:		/* outs{w|l} (%dx),... */
			case 0xe4:		/* in     port,%al */
			case 0xe5:		/* in     port,%{e}ax */
			case 0xe6:		/* out    %al,port */
			case 0xe7:		/* out    %{e}ax,port */
			case 0xec:		/* in     (%dx),%al */
			case 0xed:		/* in     (%dx),%{e}ax */
			case 0xee:		/* out    %al,(%dx) */
			case 0xef:		/* out    %{e}ax,(%dx) */
				/* analyze ... */
				if ((opcode[ptr] & (1 << 0)) == 0)	/* 8 bit access */
					width = 0;

				if ((opcode[ptr] & (1 << 1)) == 0)	/* in / out */
					found = 1;			/* in */
				else
					found = 2;			/* out */

				if ((opcode[ptr] & (1 << 7)) == 0)	/* string: ins, outs */
					string = 1;

				if ((opcode[ptr] & (1 << 3)) == 0)	/* port as immediate */
				{
					port = opcode[ptr+1];
					ptr++;
				}
				else
					port = regs->context[HOST_DX];

				break;

			/* other */
			default:
				return 0;
		}
		ptr++;
	}

	if (found == 0)
		return 0;

	port &= 0xffff;

	if (pport)
		*pport = port;

	/* check IO bitmap (if available) */
	if (!fullaccess)
	{
		unsigned int match = port & 31;	/* lower bits in word */

		/* only the first 1024 ports are covered by the IO bitmap */
		if (port+(1<<width) > 0x400)
			return 0;

		/* this assumes that port access is always aligned */
		if ((bitmap[port >> 5] & match) != match)
			return 0;
	}

	direction = (regs->context[HOST_FLAGS] >> 10 & 0x1) ? -1 : 1;	/* Direction Flag in EFLAGS */

	
	/*
	 * emulate instruction
	 */
	regs->context[HOST_IP] += ptr;

	if (string && rep)
	{
		/* rep ins or rep outs */
		diff = direction * width * regs->context[HOST_CX];
		regs->context[HOST_CX] = 0;
	}
	else if (string)
	{
		/* ins or outs */
		diff = direction * width;
	}
	else
	{
		/* in or out */
		if (width == 0)
			mask = 0xff;	/* inb ... */
		else if (width == 1)
			mask = 0xffff;	/* inw ... */
		else
			mask = 0xffffffff;	/* inl ... */
	}

	if (found == 2)
		regs->context[HOST_SI] += diff;
	else
	{
		regs->context[HOST_DI] += diff;
		regs->context[HOST_AX] |= mask;
	}

	return 1;	/* successfully emulated */
}

int arch_fixup(unsigned long address, struct pt_regs *regs)
{
	int port;

	if (emulate_ioport(regs, (unsigned char*)regs->context[HOST_IP], 0, 1, &port))
	{
		if (port != -1)
			printk(KERN_DEBUG "arch_fixup: access to IO port 0x%04x detected, emulating\n", port);

		return 1;
	}

	return(0);
}


/*
 * Check if a user mode exception was caused by an IO port instruction
 * returns 1 if an IO port instruction was found
 *
 * opcode refers to a copy of the up-to-15-bytes long opcode
 *
 * NOTE: this is evil x86 magic, I suggest reading an x86 manual to understand it.
 *
 */
static int check_ioport(const unsigned char *opcode)
{
	unsigned int ptr;

	/* dispatch opcode with all valid prefixes and operands */
	/* maximum opcode lenght is 15 */
	for (ptr = 0; ptr < 15; ptr++) {
		switch (opcode[ptr]) {
		/* prefixes are ignored */
		case 0x66:		/* operand size */
		case 0xf3:		/* REP */
		case 0x67:		/* address size (affects string operatios memory pointer) */
		case 0x26:		/* ES: */
		case 0x2e:		/* CS: */
		case 0x36:		/* SS: */
		case 0x3e:		/* DS: */
		case 0x64:		/* FS: */
		case 0x65:		/* GS: */
		case 0xf0:		/* LOCK */
			break;

		/* in / out opcodes */
		case 0x6c:		/* insb   (%dx),... */
		case 0x6d:		/* ins{w|l} (%dx),... */
		case 0x6e:		/* outsb   (%dx),... */
		case 0x6f:		/* outs{w|l} (%dx),... */
		case 0xe4:		/* in     port,%al */
		case 0xe5:		/* in     port,%{e}ax */
		case 0xe6:		/* out    %al,port */
		case 0xe7:		/* out    %{e}ax,port */
		case 0xec:		/* in     (%dx),%al */
		case 0xed:		/* in     (%dx),%{e}ax */
		case 0xee:		/* out    %al,(%dx) */
		case 0xef:		/* out    %{e}ax,(%dx) */
			return 1;

		/* other */
		default:
			return 0;
		}
	}

	return 0;
}

/*
 * fixupcode for exceptions from userspace
 *
 * return: 1 : handled
 *         0 : unhandled, force signal....
 */
int emulate_user(struct pt_regs *regs)
{
	unsigned char opcode[16];

	/* fetch opcode from userspace */
	if (copy_from_user(&opcode, (void*)regs->context[HOST_IP], sizeof(opcode)))
		return 0;

	if ((opcode[0] == 0xfa) ||	/* opcode: cli */
	    (opcode[0] == 0xfb))	/* opcode: sti */
	{
		if (current->thread.arch.iopl == 0)
			return 0;
		regs->context[HOST_IP]++;	/* increment %eip */
		return 1;	/* has called IOPL, we ignore cli and sti calls */
	}

	if (check_ioport(opcode)) {
		if (ioport_get_mm_state(current->mm) == 0) {
			/* IO port instruction, but IO ports not mapped:
			 * map IO ports and retry instruction
			 *
			 * sys_iopl() with the current IOPL setting does the trick for us,
			 * it also updates the ioport state in MM.
			 */
			sys_iopl(current->thread.arch.iopl);
			return 1;
		}
	}

	/* maybe the fault was caused by an IO-port operation */
	return emulate_ioport(regs, opcode, current->thread.arch.io_bitmap, current->thread.arch.iopl, 0);
}

/*
 * handle "FPU not available" exception: activate FPU
 *
 * depending on the CPU, we must provide an SSE (FXSR) context
 * or just initialize a normal i387 style FPU
 */
void activate_fpu(struct pt_regs *regs)
{
	int i;

	regs->context[HOST_USED_FPU] |= 1;

	for (i = HOST_X_FCW_FSW; i < HOST_FP_SIZE; i++)
		regs->context[i] = 0;

	if (elf_aux_hwcap & X86_FEATURE_XMM)
	{
		/* SSE machine */
		regs->context[HOST_X_FCW_FSW] = 0x0000037f;
		regs->context[HOST_MXCSR] = 0x00001f80;
	}
	else
	{
#ifdef CONFIG_64BIT
		panic("%s: cpu feature SSE not available\n", __func__);
#else
		/* normal i387 FPU */
		regs->context[HOST_FCW] = 0xffff037f;
		regs->context[HOST_FSW] = 0xffff0000;
		regs->context[HOST_FTW] = 0xffffffff;
		regs->context[HOST_FOS] = 0xffff0000;
#endif
	}
}
