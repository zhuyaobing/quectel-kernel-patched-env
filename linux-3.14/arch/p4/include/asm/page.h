/*
 * linux/include/asm-p4/page.h
 *
 * adopted from linux/include/asm-i386/page.h and linux/include/asm-ppc/page.h
 *
 * 2004-01-22, azu@sysgo.de: rewritten to fit virt-virt mapping
 * 2006-05-30, azu@sysgo.de: Adapted to linux 2.6.15
 */

#ifndef __P4_PAGE_H
#define __P4_PAGE_H

#include <linux/const.h>

/* PAGE_SHIFT determines the page size */
#define PAGE_SHIFT	12
#define PAGE_SIZE	(_AC(1,UL) << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE-1))

#ifdef __KERNEL__
#ifndef __ASSEMBLY__

#include <asm/memory.h>
#include <asm-generic/memory_model.h>

struct page;

#define clear_page(page)	memset((void *)(page), 0, PAGE_SIZE)
#define copy_page(to,from)	memcpy((void *)(to), (void *)(from), PAGE_SIZE)

#if defined(CONFIG_P4_X86)

#define clear_user_page(page, vaddr, pg)	clear_page(page)
#define copy_user_page(to, from, vaddr, pg)	copy_page(to, from)

#else /* CONFIG_8xx || CONFIG_6xx (both PowerPC) */

extern void clear_user_page(void *to, unsigned long vaddr, struct page *page);
extern void copy_user_page(void *to, void *from, unsigned long vaddr, struct page *page);

#endif

#if defined(CONFIG_P4_X86) || defined(CONFIG_P4_PPC) || defined(CONFIG_P4_ARM)

/* We do define AT_SYSINFO_EHDR but don't use the gate mechanism */
#define __HAVE_ARCH_GATE_AREA		1

#endif

#ifdef CONFIG_P4_ARM
/*
 * With EABI on ARMv5 and above we must have 64-bit aligned slab pointers.
 */
#if defined(CONFIG_AEABI) && (__LINUX_ARM_ARCH__ >= 5)
#define ARCH_SLAB_MINALIGN 8
#endif

#endif

#define STRICT_MM_TYPECHECKS

#ifdef STRICT_MM_TYPECHECKS
/*
 * These are used to make use of C type-checking..
 */
typedef struct { unsigned long pte; } pte_t;
typedef struct { unsigned long pgd; } pgd_t;
typedef struct { unsigned long pgprot; } pgprot_t;

#ifdef CONFIG_3_LEVEL_PGTABLES
typedef struct { unsigned long pmd; } pmd_t;
#define pmd_val(x)	((x).pmd)
#define __pmd(x) ((pmd_t) { (x) } )
#endif

#define pte_val(x)	((x).pte)
#define pgd_val(x)	((x).pgd)
#define pgprot_val(x)	((x).pgprot)

#define __pte(x)	((pte_t) { (x) } )
#define __pgd(x)	((pgd_t) { (x) } )
#define __pgprot(x)	((pgprot_t) { (x) } )

#else
/*
 * .. while these make it easier on the compiler
 */
typedef unsigned long pte_t;
typedef unsigned long pgd_t;
typedef unsigned long pgprot_t;

#define pte_val(x)	(x)
#define pgd_val(x)	(x)
#define pgprot_val(x)	(x)

#define __pte(x)	(x)
#define __pgd(x)	(x)
#define __pgprot(x)	(x)

#endif



#define pte_get_bits(p, bits) ((p).pte & (bits))
#define pte_set_bits(p, bits) ((p).pte |= (bits))
#define pte_clear_bits(p, bits) ((p).pte &= ~(bits))
#define pte_copy(to, from) ((to).pte = (from).pte)
#define pte_is_zero(p) (!((p).pte & ~_PAGE_NEWPAGE))
#define pte_set_val(p, phys, prot) (p).pte = (phys | pgprot_val(prot))

typedef unsigned long pfn_t;
typedef unsigned long phys_t;

#endif /* __ASSEMBLY__ */


#define __pa(x)		((unsigned long)(x)-PAGE_OFFSET)
#define __va(x)		((void *)((unsigned long)(x)+PAGE_OFFSET))




#define phys_to_pfn(p) ((p) >> PAGE_SHIFT)
#define pfn_to_phys(pfn) ((pfn) << PAGE_SHIFT)

#define pfn_valid(pfn)		((pfn) < max_mapnr)

#define virt_to_page(kaddr)	(pfn_to_page(__pa(kaddr) >> PAGE_SHIFT))
#define virt_addr_valid(kaddr)	(pfn_valid(__pa(kaddr) >> PAGE_SHIFT))

#define PHYS_TO_NID(addr)	(0)

/*
 * FIXME: maybe optimize standard flags later
 */
#define VM_DATA_DEFAULT_FLAGS	(VM_READ | VM_WRITE | VM_EXEC | \
				 VM_MAYREAD | VM_MAYWRITE | VM_MAYEXEC)



#endif /* __KERNEL__ */

#ifndef __ASSEMBLY__
typedef struct page *pgtable_t;
#endif

#include <asm-generic/getorder.h>

#endif
