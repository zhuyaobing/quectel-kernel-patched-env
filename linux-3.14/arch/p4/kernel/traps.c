/* Copyright (c) 2016 OpenSynergy GmbH */
/*
 * Copyright (C) 2000, 2001 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 *
 * merged from different files from arch/uml/kernel
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/hardirq.h>
#include <linux/version.h>
#include <asm/pgtable.h>
#include <asm/pgalloc.h>
#include <asm/tlbflush.h>
#include <asm/current.h>
#include <asm/page.h>
#include <asm/processor.h>
#include <asm/internals.h>
#include <asm/host_tasks.h>
#include <asm/cacheflush.h>
#include <asm/vmapi_calls.h>

#include <asm/host_context.h>

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,12,0)
static inline bool pagefault_disabled(void)
{
	return in_atomic();
}
#endif

/*
 * Note this is constrained to return 0, -EFAULT, -EACCESS, -ENOMEM by
 * segv().
 */
int handle_page_fault(unsigned long address, unsigned long error_code,
		      int is_user, int *code_out)
{
	struct mm_struct *mm = current->mm;
	struct vm_area_struct *vma;
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	int err = -EFAULT;
	int err2;
	int is_write = error_code & HOST_PF_WRITE;
	unsigned int flags = FAULT_FLAG_ALLOW_RETRY | FAULT_FLAG_KILLABLE;

	*code_out = SEGV_MAPERR;

	/*
	 * If the fault was during atomic operation, don't take the fault, just
	 * fail.
	 */
	if (in_atomic())
		goto out_nosemaphore;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)
	if (is_user)
		flags |= FAULT_FLAG_USER;
#endif

retry:
	down_read(&mm->mmap_sem);
	vma = find_vma(mm, address);
	if (unlikely(!vma))
		goto out;
	else if (likely(vma->vm_start <= address))
		goto good_area;
	else if (unlikely(!(vma->vm_flags & VM_GROWSDOWN)))
		goto out;
	else if (unlikely(expand_stack(vma, address)))
		goto out;

good_area:
	*code_out = SEGV_ACCERR;
	if (is_write) {
		if (!(vma->vm_flags & VM_WRITE))
			goto out;
		flags |= FAULT_FLAG_WRITE;
	} else {
		/* Don't require VM_READ|VM_EXEC for write faults! */
		if (!(vma->vm_flags & (VM_READ | VM_EXEC)))
			goto out;
	}

#ifdef CONFIG_P4_X86
	/* NX (Not eXecute) handling */
	if (error_code & HOST_PF_EXEC) {
		/* PikeOS logic: when PAE and NX is available, NX is active */
#ifdef CONFIG_X86_32
		if (cpu_has_pae && cpu_has_nx)
#endif
		{
			if (!(vma->vm_flags & VM_EXEC))
				goto out;
		}
	}
#endif

	do {
		int fault;

		fault = handle_mm_fault(mm, vma, address, flags);
		if ((fault & VM_FAULT_RETRY) && fatal_signal_pending(current))
			goto out_nosemaphore;

		if (unlikely(fault & VM_FAULT_ERROR)) {
			if (fault & VM_FAULT_OOM) {
				goto out_of_memory;
			} else if (fault & VM_FAULT_SIGBUS) {
				err = -EACCES;
				goto out;
			}
			BUG();
		}
		if (flags & FAULT_FLAG_ALLOW_RETRY) {
			if (fault & VM_FAULT_MAJOR)
				current->maj_flt++;
			else
				current->min_flt++;
			if (fault & VM_FAULT_RETRY) {
				flags &= ~FAULT_FLAG_ALLOW_RETRY;
				flags |= FAULT_FLAG_TRIED;

				goto retry;
			}
		}

		pgd = pgd_offset(mm, address);
		pud = pud_offset(pgd, address);
		pmd = pmd_offset(pud, address);
		pte = pte_offset_kernel(pmd, address);
	} while (!pte_present(*pte));
	err = 0;
	/*
	 * The below warning was added in place of
	 *	pte_mkyoung(); if (is_write) pte_mkdirty();
	 * If it's triggered, we'd see normally a hang here (a clean pte is
	 * marked read-only to emulate the dirty bit).
	 * However, the generic code can mark a PTE writable but clean on a
	 * concurrent read fault, triggering this harmlessly. So comment it out.
	 */
#if 0
	WARN_ON(!pte_young(*pte) || (is_write && !pte_dirty(*pte)));
#endif

	/* may fail due to shortage of host kernel memory */
	err2 = flush_tlb_pagefault(mm, address);
	if (err2 < 0) {
		printk(KERN_WARNING "VM: system out of kernel memory "
				    "for process %s\n", current->comm);
		err = -ENOMEM;
	} else if (err2 > 0) {
		/* read-only I/O mappings */
		err = -EFAULT;
	}
out:
	up_read(&mm->mmap_sem);
out_nosemaphore:
	return err;

out_of_memory:
	/*
	 * We ran out of memory, call the OOM killer, and return the userspace
	 * (which will retry the fault, or kill us if we got oom-killed).
	 */
	up_read(&mm->mmap_sem);
	if (!is_user)
		goto out_nosemaphore;
	pagefault_out_of_memory();
	return 0;
}
EXPORT_SYMBOL(handle_page_fault);

void user_segv(struct pt_regs *regs)
{
	unsigned long address;
	struct siginfo si;
	int err;

	BUG_ON(current->mm == NULL);

	address = pt_regs_get_fault_addr(regs);

	err = handle_page_fault(address, pt_regs_get_ex_pf(regs), 1, &si.si_code);
#ifdef  CONFIG_SHOW_USER_EXCEPTIONS
#ifndef CONFIG_SHOW_USER_EXCEPTIONS_ALL
	if (err)
#endif
	{
		printk("#### TRAP_SEG pid:%d,%s, addr:0x%lx, ip:0x%lx, flag:0x%x, ret:%d\n",
		       task_pid_nr(current), current->comm, address,
		       PT_REGS_IP(regs), pt_regs_get_ex_pf(regs), err);
		//show_regs(regs);
	}
#endif /* CONFIG_SHOW_USER_EXCEPTIONS */
	if (err == 0)
		return;

	if (err == -EACCES) {
		si.si_signo = SIGBUS;
		si.si_errno = 0;
		si.si_code = BUS_ADRERR;
		si.si_addr = (void *)address;
		//current->thread.arch.faultinfo = fi;
		force_sig_info(SIGBUS, &si, current);
	} else if (err == -ENOMEM) {
		printk("VM: killing process %s\n", current->comm);
		do_exit(SIGKILL);
	} else {
		BUG_ON(err != -EFAULT);
		si.si_signo = SIGSEGV;
		si.si_addr = (void *) address;
		//UML: current->thread.arch.faultinfo = fi;
		force_sig_info(SIGSEGV, &si, current);
	}
}

static int raise_error(struct notifier_block *this, unsigned long event, void *ptr)
{
	vmapi_partition_error(0, ptr);
	return NOTIFY_DONE;
}

static struct notifier_block panic_block = {
	.notifier_call = raise_error,
};

/* this one is called by start_kernel */
void __init trap_init(void)
{
	/* Setup panic notifier */
	atomic_notifier_chain_register(&panic_notifier_list, &panic_block);
}
