/*
 * linux/arch/p4/include/asm/pgtable.h
 *
 * adopted from linux/include/asm-i386/page.h and linux/include/asm-ppc/page.h
 * keep in sync with arch/um/include/asm/pgtable.h
 *
 * 2004-01-22, azu: rewritten to fit virt-virt mapping
 * 2012-01-10, azu: cleaned up for 2.6.39
 *
 */
/*
 * Copyright (C) 2000, 2001, 2002 Jeff Dike (jdike@karaya.com)
 * Derived from include/asm-i386/pgtable.h
 * Licensed under the GPL
 */
#ifndef __P4_PGTABLE_H
#define __P4_PGTABLE_H

#include <asm/page.h>

struct vm_area_struct;

/*
 * match mappings of Linux page table flags to PikeOS native format:
 * #define P4_M_UPDATE         (1U << 0)    - (implicit)
 * #define P4_M_READ           (1U << 1)    present (always set)
 * #define P4_M_WRITE          (1U << 2)    writable
 * #define P4_M_EXEC           (1U << 3)    executable
 * #define P4_M_C_ENABLE       (1U << 4)    cache
 * #define P4_M_C_WRITEBACK    (1U << 5)    cache
 * #define P4_M_C_PREFETCH     (1U << 6)    cache
 * #define P4_M_C_COHERENCY    (1U << 7)    cache
 * #define P4_M_C_PLATFORM1    (1U << 8)    - (MIPS implicit)
 * #define P4_M_C_PLATFORM2    (1U << 9)    -
 * #define P4_M_C_UPDATE       (1U << 10)   - (implicit)
 * #define P4_M_REPLACE        (1U << 11)   - (implicit)
 *
 * Implicit bits are always set when calling p4_mem_map().
 */
#define _PAGE_PRESENT	0x001
/*	not uned	0x002 */
#define _PAGE_RW	0x004
#define _PAGE_EXEC	0x008
#define _PAGE_CACHE_E	0x010
#define _PAGE_CACHE_W	0x020
#define _PAGE_CACHE_P	0x040
#define _PAGE_CACHE_C	0x080
/*	_PAGE_PROTNONE	0x100 */
#define _PAGE_NEWPAGE	0x200
#define _PAGE_ACCESSED	0x400
#define _PAGE_DIRTY	0x800

#define _PAGE_ALL_BITS	0xfff

/* If _PAGE_PRESENT is clear, we use these: */
#define _PAGE_FILE	0x400	/* nonlinear file mapping, saved PTE; unset:swap */
#define _PAGE_PROTNONE	0x100	/* if the user mapped it with PROT_NONE;
				   pte_present gives true */

/* transfrom Linux bits to PikeOS bits */
#define _PAGE_P4_MASK	0x0fc
#define _PAGE_P4_SET	0xc03

/*
 * cache modes: we match PikeOS cache modes to Linux
 *
 * EWPC  cache mode
 * ==========================
 * 1111  cached write-back
 * 1011  cached write-through
 * 0111  uncached write combining
 * 0001  uncached strongly ordered
 *
 * EWPC refers to the P4_M_C bits ENABLED, WRITEBACK, PREFETCH and COHERENCY.
 */
#define _PAGE_CACHE_MASK        (_PAGE_CACHE_E|_PAGE_CACHE_W|_PAGE_CACHE_P|_PAGE_CACHE_C)
#define _PAGE_NOCACHE           (            0|            0|            0|_PAGE_CACHE_C)
#define _PAGE_NOCACHE_NONSHARED (            0|            0|            0|            0)
#define _PAGE_CACHE_WC          (            0|_PAGE_CACHE_W|_PAGE_CACHE_P|_PAGE_CACHE_C)
#define _PAGE_CACHE_WT          (_PAGE_CACHE_E|            0|_PAGE_CACHE_P|_PAGE_CACHE_C)
#define _PAGE_CACHE_WB          (_PAGE_CACHE_E|_PAGE_CACHE_W|_PAGE_CACHE_P|_PAGE_CACHE_C)

