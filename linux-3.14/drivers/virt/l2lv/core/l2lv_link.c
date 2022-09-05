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
#include <linux/hrtimer.h>
#include <linux/kthread.h>
#include <linux/random.h>
#include <linux/slab.h>

#include <linux/l2lv/l2lv_hyp_support.h>
#include <linux/l2lv/l2lv_link.h>

#include "l2lv_port.h"
#include "l2lv_joblist.h"
#include "l2lv_debug.h"

static void l2lv_rx_int(void *data)
{
	struct l2lv_link *link = data;

	hyp_spin_lock(&link->joblist->hyp_spinlock);
	l2lv_joblist_push(link->joblist, link);
	hyp_spin_unlock(&link->joblist->hyp_spinlock);
}

static int l2lv_rx_thread(void *arg)
{
	struct l2lv_joblist *joblist = arg;
	struct l2lv_link *link;
	int size;

	while (!kthread_should_stop()) {
		link = l2lv_joblist_pop(joblist);
		if (!link) {
			pr_warn("l2lv_rx error: Failed to pop link from joblist\n");
			continue;
		}
		while (l2lv_port_rx_state(link->port) == L2LV_PORT_RCVQ_NOTEMPTY) {
			l2lv_port_receive(link->port, link->rcv_buffer, &size);
			if (size <= 0) {
				pr_warn("l2lv_rx error: l2lv_port_receive failed: %d\n", size);
				continue;
			}
			joblist->data_callback(link, size);
		}

		if (l2lv_port_interrupt_enable(link->port)) {
			/*
			 * if there has been data in the port before enabling
			 * interrupts, add a job to the list to handle it in
			 * the next iteration
			 */
			l2lv_joblist_push(link->joblist, link);
		}
	}
	return 0;
}

static inline void l2lv_lock_link(struct l2lv_link *link)
{
	mutex_lock(&link->lock);
}

static inline void l2lv_unlock_link(struct l2lv_link *link)
{
	mutex_unlock(&link->lock);
}


void l2lv_set_defined_link_state(struct l2lv_link *link,
				     struct l2lv_joblist *joblist,
				     uint32_t link_id,
				     l2lv_handle_sync_msg_t sync_msg_cb)
{
	mutex_init(&link->lock);
	link->channels_count = 0;
	link->port = NULL;
	link->joblist = joblist;
	link->link_id = link_id;
	link->sync_msg_cb = sync_msg_cb;
	link->snd_buffer = NULL;
}

int l2lv_init_link(struct l2lv_link *link,
		       struct l2lv_joblist *joblist,
		       uint32_t link_id,
		       const char *ip_name,
		       const char *op_name,
		       l2lv_handle_sync_msg_t sync_msg_cb)
{
	int ret = 0;
	struct l2lv_config_port port_config;
	l2lv_set_defined_link_state(link, joblist, link_id, sync_msg_cb);
	if (!joblist->thread)
		joblist->thread = kthread_run(l2lv_rx_thread, joblist,
				"l2lv_rx");

	port_config.id[L2LV_PORT_RECEIVE] = ip_name;
	port_config.id[L2LV_PORT_SEND] = op_name;
	port_config.rcv_isr = l2lv_rx_int;
	port_config.rcv_user = link;
	link->port = kzalloc(sizeof(struct l2lv_port), GFP_KERNEL);
	if (!link->port)
		goto error;
	ret = l2lv_port_open(link->port, &port_config);
	if (ret)
		goto error;

	if (l2lv_port_interrupt_enable(link->port)) {
		l2lv_joblist_push(link->joblist, link);
	}

	pr_info("Initialized link:\n"
		"  id:          %d\n"
		"  input port:  %s\n"
		"  output port:  %s\n",
		link_id, ip_name, op_name);

	return ret;

 error:
	if (link && link->port) {
		l2lv_port_close(link->port);
		kfree(link->port);
	}
	return ret;
}
EXPORT_SYMBOL(l2lv_init_link);

static void destroy_channel(struct l2lv_channel *channel)
{
	if (channel->shm.kernel_vaddr)
		l2lv_unmap_shm(&channel->shm);
}
EXPORT_SYMBOL(destroy_channel);

