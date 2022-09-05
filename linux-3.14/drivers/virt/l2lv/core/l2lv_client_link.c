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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kthread.h>
#include <linux/l2lv/l2lv_client_link.h>
#include "l2lv_debug.h"
#include "l2lv_joblist.h"

MODULE_LICENSE("LGPL");

static unsigned int l2lv_client_link_id = L2LV_LINK_ID_INVALID;
module_param(l2lv_client_link_id, uint, 0444);

static char *l2lv_client_oport_name = "REQ_Port";
module_param(l2lv_client_oport_name, charp, 0);

static char *l2lv_client_iport_name = "ACK_Port";
module_param(l2lv_client_iport_name, charp, 0);

static struct l2lv_link dev_link;
static int    g_l2lv_link_initialized = 0;

static struct l2lv_joblist joblist;

static void promote_sync_msg(struct l2lv_channel *channel,
			     struct l2lv_sync_msg *msg)
{
	if (channel->on_sync)
		channel->on_sync(channel, (char *)msg, sizeof(*msg));
}

static void l2lv_client_handle_sync_msg(struct l2lv_channel *channel,
					    struct l2lv_sync_msg *msg)
{
	struct l2lv_link *link = channel->link;

	switch (channel->own_state) {
	case L2LV_CLIENT_STATE_OFFLINE:
		switch (msg->event) {
		case L2LV_SYNC_OPEN_ACK:
			pr_debug("channel %d: STATE_OFFLINE: OPEN_ACK\n",
				 channel->channel_id);
			channel->own_state = L2LV_CLIENT_STATE_READY;
			promote_sync_msg(channel, msg);
			wake_up_interruptible(&channel->wait_q);
			TEST_MSG("L2LV_CLIENT_STATE_OFFLINE->L2LV_CLIENT_STATE_READY");
			break;
		default:
			pr_debug("channel %d: STATE_OFFLINE: ignoring event %d\n",
				 channel->channel_id, msg->event);
		}
		break;
	case L2LV_CLIENT_STATE_READY:
		switch (msg->event) {
		case L2LV_SYNC_SERVER_INIT:
			pr_debug("channel %d: CLIENT_STATE_READY: SERVER_INIT\n",
				 channel->channel_id);
			channel->own_state = L2LV_CLIENT_STATE_OFFLINE;
			promote_sync_msg(channel, msg);
			l2lv_send_sync(link, channel->channel_id,
					   L2LV_SYNC_OPEN_REQ);
			TEST_MSG("L2LV_CLIENT_STATE_READY->L2LV_CLIENT_STATE_OFFLINE");
			break;
		default:
			pr_debug("channel %d: CLIENT_STATE_READY: ignoring event %d\n",
				 channel->channel_id, msg->event);
			break;
		}
		break;
	case L2LV_CLIENT_STATE_REQ_DATA:
		switch (msg->event) {
		case L2LV_SYNC_SERVER_INIT:
			pr_debug("channel %d: STATE_REQ_DATA: SERVER_INIT\n",
				 channel->channel_id);
			channel->own_state = L2LV_CLIENT_STATE_OFFLINE;
			promote_sync_msg(channel, msg);
			l2lv_send_sync(link, channel->channel_id,
					   L2LV_SYNC_OPEN_REQ);
			wake_up_interruptible(&channel->wait_q);
			TEST_MSG("L2LV_CLIENT_STATE_REQ_DATA->L2LV_CLIENT_STATE_OFFLINE");
			break;
		default:
			pr_debug("channel %d: STATE_REQ_DATA: ignoring event %d\n",
				 channel->channel_id, msg->event);
			break;
		}
		break;
	default:
		pr_warn("channel %d: receive sync message in invalid state %d\n",
			 channel->channel_id, channel->own_state);
#ifdef CONFIG_L2LV_TEST
		switch (channel->own_state) {
		case L2LV_CLIENT_STATE_UNAVAIL:
			TEST_MSG("L2LV_CLIENT_STATE_UNAVAIL->L2LV_CLIENT_STATE_OFFLINE");
			break;
		case L2LV_STATE_UNINITIALIZED:
			TEST_MSG("L2LV_STATE_UNINITIALIZED->L2LV_CLIENT_STATE_OFFLINE");
			break;
		}
#endif
		channel->own_state = L2LV_CLIENT_STATE_OFFLINE;
		break;
	}
}

