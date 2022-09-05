/*
 * Copyright (C) 2003 PathScale, Inc.
 * Copyright (C) 2003 - 2007 Jeff Dike (jdike@{addtoit,linux.intel}.com)
 * Licensed under the GPL
 *
 * 2015-01-23, david.engraf@sysgo.com: adapted from UM by using the P4 register
 *              macros and simplify the FPU handling
 *
 * NOTE: Keep in sync with arch/x86/um/signal.c
 */


#include <linux/personality.h>
#include <linux/ptrace.h>
#include <linux/kernel.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>
#include <asm/ucontext.h>

#ifdef CONFIG_X86_32

/*
 * FPU tag word conversions.
 */

static inline unsigned short twd_i387_to_fxsr(unsigned short twd)
{
	unsigned int tmp; /* to avoid 16 bit prefixes in the code */

	/* Transform each pair of bits into 01 (valid) or 00 (empty) */
	tmp = ~twd;
	tmp = (tmp | (tmp>>1)) & 0x5555; /* 0V0V0V0V0V0V0V0V */
	/* and move the valid bits to the lower byte. */
	tmp = (tmp | (tmp >> 1)) & 0x3333; /* 00VV00VV00VV00VV */
	tmp = (tmp | (tmp >> 2)) & 0x0f0f; /* 0000VVVV0000VVVV */
	tmp = (tmp | (tmp >> 4)) & 0x00ff; /* 00000000VVVVVVVV */
	return tmp;
}

static inline unsigned long twd_fxsr_to_i387(struct user_fxsr_struct *fxsave)
{
	struct _fpxreg *st = NULL;
	unsigned long twd = (unsigned long) fxsave->twd;
	unsigned long tag;
	unsigned long ret = 0xffff0000;
	int i;

#define FPREG_ADDR(f, n)	((char *)&(f)->st_space + (n) * 16)

	for (i = 0; i < 8; i++) {
		if (twd & 0x1) {
			st = (struct _fpxreg *) FPREG_ADDR(fxsave, i);

			switch (st->exponent & 0x7fff) {
			case 0x7fff:
				tag = 2;		/* Special */
				break;
			case 0x0000:
				if ( !st->significand[0] &&
				     !st->significand[1] &&
				     !st->significand[2] &&
				     !st->significand[3] ) {
					tag = 1;	/* Zero */
				} else {
					tag = 2;	/* Special */
				}
				break;
			default:
				if (st->significand[3] & 0x8000) {
					tag = 0;	/* Valid */
				} else {
					tag = 2;	/* Special */
				}
				break;
			}
		} else {
			tag = 3;			/* Empty */
		}
		ret |= (tag << (2 * i));
		twd = twd >> 1;
	}
	return ret;
}

#endif

static int copy_sc_from_user(struct pt_regs *regs,
			     struct sigcontext __user *from)
{
	struct sigcontext sc;
	int err;

	/* Always make any pending restarted system calls return -EINTR */
	current_thread_info()->restart_block.fn = do_no_restart_syscall;

	err = copy_from_user(&sc, from, sizeof(sc));
	if (err)
		return err;

#define GETREG(regno, regname) regs->context[HOST_##regno] = sc.regname

#ifdef CONFIG_X86_32
	GETREG(GS, gs);
	GETREG(FS, fs);
	GETREG(ES, es);
	GETREG(DS, ds);
#endif
	GETREG(DI, di);
	GETREG(SI, si);
	GETREG(BP, bp);
	GETREG(SP, sp);
	GETREG(BX, bx);
	GETREG(DX, dx);
	GETREG(CX, cx);
	GETREG(AX, ax);
	GETREG(IP, ip);

#ifdef CONFIG_X86_64
	GETREG(R8, r8);
	GETREG(R9, r9);
	GETREG(R10, r10);
	GETREG(R11, r11);
	GETREG(R12, r12);
	GETREG(R13, r13);
	GETREG(R14, r14);
	GETREG(R15, r15);
#endif

	GETREG(CS, cs);
	GETREG(FLAGS, flags);
#ifdef CONFIG_X86_32
	GETREG(SS, ss);
#endif

#undef GETREG

