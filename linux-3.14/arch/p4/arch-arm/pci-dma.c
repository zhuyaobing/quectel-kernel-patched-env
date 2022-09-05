/* Copyright (c) 2016 OpenSynergy GmbH */
/*
 * taken from arch-mips
 * Dynamic DMA mapping support.
 *
 * On i386 there is no hardware dynamic DMA address translation,
 * so consistent alloc/free are merely page allocation/freeing.
 * The rest of the dynamic DMA mapping interface is implemented
 * in asm/pci.h.
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/pci.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/dma-mapping.h>
#include <linux/vmalloc.h>
#include <linux/spinlock.h>
#include <asm/tlbflush.h>
#include <asm/pgtable.h>
#include <asm/iomem.h>
#include <asm/cacheflush.h>
#include <asm/memory.h>

#ifdef CONSISTENT_DMA_SIZE
#define DEFAULT_CONSISTENT_DMA_SIZE CONSISTENT_DMA_SIZE
#else
#define DEFAULT_CONSISTENT_DMA_SIZE SZ_2M
#endif

unsigned long consistent_size = DEFAULT_CONSISTENT_DMA_SIZE;

void __init init_consistent_dma_size(unsigned long size)
{
	consistent_size = size;
}

#include "vmregion.h"

static struct arm_vmregion_head consistent_head;

static int __init consistent_init(void)
{
	spin_lock_init(&consistent_head.vm_lock);
	INIT_LIST_HEAD(&consistent_head.vm_list);
	consistent_head.vm_start = consistent_start;
	consistent_head.vm_end = consistent_start + consistent_size;
	return 0;
}

core_initcall(consistent_init);

static void *__dma_remap(unsigned long phys_addr, void* kvirt,
			 unsigned long size, gfp_t gfp, unsigned long flags)
{
	struct arm_vmregion *area;
	pgprot_t pgprot;

	pr_debug("%s: paddr:%08lx kvirt:%p size:%lu\n", __func__, phys_addr, kvirt, size);

	area = arm_vmregion_alloc(&consistent_head, PAGE_SIZE,
				  roundup(size, PAGE_SIZE), gfp);
	if (!area)
		return NULL;

	pgprot = __pgprot(_PAGE_PRESENT | _PAGE_RW
			| _PAGE_DIRTY | _PAGE_ACCESSED | flags);

	if (ioremap_page_range((unsigned long) area->vm_start,
		(unsigned long)area->vm_end, (unsigned long)kvirt, pgprot)) {
		arm_vmregion_free(&consistent_head, area);
		return NULL;
	}
	area->vm_paddr = phys_addr;
	update_mmu_kernel_range(area->vm_start, area->vm_end);

	return (void __iomem *)area->vm_start;
}

static void *dma_alloc_with_prot(struct device *dev, size_t size,
			   dma_addr_t *dma_handle, gfp_t gfp, unsigned long prot_val)
{
	void *ret;
	void *memory;

	dev_dbg(dev, "%s: size:%u gfp:%08x prot_vaql:%08lx\n", __func__,
	       size, gfp, prot_val);

	if (dma_alloc_from_coherent(dev, size, dma_handle, &memory))
		return memory;

	/* ignore region specifiers */
	gfp &= ~(__GFP_DMA | __GFP_HIGHMEM);

	if (dev == NULL || (dev->coherent_dma_mask < 0xffffffff))
		gfp |= GFP_DMA;

	ret = (void *)alloc_pages_exact(size, gfp);

	if (ret != NULL) {
		*dma_handle = virt_to_phys(ret);
		memset(ret, 0, size);
		uk_flush_dcache_range((unsigned long)ret, (unsigned long)size);

		ret = __dma_remap(*dma_handle, ret, size, gfp, prot_val);
	}
	return ret;
}

void *dma_alloc_coherent(struct device *dev, size_t size,
			   dma_addr_t *dma_handle, gfp_t gfp)
{
	return dma_alloc_with_prot( dev, size, dma_handle, gfp, _PAGE_NOCACHE);
}
EXPORT_SYMBOL(dma_alloc_coherent);

/* same as dma_alloc_coherent */
void *dma_alloc_noncacheable(struct device *dev, size_t size,
			   dma_addr_t *dma_handle, gfp_t gfp)
{
	return dma_alloc_with_prot( dev, size, dma_handle, gfp, _PAGE_NOCACHE);
}
EXPORT_SYMBOL(dma_alloc_noncacheable);

void *dma_alloc_writecombine(struct device *dev, size_t size,
			   dma_addr_t *dma_handle, gfp_t gfp)
{
	return dma_alloc_with_prot( dev, size, dma_handle, gfp, _PAGE_CACHE_WC);
}
EXPORT_SYMBOL(dma_alloc_writecombine);

void *dma_alloc_writethrough(struct device *dev, size_t size,
			   dma_addr_t *dma_handle, gfp_t gfp)
{
	return dma_alloc_with_prot( dev, size, dma_handle, gfp, _PAGE_CACHE_WT);
}

