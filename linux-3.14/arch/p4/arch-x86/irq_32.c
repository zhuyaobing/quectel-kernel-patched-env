/*
 *	Needed for get_irq_regs and to be SMP safe
 *	Copied from arch/x86/irq_32.c and reduced to
 *	a minimum
 */

#include <linux/cpu.h>
#include <linux/percpu.h>
#include <linux/module.h>

DEFINE_PER_CPU(struct pt_regs *, irq_regs);
EXPORT_PER_CPU_SYMBOL(irq_regs);