	if (sc.fpstate) {
		struct _fpstate fp;
		unsigned long *fpregs = (unsigned long *)&fp;
		int i;

		/* check if we can store all PikeOS FPU register */
		BUILD_BUG_ON(sizeof(struct _fpstate) < HOST_FP_SIZE + sizeof(long));

		err = copy_from_user(&fp, sc.fpstate, sizeof(fp));
		if (err)
			return 1;

		for(i = 0; i < sizeof(struct user_i387_struct) / sizeof(long); i++)
			regs->context[HOST_REG_SIZE + i] = fpregs[i];
		regs->context[HOST_USED_FPU] |= 1;
	} else
		regs->context[HOST_USED_FPU] = 0;

	return 0;
}

static int copy_sc_to_user(struct sigcontext __user *to,
			   struct _fpstate __user *to_fp, struct pt_regs *regs,
			   unsigned long mask)
{
	struct sigcontext sc;
	int err;
	memset(&sc, 0, sizeof(struct sigcontext));

#define PUTREG(regno, regname) sc.regname = regs->context[HOST_##regno]

#ifdef CONFIG_X86_32
	PUTREG(GS, gs);
	PUTREG(FS, fs);
	PUTREG(ES, es);
	PUTREG(DS, ds);
#endif
	PUTREG(DI, di);
	PUTREG(SI, si);
	PUTREG(BP, bp);
	PUTREG(SP, sp);
	PUTREG(BX, bx);
	PUTREG(DX, dx);
	PUTREG(CX, cx);
	PUTREG(AX, ax);
#ifdef CONFIG_X86_64
	PUTREG(R8, r8);
	PUTREG(R9, r9);
	PUTREG(R10, r10);
	PUTREG(R11, r11);
	PUTREG(R12, r12);
	PUTREG(R13, r13);
	PUTREG(R14, r14);
	PUTREG(R15, r15);
#endif

	sc.cr2 = regs->context[HOST_VECTOR];
	sc.err = pt_regs_get_ex_pf(regs) & HOST_PF_WRITE ? 2 : 0;
	sc.trapno = regs->trap;
	PUTREG(IP, ip);
	PUTREG(CS, cs);
	PUTREG(FLAGS, flags);
#ifdef CONFIG_X86_32
	PUTREG(SP, sp_at_signal);
	PUTREG(SS, ss);
#endif
#undef PUTREG
	sc.oldmask = mask;

	if(regs->context[HOST_USED_FPU] & 1) {
		/* save fpu context */
		int i;
		struct _fpstate fp;
		unsigned long *fpregs = (unsigned long *)&fp;

		for(i = 0; i < HOST_FP_SIZE; i++)
			fpregs[i] = regs->context[HOST_REG_SIZE + i];

		err = copy_to_user(to_fp, &fp, sizeof(fp));
		sc.fpstate = to_fp;
	} else
		sc.fpstate = NULL;

	err = copy_to_user(to, &sc, sizeof(struct sigcontext));
	if (err)
		return 1;

	return 0;
}

#ifdef CONFIG_X86_32
static int copy_ucontext_to_user(struct ucontext __user *uc,
				 struct _fpstate __user *fp, sigset_t *set,
				 unsigned long sp)
{
	int err = 0;

	err |= __save_altstack(&uc->uc_stack, sp);
	err |= copy_sc_to_user(&uc->uc_mcontext, fp, &current->thread.regs, 0);
	err |= copy_to_user(&uc->uc_sigmask, set, sizeof(*set));
	return err;
}

struct sigframe
{
	char __user *pretcode;
	int sig;
	struct sigcontext sc;
	struct _fpstate fpstate;
	unsigned long extramask[_NSIG_WORDS-1];
	char retcode[8];
};

struct rt_sigframe
{
	char __user *pretcode;
	int sig;
	struct siginfo __user *pinfo;
	void __user *puc;
	struct siginfo info;
	struct ucontext uc;
	struct _fpstate fpstate;
	char retcode[8];
};

