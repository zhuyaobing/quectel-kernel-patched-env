/*
 *  linux/arch/p4/mm/extable.c
 */
#include <linux/module.h>
#include <asm/uaccess.h>

/* this function can be overriden by the architecture (check x86) */
int __attribute__((weak)) arch_fixup(unsigned long address, struct pt_regs *regs)
{
	return 0;
}

/* generic exception handler */
int fixup_exception(unsigned long address, struct pt_regs *regs)
{
	const struct exception_table_entry *fixup;

	fixup = search_exception_tables(address);
	if (fixup) {
		PT_REGS_IP(regs) = fixup->fixup;
		return 1;
	}
	
	return arch_fixup(address, regs);
}
