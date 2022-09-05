/*
 * linux/include/asm-p4/pci-i386.h
 *
 * PCI handling for standard x86 PCs, taken from i386 variant
 *
 * 20060613, azu@sysgo.de: ported to p4
 */

#ifndef __i386_PCI_H
#define __i386_PCI_H

#ifdef __KERNEL__
#include <linux/mm.h>		/* for struct page */

struct pci_sysdata {
	int             domain;         /* PCI domain */
	int             node;           /* NUMA node */
#ifdef CONFIG_X86_64
	void            *iommu;         /* IOMMU private data */
#endif
};

extern int pci_routeirq;

/* scan a bus after allocating a pci_sysdata for it */
extern struct pci_bus *pci_scan_bus_on_node(int busno, struct pci_ops *ops,
											int node);
extern struct pci_bus *pci_scan_bus_with_sysdata(int busno);

/* Can be used to override the logic in pci_scan_bus for skipping
   already-configured bus numbers - to be used for buggy BIOSes
   or architectures with incomplete PCI setup by the loader */

#ifdef CONFIG_PCI
extern unsigned int pcibios_assign_all_busses(void);
#else
#define pcibios_assign_all_busses()	0
#endif
#define pcibios_scan_all_fns(a, b)	0

extern unsigned long pci_mem_start;
#define PCIBIOS_MIN_IO		0x1000
#define PCIBIOS_MIN_MEM		(pci_mem_start)

#define PCIBIOS_MIN_CARDBUS_IO	0x4000

void pcibios_config_init(void);
struct pci_bus * pcibios_scan_root(int bus);

void pcibios_set_master(struct pci_dev *dev);
void pcibios_penalize_isa_irq(int irq, int active);
struct irq_routing_table *pcibios_get_irq_routing_table(void);
int pcibios_set_irq_routing(struct pci_dev *dev, int pin, int irq);

/* Dynamic DMA mapping stuff.
 * i386 has everything mapped statically.
 */

#include <linux/types.h>
#include <linux/slab.h>
#include <asm/scatterlist.h>
#include <linux/string.h>
#include <asm/io.h>

struct pci_dev;

/* The PCI address space does equal the physical memory
 * address space.  The networking and block device layers use
 * this boolean for bounce buffer decisions.
 */
#define PCI_DMA_BUS_IS_PHYS	(1)

#define HAVE_PCI_MMAP
extern int pci_mmap_page_range(struct pci_dev *dev, struct vm_area_struct *vma,
			       enum pci_mmap_state mmap_state, int write_combine);

/*static inline void pcibios_add_platform_entries(struct pci_dev *dev)
{
}*/

#ifdef CONFIG_PCI
static inline void pci_dma_burst_advice(struct pci_dev *pdev,
					enum pci_dma_burst_strategy *strat,
					unsigned long *strategy_parameter)
{
	*strat = PCI_DMA_BURST_INFINITY;
	*strategy_parameter = ~0UL;
}
#endif

#endif /* __KERNEL__ */

/* implement the pci_ DMA API in terms of the generic device dma_ one */
#include <asm-generic/pci-dma-compat.h>

/* generic pci stuff */
#include <asm-generic/pci.h>

#endif /* __i386_PCI_H */
