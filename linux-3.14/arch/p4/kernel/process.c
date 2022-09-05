/*
 * Copyright (C) 2000, 2001, 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 *
 * adapted for P4/Linux by Alex Zuepke <azu@sysgo.de>
 * 2009-09-15, den@sysgo.com: added architecture independent sys_clone()
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/utsname.h>
#include <linux/fs.h>
#include <linux/utime.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/capability.h>
#include <linux/random.h>
#include <linux/ptrace.h>
#include <linux/start_kernel.h>
#include <linux/tick.h>
#include <asm/mman.h>
#include <asm/segment.h>
#include <asm/stat.h>
#include <asm/pgtable.h>
#include <asm/processor.h>
#include <asm/pgalloc.h>
#include <asm/uaccess.h>
#include <asm/user.h>
#include <asm/internals.h>
#include <asm/kernthreads.h>
#include <asm/vmapi_calls.h>
#include <asm/host_threads.h>
#include <asm/host_tasks.h>
#include <asm/mmu_context.h>

void arch_cpu_idle_prepare(void)
{
	atomic_inc(&init_mm.mm_count);
	current->active_mm = &init_mm;
	BUG_ON(current->mm);
	enter_lazy_tlb(&init_mm, current);
}

void arch_cpu_idle(void)
{
	/*
	 * We need to re-enable local IRQs before going into idle to receive the
	 * wakeup signal up by the timer / IPI interrupt. A real hardware always
	 * wakes up even when interrupts are disabled (check wfi on ARM).
	 */
	local_irq_enable();
	kern_idle(smp_processor_id());
}

/* Free current thread data structures etc ... */
void exit_thread(void)
{
}

/* called from fs/exec.c */
void flush_thread(void)
{
	arch_clear_tls(current);

	flush_context();
}

/*
 * We keep the current thread pointer in this variable
 * Keep in sync!
 */
struct thread_info *__current_thread[NR_CPUS];
EXPORT_SYMBOL(__current_thread);


/*
 * switch between two kernel tasks
 * uses switch_buf from thread_struct
 */
void *__switch_to(struct task_struct *prev,
	struct task_struct *next)
{
	arch_jmp_buf *next_buf;
	unsigned int cpu = smp_processor_id();

	/* XXX need to check runqueues[cpu].idle */
	next->thread.prev_sched = prev;

	/* keep current thread in sync */
	__current_thread[cpu] = task_thread_info(next);
	BUG_ON(cpu != task_thread_info(next)->cpu);

	/* Thread switching here */
	next_buf = &next->thread.switch_buf;
	if (arch_setjmp(prev->thread.switch_buf) == 0)
		arch_longjmp(*next_buf, 1);

	return current->thread.prev_sched;
}

/*
 * This is called magically, by its address being stuffed in a jmp_buf
 * and being longjmp-d to.
 */
void new_thread_handler(void)
{
	int (*fn)(void *), n;
	void *arg;

	if (current->thread.prev_sched != NULL) {
		schedule_tail(current->thread.prev_sched);
		current->thread.prev_sched = NULL;
	}

	fn = current->thread.new_thread.proc;
	arg = current->thread.new_thread.arg;

	/*
	 * callback returns only if the kernel thread execs a process
	 */
	n = fn(arg);
	userspace(&current->thread.regs);
}

/* Called magically, see new_thread_handler above */
void fork_handler(void)
{
	schedule_tail(current->thread.prev_sched);
	current->thread.prev_sched = NULL;

	userspace(&current->thread.regs);
}

int copy_thread(unsigned long clone_flags, unsigned long sp,
		unsigned long arg, struct task_struct * p)
{
	int kthread = current->flags & PF_KTHREAD;
	struct pt_regs *regs = current_pt_regs();
	int ret;
	void (*handler)(void);

	p->thread = (struct thread_struct) INIT_THREAD;	/* setup valid context */
	p->thread.kernel_stack = (unsigned long) task_thread_info(p) + THREAD_SIZE - 16;

	if (!kthread) {
		/* forking userspace context: copy context, this will also set
		 * is_user
		 */
		memcpy(&p->thread.regs, &current->thread.regs, sizeof(p->thread.regs));

		/* set proper return from fork values */
		PT_REGS_SET_SYSCALL_RETURN(&p->thread.regs, 0);
		if (sp != 0)
			PT_REGS_SP(&p->thread.regs) = sp;
		/* Here is a diff between P4Linux and native. A native Linux
		 * will use the same stack for kernel and user address space,
		 * thus KSTK_ESP is valid in both modes. On P4Linux we cannot
		 * use the same stack because we have a distinct address space.
		 */

		handler = fork_handler;
	} else {
		/* creating kernel thread or initial userspace context */
		/* NOTE: a valid context was set by the INIT_THREAD macro */
		p->thread.new_thread.proc = (int (*)(void *))sp;
		p->thread.new_thread.arg = (void *)arg;

		handler = new_thread_handler;
	}

	arch_set_jmp_buf(p->thread.switch_buf, handler, (unsigned long) p->thread.kernel_stack);

	ret = 0;
	/* Set a new TLS for the child thread? */
	if (clone_flags & CLONE_SETTLS)
		ret = arch_copy_tls(p, regs);

	return ret;
}

