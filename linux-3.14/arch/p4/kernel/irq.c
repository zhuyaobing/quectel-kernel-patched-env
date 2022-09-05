/*
 * Copyright (C) 2000 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 * Derived (i.e. mostly copied) from arch/i386/kernel/irq.c:
 *	Copyright (C) 1992, 1998 Linus Torvalds, Ingo Molnar
 *
 * Adapted to p4/linux by Alex Zuepke <azu@sysgo.de>
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/smp.h>
#include <linux/kernel_stat.h>
#include <linux/interrupt.h>
#include <linux/random.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/seq_file.h>
#include <linux/profile.h>
#include <linux/hardirq.h>
#include <linux/irqdomain.h>
#include <linux/of.h>
#include <asm/irq.h>
#include <asm/hw_irq.h>
#include <asm/atomic.h>
#include <asm/signal.h>
#include <asm/errno.h>
#include <asm/uaccess.h>
#include <asm/internals.h>
#include <asm/kernthreads.h>
#include <asm/host_threads.h>
#include <asm/smp.h>

#ifdef CONFIG_HAVE_IRQ_XLATE
int bsp_irq_xlate(struct irq_domain *d,
				struct device_node *controller,
				const u32 *intspec, unsigned int intsize,
				unsigned long *out_hwirq, unsigned int *out_type);
#endif

static struct pt_regs dummy_regs;

/* synchronization for IRQ and main thread */
static int P4INT_irq_enable[NUM_P4_IRQS];
static volatile int P4INT_irq_shutdown[NUM_P4_IRQS];

/*
 * main entry point for all IRQ's
 */
void do_IRQ(unsigned int irq)
{
	struct pt_regs *old_regs;
	int ret;

	BUG_ON(arch_irqs_disabled());
	ret = arch_local_irq_lock();
	if (ret == UK_ERR_CANCEL) {
		/* thread was canceled by cancel_irq_thread() check
		 * disable_P4INT_irq or shutdown_P4INT_irq
		 */
		BUG_ON(irq >= NUM_P4_IRQS);	/* must be an INT irq */
		BUG_ON(P4INT_irq_enable[irq]);	/* must not be enabled */
		return;
	}
	BUG_ON(ret);

	/* IRQs are handled with dummy registers, thus we have to ensure that
	 * is_user is correctly configured.
	 */
	dummy_regs.is_user = __is_user_mode(hard_smp_processor_id());
	old_regs = set_irq_regs(&dummy_regs);

	irq_enter();
	generic_handle_irq(irq);
	irq_exit();

	/* test on pending signals etc */
	test_and_wakeup_userspace(hard_smp_processor_id());

	set_irq_regs(old_regs);

	arch_local_irq_unlock();
}
EXPORT_SYMBOL(do_IRQ);

/*
 * P4INT_thread()
 *
 * is hw-irq dispatcher stub and runs in P4 thread context
 *
 * NOTE: do not call any linux functions, as
 *       this code is not executed in linux context!
 */
static void P4INT_thread(unsigned int irq, void *dev)
{
	int err;

	/* assign interrupt to this thread */
	err = uk_thread_irq_attach(irq);

	if (err) {
		printk("P4INT_thread: can't attach to IRQ %d, error 0x%x\n", irq, -err);

		/* sleep forever */
		uk_thread_suspend();
	}

	while (1) {
		/* wait for int */
		if (P4INT_irq_enable[irq])
			err = uk_thread_irq_wait(1);
		else
			err = UK_ERR_CANCEL;

		if (P4INT_irq_shutdown[irq])
			break;

		/* check for errors */
		if (err == UK_ERR_CANCEL) {
			/* disable_P4INT_irq() has cancelled us. wait for
			 * enable_P4INT_irq()'s notification.
			 */
			if (!P4INT_irq_enable[irq])
				uk_thread_ev_wait();
			else
				uk_thread_ev_clear();
		} else if (err) {
			printk("P4INT_thread: failed to wait on IRQ %d, error 0x%x\n", irq, -err);
		} else {
			BUG_ON(!P4INT_irq_enable[irq]);
			do_IRQ(irq);
		}

		if (P4INT_irq_shutdown[irq])
			break;

		/* lazily migrate CPUs (if requested */
		kthread_migrate_irq_thread_lazily();
	}

	P4INT_irq_shutdown[irq] = 2;
	uk_thread_delete(kthread_thread_id());
}




