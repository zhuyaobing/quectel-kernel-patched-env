/*
 * hacked version of linux/arch/powerpc/kernel/signal_32.c
 * azu@sysgo.de, 20061024:
 */

/*
 * Signal handling for 32bit PPC and 32bit tasks on 64bit PPC
 *
 *  PowerPC version
 *    Copyright (C) 1995-1996 Gary Thomas (gdt@linuxppc.org)
 * Copyright (C) 2001 IBM
 * Copyright (C) 1997,1998 Jakub Jelinek (jj@sunsite.mff.cuni.cz)
 * Copyright (C) 1997 David S. Miller (davem@caip.rutgers.edu)
 *
 *  Derived from "arch/i386/kernel/signal.c"
 *    Copyright (C) 1991, 1992 Linus Torvalds
 *    1997-11-28  Modified for POSIX.1b signals by Richard Henderson
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */

#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/kernel.h>
#include <linux/signal.h>
#include <linux/errno.h>
#include <linux/elf.h>
#include <linux/wait.h>
#include <linux/ptrace.h>
#include <linux/unistd.h>
#include <linux/stddef.h>
#include <linux/tty.h>
#include <linux/binfmts.h>
#include <linux/suspend.h>
#include <linux/freezer.h>

#include <asm/uaccess.h>
#include <asm/cacheflush.h>
#include <asm/sigcontext.h>
#include <asm/ucontext.h>
#include <asm/pgtable.h>
#include <asm/reg.h>

#undef DEBUG_SIG

/*
 * These are the flags in the MSR that the user is allowed to change
 * by modifying the saved value of the MSR on the stack.  SE and BE
 * should not be in this list since gdb may want to change these.  I.e,
 * you should be able to step out of a signal handler to see what
 * instruction executes next after the signal handler completes.
 * Alternately, if you stepped into a signal handler, you should be
 * able to continue 'til the next breakpoint from within the signal
 * handler, even if the handler returns.
 */
#define MSR_USERCHANGE	(MSR_FE0 | MSR_FE1)

#define _BLOCKABLE (~(sigmask(SIGKILL) | sigmask(SIGSTOP)))

#define GP_REGS_SIZE	(min(sizeof(elf_gregset_t), sizeof(struct sys_pt_regs)))

/* needed for sysctl, but unsued */
int show_unhandled_signals = 0;

static inline int put_sigset_t(sigset_t __user *uset, sigset_t *set)
{
	return copy_to_user(uset, set, sizeof(*uset));
}

static inline int get_sigset_t(sigset_t *set, const sigset_t __user *uset)
{
	return copy_from_user(set, uset, sizeof(*uset));
}

static inline int get_old_sigaction(struct k_sigaction *new_ka,
		struct old_sigaction __user *act)
{
	old_sigset_t mask;

	if (!access_ok(VERIFY_READ, act, sizeof(*act)) ||
			__get_user(new_ka->sa.sa_handler, &act->sa_handler) ||
			__get_user(new_ka->sa.sa_restorer, &act->sa_restorer))
		return -EFAULT;
	__get_user(new_ka->sa.sa_flags, &act->sa_flags);
	__get_user(mask, &act->sa_mask);
	siginitset(&new_ka->sa.sa_mask, mask);
	return 0;
}

#define to_user_ptr(p)		(p)
#define from_user_ptr(p)	(p)

static inline int save_general_regs(struct pt_regs *regs,
		struct mcontext __user *frame)
{
	struct sys_pt_regs tmp;

	/* copy all 32 gprs -- these are in right order  ... */
	memcpy(&tmp, &regs->context[0], 32*4);

	/* copy other regs by hand -- wrong order ... */
	tmp.nip		= regs->context[HOST_IP];
	tmp.msr		= regs->context[HOST_MSR];
	tmp.orig_gpr3	= regs->orig_r3;
	tmp.ctr		= regs->context[HOST_CTR];
	tmp.link	= regs->context[HOST_LR];
	tmp.xer		= regs->context[HOST_XER];
	tmp.ccr		= regs->context[HOST_CR];
	tmp.mq		= 0;
	tmp.trap	= regs->trap;
	tmp.dar		= regs->context[HOST_DAR];
	tmp.dsisr	= regs->context[HOST_DSISR];
	tmp.result	= regs->result;

