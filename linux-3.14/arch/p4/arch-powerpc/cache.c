/*
 * linux/arch/p4/arch-powerpc/cache.c - PowerPC cache handling code
 *
 * taken from linux/arch/ppc/mm/init.c
 *
 * 05 july 2004, azu@sysgo.com
 * 14 march 2011, den@sysgo.com
 */

#include <linux/threads.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <asm/pgtable.h>
#include <asm/host_exception.h>
#include <asm/cacheflush.h>

/* void __flush_icache_range(unsigned long start, unsigned long stop)
 * can be done in user space on ppc -> misc.S
 */

/*
 * This is called when a page has been modified by the kernel.
 * It just marks the page as not i-cache clean.  We do the i-cache
 * flush later when the page is given to a user process, if necessary.
 */
void flush_dcache_page(struct page *page)
{
	/* avoid an atomic op if possible */
	if (test_bit(PG_arch_1, &page->flags))
		clear_bit(PG_arch_1, &page->flags);
}
EXPORT_SYMBOL(flush_dcache_page);

void flush_dcache_icache_page(struct page *page)
{
	//printk("--fdip 0x%08x\n", page_address(page));
	__flush_dcache_icache(page_address(page));
}

/* prototypes defined in page.h */
void clear_user_page(void *to, unsigned long vaddr, struct page *page)
{
	clear_page(to);

	flush_dcache_page(page);
}

void copy_user_page(void *to, void *from, unsigned long vaddr, struct page *page)
{
	copy_page(to, from);

	flush_dcache_page(page);
}

/*
 * This is called at the end of handling a user page fault, when the
 * fault has been handled by updating a PTE in the linux page tables.
 */
void update_mmu_cache(struct vm_area_struct *vma, unsigned long address,
		      pte_t *pte)
{
	//printk("--umc 0x%08x\n", address);

	unsigned long pfn = pte_pfn(*pte);
	struct page *page = pfn_to_page(pfn);

	if (!pfn_valid(pfn))
		return;

	//FIXME: check for zero page?

	if (!PageReserved(page)
		&& !test_bit(PG_arch_1, &page->flags)) {
		flush_dcache_icache_page(page);
		set_bit(PG_arch_1, &page->flags);
	}
}
