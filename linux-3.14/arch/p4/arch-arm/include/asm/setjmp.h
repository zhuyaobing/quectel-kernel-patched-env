/*
 *  simple setjmp/longjmp for arm
 * 
 *  Copyright (C) 2006 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#ifndef __SETJMP_ARM_H_
#define __SETJMP_ARM_H_

#include <linux/compiler.h>

//syncronize with arch/p4/arch-arm/setjmp.S
#define ARCH_JMPBUF_FP   7	/* R11 */
#define ARCH_JMPBUF_SP   8
#define ARCH_JMPBUF_LR   9
#define ARCH_JMPBUF_SIZE 10

/* generic names */
#define ARCH_JMPBUF_IP ARCH_JMPBUF_LR

typedef struct __arch_jmp_buf_tag
{
	unsigned long regs[ARCH_JMPBUF_SIZE];
} arch_jmp_buf[1];

extern int arch_setjmp(arch_jmp_buf env);
extern void arch_longjmp(arch_jmp_buf env, int val) __noreturn;
extern void arch_dojmp(void *pc, unsigned long sp, int val) __noreturn;

static inline void arch_set_jmp_buf(arch_jmp_buf env, void *pc, unsigned long sp)
{
	/* set instruction pointer and stack in jump buf */
	env[0].regs[ARCH_JMPBUF_SP] = sp;
	env[0].regs[ARCH_JMPBUF_LR] = (unsigned long) pc;
}

#endif /* __SETJMP_ARM_H_ */
