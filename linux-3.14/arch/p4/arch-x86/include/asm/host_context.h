/*
 * linux/arch/p4/arch-x86/include/asm/host_context.h
 *
 * This describes the host-context of P4/x86
 *
 * NOTE: Keep in sync with p4/kernel/arch/x86/include/kernel/p4regs.h
 *
 * 02. april 2004, azu@sysgo.de
 */


#ifndef __HOST_CONTEXT_I386_H_
#define __HOST_CONTEXT_I386_H_

#ifdef CONFIG_X86_32

#define HOST_AUDIT_ARCH AUDIT_ARCH_I386

/* exception frame */
/* regs */
#define HOST_EAX	0
#define HOST_ECX	1
#define HOST_EDX	2
#define HOST_EBX	3
#define HOST_EBP	4
#define HOST_ESI	5
#define HOST_EDI	6
#define HOST_ES		7
#define HOST_DS		8
#define HOST_VECTOR	9
#define HOST_ERROR	10
#define HOST_EIP	11
#define HOST_CS		12
#define HOST_EFLAGS	13
#define HOST_ESP	14
#define HOST_SS		15
#define HOST_TLS0A	16
#define HOST_TLS0B	17
#define HOST_TLS1A	18
#define HOST_TLS1B	19
#define HOST_FS		20
#define HOST_GS		21
#define HOST_EX_CODE	22
#define HOST_USED_FPU	23
/* fp-regs */
/* FNSAVE registers */
#define HOST_FCW	24
#define HOST_FSW	25
#define HOST_FTW	26
#define HOST_FIP	27
#define HOST_FCS	28
#define HOST_FOO	29
#define HOST_FOS	30
#define HOST_FST	31
#define HOST_FST_LAST	50
/* FXSAVE registers */
#define HOST_X_FCW_FSW	24
#define HOST_X_FTW_FOP	25
#define HOST_MXCSR	30

/* sizes */
#define HOST_FRAME_SIZE	152
#define HOST_REG_SIZE	24	/* integer context */
#define HOST_FP_SIZE	128	/* i387 FPU+SSE context */
#define HOST_XFP_SIZE	0	/* no xfp context */

/* common names */
#define HOST_IP		HOST_EIP
#define HOST_SP		HOST_ESP

#define HOST_AX		HOST_EAX
#define HOST_BX		HOST_EBX
#define HOST_CX		HOST_ECX
#define HOST_DX		HOST_EDX
#define HOST_SI		HOST_ESI
#define HOST_DI		HOST_EDI
#define HOST_BP		HOST_EBP
#define HOST_FLAGS	HOST_EFLAGS

#else /* CONFIG_X86_32 */

#define HOST_AUDIT_ARCH AUDIT_ARCH_X86_64

#define HOST_RDI       0
#define HOST_RSI       1
#define HOST_RDX       2
#define HOST_R10       3
#define HOST_R8        4
#define HOST_R9        5
#define HOST_RCX       6
#define HOST_R11       7
#define HOST_RAX       8

/* non-volatile registers */
#define HOST_RBX       9
#define HOST_RBP       10
#define HOST_R12       11
#define HOST_R13       12
#define HOST_R14       13
#define HOST_R15       14

/* exception related */
#define HOST_VECTOR    15       /* vector (ID) of exception                */
#define HOST_ERROR     16      /* some exceptions save an error code here */
#define HOST_RIP       17
#define HOST_CS        18
#define HOST_RFLAGS    19
#define HOST_RSP       20
#define HOST_SS        21

/* default */
#define HOST_FS_BASE   22
#define HOST_GS_BASE   23
#define HOST_EX_CODE   24      /* generic exception code */
#define HOST_USED_FPU  25      /* see below for special bits */

/* FNSAVE registers */
/* ignore, SSE always available */

/* FXSAVE registers */
#define HOST_X_FCW_FSW	26
#define HOST_MXCSR	29

