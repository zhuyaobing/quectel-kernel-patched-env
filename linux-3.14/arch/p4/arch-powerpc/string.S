/*
 * String handling functions for PowerPC.
 *
 * Copyright (C) 1996 Paul Mackerras.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include <asm/processor.h>
#include <asm/cache.h>
#include <asm/errno.h>
#include <arch/asm/ppc_asm.h>

CACHELINE_BYTES = L1_CACHE_BYTES
LG_CACHELINE_BYTES = L1_CACHE_SHIFT
CACHELINE_MASK = (L1_CACHE_BYTES-1)

_GLOBAL(strcpy)
	addi	r5,r3,-1
	addi	r4,r4,-1
1:	lbzu	r0,1(r4)
	cmpwi	0,r0,0
	stbu	r0,1(r5)
	bne	1b
	blr

/* This clears out any unused part of the destination buffer,
   just as the libc version does.  -- paulus */
_GLOBAL(strncpy)
	PPC_LCMPI 0,r5,0
	beqlr
	mtctr	r5
	addi	r6,r3,-1
	addi	r4,r4,-1
1:	lbzu	r0,1(r4)
	cmpwi	0,r0,0
	stbu	r0,1(r6)
	bdnzf	2,1b		/* dec ctr, branch if ctr != 0 && !cr0.eq */
	bnelr			/* if we didn't hit a null char, we're done */
	mfctr	r5
	PPC_LCMPI 0,r5,0	/* any space left in destination buffer? */
	beqlr			/* we know r0 == 0 here */
2:	stbu	r0,1(r6)	/* clear it out if so */
	bdnz	2b
	blr

_GLOBAL(strcat)
	addi	r5,r3,-1
	addi	r4,r4,-1
1:	lbzu	r0,1(r5)
	cmpwi	0,r0,0
	bne	1b
	addi	r5,r5,-1
1:	lbzu	r0,1(r4)
	cmpwi	0,r0,0
	stbu	r0,1(r5)
	bne	1b
	blr

_GLOBAL(strcmp)
	addi	r5,r3,-1
	addi	r4,r4,-1
1:	lbzu	r3,1(r5)
	cmpwi	1,r3,0
	lbzu	r0,1(r4)
	subf.	r3,r0,r3
	beqlr	1
	beq	1b
	blr

_GLOBAL(strncmp)
	PPC_LCMPI 0,r5,0
	beq-	2f
	mtctr	r5
	addi	r5,r3,-1
	addi	r4,r4,-1
1:	lbzu	r3,1(r5)
	cmpwi	1,r3,0
	lbzu	r0,1(r4)
	subf.	r3,r0,r3
	beqlr	1
	bdnzt	eq,1b
	blr
2:	li	r3,0
	blr

_GLOBAL(strlen)
	addi	r4,r3,-1
1:	lbzu	r0,1(r4)
	cmpwi	0,r0,0
	bne	1b
	subf	r3,r3,r4
	blr

/*
 * Use dcbz on the complete cache lines in the destination
 * to set them to zero.  This requires that the destination
 * area is cacheable.  -- paulus
 */
_GLOBAL(cacheable_memzero)
	mr	r5,r4
	li	r4,0
	addi	r6,r3,-4
	cmplwi	0,r5,4
	blt	7f
	stwu	r4,4(r6)
	beqlr
	andi.	r0,r6,3
	add	r5,r0,r5
	subf	r6,r0,r6
	clrlwi	r7,r6,32-LG_CACHELINE_BYTES
	add	r8,r7,r5
	srwi	r9,r8,LG_CACHELINE_BYTES
	addic.	r9,r9,-1	/* total number of complete cachelines */
	ble	2f
	xori	r0,r7,CACHELINE_MASK & ~3
	srwi.	r0,r0,2
	beq	3f
	mtctr	r0
4:	stwu	r4,4(r6)
	bdnz	4b
3:	mtctr	r9
	li	r7,4
#if !defined(CONFIG_8xx)
10:	dcbz	r7,r6
#else
10:	stw	r4, 4(r6)
	stw	r4, 8(r6)
	stw	r4, 12(r6)
	stw	r4, 16(r6)
#if CACHE_LINE_SIZE >= 32
	stw	r4, 20(r6)
	stw	r4, 24(r6)
	stw	r4, 28(r6)
	stw	r4, 32(r6)
