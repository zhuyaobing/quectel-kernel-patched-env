/*
 * Copyright (C) 2013-2014 OpenSynergy GmbH
 *
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
#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/vmalloc.h>
#include <linux/spinlock.h>
#include <linux/file.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/l2lv/l2lv_server_links.h>
#include <asm/uaccess.h>
#include <linux/l2lv/test/test_server.h>
#include "../core/l2lv_debug.h"
#include "test_private_interface.h"
#define TEST_DEV_NAME	"test_server"

static int test_major;
static int test_minor;
static struct class *test_class;
static struct l2lv_links *links;
static struct l2lv_link *link;
static struct platform_device *test_device;


static void on_receive(struct l2lv_channel *channel,
		       char *rcv_buffer,
		       uint32_t size)
{
	struct l2lv_msg_header *header =
		(struct l2lv_msg_header *) rcv_buffer;

	struct l2lv_msg_header *ack;

	if (size != header->channel_size) {
		DEBUG_MSG("[TEST_SERVER]: invalid message content: channel_size = %d, size=%d\n",
			  header->channel_size,
			  size);
		return;
	}

	switch (header->message_type) {
	case L2LV_MSGT_SYNC:
		DEBUG_MSG("[TEST_SERVER]: sync message from link %d, channel %d, event %d\n",
			  channel->link->link_id,
			  header->channel_id,
			  ((struct l2lv_sync_msg *) rcv_buffer)->event);
		break;
	case TEST_SEND_MSG_REQ:
		DEBUG_MSG("[TEST_SERVER]: received msg request\n");
		ack = (struct l2lv_msg_header *) l2lv_get_send_buf(
						     channel->link);
		ack->message_type = TEST_SEND_MSG_ACK;
		l2lv_send(channel->link, L2LV_CHANNEL_TEST,
			      sizeof(struct l2lv_msg_header), false);
		break;
	case TEST_SEND_DATA_SHM_REQ:
		DEBUG_MSG("[TEST_SERVER]: received shm message request");
		if (strncmp("TEST", channel->shm.kernel_vaddr, 4) == 0) {
			TEST_MSG("MESSAGE_CONTENT_VALID!");
			ack = (struct l2lv_msg_header *)
				l2lv_get_send_buf(channel->link);
			ack->message_type = TEST_SEND_DATA_SHM_ACK;
			l2lv_send(channel->link, L2LV_CHANNEL_TEST,
				      sizeof(struct l2lv_msg_header),
				      false);
		} else
			TEST_MSG("MESSAGE_CONTENT_INVALID!");
		break;
	case TEST_SEND_DATA_TO_OFFLINE_REQ:
		while (1)
			msleep(100);
		break;
	default:
		DEBUG_MSG("[TEST_SERVER]: invalid message type %d\n",
			header->message_type);
	}
}


static int test_set_link(struct test_set_link *set_link)
{
	int ret = 0;
	struct l2lv_channel *channel = NULL;
	link = l2lv_get_link_by_id(links, set_link->link_id);

	if (link == NULL) {
		ret = l2lv_add_link(links, set_link->link_id,
					set_link->ip_name, set_link->op_name);
		if (ret != 0) {
			DEBUG_MSG("[TEST_SERVER]: Cannot add more links!\n");
			goto err_add_link;
		}
		link = l2lv_get_link_by_id(links, set_link->link_id);
	}

	channel = l2lv_get_channel_by_id(link, L2LV_CHANNEL_TEST);
	if (!channel) {
		ret = l2lv_add_channel(link, L2LV_CHANNEL_TEST,
					   set_link->shm_name, on_receive,
					   on_receive, NULL);
		if (ret != 0) {
			DEBUG_MSG("[TEST_SERVER]: Cannot add channel!\n");
			goto err_add_channel;
		}

		channel = l2lv_get_channel_by_id(link,
						     L2LV_CHANNEL_TEST);
		channel->own_state = L2LV_SERVER_STATE_OFFLINE;
		TEST_MSG("L2LV_STATE_UNINITIALIZED->L2LV_SERVER_STATE_OFFLINE");
		l2lv_send_sync(link, L2LV_CHANNEL_TEST,
				   L2LV_SYNC_SERVER_INIT);
	}

	return 0;

err_add_channel:
	if (link != NULL && link->channels_count == 0)
		l2lv_remove_link(links, set_link->link_id);
err_add_link:
	return ret;
}


static long test_get_state(unsigned int *state)
{
	long ret = 0;
	struct l2lv_channel *channel =
		l2lv_get_channel_by_id(link, L2LV_CHANNEL_TEST);
	if (!channel) {
		DEBUG_MSG("[TEST_SERVER]: Channel does not exist!\n");
		ret = -EFAULT;
		goto out;
	}
	switch (channel->own_state) {
	case L2LV_SERVER_STATE_UNAVAIL:
		*state = STATE_UNAVAIL;
		break;
	case L2LV_SERVER_STATE_OFFLINE:
		*state = STATE_OFFLINE;
		break;
	case L2LV_SERVER_STATE_READY:
		*state = STATE_READY;
		break;
	default:
		DEBUG_MSG("[TEST_SERVER]: Invalid server state!\n");
		ret = -EINVAL;
	}

out:
	return ret;
}


static long test_ioctl(struct file *file,
	       unsigned int cmd,
	       unsigned long arg)
{
	unsigned int state = 0;
	long ret = 0;
	struct test_set_link set_link;
	switch (cmd) {
	case TEST_IOC_GET_STATE:
		ret = test_get_state(&state);
		if (ret)
			goto out;

		if (copy_to_user((unsigned int *)arg,
				 &state,
				 sizeof(unsigned int))) {
			ret = -EACCES;
			goto out;
		}
		break;
	case TEST_IOC_SET_LINK:
		if (copy_from_user(&set_link, (struct test_set_link *) arg,
				  sizeof(struct test_set_link))) {
			ret = -EFAULT;
			goto out;
		}

		if (test_set_link(&set_link)) {
			ret = -EINVAL;
			goto out;
		}

		break;
	default:
		DEBUG_MSG("[TEST_SERVER]: Invalid ioctl command!\n");
		ret = -EINVAL;
	}

out:
	return ret;
}


static int test_open(struct inode *inode,
	      struct file *file)
{
	return 0;
}


static int test_release(struct inode *inode,
		 struct file *file)
{
	return 0;
}


static const struct file_operations test_fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl	= test_ioctl,
	.open		= test_open,
	.release	= test_release,
};


static int test_probe(struct platform_device *pdev)
{
	struct device *dev;

	dev = device_create(test_class, 0, MKDEV(test_major, pdev->id), 0,
			    TEST_DEV_NAME);
	if (IS_ERR(dev)) {
		DEBUG_MSG("[TEST_SERVER]: Cannot create device %s errno = %ld\n",
			  TEST_DEV_NAME, PTR_ERR(dev));
	}
	return 0;
}


static int test_remove(struct platform_device *pdev)
{
	device_destroy(test_class, MKDEV(test_major, pdev->id));
	return 0;
}


static struct platform_driver test_driver = {
	.driver = {
		.name	= TEST_DEV_NAME,
		.owner	= THIS_MODULE,
	},
	.probe	= test_probe,
	.remove	= test_remove,
};


static void __exit test_server_exit(void)
{
	platform_device_unregister(test_device);
	platform_driver_unregister(&test_driver);
	class_destroy(test_class);
	unregister_chrdev(test_major, TEST_DEV_NAME);
}


static int __init test_server_init(void)
{
	int ret;
	test_major = 0;
	test_minor = 0;

	DEBUG_MSG("[TEST_SERVER]: INIT\n");

	ret = register_chrdev(test_major, TEST_DEV_NAME, &test_fops);
	if (ret > 0) {
		test_major = ret;
		ret = 0;
	} else {
		DEBUG_MSG("[TEST_SERVER]: could not register major number error %d\n",
			  ret);
		return -ENODEV;
	}

	test_class = class_create(THIS_MODULE, TEST_DEV_NAME);
	if (!test_class) {
		DEBUG_MSG("[TEST_SERVER]: Could not create class!\n");
		ret = -ENOMEM;
		goto err_class_create;
	}

	ret = platform_driver_register(&test_driver);
	if (ret != 0) {
		DEBUG_MSG("[TEST_SERVER]: Could not register platform driver\n");
		ret = -ENOMEM;
		goto err_dev;
	}

	test_device = platform_device_alloc(TEST_DEV_NAME, 0);
	if (!test_device) {
		DEBUG_MSG("[TEST_SERVER]: Could not allocate platform device\n");
		ret = -ENOMEM;
		goto err_dev_alloc;
	}

	ret = platform_device_add(test_device);
	if (ret != 0) {
		DEBUG_MSG("[TEST_SERVER]: Could not add platform device\n");
		ret = -ENOMEM;
		goto err_dev_add;
	}

	links = l2lv_server_get_links();
	return 0;

err_dev_add:
	platform_device_unregister(test_device);
err_dev_alloc:
	platform_driver_unregister(&test_driver);
err_dev:
	class_destroy(test_class);
err_class_create:
	unregister_chrdev(test_major, TEST_DEV_NAME);
	return ret;
}


module_init(test_server_init);
module_exit(test_server_exit);
MODULE_AUTHOR("Opensynergy GmbH");
MODULE_LICENSE("LGPL");
