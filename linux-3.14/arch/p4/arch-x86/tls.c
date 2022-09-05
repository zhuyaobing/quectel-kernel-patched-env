/*
 * tls.c - TLS support for P4/Linux
 *
 * taken from linux/arch/i386/kernel/process.c
 */

#include <linux/sched.h>
#include <asm/elf.h>
#include <asm/ptrace.h>
#include <asm/uaccess.h>
#include <uapi/asm/ldt.h>
#include <asm/ldt.h>

int arch_copy_tls(struct task_struct *new, struct pt_regs *regs)
{
	struct desc_struct *desc = (struct desc_struct*)&new->thread.regs.context[HOST_TLS0A];
	struct user_desc info;
	int idx, ret = -EFAULT;

	if (copy_from_user(&info,
			   (void __user *) PT_REGS_SI(&new->thread.regs),
			   sizeof(info)))
		goto out;

	ret = -EINVAL;
	if (LDT_empty(&info))
		goto out;

	idx = info.entry_number;

	ret = -EINVAL;
	if (idx < GDT_ENTRY_TLS_MIN || idx > GDT_ENTRY_TLS_MAX)
		goto out;;

	desc->a = LDT_entry_a(&info);
	desc->b = LDT_entry_b(&info);
	ret = 0;
out:
	return ret;
}

void arch_clear_tls(struct task_struct *tsk)
{
	/* clean TLS space */
	tsk->thread.regs.context[HOST_TLS0A] = 0;
	tsk->thread.regs.context[HOST_TLS0B] = 0;
	tsk->thread.regs.context[HOST_TLS1A] = 0;
	tsk->thread.regs.context[HOST_TLS1B] = 0;
}


/*
 * sys_alloc_thread_area: get a yet unused TLS descriptor index.
 */
static int get_free_idx(void)
{
	struct desc_struct *desc = (struct desc_struct*)&current->thread.regs.context[HOST_TLS0A];
	int idx;

	for (idx = 0; idx < GDT_ENTRY_TLS_ENTRIES; idx++)
		if (desc_empty(desc + idx))
			return idx + GDT_ENTRY_TLS_MIN;
	return -ESRCH;
}

/*
 * Set a given TLS descriptor:
 */
asmlinkage int sys_set_thread_area(struct user_desc __user *u_info)
{
	struct user_desc info;
	struct desc_struct *desc;
	int cpu, idx;

	if (copy_from_user(&info, u_info, sizeof(info)))
		return -EFAULT;
	idx = info.entry_number;

	/*
	 * index -1 means the kernel should try to find and
	 * allocate an empty descriptor:
	 */
	if (idx == -1) {
		idx = get_free_idx();
		if (idx < 0)
			return idx;
		if (put_user(idx, &u_info->entry_number))
			return -EFAULT;
	}

	if (idx < GDT_ENTRY_TLS_MIN || idx > GDT_ENTRY_TLS_MAX)
		return -EINVAL;

	desc = (struct desc_struct*)&current->thread.regs.context[HOST_TLS0A] + idx - GDT_ENTRY_TLS_MIN;

	/*
	 * We must not get preempted while modifying the TLS.
	 */
	cpu = get_cpu();

	if (LDT_empty(&info)) {
		desc->a = 0;
		desc->b = 0;
	} else {
		desc->a = LDT_entry_a(&info);
		desc->b = LDT_entry_b(&info);
	}

	put_cpu();

	return 0;
}

/*
 * Get the current Thread-Local Storage area:
 */
asmlinkage int sys_get_thread_area(struct user_desc __user *u_info)
{
	struct user_desc info;
	struct desc_struct *desc;
	int idx;

	if (get_user(idx, &u_info->entry_number))
		return -EFAULT;
	if (idx < GDT_ENTRY_TLS_MIN || idx > GDT_ENTRY_TLS_MAX)
		return -EINVAL;

	memset(&info, 0, sizeof(info));

	desc = (struct desc_struct*)&current->thread.regs.context[HOST_TLS0A] + idx - GDT_ENTRY_TLS_MIN;

	info.entry_number = idx;
	info.base_addr = GET_BASE(desc);
	info.limit = GET_LIMIT(desc);
	info.seg_32bit = GET_32BIT(desc);
	info.contents = GET_CONTENTS(desc);
	info.read_exec_only = !GET_WRITABLE(desc);
	info.limit_in_pages = GET_LIMIT_PAGES(desc);
	info.seg_not_present = !GET_PRESENT(desc);
	info.useable = GET_USEABLE(desc);

	if (copy_to_user(u_info, &info, sizeof(info)))
		return -EFAULT;
	return 0;
}
