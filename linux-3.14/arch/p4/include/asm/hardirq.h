#ifndef __P4_HARDIRQ_H
#define __P4_HARDIRQ_H

#include <linux/threads.h>
#include <linux/irq.h>

typedef struct {
	unsigned int __softirq_pending;
} ____cacheline_aligned irq_cpustat_t;

#include <linux/irq_cpustat.h>

extern void ack_bad_irq(unsigned int irq);

#endif
