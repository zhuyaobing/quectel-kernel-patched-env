/*
 * linux/arch/p4/mm/pageattr.c
 *
 * ported from linux/arch/i386/mm/pageattr.c:
 *
 * Copyright 2002 Andi Kleen, SuSE Labs.
 * Thanks to Ben LaHaise for precious feedback.
 */

#include <linux/mm.h>
#include <linux/module.h>
#include <asm/cacheflush.h>
#include <asm/pgalloc.h>
#include <asm/host_tasks.h>

static DEFINE_SPINLOCK(cpa_lock);

/*
 * Change the page attributes of an page in the linear mapping.
 *
 * This should be used when a page is mapped with a different caching policy
 * than write-back somewhere - some CPUs do not like it when mappings with
 * different caching policies exist. This changes the page attributes of the
 * in kernel linear mapping too.
 *
 * The caller needs to ensure that there are no conflicting mappings elsewhere.
 * This function only deals with the kernel linear map.
 *
 * Caller must call global_flush_tlb() after this.
 */
int change_page_attr(unsigned long address, int numpages, pgprot_t prot)
{
	unsigned long flags;

#ifdef CONFIG_DEBUG_IOREMAP
	printk("--- changing attr of 0x%08lx, %d pages, flags %x\n", address, numpages, pgprot_val(prot));
#endif

	spin_lock_irqsave(&cpa_lock, flags);
	uk_task_chattr(uk_task_id(), address, numpages << PAGE_SHIFT, pgprot_val(prot));
	spin_unlock_irqrestore(&cpa_lock, flags);

	return 0;
}

EXPORT_SYMBOL(change_page_attr);
