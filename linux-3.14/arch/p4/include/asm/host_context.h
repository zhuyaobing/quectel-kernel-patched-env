#ifndef __P4_HOST_CONTEXT_H
#define __P4_HOST_CONTEXT_H

/*
 * keep these trapcodes in sync with p4/lib/include/p4exception.h
 */
typedef enum {
	HOST_TRAP_NOT,	/* lthread_exreg()ed in while running in userspace */
	HOST_TRAP_ILL,    /* illegal instruction      */
	HOST_TRAP_BRK,    /* breakpoint               */
	HOST_TRAP_ARI,    /* arithmetic overflow      */
	HOST_TRAP_FP,     /* FP coprocessor exception */
	HOST_TRAP_BUS,    /* bus error                */
	HOST_TRAP_SEG,    /* segment violation        */
	HOST_TRAP_TRP,    /* trap exception           */
	HOST_TRAP_SYS,    /* (non-P4) system call     */
	HOST_TRAP_FP_UNAVAIL,   /* FPU code executed, but FPU disabled */
	HOST_TRAP_CTXT,   /* corrupted register context (internal error) */
	/*P4_TRAP_TASK_ACTIVATED */ /* cannot happen */
	/*P4_TRAP_TASK_ZOMBIE */    /* cannot happen */
} uk_trapcode_t;

#define HOST_GET_EX_CODE(x) ((x) >> 16)
#define HOST_SET_EX_CODE(x) ((x) << 16)
#define HOST_GET_EX_PF(x)   ((x) & 0xffff)

/* pagefault flags */
#define HOST_PF_NOMAP (1 << 0)
#define HOST_PF_READ  (1 << 1)
#define HOST_PF_WRITE (1 << 2)
#define HOST_PF_EXEC  (1 << 3)
/* NOTE: P4_API_VERSION must be >= 0x30006 */
#define HOST_PF_ALIGN (1 << 4)

struct pt_regs;

static inline unsigned int pt_regs_get_ex_type(struct pt_regs *regs);
static inline unsigned int pt_regs_get_ex_pf(struct pt_regs *regs);

#include <arch-override/asm/host_context.h>

static inline unsigned int pt_regs_get_ex_type(struct pt_regs *regs)
{
	return HOST_GET_EX_CODE(regs->context[HOST_EX_CODE]);
}

static inline unsigned int pt_regs_get_ex_pf(struct pt_regs *regs)
{
	return HOST_GET_EX_PF(regs->context[HOST_EX_CODE]);
}

#endif