/* compat names */
#define _PAGE_IOMAP             0 /* only used by Xen */
#define _PAGE_CACHE_UC_MINUS    (_PAGE_NOCACHE)

#ifdef CONFIG_3_LEVEL_PGTABLES
#include <asm/cmpxchg.h>

#define _PAGE_USER 0	/* not supported */
#define pmd_alloc_one pmd_alloc_one_unused	/* use generic inline version */
#include <../../../um/include/asm/pgtable-3level.h>
#undef pmd_alloc_one
#else

/* PTE bits differ need to check the impact */
/* #include <../../../um/include/asm/pgtable-2level.h> */

#include <asm-generic/pgtable-nopmd.h>


/* PGDIR_SHIFT determines what a third-level page table entry can map */
#define PGDIR_SHIFT	22
#define PGDIR_SIZE	(1UL << PGDIR_SHIFT)
#define PGDIR_MASK	(~(PGDIR_SIZE-1))

/*
 * entries per page directory level: the i386 is two-level, so
 * we don't really have any PMD directory physically.
 */
#define PTRS_PER_PTE	1024
#define USER_PTRS_PER_PGD ((TASK_SIZE + (PGDIR_SIZE - 1)) / PGDIR_SIZE)
#define PTRS_PER_PGD	1024

#define FIRST_USER_ADDRESS	0

#define pte_ERROR(e) \
        printk("%s:%d: bad pte %p(%08lx).\n", __FILE__, __LINE__, &(e), \
	       pte_val(e))
#define pgd_ERROR(e) \
        printk("%s:%d: bad pgd %p(%08lx).\n", __FILE__, __LINE__, &(e), \
	       pgd_val(e))

static inline int pgd_newpage(pgd_t pgd)	{ return 0; }
static inline void pgd_mkuptodate(pgd_t pgd)	{ }

#define set_pmd(pmdptr, pmdval) (*(pmdptr) = (pmdval))

#define pte_pfn(x) phys_to_pfn(pte_val(x))
#define pfn_pte(pfn, prot) __pte(pfn_to_phys(pfn) | pgprot_val(prot))
#define pfn_pmd(pfn, prot) __pmd(pfn_to_phys(pfn) | pgprot_val(prot))

/*
 * Bits 0 through 3 are taken
 */
#define PTE_FILE_MAX_BITS	28

#define pte_to_pgoff(pte) (pte_val(pte) >> 4)

#define pgoff_to_pte(off) ((pte_t) { ((off) << 4) + _PAGE_FILE })

#endif /* CONFIG_3_LEVEL_PGTABLES */

#define pmd_page_kernel(pmd) \
	((unsigned long) __va(pmd_val(pmd) & PAGE_MASK))

extern pgd_t swapper_pg_dir[PTRS_PER_PGD];

/* zero page used for uninitialized stuff */
extern unsigned long *empty_zero_page;

#define pgprot_noncached(prot) __pgprot((pgprot_val(prot) & ~_PAGE_CACHE_MASK) | _PAGE_NOCACHE)
#define pgprot_writethrough(prot) __pgprot((pgprot_val(prot) & ~_PAGE_CACHE_MASK) | _PAGE_CACHE_WT)
#define pgprot_writecombine(prot) __pgprot((pgprot_val(prot) & ~_PAGE_CACHE_MASK) | _PAGE_CACHE_WC)

#define pgtable_cache_init() do ; while (0)

/*
 * pgd entries used up by user/kernel:
 */

#define USER_PGD_PTRS (TASK_SIZE >> PGDIR_SHIFT)
#define KERNEL_PGD_PTRS (PTRS_PER_PGD-USER_PGD_PTRS)

#ifndef __ASSEMBLY__