int setup_signal_stack_sc(unsigned long stack_top, int sig,
			  struct k_sigaction *ka, struct pt_regs *regs,
			  sigset_t *mask)
{
	struct sigframe __user *frame;
	void __user *restorer;
	int err = 0;

	/* This is the same calculation as i386 - ((sp + 4) & 15) == 0 */
	stack_top = ((stack_top + 4) & -16UL) - 4;
	frame = (struct sigframe __user *) stack_top - 1;
	if (!access_ok(VERIFY_WRITE, frame, sizeof(*frame)))
		return 1;

	restorer = frame->retcode;
	if (ka->sa.sa_flags & SA_RESTORER)
		restorer = ka->sa.sa_restorer;

	err |= __put_user(restorer, &frame->pretcode);
	err |= __put_user(sig, &frame->sig);
	err |= copy_sc_to_user(&frame->sc, &frame->fpstate, regs, mask->sig[0]);
	if (_NSIG_WORDS > 1)
		err |= __copy_to_user(&frame->extramask, &mask->sig[1],
				      sizeof(frame->extramask));

	/*
	 * This is popl %eax ; movl $,%eax ; int $0x80
	 *
	 * WE DO NOT USE IT ANY MORE! It's only left here for historical
	 * reasons and because gdb uses it as a signature to notice
	 * signal handler stack frames.
	 */
	err |= __put_user(0xb858, (short __user *)(frame->retcode+0));
	err |= __put_user(__NR_sigreturn, (int __user *)(frame->retcode+2));
	err |= __put_user(0x80cd, (short __user *)(frame->retcode+6));

	if (err)
		return err;

	PT_REGS_SP(regs) = (unsigned long) frame;
	PT_REGS_IP(regs) = (unsigned long) ka->sa.sa_handler;
	PT_REGS_AX(regs) = (unsigned long) sig;
	PT_REGS_DX(regs) = (unsigned long) 0;
	PT_REGS_CX(regs) = (unsigned long) 0;
	return 0;
}

int setup_signal_stack_si(unsigned long stack_top, int sig,
			  struct k_sigaction *ka, struct pt_regs *regs,
			  siginfo_t *info, sigset_t *mask)
{
	struct rt_sigframe __user *frame;
	void __user *restorer;
	int err = 0;

	stack_top &= -8UL;
	frame = (struct rt_sigframe __user *) stack_top - 1;
	if (!access_ok(VERIFY_WRITE, frame, sizeof(*frame)))
		return 1;

	restorer = frame->retcode;
	if (ka->sa.sa_flags & SA_RESTORER)
		restorer = ka->sa.sa_restorer;

	err |= __put_user(restorer, &frame->pretcode);
	err |= __put_user(sig, &frame->sig);
	err |= __put_user(&frame->info, &frame->pinfo);
	err |= __put_user(&frame->uc, &frame->puc);
	err |= copy_siginfo_to_user(&frame->info, info);
	err |= copy_ucontext_to_user(&frame->uc, &frame->fpstate, mask,
					PT_REGS_SP(regs));

	/*
	 * This is movl $,%eax ; int $0x80
	 *
	 * WE DO NOT USE IT ANY MORE! It's only left here for historical
	 * reasons and because gdb uses it as a signature to notice
	 * signal handler stack frames.
	 */
	err |= __put_user(0xb8, (char __user *)(frame->retcode+0));
	err |= __put_user(__NR_rt_sigreturn, (int __user *)(frame->retcode+1));
	err |= __put_user(0x80cd, (short __user *)(frame->retcode+5));

	if (err)
		return err;

	PT_REGS_SP(regs) = (unsigned long) frame;
	PT_REGS_IP(regs) = (unsigned long) ka->sa.sa_handler;
	PT_REGS_AX(regs) = (unsigned long) sig;
	PT_REGS_DX(regs) = (unsigned long) &frame->info;
	PT_REGS_CX(regs) = (unsigned long) &frame->uc;
	return 0;
}

long sys_sigreturn(void)
{
	unsigned long sp = PT_REGS_SP(&current->thread.regs);
	struct sigframe __user *frame = (struct sigframe __user *)(sp - 8);
	sigset_t set;
	struct sigcontext __user *sc = &frame->sc;
	unsigned long __user *oldmask = &sc->oldmask;
	unsigned long __user *extramask = frame->extramask;
	int sig_size = (_NSIG_WORDS - 1) * sizeof(unsigned long);

	if (copy_from_user(&set.sig[0], oldmask, sizeof(set.sig[0])) ||
	    copy_from_user(&set.sig[1], extramask, sig_size))
		goto segfault;

	set_current_blocked(&set);

	if (copy_sc_from_user(&current->thread.regs, sc))
		goto segfault;

	/* Avoid ERESTART handling */
	PT_REGS_SYSCALL_NR(&current->thread.regs) = -1;
	return PT_REGS_SYSCALL_RET(&current->thread.regs);

 segfault:
	force_sig(SIGSEGV, current);
	return 0;
}

