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
/*
 * Virtual block device
 *
 * (based on nbd.c)
 */

#include <linux/bio.h>
#include <linux/blkdev.h>
#include <linux/compiler.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/hdreg.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/major.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/vmalloc.h>
#include <linux/completion.h>

#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/types.h>

#include <linux/l2lv/l2lv_client_link.h>
#include <linux/l2lv/l2lv_defs.h>
#include "vbd_client.h"
#include "vbd_debug.h"
#include "vbd_defs.h"
#include "vbd_private_if.h"

static int vbd_major = 0;
static unsigned int vbds_max = 2;
static char *shm_name = VBD_SHM_NAME;

static struct vbd_link *vbd_link;
static struct vbd_device *vbd_dev;
static const struct block_device_operations vbd_fops;

static int vbd_transfer(struct vbd_device *dev, struct request *req) {
	struct l2lv_channel *channel = l2lv_get_channel_by_id(vbd_link->dev_link, L2LV_CHANNEL_VBD);
	int ret = 0;
	unsigned int flags = 0;

	if (channel->own_state == L2LV_CLIENT_STATE_READY) {
		vbd_tx_req_msg_t *req_message;
		struct req_iterator iter;
		struct bio_vec bvec;
		unsigned int offset = 0;

		flags |= (rq_data_dir(req) ? VBD_WRITE : VBD_READ);

		if ((req->cmd_flags & REQ_FLUSH) || (req->cmd_flags & REQ_FUA)) {
			flags |= VBD_FLUSH_CACHE;
		}

		rq_for_each_segment(bvec, req, iter) {
			if (dev->bufsize < blk_rq_bytes(req) + offset){
				LOG("ERROR: Shared memory buffersize is too small: 0x%X - needed 0x%X\n", dev->bufsize, blk_rq_bytes(req) + offset);
				return -EINVAL;
			}
			offset += bvec.bv_len;
		}
		offset = 0;

		if (flags & VBD_WRITE) {

			rq_for_each_segment(bvec, req, iter) {
				void *kaddr = kmap(bvec.bv_page);
				memcpy(channel->shm.kernel_vaddr + dev->bufoffs + offset, kaddr + bvec.bv_offset, bvec.bv_len);
				kunmap(bvec.bv_page);
				offset += bvec.bv_len;
			}
		}

		req_message = (vbd_tx_req_msg_t *)l2lv_get_send_buf(vbd_link->dev_link);
		req_message->header.header.message_type = VBD_TRANSFER_REQ;
		req_message->header.vbd_id = dev->vbd_id;
		req_message->size 	= blk_rq_bytes(req);
		req_message->offset	= blk_rq_pos(req) << 9; /* blk_size = 512 */
		req_message->buf_offs = dev->bufoffs;
		req_message->flags	= flags;
		DBG("Sending VBD_TRANSFER_REQ for %s - %#.8x\n", req_message->flags & VBD_WRITE ? "VBD_WRITE" : "VBD_READ", (int)req);
		DBG("\t==> From %llu, size %lu, buf_offs=0x%lX\n", req_message->offset, req_message->size, req_message->buf_offs);

		ret = l2lv_send(vbd_link->dev_link, L2LV_CHANNEL_VBD, sizeof(vbd_tx_req_msg_t), true);
		offset = 0;
		if (!(flags & VBD_WRITE)) {
			rq_for_each_segment(bvec, req, iter) {
				void *kaddr = kmap(bvec.bv_page);
				memcpy(kaddr + bvec.bv_offset, channel->shm.kernel_vaddr + dev->bufoffs + offset, bvec.bv_len);
				kunmap(bvec.bv_page);
				offset += bvec.bv_len;
			}
		}
	} else {
		LOG("Channel state %d\n", channel->own_state);
		return -EAGAIN;
	}

	return ret;
}