#define _PAGE_TABLE	(_PAGE_PRESENT | _PAGE_RW | _PAGE_ACCESSED | _PAGE_DIRTY)
#define _KERNPG_TABLE	_PAGE_TABLE
#define _PAGE_CHG_MASK	(PAGE_MASK | _PAGE_ACCESSED | _PAGE_DIRTY | _PAGE_EXEC)

#define PAGE_NONE	__pgprot(_PAGE_PROTNONE | _PAGE_ACCESSED)
#define PAGE_SHARED	__pgprot(_PAGE_CACHE_WB | _PAGE_PRESENT | _PAGE_RW | _PAGE_ACCESSED)
#define PAGE_SHARED_EXEC	__pgprot(_PAGE_CACHE_WB | _PAGE_PRESENT | _PAGE_RW | _PAGE_ACCESSED | _PAGE_EXEC)
#define PAGE_COPY	__pgprot(_PAGE_CACHE_WB | _PAGE_PRESENT | _PAGE_ACCESSED)
#define PAGE_COPY_EXEC	__pgprot(_PAGE_CACHE_WB | _PAGE_PRESENT | _PAGE_ACCESSED | _PAGE_EXEC)
#define PAGE_READONLY	__pgprot(_PAGE_CACHE_WB | _PAGE_PRESENT | _PAGE_ACCESSED)
#define PAGE_READONLY_EXEC	__pgprot(_PAGE_CACHE_WB | _PAGE_PRESENT | _PAGE_ACCESSED | _PAGE_EXEC)

#if defined(CONFIG_P4_PPC)
/* these archs use standard vmalloc() memory for modules, enable exec access */
#define PAGE_KERNEL	__pgprot(_PAGE_CACHE_WB | _PAGE_PRESENT | _PAGE_RW | _PAGE_DIRTY | _PAGE_ACCESSED | _PAGE_EXEC)
#else
#define PAGE_KERNEL	__pgprot(_PAGE_CACHE_WB | _PAGE_PRESENT | _PAGE_RW | _PAGE_DIRTY | _PAGE_ACCESSED)
#endif
#define PAGE_KERNEL_EXEC	__pgprot(_PAGE_CACHE_WB | _PAGE_PRESENT | _PAGE_RW | _PAGE_DIRTY | _PAGE_ACCESSED | _PAGE_EXEC)

/*
 * The table below defines the page protection levels that we insert into our
 * Linux page table version.  These get translated into the best that the
 * architecture can perform.  Note that not all architecture supports exceution
 * protection:
 *  1) If we can do execute protection, then read is implied
 *  2) write implies read permissions
 */
#define __P000	PAGE_NONE
#define __P001	PAGE_READONLY
#define __P010	PAGE_COPY
#define __P011	PAGE_COPY
#define __P100	PAGE_READONLY_EXEC
#define __P101	PAGE_READONLY_EXEC
#define __P110	PAGE_COPY_EXEC
#define __P111	PAGE_COPY_EXEC

#define __S000	PAGE_NONE
#define __S001	PAGE_READONLY
#define __S010	PAGE_SHARED
#define __S011	PAGE_SHARED
#define __S100	PAGE_READONLY_EXEC
#define __S101	PAGE_READONLY_EXEC
#define __S110	PAGE_SHARED_EXEC
#define __S111	PAGE_SHARED_EXEC


/*
 * ZERO_PAGE is a global shared page that is always zero: used
 * for zero-mapped memory areas etc..
 */
#define ZERO_PAGE(vaddr) virt_to_page(empty_zero_page)

#define pte_clear(mm,addr,xp) pte_set_val(*(xp), (phys_t) 0, __pgprot(_PAGE_NEWPAGE))

#define pmd_none(x)	(!((unsigned long)pmd_val(x) & ~_PAGE_NEWPAGE))
#define pmd_bad(x)	((pmd_val(x) & (~PAGE_MASK)) != _PAGE_TABLE)
#define pmd_present(x)	(pmd_val(x) & _PAGE_PRESENT)
#define pmd_clear(xp)	do { pmd_val(*(xp)) = _PAGE_NEWPAGE; } while (0)

