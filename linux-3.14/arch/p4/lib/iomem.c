/*
 *  IO memory (and ports) management
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 * 
 *  Copyright (C) 2004-2013 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 * Version 2004-01-26, azu: initial
 * Version 2005-01-06, azu: updated to new (almost ...) VM API
 * Version 2006-06-28, azu: new process detection logic
 * Version 2006-12-14, azu: cleanup of iomem subsystem
 * Version 2007-05-02, azu: pinfo is gone, use vm_proc_mem_iterate() now
 * Version 2013-05-16, azu: provide cache attributes from memory requirement
 * Version 2013-06-12, azu: provide caching mode override mechanism:
 *    create properties for each mapping which should be overridden:
 *      <prop_dir name="app/linux/cache_mode">
 *          <prop_uint32 name="VGA" data="0x0f0"/>
 *          <prop_uint32 name="e1000_iomem" data="0x060"/>
 *      </prop_dir>
 *    the entries are indexed by their VMIT name, not existing entries
 *    are silently ignored.
 *
 * Valid cache bits are:
 *   #define P4_M_C_ENABLE       (1U << 4)
 *   #define P4_M_C_WRITEBACK    (1U << 5)
 *   #define P4_M_C_PREFETCH     (1U << 6)
 *   #define P4_M_C_COHERENCY    (1U << 7)
 *
 * The standard VMIT combinations are:
 *   value  CPWE  cache mode
 *  ==================================
 *   0x0f0  1111  write-back (VM_MEM_CACHE_CB)
 *   0x0d0  1101  write-through (VM_MEM_CACHE_WT)
 *   0x080  1000  uncached (VM_MEM_CACHE_INHIBIT)
 *
 *   0x0e0  1110  write combining
 */

#include <vm.h>
#include <asm/memory.h>
#include <asm/iomem.h>
#include <asm/wrapper.h>
#include <asm/debugout.h>
#include <asm/page.h>
#include <asm/pikeos_version.h>

#include <linux/init.h>

#define IOMEM_TYPE_INVALID	0
#define IOMEM_TYPE_RAM		1
#define IOMEM_TYPE_IOMEM	2
#define IOMEM_TYPE_PHYSROM	3
#define IOMEM_TYPE_IOPORT	4

static unsigned int num_proc_memsegs;

struct iomem_desc {
	unsigned char type;
	unsigned char padding;
	unsigned short cache_mode;	/* P4_M_C_{ENABLE|WRITEBACK|PREFETCH|COHERENCY} */
	unsigned long phys;
	unsigned long virt;
	unsigned long size;
};

static struct iomem_desc* iomem_table;
static unsigned int iomem_count;

/* kernel definitions taken from header file to reduce the number of kernel
 * header includes
 */
extern int sprintf(char *buf, const char * fmt, ...);

/*
 * iomem_init()
 *
 * count memory segments mapped for our process
 */
void iomem_init(void)
{
	vm_proc_memseg_t memseg;

	for (num_proc_memsegs = 0;
		 vm_proc_mem_iterate(num_proc_memsegs, &memseg) == VM_E_OK;
		 num_proc_memsegs++) {
		/* NOP */
	}
}

/*
 * iomem_find_bootram()
 *
 * search for bootram in KSEG
 *
 * now limited to CONTIG RAM !!!
 */
int iomem_find_bootram(unsigned long *start, unsigned long *size, unsigned long *phys)
{
	vm_proc_memseg_t memseg;
	unsigned long virt;
	unsigned int i;

	for (i = 0; i < num_proc_memsegs; i++)
	{
		/* XXX this should not fail - assert this? */
		vm_proc_mem_iterate(i, &memseg);

		virt = VM_MEMSEG_ADDR(memseg.virtbase);

		if ((VM_MEMSEG_MEMTYPE(memseg.physbase) == VM_MEM_TYPE_RAM)  &&
		    (VM_MEMSEG_ACCESS(memseg.virtbase) == VM_MEM_ACCESS_RWX) &&
		    (virt == KSEG_BASE) &&
		    (memseg.size >= 2*P4_PAGESIZE) &&
		    (VM_MEMSEG_ISCONTIG(memseg.virtbase)))
		{
			/* found */
			*start = virt;
			*size = memseg.size;
			*phys = VM_MEMSEG_ADDR(memseg.physbase);
			return 1;
		}
	}

	return 0;	/* NOT FOUND */
}

/*
 * internal routine iomem_set_cache_mode()
 *
 * 1. locate memory requirement by name
 * 2. determine where it is mapped in Linux kernel address space
 * 3. apply new cache mode
 */
