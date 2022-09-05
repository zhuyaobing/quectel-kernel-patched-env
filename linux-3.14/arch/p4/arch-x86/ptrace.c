/*
 * Copyright (C) 2000, 2001, 2002 Jeff Dike (jdike@karaya.com)
 * Copyright (c) 2004, 2006, Alex Zuepke <azu@sysgo.de>
 *
 * 2014-01-21 David Engraf <den@sysgo.com> adapt to X86_64
 *
 * Licensed under the GPL
 */

#include <linux/sched.h>
#include <asm/elf.h>
#include <asm/ptrace.h>
#include <asm/uaccess.h>
#include <uapi/asm/ldt.h>
#include <asm/ldt.h>

extern void activate_fpu(struct pt_regs *regs);


/*
 * NOTE: x86 debug registers DR0..7 handling not supported by P4
 */

int getreg(struct task_struct *child, int addr, unsigned long *value)
{
	int index;

	if ((addr & 3) || addr < 0)
		return -EIO;

#ifdef CONFIG_64BIT
	index = addr;

#else
	index = addr >> 2;
#endif

	/* debug_reg or unknown register, just return 0 */
	*value = 0;

	if(index < FRAME_SIZE)
		*value = PT_REG(&child->thread.regs, index);
	else if ((addr >= offsetof(struct user, u_debugreg[0])) &&
		(addr <= offsetof(struct user, u_debugreg[7]))) {
		/* ignore debugreg and return default */
	}

	return 0;
}

int putreg(struct task_struct *child, int addr, unsigned long value)
{
	int index;

	if ((addr & 3) || addr < 0)
		return -EIO;

#ifdef CONFIG_64BIT
	index = addr;
#else
	index = addr >> 2;
#endif

	if(index < FRAME_SIZE) {
		PT_REGS_SET(&child->thread.regs, index, value);
		return 0;
	}

	return -EIO;
}

int get_gpregs(unsigned long buf, struct task_struct *child)
{
	unsigned long tmp = 0;
	int i;

	if (!access_ok(VERIFY_WRITE, (unsigned long *)buf,
		FRAME_SIZE_OFFSET)) {
		return -EIO;
	}
	for ( i = 0; i < FRAME_SIZE_OFFSET; i += sizeof(long) ) {
		getreg(child, i, &tmp);
		__put_user(tmp, (unsigned long *) buf);
		buf += sizeof(long);
	}

	return 0;
}

int set_gpregs(unsigned long buf, struct task_struct *child)
{
	unsigned long tmp = 0;
	int i;
	if (!access_ok(VERIFY_READ, (unsigned *)buf,
		FRAME_SIZE_OFFSET)) {
		return -EIO;
	}
	for ( i = 0; i < FRAME_SIZE_OFFSET; i += sizeof(long) ) {
		__get_user(tmp, (unsigned long *) buf);
		putreg(child, i, tmp);
		buf += sizeof(long);
	}

	return 0;
}

#ifdef CONFIG_64BIT
/* special handling on x86_64: fpregs uses fxsr (check native) */
int get_fpregs(unsigned long buf, struct task_struct *child)
{
	if (!cpu_has_fxsr) {
		printk(KERN_ERR "Warning: Not supported on P4Linux\n");
		return -ENODEV;
	}

	if (!child->thread.regs.context[HOST_USED_FPU] & 1)
		activate_fpu(&child->thread.regs);

	if(copy_to_user((void*)buf, &child->thread.regs.context[HOST_X_FCW_FSW], sizeof(struct user_i387_struct)))
		return -EIO;
	return 0;
}

int set_fpregs(unsigned long buf, struct task_struct *child)
{
	struct user_i387_struct tmp;

	if (!cpu_has_fxsr) {
		printk(KERN_ERR "Warning: Not supported on P4Linux\n");
		return -ENODEV;
	}

	if(copy_from_user(&tmp, (void*)buf, sizeof(struct user_i387_struct)))
		return -EIO;

	child->thread.regs.context[HOST_USED_FPU] |= 1;
	memcpy(&child->thread.regs.context[HOST_X_FCW_FSW], &tmp, sizeof(struct user_i387_struct));
	return 0;
}

#else

/*
 * normal i387 FPU handling
 */
int get_fpregs(unsigned long buf, struct task_struct *child)
{
	/* not supported on SSE machines */
	if (elf_aux_hwcap & X86_FEATURE_XMM)
		return -EIO;

	if (!child->thread.regs.context[HOST_USED_FPU] & 1)
		activate_fpu(&child->thread.regs);

	if(copy_to_user((void*)buf, &child->thread.regs.context[HOST_FCW], sizeof(struct user_i387_struct)))
		return -EIO;
	return 0;
}

