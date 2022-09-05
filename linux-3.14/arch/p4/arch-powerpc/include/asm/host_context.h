/*
 * linux/arch/p4/include/sysdep-ppc/host_context.h
 *
 * This describes the host-context of P4/ppc
 *
 * NOTE: Keep in sync with P4/arch/ppc/include/p4regs.h !!!
 *
 * 15. mai 2004, azu@sysgo.de
 */
/*
 * Licensed under the GPL
 */

#ifndef __HOST_CONTEXT_PPC_H_
#define __HOST_CONTEXT_PPC_H_

/* exception frame */
/* regs */
#define HOST_R0		0	/* syscall */
#define HOST_R1		1	/* stack */
#define HOST_R2		2	/* TLS pointer */
#define HOST_R3		3	/* param0 */
#define HOST_R4		4	/* param1 */
#define HOST_R5		5	/* param2 */
#define HOST_R6		6	/* param3 */
#define HOST_R7		7	/* param4 */
#define HOST_R8		8	/* param5 */
#define HOST_R9		9	/* param6 */
#define HOST_R10	10	/* param7 */
#define HOST_R11	11
#define HOST_R12	12
#define HOST_R13	13
#define HOST_R14	14
#define HOST_R15	15
#define HOST_R16	16
#define HOST_R17	17
#define HOST_R18	18
#define HOST_R19	19
#define HOST_R20	20
#define HOST_R21	21
#define HOST_R22	22
#define HOST_R23	23
#define HOST_R24	24
#define HOST_R25	25
#define HOST_R26	26
#define HOST_R27	27
#define HOST_R28	28
#define HOST_R29	29
#define HOST_R30	30
#define HOST_R31	31
#define HOST_CR		32	/* condition register */
#define HOST_XER 	33	/* integer error register */
#define HOST_LR		34	/* link register */
#define HOST_CTR	35	/* counter */
#define HOST_DAR	36	/* data address register */
#define HOST_EX_CODE	37	/* exception code */
#define HOST_DSISR	38	/* DSI (=data storage interrupt) status register */
#define HOST_SRR0	39	/* srr0 contains saved program counter */
#define HOST_SRR1	40	/* ssr1 contains saved msr bits */
#define HOST_TRAP	41	/* entry vector >> 8 (00 - 1f) */

#if defined(CONFIG_P4_PPC_OEA) || defined(CONFIG_P4_PPC_4xx) || defined(CONFIG_P4_PPC_E500MC)
#define HOST_FR0	42	/* 64 bit wide ... */
#define HOST_FPSCR	106

/* sizes */
#define HOST_FRAME_SIZE	108
#endif

#if defined(CONFIG_P4_PPC_SPE)
#define HOST_FPSCR	42		/* SPE FSCR, 32 bit */
#define HOST_FR0	44		/* upper parts of GPRs */

/* sizes */
#define HOST_FRAME_SIZE	78
#endif

/* common sizes */
#define HOST_REG_SIZE	42	/* 42x32bit integer context */

/* wrappers */
#define HOST_IP		HOST_SRR0
#define HOST_MSR	HOST_SRR1
#define HOST_SP		HOST_R1
#define HOST_FAULT	HOST_DAR


struct pt_regs
{
	unsigned long context[HOST_FRAME_SIZE];

	/* you may reorder these */
	unsigned long trap;		/* is segv fixable? 13 || 14 */
	unsigned long orig_r3;		/* R3 on syscall entry */
	long result;			/* R3 on syscall exit */
	int is_user;
};

#define EMPTY_REGS {	\
	{0, /* r0 etc */	\
	} }

