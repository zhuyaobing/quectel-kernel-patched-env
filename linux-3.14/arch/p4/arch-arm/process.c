/*
 * linux/arch/p4/arch-arm/process.c - ARM dump registers
 *
 * 2009-09-22, david.engraf@sysgo.com: create file for dump_fpu
 * 2013-10-09, david.engraf@sysgo.com: add gate functions copied from orig
 */

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <asm/elf.h>

/*
 * fill in the fpe structure for a core dump...
 */
int dump_fpu (struct pt_regs *regs, struct user_fp *fp)
{
#if 0
	struct thread_info *thread = current_thread_info();
	int used_math = thread->used_cp[1] | thread->used_cp[2];

	if (used_math)
		memcpy(fp, &thread->fpstate.soft, sizeof (*fp));

	return used_math != 0;
#endif
	printk(KERN_ERR "FIXME: %s not complete\n", __FUNCTION__);
	return 0;
}
EXPORT_SYMBOL(dump_fpu);

/*
 * The vectors page is always readable from user space for the
 * atomic helpers and the signal restart code. Insert it into the
 * gate_vma so that it is visible through ptrace and /proc/<pid>/mem.
 */
static struct vm_area_struct gate_vma;

static int __init gate_vma_init(void)
{
	gate_vma.vm_start	= 0xffff0000;
	gate_vma.vm_end		= 0xffff0000 + PAGE_SIZE;
	gate_vma.vm_page_prot	= PAGE_READONLY_EXEC;
	gate_vma.vm_flags	= VM_READ | VM_EXEC |
				  VM_MAYREAD | VM_MAYEXEC;
	return 0;
}
arch_initcall(gate_vma_init);

struct vm_area_struct *get_gate_vma(struct mm_struct *mm)
{
	return &gate_vma;
}

int in_gate_area(struct mm_struct *mm, unsigned long addr)
{
	return (addr >= gate_vma.vm_start) && (addr < gate_vma.vm_end);
}

int in_gate_area_no_mm(unsigned long addr)
{
	return in_gate_area(NULL, addr);
}

const char *arch_vma_name(struct vm_area_struct *vma)
{
	return (vma == &gate_vma) ? "[vectors]" : NULL;
}