	return __copy_to_user(&frame->mc_gregs, &tmp, GP_REGS_SIZE);
}

static inline int restore_general_regs(struct pt_regs *regs,
		struct mcontext __user *sr)
{
	struct sys_pt_regs tmp;

	if (__copy_from_user(&tmp, &sr->mc_gregs, GP_REGS_SIZE))
		return -EFAULT;

	tmp.msr = (regs->context[HOST_MSR] & ~MSR_USERCHANGE)
		| (tmp.msr & MSR_USERCHANGE);

	/* copy all 32 gprs -- these are in right order  ... */
	memcpy(&regs->context[0], &tmp, 32*4);

	/* copy other regs by hand -- wrong ordered ... */
	regs->context[HOST_CR]		= tmp.ccr;
	regs->context[HOST_XER]		= tmp.xer;
	regs->context[HOST_LR]		= tmp.link;
	regs->context[HOST_CTR]		= tmp.ctr;
	regs->context[HOST_DAR]		= tmp.dar;
	//regs->context[HOST_BAR]	= 0;
	regs->context[HOST_DSISR]	= tmp.dsisr;
	regs->context[HOST_IP]		= tmp.nip;
	regs->context[HOST_MSR]		= tmp.msr;
	regs->trap			= tmp.trap;
	regs->result			= tmp.result;
	regs->orig_r3			= tmp.orig_gpr3;

	return 0;
}

int kern_do_signal(sigset_t *oldset, struct pt_regs *regs);

/*
 * When we have signals to deliver, we set up on the
 * user stack, going down from the original stack pointer:
 *	a sigregs struct
 *	a sigcontext struct
 *	a gap of __SIGNAL_FRAMESIZE bytes
 *
 * Each of these things must be a multiple of 16 bytes in size.
 *
 */
struct sigregs {
	struct mcontext	mctx;		/* all the register values */
	/*
	 * Programs using the rs6000/xcoff abi can save up to 19 gp
	 * regs and 18 fp regs below sp before decrementing it.
	 */
	int			abigap[56];
};

/* We use the mc_pad field for the signal return trampoline. */
#define tramp	mc_pad

/*
 *  When we have rt signals to deliver, we set up on the
 *  user stack, going down from the original stack pointer:
 *	one rt_sigframe struct (siginfo + ucontext + ABI gap)
 *	a gap of __SIGNAL_FRAMESIZE+16 bytes
 *  (the +16 is to get the siginfo and ucontext in the same
 *  positions as in older kernels).
 *
 *  Each of these things must be a multiple of 16 bytes in size.
 *
 */
struct rt_sigframe {
	struct siginfo info;
	struct ucontext	uc;
	/*
	 * Programs using the rs6000/xcoff abi can save up to 19 gp
	 * regs and 18 fp regs below sp before decrementing it.
	 */
	int			abigap[56];
};

/*
 * Save the current user registers on the user stack.
 * We only save the altivec/spe registers if the process has used
 * altivec/spe instructions at some point.
 */
