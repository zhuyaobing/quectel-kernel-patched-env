/*
 * azu@sysgo
 *
 * needs fixing, this are the leftovers from archparam-ppc.h
 */
#ifndef __P4_ELF_PPC_H
#define __P4_ELF_PPC_H

//#error FIXME: Broken stuff

#include <linux/types.h>
#include <asm/host_context.h>
#include <asm/string.h>
#include <asm/cputable.h>
#include <asm/auxvec.h>

#define ELF_PLATFORM (0)

#define ELF_ET_DYN_BASE (0x08000000)

/* the following stolen from asm-ppc/elf.h */
#define ELF_NGREG	48	/* includes nip, msr, lr, etc. */
#define ELF_NFPREG	33	/* includes fpscr */
/* General registers */
typedef unsigned long elf_greg_t;
typedef elf_greg_t elf_gregset_t[ELF_NGREG];

typedef unsigned long elf_greg_t32;
#define ELF_GREG_TYPE elf_greg_t32

/* Floating point registers */
typedef double elf_fpreg_t;
typedef elf_fpreg_t elf_fpregset_t[ELF_NFPREG];

#ifdef __KERNEL__

/* Common routine for both 32-bit and 64-bit processes */
static inline void ppc_elf_core_copy_regs(elf_gregset_t elf_regs,
                                            struct pt_regs *regs)
{
        int i, nregs;

        memset((void *)elf_regs, 0, sizeof(elf_gregset_t));

        /* Our registers are always unsigned longs, whether we're a 32 bit
         * process or 64 bit, on either a 64 bit or 32 bit kernel.
         * Don't use ELF_GREG_TYPE here. */
        nregs = sizeof(struct pt_regs) / sizeof(unsigned long);
        if (nregs > ELF_NGREG)
                nregs = ELF_NGREG;

        for (i = 0; i < nregs; i++) {
                /* This will correctly truncate 64 bit registers to 32 bits
                 * for a 32 bit process on a 64 bit kernel. */
                elf_regs[i] = (elf_greg_t)((ELF_GREG_TYPE *)regs)[i];
	        //elf_regs[i] = (elf_greg_t)((unsigned long *)regs)[i];
        }
}
#define ELF_CORE_COPY_REGS(gregs, regs) ppc_elf_core_copy_regs(gregs, regs);

#endif

#define ELF_DATA        ELFDATA2MSB
#define ELF_ARCH	EM_PPC
#define ELF_CLASS	ELFCLASS32

extern int dcache_bsize;
extern int icache_bsize;
extern int ucache_bsize;

/* vDSO has arch_setup_additional_pages */
#define ARCH_HAS_SETUP_ADDITIONAL_PAGES
struct linux_binprm;
extern int arch_setup_additional_pages(struct linux_binprm *bprm,
				       int uses_interp);
#define VDSO_AUX_ENT(a,b) NEW_AUX_ENT(a,b)

/*
 * The requirements here are:
 * - keep the final alignment of sp (sp & 0xf)
 * - make sure the 32-bit value at the first 16 byte aligned position of
 *   AUXV is greater than 16 for glibc compatibility.
 *   AT_IGNOREPPC is used for that.
 * - for compatibility with glibc ARCH_DLINFO must always be defined on PPC,
 *   even if DLINFO_ARCH_ITEMS goes to zero or is undefined.
 */
#define ARCH_DLINFO							\
do {									\
	/* Handle glibc compatibility. */				\
	NEW_AUX_ENT(AT_IGNOREPPC, AT_IGNOREPPC);			\
	NEW_AUX_ENT(AT_IGNOREPPC, AT_IGNOREPPC);			\
	/* Cache size items */						\
	NEW_AUX_ENT(AT_DCACHEBSIZE, dcache_bsize);			\
	NEW_AUX_ENT(AT_ICACHEBSIZE, icache_bsize);			\
	NEW_AUX_ENT(AT_UCACHEBSIZE, ucache_bsize);			\
	VDSO_AUX_ENT(AT_SYSINFO_EHDR, current->mm->context.vdso_base);	\
} while (0)

#define ELF_HWCAP (0)

#define SET_PERSONALITY(ex) do ; while(0)

#define ELF_EXEC_PAGESIZE 4096

#define elf_check_arch(x) (1)

#define ELF_CLASS ELFCLASS32

#define USE_ELF_CORE_DUMP

/* vrregs */
#define ELF_NVRREG	33	/* includes vscr */
typedef __vector128 elf_vrreg_t;
typedef elf_vrreg_t elf_vrregset_t[ELF_NVRREG];



