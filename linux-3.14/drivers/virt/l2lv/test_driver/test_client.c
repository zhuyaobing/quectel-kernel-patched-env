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
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/l2lv/l2lv_client_link.h>
#include <asm/uaccess.h>
#include "test_private_interface.h"
#include <linux/l2lv/test/test_client.h>
#include "../core/l2lv_debug.h"

#define TEST_SHM_NAME "shm:/shm_test"
#define TEST_DEV_NAME "test_client"

static int test_major;
static int test_minor;
static struct class *test_class;
static struct device *device;
static struct test_link *test_link;
static struct cdev cdev;
static char *shm_name = TEST_SHM_NAME;
module_param(shm_name, charp, 0);
MODULE_PARM_DESC(shm_name, "Shared memory region to be used by the driver.");

static void on_receive(struct l2lv_channel *channel, char *rcv_buffer,
		       uint32_t size)
{
	struct l2lv_msg_header *header =
	    (struct l2lv_msg_header *)rcv_buffer;

	if (size != header->channel_size) {
		DEBUG_MSG("[TEST_CLIENT]: invalid message content: channel_size = %d, size=%d\n",
			  header->channel_size, size);
		return;
	}

	switch (header->message_type) {
	case L2LV_MSGT_SYNC:
		DEBUG_MSG("[TEST_CLIENT]: sync message from link %d, channel %d, event %d",
			  channel->link->link_id,
			  header->channel_id,
			  ((struct l2lv_sync_msg *) rcv_buffer)->event);
		break;
	case TEST_SEND_MSG_ACK:
		DEBUG_MSG("[TEST_CLIENT]: received ack!\n");
		break;
	case TEST_SEND_DATA_SHM_ACK:
		DEBUG_MSG("[TEST_CLIENT]: Send data in shm succeeded!");
		break;
	default:
		DEBUG_MSG("[TEST_CLIENT]: received invalid message type\n");
	}
}


static long test_get_state(unsigned int *state)
{
	long ret = 0;
	struct l2lv_channel *channel =
		l2lv_get_channel_by_id(test_link->test_link,
					   L2LV_CHANNEL_TEST);
	if (!channel) {
		DEBUG_MSG("[TEST_SERVER]: Channel does not exist!\n");
		ret = -EFAULT;
		goto out;
	}

	switch (channel->own_state) {
	case L2LV_CLIENT_STATE_UNAVAIL:
		*state = STATE_UNAVAIL;
		break;
	case L2LV_CLIENT_STATE_OFFLINE:
		*state = STATE_OFFLINE;
		break;
	case L2LV_CLIENT_STATE_READY:
		*state = STATE_READY;
		break;
	case L2LV_CLIENT_STATE_REQ_DATA:
		*state = STATE_REQ_DATA;
		break;
	case L2LV_STATE_UNINITIALIZED:
		*state = STATE_UNINIT;
		break;
	default:
		DEBUG_MSG("[TEST_CLIENT]: Invalid server state!\n");
		ret = -EINVAL;
		goto out;
	}
out:
	return ret;
}


static void test_send_data(void)
{
	struct l2lv_channel *channel;
	struct l2lv_msg_header *header;
	channel = l2lv_get_channel_by_id(test_link->test_link,
					     L2LV_CHANNEL_TEST);

	header = (struct l2lv_msg_header *) l2lv_get_send_buf(
						test_link->test_link);

	header->message_type = TEST_SEND_MSG_REQ;

	l2lv_send(test_link->test_link, L2LV_CHANNEL_TEST,
		      sizeof(struct l2lv_msg_header), true);
}


static void test_send_data_shm(void)
{
	struct l2lv_channel *channel;
	struct l2lv_msg_header *header;
	char *buffer = "TEST";

	channel = l2lv_get_channel_by_id(test_link->test_link,
					     L2LV_CHANNEL_TEST);

	header = (struct l2lv_msg_header *) l2lv_get_send_buf(
						test_link->test_link);
	header->message_type = TEST_SEND_DATA_SHM_REQ;

	memcpy(channel->shm.kernel_vaddr, buffer, 4);

	l2lv_send(test_link->test_link, L2LV_CHANNEL_TEST,
		      sizeof(struct l2lv_msg_header), true);
}


