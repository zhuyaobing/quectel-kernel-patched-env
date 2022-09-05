/* Copyright (c) 2016 OpenSynergy GmbH */
#ifndef __P4_PROCESSOR_ARM_H
#define __P4_PROCESSOR_ARM_H

#ifdef __KERNEL__

#include <asm/unified.h>

struct arch_thread {
	int empty;
};

#define INIT_ARCH_THREAD { .empty = 0 }

#include "arch/asm/user.h"

#include "asm/processor-generic.h"

/*
 * Default implementation of macro that returns current
 * instruction pointer ("program counter").
 */
#define current_text_addr() ({ __label__ _l; _l: &&_l;})

#define cpu_relax()	barrier()

#ifdef CONFIG_SMP
#define __ALT_SMP_ASM(smp, up)                                         \
       "9998:  " smp "\n"                                              \
       "       .pushsection \".alt.smp.init\", \"a\"\n"                \
       "       .long   9998b\n"                                        \
       "       " up "\n"                                               \
       "       .popsection\n"
#else
#define __ALT_SMP_ASM(smp, up) up
#endif

/*
 * Prefetching support - only ARMv5.
 */
#if __LINUX_ARM_ARCH__ >= 5

#define ARCH_HAS_PREFETCH
static inline void prefetch(const void *ptr)
{
	__asm__ __volatile__(
		"pld\t%a0"
		:
		: "p" (ptr)
		: "cc");
}

#define ARCH_HAS_PREFETCHW
#define prefetchw(ptr)	prefetch(ptr)

#define ARCH_HAS_SPINLOCK_PREFETCH
#define spin_lock_prefetch(x) do { } while (0)

#endif

#endif

#endif
