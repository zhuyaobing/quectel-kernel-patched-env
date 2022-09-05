/*
 * linux/arch/p4/include/asm/cacheflush.h
 *
 * Common cache flush interface.
 */
#ifndef __P4_CACHEFLUSH_H
#define __P4_CACHEFLUSH_H

#include <linux/mm.h>
#include <asm/host_cache.h>

/*
 * No cache flushing is required when page tables are changed,
 * because the microkernel handles this.
 */
#define flush_cache_all()			do { } while (0)
#define flush_cache_mm(mm)			do { } while (0)
#define flush_cache_dup_mm(mm)		do { } while (0)
#define flush_cache_range(vma, start, end)	do { } while (0)
#define flush_cache_page(vma, vmaddr, pfn)	do { } while (0)
#define flush_icache_page(vma, page)		do { } while (0)
#define flush_cache_vmap(start, end)		do { } while (0)
#define flush_cache_vunmap(start, end)		do { } while (0)

#define flush_dcache_mmap_lock(mapping)		do { } while (0)
#define flush_dcache_mmap_unlock(mapping)	do { } while (0)

/* sub-architecture specific cache routines */
#include <arch-override/asm/cacheflush.h>

#define copy_to_user_page(vma, page, vaddr, dst, src, len) \
	do { \
		memcpy(dst, src, len); \
		__flush_icache_range((unsigned long)dst, (unsigned long)dst + len);  \
	} while (0)
#define copy_from_user_page(vma, page, vaddr, dst, src, len) \
	memcpy(dst, src, len)

int change_page_attr(unsigned long address, int numpages, pgprot_t prot);

#ifdef CONFIG_P4_X86
/* tricks required for AGP / DRM */
#define set_pages_uc(page, numpages) change_page_attr((unsigned long)pfn_to_phys(page_to_pfn((page))), (numpages), __pgprot(_PAGE_NOCACHE))
#define set_pages_wb(page, numpages) change_page_attr((unsigned long)pfn_to_phys(page_to_pfn((page))), (numpages), __pgprot(_PAGE_CACHE_WB))

#define set_memory_uc(addr, numpages) change_page_attr((addr), (numpages), __pgprot(_PAGE_NOCACHE))
#define set_memory_wc(addr, numpages) change_page_attr((addr), (numpages), __pgprot(_PAGE_CACHE_WC))
#define set_memory_wb(addr, numpages) change_page_attr((addr), (numpages), __pgprot(_PAGE_CACHE_WB))

#define wbinvd_on_all_cpus() uk_flush_dcache_all()
#endif

#endif
