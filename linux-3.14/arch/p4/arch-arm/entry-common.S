/*
 * linux/arch/p4/arch-arm/entry-common.S - ARM syscall table
 *
 * taken from arch/arm/kernel/entry-common.S and reduced to syscall
 * table and wrapper functions
 *
 * 2009-09-22, David Engraf <den@sysgo.com>
 */

#include <linux/linkage.h>

	.align	5

	.equ NR_syscalls,0
#define CALL(x) .equ NR_syscalls,NR_syscalls+1
#include "../../arm/kernel/calls.S"
#undef CALL
#define CALL(x) .long x

#define ABI(native, compat) native
#define OBSOLETE(syscall) sys_ni_syscall

	.type	sys_call_table, #object
ENTRY(sys_call_table)
#include "../../arm/kernel/calls.S"
#undef ABI
#undef OBSOLETE

	.global num_syscalls
num_syscalls:
	.long NR_syscalls

/*============================================================================
 * Special system call wrappers, removed reading of pt_regs (see calling function)
 */
sys_fork_wrapper:
		b	sys_fork

sys_vfork_wrapper:
		b	sys_vfork

sys_execve_wrapper:
		b	sys_execve

sys_clone_wrapper:
		b	sys_clone

sys_sigsuspend_wrapper:
		b	sys_sigsuspend

sys_rt_sigsuspend_wrapper:
		b	sys_rt_sigsuspend

sys_sigreturn_wrapper:
		b	sys_sigreturn

sys_rt_sigreturn_wrapper:
		b	sys_rt_sigreturn

sys_sigaltstack_wrapper:
		b	sys_sigaltstack

sys_statfs64_wrapper:
		teq	r1, #88
		moveq	r1, #84
		b	sys_statfs64

sys_fstatfs64_wrapper:
		teq	r1, #88
		moveq	r1, #84
		b	sys_fstatfs64

sys_mmap2:
		b	sys_mmap_pgoff