/*
 * Generic P4-Interrupt mask & ack code
 *
 * A thread waiting for the HW IRQ is started and the
 * registered handler is called by do_IRQ().
 *
 * A shutdown kills the helper thread.
 */

static unsigned int startup_P4INT_irq(struct irq_data *data)
{
	char s[20];
	sprintf(s, "hw_irq(%d)", data->irq);

	if (P4INT_irq_shutdown[data->irq]) {
		/* stopped previously, do cleanup now */
		while (P4INT_irq_shutdown[data->irq] != 2)
			cpu_relax();

		disable_irq_thread(data->irq);
	}

	P4INT_irq_shutdown[data->irq] = 0;
	P4INT_irq_enable[data->irq] = 1;
	return enable_irq_thread(data->irq, s, P4INT_thread, NULL);
}

/*
 * shutdown_P4INT_irq
 *
 * NOTE: This function is called before the interrupt is synchronized. Thus
 *       do_IRQ may be active on another CPU and we cannot clear the PikeOS
 *       interrupt thread here. Wakeup thread (when blocking) and let it
 *       terminates itself.
 */
static void shutdown_P4INT_irq(struct irq_data *data)
{
	P4INT_irq_enable[data->irq] = 0;
	P4INT_irq_shutdown[data->irq] = 1;
	cancel_irq_thread(data->irq);
}

static void enable_P4INT_irq(struct irq_data *data)
{
	BUG_ON(P4INT_irq_shutdown[data->irq]);

	P4INT_irq_enable[data->irq] = 1;
	uk_thread_ev_signal(get_irq_threadid(data->irq));
}

static void disable_P4INT_irq(struct irq_data *data)
{
	BUG_ON(P4INT_irq_shutdown[data->irq]);

	P4INT_irq_enable[data->irq] = 0;
	cancel_irq_thread(data->irq);
}

static int set_type_P4INT_irq(struct irq_data *data, unsigned int type)
{
	if (type & IRQ_TYPE_LEVEL_MASK) {
		/* Level interrupts must be handled with additional overhead
		 * due to mask/unmask calls for earch interrupt. Thus we have to
		 * switch to the more complex handler here. The level handler 
		 * is mandatory for threaded interrupts.
		 */
		__irq_set_handler_locked(data->irq, handle_level_irq);

	} else {
		__irq_set_handler_locked(data->irq, handle_simple_irq);
	}

	if (type != IRQ_TYPE_NONE) {
		printk(KERN_ERR "P4INT: IRQ#%d requested %s\n", data->irq,
		       type & IRQ_TYPE_LEVEL_MASK ? "level triggered" :
		       type & IRQ_TYPE_EDGE_BOTH ? "edge triggered" : "unknown");
	}

	return IRQ_SET_MASK_OK;
}

static int set_affinity_P4INT_irq(struct irq_data *data, const struct cpumask *dest, bool force)
{
	unsigned int cpu;
	cpu = cpumask_first(dest);
	if(cpu >= nr_cpumask_bits)
		return -EINVAL;
	migrate_irq_thread(data->irq, cpu);
	return 0;
}

struct irq_chip P4INT_irq_type = {
	.name = "P4INT",
	.irq_startup = startup_P4INT_irq,
	.irq_shutdown = shutdown_P4INT_irq,
	.irq_enable = enable_P4INT_irq,
	.irq_disable = disable_P4INT_irq,
	.irq_set_affinity = set_affinity_P4INT_irq,
	.irq_set_type = set_type_P4INT_irq,
	/* mask/unmask is used by level IRQs only */
	.irq_mask = disable_P4INT_irq,
	.irq_unmask = enable_P4INT_irq,
};


