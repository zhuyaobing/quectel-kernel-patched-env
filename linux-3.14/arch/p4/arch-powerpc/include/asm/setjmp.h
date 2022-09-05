/*
 *  simple setjmp/longjmp for PowerPC
 * 
 *  Copyright (C) 2004 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#ifndef __SETJMP_PPC_H_
#define __SETJMP_PPC_H_

#include <linux/compiler.h>

#define ARCH_JMPBUF_R1 0
#define ARCH_JMPBUF_CR 1
#define ARCH_JMPBUF_R13 2
#define ARCH_JMPBUF_R14 3
#define ARCH_JMPBUF_R15 4
#define ARCH_JMPBUF_R16 5
#define ARCH_JMPBUF_R17 6
#define ARCH_JMPBUF_R18 7
#define ARCH_JMPBUF_R19 8
#define ARCH_JMPBUF_R20 9
#define ARCH_JMPBUF_R21 10
#define ARCH_JMPBUF_R22 11
#define ARCH_JMPBUF_R23 12
#define ARCH_JMPBUF_R24 13
#define ARCH_JMPBUF_R25 14
#define ARCH_JMPBUF_R26 15
#define ARCH_JMPBUF_R27 16
#define ARCH_JMPBUF_R28 17
#define ARCH_JMPBUF_R29 18
#define ARCH_JMPBUF_R30 19
#define ARCH_JMPBUF_R31 20
#define ARCH_JMPBUF_LR 21
#define ARCH_JMPBUF_SIZE 22

/* generic names */
#define ARCH_JMPBUF_SP ARCH_JMPBUF_R1
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
	env[0].regs[ARCH_JMPBUF_R1] = sp;
	env[0].regs[ARCH_JMPBUF_LR] = (unsigned long) pc;
	env[0].regs[ARCH_JMPBUF_CR] = 0;
}

#endif /* __SETJMP_PPC_H_ */
