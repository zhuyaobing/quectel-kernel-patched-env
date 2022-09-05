/*
 *  P4Linux SMP subsystem
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 * 
 *  Copyright (C) 2011 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#include <linux/percpu.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/threads.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <linux/hardirq.h>
#include <linux/cpu.h>
#include <linux/delay.h>
#include <linux/profile.h>
#include <asm/smp.h>
#include <asm/processor.h>
#include <asm/internals.h>
#include <asm/host_threads.h>
#include <asm/vmapi_calls.h>


enum ipi_message_type {
	IPI_RESCHEDULE,
	IPI_CALL_FUNC,
	IPI_CALL_FUNC_SINGLE,
	IPI_CPU_STOP,
};

static struct {
	unsigned long bits ____cacheline_aligned;
	unsigned int ipi_irq;
} ipi_data[NR_CPUS] __cacheline_aligned;

/* NOTE: this is run in PikeOS context! */
static void ipi_thread(unsigned int irq, void *dev)
{
	while (1) {
		uk_thread_event_wait();
		do_IRQ(irq);
	}
}

static irqreturn_t ipi_interrupt(int irq, void *dev)
{
	unsigned long *pending_ipis;
	unsigned long which;
	unsigned long ops;
	unsigned int cpu;

	cpu = hard_smp_processor_id();
	pending_ipis = &ipi_data[cpu].bits;

	/* FIXME: SMP: the bit handling stuff seems broken!" */
	while ((ops = xchg(pending_ipis, 0)) != 0) {
	  do {
		which = ops & -ops;
		ops &= ~which;
		which = __ffs(which);

		switch (which) {
		case IPI_RESCHEDULE:
			/* Reschedule callback. */
			scheduler_ipi();
			break;

		case IPI_CALL_FUNC:
			generic_smp_call_function_interrupt();
			break;

		case IPI_CALL_FUNC_SINGLE:
			generic_smp_call_function_single_interrupt();
			break;

		case IPI_CPU_STOP:
			/* FIXME: SMP: this should really kill the CPU! */
			while (1)
				cpu_relax();
			break;

		default:
			printk(KERN_CRIT "Unknown IPI on CPU %d: %lu\n",
			       cpu, which);
			break;
		}
	  } while (ops);

	  mb();	/* Order data access and bit testing. */
	}

	return IRQ_HANDLED;
}

static struct irqaction ipi_irq = {
	.name		= "IPI",
	.handler	= ipi_interrupt,
	.flags		= IRQF_DISABLED,
};

static unsigned int ipi_init(unsigned int cpu)
{
	unsigned int irq;
	char name[32];

	sprintf(name, "IPI CPU#%d", cpu);

	/* setup the interrupt for the ticker */
	irq = get_free_async_irq();
	BUG_ON(!irq);

	setup_irq(irq, &ipi_irq);

	/* startup the IPI thread */
	if (enable_irq_thread(irq, name, ipi_thread, NULL))
		panic("time_init: could not start IPI thread");

	bind_irq_thread(irq, cpu);

	return irq;
}

void smp_prepare_boot_cpu(void)
{
	BUG_ON(hard_smp_processor_id() != 0);

	set_cpu_online(0, true);
}

void smp_prepare_cpus(unsigned int max_cpus)
{
	unsigned int native_cpus;
	unsigned int cpu;

	memset(ipi_data, 0, sizeof(ipi_data));

	native_cpus = kthread_native_cpu_num();

	/* limit to available CPUs */
	if (max_cpus > native_cpus)
		max_cpus = native_cpus;

	if (max_cpus > 1) {
		/* start IPI thread for boot CPU */
		ipi_data[0].ipi_irq = ipi_init(0);

		/* register additional CPUs */
		for (cpu = 0; cpu < max_cpus; cpu++)
			set_cpu_present(cpu, true);
	}
}

