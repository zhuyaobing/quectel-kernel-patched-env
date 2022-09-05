/*
 *  linux/arch/p4/kernel/time.c
 *
 *  Copyright (C) 1991, 1992, 1995  Linus Torvalds
 *
 *  adapted from x86, ppc and arm-timers
 *  to P4/linux by Alex Zuepke <azu at sysgo dot de>
 *  implement clockevent by David Engraf <den@sysgo.com>
 *
 * The timer is implemented via an async thread per cpu. We support PERIODIC as
 * well as ONESHOT mode. CONFIG_NOHZ is also supported and reduces the number of
 * interrupts dramatically. A broadcast timer is no longer required because we
 * have full featured per cpu timers.
 */

#include <linux/clockchips.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <linux/stddef.h>
#include <linux/spinlock.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/profile.h>
#include <asm/kernthreads.h>
#include <asm/host_threads.h>
#include <asm/internals.h>
#include <asm/locking.h>
#include <asm/host_tasks.h>

#define NSECS_PER_JIFFY   (NSEC_PER_SEC/HZ)

#define MODE_ONE_SHOOT	0
#define MODE_PERIODIC	1

struct p4_timer {
	volatile unsigned long long nsec_to_wait;
	volatile int current_mode;
	struct uk_uevent irq_sleep;
	struct clock_event_device clockevent;
	struct irqaction timer_irq;
};

static DEFINE_PER_CPU(struct p4_timer, p4timer_per_cpu);
static char generic_timer_name[] = "timer";
static char local_timer_name[] = "local_timer";

/* NOTE: this is run in PikeOS context for every CPU! */
static void timer_thread(unsigned int irq, void *dev)
{
	unsigned long long next_time;
	int ret;
	struct p4_timer *timer = (struct p4_timer *)dev;

	next_time = uk_get_time_ns();
	while (1)
	{
		if (timer->current_mode == MODE_ONE_SHOOT) {
			if (timer->nsec_to_wait == 0) {
				/* this is reached only on mode change, when the timer is
				 * configured do_IRQ calls set_mode to specfiy the new timeout
				 * value before it returns
				 */
				sleep_in_irq_thread(&timer->irq_sleep);

				if (timer->current_mode == MODE_PERIODIC) {
					/* mode change DISABLED -> PERIODIC */
					next_time = uk_get_time_ns();
					continue;
				} else if (timer->nsec_to_wait == 0) {
					/* DISABLED -> ONE_SHOOT but no time specified yet -> wait
					* again
					*/
					continue;
				}
			}

			next_time = timer->nsec_to_wait;
			timer->nsec_to_wait = 0;
			/* new timeout accepted, clear pending event if it was not already
			 * consumpted by uk_uevent_wait_timeout
			 */
			uk_uevent_clear_pending(&timer->irq_sleep);
		} else {
			next_time += NSECS_PER_JIFFY;
		}

		ret = uk_uevent_wait_timeout_abs(&timer->irq_sleep, next_time);
		if (ret == 0) {
			continue;
		} else if (ret != ETIME) {
			/* This normally happens in PERIODIC mode when the
			 * PikeOS time partition scheduling is higher than
			 * CONFIG_HZ or the board timer is inaccurate. Don't
			 * syncronize because our jiffies value may run out when
			 * CONFIG_HIGH_RES_TIMER is not activated. When high
			 * resolution timer is activated, jiffies is always
			 * updated to the correct value with the help of
			 * uk_get_time_ns.
			 */
		}

		do_IRQ(irq);
	}
}

/*
 * Clock event interface:
 */
static void p4timer_set_mode(enum clock_event_mode mode,
			    struct clock_event_device *evt)
{
	struct p4_timer *timer = container_of(evt, struct p4_timer, clockevent);

	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
		timer->current_mode = MODE_PERIODIC;
		wake_irq_thread(&timer->irq_sleep);
		break;

	case CLOCK_EVT_MODE_SHUTDOWN:
	case CLOCK_EVT_MODE_UNUSED:
	case CLOCK_EVT_MODE_ONESHOT:
		timer->current_mode = MODE_ONE_SHOOT;
		timer->nsec_to_wait = 0;
		wake_irq_thread(&timer->irq_sleep);
		break;
 
	case CLOCK_EVT_MODE_RESUME:
		break;
 	}
}


/* This function is called by timer_thread -> do_IRQ thus the timer_thread is
 * running and receives the new timeout when do_IRQ is finished. In NO_HZ mode
 * also other interrupts may trigger next_event, thus we need to wakeup
 * timer_thread when it is sleeping
 */
static int p4timer_next_event(unsigned long delta,
			     struct clock_event_device *evt)
{
	struct p4_timer *timer = container_of(evt, struct p4_timer, clockevent);

