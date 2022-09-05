/*
 * linux/arch/p4/arch-arm/fault.c - ARM exception handling
 *
 * 2009-09-22, David Engraf <den@sysgo.com>
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

int (*alignment_handler)(unsigned long addr, unsigned int fsr, struct pt_regs *regs);

int arch_fixup(unsigned long address, struct pt_regs *regs)
{
	if (pt_regs_is_alignment_fault(regs)) {
#ifdef CONFIG_ALIGNMENT_TRAP
		/* alignment exception */
		if (alignment_handler &&
			alignment_handler(pt_regs_get_fault_addr(regs),
						1, regs) == 0) {
				return 1;
			
		}
#endif
	}

	return 0;
}

void __init
hook_fault_code(int nr, int (*fn)(unsigned long, unsigned int, struct pt_regs *),
		int sig, int code, const char *name)
{
	alignment_handler = fn;
}

#ifdef CONFIG_PROC_FS
static int __init proc_cpu_init(void)
{
	struct proc_dir_entry *res;

	res = proc_mkdir("cpu", NULL);
	if (!res)
		return -ENOMEM;
	return 0;
}
fs_initcall(proc_cpu_init);
#endif
