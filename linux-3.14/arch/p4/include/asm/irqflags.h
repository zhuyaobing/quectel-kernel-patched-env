/*
 *  PikeOS interrupt enable / disable abstraction layer
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 * 
 *  Copyright (C) 2011-2014 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#ifndef __P4_IRQFLAGS_H__
#define __P4_IRQFLAGS_H__

#include <linux/types.h>

/*
 * flag state:
 * - 0: irq enabled
 * - 1: irq disabled
 */
unsigned long arch_local_save_flags(void);
unsigned long arch_local_irq_save(void);
void arch_local_irq_restore(unsigned long flags);
void arch_local_irq_enable(void);

static inline void arch_local_irq_disable(void)
{
	arch_local_irq_save();
}

static inline bool arch_irqs_disabled_flags(unsigned long flags)
{
	return flags != 0;
}

static inline int arch_irqs_disabled(void)
{
	unsigned long flags = arch_local_save_flags();

	return arch_irqs_disabled_flags(flags);
}

/* special hooks for do_IRQ to serialize further interrupts */
int arch_local_irq_lock(void);
void arch_local_irq_unlock(void);

/* hooks for the THREAD_KPAGER thread to steal a CPUs lock */
unsigned int arch_local_irq_steal(unsigned int faulting_uid);
void arch_local_irq_unsteal(unsigned int old_owner);
void arch_local_unlock_owned_mutex(int cpu_idx, unsigned int mutex_lock_uid);

#endif
