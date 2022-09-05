/*
 * linux/arch/p4/arch-arm/syscall.c - ARM syscall handling
 *
 * 2009-09-21, David Engraf <den@sysgo.com>
 */

#include <linux/kernel.h>
#include <linux/thread_info.h>
#include <asm/host_context.h>
#include <asm/syscalls.h>
#include <asm/internals.h>

extern int arm_syscall(int no, struct pt_regs *regs);
extern int num_syscalls;
#define NR_syscalls num_syscalls
void arch_handle_syscall(struct pt_regs *regs)
{
	unsigned int syscall;
	long result;

	if (unlikely(test_thread_flag(TIF_SYSCALL_TRACE)))
		syscall_trace(regs, 0);

	syscall = PT_REGS_SYSCALL_NR(regs);

	if (syscall < NR_syscalls)
		result = EXECUTE_SYSCALL(syscall, regs);
	else
		result = arm_syscall(syscall, regs);

	PT_REGS_SET_SYSCALL_RETURN(regs, result);

	if((result == -ERESTARTNOHAND) ||
	   (result == -ERESTARTSYS) ||
	   (result == -ERESTARTNOINTR))
		do_signal(regs);

	if (unlikely(test_thread_flag(TIF_SYSCALL_TRACE)))
		syscall_trace(regs, 1);
}
