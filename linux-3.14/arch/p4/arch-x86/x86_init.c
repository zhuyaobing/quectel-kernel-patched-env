/*
 * Copyright (C) 2009 Thomas Gleixner <tglx@linutronix.de>
 *
 *  For licencing details see kernel-base/COPYING
 *
 *  derived from arch-x86/kernel and used by x86 pci layer
 */
#include <linux/init.h>
#include <linux/ioport.h>

#include <asm/bios_ebda.h>
#include <asm/paravirt.h>
#include <asm/pci_x86.h>
#include <asm/mpspec.h>
#include <asm/setup.h>
#include <asm/e820.h>
#include <asm/time.h>
#include <asm/irq.h>
#include <asm/pat.h>
#include <asm/iommu.h>

#include <asm/x86_init.h>

void __cpuinit x86_init_noop(void) { }
void __init x86_init_uint_noop(unsigned int unused) { }
void __init x86_init_pgd_noop(pgd_t *unused) { }
int __init iommu_init_noop(void) { return 0; }
void iommu_shutdown_noop(void) { }

/* This is set by the PCI code if either type 1 or type 2 PCI is detected */
_Bool port_cf9_safe = false;

/*
 * The platform setup functions are preset with the default functions
 * for standard PC hardware.
 */
struct x86_init_ops x86_init __initdata = {

	.iommu = {
		.iommu_init		= iommu_init_noop,
	},

	.pci = {
		.init			= x86_default_pci_init,
		.init_irq		= x86_default_pci_init_irq,
		.fixup_irqs		= x86_default_pci_fixup_irqs,
	},
};