void l2lv_destroy_link(struct l2lv_link *link)
{
	unsigned int i;

	l2lv_lock_link(link);
	for (i = 0; i < link->channels_count; ++i)
		destroy_channel(&link->channels[i]);

	link->channels_count = 0;
	if (link->port) {
		l2lv_port_close(link->port);
		kfree(link->port);
	}
	l2lv_unlock_link(link);
}
EXPORT_SYMBOL(l2lv_destroy_link);

int l2lv_map_channel_shm(struct l2lv_channel *channel,
			     const char *shm_name)
{
	int ret = 0;
	if (shm_name && shm_name[0]) {
		ret = l2lv_map_shm(&channel->shm, shm_name);
		if (ret) {
			pr_warn("Failed to open shared memory %s\n",
				shm_name);
			ret = -EINVAL;
		}
	} else {
		channel->shm.kernel_vaddr = NULL;
		channel->shm.size = 0;
	}

	return ret;
}

inline void l2lv_print_channel_info(struct l2lv_link *link,
					struct l2lv_channel *channel,
					const char *shm_name)
{
	if (channel->shm.kernel_vaddr)
		pr_info("Added channel: link = %d, channel = %d\n"
			"               %s (%lx kB at kernel address 0x%p)\n",
			link->link_id, channel->channel_id, shm_name,
			channel->shm.size >> 10, channel->shm.kernel_vaddr);
	else
		pr_info("Added channel: link = %d, channel = %d\n"
			"               (no shared memory)\n",
			link->link_id, channel->channel_id);
}

int l2lv_add_channel(struct l2lv_link *link,
			 unsigned int channel_id,
			 const char *shm_name,
			 l2lv_receive_cb_t on_receive,
			 l2lv_receive_cb_t on_sync,
			 void *user_obj)
{
	int ret = 0;
	struct l2lv_channel *channel;

	l2lv_lock_link(link);

	channel = l2lv_get_channel_by_id(link, channel_id);
	if (channel) {
		pr_warn("Channel %d exists\n", channel_id);
		ret = -EEXIST;
		goto out;
	}
	if (link->channels_count == L2LV_MAX_CHANNEL) {
		pr_warn("Too many channels for link %d\n", link->link_id);
		ret = -ENOMEM;
		goto out;
	}
	channel = &link->channels[link->channels_count];

	ret = l2lv_map_channel_shm(channel, shm_name);
	if (ret)
		goto out;

	++link->channels_count;
	channel->channel_id = channel_id;
	channel->user_obj = user_obj;
	channel->link = link;
	channel->on_receive = on_receive;
	channel->on_sync = on_sync;
	channel->own_state = L2LV_STATE_UNINITIALIZED;
	TEST_MSG("L2LV_STATE_UNINITIALIZED");
	init_waitqueue_head(&channel->wait_q);

	l2lv_print_channel_info(link, channel, shm_name);

 out:
	l2lv_unlock_link(link);
	return ret;
}
EXPORT_SYMBOL(l2lv_add_channel);

void l2lv_remove_channel(struct l2lv_link *link,
			     uint32_t channel_id)
{
	struct l2lv_channel *channel = NULL;
	unsigned int i;

	l2lv_lock_link(link);

	for (i = 0; i < link->channels_count; ++i) {
		if (channel_id == link->channels[i].channel_id) {
			channel = &link->channels[i];
			destroy_channel(channel);
			break;
		}
	}
	if (channel) {
		for (++i; i < link->channels_count; ++i) {
			memcpy(&link->channels[i - 1], &link->channels[i],
			       sizeof(*channel));
		}
		--link->channels_count;
	}

	l2lv_unlock_link(link);
}
EXPORT_SYMBOL(l2lv_remove_channel);

struct l2lv_channel *l2lv_get_channel_by_id(struct l2lv_link *link,
						    uint32_t channel_id)
{
	struct l2lv_channel *channel = NULL;
	unsigned int i;

	for (i = 0; i < link->channels_count; ++i) {
		if (link->channels[i].channel_id == channel_id) {
			channel = &link->channels[i];
			break;
		}
		pr_debug("GET CHANNEL\n");
	}

	return channel;
}
EXPORT_SYMBOL(l2lv_get_channel_by_id);