#define pmd_newpage(x)  (pmd_val(x) & _PAGE_NEWPAGE)
#define pmd_mkuptodate(x) (pmd_val(x) &= ~_PAGE_NEWPAGE)

#define pud_newpage(x)  (pud_val(x) & _PAGE_NEWPAGE)
#define pud_mkuptodate(x) (pud_val(x) &= ~_PAGE_NEWPAGE)

#define pages_to_mb(x) ((x) >> (20-PAGE_SHIFT))

#define pmd_page(pmd) pfn_to_page(phys_to_pfn(pmd_val(pmd) & PAGE_MASK))

#define pte_page(x) pfn_to_page(pte_pfn(x))

#define pte_present(x)	pte_get_bits(x, (_PAGE_PRESENT | _PAGE_PROTNONE))

/*
 * =================================
 * Flags checking section.
 * =================================
 */

static inline int pte_none(pte_t pte)
{
	return pte_is_zero(pte);
}

static inline int pte_exec(pte_t pte){
	return((pte_get_bits(pte, _PAGE_EXEC)) &&
	       !(pte_get_bits(pte, _PAGE_PROTNONE)));
}

static inline int pte_write(pte_t pte)
{
	return((pte_get_bits(pte, _PAGE_RW)) &&
	       !(pte_get_bits(pte, _PAGE_PROTNONE)));
}

/*
 * The following only works if pte_present() is not true.
 */
static inline int pte_file(pte_t pte)
{
	return pte_get_bits(pte, _PAGE_FILE);
}

static inline int pte_special(pte_t pte)
{
	return 0;
}

static inline int pte_dirty(pte_t pte)
{
	return pte_get_bits(pte, _PAGE_DIRTY);
}

static inline int pte_young(pte_t pte)
{
	return pte_get_bits(pte, _PAGE_ACCESSED);
}

static inline int pte_newpage(pte_t pte)
{
	return pte_get_bits(pte, _PAGE_NEWPAGE);
}

/*
 * =================================
 * Flags setting section.
 * =================================
 */

static inline pte_t pte_mknewpage(pte_t pte)
{
	pte_set_bits(pte, _PAGE_NEWPAGE);
	return(pte);
}

static inline pte_t pte_mkclean(pte_t pte)
{
	pte_clear_bits(pte, _PAGE_DIRTY);
	return(pte);
}

static inline pte_t pte_mkold(pte_t pte)
{
	pte_clear_bits(pte, _PAGE_ACCESSED);
	return(pte);
}

static inline pte_t pte_wrprotect(pte_t pte)
{
	pte_clear_bits(pte, _PAGE_RW);
	return(pte_mknewpage(pte));
}

static inline pte_t pte_mkdirty(pte_t pte)
{
	pte_set_bits(pte, _PAGE_DIRTY);
	return(pte);
}

static inline pte_t pte_mkyoung(pte_t pte)
{
	pte_set_bits(pte, _PAGE_ACCESSED);
	return(pte);
}

static inline pte_t pte_mkwrite(pte_t pte)
{
	pte_set_bits(pte, _PAGE_RW);
	return(pte_mknewpage(pte));
}

static inline pte_t pte_mkuptodate(pte_t pte)
{
	pte_clear_bits(pte, _PAGE_NEWPAGE);
	return(pte);
}

static inline pte_t pte_mkspecial(pte_t pte)
{
	return(pte);
}

static inline void set_pte(pte_t *pteptr, pte_t pteval)
{
	pte_copy(*pteptr, pteval);

	/* If it's a swap entry, it needs to be marked _PAGE_NEWPAGE so
	 * fix_range knows to unmap it.
	 */
	*pteptr = pte_mknewpage(*pteptr);
}
#define set_pte_at(mm,addr,ptep,pteval) set_pte(ptep,pteval)