#else

struct rt_sigframe
{
	char __user *pretcode;
	struct ucontext uc;
	struct siginfo info;
	struct _fpstate fpstate;
};

int setup_signal_stack_si(unsigned long stack_top, int sig,
			  struct k_sigaction *ka, struct pt_regs * regs,
			  siginfo_t *info, sigset_t *set)
{
	struct rt_sigframe __user *frame;
	int err = 0;

	frame = (struct rt_sigframe __user *)
		round_down(stack_top - sizeof(struct rt_sigframe), 16);
	/* Subtract 128 for a red zone and 8 for proper alignment */
	frame = (struct rt_sigframe __user *) ((unsigned long) frame - 128 - 8);

	if (!access_ok(VERIFY_WRITE, frame, sizeof(*frame)))
		goto out;

	if (ka->sa.sa_flags & SA_SIGINFO) {
		err |= copy_siginfo_to_user(&frame->info, info);
		if (err)
			goto out;
	}

	/* Create the ucontext.  */
	err |= __put_user(0, &frame->uc.uc_flags);
	err |= __put_user(0, &frame->uc.uc_link);
	err |= __save_altstack(&frame->uc.uc_stack, PT_REGS_SP(regs));
	err |= copy_sc_to_user(&frame->uc.uc_mcontext, &frame->fpstate, regs,
			       set->sig[0]);
	err |= __put_user(&frame->fpstate, &frame->uc.uc_mcontext.fpstate);
	if (sizeof(*set) == 16) {
		err |= __put_user(set->sig[0], &frame->uc.uc_sigmask.sig[0]);
		err |= __put_user(set->sig[1], &frame->uc.uc_sigmask.sig[1]);
	}
	else
		err |= __copy_to_user(&frame->uc.uc_sigmask, set,
				      sizeof(*set));

	/*
	 * Set up to return from userspace.  If provided, use a stub
	 * already in userspace.
	 */
	/* x86-64 should always use SA_RESTORER. */
	if (ka->sa.sa_flags & SA_RESTORER)
		err |= __put_user(ka->sa.sa_restorer, &frame->pretcode);
	else
		/* could use a vstub here */
		return err;

	if (err)
		return err;

	/* Set up registers for signal handler */
	{
		struct exec_domain *ed = current_thread_info()->exec_domain;
		if (unlikely(ed && ed->signal_invmap && sig < 32))
			sig = ed->signal_invmap[sig];
	}

	PT_REGS_SP(regs) = (unsigned long) frame;
	PT_REGS_DI(regs) = sig;
	/* In case the signal handler was declared without prototypes */
	PT_REGS_AX(regs) = 0;

	/*
	 * This also works for non SA_SIGINFO handlers because they expect the
	 * next argument after the signal number on the stack.
	 */
	PT_REGS_SI(regs) = (unsigned long) &frame->info;
	PT_REGS_DX(regs) = (unsigned long) &frame->uc;
	PT_REGS_IP(regs) = (unsigned long) ka->sa.sa_handler;
 out:
	return err;
}
#endif

long sys_rt_sigreturn(void)
{
	unsigned long sp = PT_REGS_SP(&current->thread.regs);
	struct rt_sigframe __user *frame =
		(struct rt_sigframe __user *)(sp - sizeof(long));
	struct ucontext __user *uc = &frame->uc;
	sigset_t set;

	if (copy_from_user(&set, &uc->uc_sigmask, sizeof(set)))
		goto segfault;

	set_current_blocked(&set);

	if (copy_sc_from_user(&current->thread.regs, &uc->uc_mcontext))
		goto segfault;

	/* Avoid ERESTART handling */
	PT_REGS_SYSCALL_NR(&current->thread.regs) = -1;
	return PT_REGS_SYSCALL_RET(&current->thread.regs);

 segfault:
	force_sig(SIGSEGV, current);
	return 0;
}
