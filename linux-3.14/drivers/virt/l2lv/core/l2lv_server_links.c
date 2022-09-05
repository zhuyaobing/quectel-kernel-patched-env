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
#include <linux/l2lv/l2lv_server_links.h>
#include <linux/semaphore.h>
#include "l2lv_debug.h"
#include "l2lv_joblist.h"
#include "l2lv_port.h"
#include "l2lv_local_port.h"

MODULE_LICENSE("LGPL");

static DEFINE_SEMAPHORE(g_links_sem);

static struct l2lv_joblist g_joblist;
static struct l2lv_links   g_shdrv_links;
static int                 g_l2lv_links_initialized = 0;

static void l2lv_client_handle_sync_msg(struct l2lv_channel *channel,
					    struct l2lv_sync_msg *msg);

static void promote_sync_msg(struct l2lv_channel *channel,
			     struct l2lv_sync_msg *msg)
{
	if (channel->on_sync)
		channel->on_sync(channel, (char *)msg, sizeof(*msg));
}

static void l2lv_handle_server_sync_msg(struct l2lv_channel *channel,
					    struct l2lv_sync_msg *msg)
{
	struct l2lv_link *link = channel->link;

	switch (channel->own_state) {
	case L2LV_SERVER_STATE_OFFLINE:
		switch (msg->event) {
		case L2LV_SYNC_OPEN_REQ:
			pr_debug("channel %d: SERVER_STATE_OFFLINE: OPEN_REQ\n",
				 channel->channel_id);
			promote_sync_msg(channel, msg);
			channel->own_state = L2LV_SERVER_STATE_READY;
			l2lv_send_sync(link, channel->channel_id,
					   L2LV_SYNC_OPEN_ACK);
			TEST_MSG("L2LV_SERVER_STATE_OFFLINE->L2LV_SERVER_STATE_READY");
			break;
		default:
			pr_info("channel %d: SERVER_STATE_OFFLINE: ignoring event %d\n",
				channel->channel_id, msg->event);
			break;
		}
		break;
	case L2LV_SERVER_STATE_READY:
		switch (msg->event) {
		case L2LV_SYNC_OPEN_REQ:
			pr_debug("channel %d: SERVER_STATE_READY: OPEN_REQ\n",
				 channel->channel_id);
			promote_sync_msg(channel, msg);
			l2lv_send_sync(link, channel->channel_id,
					   L2LV_SYNC_OPEN_ACK);
			break;
		default:
			pr_debug("channel %d: SERVER_STATE_READY: ignoring event %d\n",
				 channel->channel_id, msg->event);
			break;
		}
		break;
	default:
		pr_warn("channel %d: invalid state %d\n",
			 channel->channel_id, channel->own_state);
	}
}

static struct l2lv_link *l2lv_get_link_by_id_intern(struct l2lv_links *links,
					      unsigned int link_id)
{
	struct l2lv_link *link = 0;
	unsigned int i;
	for (i = 0; i < links->links_count; ++i) {
		if (links->links[i].link_id == link_id) {
			link = &links->links[i];
			break;
		}
	}
	return link;
}

/*
 * Add new link to links
 *
 * Return values:
 *  0: ok
 *  -EEXIST: link_id already exists
 *  -ENOMEM: out of memory (more than L2LV_MAX_CLIENTS links required)
 *  -EINVAL: invalid argument
 */
int l2lv_add_link(struct l2lv_links *links, unsigned int link_id,
		      const char *ip_name, const char *op_name)
{
	struct l2lv_link *link;
	int ret = 0;

	down(&g_links_sem);

	link = l2lv_get_link_by_id_intern(links, link_id);
	if (link) {
		if (link_id == L2LV_LINK_ID_LOCAL) {
			up(&g_links_sem);
			return 0;
		}
		ret = -EEXIST;
		goto error;
	}
	if (links->links_count == L2LV_MAX_CLIENTS) {
		pr_warn("error: too many links\n");
		ret = -ENOMEM;
		goto error;
	}
	link = &links->links[links->links_count];

	ret = l2lv_init_link(link, &g_joblist, link_id, ip_name, op_name,
				 l2lv_handle_server_sync_msg);
	if (ret)
		goto error;

	l2lv_lock_joblist(link->joblist);
	l2lv_joblist_push(link->joblist, link);
	l2lv_unlock_joblist(link->joblist);

	++links->links_count;

 error:
	up(&g_links_sem);

	return ret;
}
EXPORT_SYMBOL(l2lv_add_link);