static int save_user_regs(struct pt_regs *regs, struct mcontext __user *frame,
		int sigret)
{
	CHECK_FULL_REGS(regs);

	/* save general and floating-point registers */
	if (save_general_regs(regs, frame) ||
	    __copy_to_user(&frame->mc_fregs, &regs->context[HOST_FR0],
		    ELF_NFPREG * sizeof(double)))
		return 1;

#ifdef CONFIG_ALTIVEC
	/* save altivec registers */
	if (current->thread.used_vr) {
		flush_altivec_to_thread(current);
		if (__copy_to_user(&frame->mc_vregs, current->thread.vr,
				   ELF_NVRREG * sizeof(vector128)))
			return 1;
		/* set MSR_VEC in the saved MSR value to indicate that
		   frame->mc_vregs contains valid data */
		if (__put_user(regs->msr | MSR_VEC, &frame->mc_gregs[PT_MSR]))
			return 1;
	}
	/* else assert((regs->msr & MSR_VEC) == 0) */

	/* We always copy to/from vrsave, it's 0 if we don't have or don't
	 * use altivec. Since VSCR only contains 32 bits saved in the least
	 * significant bits of a vector, we "cheat" and stuff VRSAVE in the
	 * most significant bits of that same vector. --BenH
	 */
	if (__put_user(current->thread.vrsave, (u32 __user *)&frame->mc_vregs[32]))
		return 1;
#endif /* CONFIG_ALTIVEC */

#ifdef CONFIG_SPE
	/* save spe registers */
	if (current->thread.used_spe) {
		flush_spe_to_thread(current);
		if (__copy_to_user(&frame->mc_vregs, current->thread.evr,
				   ELF_NEVRREG * sizeof(u32)))
			return 1;
		/* set MSR_SPE in the saved MSR value to indicate that
		   frame->mc_vregs contains valid data */
		if (__put_user(regs->msr | MSR_SPE, &frame->mc_gregs[PT_MSR]))
			return 1;
	}
	/* else assert((regs->msr & MSR_SPE) == 0) */

	/* We always copy to/from spefscr */
	if (__put_user(current->thread.spefscr, (u32 __user *)&frame->mc_vregs + ELF_NEVRREG))
		return 1;
#endif /* CONFIG_SPE */

	if (sigret) {
		/* Set up the sigreturn trampoline: li r0,sigret; sc */
		if (__put_user(0x38000000UL + sigret, &frame->tramp[0])
		    || __put_user(0x44000002UL, &frame->tramp[1]))
			return 1;
		flush_icache_range((unsigned long) &frame->tramp[0],
				   (unsigned long) &frame->tramp[2]);
	}

	return 0;
}

/*
 * Restore the current user register values from the user stack,
 * (except for MSR).
 */
static long restore_user_regs(struct pt_regs *regs,
			      struct mcontext __user *sr, int sig)
{
	long err;
	unsigned int save_r2 = 0;
#if defined(CONFIG_ALTIVEC) || defined(CONFIG_SPE)
	unsigned long msr;
#endif

	/*
	 * restore general registers but not including MSR or SOFTE. Also
	 * take care of keeping r2 (TLS) intact if not a signal
	 */
	if (!sig)
		save_r2 = (unsigned int)regs->context[HOST_R2];
	err = restore_general_regs(regs, sr);
	if (!sig)
		regs->context[HOST_R2] = (unsigned long) save_r2;
	if (err)
		return 1;

	if (__copy_from_user(&regs->context[HOST_FR0], &sr->mc_fregs,
			     sizeof(sr->mc_fregs)))
		return 1;

#ifdef CONFIG_ALTIVEC
	/* force the process to reload the altivec registers from
	   current->thread when it next does altivec instructions */
	regs->msr &= ~MSR_VEC;
	if (!__get_user(msr, &sr->mc_gregs[PT_MSR]) && (msr & MSR_VEC) != 0) {
		/* restore altivec registers from the stack */
		if (__copy_from_user(current->thread.vr, &sr->mc_vregs,
				     sizeof(sr->mc_vregs)))
			return 1;
	} else if (current->thread.used_vr)
		memset(current->thread.vr, 0, ELF_NVRREG * sizeof(vector128));

	/* Always get VRSAVE back */
	if (__get_user(current->thread.vrsave, (u32 __user *)&sr->mc_vregs[32]))
		return 1;
#endif /* CONFIG_ALTIVEC */

#ifdef CONFIG_SPE
	/* force the process to reload the spe registers from
	   current->thread when it next does spe instructions */
	regs->msr &= ~MSR_SPE;
	if (!__get_user(msr, &sr->mc_gregs[PT_MSR]) && (msr & MSR_SPE) != 0) {
		/* restore spe registers from the stack */
		if (__copy_from_user(current->thread.evr, &sr->mc_vregs,
				     ELF_NEVRREG * sizeof(u32)))
			return 1;
	} else if (current->thread.used_spe)
		memset(current->thread.evr, 0, ELF_NEVRREG * sizeof(u32));

