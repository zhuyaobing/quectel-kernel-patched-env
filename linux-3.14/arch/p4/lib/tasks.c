/*
 *  Task-related p4-functions
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 * 
 *  Copyright (C) 2003-2011 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 */

#include "p4.h"
#include "vm.h"
#include <asm/host_tasks.h>
#include <asm/debugout.h>
#include <asm/mmu_context.h>
#include <asm/pgtable.h>

#include <linux/init.h>

/*
 * Initial stack used by linux_main() for low level inits only. The linux kernel
 * threads will run on their specific stacks.
 */
VM_DECLARE_STACK(0x1000)

/*
 * PikeOS task ID of Linux kernel
 */
unsigned int __pikeos_taskid;
unsigned long __pikeos_cpumask;
unsigned int __pikeos_mcp;
unsigned int __pikeos_max_threads;
unsigned int __pikeos_first_child_task;
unsigned int __pikeos_last_child_task;

#ifdef CONFIG_P4_KMEM_FLUSH
static int kmem_flush;
static unsigned int kmem_free_cached;

static void kmem_init(void)
{
	kmem_flush = uk_kmem_total();
	/* total pages of zero -- service not available */
	if (kmem_flush != 0) {
		kmem_free_cached = uk_kmem_free();
	}
}
#else
#define kmem_init() do { } while (0)
#endif

/*
 * uk_task_init()
 *
 * Provide common task attributes for the P4Linux port.
 *
 */
void __init uk_task_init(void)
{
	P4_task_attr_t attr;
	vm_procinfo_t info;

	/* NOTE: __pikeos_taskid is returned by uk_task_id() */
	__pikeos_taskid = p4_my_task();

	/* cannot fail */
	p4_task_get_attr(P4_TASK_MYSELF, &attr, NULL, NULL);

	__pikeos_mcp = attr.mcp;
	__pikeos_cpumask = attr.cpu_mask;
	__pikeos_max_threads = attr.max_threads;

	vm_procinfo(VM_RESPART_MYSELF, VM_PROC_MYSELF, &info);

	__pikeos_first_child_task = info.first_child_task_num;
	__pikeos_last_child_task = info.last_child_task_num;

	kmem_init();
}

/*
 * task_activate()
 *
 * create an empty task envelope
 */
unsigned int uk_task_activate(unsigned int taskno)
{
	P4_task_activate_t ta;
	P4_e_t err;

	ta.name         = "linux userspace";
	ta.max_threads  = 1;
	ta.mcp          = 0;
	ta.rp_id        = P4_RESPART_INHERIT;
	ta.dnotify      = P4_UID_INVALID;
	P4_AB_CLEAR_SET(&ta.ability_mask);
	ta.cpu_mask     = 0;

	err = p4_task_activate(taskno, &ta, 0);

	return err;
}

/*
 * uk_task_unmap
 *
 * unmaps len/PAGESIZE pages beginning at virt in the task described by index
 *
 * returns negative numbers on errors
 * returns the number of unmapped fpages (zero or positive)
 */
void uk_task_unmap(unsigned int taskno, unsigned long virt, unsigned long len)
{
	P4_e_t err;

	err = p4_mem_unmap(taskno, virt, len);

	if (unlikely(err != P4_E_OK))
		die_runtime("uk_task_unmap(task:%d, v:0x%08lx, len:%lu) failed, err:0x%x\n",
			taskno, virt, len, err);
}

/*
 * uk_task_flush_ports
 */
static inline unsigned int uk_task_flush_ports(unsigned int taskno)
{
#ifdef P4_FEATURE_IOPORT
	/* on x86, we have 64K I/O ports */
	return p4_ioport_unmap(taskno, 0, 0x10000);
#else
	return P4_E_OK;
#endif
}

/*
 * uk_task_unmap
 *
 * unmaps len/PAGESIZE pages beginning at virt in the task described by index
 *
 * returns negative numbers on errors
 * returns the number of unmapped fpages (zero or positive)
 */
