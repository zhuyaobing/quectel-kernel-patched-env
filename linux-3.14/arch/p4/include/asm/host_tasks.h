/*
 *  task-related p4-functions
 * 
 *  Copyright (C) 2003-2014 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#ifndef __HOST_TASKS_H_
#define __HOST_TASKS_H_

/* this should be seen as an abstractionlayer between linux and the p4-library */
/* because including p4.h is very problematic and should be avoided */

void uk_task_init(void);
unsigned int uk_task_activate(unsigned int taskno);

void uk_task_unmap(unsigned int taskno, unsigned long virt, unsigned long len);
int uk_task_map(unsigned int taskno, unsigned long virt, unsigned long phys,
	unsigned long len, unsigned long flags);
void uk_task_flush(unsigned int taskno);
void uk_task_chattr(unsigned int taskno, unsigned long virt, unsigned long len,
	unsigned long flags);

/* only x86 */
void uk_task_map_ioport(unsigned int taskno, unsigned long from, unsigned long num);
void uk_task_unmap_ioport(unsigned int taskno, unsigned long from, unsigned long num);

/* former kmapper */

/* various bits from kinfo */
unsigned int uk_task_kinfo_num_cpu(void);
unsigned int uk_task_kinfo_api_version(void);
const char *uk_task_kinfo_build_id(void);
const char *uk_task_kinfo_asp_id(void);
const char *uk_task_kinfo_psp_id(void);
unsigned int uk_task_kinfo_reg_size(void);
unsigned long long uk_task_kinfo_timeout_resolution(void);
int uk_task_kinfo_has_mips_dsp(void);

#ifdef CONFIG_P4_ARM
int uk_task_kinfo_has_fpu(void);
#endif
#ifdef CONFIG_P4_PPC
int uk_task_kinfo_pvr(void);
#endif

/* kernel memory handling */
int uk_kmem_try_thread(void);
int uk_kmem_try_iopbm(void);
int uk_kmem_try_mapping(void);

/* cache alignment handling */
unsigned long uk_get_align_mask(void);

/* total / free memory in the microkernel memory allocator, in pages */
unsigned long uk_global_total(void);
unsigned long uk_global_free(void);

/* returns total / free kernel memory of the Linux resource partition */
unsigned long uk_kmem_total(void);
unsigned long uk_kmem_free(void);

static inline unsigned int uk_task_id(void)
{
	extern unsigned int __pikeos_taskid;

	return __pikeos_taskid;
}

static inline unsigned int uk_task_cpu_mask(void)
{
	extern unsigned long __pikeos_cpumask;

	return __pikeos_cpumask;
}

static inline unsigned int uk_task_max_threads(void)
{
	extern unsigned int __pikeos_max_threads;

	return __pikeos_max_threads;
}

static inline unsigned int uk_task_mcp(void)
{
	extern unsigned int __pikeos_mcp;

	return __pikeos_mcp;
}

static inline unsigned int uk_task_first_child_task(void)
{
	extern unsigned int __pikeos_first_child_task;

	return __pikeos_first_child_task;
}

static inline unsigned int uk_task_last_child_task(void)
{
	extern unsigned int __pikeos_last_child_task;

	return __pikeos_last_child_task;
}

#endif