	/* Always get SPEFSCR back */
	if (__get_user(current->thread.spefscr, (u32 __user *)&sr->mc_vregs + ELF_NEVRREG))
		return 1;
#endif /* CONFIG_SPE */

	return 0;
}

/*
 * Restore the user process's signal mask
 */
static void restore_sigmask(sigset_t *set)
{
	sigdelsetmask(set, ~_BLOCKABLE);
	spin_lock_irq(&current->sighand->siglock);
	current->blocked = *set;
	recalc_sigpending();
	spin_unlock_irq(&current->sighand->siglock);
}

/*
 * Set up a signal frame for a "real-time" signal handler
 * (one which gets siginfo).
 */
static int handle_rt_signal(unsigned long sig, struct k_sigaction *ka,
		siginfo_t *info, sigset_t *oldset,
		struct pt_regs *regs, unsigned long newsp)
{
	struct rt_sigframe __user *rt_sf;
	struct mcontext __user *frame;
	unsigned long origsp = newsp;

	/* Set up Signal Frame */
	/* Put a Real Time Context onto stack */
	newsp -= sizeof(*rt_sf);
	rt_sf = (struct rt_sigframe __user *)newsp;

	/* create a stack frame for the caller of the handler */
	newsp -= __SIGNAL_FRAMESIZE + 16;

	if (!access_ok(VERIFY_WRITE, (void __user *)newsp, origsp - newsp))
		goto badframe;

	/* Put the siginfo & fill in most of the ucontext */
	if (copy_siginfo_to_user(&rt_sf->info, info)
	    || __put_user(0, &rt_sf->uc.uc_flags)
	    || __put_user(0, &rt_sf->uc.uc_link)
	    || __put_user(current->sas_ss_sp, &rt_sf->uc.uc_stack.ss_sp)
	    || __put_user(sas_ss_flags(regs->context[HOST_R1]),
			  &rt_sf->uc.uc_stack.ss_flags)
	    || __put_user(current->sas_ss_size, &rt_sf->uc.uc_stack.ss_size)
	    || __put_user(to_user_ptr(&rt_sf->uc.uc_mcontext),
		    &rt_sf->uc.uc_regs)
	    || put_sigset_t(&rt_sf->uc.uc_sigmask, oldset))
		goto badframe;

	/* Save user registers on the stack */
	frame = &rt_sf->uc.uc_mcontext;
	/* no VSDO trampoline on P4 */
	if (save_user_regs(regs, frame, __NR_rt_sigreturn))
		goto badframe;
	regs->context[HOST_LR] = (unsigned long) frame->tramp;

	regs->context[HOST_FPSCR] = 0;	/* turn off all fp exceptions */

	if (put_user(regs->context[HOST_R1], (u32 __user *)newsp))
		goto badframe;
	regs->context[HOST_R1] = newsp;
	regs->context[HOST_R3] = sig;
	regs->context[HOST_R4] = (unsigned long) &rt_sf->info;
	regs->context[HOST_R5] = (unsigned long) &rt_sf->uc;
	regs->context[HOST_R6] = (unsigned long) rt_sf;
	regs->context[HOST_IP] = (unsigned long) ka->sa.sa_handler;
	regs->trap = 0;
	return 1;

badframe:
#ifdef DEBUG_SIG
	printk("badframe in handle_rt_signal, regs=%p frame=%p newsp=%lx\n",
	       regs, frame, newsp);
#endif
	force_sigsegv(sig, current);
	return 0;
}

static int do_setcontext(struct ucontext __user *ucp, struct pt_regs *regs, int sig)
{
	sigset_t set;
	struct mcontext __user *mcp;

	if (get_sigset_t(&set, &ucp->uc_sigmask))
		return -EFAULT;
	if (__get_user(mcp, &ucp->uc_regs))
		return -EFAULT;
	restore_sigmask(&set);
	if (restore_user_regs(regs, mcp, sig))
		return -EFAULT;

	return 0;
}

