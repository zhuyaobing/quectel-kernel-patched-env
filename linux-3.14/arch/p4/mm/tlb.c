/*
 * Copyright (C) 2000, 2001, 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 *
 * adapted to p4/linux by Alex Zuepke <azu at sysgo dot de>
 * 20060531, azu@sysgo.de: adapted to linux 2.6.15:
 *           synced with arch/um/kernel/tlb.c and arch/um/kernel/skas/tlb.c
 */

#include <linux/stddef.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <asm/page.h>
#include <asm/pgalloc.h>
#include <asm/pgtable.h>
#include <asm/tlbflush.h>
#include <asm/mmu.h>
#include <asm/internals.h>
#include <asm/host_tasks.h>

#define ADD_ROUND(n, inc) (((n) + (inc)) & ~((inc) - 1))

/*
 * called by mm/vmalloc.c to map and unmap pages
 */
void update_mmu_kernel_range(unsigned long start, unsigned long end)
{
	struct mm_struct *mm;
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	unsigned long addr, last;

	mm = &init_mm;
	for (addr = start; addr < end;) {
		pgd = pgd_offset(mm, addr);
		if (!pgd_present(*pgd)) {
			last = ADD_ROUND(addr, PGDIR_SIZE);
			if (last > end)
				last = end;
			if (pmd_newpage(*pmd)) {
				uk_task_unmap(uk_task_id(), addr, last - addr);
			}
			addr = last;
			continue;
		}

		pud = pud_offset(pgd, addr);
		if (!pud_present(*pud)) {
			last = ADD_ROUND(addr, PUD_SIZE);
			if (last > end)
				last = end;
			if (pud_newpage(*pud)) {
				uk_task_unmap(uk_task_id(), addr, last - addr);
			}
			addr = last;
			continue;
		}

		pmd = pmd_offset(pud, addr);
		if (!pmd_present(*pmd)) {
			last = ADD_ROUND(addr, PMD_SIZE);
			if (last > end)
				last = end;
			if (pmd_newpage(*pmd)) {
				uk_task_unmap(uk_task_id(), addr, last - addr);
			}
			addr = last;
			continue;
		}

		pte = pte_offset_kernel(pmd, addr);
		if (!pte_present(*pte) || pte_newpage(*pte)) {
			if (pte_val(*pte) & _PAGE_PRESENT) {
				int ret;
				ret = uk_task_map(uk_task_id(), addr, pte_val(*pte) & PAGE_MASK,
				                  PAGE_SIZE, pte_val(*pte));
				if (ret > 0) {
					/* retry read-only mapping */
					unsigned long prot = pte_val(*pte) & ~_PAGE_RW;
					uk_task_map(uk_task_id(), addr, pte_val(*pte) & PAGE_MASK,
				                  PAGE_SIZE, prot);
				}
			} else {
				uk_task_unmap(uk_task_id(), addr, PAGE_SIZE);
			}
		}
		addr += PAGE_SIZE;
	}
}

static int fix_range(struct mm_struct *mm, unsigned long start_addr, unsigned long end_addr, int force)
{
	pgd_t *npgd;
	pud_t *npud;
	pmd_t *npmd;
	pte_t *npte;

	unsigned long addr, end;
	unsigned long prot;
	int err;
	
	/* not yet active address space? */
	if ((mm == NULL) || (mm->context.taskid == 0))
		return 0;

	for (addr = start_addr; addr < end_addr;) {
		npgd = pgd_offset(mm, addr);
		if (!pgd_present(*npgd)) {
			end = ADD_ROUND(addr, PGDIR_SIZE);
			if (end > end_addr)
				end = end_addr;
			if (force || pgd_newpage(*npgd)) {
				uk_task_unmap(mm->context.taskid, addr, end - addr);


				pgd_mkuptodate(*npgd);
			}
			addr = end;
			continue;
		}

		npud = pud_offset(npgd, addr);
		if (!pud_present(*npud)){
			end = ADD_ROUND(addr, PUD_SIZE);
			if (end > end_addr)
				end = end_addr;
			if (force || pud_newpage(*npud)) {
				uk_task_unmap(mm->context.taskid, addr, end - addr);


				pud_mkuptodate(*npud);
			}
			addr = end;
			continue;
		}

		npmd = pmd_offset(npud, addr);
		if (!pmd_present(*npmd)) {
			end = ADD_ROUND(addr, PMD_SIZE);
			if (end > end_addr)
				end = end_addr;
			if (force || pmd_newpage(*npmd)) {
				uk_task_unmap(mm->context.taskid, addr, end - addr);


				pmd_mkuptodate(*npmd);
			}
			addr = end;
			continue;
		}

		npte = pte_offset_kernel(npmd, addr);
		prot = pte_get_bits(*npte, _PAGE_ALL_BITS);
		if (!pte_young(*npte)) {
			prot &= ~_PAGE_PRESENT;
		} else if (!pte_dirty(*npte)) {
			prot &= ~_PAGE_RW;
		}
		if (force || pte_newpage(*npte)) {
			if (prot & _PAGE_PRESENT) {
				err = uk_task_map(mm->context.taskid, addr, pte_val(*npte) & PAGE_MASK, PAGE_SIZE, prot);
				if (unlikely(err))
					return err;
			} else {
				uk_task_unmap(mm->context.taskid, addr, PAGE_SIZE);
			}

		}

		*npte = pte_mkuptodate(*npte);
		addr += PAGE_SIZE;
	}
	return 0;
}