static void test_req_to_offline(void)
{
	struct l2lv_channel *channel;
	struct l2lv_msg_header *header;
	channel = l2lv_get_channel_by_id(test_link->test_link,
					     L2LV_CHANNEL_TEST);

	header = (struct l2lv_msg_header *) l2lv_get_send_buf(
						test_link->test_link);

	header->message_type = TEST_SEND_DATA_TO_OFFLINE_REQ;

	l2lv_send(test_link->test_link, L2LV_CHANNEL_TEST,
		      sizeof(struct l2lv_msg_header), true);
}


static long test_ioctl(struct file *file,
		unsigned int cmd,
		unsigned long arg)
{
	long ret = 0;
	unsigned int state;
	struct l2lv_channel *channel;
	switch (cmd) {
	case TEST_IOC_GET_STATE:
		ret = test_get_state(&state);
		if (ret)
			goto out;

		if (copy_to_user((unsigned int *) arg, &state,
		   sizeof(unsigned int))) {
			ret = -EFAULT;
			goto out;
		}
		break;
	case TEST_IOC_OPEN:
		channel = l2lv_get_channel_by_id(test_link->test_link,
			  L2LV_CHANNEL_TEST);
		ret = l2lv_client_open_channel(channel);
		if (ret) {
			DEBUG_MSG("[TEST_CLIENT]: open channel failed\n");
			goto out;
		}
		break;
	case TEST_IOC_SEND_FAKE_DATA:
		test_send_data();
		break;
	case TEST_IOC_CLOSE:
		channel = l2lv_get_channel_by_id(test_link->test_link,
						     L2LV_CHANNEL_TEST);
		l2lv_client_close_channel(channel);
		break;
	case TEST_IOC_SEND_SHM:
		test_send_data_shm();
		break;
	case TEST_IOC_REQ_DATA_TO_OFFLINE:
		test_req_to_offline();
		break;
	default:
		DEBUG_MSG("[TEST_CLIENT]: Invalid ioctl command!\n");
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
	.open		= test_open,
	.release	= test_release,
	.unlocked_ioctl	= test_ioctl,
};


static void __exit test_client_exit(void)
{
	cdev_del(&cdev);
	unregister_chrdev(test_major, TEST_DEV_NAME);
}


static int __init test_client_init(void)
{
	int ret;
	int devno;
	test_minor = 0;
	test_major = 0;

	ret = register_chrdev(test_major, TEST_DEV_NAME, &test_fops);
	if (ret > 0) {
		test_major = ret;
		ret = 0;
	} else {
		DEBUG_MSG("[TEST_CLIENT]: could not register major number error %d\n",
			  ret);
		return -ENODEV;
	}

	test_class = class_create(THIS_MODULE, TEST_DEV_NAME);

	test_link = kcalloc(1, sizeof(*test_link), GFP_KERNEL);
	if (!test_link)
		goto err_alloc_link;

	test_link->test_link = l2lv_get_client_link();
	strcpy(test_link->shm_name, shm_name);

	ret = l2lv_add_channel(test_link->test_link, L2LV_CHANNEL_TEST,
				   test_link->shm_name, on_receive, on_receive,
				   test_link);
	if (ret) {
		DEBUG_MSG("[TEST_CLIENT]: unable to add channel\n");
		goto err_add_channel;
	}

	cdev_init(&cdev, &test_fops);
	cdev.owner = THIS_MODULE;
	devno = MKDEV(test_major, test_minor);
	ret = cdev_add(&cdev, devno, 1);
	if (ret) {
		DEBUG_MSG("[TEST_CLIENT]: unable to add device\n");
		goto err_add_device;
	}

	device = device_create(test_class, NULL,
			       devno, NULL,
			       TEST_DEV_NAME "%d", 0);
	if (IS_ERR(device)) {
		DEBUG_MSG("[TEST_CLIENT]: error %ld, unable to create %s%d",
			  PTR_ERR(device), TEST_DEV_NAME, 0);
		goto err_device_create;
	}

	return 0;
err_device_create:
	cdev_del(&cdev);
err_add_device:

err_add_channel:
	kfree(test_link);

err_alloc_link:
	test_client_exit();
	return ret;
}

module_init(test_client_init);
module_exit(test_client_exit);
MODULE_AUTHOR("Opensynergy GmbH");
MODULE_LICENSE("LGPL");
