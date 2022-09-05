/*
 *  MMU context allocation is task creation (overcommit version)
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
#include <linux/spinlock.h>
#include <asm/wrapper.h>
#include <asm/debugout.h>
#include <asm/segment.h>
#include <asm/mmu.h>
#include <asm/kernthreads.h>
#include <asm/host_tasks.h>
#include <asm/host_exception.h>
#include <asm/internals.h>

#ifdef DEBUG
#define Dprintk(x) printk x
#else
#define Dprintk(x)
#endif

static spinlock_t mmu_context_lock = __SPIN_LOCK_UNLOCKED(mmu_context_lock);

struct task_elem {
	struct list_head list;
	unsigned int taskid;
	struct mm_struct *mm;
#ifdef CONFIG_P4_X86
	int ioports_mapped;
#endif
};

/* free tasks (no order) */
static struct list_head free;
/* currently active tasks (LRU) */
static struct list_head active;
/* currently inactive tasks (inverse LRU) */
static struct list_head inactive;

/*
 * mmu_task_init()
 *
 * Detect number of child tasks and put them on the free list.
 */
void __init mmu_task_init(void)
{
	unsigned int num_tasks;
	struct task_elem *t;
	unsigned int need;
	unsigned int err;
	unsigned int i;

	INIT_LIST_HEAD(&free);
	INIT_LIST_HEAD(&active);
	INIT_LIST_HEAD(&inactive);

	num_tasks = 0;
	for (i = uk_task_first_child_task(); i <= uk_task_last_child_task(); i++) {
		t = wrapped_malloc(sizeof(*t));
		if (!t)
			die_early("mmu_task_init(): malloc failed");

		t->taskid = i;
		t->mm = NULL;
#ifdef CONFIG_P4_X86
		t->ioports_mapped = 0;
#endif
		list_add(&t->list, &free);
		num_tasks++;

		err = uk_task_activate(i);
		if (err != 0)
			die_early("mmu_task_init(): creating of task %d failed, err:0x%x\n", i, err);
	}

	/* need twice as many task as we have CPUs */
	need = 2 * kthread_native_cpu_num();
	if (num_tasks < need)
		die_early("mmu_task_init(): config error, linux process needs %d child tasks, has %d", need, num_tasks);
}

int init_new_context(struct task_struct *task, struct mm_struct *mm)
{
	mm->context.task = NULL;
	mm->context.taskid = 0;

	return 0;
}

static bool _get_uk_task(struct mm_struct *mm)
{
	struct task_elem *t;
	bool flush = false;

	t = mm->context.task;
	if (t != NULL) {
		Dprintk(("!!active mm=%p task=%d\n", mm, t->taskid));
		goto update_lru;	/* MM has a task already */
	}

	flush = true;
	if (!list_empty(&free)) {
		/* allocate fresh task */
		t = list_first_entry(&free, struct task_elem, list);
		Dprintk(("!!fresh mm=%p task=%d\n", mm, t->taskid));
		/* the LRU update below enqueues the task on the used list */

		BUG_ON(t->mm != NULL);
	} else {
		/* steal inactive task */
		BUG_ON(list_empty(&inactive));
		t = list_first_entry(&inactive, struct task_elem, list);
		Dprintk(("!!inactive mm=%p task=%d\n", mm, t->taskid));
		/* the LRU update below enqueues the task on the used list */

		/* unregister previous mm */
		BUG_ON(t->mm == NULL);
		BUG_ON(t->mm->context.task != t);
		t->mm->context.task = NULL;
		t->mm->context.taskid = 0;
	}

	/* register task <-> mm */
	t->mm = mm;
	mm->context.task = t;
	mm->context.taskid = t->taskid;

update_lru:
	/* keep LRU up to date */
	list_del(&t->list);
	list_add(&t->list, &active);

	return flush;
}

