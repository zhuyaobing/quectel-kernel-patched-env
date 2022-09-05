/*
 * Copyright (C) 2000, 2001, 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 *
 * azu@sysgo.de: adapted to P4
 */

#ifndef __P4_PROCESSOR_GENERIC_H
#define __P4_PROCESSOR_GENERIC_H

struct pt_regs;

struct task_struct;

#include <asm/ptrace.h>
#include <asm/setjmp.h>

#ifdef CONFIG_P4_X86
#include <asm/cpufeature.h>
#endif

struct mm_struct;

#ifdef CONFIG_P4_ARM
/* used for ptrace */
union debug_insn {
	u32	arm;
	u16	thumb;
};

struct debug_entry {
	u32			address;
	union debug_insn	insn;
};

struct debug_info {
	int                     nsaved;
	struct debug_entry      bp[2];
};
#endif

struct thread_struct {
	unsigned int uid;		/* P4 uid of userspace thread */
	unsigned long kernel_stack;
	struct pt_regs regs;
	struct task_struct *prev_sched;
	struct arch_thread arch;
	struct {
		int (*proc)(void *);
		void *arg;
	} new_thread;
	arch_jmp_buf switch_buf;

#ifdef CONFIG_P4_ARM
	/* used for ptrace */
	struct debug_info debug;
#endif
};

#define INIT_THREAD \
{ \
	.uid			= 0, \
	.kernel_stack		= 0, \
	.regs		   	= EMPTY_REGS, \
	.prev_sched		= NULL, \
	.arch			= INIT_ARCH_THREAD, \
	.new_thread		= { NULL, NULL }, \
}


static inline void release_thread(struct task_struct *task)
{
}

/*
 * User space process size: 2 GB (default on x86_32, 512 GB on x86_64). Keep in
 * sync with P4_MEM_USR_END.
 */
#ifdef CONFIG_64BIT
/* up to 512 GB with a 3-level page table */
#define TASK_SIZE		0x8000000000
#else
#define TASK_SIZE		0x80000000
#endif

#define STACK_TOP		TASK_SIZE
#define STACK_TOP_MAX		STACK_TOP

/* This decides where the kernel will search for a free chunk of vm
 * space during mmap's.
 */
#define TASK_UNMAPPED_BASE	(0x40000000)

extern void start_thread(struct pt_regs *regs, unsigned long entry,
			 unsigned long stack);

#define task_pt_regs(p) \
	((struct pt_regs *)&p->thread.regs)

#define KSTK_EIP(tsk) (tsk->thread.switch_buf[0].regs[ARCH_JMPBUF_IP])
/* On a native system the stack pointer does not differ between the user and
 * kernel address space. We always return the kernel valid stack pointer here.
 * Check copy_thread for more information about this restriction.
 */
#define KSTK_ESP(tsk) (tsk->thread.switch_buf[0].regs[ARCH_JMPBUF_SP])

#define thread_saved_pc(tsk)	\
	((unsigned long)(KSTK_EIP(tsk)))
#ifdef CONFIG_P4_ARM
#define thread_saved_sp(tsk)	\
	((unsigned long)(KSTK_ESP(tsk)))
#define thread_saved_fp(tsk)	\
	((unsigned long)(tsk->thread.switch_buf[0].regs[ARCH_JMPBUF_FP]))
#endif

unsigned long get_wchan(struct task_struct *p);

struct cpuinfo_p4 {
	int unused;
#ifdef CONFIG_P4_X86
	__u8	x86;			/* CPU family */
	__u8	x86_vendor;		/* CPU vendor */
	__u8	x86_model;
#define X86_VENDOR_INTEL	0
#define X86_VENDOR_CYRIX	1
#define X86_VENDOR_AMD		2
#define X86_VENDOR_UMC		3
#define X86_VENDOR_CENTAUR	5
#define X86_VENDOR_TRANSMETA	7
#define X86_VENDOR_NSC		8
#define X86_VENDOR_NUM		9
#define X86_VENDOR_UNKNOWN	0xff

	__u8	x86_mask;
	u16		x86_clflush_size;
	char	x86_vendor_id[16];
	__u32	x86_capability[NCAPINTS];
	/* Max extended CPUID function supported: */
	__u32	extended_cpuid_level;
	/* Maximum supported CPUID level, -1=no CPUID: */
	int		cpuid_level;
#endif
};

extern struct cpuinfo_p4 cpu_data[];

#ifdef CONFIG_P4_X86
#define boot_cpu_data cpu_data[0]
#define cpu_data(cpu) boot_cpu_data
#define cpuinfo_x86 cpuinfo_p4
extern void detect_x86_cpu(void);

#define wbinvd() uk_flush_dcache_all()

#endif

#define current_cpu_data cpu_data[smp_processor_id()]
#define raw_current_cpu_data cpu_data[raw_smp_processor_id()]

#ifdef CONFIG_P4_X86
extern void free_init_pages(char *what, unsigned long begin, unsigned long end);
#endif

#endif