long sys_swapcontext(struct ucontext __user *old_ctx,
		       struct ucontext __user *new_ctx,
		       int ctx_size)
{
	struct pt_regs *regs = &current->thread.regs;
	unsigned char tmp;

	/* Context size is for future use. Right now, we only make sure
	 * we are passed something we understand
	 */
	if (ctx_size < sizeof(struct ucontext))
		return -EINVAL;

	if (old_ctx != NULL) {
		if (!access_ok(VERIFY_WRITE, old_ctx, sizeof(*old_ctx))
		    || save_user_regs(regs, &old_ctx->uc_mcontext, 0)
		    || put_sigset_t(&old_ctx->uc_sigmask, &current->blocked)
		    || __put_user(to_user_ptr(&old_ctx->uc_mcontext),
			    &old_ctx->uc_regs))
			return -EFAULT;
	}
	if (new_ctx == NULL)
		return 0;
	if (!access_ok(VERIFY_READ, new_ctx, sizeof(*new_ctx))
	    || __get_user(tmp, (u8 __user *) new_ctx)
	    || __get_user(tmp, (u8 __user *) (new_ctx + 1) - 1))
		return -EFAULT;

	/*
	 * If we get a fault copying the context into the kernel's
	 * image of the user's registers, we can't just return -EFAULT
	 * because the user's registers will be corrupted.  For instance
	 * the NIP value may have been updated but not some of the
	 * other registers.  Given that we have done the access_ok
	 * and successfully read the first and last bytes of the region
	 * above, this should only happen in an out-of-memory situation
	 * or if another thread unmaps the region containing the context.
	 * We kill the task with a SIGSEGV in this situation.
	 */
	if (do_setcontext(new_ctx, regs, 0))
		do_exit(SIGSEGV);

	//sigreturn_exit(regs);           /* doesn't return */
	force_successful_syscall_return();
	return regs->context[HOST_R3];
}

long sys_rt_sigreturn(void)
{
	struct pt_regs *regs = &current->thread.regs;
	struct rt_sigframe __user *rt_sf;

	/* Always make any pending restarted system calls return -EINTR */
	current_thread_info()->restart_block.fn = do_no_restart_syscall;

	rt_sf = (struct rt_sigframe __user *)
		(regs->context[HOST_R1] + __SIGNAL_FRAMESIZE + 16);
	if (!access_ok(VERIFY_READ, rt_sf, sizeof(*rt_sf)))
		goto bad;
	if (do_setcontext(&rt_sf->uc, regs, 1))
		goto bad;

	/*
	 * It's not clear whether or why it is desirable to save the
	 * sigaltstack setting on signal delivery and restore it on
	 * signal return.  But other architectures do this and we have
	 * always done it up until now so it is probably better not to
	 * change it.  -- paulus
	 */
	restore_altstack(&rt_sf->uc.uc_stack);

	//sigreturn_exit(regs);           /* doesn't return */
	force_successful_syscall_return();
	return regs->context[HOST_R3];

 bad:
	force_sig(SIGSEGV, current);
	return 0;
}