static void _put_uk_task(struct mm_struct *mm)
{
	struct task_elem *t;

	t = mm->context.task;
	Dprintk(("!!put mm=%p task=%d\n", mm, t->taskid));
	BUG_ON(t == NULL);
	BUG_ON(t->mm == NULL);

	/* active -> inactive */
	list_del(&t->list);
	list_add_tail(&t->list, &inactive);
}

static void _free_uk_task(struct mm_struct *mm)
{
	struct task_elem *t;

	t = mm->context.task;
	if (t != NULL) {
		/* active -> free */
		Dprintk(("!!free mm=%p task=%d\n", mm, t->taskid));
		BUG_ON(t->mm == NULL);
		t->mm = NULL;
		list_del(&t->list);
		list_add(&t->list, &free);
	}
}

void destroy_context(struct mm_struct *mm)
{
	unsigned long flags;

	spin_lock_irqsave(&mmu_context_lock, flags);

	_free_uk_task(mm);

	spin_unlock_irqrestore(&mmu_context_lock, flags);
}

void switch_mm(struct mm_struct *prev, struct mm_struct *next, struct task_struct *tsk)
{
	unsigned long flags;
	bool flush = false;
	unsigned int cpu;

	Dprintk(("!!switch\n"));
	if (prev != next) {
		cpu = smp_processor_id();

		spin_lock_irqsave(&mmu_context_lock, flags);

		cpumask_clear_cpu(cpu, mm_cpumask(prev));
		cpumask_set_cpu(cpu, mm_cpumask(next));

		if ((prev != &init_mm) && cpumask_empty(mm_cpumask(prev)))
			_put_uk_task(prev);

		if (next != &init_mm)
			flush = _get_uk_task(next);

		spin_unlock_irqrestore(&mmu_context_lock, flags);

		if (flush) {
			uk_task_flush(next->context.taskid);
#ifdef CONFIG_P4_X86
			{
				struct task_elem *t = next->context.task;
				t->ioports_mapped = 0;
			}
#endif
		}
	}
}

void activate_mm(struct mm_struct *old, struct mm_struct *new)
{
	Dprintk(("!!activate\n"));
	switch_mm(old, new, NULL);
}

void enter_lazy_tlb(struct mm_struct *mm, struct task_struct *tsk)
{
	Dprintk(("!!lazy mm=%p task=?\n", current->mm));
}

void flush_context(void)
{
	Dprintk(("!!flush mm=%p task=?\n", current->mm));
}

#ifdef CONFIG_P4_KMEM_FLUSH
/*
 * flush all INACTIVE tasks (and make them FREE)
 *
 * called by arch/p4/lib/tasks.c:uk_kmem_flush() to free some kmem
 */
void flush_for_kmem(void)
{
	struct task_elem *t;
	unsigned long flags;

	spin_lock_irqsave(&mmu_context_lock, flags);

	while (!list_empty(&inactive)) {
		/* steal inactive task */
		t = list_first_entry(&inactive, struct task_elem, list);

		/* unregister previous mm */
		BUG_ON(t->mm == NULL);
		BUG_ON(t->mm->context.task != t);
		t->mm->context.task = NULL;
		t->mm->context.taskid = 0;

		/* inactive -> free */
		t->mm = NULL;
		list_del(&t->list);
		list_add_tail(&t->list, &free);

#ifdef CONFIG_DEBUG_KMEM
		printk(KERN_INFO "short of kmem, flushing task %d", t->taskid);
#endif

		uk_task_flush(t->taskid);
#ifdef CONFIG_P4_X86
		t->ioports_mapped = 0;
#endif
	}

	spin_unlock_irqrestore(&mmu_context_lock, flags);
}
#endif

#ifdef CONFIG_P4_X86
int ioport_get_mm_state(struct mm_struct *mm)
{
	struct task_elem *t = mm->context.task;

	return t->ioports_mapped;
}

void ioport_update_mm_state(struct mm_struct *mm)
{
	struct task_elem *t = mm->context.task;

	t->ioports_mapped = 1;
}
#endif