static __init int iomem_set_cache_mode(const char *name, unsigned int cache_mode)
{
	vm_mem_desc_t mdesc;
	vm_mem_stat_t mstat;
	P4_address_t virt;
	unsigned int i;
	vm_e_t err;

	/* only P4_M_C_{ENABLE, WRITEBACK, PREFETCH, COHERENCY} allowed */
	if (cache_mode & ~0x0f0)
		die_early("invalid cache mode 0x%x for memrq", cache_mode, name);

	/* get memory descriptor for a given name */
#if PIKEOS_VM_VERSION >= 34
	err = vm_mem_lookup(VM_RESPART_MYSELF, name, &mdesc);
#else
	err = vm_mem_lookup(name, &mdesc);
#endif
	if (err != VM_E_OK)
		return err;

	/* status information about a memory object */
	err = vm_mem_stat(VM_RESPART_MYSELF, mdesc, &mstat);
	if (err != VM_E_OK)
		return err;

	if (mstat.otype == VM_MEM_TYPE_IO_PORT || mstat.otype == VM_MEM_TYPE_KMEM)
		return VM_E_INVAL_CONFIG;	/* type mismatch */

	/* locate matching iomem entry by physical address */
	for (i = 0; i < iomem_count; i++) {
		if (iomem_table[i].phys == mstat.paddress)
			break;
	}
	if (i == iomem_count)
		return VM_E_NO_ENT;	/* not mapped in Linux kernel space */

	/* remap in alternative cache mode */
	virt = iomem_table[i].virt;
#if 0
	debugout("found '%s' mapped at 0x%x\n", name, virt);
#endif
	cache_mode |= P4_M_C_UPDATE;
	err = p4_mem_set_attr(P4_TASK_MYSELF, virt, mstat.size, cache_mode, NULL);
	if (err != 0)
		die_early("iomem_remap: cache mode override for '%s' failed, error 0x%x", cache_mode, err);

	return VM_E_OK;
}

/*
 * iomem_fix_cache_mode()
 *
 * try to apply alternative cache settings for a memory section
 * returns zero on success
 */
static __init void iomem_fix_cache_mode(void)
{
	char propname[VM_NAME_LEN + 32];
	char memrqname[VM_NAME_LEN];
	P4_uint32_t cache_mode;
	P4_size_t read_size;
	vm_file_desc_t fd;
	unsigned int i;
	vm_e_t err;

	sprintf(propname, "prop:/app/%s/cache_mode",
#if PIKEOS_VM_VERSION >= 34
		vmapi_get_process_name()
#else
		/* < PikeOS 4.0 */
		vmapi_get_partname()
#endif
		);

	err = vm_open(propname, VM_O_RD, &fd);
	if (err == VM_E_NO_ENT)
		return;
	if (err != VM_E_OK)
		die_early("property '%s' access error 0x%x\n", propname, err);

	/* iterate entries in directory */
	for (i = 0; ; i++) {
		err = vm_read_at(&fd, memrqname, sizeof(memrqname), i, &read_size);
		if (err != VM_E_OK)
			break;

		/* read directory entry */
		err = vm_prop_read(&fd, memrqname, VM_PROP_T_UINT32, sizeof(cache_mode),
		                   &cache_mode, NULL, NULL);
		if (err == VM_E_OK) {
			err = iomem_set_cache_mode(memrqname, cache_mode);

#if 0
			if (err == 0) {
				debugout("iomem_fix_cache_mode: updated cache mode of '%s' to 0x%x\n", memrqname, cache_mode);
			} else {
				debugout("iomem_fix_cache_mode: memrq '%s' not found\n", memrqname);
			}
#endif
		} else {
			debugout("iomem_fix_cache_mode: cannot access '%s', err 0x%x\n", memrqname, err);
		}
	}

	vm_close(&fd);
}

/*
 * iomem_fill_table()
 *
 * find all usable entries
 */
