/*
 *  IO remapping
 * 
 *  Copyright (C) 2004-2010 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 *  Version 2004-01-27, azu: initial
 *  2006-06-12, azu@sysgo.de: Ported to 2.6.15
 *  2006-06-21, azu@sysgo.de: remap IO mappings (to set the cache bits)
 *    taken from arch/i386/mm/ioremap.c, (C) Copyright 1995 1996 Linus Torvalds
 *  2013-05-16, azu@sysgo.de: rework REMAP_IOMAPPINGS
 */

#include <linux/sched.h>
#include <linux/pagemap.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/memory.h>
#include <asm/pgtable.h>
#include <asm/iomem.h>
#include <asm/cacheflush.h>
#include <asm/tlbflush.h>
#include <asm/internals.h>

/*
 * Enable this if you don't want to remap IO mappings, ie take the already
 * mapped stuff instead. Cache bits must be set properly before!
 */
#define REMAP_IOMAPPINGS


#ifdef REMAP_IOMAPPINGS
static int ioremap_pte_range(pmd_t *pmd, unsigned long addr,
		unsigned long end, unsigned long phys_addr, unsigned long flags)
{
	pte_t *pte;
	unsigned long pfn;

	pfn = phys_addr >> PAGE_SHIFT;
	pte = pte_alloc_kernel(pmd, addr);
	if (!pte)
		return -ENOMEM;
	do {
		BUG_ON(!pte_none(*pte));
		set_pte(pte, pfn_pte(pfn, __pgprot(_PAGE_PRESENT | _PAGE_RW |
					_PAGE_DIRTY | _PAGE_ACCESSED | flags)));
		pfn++;
	} while (pte++, addr += PAGE_SIZE, addr != end);
	return 0;
}

static inline int ioremap_pmd_range(pud_t *pud, unsigned long addr,
		unsigned long end, unsigned long phys_addr, unsigned long flags)
{
	pmd_t *pmd;
	unsigned long next;

	phys_addr -= addr;
	pmd = pmd_alloc(&init_mm, pud, addr);
	if (!pmd)
		return -ENOMEM;
	do {
		next = pmd_addr_end(addr, end);
		if (ioremap_pte_range(pmd, addr, next, phys_addr + addr, flags))
			return -ENOMEM;
	} while (pmd++, addr = next, addr != end);
	return 0;
}

static inline int ioremap_pud_range(pgd_t *pgd, unsigned long addr,
		unsigned long end, unsigned long phys_addr, unsigned long flags)
{
	pud_t *pud;
	unsigned long next;

	phys_addr -= addr;
	pud = pud_alloc(&init_mm, pgd, addr);
	if (!pud)
		return -ENOMEM;
	do {
		next = pud_addr_end(addr, end);
		if (ioremap_pmd_range(pud, addr, next, phys_addr + addr, flags))
			return -ENOMEM;
	} while (pud++, addr = next, addr != end);
	return 0;
}

static int ioremap_page_range(unsigned long addr,
		unsigned long end, unsigned long phys_addr, unsigned long flags)
{
	unsigned long start = addr;
	pgd_t *pgd;
	unsigned long next;
	int err;

	BUG_ON(addr >= end);
	flush_cache_all();
	phys_addr -= addr;
	pgd = pgd_offset_k(addr);
	do {
		next = pgd_addr_end(addr, end);
		err = ioremap_pud_range(pgd, addr, next, phys_addr+addr, flags);
		if (err)
			break;
	} while (pgd++, addr = next, addr != end);

	update_mmu_kernel_range(start, end);
	flush_tlb_all();
	return err;
}
#endif /* REMAP_IOMAPPINGS */

/*
 * Remap an arbitrary physical address space into the kernel virtual
 * address space. Needed when the kernel wants to access high addresses
 * directly.
 *
 * NOTE! We need to allow non-page-aligned mappings too: we will obviously
 * have to convert them into an offset in a page-aligned mapping, but the
 * caller shouldn't need to know that small detail.
 *
 * 'flags' are normal _PAGE_* flags from pgtable.h specifying the cache mode.
 */
