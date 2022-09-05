/* Copyright (c) 2016 OpenSynergy GmbH */
/*
 * linux/arch/p4/arch-arm/traps.c - ARM special syscall handling
 *
 * taken from linux/arch/arm/kernel/traps.c and reduced
 *
 * 2009-09-22, David Engraf <den@sysgo.com>
 */

#include <linux/module.h>
#include <linux/signal.h>
#include <linux/spinlock.h>
#include <linux/personality.h>
#include <linux/kallsyms.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/kprobes.h>

#include <asm/atomic.h>
#include <asm/cacheflush.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/io.h>

void arm_notify_die(const char *str, struct pt_regs *regs,
		struct siginfo *info, unsigned long err, unsigned long trap)
{
	if (user_mode(regs)) {
		force_sig_info(info->si_signo, info, current);
	} else {
		panic(str);
	}
}

static int bad_syscall(int n, struct pt_regs *regs)
{
	struct thread_info *thread = current_thread_info();
	siginfo_t info;

	if (current->personality != PER_LINUX &&
	    current->personality != PER_LINUX_32BIT &&
	    thread->exec_domain->handler) {
		thread->exec_domain->handler(n, regs);
		return regs->context[HOST_R0];
	}

#ifdef CONFIG_DEBUG_USER
	if (user_debug & UDBG_SYSCALL) {
		printk(KERN_ERR "[%d] %s: obsolete system call %08x.\n",
			task_pid_nr(current), current->comm, n);
		dump_instr(regs);
	}
#endif

	info.si_signo = SIGILL;
	info.si_errno = 0;
	info.si_code  = ILL_ILLTRP;
	info.si_addr  = (void __user *)instruction_pointer(regs) -
			 (thumb_mode(regs) ? 2 : 4);

	arm_notify_die("Oops - bad syscall", regs, &info, n, 0);

	return regs->context[HOST_R0];
}

static inline void
do_cache_op(unsigned long start, unsigned long end, int flags)
{
	struct mm_struct *mm = current->active_mm;
	struct vm_area_struct *vma;

	if (end < start || flags)
		return;

	down_read(&mm->mmap_sem);
	vma = find_vma(mm, start);
	if (vma && vma->vm_start < end) {
		if (start < vma->vm_start)
			start = vma->vm_start;
		if (end > vma->vm_end)
			end = vma->vm_end;

		/* Native ARM Linux flush_cache_user_range() aligns to pages! */
		flush_icache_range_user(start & PAGE_MASK, PAGE_ALIGN(end));
	}
	up_read(&mm->mmap_sem);
}

/*
 * Handle all unrecognised system calls.
 *  0x9f0000 - 0x9fffff are some more esoteric system calls
 */
//TODO: ARM_EABI
#undef __NR_SYSCALL_BASE
#define __NR_SYSCALL_BASE	0
#define NR(x) ((__ARM_NR_##x) - __ARM_NR_BASE)

/*
 * getrandom(2) syscall introduced with Linux kernel 3.17
 *
 * For details see:
 *
 * https://lwn.net/Articles/605828/
 * http://lxr.free-electrons.com/source/arch/arm/kernel/calls.S?v=3.17#L396
 *
 */
