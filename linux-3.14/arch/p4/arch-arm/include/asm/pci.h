/*
 * linux/include/asm-p4/arch-arm/pci.h
 *
 * PCI handling
 */

#ifndef __ARM_PCI_H
#define __ARM_PCI_H

/*
 * The PCI address space does equal the physical memory address space.
 * The networking and block device layers use this boolean for bounce
 * buffer decisions.
 */
#define PCI_DMA_BUS_IS_PHYS     (1)

#endif
