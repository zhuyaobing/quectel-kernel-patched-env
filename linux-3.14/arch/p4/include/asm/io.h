#ifndef __P4_IO_H
#define __P4_IO_H

/* common generic code for all sub-arches */

#include <linux/compiler.h>

/*
 * Convert a physical pointer to a virtual kernel pointer for /dev/mem
 * access
 */
#define xlate_dev_mem_ptr(p)	__va(p)

/*
 * Convert a virtual cached pointer to an uncached pointer
 */
#define xlate_dev_kmem_ptr(p)	p

/**
 *	virt_to_phys	-	map virtual addresses to physical
 *	@address: address to remap
 *
 *	The returned physical address is the physical (CPU) mapping for
 *	the memory address given. It is only valid to use this function on
 *	addresses directly mapped or allocated via kmalloc.
 *
 *	This function does not give bus mappings for DMA transfers. In
 *	almost all conceivable cases a device driver should not be using
 *	this function
 */

extern unsigned long virt_to_phys(volatile void * address);

/**
 *	phys_to_virt	-	map physical address to virtual
 *	@address: address to remap
 *
 *	The returned virtual address is a current CPU mapping for
 *	the memory address given. It is only valid to use this function on
 *	addresses that have a kernel mapping
 *
 *	This function does not handle bus mappings for DMA transfers. In
 *	almost all conceivable cases a device driver should not be using
 *	this function
 */

extern void * phys_to_virt(unsigned long address);

/* try_phys_to_virt() works exactly the same, but doesn't panic if a resource isn't availabe */
extern void *try_phys_to_virt(unsigned long address);

/*
 * Change "struct page" to physical address, used only by the BIO layer.
 *
 * For BIO.  "will die".  Kill me when bio_to_phys() and bvec_to_phys() die.
 *
 * NOTE: This is kludgy and collides with P4 memory modell.
 *       Returned addresses are DMA addresses!
 */
#define page_to_phys(page)    ((dma_addr_t)virt_to_phys((void*)(page_to_pfn(page) << PAGE_SHIFT)))

extern void __iomem * __ioremap(unsigned long offset, unsigned long size, unsigned long flags);

/**
 * ioremap     -   map bus memory into CPU space
 * @offset:    bus address of the memory
 * @size:      size of the resource to map
 *
 * ioremap performs a platform specific sequence of operations to
 * make bus memory CPU accessible via the readb/readw/readl/writeb/
 * writew/writel functions and the other mmio helpers. The returned
 * address is not guaranteed to be usable directly as a virtual
 * address.
 */
/* ioremap() depends on the architecture, see include/arch-override/asm/io.h */

extern void __iomem * ioremap_nocache(unsigned long offset, unsigned long size);
extern void __iomem * ioremap_cache(unsigned long offset, unsigned long size);
extern void iounmap(volatile void __iomem *addr);


#include <arch-override/asm/io.h>

#endif
