/*
 * linux/arch/p4/arch-ppc/syscalls.c
 *
 * 25 oct 2006, azu@sysgo.de: revamped for kernel 2.6.15
 */

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/sys.h>
#include <linux/ptrace.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/sem.h>
#include <linux/msg.h>
#include <linux/shm.h>
#include <linux/stat.h>
#include <linux/mman.h>
#include <linux/ipc.h>
#include <linux/utsname.h>
#include <linux/file.h>
#include <linux/init.h>
#include <linux/personality.h>
#include <asm/unistd.h>
#include <asm/signal.h>
#include <asm/uaccess.h>
#include <asm/syscalls.h>
#include <asm/internals.h>
#include <linux/ipc.h>
#include <linux/semaphore.h>

/*
 * syscall handing
 *
 * de-obfuscated from entry.S, kernel 2.6 version
 */
extern syscall_handler_t *sys_call_table[];

void arch_handle_syscall(struct pt_regs *regs)
{
	unsigned long syscall;
	long result;
	struct thread_info *ti;

	/* last_syscall seems not to be necessary, so we don't care about it */

	/* preparation: clean entry */
	regs->trap = 0x0c00;
	regs->context[HOST_CR] &= ~0x10000000;		/* clear S0 bit in CR */
	regs->orig_r3  = regs->context[HOST_R3];	/* save "ORIG_R3" */
	regs->result = 0;

	ti = task_thread_info(current);

	if (unlikely(test_thread_flag(TIF_SYSCALL_TRACE)))
		syscall_trace(regs, 0);

	syscall = regs->context[HOST_R0];

	/* normal path */
	if (syscall < (unsigned long)NR_syscalls && sys_call_table[syscall])
		result = EXECUTE_SYSCALL(syscall, regs);
	else
		result = -ENOSYS;

	regs->result = result;

	/* ret_from_syscall path */
	if (result < 0 && result > -_LAST_ERRNO && !test_thread_flag(TIF_NOERROR))
	{
		regs->context[HOST_CR] |= 0x10000000;
		result = -result;
	}

	regs->context[HOST_R3] = result;

	if (unlikely(test_thread_flag(TIF_NOERROR)))
		clear_thread_flag(TIF_NOERROR);

	/* syscall_exit_work here ... */
	if (unlikely(test_thread_flag(TIF_SYSCALL_TRACE)))
		syscall_trace(regs, 1);
}
