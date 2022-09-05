/*
 * linux/arch/p4/arch-arm/ptrace.c - ARM ptrace handling
 * derived from arch/arm/kernel/ptrace.c
 *
 * 2010-09-20, David Engraf <den@sysgo.com>
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/ptrace.h>
#include <linux/user.h>
#include <linux/security.h>
#include <linux/init.h>
#include <linux/signal.h>
#include <linux/uaccess.h>

#include <asm/pgtable.h>
#include <asm/traps.h>
#include <asm/vfp.h>

#define REG_PC	15
#define REG_PSR	16
/*
 * does not yet catch signals sent when the child dies.
 * in exit.c or in signal.c.
 */

#if 0
/*
 * Breakpoint SWI instruction: SWI &9F0001
 */
#define BREAKINST_ARM	0xef9f0001
#define BREAKINST_THUMB	0xdf00		/* fill this in later */
#else
/*
 * New breakpoints - use an undefined instruction.  The ARM architecture
 * reference manual guarantees that the following instruction space
 * will produce an undefined instruction exception on all CPUs:
 *
 *  ARM:   xxxx 0111 1111 xxxx xxxx xxxx 1111 xxxx
 *  Thumb: 1101 1110 xxxx xxxx
 */
#define BREAKINST_ARM	0xe7f001f0
#define BREAKINST_THUMB	0xde01
#endif

/*
 * Perform get_thread_area on behalf of the traced child.
 */
int ptrace_get_thread_area(struct task_struct *child, long idx, void *user_desc)
{
	return (put_user(child->thread.regs.context[HOST_TLS], 
					  (unsigned long __user *) user_desc));
}

/*
 * this routine will get a word off of the processes privileged stack.
 * the offset is how far from the base addr as stored in the THREAD.
 * this routine assumes that all the privileged stacks are in our
 * data space.
 */
static inline long get_user_reg(struct task_struct *task, int offset)
{
	/* orig_r0 is not part of context */
	if (offset == 17)
		return task_pt_regs(task)->orig_r0;

	return task_pt_regs(task)->context[offset];
}

/*
 * this routine will put a word on the processes privileged stack.
 * the offset is how far from the base addr as stored in the THREAD.
 * this routine assumes that all the privileged stacks are in our
 * data space.
 */
static inline int
put_user_reg(struct task_struct *task, int offset, long data)
{
	struct pt_regs newregs, *regs = task_pt_regs(task);
	int ret = -EINVAL;

	/* orig_r0 is not part of context */
	if (offset == 17) {
		regs->orig_r0 = data;
		return 0;
	}

	newregs = *regs;
	newregs.context[offset] = data;

	if (valid_user_regs(&newregs)) {
		regs->context[offset] = data;
		ret = 0;
	}

	return ret;
}

/*
 * Called by kernel/ptrace.c when detaching..
 */
void ptrace_disable(struct task_struct *child)
{
	/* Nothing to do. */
}

/*
 * Handle hitting a breakpoint.
 */
void ptrace_break(struct task_struct *tsk, struct pt_regs *regs)
{
	siginfo_t info;

	info.si_signo = SIGTRAP;
	info.si_errno = 0;
	info.si_code  = TRAP_BRKPT;
	info.si_addr  = (void __user *)instruction_pointer(regs);

	force_sig_info(SIGTRAP, &info, tsk);
}

/*static*/ int break_trap(struct pt_regs *regs, unsigned int instr)
{
	ptrace_break(current, regs);
	return 0;
}

/*
 * Read the word at offset "off" into the "struct user".  We
 * actually access the pt_regs stored on the kernel stack.
 */
int ptrace_read_user(struct task_struct *tsk, unsigned long off,
			    unsigned long __user *ret)
{
	unsigned long tmp;

	if (off & 3 || off >= sizeof(struct user))
		return -EIO;

	tmp = 0;
	if (off == PT_TEXT_ADDR)
		tmp = tsk->mm->start_code;
	else if (off == PT_DATA_ADDR)
		tmp = tsk->mm->start_data;
	else if (off == PT_TEXT_END_ADDR)
		tmp = tsk->mm->end_code;
	else if (off < sizeof(struct pt_regs))
		tmp = get_user_reg(tsk, off >> 2);

	return put_user(tmp, ret);
}

/*
 * Write the word at offset "off" into "struct user".  We
 * actually access the pt_regs stored on the kernel stack.
 */
int ptrace_write_user(struct task_struct *tsk, unsigned long off,
			     unsigned long val)
{
	if (off & 3 || off >= sizeof(struct user))
		return -EIO;

	if (off >= sizeof(struct pt_regs))
		return 0;

	return put_user_reg(tsk, off >> 2, val);
}

/*
 * Read a general register set.
 */
int get_gpregs(unsigned long buf, struct task_struct *child)
{
	struct pt_regs *regs = &child->thread.regs;	
	int i;
	__s32 __user *data = (__s32 __user *)buf;	

	if (!access_ok(VERIFY_WRITE, data, 18 * 4))
		return -EIO;

	for (i = 0; i < 17; i++)
		__put_user (regs->context[HOST_R0 + i], data + i);
	__put_user (regs->orig_r0, data + 17);

	return 0;
}

/*
 * Write a general register set.
 */
int set_gpregs(unsigned long buf, struct task_struct *child)
{
	struct pt_regs *regs = &child->thread.regs;	
	int i;
	__s32 __user *data = (__s32 __user *)buf;	

	if (!access_ok(VERIFY_READ, data, 18 * 4))
		return -EIO;

	for (i = 0; i < 17; i++)
		__get_user (regs->context[HOST_R0 + i], data + i);
	__get_user (regs->orig_r0, data + 18);

	/* badvaddr, status, and cause may not be written.  */

	return 0;
}

/*
 * Get the child FPU state.
 */
int get_fpregs(unsigned long buf, struct task_struct *child)
{
	/* not implemented */
	return -EIO;
}

/*
 * Set the child FPU state.
 */
int set_fpregs(unsigned long buf, struct task_struct *child)
{
	/* not implemented */
	return -EIO;
}

#if defined (CONFIG_VFP)

/*
 * Get the child VFP state.
 */
int get_vfpregs(unsigned long buf, struct task_struct *child)
{
	struct pt_regs *regs = &child->thread.regs;
	struct user_vfp __user *ufp = (struct user_vfp __user *)buf;
	int err = 0;

	/* copy the floating point registers */
	err |= __copy_to_user(&ufp->fpregs, &regs->context[HOST_FPREGS], sizeof(ufp->fpregs));

	/* copy the status and control register */
	err |= __put_user(regs->context[HOST_FPSCR], &ufp->fpscr);

	return err ? -EFAULT : 0;
}

/*
 * Set the child VFP state.
 */
int set_vfpregs(unsigned long buf, struct task_struct *child)
{
	struct pt_regs *regs = &child->thread.regs;
	struct user_vfp __user *ufp = (struct user_vfp __user *)buf;
	int err = 0;

	/* copy the floating point registers */
	err |= __copy_from_user(&regs->context[HOST_FPREGS], &ufp->fpregs, sizeof(ufp->fpregs));

	/* copy the status and control register */
	err |= __get_user(regs->context[HOST_FPSCR], &ufp->fpscr);

	if (!err) {
		/* enable VFP after VFP context update */
		regs->context[HOST_FPEXC] = FPEXC_EN;
	}

	return err ? -EFAULT : 0;
}

#endif
