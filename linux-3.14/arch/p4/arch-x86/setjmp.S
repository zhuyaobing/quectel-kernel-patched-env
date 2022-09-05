/*
 *  simple setjmp/longjmp for i386
 * 
 *  Copyright (C) 2003 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

/* order in arch_jmp_buf
 *
 *  0 ebx
 *  4 esi
 *  8 edi
 * 12 ebp
 * 16 esp
 * 20 eip
 */

.text
.align 16
.globl arch_setjmp

/*
int arch_setjmp(jmp_buf env);
*/

arch_setjmp:
	push %ebp		# store old ebp

	mov 8(%esp),%ebp	# ebp points to env

	mov %ebx,0(%ebp)	# save ebx
	mov %esi,4(%ebp)	# save esi
	mov %edi,8(%ebp)	# save edi
	mov 0(%esp),%eax
	mov %eax,12(%ebp)	# save ebp
	mov %esp,16(%ebp)	# save esp

	mov 4(%esp),%eax
	mov %eax,20(%ebp)	# save eip
	xor %eax,%eax
	pop %ebp
	ret


.text
.align 16
.globl arch_longjmp

/*
void arch_longjmp(jmp_buf env, int val)
*/

arch_longjmp:
	push %ebp		# spare
	mov 12(%esp),%ecx	# val
	mov 8(%esp),%ebp	# ebp points to env

	mov 0(%ebp),%ebx	# restore ebx
	mov 4(%ebp),%esi	# restore esi
	mov 8(%ebp),%edi	# restore edi
	mov 16(%ebp),%esp	# restore esp (the old stack)
	mov 12(%ebp),%eax
	mov %eax,0(%esp)	# put old ebp on stack
	mov 20(%ebp),%eax
	mov %eax,4(%esp)	# put old eip on stack

	mov %ecx,%eax		# return value
	test %eax,%eax		# must be != 0
	jne 1f
	inc %eax
1:	pop %ebp		# restore ebp
	ret


.text
.align 16
.globl arch_dojmp

/*
void arch_dojmp(void *pc, unsigned long sp, int val)

saves context on old stack and calls int (*pc)(int val) on new stack sp
*/

arch_dojmp:
	mov 12(%esp),%eax	# val
	mov 8(%esp),%edx	# sp
	mov 4(%esp),%ebx	# pc
	mov %edx,%esp
	push %eax		# param: val
	pushl $0		# push invalid return address
	jmp *%ebx