#define __NR_renameat2 (__NR_SYSCALL_BASE + 382)         // introduced in 3.15
#define __NR_getrandom (__NR_SYSCALL_BASE + 384)         // introduced in 3.17
#define __NR_memfd_create (__NR_SYSCALL_BASE + 385)      // introduced in 3.17
#define __NR_copy_file_range (__NR_SYSCALL_BASE + 391)   // introduced in 4.5
int arm_syscall(int no, struct pt_regs *regs)
{
	siginfo_t info;

	/* special handling for arch-specific renameat2(2) syscall*/
	/* special handling for arch-specific getrandom(2) syscall*/
	/* special handling for arch-specific memfd_create(2) syscall*/
	/* special handling for arch-specific copy_file_range(2) syscall*/
	if (no == __NR_renameat2 ||
	    no == __NR_getrandom ||
	    no == __NR_memfd_create ||
	    no == __NR_copy_file_range) {
		/*
		 * Rather than raising SIGILL return -ENOSYS. This way the
		 * calling process can gracefully determine whether this feature
		 * is supported and use alternative.
		 */
		return -ENOSYS;
	}

	if ((no >> 16) != (__ARM_NR_BASE>> 16))
		return bad_syscall(no, regs);

	switch (no & 0xffff) {
	case 0: /* branch through 0 */
		info.si_signo = SIGSEGV;
		info.si_errno = 0;
		info.si_code  = SEGV_MAPERR;
		info.si_addr  = NULL;

		arm_notify_die("branch through zero", regs, &info, 0, 0);
		return 0;

	case NR(breakpoint): /* SWI BREAK_POINT */
		regs->context[HOST_EPC] -= thumb_mode(regs) ? 2 : 4;
		//ptrace_break(current, regs);
		printk(KERN_ERR "den: %s breakpoint not complete\n", __FUNCTION__);
		return regs->context[HOST_R0];

	/*
	 * Flush a region from virtual address 'r0' to virtual address 'r1'
	 * _exclusive_.  There is no alignment requirement on either address;
	 * user space does not need to know the hardware cache layout.
	 *
	 * r2 contains flags.  It should ALWAYS be passed as ZERO until it
	 * is defined to be something else.  For now we ignore it, but may
	 * the fires of hell burn in your belly if you break this rule. ;)
	 *
	 * (at a later date, we may want to allow this call to not flush
	 * various aspects of the cache.  Passing '0' will guarantee that
	 * everything necessary gets flushed to maintain consistency in
	 * the specified region).
	 */
	case NR(cacheflush):
		do_cache_op(regs->context[HOST_R0], regs->context[HOST_R1], regs->context[HOST_R2]);
		return 0;

	case NR(usr26):
		if (!(elf_hwcap & HWCAP_26BIT))
			break;
		regs->context[HOST_CPSR] &= ~MODE32_BIT;
		return regs->context[HOST_R0];

	case NR(usr32):
		if (!(elf_hwcap & HWCAP_26BIT))
			break;
		regs->context[HOST_CPSR] |= MODE32_BIT;
		return regs->context[HOST_R0];

	case NR(set_tls):
		regs->context[HOST_TLS] = regs->context[HOST_R0];
		return 0;

#ifdef CONFIG_NEEDS_SYSCALL_FOR_CMPXCHG
	/*
	 * Atomically store r1 in *r2 if *r2 is equal to r0 for user space.
	 * Return zero in r0 if *MEM was changed or non-zero if no exchange
	 * happened.  Also set the user C flag accordingly.
	 * If access permissions have to be fixed up then non-zero is
	 * returned and the operation has to be re-attempted.
	 *
	 * *NOTE*: This is a ghost syscall private to the kernel.  Only the
	 * __kuser_cmpxchg code in entry-armv.S should be aware of its
	 * existence.  Don't ever use this from user code.
	 */
	case 0xfff0:
	for (;;) {
		unsigned long val;

		get_user(val, (int __user*)regs->context[HOST_R2]);
		regs->context[HOST_CPSR] &= ~PSR_C_BIT;
		val -= regs->context[HOST_R0];
		if (val == 0) {
			put_user(regs->context[HOST_R1], (int __user*)regs->context[HOST_R2]);
			regs->context[HOST_CPSR] |= PSR_C_BIT;
		}

		return val;
	}
#endif

	default:
		/* Calls 9f00xx..9f07ff are defined to return -ENOSYS
		   if not implemented, rather than raising SIGILL.  This
		   way the calling program can gracefully determine whether
		   a feature is supported.  */
		if (no <= 0x7ff)
			return -ENOSYS;
		break;
	}
#ifdef CONFIG_DEBUG_USER
	/*
	 * experience shows that these seem to indicate that
	 * something catastrophic has happened
	 */
	if (user_debug & UDBG_SYSCALL) {
		printk("[%d] %s: arm syscall %d\n",
		       task_pid_nr(current), current->comm, no);
		dump_instr(regs);
		if (user_mode(regs)) {
			__show_regs(regs);
			c_backtrace(regs->ARM_fp, processor_mode(regs));
		}
	}
#endif
	info.si_signo = SIGILL;
	info.si_errno = 0;
	info.si_code  = ILL_ILLTRP;
	info.si_addr  = (void __user *)instruction_pointer(regs) -
			 (thumb_mode(regs) ? 2 : 4);

	arm_notify_die("Oops - bad syscall(2)", regs, &info, no, 0);
	return 0;
}

asmlinkage void __div0(void)
{
	printk("Division by zero in kernel.\n");
	dump_stack();
}
EXPORT_SYMBOL(__div0);

void abort(void)
{
	BUG();

	/* if that doesn't kill us, halt */
	panic("Oops failed to kill thread");
}
EXPORT_SYMBOL(abort);

void __bad_xchg(volatile void *ptr, int size)
{
	printk("xchg: bad data size: pc 0x%p, ptr 0x%p, size %d\n",
		__builtin_return_address(0), ptr, size);
	BUG();
}
EXPORT_SYMBOL(__bad_xchg);

void __attribute__((noreturn)) __bug(const char *file, int line)
{
	printk(KERN_CRIT"kernel BUG at %s:%d!\n", file, line);
	*(int *)0 = 0;

	/* Avoid "noreturn function does return" */
	for (;;);
}
EXPORT_SYMBOL(__bug);
