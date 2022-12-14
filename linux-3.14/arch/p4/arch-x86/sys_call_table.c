/*
 * System call table for UML/i386, copied from arch/x86/kernel/syscall_*.c
 * with some changes for UML.
 */

#include <linux/linkage.h>
#include <linux/sys.h>
#include <linux/cache.h>
#include <asm/asm-offsets.h>

/* Not going to be implemented by P4Linux. */
#define sys_vm86old sys_ni_syscall
#define sys_vm86 sys_ni_syscall

#define old_mmap sys_old_mmap

#define ptregs_iopl sys_iopl
#define ptregs_vm86old sys_vm86old
#define ptregs_vm86 sys_vm86
#define ptregs_sigreturn sys_sigreturn
#define ptregs_rt_sigreturn sys_rt_sigreturn

#define __SYSCALL_I386(nr, sym, compat) extern asmlinkage void sym(void) ;
#include <asm/syscalls_32.h>

#undef __SYSCALL_I386
#define __SYSCALL_I386(nr, sym, compat) [ nr ] = sym,

typedef asmlinkage void (*sys_call_ptr_t)(void);

extern asmlinkage void sys_ni_syscall(void);

const sys_call_ptr_t sys_call_table[] __cacheline_aligned = {
	/*
	 * Smells like a compiler bug -- it doesn't work
	 * when the & below is removed.
	 */
	[0 ... __NR_syscall_max] = &sys_ni_syscall,
#include <asm/syscalls_32.h>
};