/*
 * Kernel pagefault handling
 * called by mm/memory.c and various ptep_* functions
 * always called with a valid vma->vm_mm!
 */
void flush_tlb_page(struct vm_area_struct *vma, unsigned long address)
{
	address &= PAGE_MASK;

	/*
	 * vma->vm_mm is never zero!
	 */
	BUG_ON(vma->vm_mm == NULL);
	fix_range(vma->vm_mm, address, address + PAGE_SIZE, 0);
}

/*
 * Kernel pagefault handling
 * called by arch/p4/kernel/traps.c::handle_page_fault() to establish the final mapping
 * always called with mm == current->mm
 *
 * MUST NOT FAIL!
 *
 * returns non-zero on errors
 */
int flush_tlb_pagefault(struct mm_struct *mm, unsigned long address)
{
	address &= PAGE_MASK;

	return fix_range(mm, address, address + PAGE_SIZE, 1);
}

/*
 * called by mm/mprotect::somewhere from sys_mprotect()
 */
void flush_tlb_range(struct vm_area_struct *vma, unsigned long start, unsigned long end)
{
	/*
	 * vma->vm_mm is never zero!
	 */
	BUG_ON(vma->vm_mm == NULL);
	fix_range(vma->vm_mm, start, end, 0);
}

/*
 * called by kernel/fork.c::dup_mmap() -> flush(oldmm)
 */
void flush_tlb_mm(struct mm_struct *mm)
{
	/* Don't bother flushing if this address space is about to be
	* destroyed.
	*/
	if(atomic_read(&mm->mm_users) == 0)
		return;
	fix_range(mm, 0, TASK_SIZE, 0);
}

void force_flush_all(void)
{
	fix_range(current->mm, 0, TASK_SIZE, 1);
}


/*
 * kernel virtual memory handling for drivers using VM space
 * to communicate with external servers
 * use unmap_vm_area() or remove_vm_area() to flush
 *
 * Implementation note: this creates just a dummy mapping to address 0
 */

static int fake_map_pte_range(pmd_t *pmd, unsigned long addr, unsigned long end)
{
	pte_t *pte;
	pte_t p;

	pte_set_val(p, 0, PAGE_KERNEL);

	pte = pte_alloc_kernel(pmd, addr);
	if (!pte)
		return -ENOMEM;
	do {
		WARN_ON(!pte_none(*pte));
		/* can't use set_pte(),
		 * 'cause we don't want _PAGE_NEWPAGE or _PAGE_NEWPROT set */
		pte_copy(*pte, p);
	} while (pte++, addr += PAGE_SIZE, addr != end);
	return 0;
}

static inline int fake_map_pmd_range(pud_t *pud, unsigned long addr, unsigned long end)
{
	pmd_t *pmd;
	unsigned long next;

	pmd = pmd_alloc(&init_mm, pud, addr);
	if (!pmd)
		return -ENOMEM;
	do {
		next = pmd_addr_end(addr, end);
		if (fake_map_pte_range(pmd, addr, next))
			return -ENOMEM;
	} while (pmd++, addr = next, addr != end);
	return 0;
}

static inline int fake_map_pud_range(pgd_t *pgd, unsigned long addr, unsigned long end)
{
	pud_t *pud;
	unsigned long next;

	pud = pud_alloc(&init_mm, pgd, addr);
	if (!pud)
		return -ENOMEM;
	do {
		next = pud_addr_end(addr, end);
		if (fake_map_pmd_range(pud, addr, next))
			return -ENOMEM;
	} while (pud++, addr = next, addr != end);
	return 0;
}

int vm_protect(unsigned long addr, unsigned long size)
{
	pgd_t *pgd;
	unsigned long next;
	unsigned long end = addr + size;
	int err;

	BUG_ON(addr >= end);
	pgd = pgd_offset_k(addr);
	do {
		next = pgd_addr_end(addr, end);
		err = fake_map_pud_range(pgd, addr, next);
		if (err)
			break;
	} while (pgd++, addr = next, addr != end);
	return err;
}
EXPORT_SYMBOL(vm_protect);

EXPORT_SYMBOL(get_vm_area);
EXPORT_SYMBOL(remove_vm_area);
