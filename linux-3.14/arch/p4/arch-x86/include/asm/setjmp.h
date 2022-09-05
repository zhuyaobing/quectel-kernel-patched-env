/*
 *  simple setjmp/longjmp for x86
 * 
 *  Copyright (C) 2003-2014 SYSGO AG
 * 
 *  2014-11-24 David Engraf - Add amd64 support
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#ifndef __SETJMP_X86_H_
#define __SETJMP_X86_H_

#ifdef CONFIG_X86_32

#include <linux/compiler.h>

#define ARCH_JMPBUF_EBX 0
#define ARCH_JMPBUF_ESI 1
#define ARCH_JMPBUF_EDI 2
#define ARCH_JMPBUF_EBP 3
#define ARCH_JMPBUF_ESP 4
#define ARCH_JMPBUF_EIP 5
#define ARCH_JMPBUF_SIZE 6

/* generic names */
#define ARCH_JMPBUF_SP ARCH_JMPBUF_ESP
#define ARCH_JMPBUF_IP ARCH_JMPBUF_EIP

#else /* CONFIG_X86_32 */

#define ARCH_JMPBUF_RBX 0
#define ARCH_JMPBUF_RSP 1
#define ARCH_JMPBUF_RBP 2
#define ARCH_JMPBUF_R12 3
#define ARCH_JMPBUF_R13 4
#define ARCH_JMPBUF_R14 5
#define ARCH_JMPBUF_R15 6
#define ARCH_JMPBUF_RIP 7
#define ARCH_JMPBUF_SIZE 8

/* generic names */
#define ARCH_JMPBUF_SP ARCH_JMPBUF_RSP
#define ARCH_JMPBUF_IP ARCH_JMPBUF_RIP

#define arch_setjmp  setjmp
#define arch_longjmp longjmp

#endif /* CONFIG_X86_32 */

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
	env[0].regs[ARCH_JMPBUF_IP] = (unsigned long) pc;
}

#endif /* __SETJMP_X86_H_ */
