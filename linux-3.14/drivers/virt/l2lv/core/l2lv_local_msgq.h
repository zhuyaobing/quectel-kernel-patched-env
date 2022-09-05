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
#ifndef _L2LV_LOCAL_MSGQ_H_
#define _L2LV_LOCAL_MSGQ_H_

#include <linux/l2lv/l2lv_defs.h>

/*
 * L2LV Local Message Queue
 *
 * Requirements: (TODO: put link to requirements here)
 *
 * Race-free execution environment. The caller must organize all locks.
 * */

#define L2LV_LMSGQ_MAX_MSGSIZE L2LV_PORT_SIZE
#define L2LV_LMSGQ_SIZE        L2LV_LOCAL_MSGQ_SIZE

struct l2lv_local_msg {
	size_t size;
	char data[L2LV_LMSGQ_MAX_MSGSIZE];
};

struct l2lv_local_msgq {
	unsigned int begin, end;
	struct l2lv_local_msg msg[L2LV_LMSGQ_SIZE];
	int push_last_called;
	wait_queue_head_t wait_q;
};

static inline int l2lv_local_msgq_isempty(struct l2lv_local_msgq *msgq)
{
	return msgq->begin == msgq->end;
}

static inline int l2lv_local_msgq_isfull(struct l2lv_local_msgq *msgq)
{
	return msgq->begin == (msgq->end + 1) % L2LV_LMSGQ_SIZE;
}

void l2lv_local_msgq_init(struct l2lv_local_msgq *msgq);

/* l2lv_local_msgq_get_buffer() and l2lv_local_msgq_push() must be
 * called alternatingly. */
void *l2lv_local_msgq_get_buffer(struct l2lv_local_msgq *msgq);
void l2lv_local_msgq_push(struct l2lv_local_msgq *msgq, unsigned int size);

/* Never call l2lv_local_msgq_pop() with an empty queue. */
void l2lv_local_msgq_pop(struct l2lv_local_msgq *msgq, char *data,
			     unsigned int *size);

#endif
