#ifndef __P4_X86_CACHEFLUSH_H
#define __P4_X86_CACHEFLUSH_H

static inline void clflush(volatile void *__p)
{
	asm volatile("clflush %0" : "+m" (*(volatile char __force *)__p));
}

void clflush_cache_range(void *addr, unsigned int size);

/* Caches aren't brain-dead on the intel. */
#define ARCH_IMPLEMENTS_FLUSH_DCACHE_PAGE 0
#define flush_dcache_page(page)			do { } while (0)

#define flush_icache_range(start, end)		do { } while (0)
#define __flush_icache_range(start, end)	do { } while (0)

#endif