#define HOST_FRAME_SIZE 90
#define HOST_REG_SIZE	26
#define HOST_FP_SIZE	64

/* for exceptions, this is the fault address */
#define HOST_FAULT     HOST_VECTOR

/*
 * define generic names for SP and PC
 */
#define HOST_SP    HOST_RSP
#define HOST_IP    HOST_RIP

#define HOST_AX		HOST_RAX
#define HOST_BX		HOST_RBX
#define HOST_CX		HOST_RCX
#define HOST_DX		HOST_RDX

#define HOST_DI		HOST_RDI
#define HOST_SI		HOST_RSI
#define HOST_BP		HOST_RBP
#define HOST_FLAGS	HOST_RFLAGS

#endif

/* selectors */
#ifdef CONFIG_X86_32
#define HOST_USER_CS	0x23
#define HOST_USER_DS	0x2b
#else
#define HOST_USER_CS	0x33
#define HOST_USER_DS	0x2b
#endif

/* TLS */
#define GDT_ENTRY_TLS_ENTRIES	2
#define GDT_ENTRY_TLS_MIN	6
#define GDT_ENTRY_TLS_MAX	(GDT_ENTRY_TLS_MIN + GDT_ENTRY_TLS_ENTRIES - 1)


struct pt_regs
{
	unsigned long context[HOST_FRAME_SIZE];

	/* you may reorder these */
	unsigned long trap;		/* trapnr: is segv fixable? 13 || 14 */
	unsigned long orig_ax;		/* syscall number */
	int is_user;
};

#ifdef CONFIG_X86_32

#define EMPTY_REGS { \
	{0, 0, 0, 0, 0, 0, 0, HOST_USER_DS, HOST_USER_DS,	/* eax - ds,es */	\
	0, 0, 0, HOST_USER_CS, 0, 0, HOST_USER_DS, /* vector - ss */	\
	0, 0, 0, 0, 0, 0, 0, /* TLS area - EX_CODE */ \
	0,	/* used_fp: disable FPU */	\
	}, \
	}

#else /* CONFIG_X86_32 */

#define EMPTY_REGS {					\
	{						\
		[HOST_RDI ... HOST_RIP] = 0,		\
		HOST_USER_CS, 0, 0, HOST_USER_DS,	/* cs - ss */ \
		[HOST_FS_BASE ... HOST_USED_FPU] = 0	\
	},						\
	}

#endif /* CONFIG_X86_32 */

#define PT_REGS_IP(r)	((r)->context[HOST_IP])
#define PT_REGS_SP(r)	((r)->context[HOST_SP])
#define PT_REGS_FLAGS(r)((r)->context[HOST_FLAGS])
#define PT_REGS_AX(r)	((r)->context[HOST_AX])
#define PT_REGS_BX(r)	((r)->context[HOST_BX])
#define PT_REGS_CX(r)	((r)->context[HOST_CX])
#define PT_REGS_DX(r)	((r)->context[HOST_DX])
#define PT_REGS_SI(r)	((r)->context[HOST_SI])
#define PT_REGS_DI(r)	((r)->context[HOST_DI])
#define PT_REGS_BP(r)	((r)->context[HOST_BP])
#define PT_REGS_ORIG_AX(r)	((r)->orig_ax)
#define PT_REGS_CS(r)	((r)->context[HOST_CS])
#define PT_REGS_DS(r)	((r)->context[HOST_DS])
#define PT_REGS_ES(r)	((r)->context[HOST_ES])
#define PT_REGS_SS(r)	((r)->context[HOST_SS])

#ifdef CONFIG_X86_64

#define PT_REGS_R8(r)	((r)->context[HOST_R8])
#define PT_REGS_R9(r)	((r)->context[HOST_R9])
#define PT_REGS_R10(r)	((r)->context[HOST_R10])
#define PT_REGS_R11(r)	((r)->context[HOST_R11])
#define PT_REGS_R12(r)	((r)->context[HOST_R12])
#define PT_REGS_R13(r)	((r)->context[HOST_R13])
#define PT_REGS_R14(r)	((r)->context[HOST_R14])
#define PT_REGS_R15(r)	((r)->context[HOST_R15])
#define PT_REGS_FS_BASE(r)	((r)->context[HOST_FS_BASE])

