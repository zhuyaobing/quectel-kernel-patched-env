#ifndef __P4_IRQ_H
#define __P4_IRQ_H

/* PikeOS supports up to 512 "native" hardware IRQs */
#define NUM_P4_IRQS     512

/* virtual IRQs (timer, events, worker threads) */
#define FIRST_ASYNC_IRQ NUM_P4_IRQS
#define NUM_ASYNC_IRQS  128

#define NR_IRQS         (NUM_ASYNC_IRQS + NUM_P4_IRQS)

static __inline__ int irq_canonicalize(int irq)
{
	return irq;
}

void arch_trigger_all_cpu_backtrace(void);
#define arch_trigger_all_cpu_backtrace arch_trigger_all_cpu_backtrace

#ifdef CONFIG_P4_ARM

/*
 * Use this value to indicate lack of interrupt
 * capability
 */
#ifndef NO_IRQ
#define NO_IRQ	((unsigned int)(-1))
#endif

#endif

#endif
