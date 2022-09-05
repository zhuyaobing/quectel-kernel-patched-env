#ifndef __P4_CACHE_H
#define __P4_CACHE_H

#if defined(CONFIG_P4_ARM) || defined(CONFIG_P4_X86) || \
	defined(CONFIG_P4_PPC_OEA) || defined(CONFIG_P4_PPC_4xx) || \
	defined(CONFIG_P4_PPC_SPE)
#define L1_CACHE_SHIFT		5
#elif defined(CONFIG_P4_PPC_E500MC)
#define L1_CACHE_SHIFT		6
#else
#error Please adapt L1_CACHE_SHIFT for this architecture
#endif

#define L1_CACHE_BYTES		(1 << L1_CACHE_SHIFT)

#define	SMP_CACHE_BYTES		L1_CACHE_BYTES

#ifdef CONFIG_P4_PPC

#if defined(__KERNEL__) && !defined(__ASSEMBLY__)
extern void cacheable_memzero(void *p, unsigned int nb);
extern void *cacheable_memcpy(void *, const void *, unsigned int);
#endif

#endif /* CONFIG_P4_PPC */

#endif