void l2lv_handle_data_message(struct l2lv_link *link,
				  int size)
{
	struct l2lv_msg_header *msg_header;
	struct l2lv_channel *channel;

	msg_header = (struct l2lv_msg_header *)link->rcv_buffer;
	channel = l2lv_get_channel_by_id(link, msg_header->channel_id);

	if (size != msg_header->channel_size) {
		pr_warn("Channel %d of link %d received inconsistent data\n",
			msg_header->channel_id, link->link_id);
		return;
	}

	if (!channel) {
		pr_warn("Received message for not existing channel %d\n",
			 msg_header->channel_id);
		return;
	}

	if (msg_header->message_type == L2LV_MSGT_SYNC) {
		pr_debug("Received sync message on channel %d of link %d",
			 msg_header->channel_id, link->link_id);
		link->sync_msg_cb(channel,
				  (struct l2lv_sync_msg *)link->rcv_buffer);
		return;
	}

	pr_debug("Received data message: %d\n", msg_header->message_type);
	switch (channel->own_state) {
	case L2LV_CLIENT_STATE_REQ_DATA:
		if (channel->on_receive)
			channel->on_receive(channel, link->rcv_buffer, size);

		channel->own_state = L2LV_CLIENT_STATE_READY;
		wake_up_interruptible(&channel->wait_q);
		TEST_MSG("L2LV_CLIENT_STATE_REQ_DATA->L2LV_CLIENT_STATE_READY");
		break;
	case L2LV_SERVER_STATE_READY:
		if (channel->on_receive)
			channel->on_receive(channel, link->rcv_buffer, size);

		break;
	default:
		pr_debug("Channel %d in invalid state %d ignored message",
			 msg_header->channel_id, channel->own_state);
		wake_up_interruptible(&channel->wait_q);
		break;
	}
}
EXPORT_SYMBOL(l2lv_handle_data_message);

void *l2lv_get_send_buf(struct l2lv_link *link)
{
	l2lv_lock_link(link);
	if (!link->port) {
		panic("l2lv_get_send_buf: link has no port!\n");
		return 0;
	}
	link->snd_buffer = l2lv_port_get_sendbuf(link->port);
	return link->snd_buffer;
}
EXPORT_SYMBOL(l2lv_get_send_buf);

int l2lv_send_ready(struct l2lv_link *link,
			unsigned int channel_id,
			size_t size)
{
	struct l2lv_msg_header *header;
	header = (struct l2lv_msg_header *) link->snd_buffer;
	header->channel_id = channel_id;
	header->channel_size = size;
	l2lv_port_send(link->port, size);
	return size;
}

int l2lv_client_wait_for_ack(struct l2lv_channel *channel,
				 struct l2lv_link *link)
{
	int ret;

	TEST_MSG("L2LV_CLIENT_STATE_READY->L2LV_CLIENT_STATE_REQ_DATA");
	l2lv_unlock_link(link);
	ret = wait_event_interruptible(channel->wait_q,
			channel->own_state != L2LV_CLIENT_STATE_REQ_DATA);
	if (ret > 0)
		ret = 0;
	else if (ret == -ERESTARTSYS)
		return ret;

	if (channel->own_state != L2LV_CLIENT_STATE_READY) {
#ifdef CONFIG_L2LV_TEST
		switch (channel->own_state) {
		case L2LV_CLIENT_STATE_UNAVAIL:
			TEST_MSG("L2LV_CLIENT_STATE_UNAVAIL->L2LV_CLIENT_STATE_READY");
			break;
		case L2LV_CLIENT_STATE_OFFLINE:
			TEST_MSG("L2LV_CLIENT_STATE_OFFLINE->L2LV_CLIENT_STATE_READY");
			break;
		case L2LV_CLIENT_STATE_REQ_DATA:
			TEST_MSG("L2LV_CLIENT_STATE_REQ_DATA->L2LV_CLIENT_STATE_READY");
		case L2LV_STATE_UNINITIALIZED:
			TEST_MSG("L2LV_STATE_UNINITIALZED->L2LV_CLIENT_STATE_READY");
			break;
		default:
			TEST_MSG("INVALID STATE");
		}
#endif
		channel->own_state = L2LV_CLIENT_STATE_READY;

		pr_debug("channel=%d, state = %d\n", channel->channel_id,
			 channel->own_state);
		ret = -EAGAIN;
	}

