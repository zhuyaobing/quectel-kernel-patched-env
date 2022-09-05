/* Copyright (c) 2016 OpenSynergy GmbH */
/*
 * linux/arch/p4/kernel/cpu.c
 *
 * Copyright (C) 2006-2009 SYSGO AG (azu@sysgo.de)
 *
 * CPU enumeration
 */

#include <linux/sched.h>
#include <linux/utsname.h>
#include <linux/errno.h>
#include <linux/seq_file.h>
#include <linux/delay.h>
#include <linux/cpu.h>
#include <asm/processor.h>
#include <asm/current.h>
#include <asm/host_tasks.h>
#include <asm/kernthreads.h>

#ifdef CONFIG_P4_BSP_ARCH_MXC
extern unsigned int system_rev;
extern unsigned int system_serial_low;
extern unsigned int system_serial_high;
extern const char *machine_name;
#endif

struct cpuinfo_p4 cpu_data[NR_CPUS];

static int show_cpuinfo(struct seq_file *m, void *v)
{
	unsigned long t, f, l;
	int index = 0;

#ifdef CONFIG_SMP
	index = (struct cpuinfo_p4 *) v - cpu_data;
	if (!cpu_online(index))
		return 0;
#endif

	/* currently, only one CPU is supported */
	seq_printf(m, "processor\t: %d\n", index);
	seq_printf(m, "host\t\t: %s\n", "P4 microkernel");
	seq_printf(m, "arch\t\t: %s\n", init_uts_ns.name.machine);
	seq_printf(m, "bogomips\t: %lu.%02lu\n",
		   loops_per_jiffy/(500000/HZ),
		   (loops_per_jiffy/(5000/HZ)) % 100);

	/* P4 specific information */
	seq_printf(m, "ukernel task\t: %d\n", uk_task_id());
	if (uk_task_kinfo_num_cpu() == 0)
		seq_printf(m, "ukernel CPU\t: #0 (UP host)\n");
	else
		seq_printf(m, "ukernel CPU\t: #%d (SMP host, %d CPUs)\n",
			   kthread_native_cpu_id(index), uk_task_kinfo_num_cpu());
	seq_printf(m, "ukernel version\t: 0x%08x\n", uk_task_kinfo_api_version());
	seq_printf(m, "ukernel build\t: %s\n", uk_task_kinfo_build_id());
	seq_printf(m, "ukernel ASP ID\t: %s\n", uk_task_kinfo_asp_id());
	seq_printf(m, "ukernel PSP ID\t: %s\n", uk_task_kinfo_psp_id());
	seq_printf(m, "ukernel threads\t: %d\n", uk_task_max_threads());
	seq_printf(m, "ukernel prio\t: %d\n", uk_task_mcp());
	f = uk_task_first_child_task();
	l = uk_task_last_child_task();
	seq_printf(m, "child tasks\t: %lu (%lu..%lu)\n", l - f + 1, f, l);

	t = uk_global_total() * PAGE_SIZE / 1024;
	f = uk_global_free() * PAGE_SIZE / 1024;
	seq_printf(m, "total ukmemory\t: %luk\n", t);
	seq_printf(m, "used ukmemory\t: %luk\n", t-f);
	seq_printf(m, "free ukmemory\t: %luk\n", f);

	t = uk_kmem_total() * PAGE_SIZE / 1024;
	if (t != 0) {
		f = uk_kmem_free() * PAGE_SIZE / 1024;
		seq_printf(m, "total kmem\t: %luk\n", t);
		seq_printf(m, "used kmem\t: %luk\n", t-f);
		seq_printf(m, "free kmem\t: %luk\n", f);
	}

	seq_printf(m, "\n");

#ifdef CONFIG_P4_BSP_ARCH_MXC
	/*
	 * Native ARM prints the hardware information after the CPUs. Contrary
	 * to ARM, P4 actually uses the c_next() iterator and
	 * calls show_cpuinfo() for every CPU. As this makes detecting the
	 * last call to show_cpuinfo() difficult, print the hardware
	 * information after the first CPU.
	 */
	if (index == 0) {
		seq_printf(m, "Hardware\t: %s\n", machine_name);
		seq_printf(m, "Revision\t: %04x\n", system_rev);
		seq_printf(m, "Serial\t\t: %08x%08x\n",
			system_serial_high, system_serial_low);
		seq_printf(m, "\n");
	}
#endif

	return 0;
}

static void *c_start(struct seq_file *m, loff_t *pos)
{
	return *pos < NR_CPUS ? cpu_data + *pos : NULL;
}

static void *c_next(struct seq_file *m, void *v, loff_t *pos)
{
	++*pos;
	return c_start(m, pos);
}

static void c_stop(struct seq_file *m, void *v)
{
}

struct seq_operations cpuinfo_op = {
	.start	= c_start,
	.next	= c_next,
	.stop	= c_stop,
	.show	= show_cpuinfo,
};

static struct cpu per_cpu_struct_cpu[NR_CPUS];

static int __init topology_init(void)
{
	unsigned int cpu;

	for_each_possible_cpu(cpu) {
		register_cpu(&per_cpu_struct_cpu[cpu], cpu);
	}

	return 0;
}

subsys_initcall(topology_init);