static void vbd_end_request(struct request *req) {
	int error = req->errors ? -EIO : 0;
	struct request_queue *q = req->q;
	unsigned long flags;

	DBG("%s: request %p: %s\n", req->rq_disk->disk_name,
			req, error ? "failed" : "done");

	spin_lock_irqsave(q->queue_lock, flags);
	__blk_end_request_all(req, error);
	spin_unlock_irqrestore(q->queue_lock, flags);
}

static int vbd_req_thread(void *arg) {

	while(!kthread_should_stop()) {
		struct request *req		= NULL;
		struct vbd_device *dev	= NULL;
		int ret = 0;

		/* wait for something to do */
		wait_event_interruptible(vbd_link->wait_q,
					 kthread_should_stop() ||
					 !list_empty(&vbd_link->req_list));

		/* extract request */
		if (list_empty(&vbd_link->req_list))
			continue;

		spin_lock_irq(&vbd_link->waitq_lock);
		req = list_first_entry(&vbd_link->req_list, struct request, queuelist);
		list_del_init(&req->queuelist);
		spin_unlock_irq(&vbd_link->waitq_lock);

		/* handle request */
		dev = req->rq_disk->private_data;

		mutex_lock(&vbd_link->send_lock);
		ret = vbd_transfer(dev, req);
		mutex_unlock(&vbd_link->send_lock);

		req->errors += ret;

		vbd_end_request(req);
	}

	return 0;
}

/*
 * vbd_request is called with queue_lock held!
 */
static void vbd_request(struct request_queue *q)
{
	struct request *req;

	while ((req = blk_fetch_request(q)) != NULL) {
		spin_unlock_irq(q->queue_lock);

		if (req->cmd_type != REQ_TYPE_FS) {
			//printk (KERN_NOTICE "Skip non-CMD request\n");
			req->errors++;
			vbd_end_request(req);
			spin_lock_irq(q->queue_lock);
			continue;
		}

		// add request to req_list
		spin_lock_irq(&vbd_link->waitq_lock);
		list_add_tail(&req->queuelist, &vbd_link->req_list);
		spin_unlock_irq(&vbd_link->waitq_lock);

		wake_up(&vbd_link->wait_q);

		spin_lock_irq(q->queue_lock);
	}
}

static int vbd_open(struct block_device *bdev, fmode_t mode)
{
	// open channel
	// check mode!
	// }
	struct vbd_device	*dev		= bdev->bd_disk->private_data;
	struct l2lv_channel	*channel	= l2lv_get_channel_by_id(vbd_link->dev_link, L2LV_CHANNEL_VBD);
	int ret = 0;

	mutex_lock(&vbd_link->send_lock);
	DBG("Opening block device\n");
	if (channel->own_state == L2LV_CLIENT_STATE_READY) {
		vbd_capa_req_msg_t *req_message = (vbd_capa_req_msg_t *)l2lv_get_send_buf(vbd_link->dev_link);
		req_message->header.header.message_type = VBD_CAPACITY_REQ;
		req_message->header.vbd_id = dev->vbd_id;

		DBG("Sending VBD_CAPACITY_REQ\n");
		l2lv_send(vbd_link->dev_link, L2LV_CHANNEL_VBD, sizeof(vbd_capa_req_msg_t), true);

		if (!dev->opened)
			ret = -EINVAL;
	} else {
		ret = -EINVAL;
	}
	mutex_unlock(&vbd_link->send_lock);
	return ret;
}

static void vbd_send_sync(struct vbd_device *dev) {
	struct l2lv_channel	*channel	= l2lv_get_channel_by_id(vbd_link->dev_link, L2LV_CHANNEL_VBD);

	mutex_lock(&vbd_link->send_lock);
	if (channel->own_state == L2LV_CLIENT_STATE_READY) {
		vbd_capa_req_msg_t *req_message = (vbd_capa_req_msg_t *)l2lv_get_send_buf(vbd_link->dev_link);
		req_message->header.header.message_type = VBD_SYNC_REQ;
		req_message->header.vbd_id = dev->vbd_id;

		DBG("Sending VBD_SYNC_REQ\n");
		l2lv_send(vbd_link->dev_link, L2LV_CHANNEL_VBD, sizeof(vbd_capa_req_msg_t), true);
	}
	mutex_unlock(&vbd_link->send_lock);
}