#endif /* CONFIG_X86_64 */

static inline unsigned long pt_regs_get_fault_addr(struct pt_regs *regs)
{
	/* for pagefaults, HOST_VECTOR contains the fault address */
	return regs->context[HOST_VECTOR];
}

/* context conversion macros */
/* exception to linux frame (m: first message word, r: pt_regs */
static inline void pt_regs_sysemu_convert(struct pt_regs *regs)
{
	unsigned int m;

	m = HOST_GET_EX_CODE(regs->context[HOST_EX_CODE]);

	/* copy exception regs */
	regs->trap  = regs->context[HOST_VECTOR]; /* contains intr vector */
	/* special handling for P4_TRAP_SYS */
#ifdef CONFIG_X86_32
	if (m == HOST_TRAP_SYS) {
		regs->trap += 0x20;
		if (regs->trap != 0x80) {
			/* convert everything not 0x80 into a GPF */
			regs->context[HOST_EX_CODE] = HOST_SET_EX_CODE(HOST_TRAP_BUS);
			regs->context[HOST_ERROR] = (regs->trap << 3) | 2;
			regs->trap = 13;	/* set to GPF */
			regs->context[HOST_IP] -= 2; /* trap -> fault conversion */
		}
	}
#endif
	/* ... if they are not handled, we must take a deeper look at GPFs */
	/* the microkernels generates P4_TRAP_BUS, vector=13 (GPF) and */
	/* the int-number is encoded as 8*n+2 in the error-register. */
	/* so, 0x402 is int 0x80 -> translate it to P4_TRAP_SYS */
	if ((m == HOST_TRAP_BUS) &&
	    (regs->context[HOST_VECTOR] == 13) &&
	    (regs->context[HOST_ERROR] == 0x402)) {
		regs->context[HOST_EX_CODE] = HOST_SET_EX_CODE(HOST_TRAP_SYS);
		regs->trap = 0x80;	/* set to 0x80 */
		regs->context[HOST_IP] += 2; /* fault -> trap conversion */
	}
	/* special handling on pagefaults: we do this in case we don't use a pager */
	if (m == HOST_TRAP_SEG) {
		regs->trap     = 14; /* fix to PF */
	}
	/* set orig_ax properly */
	if (HOST_GET_EX_CODE(regs->context[HOST_EX_CODE]) == HOST_TRAP_SYS) {
		/* it's a linux syscall */
		regs->orig_ax = regs->context[HOST_AX]; /* save "ORIG_AX" */
	} else {
		regs->orig_ax = -1;	/* set "ORIG_AX" to -1 for other exceptions */
	}
}

/* syscall macros */

struct syscall_args {
	unsigned long args[6];
};

#ifdef CONFIG_X86_32

#define SYSCALL_ARGS(r) ((struct syscall_args) \
                        { .args = { (r)->context[HOST_BX], \
                                    (r)->context[HOST_CX], \
                                    (r)->context[HOST_DX], \
                                    (r)->context[HOST_SI], \
                                    (r)->context[HOST_DI], \
                                    (r)->context[HOST_BP] } } )

