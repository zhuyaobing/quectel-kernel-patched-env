#ifndef __P4_PTRACE_H
#define __P4_PTRACE_H

#if defined(CONFIG_P4_ARM)
#include <arch-override/asm/ptrace.h>
#endif

#ifndef __ASSEMBLY__

/* rename a few symbols: these are defined in the regular
 * arch/asm/ptrace.h but we want to define them ourselves
 * in asm/host_context.h. In other words: avoid clashes
 * between arch/asm/ptrace.h and asm/host_context.h.
 */
#define pt_regs sys_pt_regs
#define send_sigtrap send_sigtrap_subarch

#include <linux/types.h>

#if defined(CONFIG_P4_X86) || defined(CONFIG_P4_PPC)
#ifdef CONFIG_64BIT
#define __FRAME_OFFSETS /* Needed to get the R* macros */
#endif
#include <uapi/asm/ptrace.h>
#define arch_has_single_step() (1)

#ifdef CONFIG_64BIT
/* these functions are not supported but defined due to i386 */
#undef PTRACE_GET_THREAD_AREA
#undef PTRACE_SET_THREAD_AREA
#undef PTRACE_GETFPXREGS
#undef PTRACE_SETFPXREGS
#endif

#endif

#undef pt_regs
#undef send_sigtrap
#undef user_mode
#undef instruction_pointer

/* pt_regs are def'd here */
#include <asm/host_context.h>

#ifdef CONFIG_P4_PPC
#define TRAP(regs)              ((regs)->trap & ~0xF)
#define __SIGNAL_FRAMESIZE	64
#define CHECK_FULL_REGS(regs)						      \
do {									      \
	if ((regs)->trap & 1)						      \
		printk(KERN_CRIT "%s: partial register set\n", __func__); \
} while (0)
#endif

#define instruction_pointer(regs) PT_REGS_IP(regs)

struct task_struct;

extern int getreg(struct task_struct *child, int addr, unsigned long *value);
extern int putreg(struct task_struct *child, int addr, unsigned long value);
extern int get_gpregs(unsigned long buf, struct task_struct *child);
extern int set_gpregs(unsigned long buf, struct task_struct *child);
extern int get_fpregs(unsigned long buf, struct task_struct *child);
extern int set_fpregs(unsigned long buf, struct task_struct *child);
extern int get_fpxregs(unsigned long buf, struct task_struct *child);
extern int set_fpxregs(unsigned long buf, struct task_struct *child);
#ifdef CONFIG_VFP
extern int get_vfpregs(unsigned long buf, struct task_struct *child);
extern int set_vfpregs(unsigned long buf, struct task_struct *child);
#endif

extern int ptrace_get_thread_area(struct task_struct *child,
		       long idx, void *data);
extern int ptrace_set_thread_area(struct task_struct *child,
		       long idx, void *data);
extern int arch_copy_tls(struct task_struct *new, struct pt_regs *regs);
extern void arch_clear_tls(struct task_struct *tsk);

/*
 * We cannot use one of the architecture specific registers to identify if we
 * were in user or kernel space because all tasks (kernel and user space) are
 * mapped to a PikeOS user task. Basically we have several entry points which
 * needs to be observed by user_mode():
 * 
 * 1. IRQ subsystem
 *    This is the common case, we use __is_user_mode() instead to check if the
 *    Linux main thread is in user or kernel mode. The IRQ subsystem has it's
 *    own register context (check do_IRQ).
 *
 * 2. exception handling kernel/user
 *    The kernel configured is_user with the initial kernel thread. The user
 *    space is configured when the kernel calls start_thread() for the init
 *    task. In both cases further threads/processes are copied from the initial
 *    one, thus is_user stays valid.
 *
 * 3. userspace syscall handling
 *    Handled in the user exception handler
 */
#define user_mode(r) ((r)->is_user)

extern int __is_user_mode(unsigned int cpu);

#endif
#define user_stack_pointer(regs) PT_REGS_SP(regs)
#endif