extern int blkdev_issue_discard(struct block_device *bdev, sector_t sector,
	sector_t nr_sects, gfp_t gfp_mask, unsigned long flags);

int vbd_issue_discard(struct block_device *bdev, sector_t sector,
	sector_t nr_sects, gfp_t gfp_mask, unsigned long flags) {
	struct vbd_device *dev;
	struct l2lv_channel *channel;
	struct completion rpc_done;
	int ret;

	if ( bdev->bd_disk->fops != &vbd_fops ) {

		LOG("Discard non-VBD block device\n");
		return blkdev_issue_discard(bdev, sector, nr_sects, gfp_mask, flags);
	}

	dev = bdev->bd_disk->private_data;
	channel = l2lv_get_channel_by_id(vbd_link->dev_link, L2LV_CHANNEL_VBD);

	mutex_lock(&vbd_link->send_lock);
	if (channel->own_state == L2LV_CLIENT_STATE_READY) {
		vbd_discard_req_msg_t *req_message = (vbd_discard_req_msg_t *)l2lv_get_send_buf(vbd_link->dev_link);
		req_message->header.header.message_type = VBD_DISCARD_REQ;
		req_message->header.vbd_id		= dev->vbd_id;
		req_message->sector = sector;
		req_message->nr_sects = nr_sects;
		req_message->gfp_mask = gfp_mask;
		req_message->flags = flags;
		req_message->rpc_done_ptr = &rpc_done;
		req_message->ret_ptr = &ret;
		init_completion(&rpc_done);

		DBG("Sending VBD_DISCARD\n");
		l2lv_send(vbd_link->dev_link, L2LV_CHANNEL_VBD, sizeof(vbd_discard_req_msg_t), true);
	}
	mutex_unlock(&vbd_link->send_lock);

	wait_for_completion(&rpc_done);

	if ( ret ) {
		LOG("Got discard result (%d)\n", ret);
	}

	return ret;
}
EXPORT_SYMBOL(vbd_issue_discard);

static void vbd_send_ioctl(struct vbd_device *dev, fmode_t mode, unsigned int cmd) {
	struct l2lv_channel	*channel	= l2lv_get_channel_by_id(vbd_link->dev_link, L2LV_CHANNEL_VBD);

	mutex_lock(&vbd_link->send_lock);
	if (channel->own_state == L2LV_CLIENT_STATE_READY) {
		vbd_ioctl_msg_t *req_message = (vbd_ioctl_msg_t *)l2lv_get_send_buf(vbd_link->dev_link);
		req_message->header.header.message_type	= VBD_IOCTL_REQ;
		req_message->header.vbd_id	= dev->vbd_id;
		req_message->mode			= mode;
		req_message->cmd			= cmd;

		DBG("Sending VBD_IOCTL\n");
		l2lv_send(vbd_link->dev_link, L2LV_CHANNEL_VBD, sizeof(vbd_ioctl_msg_t), true);
	}
	mutex_unlock(&vbd_link->send_lock);
}

static void vbd_release(struct gendisk *gd, fmode_t mode)
{
	struct vbd_device *dev = gd->private_data;

	DBG("Releasing block device\n");
	vbd_send_sync(dev);

	dev->opened = false;

	return;
}