	return ret;
}

int l2lv_check_send_queue_full(struct l2lv_link *link)
{
	if (!link->port) {
		l2lv_unlock_link(link);
		pr_warn("l2lv_check_send_queue_full: link has no port!\n");
		return -EINVAL;
	}
	if (l2lv_port_tx_state(link->port) == L2LV_PORT_SNDQ_FULL) {
		l2lv_unlock_link(link);
		pr_warn("Send queue full with max messages\n");
		return -EAGAIN;
	}
	return 0;
}

int l2lv_send(struct l2lv_link *link,
		  unsigned int channel_id,
		  size_t size,
		  int wait_for_ack)
{
	int ret = 0;
	struct l2lv_channel *channel;
	ret = l2lv_check_send_queue_full(link);
	if (ret)
		goto unlocked_out;

	channel = l2lv_get_channel_by_id(link, channel_id);
	if (!channel) {
		l2lv_unlock_link(link);
		pr_warn("Cannot find channel %d\n", channel_id);
		ret = -ENODEV;
		goto unlocked_out;
	}

	switch (channel->own_state) {
		/* Server states (ignore wait_for_ack): */
	case L2LV_SERVER_STATE_READY:
		ret = l2lv_send_ready(link, channel_id, size);
		if (IS_ERR_VALUE(ret))
			goto unlocked_out;
		break;

		/* Client states: */
	case L2LV_CLIENT_STATE_READY:
		if (wait_for_ack)
			channel->own_state = L2LV_CLIENT_STATE_REQ_DATA;

		ret = l2lv_send_ready(link, channel_id, size);
		if (IS_ERR_VALUE(ret))
			goto unlocked_out;	/* TODO: leave with wrong state here! */

		/* TODO: Always wait for ACK at client (simply remove the option) */
		if (wait_for_ack) {
			ret = l2lv_client_wait_for_ack(channel, link);
			goto unlocked_out;
		}
		break;

	case L2LV_CLIENT_STATE_OFFLINE:
		pr_debug("client sending in offline state\n");
		ret = -EAGAIN;
		break;

	default:
		pr_warn("invalid state %d\n", channel->own_state);
		ret = -EINVAL;
	}
	l2lv_unlock_link(link);

 unlocked_out:
	return ret;
}
EXPORT_SYMBOL(l2lv_send);

int l2lv_send_sync(struct l2lv_link *link,
		       uint32_t channel_id,
		       uint32_t event)
{
	int ret = 0;
	struct l2lv_sync_msg *sync_msg;
	struct l2lv_channel *channel;
	pr_debug("l2lv_send_sync: Sending sync message to link %d, channel %d, event %d.\n",
		 link->link_id, channel_id, event);

	sync_msg = (struct l2lv_sync_msg *)l2lv_get_send_buf(link);

	ret = l2lv_check_send_queue_full(link);
	if (ret) {
		pr_warn("l2lv_send_sync (link %d, channel %d, event %d): send queue full\n",
		 link->link_id, channel_id, event);
		goto unlocked_out;
	}

	channel = l2lv_get_channel_by_id(link, channel_id);
	sync_msg->header.channel_id = channel_id;
	sync_msg->header.channel_size = sizeof(*sync_msg);
	sync_msg->header.message_type = L2LV_MSGT_SYNC;
	sync_msg->event = event;

	l2lv_port_send(link->port, sizeof(*sync_msg));
	l2lv_joblist_push(link->joblist, link);
	l2lv_unlock_link(link);
	pr_debug("l2lv_send_sync: success\n");

 unlocked_out:
	return ret;
}
EXPORT_SYMBOL(l2lv_send_sync);