void uk_task_flush(unsigned int taskno)
{
	P4_e_t err;

	/* on certain systems, deleting + re-creating a task is faster than
	 * flushing all mappings
	 */
#ifdef CONFIG_P4_TASK_FLUSH_TERMINATE
	err = p4_task_terminate(taskno);

	if (err == P4_E_OK)
		err = uk_task_activate(taskno);

#else	/* !CONFIG_P4_TASK_FLUSH_TERMINATE */
	err = p4_mem_unmap(taskno, P4_MEM_USR_BASE, P4_MEM_USR_END);

	if (err == P4_E_OK)
		err = uk_task_flush_ports(taskno);
#endif

	if (unlikely(err != P4_E_OK))
		die_runtime("uk_task_flush(task:%d) failed, err:0x%x\n", taskno, err);
}

/*
 * uk_task_map
 *
 * unmaps first ...
 * then maps len/PAGESIZE pages into address-space/task described by index
 *
 */
int uk_task_map(unsigned int taskno, unsigned long virt, unsigned long phys, unsigned long len,
	 unsigned long flags)
{
	P4_access_t access;
	P4_e_t err;

	access = (flags & _PAGE_P4_MASK) | _PAGE_P4_SET;

	/* KMEM: need kernel memory */
	uk_kmem_try_mapping();

	err = p4_mem_map(taskno, virt, len, phys, access, (void*)0);

	if (unlikely(err != P4_E_OK)) {
		if (err == P4_E_NOKMEM) {
			/* better unmap broken mappings */
#ifdef CONFIG_DEBUG_KMEM
			debugout("uk_task_map(): out of kernel memory\n");
#endif
			p4_mem_unmap(taskno, virt, len);
			return -1;
		} else if (err == P4_E_PERM) {
			/* read-only I/O mappings */
			return 1;
		}

		die_runtime("uk_task_map(task:%d, v:0x%08lx, p:0x%08lx, len:%lu, flags:%x) failed, err:0x%x\n",
			taskno, virt, phys, len, flags, err);
	}
	return 0;
}

#ifdef P4_FEATURE_IOPORT

/*
 * uk_task_map_ioport
 *
 * maps a range of IO-Ports to a existing task
 */
void uk_task_map_ioport(unsigned int taskno, unsigned long from, unsigned long num)
{
	P4_e_t err;

	/* KMEM: need kernel memory */
	uk_kmem_try_iopbm();

	/* FIXME: terribly slow, optimize */
	/* it seems it is impossible to optimize this without knowing which
	 * ports are mapped to us
	 */
	while (num > 0) {
		err = p4_ioport_map(taskno, from, 1);

		from++;
		num--;
	}
}

/*
 * uk_task_unmap_ioport
 *
 * unmaps a range of IO-Ports from a existing task ...
 */
void uk_task_unmap_ioport(unsigned int taskno, unsigned long from, unsigned long num)
{
	P4_e_t err;

	err = p4_ioport_unmap(taskno, from, num);

	if (unlikely(err != P4_E_OK))
		debugout("uk_task_unmap_ioport() unmapping failed, err:0x%x", err);
}

#endif


/*
 * uk_task_chattr
 *
 * changes caching attributes of mapping
 *
 */
void uk_task_chattr(unsigned int taskno, unsigned long virt, unsigned long len, unsigned long flags)
{
	P4_access_t access;
	P4_e_t err;

	access = (flags & _PAGE_CACHE_MASK) | P4_M_C_UPDATE;
	err = p4_mem_set_attr(taskno, virt, len, access, (void*)0);

	if (unlikely(err != P4_E_OK)) {
		die_runtime("uk_task_chattr(task:%d, v:0x%08lx, len:%ld, f:%x) failed, err:0x%x\n",
			taskno, virt, len, access, err);
	}
}

/*
 * return various bits from the kernel info page
 */
unsigned int uk_task_kinfo_num_cpu(void)
{
	return p4_kinfo_get_num_cpu();
}