void *__ioremap(unsigned long phys_addr, unsigned long size, unsigned long flags)
{
	void __iomem * addr;
	unsigned long offset, last_addr;
	unsigned long kvirt;
	unsigned int cache_mode;
#ifdef REMAP_IOMAPPINGS
	struct vm_struct * area;
#endif

	/* Don't allow wraparound or zero size */
	last_addr = phys_addr + size - 1;
	if (!size || last_addr < phys_addr)
		return NULL;

	/* Mappings have to be page-aligned */
	offset = phys_addr & ~PAGE_MASK;
	phys_addr &= PAGE_MASK;
	size = PAGE_ALIGN(last_addr+1) - phys_addr;

	kvirt = __iomem_find_io(phys_addr, size, &cache_mode);

#ifdef CONFIG_DEBUG_IOREMAP
	printk(KERN_DEBUG "ioremap(phys:0x%08lx, size:0x%08lx, fl:0x%lx) = ", phys_addr, size, flags);
	if (kvirt != 0)
		printk("found at 0x%08lx\n", kvirt);
	else
		printk("NOT found\n");
#endif

	if (kvirt == 0)
		return NULL;

	addr = (void __iomem*)kvirt;

#ifdef REMAP_IOMAPPINGS
	/* no need to remap if caching mode is the same */
	if ((cache_mode & _PAGE_CACHE_MASK) == (flags & _PAGE_CACHE_MASK))
		goto kernel_mapping_ok;

	/* in case of an early boot mapping, take the source
	 * because get_vm_area() needs kmalloc() ...
	 */
	if (bootmem_retired) {
		area = get_vm_area(size, VM_IOREMAP);
		if (!area)
			return NULL;
		area->phys_addr = kvirt;
		addr = (void __iomem *) area->addr;
		if (ioremap_page_range((unsigned long) addr,
				(unsigned long) addr + size, kvirt, flags)) {
			vunmap((void __force *) addr);
			return NULL;
		}
	}

kernel_mapping_ok:
#endif

	return (void __iomem *) (offset + (char __iomem *)addr);
}
EXPORT_SYMBOL(__ioremap);

void __iomem * ioremap_cache(unsigned long offset, unsigned long size)
{
	return __ioremap(offset, size, _PAGE_CACHE_WB);
}
EXPORT_SYMBOL(ioremap_cache);

void __iomem * ioremap_nocache(unsigned long offset, unsigned long size)
{
	return __ioremap(offset, size, _PAGE_NOCACHE);
}
EXPORT_SYMBOL(ioremap_nocache);

void iounmap(volatile void __iomem *addr)
{
#ifdef REMAP_IOMAPPINGS
	struct vm_struct *p, *o;

	/* don't unmap direct mapped I/O sources */
	if ((unsigned long)addr < start_vm)
		return;

#ifdef CONFIG_PCI_VIRT
	/* don't unmap our PCI resources as well */
	if (p4_pci_is_bar_mapping((unsigned long)addr))
		return;
#endif

	addr = (volatile void __iomem *)(PAGE_MASK & (unsigned long __force)addr);

	/* Use the vm area unlocked, assuming the caller
	   ensures there isn't another iounmap for the same address
	   in parallel. Reuse of the virtual address is prevented by
	   leaving it in the global lists until we're done with it.
	   cpa takes care of the direct mappings. */
	p = find_vm_area((void __force *)addr);

	if (!p) {
		printk("iounmap: bad address %p\n", addr);
		dump_stack();
		return;
	}

	/* Finally remove it */
	o = remove_vm_area((void *)addr);
	BUG_ON(p != o || o == NULL);
	kfree(p);
#endif
}

EXPORT_SYMBOL(iounmap);

unsigned long virt_to_phys(volatile void *address)
{
	unsigned long kvirt = (unsigned long)address;

	if ((kvirt < KSEG_BASE) || (kvirt > (unsigned long)high_memory))
		panic("virt_to_phys(0x%08lx) out of bounds!\n", kvirt);

	return kvirt + kseg_phys_offset;
}