void l2lv_remove_link(struct l2lv_links *links, unsigned int link_id)
{
	struct l2lv_link *link = NULL;
	unsigned int i;

	down(&g_links_sem);

	for (i = 0; i < links->links_count; ++i) {
		if (links->links[i].link_id == link_id) {
			link = &links->links[i];
			break;
		}
	}
	if (link) {
		l2lv_destroy_link(link);
		for (++i; i < links->links_count; ++i) {
			memcpy(&links->links[i - 1], &links->links[i],
			       sizeof(links->links[i - 1]));
		}
		--links->links_count;
	}

	up(&g_links_sem);
}
EXPORT_SYMBOL(l2lv_remove_link);

struct l2lv_link *l2lv_get_link_by_id(struct l2lv_links *links,
					      unsigned int link_id)
{
	struct l2lv_link *link = 0;
	down(&g_links_sem);
	link = l2lv_get_link_by_id_intern(links, link_id);
	up(&g_links_sem);
	return link;
}
EXPORT_SYMBOL(l2lv_get_link_by_id);

/* Local client: */
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
		pr_debug("channel %d: receive sync message in invalid state %d\n",
			 channel->channel_id, channel->own_state);
		channel->own_state = L2LV_CLIENT_STATE_OFFLINE;
		TEST_MSG("L2LV_CLIENT_STATE_OFFLINE");
		break;
	}
}

static struct l2lv_link g_localclient_link;
static struct l2lv_joblist g_localclient_joblist;

static int __init l2lv_init_localclient_link(void)
{
	int ret;

	l2lv_joblist_init(&g_localclient_joblist,
			      l2lv_handle_data_message);
	ret = l2lv_init_link(&g_localclient_link, &g_localclient_joblist,
				 L2LV_LINK_ID_LOCAL, 0, 0,
				 l2lv_client_handle_sync_msg);

	return ret;
}

struct l2lv_link *l2lv_get_client_link(void)
{
	if (!g_l2lv_links_initialized) {
		panic("l2lv_get_client_link: we are not initialized\n");
	}
	return &g_localclient_link;
}
EXPORT_SYMBOL(l2lv_get_client_link);

static void __exit l2lv_destroy_links(void)
{
	unsigned int i;
	struct l2lv_link *link;
	struct l2lv_links *links = l2lv_server_get_links();

	g_l2lv_links_initialized = 0;
	down(&g_links_sem);
	for (i = 0; i < links->links_count; ++i) {
		link = &links->links[i];
		l2lv_destroy_link(link);
	}

	links->links_count = 0;
	up(&g_links_sem);
}

static int __init l2lv_init_links(void)
{
	struct l2lv_link *link, *cl_link = &g_localclient_link;
	l2lv_init_localclient_link();
	l2lv_joblist_init(&g_joblist, l2lv_handle_data_message);

	link = &g_shdrv_links.links[0];
	if (l2lv_init_link(link, &g_joblist, L2LV_LINK_ID_LOCAL, 0, 0,
				 l2lv_handle_server_sync_msg)) {
		panic("l2lv_init_links: out of memory.\n");
	}
	g_shdrv_links.links_count = 1;

	l2lv_local_port_set_remote(cl_link->port->p.local_port,
							   link->port->p.local_port);
	l2lv_local_port_set_remote(link->port->p.local_port,
							   cl_link->port->p.local_port);

	g_l2lv_links_initialized = 1;
	l2lv_lock_joblist(link->joblist);
	l2lv_joblist_push(link->joblist, link);
	l2lv_unlock_joblist(link->joblist);
	return 0;
}
module_init(l2lv_init_links);
module_exit(l2lv_destroy_links);

