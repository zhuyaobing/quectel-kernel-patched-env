#ifndef __P4_ARM_CACHEFLUSH_H
#define __P4_ARM_CACHEFLUSH_H

#define ARCH_IMPLEMENTS_FLUSH_DCACHE_PAGE 1
extern void flush_dcache_page(struct page *page);

/* icache handling */
extern void __flush_icache_range(unsigned long start, unsigned long end);
extern void flush_icache_range_user(unsigned long __user start, unsigned long __user end);
#define flush_icache_range(start, end) flush_icache_range_user(start, end)

/* special dcache handling for user space addresses */
extern void __flush_dcache_range(unsigned long start, unsigned long end);
extern void flush_dcache_range_user(unsigned long __user start, unsigned long __user end);

/*
 * These are private to the dma-mapping API.  Do not use directly.
 * Their sole purpose is to ensure that data held in the cache
 * is visible to DMA, or data written by DMA to system memory is
 * visible to the CPU.
 */
extern void dmac_map_area(const void *, size_t, int);
extern void dmac_unmap_area(const void *, size_t, int);
extern void dmac_flush_range(const void *, const void *);

#endif

