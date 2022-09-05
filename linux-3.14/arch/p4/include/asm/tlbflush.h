/*
 * Copyright (C) 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#ifndef __UM_TLBFLUSH_H
#define __UM_TLBFLUSH_H

#include <linux/mm.h>

/*
 * TLB flushing:
 *
 *  - flush_tlb_all() flushes all processes TLBs
 *  - flush_tlb_mm(mm) flushes the specified mm context TLB's
 *  - flush_tlb_page(vma, vmaddr) flushes one page
 *  - flush_tlb_range(vma, start, end) flushes a range of pages
 */

static inline void flush_tlb_all(void)
{
	/* not necessary */
}
extern void flush_tlb_mm(struct mm_struct *mm);
extern void flush_tlb_range(struct vm_area_struct *vma, unsigned long start, 
			    unsigned long end);
extern void flush_tlb_page(struct vm_area_struct *vma, unsigned long vmaddr);

static inline void flush_tlb_kernel_range(unsigned long start, unsigned long end)
{
	/* not necessary, we have update_mmu_kernel_range() instead */
}

/* arch/p4 specific: enforce update of kernel TLBs */
extern void update_mmu_kernel_range(unsigned long addr, unsigned long end);

/* arch/p4 specific: called in the pagefault path */
extern int flush_tlb_pagefault(struct mm_struct *mm, unsigned long vmaddr);


/*
 * kernel virtual memory handling for drivers using VM space
 * to communicate with external servers
 * use unmap_vm_area() or remove_vm_area() to flush
 */
extern int vm_protect(unsigned long addr, unsigned long size);

#endif
