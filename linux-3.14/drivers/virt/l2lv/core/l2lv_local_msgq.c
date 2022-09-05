/* Copyright (C) 2013-2016 OpenSynergy GmbH
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
#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include "l2lv_local_msgq.h"

#define L2LV_LMSGQ_GETBUF (0)
#define L2LV_LMSGQ_PUSH (1)

void l2lv_local_msgq_init(struct l2lv_local_msgq *msgq) {
	msgq->begin = msgq->end = 0;
	msgq->push_last_called = L2LV_LMSGQ_PUSH;
	init_waitqueue_head(&msgq->wait_q);
}

void *l2lv_local_msgq_get_buffer(struct l2lv_local_msgq *msgq) {
	if (msgq->push_last_called == L2LV_LMSGQ_GETBUF) {
		panic("l2lv_local_msgq_get_buffer: Execution order violation.\n");
	}
	if (l2lv_local_msgq_isfull(msgq)) {
		/* We wait here possibly holding a mutex of the environment locked.
		 * This is ok, since all other processes waiting on that mutex,
		 * wait for this resource. So no dead lock. */
		(void) wait_event_interruptible(msgq->wait_q, !l2lv_local_msgq_isfull(msgq));
		if (l2lv_local_msgq_isfull(msgq)) {
			/* -ERESTARTSYS and friends */
			return 0;
		}
	}
	msgq->push_last_called = L2LV_LMSGQ_GETBUF;
	return msgq->msg[msgq->end].data;
}

void l2lv_local_msgq_push(struct l2lv_local_msgq *msgq, unsigned int size) {
	if (msgq->push_last_called == L2LV_LMSGQ_PUSH) {
		panic("l2lv_local_msgq_push: Execution order violation.\n");
	}
	msgq->push_last_called = L2LV_LMSGQ_PUSH;
	msgq->msg[msgq->end].size = size;
	msgq->end = (msgq->end + 1) % L2LV_LMSGQ_SIZE;
}

void l2lv_local_msgq_pop(struct l2lv_local_msgq *msgq, char *data,
			     unsigned int *size) {
	struct l2lv_local_msg *msg = &msgq->msg[msgq->begin];
	if (l2lv_local_msgq_isempty(msgq)) {
		panic("l2lv_local_msgq_pop: Pop with empty queue.\n");
	}
	memcpy(data, msg->data, msg->size);
	*size = msg->size;
	msgq->begin = (msgq->begin + 1) % L2LV_LMSGQ_SIZE;
	wake_up_interruptible(&msgq->wait_q);
}

MODULE_LICENSE("LGPL");