	/* delta is already in nanoseconds, so no convervsion is needed */
	timer->nsec_to_wait = (uk_get_time_ns() + delta);
	wake_irq_thread(&timer->irq_sleep);

	return 0;
}

static irqreturn_t timer_interrupt(int irq, void *dev)
{
	struct p4_timer *timer = (struct p4_timer *)dev;

	(*timer->clockevent.event_handler)(&timer->clockevent);

	return IRQ_HANDLED;
}

/*
 * Clock source interface:
 */
static cycle_t p4timer_read(struct clocksource *cs)
{
	return div_u64(uk_get_time_ns(), 1000);
}

/*
 * We are using nanoseconds resolution
 */
static struct clocksource p4timer_clocksource = {
	.name		= "p4_timer",
	.rating		= 300,
	.read		= p4timer_read,
	.mask		= CLOCKSOURCE_MASK(64),
	/* WARNING: Do not use 1 for mult because timekeeper may decrease it
	 *          to 0 to reduce the error value (see timekeeping_adjust)
	 */
	.mult		= 1000,
	.shift		= 0,
	.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
};

/*
 * Setup per cpu timer
 */
void local_timer_setup(unsigned int cpu)
{
	int irq, ret;
	struct p4_timer *timer;
	char timer_name[32];
	
	timer = &per_cpu(p4timer_per_cpu, cpu);

	/* setup the interrupt for the ticker */
	irq = get_free_async_irq();
	BUG_ON(!irq);

	uk_uevent_init(&timer->irq_sleep);

	timer->timer_irq.handler = timer_interrupt,
	timer->timer_irq.flags = IRQF_TIMER | IRQF_DISABLED,
	timer->timer_irq.name = generic_timer_name;
	timer->timer_irq.dev_id = timer;

	ret = setup_irq(irq, &timer->timer_irq);
	if (ret) {
		/* System will not correctly start without per CPU timer */
		panic("time_init: setting up timer IRQ %d for CPU#%d failed: %d\n",
			 irq, cpu, ret);
	}

	sprintf(timer_name, "timer CPU#%d", cpu);
	/* startup the timer thread and wait in ONE_SHOOT mode */
	if (enable_irq_thread(irq, timer_name, timer_thread, timer))
		panic("time_init: could not start timer thread");

	bind_irq_thread(irq, cpu);

	/* we're using ns as clock rate with p4 resolution as minimal value, thus
	 * we don't need to convert between a clock rate and ns
	 */
	timer->clockevent.name = local_timer_name;
	timer->clockevent.features = CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT,
	timer->clockevent.set_mode = p4timer_set_mode,
	timer->clockevent.set_next_event = p4timer_next_event,
	timer->clockevent.rating = 250,
	timer->clockevent.shift = 0;
	timer->clockevent.mult = 1;
	timer->clockevent.max_delta_ns = NSEC_PER_SEC;
	timer->clockevent.min_delta_ns = uk_task_kinfo_timeout_resolution();
	timer->clockevent.irq = irq;
	timer->clockevent.cpumask = cpumask_of(cpu);

	clockevents_register_device(&timer->clockevent);

	pr_info("p4_get_time: %lld ns\n",uk_get_time_ns());

	pr_info("Timer IRQ %d for CPU#%d started\n", irq, cpu);

}

void __init time_init(void)
{
	clocksource_register(&p4timer_clocksource);
	
	if (uk_task_kinfo_timeout_resolution() > NSECS_PER_JIFFY) {
		printk(KERN_WARNING "\n");
		printk(KERN_WARNING "WARNING: P4 board timer resolution inaccurate, requested %ld ns, is %llu ns\n",
			   NSECS_PER_JIFFY, uk_task_kinfo_timeout_resolution());
		printk(KERN_WARNING "         Please decrease UK_NS_PER_TICK/UK_NS_PER_TP_TICK for a more accurate timer\n");
		printk(KERN_WARNING "\n");
#ifndef CONFIG_HIGH_RES_TIMERS
		/* This is a very bad combination, without the high resolution 
		 * interface it's not possible to get correct timings because
		 * it linux fully relies on the ticker which is not synchronized
		 * with the PikeOS ticker.
		 */
#endif
	}
#ifndef CONFIG_HIGH_RES_TIMERS
	printk(KERN_WARNING "\n");
	printk(KERN_WARNING "WARNING: CONFIG_HIGH_RES_TIMERS is not activated, this may lead to inacurate timings.\n");
	printk(KERN_WARNING "\n");
#endif

	/* initialize CPU#0 timer */
	local_timer_setup(0);

#ifdef CONFIG_BSP
	bsp_time_init();
#endif
}