static void __cpuinit other_cpu_main(void)
{
	unsigned int cpu = hard_smp_processor_id();

	BUG_ON(cpu == 0);
	BUG_ON(cpu != hard_smp_processor_id());

	local_irq_disable();

	atomic_inc(&init_mm.mm_count);
	current->active_mm = &init_mm;

	preempt_disable();
	notify_cpu_starting(cpu);
	//calibrate_delay();

	/* Enable local timers */
	local_timer_setup(cpu);

	local_irq_enable();

	set_cpu_online(cpu, true);

	cpu_startup_entry(CPUHP_ONLINE);
	/* never returns */
}

static struct task_struct * __cpuinit do_boot_cpu(unsigned int cpu)
{
	struct task_struct *idle;
	unsigned long stack;

	current->thread.new_thread.proc = NULL;
	current->thread.new_thread.arg = NULL;
	idle = fork_idle(cpu);
	if (IS_ERR(idle))
		panic(KERN_ERR "CPU%u: fork() failed\n", cpu);

	stack = (unsigned long) idle->thread.kernel_stack;

	__current_thread[cpu] = task_thread_info(idle);
	task_thread_info(idle)->cpu = cpu;

	kthreads_boot_cpu(cpu, other_cpu_main, (void *) stack);

	return idle;
}

int __cpuinit __cpu_up(unsigned int cpu, struct task_struct *tidle)
{
	struct task_struct *idle;

	idle = do_boot_cpu(cpu);

	/* start IPI thread */
	ipi_data[cpu].ipi_irq = ipi_init(cpu);

	while (!cpu_online(cpu))
		cpu_relax();

	return 0;
}

void smp_cpus_done(unsigned int max_cpus)
{
	/* FIXME: SMP: nothing to do */
}

static inline void send_ipi(unsigned int cpu, unsigned int cmd)
{
	mb();
	set_bit(cmd, &ipi_data[cpu].bits);
	mb();

	uk_thread_ev_signal(get_irq_threadid(ipi_data[cpu].ipi_irq));
}

void smp_send_reschedule(int cpu)
{
	send_ipi(cpu, IPI_RESCHEDULE);
}

void arch_send_call_function_single_ipi(int cpu)
{
	send_ipi(cpu, IPI_CALL_FUNC_SINGLE);
}

void arch_send_call_function_ipi_mask(const struct cpumask *mask)
{
	unsigned int cpu;

	for_each_cpu(cpu, mask)
		send_ipi(cpu, IPI_CALL_FUNC);
}

void smp_send_stop(void)
{
	unsigned int i;

	printk(KERN_INFO "Stopping all %d CPUs...", num_online_cpus());
	for (i = 0; i < num_online_cpus(); i++) {
		if (i == hard_smp_processor_id())
			continue;
		send_ipi(i, IPI_CPU_STOP);
	}
	printk(KERN_CONT "done\n");
}

#ifdef CONFIG_HOTPLUG_CPU
/*
 * __cpu_disable runs on the processor to be shutdown.
 */
int __cpu_disable(void)
{
	unsigned int cpu = smp_processor_id();

	/* we don't allow CPU 0 to be shutdown (due to interrupts) */
	if (cpu == 0)
		return -EPERM;

	/* NOTE: not yet supported, see below */
	return -EPERM;

	/*
	 * Take this CPU offline.  Once we clear this, we can't return,
	 * and we must not schedule until we're ready to give up the cpu.
	 */
	set_cpu_online(cpu, false);

	/* TODO: free resources for local timer and ipi thread */
	return 0;
}

static DECLARE_COMPLETION(cpu_died);
/*
 * called on the thread which is asking for a CPU to be shutdown
 */
void __cpu_die(unsigned int cpu)
{
	if (!wait_for_completion_timeout(&cpu_died, msecs_to_jiffies(5000))) {
		pr_err("CPU%u: cpu didn't die\n", cpu);
		return;
	}

	printk(KERN_NOTICE "CPU%u: shutdown\n", cpu);
}

/* Called from the idle thread for the CPU which has been shutdown. */
void arch_cpu_idle_dead(void)
{
	/* Tell __cpu_die() that this CPU is now safe to dispose of */
	complete(&cpu_died);

	/* TODO: notify PikeOS to stop current CPU */
	BUG_ON(1);

	while (1)
		vmapi_delay(1);
}

#endif