#define PT_REGS_R0(r)	((r)->context[HOST_R0])
#define PT_REGS_R1(r)	((r)->context[HOST_R1])
#define PT_REGS_R3(r)	((r)->context[HOST_R3])
#define PT_REGS_R4(r)	((r)->context[HOST_R4])
#define PT_REGS_R5(r)	((r)->context[HOST_R5])
#define PT_REGS_R6(r)	((r)->context[HOST_R6])
#define PT_REGS_R7(r)	((r)->context[HOST_R7])
#define PT_REGS_R8(r)	((r)->context[HOST_R8])
#define PT_REGS_R9(r)	((r)->context[HOST_R9])
#define PT_REGS_R10(r)	((r)->context[HOST_R10])
#define PT_REGS_R11(r)	((r)->context[HOST_R11])
#define PT_REGS_R12(r)	((r)->context[HOST_R12])
#define PT_REGS_R13(r)	((r)->context[HOST_R13])
#define PT_REGS_R14(r)	((r)->context[HOST_R14])
#define PT_REGS_R15(r)	((r)->context[HOST_R15])
#define PT_REGS_R16(r)	((r)->context[HOST_R16])
#define PT_REGS_R17(r)	((r)->context[HOST_R17])
#define PT_REGS_R18(r)	((r)->context[HOST_R18])
#define PT_REGS_R19(r)	((r)->context[HOST_R19])
#define PT_REGS_R20(r)	((r)->context[HOST_R20])
#define PT_REGS_R21(r)	((r)->context[HOST_R21])
#define PT_REGS_R22(r)	((r)->context[HOST_R22])
#define PT_REGS_R23(r)	((r)->context[HOST_R23])
#define PT_REGS_R24(r)	((r)->context[HOST_R24])
#define PT_REGS_R25(r)	((r)->context[HOST_R25])
#define PT_REGS_R26(r)	((r)->context[HOST_R26])
#define PT_REGS_R27(r)	((r)->context[HOST_R27])
#define PT_REGS_R28(r)	((r)->context[HOST_R28])
#define PT_REGS_R29(r)	((r)->context[HOST_R29])
#define PT_REGS_R30(r)	((r)->context[HOST_R30])
#define PT_REGS_R31(r)	((r)->context[HOST_R31])

#define PT_REGS_NIP(r)	((r)->context[HOST_IP])
#define PT_REGS_MSR(r)	((r)->context[HOST_MSR])
#define PT_REGS_ORIG_R3(r)	((r)->orig_r3)
#define PT_REGS_CTR(r)	((r)->context[HOST_CTR])
#define PT_REGS_LNK(r)	((r)->context[HOST_LR])
#define PT_REGS_XER(r)	((r)->context[HOST_XER])
#define PT_REGS_CCR(r)	((r)->context[HOST_CR])
#define PT_REGS_MQ(r)	0	/* FIXME! */
#define PT_REGS_TRAP(r)	((r)->context[HOST_TRAP])

/* compat */
#define PT_REGS_IP		PT_REGS_NIP
#define PT_REGS_SP(r)	((r)->context[HOST_SP])

static inline unsigned long pt_regs_get_fault_addr(struct pt_regs *regs)
{
	return regs->context[HOST_DAR];
}

static inline void pt_regs_sysemu_convert(struct pt_regs *regs)
{
	/* copy exception regs */
	regs->trap = regs->context[HOST_TRAP] = regs->context[HOST_TRAP] << 8;	/* trap nr */

	/* result must be zero, see transfer_to_handler_cont in head.S */
	regs->result = 0;
}


#define PT_REG(regs, reg)				\
({	unsigned long val; 				\
	if(reg < 32)					\
	{	/* r0 .. r31 */				\
		val = (regs)->context[(reg)];		\
	}						\
	else						\
	{						\
		panic("Bad register in PT_REG : %d\n", reg);	\
		val = -1; 				\
	}						\
	val;						\
})

#define PT_REGS_SET(regs, reg, val)			\
do {							\
	if(reg < 32)					\
	{	/* r0 .. r31 */				\
		(regs)->context[(reg)] = val;		\
	}						\
	else						\
	{						\
		panic("Bad register in PT_REGS_SET : %d\n", reg);  \
	}						\
} while (0)

#define PT_REGS_SET_SYSCALL_RETURN(r, res)		\
do {							\
	(r)->result = (res);				\
	if((res) < 0 && (res) > -_LAST_ERRNO)		\
	{						\
		(r)->context[HOST_CR] |= 0x10000000;	\
		(r)->context[HOST_R3] = -(res);		\
	}						\
	else						\
	{						\
		(r)->context[HOST_R3] = (res);		\
	}						\
} while (0)

#endif
