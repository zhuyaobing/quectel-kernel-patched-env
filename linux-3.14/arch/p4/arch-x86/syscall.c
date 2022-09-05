/*
 * architecture syscall handling
 *
 * 20060521, azu@sysgo.de: 
 */

#include <linux/kernel.h>
#include <asm/unistd.h>
#include <asm/mman.h>
#include <asm/errno.h>
#include <asm/signal.h>
#include <asm/uaccess.h>
#include <asm/host_context.h>
#include <asm/syscalls.h>
#include <asm/internals.h>
#include <asm/asm-offsets.h>     /* For NR_syscalls */

void arch_handle_syscall(struct pt_regs *regs)
{
	unsigned int syscall;
	long result;

	if (unlikely(test_thread_flag(TIF_SYSCALL_TRACE)))
	{
		/* on i386, strace expects -ENOSYS in return register */
		regs->context[HOST_AX] = -ENOSYS;
		syscall_trace(regs, 0);
	}

	syscall = PT_REGS_SYSCALL_NR(regs);

	if (syscall < NR_syscalls)
		result = EXECUTE_SYSCALL(syscall, regs);
	else
		result = -ENOSYS;

	PT_REGS_SET_SYSCALL_RETURN(regs, result);

	if((result == -ERESTARTNOHAND) ||
	   (result == -ERESTARTSYS) ||
	   (result == -ERESTARTNOINTR))
		do_signal(regs);

	if (unlikely(test_thread_flag(TIF_SYSCALL_TRACE)))
	{
		syscall_trace(regs, 1);
	}
}