unsigned int uk_task_kinfo_api_version(void)
{
	return p4_kinfo_get_api_version();
}

const char *uk_task_kinfo_build_id(void)
{
	return p4_kinfo_get_kernel_build_id();
}

const char *uk_task_kinfo_asp_id(void)
{
	return p4_kinfo_get_asp_build_id();
}

const char *uk_task_kinfo_psp_id(void)
{
	return p4_kinfo_get_psp_build_id();
}

unsigned int uk_task_kinfo_reg_size(void)
{
	return p4_kinfo_get_regs_size();
}

unsigned long long uk_task_kinfo_timeout_resolution(void)
{
	return p4_kinfo_get_timeout_resolution();
}

#ifdef CONFIG_P4_ARM
int uk_task_kinfo_has_fpu(void)
{
	return p4_kinfo_arch()->has_fpu;
}
#endif

#ifdef CONFIG_P4_PPC
int uk_task_kinfo_pvr(void)
{
	return p4_kinfo_arch()->pvr;
}
#endif

/* ----------------------------------------------------------------- */

/*
 * uk_respart_total()
 * uk_respart_free()
 *
 * returns total / free kernel memory of the Linux resource partition
 * returns memory in PAGES!
 *
 * The system call was added for PikeOS 3.3. A total number of 0 indicates
 * that this service is not available.
 * For performance reasons, only uk_kmem_total() contains a runtime check.
 */
#if P4_API_VERSION >= 0x30005
unsigned long uk_kmem_total(void)
{
	P4_size_t pages;
	P4_e_t err;

	err = p4_respart_get_kmem(NULL, &pages);
	if (err == P4_E_OK)
		return pages;
	return 0;
}
unsigned long uk_kmem_free(void)
{
	P4_size_t pages;

	p4_respart_get_kmem(&pages, NULL);

	return pages;
}
#else
unsigned long uk_kmem_total(void)
{
	return 0;
}
unsigned long uk_kmem_free(void)
{
	return 0;
}
#endif

/*
 * kernel memory handling
 *
 * free kernel memory (by cleaning address spaces) if memory is needed
 */

#ifdef CONFIG_P4_KMEM_FLUSH
/*
 * uk_kmem_flush(number_of_pages)
 *
 * free up to "pages" pages of kernel memory
 * by flushing least recently used address spaces
 *
 * returns zero if enough pages are available
 */
static int uk_kmem_flush(unsigned int pages)
{
	if (!kmem_flush)
		return 0;

	/* check cached value */
	if (kmem_free_cached >= pages) {
		kmem_free_cached -= pages;
		return 0;
	}

	/* update cached value */
	kmem_free_cached = uk_kmem_free();
	if (kmem_free_cached >= pages) {
		kmem_free_cached -= pages;
		return 0;
	}

	/* flush kmem */
	flush_for_kmem();

	/* update cached value again */
	kmem_free_cached = uk_kmem_free();
	if (kmem_free_cached >= pages) {
		kmem_free_cached -= pages;
		return 0;
	}

	return -1;
}
#else
#define uk_kmem_flush(p) 0
#endif

/*
 * uk_kmem_try_XX()
 *
 * try to reserve kernel memory for object XX
 */
int uk_kmem_try_thread(void)
{
	/* threads need one page */
	return uk_kmem_flush(1);
}

#ifdef P4_FEATURE_IOPORT
/* only available on x86 */
int uk_kmem_try_iopbm(void)
{
	/* x86 IO-Bitmap: two pages */
	return uk_kmem_flush(2);
}
#endif

int uk_kmem_try_mapping(void)
{
	/* pagetable: one page */
	return uk_kmem_flush(1);
}

/*
 * uk_global_total()
 * uk_global_free()
 *
 * returns total / free memory in the microkernel memory allocator
 * returns memory in PAGES!
 */
unsigned long uk_global_total(void)
{
	return p4_kinfopage()->allpages;
}
unsigned long uk_global_free(void)
{
	return p4_kinfo_get_free_pages();
}