int sys_debug_setcontext(struct ucontext __user *ctx,
			 int ndbg, struct sig_dbg_op __user *dbg)
{
	struct pt_regs *regs = &current->thread.regs;
	struct sig_dbg_op op;
	int i;
	unsigned long new_msr = regs->context[HOST_MSR];
#if defined(CONFIG_4xx) || defined(CONFIG_BOOKE)
	//NOTE: dbcr0 does not exist on P4Linux
	//unsigned long new_dbcr0 = current->thread.dbcr0;
#endif

	for (i=0; i<ndbg; i++) {
		if (__copy_from_user(&op, dbg, sizeof(op)))
			return -EFAULT;
		switch (op.dbg_type) {
		case SIG_DBG_SINGLE_STEPPING:
#if defined(CONFIG_4xx) || defined(CONFIG_BOOKE)
			if (op.dbg_value) {
				new_msr |= MSR_DE;
				//new_dbcr0 |= (DBCR0_IDM | DBCR0_IC);
			} else {
				new_msr &= ~MSR_DE;
				//new_dbcr0 &= ~(DBCR0_IDM | DBCR0_IC);
			}
#else
			if (op.dbg_value)
				new_msr |= MSR_SE;
			else
				new_msr &= ~MSR_SE;
#endif
			break;
		case SIG_DBG_BRANCH_TRACING:
#if defined(CONFIG_4xx) || defined(CONFIG_BOOKE)
			return -EINVAL;
#else
			if (op.dbg_value)
				new_msr |= MSR_BE;
			else
				new_msr &= ~MSR_BE;
#endif
			break;

		default:
			return -EINVAL;
		}
	}

	/* We wait until here to actually install the values in the
	   registers so if we fail in the above loop, it will not
	   affect the contents of these registers.  After this point,
	   failure is a problem, anyway, and it's very unlikely unless
	   the user is really doing something wrong. */
	regs->context[HOST_MSR] = new_msr;
#if defined(CONFIG_4xx) || defined(CONFIG_BOOKE)
	//current->thread.dbcr0 = new_dbcr0;
#endif

	/*
	 * If we get a fault copying the context into the kernel's
	 * image of the user's registers, we can't just return -EFAULT
	 * because the user's registers will be corrupted.  For instance
	 * the NIP value may have been updated but not some of the
	 * other registers.  Given that we have done the access_ok
	 * and successfully read the first and last bytes of the region
	 * above, this should only happen in an out-of-memory situation
	 * or if another thread unmaps the region containing the context.
	 * We kill the task with a SIGSEGV in this situation.
	 */
	if (do_setcontext(ctx, regs, 1)) {
		force_sig(SIGSEGV, current);
		goto out;
	}

	/*
	 * It's not clear whether or why it is desirable to save the
	 * sigaltstack setting on signal delivery and restore it on
	 * signal return.  But other architectures do this and we have
	 * always done it up until now so it is probably better not to
	 * change it.  -- paulus
	 */
	restore_altstack(&ctx->uc_stack);

	//sigreturn_exit(regs);           /* doesn't return */
	force_successful_syscall_return();
	return regs->context[HOST_R3];

 out:
	return 0;
}

/*
 * OK, we're invoking a handler
 */
static int handle_signal(unsigned long sig, struct k_sigaction *ka,
		siginfo_t *info, sigset_t *oldset, struct pt_regs *regs,
		unsigned long newsp)
{
	struct sigcontext __user *sc;
	struct sigregs __user *frame;
	unsigned long origsp = newsp;

	/* Set up Signal Frame */
	newsp -= sizeof(struct sigregs);
	frame = (struct sigregs __user *) newsp;

	/* Put a sigcontext on the stack */
	newsp -= sizeof(*sc);
	sc = (struct sigcontext __user *) newsp;

	/* create a stack frame for the caller of the handler */
	newsp -= __SIGNAL_FRAMESIZE;

	if (!access_ok(VERIFY_WRITE, (void __user *) newsp, origsp - newsp))
		goto badframe;

#if _NSIG != 64
#error "Please adjust handle_signal()"
#endif
	if (__put_user(to_user_ptr(ka->sa.sa_handler), &sc->handler)
	    || __put_user(oldset->sig[0], &sc->oldmask)
	    || __put_user(oldset->sig[1], &sc->_unused[3])
	    || __put_user(to_user_ptr(frame), &sc->regs)
	    || __put_user(sig, &sc->signal))
		goto badframe;

	/* no VSDO trampoline on P4 */
	if (save_user_regs(regs, &frame->mctx, __NR_sigreturn))
		goto badframe;
	regs->context[HOST_LR] = (unsigned long) frame->mctx.tramp;

	regs->context[HOST_FPSCR] = 0;	/* turn off all fp exceptions */

	if (put_user(regs->context[HOST_R1], (u32 __user *)newsp))
		goto badframe;
	regs->context[HOST_R1] = newsp;
	regs->context[HOST_R3] = sig;
	regs->context[HOST_R4] = (unsigned long) sc;
	regs->context[HOST_IP] = (unsigned long) ka->sa.sa_handler;
	regs->trap = 0;

	return 1;

badframe:
#ifdef DEBUG_SIG
	printk("badframe in handle_signal, regs=%p frame=%p newsp=%lx\n",
	       regs, frame, newsp);
#endif
	force_sigsegv(sig, current);
	return 0;
}

/*
 * Do a signal return; undo the signal stack.
 */
