/*
 * System call table for UML/x86-64, copied from arch/x86/kernel/syscall_*.c
 * with some changes for UML.
 */

#include <linux/linkage.h>
#include <linux/sys.h>
#include <linux/cache.h>
#include <asm/asm-offsets.h>

/*
 * The UML TLS problem. Note that x86_64 does not implement this, so the below
 * is needed only for the ia32 compatibility.
 */

/* same handling as UML (check syscall.c) */
#define sys_mmap old_mmap

#define stub_clone sys_clone
#define stub_fork sys_fork
#define stub_vfork sys_vfork
#define stub_execve sys_execve
#define stub_rt_sigreturn sys_rt_sigreturn
#define stub_iopl sys_iopl

#define __SYSCALL_COMMON(nr, sym, compat) __SYSCALL_64(nr, sym, compat)
#define __SYSCALL_X32(nr, sym, compat) /* Not supported */

#define __SYSCALL_64(nr, sym, compat) extern asmlinkage void sym(void) ;
#include <asm/syscalls_64.h>

#undef __SYSCALL_64
#define __SYSCALL_64(nr, sym, compat) [ nr ] = sym,

typedef void (*sys_call_ptr_t)(void);

extern void sys_ni_syscall(void);

const sys_call_ptr_t sys_call_table[] __cacheline_aligned = {
	/*
	 * Smells like a compiler bug -- it doesn't work
	 * when the & below is removed.
	 */
	[0 ... __NR_syscall_max] = &sys_ni_syscall,
#include <asm/syscalls_64.h>
};