void __init iomem_fill_table(unsigned long *max_mapping)
{
	vm_proc_memseg_t memseg;
	unsigned long virt;
	unsigned long phys;
	unsigned long size;
	unsigned char type;
	unsigned int cache_mode;
	int i, usable;

	/* allocate large enough array to keep all memory entries */
	iomem_table = wrapped_malloc(num_proc_memsegs * sizeof(struct iomem_desc));
	if (!iomem_table)
		die_early("iomem_fill_table(): could not allocate");

	usable = 0;
	*max_mapping = 0;

	for (i = 0; i < num_proc_memsegs; i++)
	{
		/* XXX this should not fail - assert this? */
		vm_proc_mem_iterate(i, &memseg);
		virt = VM_MEMSEG_ADDR(memseg.virtbase);
		phys = VM_MEMSEG_ADDR(memseg.physbase);
		size = memseg.size;
		type = IOMEM_TYPE_INVALID;

		cache_mode = 0;
		switch (VM_MEMSEG_CACHE(memseg.virtbase)) {
		case VM_MEM_CACHE_CB:
			cache_mode = P4_M_C_ENABLE | P4_M_C_WRITEBACK | P4_M_C_PREFETCH;
			break;

		case VM_MEM_CACHE_WT:
			cache_mode = P4_M_C_ENABLE | P4_M_C_PREFETCH;
			break;

		case VM_MEM_CACHE_INHIBIT:
			break;

		default:
			die_early("iomem_fill_table(): unknown VM_MEM_CACHE type 0x%x found, virt 0x%08lx",
			          VM_MEMSEG_CACHE(memseg.virtbase), virt);
			break;
		}
		cache_mode |= P4_M_C_COHERENCY;	/* always set */

		switch (VM_MEMSEG_MEMTYPE(memseg.physbase)) {
		case VM_MEM_TYPE_RAM:
			type = IOMEM_TYPE_RAM;
			break;
		case VM_MEM_TYPE_IO_PORT:
			type = IOMEM_TYPE_IOPORT;
			break;
		case VM_MEM_TYPE_IO_MEM:
			if (VM_MEMSEG_ACCESS(memseg.virtbase) != VM_MEM_ACCESS_RDWR &&
			    VM_MEMSEG_ACCESS(memseg.virtbase) != VM_MEM_ACCESS_RWX)
				die_early("iomem_fill_table(): found read-only IO resource, virt 0x%08lx", virt);

			type = IOMEM_TYPE_IOMEM;
			break;
		case VM_MEM_TYPE_ROM:
			type = IOMEM_TYPE_PHYSROM;
			break;
		case VM_MEM_TYPE_KMEM:
			/* ignored */
			break;
		default:
			die_early("iomem_fill_table(): unknown VM_MEM_TYPE 0x%x found, virt 0x%08lx",
			          VM_MEMSEG_MEMTYPE(memseg.physbase), virt);
			break;
		}

		if (type == IOMEM_TYPE_RAM || type == IOMEM_TYPE_PHYSROM || type == IOMEM_TYPE_IOMEM)
		{
			/* check bounds */
			if ((virt < KSEG_BASE) || (virt + size > KSEG_END))
				die_early("iomem_fill_table(): resource 0x%08lx out of kernel virtual address space", virt);

			/* determine highest address */
			if (virt + size > *max_mapping)
				*max_mapping = virt + size;
		}

		/* save in iomem_table (only IO resources) */
		if (type == IOMEM_TYPE_RAM || type == IOMEM_TYPE_PHYSROM || type == IOMEM_TYPE_IOMEM)
		{
			iomem_table[usable].type = type;
			iomem_table[usable].cache_mode = cache_mode;
			iomem_table[usable].phys = phys;
			iomem_table[usable].virt = virt;
			iomem_table[usable].size = size;

			usable++;
		}
		/* x86 IO port */
		if (type == IOMEM_TYPE_IOPORT)
		{
			iomem_table[usable].type = type;
			iomem_table[usable].cache_mode = 0;
			iomem_table[usable].phys = phys >> 12;
			iomem_table[usable].virt = virt >> 12;
			iomem_table[usable].size = size;

			usable++;
		}
	}

	iomem_count = usable;
	if (iomem_count == 0)
		die_early("iomem_alloc_table(): no valid entries found!");

	iomem_fix_cache_mode();
}

#if 0
/*
 * iomem_dump_table()
 *
 * dump all usable entries with the memory table
 */
void iomem_dump_table(void)
{
	struct iomem_desc *iote;
	unsigned int i;

	debugout("IO memory table:\n");

	for (i = 0; i < iomem_count; i++) {
		iote = &iomem_table[i];
		debugout(" %2d %s phys: 0x%08x virt: 0x%08x"
			 " size: 0x%08x cache: %c%c%c%c\n",
			i,
			iote->type == 1 ? "RAM " :
			(iote->type == 2 ? "IO  " :
			(iote->type == 3 ? "ROM " : "PORT")),
			iote->phys, iote->virt, iote->size,
			iote->cache_mode & P4_M_C_ENABLE    ? 'E' : '-',
			iote->cache_mode & P4_M_C_WRITEBACK ? 'W' : '-',
			iote->cache_mode & P4_M_C_PREFETCH  ? 'P' : '-',
			iote->cache_mode & P4_M_C_COHERENCY ? 'C' : '-');
	}
}
#endif

/*
 * iomem_find_io(phys,size)
 *
 * search for an arbitraty I/O memory in the kernel's memory table
 * returns page aligned kernel virtual address (in kernel address space), or zero on errors
 */
unsigned long __iomem_find_io(unsigned long phys_addr, unsigned long size, unsigned int *cache_mode)
{
	struct iomem_desc *iote;
	unsigned int i;
#ifdef CONFIG_PCI_VIRT
	unsigned long kvirt;
#endif

	for (i = 0; i < iomem_count; i++) {
		iote = &iomem_table[i];

		/* skip over I/O ports */
		if (iote->type == IOMEM_TYPE_IOPORT)
			continue;

		if ((phys_addr >= iote->phys) &&
		    (phys_addr + size - 1 <= iote->phys + iote->size - 1))
		{
			/* fits! */
			if (cache_mode != NULL)
				*cache_mode = iote->cache_mode;

			return phys_addr - iote->phys + iote->virt;
		}
	}

#ifdef CONFIG_PCI_VIRT
	/* Check if PCI MMIO is memory mapped from PCI manager, do that for page aligned requests */
	if ((size) && (!(phys_addr & ~PAGE_MASK)) && (kvirt = p4_pci_find_bar(phys_addr, size)))
		return kvirt;
#endif

	/* not found */
	return 0;
}