#define PT_REG(regs, reg) \
	({	unsigned long val; \
		switch(reg){ \
		case EIP: val = (regs)->context[HOST_EIP]; break; \
		case UESP: val = (regs)->context[HOST_ESP]; break; \
		case EFL: val = (regs)->context[HOST_FLAGS]; break; \
		case EAX: val = (regs)->context[HOST_EAX]; break; \
		case EBX: val = (regs)->context[HOST_EBX]; break; \
		case ECX: val = (regs)->context[HOST_ECX]; break; \
		case EDX: val = (regs)->context[HOST_EDX]; break; \
		case ESI: val = (regs)->context[HOST_ESI]; break; \
		case EDI: val = (regs)->context[HOST_EDI]; break; \
		case EBP: val = (regs)->context[HOST_EBP]; break; \
		case ORIG_EAX: val = (regs)->orig_ax; break; \
		case CS: val = (regs)->context[HOST_CS]; break; \
		case DS: val = (regs)->context[HOST_DS]; break; \
		case ES: val = (regs)->context[HOST_ES]; break; \
		case FS: val = (regs)->context[HOST_FS]; break; \
		case GS: val = (regs)->context[HOST_GS]; break; \
		case SS: val = (regs)->context[HOST_SS]; break; \
		default:  \
			panic("Bad register in PT_REG: %d\n", reg);  \
			val = -1; \
		} \
	        val; \
	})

#define PT_REGS_SET(regs, reg, val) \
	do { \
		switch(reg){ \
		case EIP: (regs)->context[HOST_EIP] = val; break; \
		case UESP: (regs)->context[HOST_ESP] = val; break; \
		case EFL: (regs)->context[HOST_FLAGS] = val; break; \
		case EAX: (regs)->context[HOST_AX] = val; break; \
		case EBX: (regs)->context[HOST_EBX] = val; break; \
		case ECX: (regs)->context[HOST_ECX] = val; break; \
		case EDX: (regs)->context[HOST_EDX] = val; break; \
		case ESI: (regs)->context[HOST_ESI] = val; break; \
		case EDI: (regs)->context[HOST_EDI] = val; break; \
		case EBP: (regs)->context[HOST_EBP] = val; break; \
		case ORIG_EAX: (regs)->orig_ax = val; break; \
		case CS: (regs)->context[HOST_CS] = val; break; \
		case DS: (regs)->context[HOST_DS] = val; break; \
		case ES: (regs)->context[HOST_ES] = val; break; \
		case FS: (regs)->context[HOST_FS] = val; break; \
		case GS: (regs)->context[HOST_GS] = val; break; \
		case SS: (regs)->context[HOST_SS] = val; break; \
		default :  \
			panic("Bad register in PT_REGS_SET : %d\n", reg);  \
			break; \
		} \
	} while (0)

#else

#ifndef FS_BASE
/* These aren't defined in ptrace.h, but exist in struct user_regs_struct,
 * which is what x86_64 ptrace actually uses.
 */
#define FS_BASE (21 * sizeof(long))
#define GS_BASE (22 * sizeof(long))
#define DS (23 * sizeof(long))
#define ES (24 * sizeof(long))
#define FS (25 * sizeof(long))
#define GS (26 * sizeof(long))
#endif

#define UPT_SYSCALL_ARG1(r) (r)->context[HOST_DI]
#define UPT_SYSCALL_ARG2(r) (r)->context[HOST_SI]
#define UPT_SYSCALL_ARG3(r) (r)->context[HOST_DX]
#define UPT_SYSCALL_ARG4(r) (r)->context[HOST_R10]
#define UPT_SYSCALL_ARG5(r) (r)->context[HOST_R8]
#define UPT_SYSCALL_ARG6(r) (r)->context[HOST_R9]

#define FLAG_MASK 0x44dd5UL