/*
 * Conversion functions: convert a page and protection to a page entry,
 * and a page entry and page directory to the page they refer to.
 */
#define mk_pte(page, pgprot) \
	({ pte_t pte;					\
							\
	pte_set_val(pte, pfn_to_phys(page_to_pfn(page)), (pgprot));	\
	if (pte_present(pte))				\
		pte_mknewpage(pte);	\
	pte;})

static inline pte_t pte_modify(pte_t pte, pgprot_t newprot)
{
	pte_set_val(pte, (pte_val(pte) & _PAGE_CHG_MASK), newprot);
	return pte;
}

/*
 * the pgd page can be thought of an array like this: pgd_t[PTRS_PER_PGD]
 *
 * this macro returns the index of the entry in the pgd page which would
 * control the given virtual address
 */
#define pgd_index(address) (((address) >> PGDIR_SHIFT) & (PTRS_PER_PGD-1))

/*
 * pgd_offset() returns a (pgd_t *)
 * pgd_index() is used get the offset into the pgd page's array of pgd_t's;
 */
#define pgd_offset(mm, address) ((mm)->pgd+pgd_index(address))

/*
 * a shortcut which implies the use of the kernel's pgd, instead
 * of a process's
 */
#define pgd_offset_k(address) pgd_offset(&init_mm, address)

/*
 * the pmd page can be thought of an array like this: pmd_t[PTRS_PER_PMD]
 *
 * this macro returns the index of the entry in the pmd page which would
 * control the given virtual address
 */
#define pmd_index(address) (((address) >> PMD_SHIFT) & (PTRS_PER_PMD-1))

/*
 * the pte page can be thought of an array like this: pte_t[PTRS_PER_PTE]
 *
 * this macro returns the index of the entry in the pte page which would
 * control the given virtual address
 */
#define pte_index(address) (((address) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))
#define pte_offset_kernel(dir, address) \
	((pte_t *) pmd_page_kernel(*(dir)) +  pte_index(address))
#define pte_offset_map(dir, address) \
	((pte_t *)page_address(pmd_page(*(dir))) + pte_index(address))
#define pte_offset_map_nested(dir, address) pte_offset_map(dir, address)
#define pte_unmap(pte) do { } while (0)
#define pte_unmap_nested(pte) do { } while (0)

#if defined(CONFIG_P4_PPC) || defined(CONFIG_P4_ARM)
/* PPC need this for cache handling magic */
extern void update_mmu_cache(struct vm_area_struct *, unsigned long, pte_t*);
#else
#define update_mmu_cache(vma,address,pte) do ; while (0)
#endif

/* Encode and de-code a swap entry */
#define __swp_type(x)			(((x).val >> 4) & 0x3f)
#define __swp_offset(x)			((x).val >> 11)

#define __swp_entry(type, offset) \
	((swp_entry_t) { ((type) << 4) | ((offset) << 11) })
#define __pte_to_swp_entry(pte) \
	((swp_entry_t) { pte_val(pte_mkuptodate(pte)) })
#define __swp_entry_to_pte(x)		((pte_t) { (x).val })

#define kern_addr_valid(addr) (1)

/* Indicate that P4Linux has its own track pfn vma functions see mm/ioremap.c */
#define __HAVE_PFNMAP_TRACKING

/*
 * Handle IO Mappings to userspace
 */
extern int io_remap_pfn_range(struct vm_area_struct *vma, unsigned long vaddr,
	unsigned long pfn, unsigned long size, pgprot_t prot);
#define io_remap_pfn_range io_remap_pfn_range

#include <asm-generic/pgtable.h>

#endif

#ifdef CONFIG_P4_X86
#ifndef __ASSEMBLY__
#include <asm/x86_init.h>
#endif
#endif

#endif
