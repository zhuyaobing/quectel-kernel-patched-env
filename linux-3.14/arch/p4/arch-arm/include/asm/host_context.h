/*
 * linux/arch/p4/include/arch-arm/host_context.h
 *
 * This describes the host-context of P4/arm
 *
 * NOTE: Keep in sync with p4/arch/arm/include/kernel/p4reg.h !!!
 *
 * 2009-09-14, David Engraf <den@sysgo.com>
 */


#ifndef __HOST_CONTEXT_ARM_H_
#define __HOST_CONTEXT_ARM_H_

#include <asm/pikeos_version.h>

/*
 * exception frame
 * keep these registers in sync with p4/arch/arm/include/kernel/p4regoffset.h
 */
#define HOST_R0		0
#define HOST_R1		1
#define HOST_R2		2
#define HOST_R3		3
#define HOST_R4		4
#define HOST_R5		5
#define HOST_R6		6
#define HOST_R7		7
#define HOST_R8		8
#define HOST_R9		9
#define HOST_R10		10
#define HOST_R11		11
#define HOST_R12		12
#define HOST_R13		13
#define HOST_R14		14
#define HOST_R15		15

#define HOST_CPSR		16
#define HOST_TLS		17
#define HOST_FAULT		18
#define	HOST_EX_CODE	19

#define HOST_FPEXC		20
#define HOST_FPSCR		21
#define HOST_FPINST		22
#define HOST_FPINST2	23
#define HOST_FPREGS		24

#define HOST_EPC		HOST_R15
#define HOST_SP			HOST_R13
#define HOST_ARCH1		HOST_CPSR
#define HOST_ARCH2		HOST_R14

#define HOST_LR			HOST_R14
#define HOST_IP			HOST_R12
#define HOST_FP			HOST_R11

/* sizes */
#define HOST_FRAME_SIZE	88 /* must be == NCLIENTREGS! */

struct pt_regs
{
	unsigned long context[HOST_FRAME_SIZE];
	unsigned long orig_r0;		/* syscall restart */
	int is_user;
};

#define EMPTY_REGS { \
	{0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /* r0-r15 */ \
	 0x10,0,0,0, /* cpsr, tls, fault, ex_code */ \
	 0, 0, 0, 0, /* fpexc, fpscr, fpinst, fpinst2 */ \
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* fpregs */ \
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 \
	}, \
	0, /* orig_r0 */ \
	0, /* is_user */ \
}

/* theses access macros are used commonly across archs */
#define PT_REGS_IP(r)	((r)->context[HOST_EPC])
#define PT_REGS_SP(r)	((r)->context[HOST_SP])

static inline unsigned long pt_regs_get_fault_addr(struct pt_regs *regs)
{
	return regs->context[HOST_FAULT];
}

static inline unsigned int pt_regs_is_alignment_fault(struct pt_regs *regs)
{
	return (pt_regs_get_ex_type(regs) == HOST_TRAP_BUS &&
	    regs->context[HOST_EX_CODE] &
#if PIKEOS_API_VERSION < 0x30006
		    (0xffff));
#else
		    (HOST_PF_ALIGN));
#endif
}

/*
 * fill pt_regs according to what P4 passed us in pt_regs->context
 */
static inline void pt_regs_sysemu_convert(struct pt_regs *regs)
{
	unsigned int m;

	m = HOST_GET_EX_CODE(regs->context[HOST_EX_CODE]);

	/* special handling on syscall */
	if (m == HOST_TRAP_SYS) {
		/* save r0 for syscall restarting (see signal.c) */
		regs->orig_r0 = regs->context[HOST_R0];
		/* don't re-execute "syscall" */
		if (regs->context[HOST_CPSR] & 0x20)
			PT_REGS_IP(regs) = PT_REGS_IP(regs) + 2;	/* Thumb! */
		else
			PT_REGS_IP(regs) = PT_REGS_IP(regs) + 4;
	}
}

/* syscall macros */

struct syscall_args {
	unsigned long args[6];
};

#define SYSCALL_ARGS(r) ((struct syscall_args) \
                        { .args = { (r)->context[0], \
                                    (r)->context[1], \
                                    (r)->context[2], \
                                    (r)->context[3], \
                                    (r)->context[4], \
                                    (r)->context[5] } } )

#define PT_REGS_SET_SYSCALL_RETURN(r, res)	(r)->context[HOST_R0] = (res)
#define PT_REGS_SYSCALL_NR(r)				(r)->context[HOST_R7]

#endif