/* PowerPC relocations defined by the ABIs */
#define R_PPC_NONE		0
#define R_PPC_ADDR32		1	/* 32bit absolute address */
#define R_PPC_ADDR24		2	/* 26bit address, 2 bits ignored.  */
#define R_PPC_ADDR16		3	/* 16bit absolute address */
#define R_PPC_ADDR16_LO		4	/* lower 16bit of absolute address */
#define R_PPC_ADDR16_HI		5	/* high 16bit of absolute address */
#define R_PPC_ADDR16_HA		6	/* adjusted high 16bit */
#define R_PPC_ADDR14		7	/* 16bit address, 2 bits ignored */
#define R_PPC_ADDR14_BRTAKEN	8
#define R_PPC_ADDR14_BRNTAKEN	9
#define R_PPC_REL24		10	/* PC relative 26 bit */
#define R_PPC_REL14		11	/* PC relative 16 bit */
#define R_PPC_REL14_BRTAKEN	12
#define R_PPC_REL14_BRNTAKEN	13
#define R_PPC_GOT16		14
#define R_PPC_GOT16_LO		15
#define R_PPC_GOT16_HI		16
#define R_PPC_GOT16_HA		17
#define R_PPC_PLTREL24		18
#define R_PPC_COPY		19
#define R_PPC_GLOB_DAT		20
#define R_PPC_JMP_SLOT		21
#define R_PPC_RELATIVE		22
#define R_PPC_LOCAL24PC		23
#define R_PPC_UADDR32		24
#define R_PPC_UADDR16		25
#define R_PPC_REL32		26
#define R_PPC_PLT32		27
#define R_PPC_PLTREL32		28
#define R_PPC_PLT16_LO		29
#define R_PPC_PLT16_HI		30
#define R_PPC_PLT16_HA		31
#define R_PPC_SDAREL16		32
#define R_PPC_SECTOFF		33
#define R_PPC_SECTOFF_LO	34
#define R_PPC_SECTOFF_HI	35
#define R_PPC_SECTOFF_HA	36

/* PowerPC relocations defined for the TLS access ABI.  */
#define R_PPC_TLS		67 /* none	(sym+add)@tls */
#define R_PPC_DTPMOD32		68 /* word32	(sym+add)@dtpmod */
#define R_PPC_TPREL16		69 /* half16*	(sym+add)@tprel */
#define R_PPC_TPREL16_LO	70 /* half16	(sym+add)@tprel@l */
#define R_PPC_TPREL16_HI	71 /* half16	(sym+add)@tprel@h */
#define R_PPC_TPREL16_HA	72 /* half16	(sym+add)@tprel@ha */
#define R_PPC_TPREL32		73 /* word32	(sym+add)@tprel */
#define R_PPC_DTPREL16		74 /* half16*	(sym+add)@dtprel */
#define R_PPC_DTPREL16_LO	75 /* half16	(sym+add)@dtprel@l */
#define R_PPC_DTPREL16_HI	76 /* half16	(sym+add)@dtprel@h */
#define R_PPC_DTPREL16_HA	77 /* half16	(sym+add)@dtprel@ha */
#define R_PPC_DTPREL32		78 /* word32	(sym+add)@dtprel */
#define R_PPC_GOT_TLSGD16	79 /* half16*	(sym+add)@got@tlsgd */
#define R_PPC_GOT_TLSGD16_LO	80 /* half16	(sym+add)@got@tlsgd@l */
#define R_PPC_GOT_TLSGD16_HI	81 /* half16	(sym+add)@got@tlsgd@h */
#define R_PPC_GOT_TLSGD16_HA	82 /* half16	(sym+add)@got@tlsgd@ha */
#define R_PPC_GOT_TLSLD16	83 /* half16*	(sym+add)@got@tlsld */
#define R_PPC_GOT_TLSLD16_LO	84 /* half16	(sym+add)@got@tlsld@l */
#define R_PPC_GOT_TLSLD16_HI	85 /* half16	(sym+add)@got@tlsld@h */
#define R_PPC_GOT_TLSLD16_HA	86 /* half16	(sym+add)@got@tlsld@ha */
#define R_PPC_GOT_TPREL16	87 /* half16*	(sym+add)@got@tprel */
#define R_PPC_GOT_TPREL16_LO	88 /* half16	(sym+add)@got@tprel@l */
#define R_PPC_GOT_TPREL16_HI	89 /* half16	(sym+add)@got@tprel@h */
#define R_PPC_GOT_TPREL16_HA	90 /* half16	(sym+add)@got@tprel@ha */
#define R_PPC_GOT_DTPREL16	91 /* half16*	(sym+add)@got@dtprel */
#define R_PPC_GOT_DTPREL16_LO	92 /* half16*	(sym+add)@got@dtprel@l */
#define R_PPC_GOT_DTPREL16_HI	93 /* half16*	(sym+add)@got@dtprel@h */
#define R_PPC_GOT_DTPREL16_HA	94 /* half16*	(sym+add)@got@dtprel@ha */

/* keep this the last entry. */
#define R_PPC_NUM		95

#endif