static void on_receive(struct l2lv_channel *channel, char *rcv_buffer, uint32_t size) {
	struct l2lv_msg_header *header      = (struct l2lv_msg_header*)rcv_buffer;

	if (size != header->channel_size) {
		LOG("invalid message content: channel_size = %d, size = %d\n", header->channel_size, size);
		return;
	}

	switch (header->message_type) {
	case L2LV_MSGT_SYNC:
		DBG("sync message from link %d, channel %d, event %d\n", channel->link->link_id, header->channel_id, ((l2lv_sync_msg_t*)rcv_buffer)->event);
		break;
	case VBD_TRANSFER_ACK:
	{
		vbd_tx_ack_msg_t *ack_message = (vbd_tx_ack_msg_t *)rcv_buffer;
		if (ack_message->retval < 0) {
			LOG("wrong return value %d\n", ack_message->retval);
		}
		break;
	}
	case VBD_CAPACITY_ACK:
	{
		vbd_capa_ack_msg_t *ack_message = (vbd_capa_ack_msg_t *)rcv_buffer;
		if (ack_message->capacity <= 0) {
			vbd_dev[ack_message->header.vbd_id].opened = false;
			break;
		} else {
			DBG("Setting disk capacity to %llu\n", ack_message->capacity);
			vbd_dev[ack_message->header.vbd_id].opened = true;
			set_capacity(vbd_dev[ack_message->header.vbd_id].disk, ack_message->capacity);
		}
		break;
	}
	case VBD_IOCTL_ACK:
	{
		vbd_ioctl_ack_msg_t *ack_message = (vbd_ioctl_ack_msg_t *)rcv_buffer;
		if(IS_ERR_VALUE(ack_message->retval))
			LOG("IOCTL forwarding has failed!!!\n");
		break;
	}
	case VBD_SYNC_ACK:
	{
		vbd_sync_ack_msg_t *ack_message = (vbd_sync_ack_msg_t *)rcv_buffer;
		if(ack_message->retval < 0)
			LOG("error occured in opening device\n");
		break;
	}
	case VBD_DISCARD_ACK:
	{
		vbd_discard_ack_msg_t *ack_message = (vbd_discard_ack_msg_t *)rcv_buffer;
		DBG("Returning discard result (%d)\n", ack_message->retval);
		*(ack_message->ret_ptr) = ack_message->retval;
		complete(ack_message->rpc_done_ptr);
		break;
	}
	default:
		LOG("received invalid msgtype %d\n", header->message_type);
	}
}

static int vbd_ioctl(struct block_device *bdev, fmode_t mode, unsigned int cmd, unsigned long arg) {
	switch(cmd) {
	case BLKFLSBUF:
	{
		struct vbd_device *dev = bdev->bd_disk->private_data;
		vbd_send_ioctl(dev, mode, BLKFLSBUF);
		return -ENOTTY;
	}
	default:
		LOG("received unknown ioctl %#.8x\n", cmd);
		break;
	}
	return -ENOTTY;
}

static const struct block_device_operations vbd_fops =
{
	.owner   =	THIS_MODULE,
	.open    =	vbd_open,
	.release =	vbd_release,
	.ioctl	 =	vbd_ioctl,
};

