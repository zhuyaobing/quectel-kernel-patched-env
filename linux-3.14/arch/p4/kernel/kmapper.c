/*
 *  vm-mapping for the kernel
 * 
 *  Copyright (C) 2003-2011 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/kallsyms.h>
#include <asm/page.h>
#include <asm/processor.h>
#include <asm/tlbflush.h>
#include <asm/host_exception.h>
#include <asm/internals.h>
#include <asm/host_tasks.h>
#include <asm/host_threads.h>

static void show_exception(struct pt_regs *regs, unsigned int uid, char *bug_msg)
{
	unsigned int fault_flags;
	char msg[120];

	if (!bug_msg[0]) {
		/* construct message */
		switch (pt_regs_get_ex_type(regs)) {
		case HOST_TRAP_SEG:
			/* page fault */
			fault_flags = regs->context[HOST_EX_CODE];
			snprintf(msg, sizeof(msg), "#### TRAP_SEG: pagefault at 0x%lx, %c%c%c%c\n",
					pt_regs_get_fault_addr(regs),
					(fault_flags & HOST_PF_NOMAP) ? 'n' : '-',
					(fault_flags & HOST_PF_READ)  ? 'r' : '-',
					(fault_flags & HOST_PF_WRITE) ? 'w' : '-',
					(fault_flags & HOST_PF_EXEC)  ? 'x' : '-');
			break;

		case HOST_TRAP_BUS:
			fault_flags = regs->context[HOST_EX_CODE];
			snprintf(msg, sizeof(msg), "#### TRAP_BUS: %s at 0x%lx\n",
					regs->context[HOST_EX_CODE] & HOST_PF_ALIGN ? "alignment fault" : "fault",
					pt_regs_get_fault_addr(regs));
			break;

		case HOST_TRAP_NOT:
			snprintf(msg, sizeof(msg), "#### TRAP_NOT: huh? should not happen\n");
			break;

		default:
			/* other exception */
			snprintf(msg, sizeof(msg), "#### TRAP_OTHER(%d)\n",
					pt_regs_get_ex_type(regs));
			break;
		}
	}

	/* thread state */
	printk(KERN_INFO "#### kernel exception in %u.%u, type:%u\n",
		uk_uid_get_task(uid), uk_uid_get_thread(uid),
		pt_regs_get_ex_type(regs));
	printk(KERN_INFO "%s", bug_msg[0] ? bug_msg : msg);
	printk(KERN_INFO "#### IP: ");
	print_ip_sym(PT_REGS_IP(regs));
	show_regs(regs);
	show_stack(NULL, (unsigned long*)(regs->context[HOST_SP]) - 4);
}

/*
 * generic exception handler for all kernel threads
 */
void kernel_exception_handler(void)
{
	struct pt_regs registers;
	struct pt_regs *regs = &registers;
	unsigned int old_owner;
	unsigned int uid;
	int handled;
	char msg[120];

	/*
	 * prepare first message message
	 */
	uid = 0;
	regs->is_user = 0;

	do {
		/* wait for next exceptions */
		reply_and_wait_for_exception(regs, &uid);

		/* if the faulting thread holds a CPU's IRQ lock, steal it */
		old_owner = arch_local_irq_steal(uid);

		/*
		 * try to recover from different kind of exceptions, ignore
		 * TRAP_NOT
		 */
		handled = 0;
		msg[0] = 0;
		if (pt_regs_get_ex_type(regs) != HOST_TRAP_NOT)
			handled = fixup_exception(PT_REGS_IP(regs), regs);

#ifdef CONFIG_GENERIC_BUG
		/*
		 * if not already handled, check for BUG on supported
		 * architectures and print additional information
		 */
		if (!handled) {
			switch (report_bug(PT_REGS_IP(regs), regs)) {
				case BUG_TRAP_TYPE_BUG:
					snprintf(msg, sizeof(msg), "#### TRAP_BUG\n");
					break;
				case BUG_TRAP_TYPE_WARN:
					/* report_bug already dumped all information,
					* just continue
					*/
					PT_REGS_IP(regs) += 4;
					handled = 1;
					break;
				case BUG_TRAP_TYPE_NONE:
					break;
				default:
					snprintf(msg, sizeof(msg), "#### TRAP_BUG?\n");
					break;
			}
		}
#endif

		if (handled) {
#ifdef CONFIG_SHOW_KERNEL_EXCEPTIONS
			show_exception(regs, uid, msg);
#endif
			arch_local_irq_unsteal(old_owner);
		}
	} while (handled);

	oops_in_progress = 1;

	console_verbose();
	show_exception(regs, uid, msg);

	/* beware the 46 character limit of HM error messages! */
	panic("EXC %u %u.%u %08lx %08lx %08lx",
		pt_regs_get_ex_type(regs), uk_uid_get_task(uid), uk_uid_get_thread(uid),
		PT_REGS_IP(regs), PT_REGS_SP(regs), pt_regs_get_fault_addr(regs));
}