long sys_sigreturn(void)
{
	struct pt_regs *regs = &current->thread.regs;
	struct sigcontext __user *sc;
	struct sigcontext sigctx;
	struct mcontext __user *sr;
	sigset_t set;

	/* Always make any pending restarted system calls return -EINTR */
	current_thread_info()->restart_block.fn = do_no_restart_syscall;

	sc = (struct sigcontext __user *)(regs->context[HOST_R1] + __SIGNAL_FRAMESIZE);
	if (copy_from_user(&sigctx, sc, sizeof(sigctx)))
		goto badframe;

	set.sig[0] = sigctx.oldmask;
	set.sig[1] = sigctx._unused[3];
	restore_sigmask(&set);

	sr = (struct mcontext __user *)from_user_ptr(sigctx.regs);
	if (!access_ok(VERIFY_READ, sr, sizeof(*sr))
	    || restore_user_regs(regs, sr, 1))
		goto badframe;

	//sigreturn_exit(regs);           /* doesn't return */
	force_successful_syscall_return();
	return regs->context[HOST_R3];

badframe:
	force_sig(SIGSEGV, current);
	return 0;
}

/*
 * Note that 'init' is a special process: it doesn't get signals it doesn't
 * want to handle. Thus you cannot kill init even with a SIGKILL even by
 * mistake.
 */
int kern_do_signal(sigset_t *oldset, struct pt_regs *regs)
{
	siginfo_t info;
	struct k_sigaction ka;
	unsigned int frame, newsp;
	int signr, ret;

	if (try_to_freeze()) {
		signr = 0;
		if (!signal_pending(current))
			goto no_signal;
	}

	if (!oldset)
		oldset = &current->blocked;

	newsp = frame = 0;

	signr = get_signal_to_deliver(&info, &ka, regs, NULL);
no_signal:
	if (TRAP(regs) == 0x0C00		/* System Call! */
	    && regs->context[HOST_CR] & 0x10000000		/* error signalled */
	    && ((ret = regs->context[HOST_R3]) == ERESTARTSYS
		|| ret == ERESTARTNOHAND || ret == ERESTARTNOINTR
		|| ret == ERESTART_RESTARTBLOCK)) {

		if (signr > 0
		    && (ret == ERESTARTNOHAND || ret == ERESTART_RESTARTBLOCK
			|| (ret == ERESTARTSYS
			    && !(ka.sa.sa_flags & SA_RESTART)))) {
			/* make the system call return an EINTR error */
			regs->result = -EINTR;
			regs->context[HOST_R3] = EINTR;
			/* note that the cr0.SO bit is already set */
		} else {
			regs->context[HOST_IP] -= 4;	/* Back up & retry system call */
			regs->result = 0;
			regs->trap = 0;
			if (ret == ERESTART_RESTARTBLOCK)
				regs->context[HOST_R0] = __NR_restart_syscall;
			else
				regs->context[HOST_R3] = regs->orig_r3;
		}
	}

	if (signr == 0)
		return 0;		/* no signals delivered */

	if ((ka.sa.sa_flags & SA_ONSTACK) && current->sas_ss_size
	    && !on_sig_stack(regs->context[HOST_R1]))
		newsp = current->sas_ss_sp + current->sas_ss_size;
	else
		newsp = regs->context[HOST_R1];
	newsp &= ~0xfUL;

	/* Whee!  Actually deliver the signal.  */
	if (ka.sa.sa_flags & SA_SIGINFO)
		ret = handle_rt_signal(signr, &ka, &info, oldset, regs, newsp);
	else
		ret = handle_signal(signr, &ka, &info, oldset, regs, newsp);

	if (ret) {
		spin_lock_irq(&current->sighand->siglock);
		sigorsets(&current->blocked, &current->blocked,
			  &ka.sa.sa_mask);
		if (!(ka.sa.sa_flags & SA_NODEFER))
			sigaddset(&current->blocked, signr);
		recalc_sigpending();
		spin_unlock_irq(&current->sighand->siglock);
	}

	return ret;
}

void do_signal(struct pt_regs *regs)
{
	kern_do_signal(NULL, regs);
}
