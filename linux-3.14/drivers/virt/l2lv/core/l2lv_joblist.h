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
#ifndef _L2LV_JOBLIST_H_
#define _L2LV_JOBLIST_H_

#include <linux/spinlock_types.h>
#include <linux/types.h>
#include <linux/wait.h>

#include <linux/l2lv/l2lv_hyp_support.h>

#define SHGFX_MAX_RX_JOBS (512)

struct l2lv_link;
struct l2lv_joblist;

typedef void (*l2lv_handle_data_message_t)(struct l2lv_link *link,
					       int size);

struct l2lv_job {
	struct list_head node;
	struct l2lv_link *link;
	struct l2lv_joblist *parent;
};

struct l2lv_joblist {
	spinlock_t local_spinlock;
	hyp_spin_t hyp_spinlock;
	struct list_head joblist;
	struct list_head freelist;
	struct l2lv_job jobs[SHGFX_MAX_RX_JOBS];
	unsigned long flags;
	wait_queue_head_t threadq;
	struct task_struct *thread;
	l2lv_handle_data_message_t data_callback;
};

void l2lv_joblist_init(struct l2lv_joblist *joblist,
			   l2lv_handle_data_message_t data_callback);
void l2lv_joblist_exit(struct l2lv_joblist *joblist);

void l2lv_lock_joblist(struct l2lv_joblist *joblist);
void l2lv_unlock_joblist(struct l2lv_joblist *joblist);

/*
 * Usually called in interrupt context only, grabbing hyp_spinlock is required.
 *
 * If not called from interrupt context, locking with l2lv_lock_joblist is
 * needed.
 */
void l2lv_joblist_push(struct l2lv_joblist *joblist,
			   struct l2lv_link *link);
struct l2lv_link *l2lv_joblist_pop(struct l2lv_joblist *joblist);

#endif				/* _L2LV_JOBLIST_H_ */
