/*
 * Copyright (C) 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#ifndef __UM_PROCESSOR_I386_H
#define __UM_PROCESSOR_I386_H

#include "linux/string.h"
//#include "asm/host_ldt.h"
#include "asm/segment.h"

/* IO-Bitmap Emulation */
#define IO_BITMAP_SIZE 32

struct arch_thread {
	unsigned long debugregs[8];
	int debugregs_seq;
	unsigned long io_bitmap[IO_BITMAP_SIZE];
	int iopl;
};

#define INIT_ARCH_THREAD { .debugregs  		= { [0 ... 7] = 0 }, \
                           .debugregs_seq	= 0, \
                           .iopl            = 0, \
                           .io_bitmap  		= { 0,} }

#include "arch/asm/user.h"

#include "asm/processor-generic.h"

/*
 * Default implementation of macro that returns current
 * instruction pointer ("program counter"). Stolen
 * from asm-i386/processor.h
 */
#define current_text_addr() \
	({ void *pc; __asm__("movl $1f,%0\n1:":"=g" (pc)); pc; })

/* REP NOP (PAUSE) is a good thing to insert into busy-wait loops. */
static inline void rep_nop(void)
{
	__asm__ __volatile__("rep;nop": : :"memory");
}

#define cpu_relax()	rep_nop()

/* Stop speculative execution and prefetching of modified code. */
static inline void sync_core(void)
{
	int tmp;

#if defined(CONFIG_M386) || defined(CONFIG_M486)
	if (boot_cpu_data.x86 < 5)
		/* There is no speculative execution.
		 * jmp is a barrier to prefetching. */
		asm volatile("jmp 1f\n1:\n" ::: "memory");
	else
#endif
		/* cpuid is a barrier to speculative execution.
		 * Prefetched instructions are automatically
		 * invalidated when modified. */
		asm volatile("cpuid" : "=a" (tmp) : "0" (1)
			     : "ebx", "ecx", "edx", "memory");
}


static inline void __cpuid(unsigned int *eax, unsigned int *ebx,
				unsigned int *ecx, unsigned int *edx)
{
	/* ecx is often an input as well as an output. */
	asm volatile("cpuid"
	    : "=a" (*eax),
	      "=b" (*ebx),
	      "=c" (*ecx),
	      "=d" (*edx)
	    : "0" (*eax), "2" (*ecx));
}

/*
 * Generic CPUID function
 * clear %ecx since some cpus (Cyrix MII) do not set or clear %ecx
 * resulting in stale register contents being returned.
 */
static inline void cpuid(unsigned int op,
			 unsigned int *eax, unsigned int *ebx,
			 unsigned int *ecx, unsigned int *edx)
{
	*eax = op;
	*ecx = 0;
	__cpuid(eax, ebx, ecx, edx);
}

/* Some CPUID calls want 'count' to be placed in ecx */
static inline void cpuid_count(unsigned int op, int count,
			       unsigned int *eax, unsigned int *ebx,
			       unsigned int *ecx, unsigned int *edx)
{
	*eax = op;
	*ecx = count;
	__cpuid(eax, ebx, ecx, edx);
}

/*
 * CPUID functions returning a single datum
 */
static inline unsigned int cpuid_eax(unsigned int op)
{
	unsigned int eax, ebx, ecx, edx;

	cpuid(op, &eax, &ebx, &ecx, &edx);

	return eax;
}

static inline unsigned int cpuid_ebx(unsigned int op)
{
	unsigned int eax, ebx, ecx, edx;

	cpuid(op, &eax, &ebx, &ecx, &edx);

	return ebx;
}

static inline unsigned int cpuid_ecx(unsigned int op)
{
	unsigned int eax, ebx, ecx, edx;

	cpuid(op, &eax, &ebx, &ecx, &edx);

	return ecx;
}

static inline unsigned int cpuid_edx(unsigned int op)
{
	unsigned int eax, ebx, ecx, edx;

	cpuid(op, &eax, &ebx, &ecx, &edx);

	return edx;
}

#endif
