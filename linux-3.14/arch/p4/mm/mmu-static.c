/*
 *  MMU context allocation is task creation (static version)
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

static spinlock_t mmu_context_lock = __SPIN_LOCK_UNLOCKED(mmu_context_lock);

struct task_elem {
	struct list_head list;
	unsigned int taskid;
};

static struct list_head free;
static struct list_head used;

/*
 * mmu_task_init()
 *
 * Detect number of child tasks and put them on the free list.
 */
void __init mmu_task_init(void)
{
	unsigned int num_tasks;
	struct task_elem *t;
	unsigned int err;
	unsigned int i;

	INIT_LIST_HEAD(&free);
	INIT_LIST_HEAD(&used);

	num_tasks = 0;
	for (i = uk_task_first_child_task(); i <= uk_task_last_child_task(); i++) {
		t = wrapped_malloc(sizeof(*t));
		if (!t)
			die_early("mmu_task_init(): malloc failed");

		t->taskid = i;
		list_add(&t->list, &free);
		num_tasks++;

		err = uk_task_activate(i);
		if (err != 0)
			die_early("mmu_task_init(): creating of task %d failed, err:0x%x\n", i, err);
	}

	if (num_tasks == 0)
		die_early("mmu_task_init(): config error, linux process has no child tasks");
}

/*
 * init_new_context(task, mm)
 *
 * allocates a new p4-task
 * returns != 0 on error
 */
int init_new_context(struct task_struct *task, struct mm_struct *mm)
{
	struct task_elem *t;
	unsigned long flags;
	int err;

	/* find free task */
	spin_lock_irqsave(&mmu_context_lock, flags);
	if (!list_empty(&free)) {
		t = list_first_entry(&free, struct task_elem, list);
		list_del(&t->list);
		list_add_tail(&t->list, &used);

		mm->context.task = t;
		mm->context.taskid = t->taskid;

		err = 0;
	} else {
		err = -ENOMEM;
	}
	spin_unlock_irqrestore(&mmu_context_lock, flags);

	return err;
}

/*
 * destroy_context(mm)
 *
 * destroys the mm-context by releasing the accociated p4-task
 */
void destroy_context(struct mm_struct *mm)
{
	struct task_elem *t;
	unsigned long flags;

	t = mm->context.task;
	if (t != NULL) {
		uk_task_flush(t->taskid);

		spin_lock_irqsave(&mmu_context_lock, flags);

		/* move task from "used" to "free" list */
		list_del(&t->list);
		list_add(&t->list, &free);

		spin_unlock_irqrestore(&mmu_context_lock, flags);
	}
}


/*
 * switch_mm(...)
 *
 * switches active task
 */
/* Called in schedule() just before actually doing the switch_to. */
void switch_mm(struct mm_struct *prev, struct mm_struct *next,
		struct task_struct *tsk)
{
	struct task_elem *t;
	unsigned long flags;
	unsigned int cpu;

	if (prev != next) {
		cpu = smp_processor_id();

		cpumask_clear_cpu(cpu, mm_cpumask(prev));
		cpumask_set_cpu(cpu, mm_cpumask(next));

		if (next != &init_mm) {
			spin_lock_irqsave(&mmu_context_lock, flags);

			/* keep LRU up to date */
			t = next->context.task;
			list_del(&t->list);
			list_add(&t->list, &used);

			spin_unlock_irqrestore(&mmu_context_lock, flags);
		}
	}
}

void activate_mm(struct mm_struct *old, struct mm_struct *new)
{
}

void enter_lazy_tlb(struct mm_struct *mm, struct task_struct *tsk)
{
}

/*
 * called by fs/exec.c -> flush_thread() when flushing the previous mappings
 */
void flush_context(void)
{
	struct task_elem *t;
	unsigned long flags;

	force_flush_all();

	spin_lock_irqsave(&mmu_context_lock, flags);

	/* keep LRU up to date */
	t = current->mm->context.task;
	list_del(&t->list);
	list_add(&t->list, &used);

	spin_unlock_irqrestore(&mmu_context_lock, flags);
}

#ifdef CONFIG_P4_X86
int ioport_get_mm_state(struct mm_struct *mm)
{
	return 1;	/* only used in overcommit mode */
}

void ioport_update_mm_state(struct mm_struct *mm)
{
	/* empty, only required for overcommit mode */
}
#endif