#endif /* CACHE_LINE_SIZE */
#endif
	addi	r6,r6,CACHELINE_BYTES
	bdnz	10b
	clrlwi	r5,r8,32-LG_CACHELINE_BYTES
	addi	r5,r5,4
2:	srwi	r0,r5,2
	mtctr	r0
	bdz	6f
1:	stwu	r4,4(r6)
	bdnz	1b
6:	andi.	r5,r5,3
7:	cmpwi	0,r5,0
	beqlr
	mtctr	r5
	addi	r6,r6,3
8:	stbu	r4,1(r6)
	bdnz	8b
	blr

_GLOBAL(memset)
	rlwimi	r4,r4,8,16,23
	rlwimi	r4,r4,16,0,15
	addi	r6,r3,-4
	cmplwi	0,r5,4
	blt	7f
	stwu	r4,4(r6)
	beqlr
	andi.	r0,r6,3
	add	r5,r0,r5
	subf	r6,r0,r6
	srwi	r0,r5,2
	mtctr	r0
	bdz	6f
1:	stwu	r4,4(r6)
	bdnz	1b
6:	andi.	r5,r5,3
7:	cmpwi	0,r5,0
	beqlr
	mtctr	r5
	addi	r6,r6,3
8:	stbu	r4,1(r6)
	bdnz	8b
	blr

_GLOBAL(memmove)
	cmplw	0,r3,r4
	bgt	backwards_memcpy
	/* fall through */

_GLOBAL(memcpy)
	srwi.	r7,r5,3
	addi	r6,r3,-4
	addi	r4,r4,-4
	beq	2f			/* if less than 8 bytes to do */
	andi.	r0,r6,3			/* get dest word aligned */
	mtctr	r7
	bne	5f
1:	lwz	r7,4(r4)
	lwzu	r8,8(r4)
	stw	r7,4(r6)
	stwu	r8,8(r6)
	bdnz	1b
	andi.	r5,r5,7
2:	cmplwi	0,r5,4
	blt	3f
	lwzu	r0,4(r4)
	addi	r5,r5,-4
	stwu	r0,4(r6)
3:	cmpwi	0,r5,0
	beqlr
	mtctr	r5
	addi	r4,r4,3
	addi	r6,r6,3
4:	lbzu	r0,1(r4)
	stbu	r0,1(r6)
	bdnz	4b
	blr
5:	subfic	r0,r0,4
	mtctr	r0
6:	lbz	r7,4(r4)
	addi	r4,r4,1
	stb	r7,4(r6)
	addi	r6,r6,1
	bdnz	6b
	subf	r5,r0,r5
	rlwinm.	r7,r5,32-3,3,31
	beq	2b
	mtctr	r7
	b	1b

_GLOBAL(backwards_memcpy)
	rlwinm.	r7,r5,32-3,3,31		/* r0 = r5 >> 3 */
	add	r6,r3,r5
	add	r4,r4,r5
	beq	2f
	andi.	r0,r6,3
	mtctr	r7
	bne	5f
1:	lwz	r7,-4(r4)
	lwzu	r8,-8(r4)
	stw	r7,-4(r6)
	stwu	r8,-8(r6)
	bdnz	1b
	andi.	r5,r5,7
2:	cmplwi	0,r5,4
	blt	3f
	lwzu	r0,-4(r4)
	subi	r5,r5,4
	stwu	r0,-4(r6)
3:	cmpwi	0,r5,0
	beqlr
	mtctr	r5
4:	lbzu	r0,-1(r4)
	stbu	r0,-1(r6)
	bdnz	4b
	blr
5:	mtctr	r0
6:	lbzu	r7,-1(r4)
	stbu	r7,-1(r6)
	bdnz	6b
	subf	r5,r0,r5
	rlwinm.	r7,r5,32-3,3,31
	beq	2b
	mtctr	r7
	b	1b

_GLOBAL(memcmp)
	PPC_LCMPI 0,r5,0
	beq-	2f
	mtctr	r5
	addi	r6,r3,-1
	addi	r4,r4,-1
1:	lbzu	r3,1(r6)
	lbzu	r0,1(r4)
	subf.	r3,r0,r3
	bdnzt	2,1b
	blr
2:	li	r3,0
	blr

_GLOBAL(memchr)
	PPC_LCMPI 0,r5,0
	beq-	2f
	mtctr	r5
	addi	r3,r3,-1
1:	lbzu	r0,1(r3)
	cmpw	0,r0,r4
	bdnzf	2,1b
	beqlr
2:	li	r3,0
	blr

