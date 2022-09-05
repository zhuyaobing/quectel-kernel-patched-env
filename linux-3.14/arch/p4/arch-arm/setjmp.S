/*
 *  simple setjmp/longjmp for ARM see also p4/kernel/arch/arm/acontext.S
 * 
 *  Author: David Engraf <den@sysgo.com>
 * 
 *  Copyright (C) 2009 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

   .text

/*
int arch_setjmp(jmp_buf env);
*/
   .globl arch_setjmp
arch_setjmp:
   stmia r0,{r4-r11,sp,lr}
    mov r0, #0
    mov pc, lr

/*
void arch_longjmp(jmp_buf env, int val)
*/
    .globl arch_longjmp
arch_longjmp:
    mov     r1, r0
    mov     r0, #1
    ldmia   r1, {r4-r11, sp, pc}

/*
void arch_dojmp(void *pc, unsigned long sp, int val)

saves context on old stack and calls int (*pc)(int val) on new stack sp
*/
/* TODO: verify */
	.globl arch_dojmp
arch_dojmp:
	mov		lr, r0
	mov		sp, r1
	mov		r0, r2
	mov		pc, lr
