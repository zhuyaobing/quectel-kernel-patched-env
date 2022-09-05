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

#ifndef _L2LV_BLOCK_JOBLIST_H_
#define _L2LV_BLOCK_JOBLIST_H_

#include <linux/l2lv/l2lv_link.h>
#include <linux/list.h>
#include <linux/kthread.h>
#include "vbd_debug.h"
#include "vbd_defs.h"
#include "vbd_link.h"
#include "vbd_private_if.h"

#define L2LV_BLOCK_MAX_JOBS (20)

typedef struct l2lv_block_job {
    struct list_head      node;
    struct l2lv_channel  *channel;
    union req_msg {
        struct l2lv_msg_header hdr;
        vbd_tx_req_msg_t   tx_msg;
        vbd_capa_req_msg_t sync_msg;
    } req_msg;
    vbd_info           *info;
} l2lv_block_job_t;

typedef struct l2lv_block_joblist {
    struct list_head     list;
    struct list_head     freelist;
    l2lv_block_job_t     jobs[L2LV_BLOCK_MAX_JOBS];
    wait_queue_head_t    waitq;
    struct task_struct  *thread;
    struct mutex         lock;
} l2lv_block_joblist_t;

void l2lv_block_joblist_init   (l2lv_block_joblist_t *list);
void l2lv_block_joblist_free   (l2lv_block_joblist_t *list);
int  l2lv_block_joblist_add_req(l2lv_block_joblist_t *list, struct l2lv_channel *channel, struct l2lv_msg_header *req_message, vbd_info *info);

#endif
