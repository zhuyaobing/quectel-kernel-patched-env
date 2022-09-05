/* derived from arch-x86/include/asm and reduced to the functions called by
 * the x86 pci layer
 */
#ifndef _ASM_P4_X86_PLATFORM_H
#define _ASM_P4_X86_PLATFORM_H

/**
 * struct x86_init_iommu - platform specific iommu setup
 * @iommu_init:			platform specific iommu setup
 */
struct x86_init_iommu {
	int (*iommu_init)(void);
};

/**
 * struct x86_init_pci - platform specific pci init functions
 * @arch_init:			platform specific pci arch init call
 * @init:			platform specific pci subsystem init
 * @init_irq:			platform specific pci irq init
 * @fixup_irqs:			platform specific pci irq fixup
 */
struct x86_init_pci {
	int (*arch_init)(void);
	int (*init)(void);
	void (*init_irq)(void);
	void (*fixup_irqs)(void);
};

/**
 * struct x86_init_ops - functions for platform specific setup
 *
 */
struct x86_init_ops {
	struct x86_init_iommu		iommu;
	struct x86_init_pci		pci;
};

extern struct x86_init_ops x86_init;

#endif
