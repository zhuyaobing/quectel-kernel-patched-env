/* minimal implementation taken from orig ppc cputable.h */
#ifndef __ASM_POWERPC_CPUTABLE_H
#define __ASM_POWERPC_CPUTABLE_H

#ifdef __KERNEL__

/* CPU kernel features */

/* Retain the 32b definitions all use bottom half of word */
/* p4 align.c*/
#define CPU_FTR_NODSISRALIGN		ASM_CONST(0x0000000000100000)
/* kernel ptrace.c */
#define CPU_FTR_601			ASM_CONST(0x0000000000000100)
/* module */
#define CPU_FTR_LWSYNC			ASM_CONST(0x0000000008000000)
/* vdso */
#define CPU_FTR_COHERENT_ICACHE		ASM_CONST(0x00000001)
#define CPU_FTR_USE_TB			ASM_CONST(0x00000040)

#ifndef __ASSEMBLY__

struct cpu_spec {
	unsigned long	cpu_features;		/* Kernel features */
	unsigned int	mmu_features;		/* MMU features */
};

extern struct cpu_spec		*cur_cpu_spec;

extern void do_feature_fixups(unsigned long value, void *fixup_start,
			      void *fixup_end);

static inline int cpu_has_feature(unsigned long feature)
{
	/* we don't have special features */
	return 0;
	/*return (CPU_FTRS_ALWAYS & feature) ||
	       (CPU_FTRS_POSSIBLE
		& cur_cpu_spec->cpu_features
		& feature);*/
}
#endif /* !__ASSEMBLY__ */

#endif /* __KERNEL__ */

#endif
