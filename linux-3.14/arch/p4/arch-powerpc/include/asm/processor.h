#ifndef __P4_PROCESSOR_PPC_H
#define __P4_PROCESSOR_PPC_H

#include <asm/reg.h>

#if defined(__ASSEMBLY__)

#define CONFIG_ALL_PPC

#if defined(CONFIG_4xx)
#ifndef KERNELBASE 
#define KERNELBASE 0xc0000000
#endif
#endif

#else

struct arch_thread {
	int empty;
};

#define INIT_ARCH_THREAD { .empty = 0 }

#include "arch/asm/user.h"

#include "asm/processor-generic.h"

#define current_text_addr() ({ __label__ _l; _l: &&_l;})

#define cpu_relax()	barrier()

/* Macros for adjusting thread priority (hardware multi-threading) */
#define HMT_very_low()   asm volatile("or 31,31,31   # very low priority")
#define HMT_low()	 asm volatile("or 1,1,1	     # low priority")
#define HMT_medium_low() asm volatile("or 6,6,6      # medium low priority")
#define HMT_medium()	 asm volatile("or 2,2,2	     # medium priority")
#define HMT_medium_high() asm volatile("or 5,5,5      # medium high priority")
#define HMT_high()	 asm volatile("or 3,3,3	     # high priority")

#ifdef __KERNEL__
extern void flush_instruction_cache(void);
extern int fix_alignment(struct pt_regs *);
extern void cvt_fd(float *from, double *to);
extern void cvt_df(double *from, float *to);
extern void _nmask_and_or_msr(unsigned long nmask, unsigned long or_val);
#endif

#endif
#endif
