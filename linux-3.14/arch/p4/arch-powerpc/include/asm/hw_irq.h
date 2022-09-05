#ifndef _UM_HW_IRQ_PPC_H
#define _UM_HW_IRQ_PPC_H

#include "arch/asm/reg.h"

#if defined(CONFIG_BOOKE)
#define SET_MSR_EE(x)	mtmsr(x)
#define local_irq_restore(flags)	__asm__ __volatile__("wrtee %0" : : "r" (flags) : "memory")
#else
#define SET_MSR_EE(x)	mtmsr(x)
#define local_irq_restore(flags)	mtmsr(flags)
#endif

static inline void local_irq_save_ptr(unsigned long *flags)
{
	unsigned long msr;
	msr = mfmsr();
	*flags = msr;
#ifdef CONFIG_BOOKE
	__asm__ __volatile__("wrteei 0": : :"memory");
#else
	SET_MSR_EE(msr & ~MSR_EE);
#endif
	__asm__ __volatile__("": : :"memory");
}

#define local_save_flags(flags)	((flags) = mfmsr())
#define local_irq_save(flags)	local_irq_save_ptr(&flags)

#endif