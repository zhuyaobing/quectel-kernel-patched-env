/*
 * linux/arch/p4/mm/init.c
 *
 * 2004-01-22, azu@sysgo.de: fit virt-virt mapping
 * 2004-05-13, azu@sysgo.de: major cleanup
 * 2006-05-31, azu@sysgo.de: ported to linux 2.6.15
 */

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/bootmem.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/init.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/vmalloc.h>
#include <linux/initrd.h>
#include <linux/version.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/pgalloc.h>
#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/tlb.h>
#include <asm/internals.h>
#include <asm/iomem.h>
#include <asm/debugout.h>
#include <asm/host_tasks.h>
#include <asm/host_cache.h>



#define ROUND_DOWN(addr)	\
		((void *)(((unsigned long) addr) & PAGE_MASK))
#define ROUND_UP(addr)	\
		ROUND_DOWN(((unsigned long) addr) + PAGE_SIZE - 1)


/* For some reason, mmu_gathers are referenced when CONFIG_SMP is off. */
DEFINE_PER_CPU(struct mmu_gather, mmu_gathers);


pgd_t swapper_pg_dir[PTRS_PER_PGD];

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,12,0)
extern char __binary_start;
extern char __init_begin[], __init_end[];
#endif

unsigned long *empty_zero_page = NULL;

#define PHYS_PFN_UP(x)	((__pa(x) + PAGE_SIZE-1) >> PAGE_SHIFT)
#define PHYS_PFN_DOWN(x)	(__pa(x) >> PAGE_SHIFT)

static unsigned long pfn_min;
static unsigned long pfn_max;

/* memory offset for phys to virt and vv translations */
unsigned long kseg_phys_offset;

int bootmem_retired;

void __init setup_boot_memory(void)
{
	unsigned long start, size;
	unsigned long phys;
	unsigned long brk_end;
	unsigned long pfn_res;
	unsigned long bootmap_size;
	int found;

	/* reserve kernel image */
	brk_end = (unsigned long) ROUND_UP((unsigned long) &_end);

	/* probe for memory in KSEG */
	found = iomem_find_bootram(&start, &size, &phys);
	kseg_phys_offset = phys - start;

	if (!found)
		die_early("setup_boot_memory: no memory found!");

#ifdef CONFIG_P4_FLUSH_CACHE_AT_BOOT
	/* flush kseg0 from caches, some device drivers expect flushed memory */
	uk_flush_dcache_range(start, size);
#endif

	/*
	 * Our virtual memory must start at address zero, otherwise
	 * pfn doesn't point to our virtual address space
	 */
	size += start;
	start = 0;

	pfn_res = PHYS_PFN_UP(brk_end);
	pfn_min = PHYS_PFN_UP(start);
	pfn_max = PHYS_PFN_DOWN(start+size);

	/*
	 * Initialize the boot-time allocator
	 */
	bootmap_size = init_bootmem_node(NODE_DATA(0), pfn_res, pfn_min, pfn_max);


	free_bootmem(PFN_PHYS(pfn_res), PFN_PHYS(pfn_max - pfn_res));
	reserve_bootmem(PFN_PHYS(pfn_res), bootmap_size, BOOTMEM_DEFAULT);

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,12,0)
	num_physpages = pfn_max - pfn_min;
#endif
	max_mapnr = pfn_max;
}

/*
 * paging_init() sets up the page tables, initialises the zone memory
 * maps, and sets up the zero page, bad page and bad page tables.
 */
void __init paging_init(void)
{
	unsigned long zones_size[MAX_NR_ZONES];
	int zone;

	/*
	 * allocate the zero page.  Note that we count on this going ok.
	 */
	empty_zero_page = (unsigned long *) alloc_bootmem_low_pages(PAGE_SIZE);

	/*
	 * Initialise the zone size information.
	 */
	for (zone = 0; zone < MAX_NR_ZONES; zone++) {
		zones_size[zone]  = 0;
	}

	/*
	 * The size of this node has already been determined.
	 * If we need to do anything fancy with the allocation
	 * of this memory to the zones, now is the time to do
	 * it.
	 */
	 /* phys ram, tag it as DMA */
	zones_size[ZONE_DMA] = pfn_max - pfn_min;
	free_area_init(zones_size);

	/* clear the zero-page */
	memset((void *) empty_zero_page, 0, PAGE_SIZE);
}

/* called from init/main.c: print some pretty message and retire the boot allocator*/
void __init mem_init(void)
{
	unsigned int t, f;

	BUG_ON(!mem_map);

	/* this will put all memory onto the freelists */
	totalram_pages = free_all_bootmem();
	/* must be set to fix error message from ll block layer */
	max_low_pfn = pfn_max;
	bootmem_retired = 1;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)
	mem_init_print_info(NULL);
#else
	printk(KERN_INFO "Memory: %luk available\n",
	       (unsigned long) nr_free_pages() << (PAGE_SHIFT-10));
#endif

	high_memory = (void *) __va(pfn_max << PAGE_SHIFT);

	printk(KERN_INFO "vmalloc area: 0x%08lx - 0x%08lx\n",
	       VMALLOC_START, (unsigned long)VMALLOC_END);

	t = uk_global_total() * PAGE_SIZE / 1024;
	f = uk_global_free() * PAGE_SIZE / 1024;
	printk(KERN_INFO "PikeOS memory usage: %dk total, %dk used, %dk free\n",
	       t, t-f, f);
}

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,12,0)
/* This can't do anything because nothing in the kernel image can be freed
 * since it's not in kernel physical memory.
 */
#ifdef CONFIG_P4_X86
void free_init_pages(char *what, unsigned long begin, unsigned long end)
#else
void free_init_pages(const char *what, unsigned long begin, unsigned long end)
#endif
{
	unsigned long addr;

	if (begin >= end)
		return;

	printk(KERN_INFO "Freeing %s: %luk freed\n", what, (end - begin) >> 10);

	addr = begin;

	for (; addr < end; addr += PAGE_SIZE) {
		ClearPageReserved(virt_to_page(addr));
		init_page_count(virt_to_page(addr));
		memset((void *)addr, POISON_FREE_INITMEM, PAGE_SIZE);
		free_page(addr);
		totalram_pages++;
	}
}
#endif

void free_initmem(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)
	free_initmem_default(POISON_FREE_INITMEM);
#else
	free_init_pages("unused kernel memory",
			(unsigned long)(&__init_begin),
			(unsigned long)(&__init_end));
#endif
}

#if defined(CONFIG_BLK_DEV_INITRD) || defined(CONFIG_EXTRACT_ROOTFS)

void free_initrd_mem(unsigned long start, unsigned long end)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)
	free_reserved_area((void *)start, (void *)end, POISON_FREE_INITMEM,
			   "initrd");
#else
	free_init_pages("initrd memory", start, PAGE_ALIGN(end));
#endif
}

#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)
#ifdef CONFIG_P4_X86
/*
 * x86 uses this function for SMP alternatives, on P4Linux this is just a
 * wrapper to free_reserved_area
 */
void free_init_pages(char *what, unsigned long begin, unsigned long end)
{
	free_reserved_area((void *)begin, (void *)end, POISON_FREE_INITMEM, what);
}
#endif
#endif
