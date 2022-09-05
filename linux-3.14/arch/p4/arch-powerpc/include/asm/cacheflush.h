#ifndef __P4_PPC_CACHEFLUSH_H
#define __P4_PPC_CACHEFLUSH_H

#define ARCH_IMPLEMENTS_FLUSH_DCACHE_PAGE 1
extern void flush_dcache_page(struct page *page);

/* icache handling */
extern void __flush_icache_range(unsigned long start, unsigned long end);
extern void flush_icache_range_user(unsigned long __user start, unsigned long __user end);
#define flush_icache_range(start, end) flush_icache_range_user(start, end)

/* PPC specific */

/* internally used in page fault handling */
extern void flush_dcache_icache_page(struct page *page);

extern void __flush_dcache_icache(void *page_va);

extern void flush_dcache_range(unsigned long start, unsigned long stop);
#ifdef CONFIG_PPC32
extern void clean_dcache_range(unsigned long start, unsigned long stop);
extern void invalidate_dcache_range(unsigned long start, unsigned long stop);
#endif /* CONFIG_PPC32 */
#ifdef CONFIG_PPC64
extern void flush_inval_dcache_range(unsigned long start, unsigned long stop);
extern void flush_dcache_phys_range(unsigned long start, unsigned long stop);
#endif

#endif
