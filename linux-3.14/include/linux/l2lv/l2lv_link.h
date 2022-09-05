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
#ifndef _L2LV_LINK_H_
#define _L2LV_LINK_H_

#include <linux/l2lv/l2lv_defs.h>
#include <linux/l2lv/l2lv_shm.h>
#include <linux/types.h>

struct l2lv_link;
struct l2lv_channel;
struct l2lv_joblist;
struct l2lv_port;

/* Receive callback function signature */
typedef void (*l2lv_receive_cb_t)(struct l2lv_channel *channel,
				      char *rcv_buffer, unsigned int size);

typedef void (*l2lv_handle_sync_msg_t)(struct l2lv_channel *channel,
					   struct l2lv_sync_msg *msg);

/*
 * A channel represents a connection for a specific virtualized device driver.
 *
 * The channel_id is globally assigned "per purpose" and a shared device
 * driver on the server partition communicates with the virtual device drivers
 * on all configured client partitions through channels with the same
 * channel_id. A channel is assigned to exactly one link. A shared memory
 * resource may be assigned to a channel.
 */
struct l2lv_channel {
/* public: */
	struct l2lv_shm shm;
	void *user_obj;
	unsigned channel_id;
/* private: */
	struct l2lv_link *link;
	l2lv_receive_cb_t on_receive;
	l2lv_receive_cb_t on_sync;
	wait_queue_head_t wait_q;
	unsigned int own_state;
};

/*
 * A link is a connection between different partitions.
 *
 * The link_id is globally unique and statically assigned. For each pair of
 * partitions, at most one link may exist. Every configured client partition
 * possesses exactly one link.
 */
struct l2lv_link {
/* private: */
	unsigned link_id;
	struct mutex lock;
	struct l2lv_joblist *joblist;
	struct l2lv_port *port;
	l2lv_handle_sync_msg_t sync_msg_cb;
	unsigned char rcv_buffer[L2LV_PORT_SIZE];
	void *snd_buffer;
	unsigned int channels_count;
	struct l2lv_channel channels[L2LV_MAX_CHANNEL];
};

/*
 * Initialize  or destroy the given link.
 *
 * After initialization the link uses joblist for buffering incoming messages.
 * It listens on ip_name for messages and sends messages through op_name.
 * Incoming sync messages are sent to the sync_msg_cb function.
 */
int l2lv_init_link(struct l2lv_link *link,
		       struct l2lv_joblist *joblist,
		       unsigned int link_id,
		       const char *ip_name,
		       const char *op_name,
		       l2lv_handle_sync_msg_t sync_msg_cb);
void l2lv_destroy_link(struct l2lv_link *link);

/*
 * Create a new channel with channel_id for the given link.
 *
 * The channel_id is the unique channel identifier as defined in
 * l2lv_defs.h. The L2LV framework calls the on_receive and on_sync
 * functions when it receives data or sync messages for the specific channel.
 *
 * The user_obj pointer is deprecated and should not be used in new drivers.
 */
int l2lv_add_channel(struct l2lv_link *link,
			 unsigned int channel_id,
			 const char *shm_name,
			 l2lv_receive_cb_t on_receive,
			 l2lv_receive_cb_t on_sync,
			 void *user_obj);
void l2lv_remove_channel(struct l2lv_link *link,
			     unsigned int channel_id);
struct l2lv_channel *l2lv_get_channel_by_id(struct l2lv_link *link,
						    unsigned int channel_id);

/*
 * Send messages via the l2lv framework.
 *
 * Call l2lv_get_send_buf to lock and get the send buffer. After writing
 * to the send buffer, call l2lv_send to send the message and unlock the
 * send buffer again.
 */
void *l2lv_get_send_buf(struct l2lv_link *link);
int l2lv_send(struct l2lv_link *link, unsigned int channel_id,
		  unsigned int size, int wait_for_ack);
int l2lv_send_sync(struct l2lv_link *link, unsigned int channel_id,
		       unsigned int event);

/*
 * Callback function for l2lv server and l2lv client to handle data messages.
 */
void l2lv_handle_data_message(struct l2lv_link *link, int size);

#endif				/* _L2LV_LINK_H_ */