EXPORT_SYMBOL(virt_to_phys);

void *try_phys_to_virt(unsigned long address)
{
	unsigned long kvirt = address - kseg_phys_offset;

	/* ISA IO address space special handling */
#ifdef CONFIG_P4_X86
	if (address <= 0x100000)
		kvirt = iomem_find_io(address, 0);
#endif

	return (void*)kvirt;
}

void *phys_to_virt(unsigned long address)
{
	unsigned long kvirt = (unsigned long)try_phys_to_virt(address);

	if (kvirt == 0)
		panic("phys_to_virt(0x%08lx) not mapped to kernel\n", address);

	return (void*)kvirt;
}

EXPORT_SYMBOL(phys_to_virt);

int io_remap_pfn_range(struct vm_area_struct *vma, unsigned long vaddr,
	unsigned long pfn, unsigned long size, pgprot_t prot)
{
	unsigned long kvirt;

	/*
	 * Must translate phys addresses to kernel mappings for
	 * the virt-to-virt mapping to userspace
	 */
	kvirt = iomem_find_io(pfn << PAGE_SHIFT, size);

#ifdef CONFIG_DEBUG_IOREMAP
	printk(KERN_DEBUG "io_remap_pfn_range(user:0x%08lx, phys:0x%08lx, size:0x%08lx, prot:%#lx) = ", 
			vma->vm_start, pfn << PAGE_SHIFT, size, pgprot_val(prot));
	if (kvirt != 0)
		printk("found at 0x%08lx\n", kvirt);
	else
		printk("NOT found\n");
#endif

	/* resource does not exist */
	if (kvirt == 0)
		return -EINVAL;

	return remap_pfn_range(vma, vaddr, kvirt >> PAGE_SHIFT, size, prot);
}

EXPORT_SYMBOL(io_remap_pfn_range);

/* This function is called when a new pfn mapping is beeing established for
 * physical range indicated by pfn (e.g. by remap_pfn_range). We're trying
 * to detect wrong addresses and print a warning. For example if a framebuffer
 * driver uses the physical address for remapping to the user space without
 * converting physical to Linux virtual address space (see io_remap_pfn_range).
 */
int check_remap(struct vm_area_struct *vma, pgprot_t *prot,
		      unsigned long pfn, unsigned long size)
{
	/* pfn must be in the physical address space of the kernel memory. This
	 * check is not enough but a good starting point. We can also have
	 * overlapping memory areas which cannot be detected this way.
	 */

	/* NOTE: some drivers (e.g. virtual framebuffer) are using virtual
	 * memory for mapping a file provider into the linux address space.
	 * These mappings are okay and we don't have another solution for that.
	 */
#ifdef CONFIG_P4_CHECK_PFN_MAPPINGS
	if (pfn_valid(pfn) == 0) {
		unsigned int kvirt;

		printk(KERN_ERR "ERROR: New pfn mapping not in a physical address space 0x%X (available: 0 - 0x%X)",
		       (unsigned int)(pfn << PAGE_SHIFT),
		       (unsigned int)(max_mapnr << PAGE_SHIFT));

		/* check if we have a memory mapping */
		kvirt = iomem_find_io(pfn << PAGE_SHIFT, size);
		if (kvirt) {
			printk(KERN_ERR
			"       found possible memory mapping: 0x%X, size: %lu use io_remap_pfn_range in your driver\n",
				kvirt, size);
		}

		/* produce a call stack */
		BUG();

		return 1;
	}
#endif

	return 0;
}

int track_pfn_insert(struct vm_area_struct *vma, pgprot_t *prot,
		      unsigned long pfn)
{
	return check_remap(vma, prot, pfn, 0);
}

int track_pfn_remap(struct vm_area_struct *vma, pgprot_t *prot,
		    unsigned long pfn, unsigned long addr,
		    unsigned long size)
{
	return check_remap(vma, prot, pfn, size);
}

int track_pfn_copy(struct vm_area_struct *vma)
{
	return 0;
}

void untrack_pfn(struct vm_area_struct *vma, unsigned long pfn,
		 unsigned long size)
{
}