void early_init_process_setup(void)
{
	/* set current_thread pointer before starting the kernel */
	__current_thread[0] = &init_thread_info;

#ifdef CONFIG_SMP
	init_cpu_online(get_cpu_mask(0));
#endif
}

/*
 * Switch to the init_task stack and finally call start_kernel
 * start_kernel_thread is called by setup.c
 */
void __init start_kernel_thread(void)
{
	unsigned long stack;

	stack = (unsigned long) &init_stack + sizeof(init_stack);

	/* start Linux MAIN thread on CPU #0 */
	kthreads_boot_cpu(0, start_kernel, (void*) stack);
}

/*
 * Only x86 and x86_64 have an arch_align_stack().
 * All other arches have "#define arch_align_stack(x) (x)"
 * in their asm/system.h
 * As this is included in UML from asm-um/system-generic.h,
 * we can use it to behave as the subarch does.
 */
#ifndef arch_align_stack
unsigned long arch_align_stack(unsigned long sp)
{
	if (randomize_va_space)
		sp -= get_random_int() % 8192;
	return sp & ~0xf;
}
#endif

/*
 * Return the address of the waiting kernel function. Locking and scheduler
 * functions are filtered out (in_sched_functions). The low level PikeOS locking
 * and interrupt functions are also marked as __lockfunc to not appear in this
 * list. Be aware, only SMP will activate the lock section, on UP you'll still
 * see the low level lock functions.
 */
unsigned long get_wchan(struct task_struct *p)
{
	unsigned long stack_page, sp, ip;
	bool seen_sched = 0;

	if ((p == NULL) || (p == current) || (p->state == TASK_RUNNING))
		return 0;

	stack_page = (unsigned long) task_stack_page(p);
	/* Bail if the process has no kernel stack for some reason */
	if (stack_page == 0)
		return 0;

	sp = KSTK_ESP(p);
	/*
	 * Bail if the stack pointer is below the bottom of the kernel
	 * stack for some reason
	 */
	if (sp < stack_page)
		return 0;

	while (sp < stack_page + THREAD_SIZE) {
		ip = *((unsigned long *) sp);
		if (in_sched_functions(ip))
			/* Ignore everything until we're above the scheduler */
			seen_sched = 1;
		else if (kernel_text_address(ip) && seen_sched)
			return ip;

		sp += sizeof(unsigned long);
	}

	return 0;
}

/* Prepare user space register context */

// FIXME: SMP: this should go into ARCH
// FIXME: SMP: and it should disable/initialize all registers correctly

// FIXME: SMP: this is called during execve, so the current thread's registers
// FIXME: SMP: are dropped here

void start_thread(struct pt_regs *regs, unsigned long eip, unsigned long esp)
{
	set_fs(USER_DS);
#ifdef CONFIG_P4_ARM
	/* FIXME: all archs? 
	 * on ARM strace produces the following error when HOST_IP is not 0:
	 * "stray syscall exit"
	 */
	memset(regs->context, 0, sizeof(regs->context));
	/* enable ARM thumb mode */
	if ((elf_hwcap & HWCAP_THUMB) && (eip & 1))
		regs->context[HOST_CPSR] |= PSR_T_BIT;
#endif
	PT_REGS_IP(regs) = eip;
	PT_REGS_SP(regs) = esp;

	regs->is_user = 1;

	current->ptrace &= ~PT_DTRACE;
}
EXPORT_SYMBOL(start_thread);

void arch_trigger_all_cpu_backtrace(void)
{
	printk(KERN_ERR "Stack for the current CPU#%d\n", smp_processor_id());
	show_stack(NULL, NULL);
}
