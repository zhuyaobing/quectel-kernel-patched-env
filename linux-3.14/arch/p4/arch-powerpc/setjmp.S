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

#include <arch/asm/ppc_asm.h>

.text
.globl arch_setjmp

/*
int arch_setjmp(jmp_buf env);
*/

arch_setjmp:
	stw	r1,   0(r3)	/* jmp_buf in r3 */
	mfcr	r4		/* save cr */
	stw	r4,   4(r3)
	stw	r13,  8(r3)
	stw	r14, 12(r3)
	stw	r15, 16(r3)
	stw	r16, 20(r3)
	stw	r17, 24(r3)
	stw	r18, 28(r3)
	stw	r19, 32(r3)
	stw	r20, 36(r3)
	stw	r21, 40(r3)
	stw	r22, 44(r3)
	stw	r23, 48(r3)
	stw	r24, 52(r3)
	stw	r25, 56(r3)
	stw	r26, 60(r3)
	stw	r27, 64(r3)
	stw	r28, 68(r3)
	stw	r29, 72(r3)
	stw	r30, 76(r3)
	stw	r31, 80(r3)
	mflr	r4		/* save lr */
	stw	r4,  84(r3)

	li	r3, 0		/* set return value 0 */
	blr



.text
.globl arch_longjmp

/*
void arch_longjmp(jmp_buf env, int val)
*/

arch_longjmp:
	lwz	r1,   0(r3)	/* jmp_buf in r3 */
	lwz	r5,   4(r3)	/* restore cr */
	mtcr	r5
	lwz	r13,  8(r3)
	lwz	r14, 12(r3)
	lwz	r15, 16(r3)
	lwz	r16, 20(r3)
	lwz	r17, 24(r3)
	lwz	r18, 28(r3)
	lwz	r19, 32(r3)
	lwz	r20, 36(r3)
	lwz	r21, 40(r3)
	lwz	r22, 44(r3)
	lwz	r23, 48(r3)
	lwz	r24, 52(r3)
	lwz	r25, 56(r3)
	lwz	r26, 60(r3)
	lwz	r27, 64(r3)
	lwz	r28, 68(r3)
	lwz	r29, 72(r3)
	lwz	r30, 76(r3)
	lwz	r31, 80(r3)
	lwz	r5,  84(r3)	/* restore lr */
	mtlr	r5

	mr	r3, r4		/* return val in r4 */
	blr



.text
.globl arch_dojmp

/*
void arch_dojmp(void *pc, unsigned long sp, int val)

saves context on old stack and calls int (*pc)(int val) on new stack sp
*/

arch_dojmp:
	mtlr	r3		/* load link register with pc */
#if 0
	subi	r4, r4, 15	/* keep min. 16 bytes room on new stack: subtract 15 ... */
	rlwinm  r1, r4, 0, 0, 27	/* ... and align to 16 bytes */
#else
	/* FIXME: this version always has 16bytes on stack! */
	rlwinm  r4, r4, 0, 0, 27	/* align stack to 16 bytes */
	subi	r1, r4, 16		/* and subtract 16 */
#endif
	mr	r3, r5		/* pass val in r3 */
	blr			/* jump to new function */