int set_fpregs(unsigned long buf, struct task_struct *child)
{
	struct user_i387_struct tmp;

	/* not supported on SSE machines */
	if (elf_aux_hwcap & X86_FEATURE_XMM)
		return -EIO;

	if(copy_from_user(&tmp, (void*)buf, sizeof(struct user_i387_struct)))
		return -EIO;

	child->thread.regs.context[HOST_USED_FPU] |= 1;
	memcpy(&child->thread.regs.context[HOST_FCW], &tmp, sizeof(struct user_i387_struct));
	return 0;
}

/*
 * SSE register handling
 */
int get_fpxregs(unsigned long buf, struct task_struct *child)
{
	/* on old machines, take get_fpregs() instead */
	if (!(elf_aux_hwcap & X86_FEATURE_XMM))
		return -EIO;

	if (!child->thread.regs.context[HOST_USED_FPU] & 1)
		activate_fpu(&child->thread.regs);

	if(copy_to_user((void*)buf, &child->thread.regs.context[HOST_X_FCW_FSW], sizeof(struct user_fxsr_struct)))
		return -EIO;
	return 0;
}

int set_fpxregs(unsigned long buf, struct task_struct *child)
{
	struct user_fxsr_struct tmp;

	/* on old machines, take get_fpregs() instead */
	if (!(elf_aux_hwcap & X86_FEATURE_XMM))
		return -EIO;

	if(copy_from_user(&tmp, (void*)buf, sizeof(struct user_fxsr_struct)))
		return -EIO;

	child->thread.regs.context[HOST_USED_FPU] |= 1;
	memcpy(&child->thread.regs.context[HOST_X_FCW_FSW], &tmp, sizeof(struct user_fxsr_struct));
	return 0;
}
#endif

int dump_fpu(struct pt_regs *regs, elf_fpregset_t *fpu )
{
	return 1;
}

#define TRAP_FLAG 0x100

void user_enable_single_step(struct task_struct *child)
{
	unsigned long value;
	value = PT_REGS_FLAGS(&child->thread.regs);
	PT_REGS_FLAGS(&child->thread.regs) = value | TRAP_FLAG;
}

void user_disable_single_step(struct task_struct *child)
{
	unsigned long value;
	value = PT_REGS_FLAGS(&child->thread.regs);
	PT_REGS_FLAGS(&child->thread.regs) = value & ~TRAP_FLAG;
}

void ptrace_disable(struct task_struct *child)
{
	user_disable_single_step(child);
}

#ifdef CONFIG_X86_32
/*
 * Perform get_thread_area on behalf of the traced child.
 */
int ptrace_get_thread_area(struct task_struct *child,
		       long idx, void *data)
{
	struct user_desc info;
	struct desc_struct *desc;

	if (idx < GDT_ENTRY_TLS_MIN || idx > GDT_ENTRY_TLS_MAX)
		return -EINVAL;

	desc = (struct desc_struct*)&child->thread.regs.context[HOST_TLS0A] + idx - GDT_ENTRY_TLS_MIN;

	info.entry_number = idx;
	info.base_addr = GET_BASE(desc);
	info.limit = GET_LIMIT(desc);
	info.seg_32bit = GET_32BIT(desc);
	info.contents = GET_CONTENTS(desc);
	info.read_exec_only = !GET_WRITABLE(desc);
	info.limit_in_pages = GET_LIMIT_PAGES(desc);
	info.seg_not_present = !GET_PRESENT(desc);
	info.useable = GET_USEABLE(desc);

	if (copy_to_user(data, &info, sizeof(info)))
		return -EFAULT;

	return 0;
}

/*
 * Perform set_thread_area on behalf of the traced child.
 */
int ptrace_set_thread_area(struct task_struct *child,
		       long idx, void *data)
{
	struct user_desc info;
	struct desc_struct *desc;

	if (copy_from_user(&info, data, sizeof(info)))
		return -EFAULT;

	if (idx < GDT_ENTRY_TLS_MIN || idx > GDT_ENTRY_TLS_MAX)
		return -EINVAL;

	desc = (struct desc_struct*)&child->thread.regs.context[HOST_TLS0A] + idx - GDT_ENTRY_TLS_MIN;
	if (LDT_empty(&info)) {
		desc->a = 0;
		desc->b = 0;
	} else {
		desc->a = LDT_entry_a(&info);
		desc->b = LDT_entry_b(&info);
	}

	return 0;
}
#endif
