#ifndef __ASMARM_ELF_H
#define __ASMARM_ELF_H

#include <asm/hwcap.h>

#ifndef __ASSEMBLY__
/*
 * ELF register definitions..
 */
#include <asm/ptrace.h>
#include <asm/user.h>

typedef unsigned long elf_greg_t;
typedef unsigned long elf_freg_t[3];

/*
 * the native version uses pt_regs which differs in size to P4Linux and thus
 * user space applications cannot use it (e.g. core dumps)
 */
#define ELF_NGREG ((18*4) / sizeof(elf_greg_t))
typedef elf_greg_t elf_gregset_t[ELF_NGREG];

/* order of the first 16 register is identical */
#define ELF_CORE_COPY_REGS(pr_reg, regs) do { 		\
	pr_reg[0]  = regs->context[HOST_R0];		\
	pr_reg[1]  = regs->context[HOST_R1];		\
	pr_reg[2]  = regs->context[HOST_R2];		\
	pr_reg[3]  = regs->context[HOST_R3];		\
	pr_reg[4]  = regs->context[HOST_R4];		\
	pr_reg[5]  = regs->context[HOST_R5];		\
	pr_reg[6]  = regs->context[HOST_R6];		\
	pr_reg[7]  = regs->context[HOST_R7];		\
	pr_reg[8]  = regs->context[HOST_R8];		\
	pr_reg[9]  = regs->context[HOST_R9];		\
	pr_reg[10] = regs->context[HOST_R10];		\
	pr_reg[11] = regs->context[HOST_FP];		\
	pr_reg[12] = regs->context[HOST_IP];		\
	pr_reg[13] = regs->context[HOST_SP];		\
	pr_reg[14] = regs->context[HOST_LR];		\
	pr_reg[15] = regs->context[HOST_EPC];		\
	pr_reg[16] = regs->context[HOST_CPSR];		\
       pr_reg[17] = regs->orig_r0;			\
} while (0);

typedef struct user_fp elf_fpregset_t;
#endif

#define EM_ARM	40
#define EF_ARM_APCS26 0x08
#define EF_ARM_SOFT_FLOAT 0x200
#define EF_ARM_EABI_MASK 0xFF000000

#define R_ARM_NONE		0
#define R_ARM_PC24		1
#define R_ARM_ABS32		2
#define R_ARM_CALL		28
#define R_ARM_JUMP24		29
#define R_ARM_V4BX		40
#define R_ARM_PREL31		42
#define R_ARM_MOVW_ABS_NC	43
#define R_ARM_MOVT_ABS		44

#define R_ARM_THM_CALL		10
#define R_ARM_THM_JUMP24	30
#define R_ARM_THM_MOVW_ABS_NC	47
#define R_ARM_THM_MOVT_ABS	48

/*
 * These are used to set parameters in the core dumps.
 */
#define ELF_CLASS	ELFCLASS32
#ifdef __ARMEB__
#define ELF_DATA	ELFDATA2MSB
#else
#define ELF_DATA	ELFDATA2LSB
#endif
#define ELF_ARCH	EM_ARM

#ifndef __ASSEMBLY__
/*
 * This yields a string that ld.so will use to load implementation
 * specific libraries for optimization.  This is more specific in
 * intent than poking at uname or /proc/cpuinfo.
 *
 * For now we just provide a fairly general string that describes the
 * processor family.  This could be made more specific later if someone
 * implemented optimisations that require it.  26-bit CPUs give you
 * "v1l" for ARM2 (no SWP) and "v2l" for anything else (ARM1 isn't
 * supported).  32-bit CPUs give you "v3[lb]" for anything based on an
 * ARM6 or ARM7 core and "armv4[lb]" for anything based on a StrongARM-1
 * core.
 */
#define ELF_PLATFORM_SIZE 8
#define ELF_PLATFORM	(elf_platform)

extern char elf_platform[];
#endif

/*
 * This is used to ensure we don't load something for the wrong architecture.
 */
#define elf_check_arch(x) ((x)->e_machine == EM_ARM && ELF_PROC_OK(x))

/*
 * 32-bit code is always OK.  Some cpus can do 26-bit, some can't.
 */
#define ELF_PROC_OK(x)	(ELF_THUMB_OK(x) && ELF_26BIT_OK(x))

#define ELF_THUMB_OK(x) \
	((elf_hwcap & HWCAP_THUMB && ((x)->e_entry & 1) == 1) || \
	 ((x)->e_entry & 3) == 0)

#define ELF_26BIT_OK(x) \
	((elf_hwcap & HWCAP_26BIT && (x)->e_flags & EF_ARM_APCS26) || \
	  ((x)->e_flags & EF_ARM_APCS26) == 0)

#define USE_ELF_CORE_DUMP
#define ELF_EXEC_PAGESIZE	4096

/* This is the location that an ET_DYN program is loaded if exec'ed.  Typical
   use of this is to invoke "./ld.so someprog" to test out a new version of
   the loader.  We need to make sure that it is out of the way of the program
   that it will "exec", and that there is sufficient room for the brk.  */

#define ELF_ET_DYN_BASE	(2 * TASK_SIZE / 3)

/* When the program starts, a1 contains a pointer to a function to be 
   registered with atexit, as per the SVR4 ABI.  A value of 0 means we 
   have no such handler.  */
#define ELF_PLAT_INIT(_r, load_addr)	(_r)->context[HOST_R0] = 0

/*
 * Since the FPA coprocessor uses CP1 and CP2, and iWMMXt uses CP0
 * and CP1, we only enable access to the iWMMXt coprocessor if the
 * binary is EABI or softfloat (and thus, guaranteed not to use
 * FPA instructions.)
 */
#define TIF_USING_IWMMXT        17
#define SET_PERSONALITY(ex)					\
	do {								\
		if ((ex).e_flags & EF_ARM_APCS26) {			\
			set_personality(PER_LINUX);			\
		} else {						\
			set_personality(PER_LINUX_32BIT);		\
			if (elf_hwcap & HWCAP_IWMMXT && (ex).e_flags & (EF_ARM_EABI_MASK | EF_ARM_SOFT_FLOAT)) \
				set_thread_flag(TIF_USING_IWMMXT);	\
			else						\
				clear_thread_flag(TIF_USING_IWMMXT);	\
		}							\
	} while (0)

#endif