static int __init l2lv_init_client_link(void)
{
	int ret;

	l2lv_joblist_init(&joblist, l2lv_handle_data_message);

	if (l2lv_client_link_id == L2LV_LINK_ID_INVALID) {
		pr_warn("l2lv_init_client_link: invalid configuration (missing kernel parameter 'l2lv_client.l2lv_client_link_id'?)\n");
		return -EINVAL;
	}

	ret = l2lv_init_link(&dev_link, &joblist,
				 l2lv_client_link_id,
				 l2lv_client_iport_name,
				 l2lv_client_oport_name,
				 l2lv_client_handle_sync_msg);
	if (ret) {
		pr_warn("Failed to initialize link: %d\n", ret);
		return ret;
	}
	g_l2lv_link_initialized = 1;

	return 0;
}
late_initcall(l2lv_init_client_link);

struct l2lv_link *l2lv_get_client_link(void)
{
	if (!g_l2lv_link_initialized) {
		panic("l2lv_get_client_link: we are not initialized\n");
	}
	return &dev_link;
}
EXPORT_SYMBOL(l2lv_get_client_link);

int l2lv_client_open_channel(struct l2lv_channel *channel)
{
	int ret = 0;
	struct l2lv_link *link = channel->link;

#ifdef CONFIG_L2LV_TEST
	switch (channel->own_state) {
	case L2LV_CLIENT_STATE_UNAVAIL:
		TEST_MSG("L2LV_CLIENT_STATE_UNAVAIL->L2LV_CLIENT_STATE_OFFLINE");
		break;
	case L2LV_STATE_UNINITIALIZED:
		TEST_MSG("L2LV_STATE_UNINITIALIZED->L2LV_CLIENT_STATE_OFFLINE");
		break;
	default:
		TEST_MSG("L2LV_CLIENT_STATE_OFFLINE");
	}
#endif
	channel->own_state = L2LV_CLIENT_STATE_OFFLINE;
	wake_up_interruptible(&channel->wait_q);
	do {
		pr_debug("l2lv_client_open_channel send sync msg\n");
		l2lv_send_sync(link, channel->channel_id,
				   L2LV_SYNC_OPEN_REQ);
		ret = wait_event_interruptible_timeout(channel->wait_q,
			channel->own_state == L2LV_CLIENT_STATE_READY,
			msecs_to_jiffies(CLIENT_SYNC_OPEN_ACK_TIMEOUT_MS));

		if (ret == -ERESTARTSYS)
			goto out;

		ret = 0;
	} while (channel->own_state != L2LV_CLIENT_STATE_READY);
	pr_debug("l2lv_client_open_channel success\n");

 out:
	return ret;
}
EXPORT_SYMBOL(l2lv_client_open_channel);

void l2lv_client_close_channel(struct l2lv_channel *channel)
{
#ifdef CONFIG_L2LV_TEST
	switch (channel->own_state) {
	case L2LV_CLIENT_STATE_READY:
		TEST_MSG("L2LV_CLIENT_STATE_READY->L2LV_CLIENT_STATE_UNAVAIL");
		break;
	case L2LV_CLIENT_STATE_OFFLINE:
		TEST_MSG("L2LV_CLIENT_STATE_OFFLINE->L2LV_CLIENT_STATE_UNAVAIL");
		break;
	default:
		TEST_MSG("L2LV_CLIENT_STATE_UNAVAIL");
		break;
	}
#endif
	channel->own_state = L2LV_CLIENT_STATE_UNAVAIL;
	wake_up_interruptible(&channel->wait_q);
}
EXPORT_SYMBOL(l2lv_client_close_channel);