static int __init vbd_init(void) {
	struct l2lv_channel *channel;
	int err = -ENOMEM;
	int i = 0, ret;
	uint32_t offset = 0, bufsize;

	if (vbds_max > VBD_MAX_BD)
		vbds_max = VBD_MAX_BD;

	vbd_dev = kcalloc(vbds_max, sizeof(*vbd_dev), GFP_KERNEL);
	if (!vbd_dev)
		return -ENOMEM;

	vbd_major = register_blkdev(vbd_major, "vbd");
	if (vbd_major <= 0) {
		err = -EIO;
		goto err_reg_blkdev;
	}

	INFO("vbd: registered device at major %d\n", vbd_major);

	vbd_link = kcalloc(1, sizeof(*vbd_link), GFP_KERNEL);
	if (!vbd_link)
		goto err_alloc_link;

	spin_lock_init(&vbd_link->waitq_lock);
	INIT_LIST_HEAD(&vbd_link->req_list);
	init_waitqueue_head(&vbd_link->wait_q);
	mutex_init(&vbd_link->send_lock);

	// set up channel
	vbd_link->dev_link = l2lv_get_client_link();
	strcpy(vbd_link->shm_name, shm_name);

	ret = l2lv_add_channel(vbd_link->dev_link, L2LV_CHANNEL_VBD, vbd_link->shm_name, on_receive, 0, vbd_link);
	if (ret) {
		DBG("l2lv_add_channel(L2LV_CHANNEL_VBD) failed\n");
		goto err_add_channel;
	}

	channel = l2lv_get_channel_by_id(vbd_link->dev_link, L2LV_CHANNEL_VBD);
	ret = l2lv_client_open_channel(channel);
	if (ret){
		DBG("channel open failed!\n");
		goto err_open_ch;
	}
	bufsize = channel->shm.size / vbds_max;

	for (i = 0; i < vbds_max; i++) {
		struct gendisk *disk = alloc_disk(1);
		if (!disk)
			goto out;

		spin_lock_init(&vbd_dev[i].queue_lock);
		disk->queue = blk_init_queue(vbd_request, &vbd_dev[i].queue_lock);
		if (!disk->queue) {
			put_disk(disk);
			goto out;
		}

		// Tell the block layer that we are not a rotational device
		queue_flag_set_unlocked(QUEUE_FLAG_NONROT, disk->queue);

		disk->major			= vbd_major;
		disk->first_minor	= i;
		disk->fops			= &vbd_fops;
		disk->private_data	= &vbd_dev[i];

		sprintf(disk->disk_name, "vbd%d", i);
		add_disk(disk);

		vbd_dev[i].disk = disk;
		vbd_dev[i].vbd_id = i;
		vbd_dev[i].opened = false;
		vbd_dev[i].bufoffs = offset;
		vbd_dev[i].bufsize = bufsize;
		offset += bufsize;
		INFO("device %s: bufoffs=0x%lX, size=0x%lX\n", disk->disk_name,
			(unsigned long)vbd_dev[i].bufoffs, (unsigned long)vbd_dev[i].bufsize);
	}

	vbd_link->thread = kthread_run(vbd_req_thread, vbd_link, "vbd_thread");

	DBG("vbd initialized\n");

	return 0;

out:
	while (i--) {
		blk_cleanup_queue(vbd_dev[i].disk->queue);
		put_disk(vbd_dev[i].disk);
	}
err_open_ch:
	l2lv_remove_channel(vbd_link->dev_link, L2LV_CHANNEL_VBD);
err_add_channel:
	kfree(vbd_link);
err_alloc_link:
	unregister_blkdev(vbd_major, "vbd");
err_reg_blkdev:
	kfree(vbd_dev);
	return err;
}

static void __exit vbd_exit(void)
{
	int i;
	for (i = 0; i < vbds_max; i++) {
		struct gendisk *disk = vbd_dev[i].disk;
		if (disk) {
			del_gendisk(disk);
			blk_cleanup_queue(disk->queue);
			put_disk(disk);
		}
	}
	kfree(vbd_link);
	unregister_blkdev(vbd_major, "vbd");
	kfree(vbd_dev);
	printk(KERN_INFO "vbd: unregistered device at major %d\n", NBD_MAJOR);
}

#ifndef MODULE
// This is needed in order for the init function to be called *after* block layer init
late_initcall(vbd_init);
__exitcall(vbd_exit);
#else
module_init(vbd_init);
module_exit(vbd_exit);
#endif

MODULE_DESCRIPTION("Virtual Block Device");
MODULE_LICENSE("LGPL");

module_param(vbds_max, int, 0444);
MODULE_PARM_DESC(vbds_max, "number of virtual block devices to initialize (default: 16)");
module_param(shm_name, charp, 0444);
MODULE_PARM_DESC(shm_name, "name of shared memory (default: shm_name)");