#define PT_REG(regs, reg) \
	({	unsigned long val; \
		switch(reg){ \
		case R8:  val = (regs)->context[HOST_R8]; break; \
		case R9:  val = (regs)->context[HOST_R9]; break; \
		case R10: val = (regs)->context[HOST_R10]; break; \
		case R11: val = (regs)->context[HOST_R11]; break; \
		case R12: val = (regs)->context[HOST_R12]; break; \
		case R13: val = (regs)->context[HOST_R13]; break; \
		case R14: val = (regs)->context[HOST_R14]; break; \
		case R15: val = (regs)->context[HOST_R15]; break; \
		case RIP: val = (regs)->context[HOST_RIP]; break; \
		case RSP: val = (regs)->context[HOST_RSP]; break; \
		case RAX: val = (regs)->context[HOST_RAX]; break; \
		case RBX: val = (regs)->context[HOST_RBX]; break; \
		case RCX: val = (regs)->context[HOST_RCX]; break; \
		case RDX: val = (regs)->context[HOST_RDX]; break; \
		case RSI: val = (regs)->context[HOST_RSI]; break; \
		case RDI: val = (regs)->context[HOST_RDI]; break; \
		case RBP: val = (regs)->context[HOST_RBP]; break; \
		case ORIG_RAX: val = (regs)->orig_ax; break; \
		case EFLAGS: val = (regs)->context[HOST_FLAGS]; break; \
		case FS_BASE: val = (regs)->context[HOST_FS_BASE]; break; \
		case GS_BASE: val = (regs)->context[HOST_GS_BASE]; break; \
		case SS: val = (regs)->context[HOST_SS] & 0xffff; break; \
		case CS: val = (regs)->context[HOST_CS] & 0xffff; break; \
		default:  \
			panic("Bad register in PT_REG: %d\n", reg);  \
			val = -1; \
		} \
	        val; \
	})

#define PT_REGS_SET(regs, reg, val) \
	do { \
		switch(reg){ \
		case R8:  (regs)->context[HOST_R8] = val; break; \
		case R9:  (regs)->context[HOST_R9] = val; break; \
		case R10: (regs)->context[HOST_R10] = val; break; \
		case R11: (regs)->context[HOST_R11] = val; break; \
		case R12: (regs)->context[HOST_R12] = val; break; \
		case R13: (regs)->context[HOST_R13] = val; break; \
		case R14: (regs)->context[HOST_R14] = val; break; \
		case R15: (regs)->context[HOST_R15] = val; break; \
		case RIP: (regs)->context[HOST_RIP] = val; break; \
		case RSP: (regs)->context[HOST_RSP] = val; break; \
		case RAX: (regs)->context[HOST_RAX] = val; break; \
		case RBX: (regs)->context[HOST_RBX] = val; break; \
		case RCX: (regs)->context[HOST_RCX] = val; break; \
		case RDX: (regs)->context[HOST_RDX] = val; break; \
		case RSI: (regs)->context[HOST_RSI] = val; break; \
		case RDI: (regs)->context[HOST_RDI] = val; break; \
		case RBP: (regs)->context[HOST_RBP] = val; break; \
		case ORIG_RAX: (regs)->orig_ax = val; break; \
		case SS:  (regs)->context[HOST_SS] = val; break; \
		case CS:  (regs)->context[HOST_CS] = val & 0xffff; break; \
		case FS_BASE: (regs)->context[HOST_FS_BASE] = val; break; \
		case GS_BASE: (regs)->context[HOST_GS_BASE] = val; break; \
		case EFLAGS: (regs)->context[HOST_FLAGS] |= val & FLAG_MASK; break; \
		default :  \
			panic("Bad register in PT_REGS_SET : %d\n", reg);  \
			break; \
		} \
	} while (0)

#endif

#define PT_REGS_ORIG_SYSCALL(r) ((r)->context[HOST_AX])
#define PT_REGS_SYSCALL_RET(r)  ((r)->context[HOST_AX])

#define PT_REGS_SET_SYSCALL_RETURN(r, res)	(r)->context[HOST_AX] = (res)
#define PT_REGS_GET_SYSCALL_RETURN(r)	((r)->context[HOST_AX])

#define PT_REGS_RESTART_SYSCALL(r) (PT_REGS_IP(r) -= 2)

#define PT_REGS_SYSCALL_NR(r)	(r)->orig_ax

#ifdef CONFIG_X86_32
/* frame size regs, taken from linux/arch/p4/include/sysdep-i386/ptrace_user.h */
#define FRAME_SIZE_OFFSET (FRAME_SIZE * sizeof(unsigned long))

#else

#define FRAME_SIZE_OFFSET (FRAME_SIZE)

#endif

#endif