/*
 * Generic interrupt mask & ack code for asynchronous threads
 *
 * This provides means to register an asynchronous thread that acts
 * like an interrupt for the linux system. The user must provide
 * a P4 thread that does the requests in an endless loop and
 * then calls do_IRQ() to provide an interrupt for the linux kernel.
 *
 *
 */

static unsigned int startup_ASYNC_irq(struct irq_data *data)
{
	return 0;
}

static void shutdown_ASYNC_irq(struct irq_data *data)
{
}

static void enable_ASYNC_irq(struct irq_data *data)
{
}

static void disable_ASYNC_irq(struct irq_data *data)
{
}

static struct irq_chip ASYNC_irq_type = {
	.name = "ASYNC",
	.irq_startup = startup_ASYNC_irq,
	.irq_shutdown = shutdown_ASYNC_irq,
	.irq_enable = enable_ASYNC_irq,
	.irq_disable = disable_ASYNC_irq,
};

#ifdef CONFIG_IRQ_DOMAIN
int
dom_match(struct irq_domain *d, struct device_node *node)
{
	return 1;
}

int
dom_map(struct irq_domain *d, unsigned int virq, irq_hw_number_t hw)
{
	if (hw < NUM_P4_IRQS) {
		/* handler will be overwritten by set_type_P4INT_irq() */
		irq_set_chip_and_handler(hw, &P4INT_irq_type, handle_simple_irq);
		P4INT_irq_enable[hw] = 0;
		P4INT_irq_shutdown[hw] = 0;
	} else {
		irq_set_chip_and_handler(hw, &ASYNC_irq_type, handle_simple_irq);
	}
	return 0;
}

void
dom_unmap(struct irq_domain *d, unsigned int virq)
{
}

static struct irq_domain_ops p4_domain = {
	.match = dom_match,
	.map = dom_map,
	.unmap = dom_unmap,
#ifdef CONFIG_HAVE_IRQ_XLATE
	.xlate = bsp_irq_xlate,
#endif
};

void __init init_IRQ(void)
{
	struct irq_domain *irqhost;
	struct device_node *node;

#ifdef CONFIG_OF
	static const struct of_device_id __initconst pic_device_id[] = {
		{ .type	      = "sysgo,p4int", },
		{ .compatible = "sysgo,p4int", },
		{},
	};

	node = of_find_matching_node(NULL, pic_device_id);
#else
	node = NULL;
#endif

	irqhost = irq_domain_add_legacy(node, NR_IRQS, 0, 0, &p4_domain, 0);
	irq_set_default_host(irqhost);
}

#else /* CONFIG_IRQ_DOMAIN */

void __init init_IRQ(void)
{
	unsigned int i;

	/* The PikeOS interrupt model is very simple. Only one interrupt can
	 * occur at the same time until the handler terminates. Acking/masking
	 * and unmasking an interrupt is also not needed because PikeOS already
	 * handles that.
	 */
	for (i = 0; i < NUM_P4_IRQS; i++) {
		/* handler will be overwritten by set_type_P4INT_irq() */
		irq_set_chip_and_handler(i, &P4INT_irq_type, handle_simple_irq);
		P4INT_irq_enable[i] = 0;
		P4INT_irq_shutdown[i] = 0;
	}

	for (; i < NR_IRQS; i++) {
		irq_set_chip_and_handler(i, &ASYNC_irq_type, handle_simple_irq);
	}
}

#endif /* CONFIG_IRQ_DOMAIN */

void ack_bad_irq(unsigned int irq)
{
	printk(KERN_ERR "ack_bad_irq: unexpected IRQ %d\n", irq);
	BUG();
}