void dma_free_coherent(struct device *dev, size_t size,
			 void *vaddr, dma_addr_t dma_handle)
{
	struct arm_vmregion *area;
	void *page_addr;

	dev_dbg(dev, "%s: size:%u vaddr:%p dma_handle:%08x\n", __func__,
		 size, vaddr, dma_handle);

	if (dma_release_from_coherent(dev, get_order(size), vaddr))
		return;

	page_addr = (void*)phys_to_virt(dma_handle);
	area = arm_vmregion_find_remove(&consistent_head, (unsigned long)vaddr);
	if (area == NULL)
		return;

	unmap_kernel_range(area->vm_start, PAGE_ALIGN(size));
	arm_vmregion_free(&consistent_head, area);
	free_pages_exact(page_addr, size);
}
EXPORT_SYMBOL(dma_free_coherent); 

static int dma_mmap(struct device *dev, struct vm_area_struct *vma,
			void *cpu_addr, dma_addr_t dma_addr, size_t size)
{
	int ret = -ENXIO;
	unsigned long user_size, kern_size;
	struct arm_vmregion *c;

	user_size = (vma->vm_end - vma->vm_start) >> PAGE_SHIFT;

	c = arm_vmregion_find(&consistent_head, (unsigned long)cpu_addr);
	if (c) {
		unsigned long off = vma->vm_pgoff;

		kern_size = (c->vm_end - c->vm_start) >> PAGE_SHIFT;

		dev_dbg(dev, "%s: user_start:%08lx off:%08lx user_end:%08lx kern_start:%08lx kern_end:%08lx\n",
				__func__, vma->vm_start, off, vma->vm_end, c->vm_start, c->vm_end);

		if (off < kern_size &&
			user_size <= (kern_size - off)) {
			ret = io_remap_pfn_range(vma, vma->vm_start,
					      (c->vm_paddr >> PAGE_SHIFT) + off,
					      user_size << PAGE_SHIFT,
			 vma->vm_page_prot);
		}
	}

	return ret;
}

int dma_mmap_writecombine(struct device *dev, struct vm_area_struct *vma,
			void *cpu_addr, dma_addr_t dma_addr, size_t size)
{
	vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
	return dma_mmap(dev, vma, cpu_addr, dma_addr, size);
}
EXPORT_SYMBOL(dma_mmap_writecombine);

int dma_mmap_coherent(struct device *dev, struct vm_area_struct *vma,
			void *cpu_addr, dma_addr_t dma_addr, size_t size)
{
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	return dma_mmap(dev, vma, cpu_addr, dma_addr, size);
}
EXPORT_SYMBOL(dma_mmap_coherent);

/*
 * Make an area consistent for devices.
 * Note: Drivers should NOT use this function directly, as it will break
 * platforms with CONFIG_DMABOUNCE.
 * Use the driver DMA support - see dma-mapping.h (dma_sync_*)
 */
void dma_cache_maint(const void *start, size_t size, int direction)
{
#ifdef CONFIG_P4_CACHEMGR_DRIVER
	if (direction == DMA_FROM_DEVICE) {
		int err;

		err = uk_inval_dcache_range((unsigned long)start, size);
		BUG_ON(err);
	} else if (direction == DMA_TO_DEVICE)
#endif
		uk_flush_dcache_range((unsigned long)start, size);
}
EXPORT_SYMBOL(dma_cache_maint);

/*
 * - start - virtual start address of region
 * - end   - virtual end address of region
 */
void dmac_flush_range(const void *start, const void *end)
{
	flush_dcache_range_user((unsigned long)start, (unsigned long)end);
}
EXPORT_SYMBOL(dmac_flush_range);

/*
 *- start - kernel virtual start address
 *- size  - size of region
 *- dir   - DMA direction
 */
void dmac_map_area(const void *start, size_t size, int dir)
{
	if (dir == DMA_FROM_DEVICE) {
#ifdef CONFIG_P4_CACHEMGR_DRIVER
		/* works for kernel address space only */
		if (unlikely(!segment_eq(get_fs(), KERNEL_DS))) {
			pr_err("#### %s not supported\n", __func__);
			BUG();
			return;
		}
		uk_inval_dcache_range((unsigned long)start, size);
#else
		pr_err("#### %s not supported (check CONFIG_P4_CACHEMGR_DRIVER)\n",
			__func__);
#endif
	} else {
		flush_dcache_range_user((unsigned long)start, (unsigned long)start+size);
	}
}
EXPORT_SYMBOL(dmac_map_area);

/*
 *- start - kernel virtual start address
 *- size  - size of region
 *- dir   - DMA direction
 */
void dmac_unmap_area(const void *start, size_t size, int dir)
{
	if (dir != DMA_TO_DEVICE)
#ifdef CONFIG_P4_CACHEMGR_DRIVER
		if (unlikely(!segment_eq(get_fs(), KERNEL_DS))) {
			pr_err("#### %s not supported\n", __func__);
			BUG();
			return;
		}
		uk_inval_dcache_range((unsigned long)start, size);
#else
		pr_err("#### %s not supported (check CONFIG_P4_CACHEMGR_DRIVER)\n",
			__func__);
#endif
}
EXPORT_SYMBOL(dmac_unmap_area);
