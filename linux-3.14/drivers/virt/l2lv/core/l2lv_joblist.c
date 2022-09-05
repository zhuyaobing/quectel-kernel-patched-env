/*
 * Copyright (C) 2013-2014 OpenSynergy GmbH
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this file; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <linux/module.h>
#include <linux/genhd.h>
#include <linux/types.h>
#include <linux/kthread.h>
#include <linux/hrtimer.h>
#include <linux/l2lv/l2lv_hyp_support.h>
#include "l2lv_joblist.h"

#ifdef CHECK_SHDRV_JOBLIST
static void check_shdrv_joblist(struct l2lv_joblist *joblist)
{
	unsigned int cnt = 0;
	struct list_head *h;

	l2lv_lock_joblist(joblist);

	list_for_each(h, &joblist->joblist) {
		++cnt;
	}
	list_for_each(h, &joblist->freelist) {
		++cnt;
	}

	l2lv_unlock_joblist(joblist);

	if (cnt != SHGFX_MAX_RX_JOBS)
		pr_info("Joblist of L2LV is inconsistent\n");

}
#define CHECK_JOBLIST(joblist) check_shdrv_joblist(joblist)
#else
#define CHECK_JOBLIST(joblist)
#endif

void l2lv_joblist_init(struct l2lv_joblist *joblist,
			   l2lv_handle_data_message_t data_callback)
{
	unsigned int i;

	INIT_LIST_HEAD(&joblist->joblist);
	INIT_LIST_HEAD(&joblist->freelist);

	for (i = 0; i < SHGFX_MAX_RX_JOBS; i++)
		list_add(&joblist->jobs[i].node, &joblist->freelist);

	spin_lock_init(&joblist->local_spinlock);
	hyp_spin_init(&joblist->hyp_spinlock);
	init_waitqueue_head(&joblist->threadq);
	joblist->data_callback = data_callback;
}

void l2lv_joblist_exit(struct l2lv_joblist *joblist)
{
	kthread_stop(joblist->thread);
}

void l2lv_joblist_push(struct l2lv_joblist *joblist,
			   struct l2lv_link *link)
{
	struct l2lv_job *job;

	if (list_empty(&joblist->freelist)) {
		pr_info("Joblist out of free elements: Loosing messages\n");
		return;
	}

	job = list_first_entry(&joblist->freelist, struct l2lv_job, node);
	list_del(&job->node);
	job->link = link;
	job->parent = joblist;
	list_add_tail(&job->node, &joblist->joblist);
	smp_mb();
	wake_up(&joblist->threadq);
}

struct l2lv_link *l2lv_joblist_pop(struct l2lv_joblist *joblist)
{
	struct l2lv_job *job = NULL;
	struct l2lv_link *link = NULL;

	might_sleep();
	wait_event(joblist->threadq, !list_empty(&joblist->joblist));

	l2lv_lock_joblist(joblist);
	if (list_empty(&joblist->joblist))
		goto out;

	job = list_first_entry(&joblist->joblist,
			struct l2lv_job,
			node);
	list_del(&job->node);
	link = job->link;
	job->link = NULL;
	list_add(&job->node, &joblist->freelist);
	CHECK_JOBLIST(joblist);

out:
	l2lv_unlock_joblist(joblist);

	return link;
}

void l2lv_lock_joblist(struct l2lv_joblist *joblist)
{
	/*
	 * We need to grab two locks:
	 *  - local_spinlock is to protect against IRQ on same core
	 *  - hyp_spinlock is to protect against IRQ on different core
	 */
	spin_lock_irqsave(&joblist->local_spinlock, joblist->flags);
	hyp_spin_lock(&joblist->hyp_spinlock);
}

void l2lv_unlock_joblist(struct l2lv_joblist *joblist)
{
	hyp_spin_unlock(&joblist->hyp_spinlock);
	spin_unlock_irqrestore(&joblist->local_spinlock, joblist->flags);
}