struct l2lv_links *l2lv_server_get_links(void)
{
	if (!g_l2lv_links_initialized) {
		panic("l2lv_server_get_links: we are not initialized\n");
	}
	return &g_shdrv_links;
}
EXPORT_SYMBOL(l2lv_server_get_links);

int l2lv_client_open_channel(struct l2lv_channel *channel)
{
	int ret = 0;
	struct l2lv_link *link = channel->link;

	channel->own_state = L2LV_CLIENT_STATE_OFFLINE;
	TEST_MSG("L2LV_CLIENT_STATE_OFFLINE");
	wake_up_interruptible(&channel->wait_q);

	ret = l2lv_send_sync(link, channel->channel_id,
				 L2LV_SYNC_OPEN_REQ);
	if (ret)
		goto out;

	ret = wait_event_interruptible_timeout(channel->wait_q,
					       channel->own_state ==
					       L2LV_CLIENT_STATE_READY,
					       msecs_to_jiffies(300));

	if (channel->own_state != L2LV_CLIENT_STATE_READY) {
		if (ret == 0) {
			pr_debug("no local server configured\n");
			ret = -ENODEV;
		} else if (ret == -ERESTARTSYS) {
			pr_debug("sync INTERRUPTED\n");
		} else {
			pr_warn("internal error\n");
		}
		goto out;
	}

	ret = 0;

 out:
	return ret;
}
EXPORT_SYMBOL(l2lv_client_open_channel);

void l2lv_client_close_channel(struct l2lv_channel *channel)
{
	channel->own_state = L2LV_CLIENT_STATE_UNAVAIL;
	TEST_MSG("L2LV_CLIENT_STATE_UNAVAIL");
	wake_up_interruptible(&channel->wait_q);
}
EXPORT_SYMBOL(l2lv_client_close_channel);

void l2lv_set_local_buffer(struct l2lv_channel *channel)
{
	struct l2lv_link *link = channel->link;
	struct l2lv_link *local_link = l2lv_get_client_link();
	struct l2lv_channel *local_channel;

	if (link->link_id != L2LV_LINK_ID_LOCAL) {
		pr_warn("link %d is not local\n", link->link_id);
		return;
	}
	if (link == local_link) {
		local_link = l2lv_get_link_by_id(l2lv_server_get_links(), L2LV_LINK_ID_LOCAL);
		return;
	}

	local_channel = l2lv_get_channel_by_id(local_link,
						   channel->channel_id);
	if (!local_channel) {
		pr_warn("link %d has no channel %d\n", local_link->link_id,
			channel->channel_id);
		return;
	}
	pr_debug("Setting local buffer of link %d, channel %d: addr=0x%p, size=0x%lx\n",
		 local_link->link_id, local_channel->channel_id,
		 channel->shm.kernel_vaddr, channel->shm.size);

	local_channel->shm.kernel_vaddr = channel->shm.kernel_vaddr;
	local_channel->shm.size = channel->shm.size;
}
EXPORT_SYMBOL(l2lv_set_local_buffer);

void l2lv_unset_local_buffer(struct l2lv_channel *channel)
{
	struct l2lv_link *link = channel->link;
	struct l2lv_link *local_link = l2lv_get_client_link();
	struct l2lv_channel *local_channel;

	if (link->link_id != L2LV_LINK_ID_LOCAL) {
		pr_warn("link %d is not local\n", link->link_id);
		return;
	}
	if (link == local_link) {
		pr_warn("Trying to set local buffer on client link\n");
		return;
	}

	local_channel = l2lv_get_channel_by_id(local_link,
						   channel->channel_id);
	if (!local_channel) {
		pr_debug("link %d has no channel %d\n", local_link->link_id,
			 channel->channel_id);
		return;
	}

	pr_debug("Unsetting local buffer of link %d, channel %d: addr=0x%p, size=0x%lx\n",
		 local_link->link_id, local_channel->channel_id,
		 channel->shm.kernel_vaddr, channel->shm.size);

	local_channel->shm.kernel_vaddr = NULL;
	local_channel->shm.size = 0;
}
EXPORT_SYMBOL(l2lv_unset_local_buffer);
