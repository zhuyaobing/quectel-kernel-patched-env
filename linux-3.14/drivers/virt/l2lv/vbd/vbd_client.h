/* Copyright (C) 2013-2014 OpenSynergy GmbH
 * This file is part of the OpenSynergy L2LV framework
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
#ifndef _L2LV_VBD_H_
#define _L2LV_VBD_H_

#include <linux/types.h>

/* userspace doesn't need the vbd_device structure */
#ifdef __KERNEL__

#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/wait.h>

#include <linux/l2lv/l2lv.h>
#include <linux/l2lv/l2lv_defs.h>
#include <linux/l2lv/l2lv_link.h>

#define VBD_SHM_NAME	"shm:/shm_vbd"

struct request;

struct vbd_link {
	struct l2lv_link   *dev_link;
	char				shm_name[L2LV_MAX_SHM_NAMELEN];

	struct task_struct	*thread;
	wait_queue_head_t	wait_q;
	spinlock_t			waitq_lock;
	struct list_head	req_list;
	struct mutex 		send_lock;
};

struct vbd_device {
	int					vbd_id;
	int					opened;

	struct gendisk		*disk;
	spinlock_t			queue_lock;
	uint32_t         bufoffs;
	uint32_t         bufsize;
};

#endif /* __KERNEL__ */

#endif /* _L2LV_VBD_H_ */
