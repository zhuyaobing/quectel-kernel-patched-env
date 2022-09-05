/*  vbd/vbd_link.c
 *
 *  Copyright (C) 2013-2014 OpenSynergy GmbH
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <linux/file.h>
#include <linux/genhd.h>
#include <linux/hrtimer.h>
#include <linux/ioctl.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/random.h>

#include <asm/uaccess.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/syscalls.h>

#include <linux/l2lv/l2lv_defs.h>
#include "vbd_debug.h"
#include "vbd_defs.h"
#include "vbd_interface.h"
#include "vbd_link.h"
#include "vbd_joblist.h"

MODULE_LICENSE("GPL");

void vbd_init_links(vbd_links_info *links_info) {
	links_info->drv_links = l2lv_server_get_links();
    links_info->joblist = kmalloc(sizeof(l2lv_block_joblist_t), GFP_KERNEL);
    l2lv_block_joblist_init(links_info->joblist);
}
EXPORT_SYMBOL(vbd_init_links);

void vbd_destroy_links(vbd_links_info *links_info) {
	struct l2lv_links *drv_links = links_info->drv_links;
	unsigned int i;
	for (i=0; i<drv_links->links_count; ++i){
		struct l2lv_link    *drv_link = &drv_links->links[i];
		struct l2lv_channel	*channel  = l2lv_get_channel_by_id(drv_link, L2LV_CHANNEL_VBD);
		if (channel){
			l2lv_remove_channel(drv_link, L2LV_CHANNEL_VBD);
		}
		if (drv_link->channels_count == 0){
			l2lv_remove_link(drv_links, drv_link->link_id);
			--i;
		}
	}

    l2lv_block_joblist_free(links_info->joblist);
    kfree(links_info->joblist);
    links_info->joblist = 0;
}

static vbd_info* get_vbd_info(vbd_infos *vbd_infos, int id) {
	if (!vbd_infos)
		goto err;

	if (id >= vbd_infos->count)
		goto err;

	return vbd_infos->infos[id];

err:
	return ERR_PTR(-EINVAL);
}

static void on_vbd_capacity_req(struct l2lv_channel *channel, vbd_capa_req_msg_t *req_message) {
	vbd_capa_ack_msg_t *ack_message;
	struct vbd_infos *vbd_infos = channel->user_obj;
	struct vbd_info  *vbd_info;
	struct block_device *bdev;
	struct gendisk *disk;
	struct hd_struct *partition;
	int partno = 0;
	unsigned long long capacity = 0;

	vbd_info = get_vbd_info(vbd_infos, req_message->header.vbd_id);
	if (IS_ERR(vbd_info)) {
		LOG("invalid block device id\n");
		goto out;
	}

	bdev = lookup_bdev(vbd_info->part_name);
	if (IS_ERR(bdev)) {
		LOG("block device for %s does not exist\n", vbd_info->part_name);
		goto out;
	}

	disk = get_gendisk(bdev->bd_dev, &partno);
	if (!disk) {
		LOG("Gendisk for %s not found\n", vbd_info->part_name);
		goto out;
	}

	partition = disk_get_part(disk, partno);
	if (!partition) {
		LOG("Partition for %s not found\n", vbd_info->part_name);
		goto out;
	}

	capacity = (unsigned long long)partition->nr_sects;

out:
	ack_message = (vbd_capa_ack_msg_t *)l2lv_get_send_buf(channel->link);
	ack_message->header.header.message_type   = VBD_CAPACITY_ACK;
	ack_message->header.vbd_id				= req_message->header.vbd_id;
	ack_message->capacity					 = capacity;
	DBG("Send VBD_CAPACITY_ACK\n");
	l2lv_send(channel->link, L2LV_CHANNEL_VBD, sizeof(vbd_capa_ack_msg_t), 0);
	return;
}

extern int blkdev_issue_discard(struct block_device *bdev, sector_t sector,
	sector_t nr_sects, gfp_t gfp_mask, unsigned long flags);
extern int blkdev_get(struct block_device *bdev, fmode_t mode, void *holder);
extern void blkdev_put(struct block_device *bdev, fmode_t mode);

static void on_vbd_discard_req(struct l2lv_channel *channel, vbd_discard_req_msg_t *req_message) {
	vbd_discard_ack_msg_t	*ack_message;
	struct vbd_infos	*vbd_infos = channel->user_obj;
	struct vbd_info		*vbd_info;
	struct block_device	*bdev;
	int			ret;

	vbd_info = get_vbd_info(vbd_infos, req_message->header.vbd_id);
	if (IS_ERR(vbd_info)) {
		LOG("invalid block device id\n");
		ret = -EINVAL;
		goto out;
	}

	bdev = lookup_bdev(vbd_info->part_name);
	if (IS_ERR(bdev)) {
		LOG("block device for %s does not exist\n", vbd_info->part_name);
		ret = -ENODEV;
		goto out;
	}

	if(blkdev_get(bdev, FMODE_READ|FMODE_WRITE|FMODE_EXCL, &bdev) < 0) {
		ret = -EBUSY;
		goto out;
	}

	ret = blkdev_issue_discard(bdev,
			req_message->sector,
			req_message->nr_sects,
			req_message->gfp_mask,
			req_message->flags);

	blkdev_put(bdev, FMODE_READ|FMODE_WRITE|FMODE_EXCL);

out:
	ack_message = (vbd_discard_ack_msg_t *)l2lv_get_send_buf(channel->link);
	ack_message->header.header.message_type = VBD_DISCARD_ACK;
	ack_message->header.vbd_id = req_message->header.vbd_id;
	ack_message->retval = ret;
	ack_message->ret_ptr = req_message->ret_ptr;
	ack_message->rpc_done_ptr = req_message->rpc_done_ptr;
	DBG("Send VBD_DISCARD_ACK\n");
	l2lv_send(channel->link, L2LV_CHANNEL_VBD, sizeof(vbd_discard_ack_msg_t), 0);
	return;
}

static void on_vbd_ioctl_req(struct l2lv_channel *channel, vbd_ioctl_msg_t *req_message) {
	vbd_ioctl_ack_msg_t *ack_message;
	struct vbd_infos *vbd_infos = channel->user_obj;
	vbd_info  *vbd_info;
	long ret = 0;

	vbd_info = get_vbd_info(vbd_infos, req_message->header.vbd_id);
	if (IS_ERR(vbd_info)) {
		DBG("invalid block device id\n");
		goto out;

	}

	switch (req_message->cmd) {
	case BLKFLSBUF:
		{
			ret = vbd_sys_ioctl(vbd_info->part_name);
			if(IS_ERR_VALUE(ret))
				LOG("Forwarding IOCTL failed 0x%lx\n", ret);
			break;
		}
	default:
		LOG("Received unknown IOCTL\n");
		break;
	}
out:
	ack_message = (vbd_ioctl_ack_msg_t *)l2lv_get_send_buf(channel->link);
	ack_message->header.header.message_type   = VBD_IOCTL_ACK;
	ack_message->header.vbd_id                = req_message->header.vbd_id;
	ack_message->retval 			  = ret;
	DBG("Send VBD_IOCTL_ACK\n");
	l2lv_send(channel->link, L2LV_CHANNEL_VBD, sizeof(vbd_ioctl_ack_msg_t), false);
}

static void on_receive(struct l2lv_channel *channel, char *rcv_buffer, uint32_t size) {
	struct l2lv_msg_header *header    = (struct l2lv_msg_header*)rcv_buffer;
	struct vbd_infos	*vbd_infos = channel->user_obj;
	struct vbd_info		*vbd_info;

	if (size != header->channel_size){	  // TODO: can void?
		LOG("invalid message content: channel_size = %d, size = %d\n", header->channel_size, size);
		return;
	}

	switch (header->message_type){
	case L2LV_MSGT_SYNC:
		DBG("sync message from link %d, channel %d, event %d\n", channel->link->link_id, header->channel_id, ((struct l2lv_sync_msg*)rcv_buffer)->event);
		break;
	case VBD_TRANSFER_REQ:
        if (size != sizeof(vbd_tx_req_msg_t)){
            LOG("invalid message content: channel_size = %d, expected = %d\n", size, sizeof(vbd_tx_req_msg_t));
            return;
        }
        vbd_info  = get_vbd_info(vbd_infos, ((vbd_tx_req_msg_t *)rcv_buffer)->header.vbd_id);
        if (IS_ERR(vbd_info)) {
            LOG("invalid block device id\n");
            return;
        }
        l2lv_block_joblist_add_req(vbd_infos->links_info->joblist, channel, header, vbd_info);
		break;
	case VBD_CAPACITY_REQ:
		on_vbd_capacity_req(channel, (vbd_capa_req_msg_t *)rcv_buffer);
		break;
	case VBD_SYNC_REQ:
        vbd_info  = get_vbd_info(vbd_infos, ((vbd_capa_req_msg_t *)rcv_buffer)->header.vbd_id);
        if (IS_ERR(vbd_info)) {
            DBG("invalid block device id\n");
            return;
        }
        l2lv_block_joblist_add_req(vbd_infos->links_info->joblist, channel, header, vbd_info);
        break;
	case VBD_IOCTL_REQ:
		on_vbd_ioctl_req(channel, (vbd_ioctl_msg_t *)rcv_buffer);
		break;
	case VBD_DISCARD_REQ:
		on_vbd_discard_req(channel, (vbd_discard_req_msg_t *)rcv_buffer);
		break;
	default:
		LOG("received invalid msgtype %d\n", header->message_type);
	}
}

int vbd_set_link(vbd_links_info *links_info, const _vbd_setlink_t *set_link) {
	int				 ret		= 0;
	struct l2lv_links *drv_links = links_info->drv_links;
	struct l2lv_link  *drv_link  = l2lv_get_link_by_id(drv_links, set_link->link_id);
	struct l2lv_channel	*channel   = 0;
	struct vbd_info		*vbd_info;
	struct vbd_infos	*vbd_infos;

	// add link if not yet available
	if (drv_link == 0) {
		ret = l2lv_add_link(drv_links, set_link->link_id, set_link->ip_name, set_link->op_name);
		if (ret != 0){
			LOG("error: too many links\n");
			goto err_add_link;
		}
		drv_link = l2lv_get_link_by_id(drv_links, set_link->link_id);
	}

	/*
	 * get channel
	 * if not yet available, set up and init vbd_infos
	 * vbd_infos contains vbd_info structures
	 *
	 * if available get vbd_infos from channel data
	 */
	channel = l2lv_get_channel_by_id(drv_link, L2LV_CHANNEL_VBD);
	if (!channel) {
		vbd_infos = kmalloc(sizeof(*vbd_infos), GFP_KERNEL);
		vbd_infos->count = 0;
        vbd_infos->links_info = links_info;
		ret = l2lv_add_channel(drv_link, L2LV_CHANNEL_VBD, set_link->shm_name, on_receive, on_receive, vbd_infos);
		if (ret != 0) {
			LOG("l2lv_add_channel failed\n");
			goto err_add_channel;
		}
		channel = l2lv_get_channel_by_id(drv_link, L2LV_CHANNEL_VBD);
		channel->own_state = L2LV_SERVER_STATE_OFFLINE;
		l2lv_send_sync(drv_link, L2LV_CHANNEL_VBD, L2LV_SYNC_SERVER_INIT);
	} else {
		vbd_infos = channel->user_obj;
	}

	// set up vbd_info and add to vbd_infos

	if (vbd_infos->count == VBD_MAX_BD) {
		LOG("Maximum number of virtual block devices for link %d reached!\n", drv_link->link_id);
		goto err_max_vbd;
	}

	vbd_info = kmalloc(sizeof(*vbd_info), GFP_KERNEL);
	strcpy(vbd_info->part_name, set_link->part_name);
	vbd_infos->infos[vbd_infos->count++] = vbd_info;

	INFO("Link for virtual block device driver\n");
	INFO("  link id:		%d\n", set_link->link_id);
	INFO("  shared memory:  %s (%d kB at kernel address 0x%08x)\n", set_link->shm_name, (int)channel->shm.size >> 10, (uint32_t)channel->shm.kernel_vaddr);
	INFO("  input port:	 %s\n", set_link->ip_name);
	INFO("  output port:	%s\n", set_link->op_name);
	INFO("  count:		  %d\n", vbd_infos->count);
	INFO("  partition name: %s\n", set_link->part_name);
	return 0;

// error handling
err_max_vbd:
err_add_channel:
	if (drv_link != 0 && drv_link->channels_count == 0)
		l2lv_remove_link(drv_links, set_link->link_id);
err_add_link:
	return ret;
}
EXPORT_SYMBOL(vbd_set_link);

int vbd_remove_link(vbd_links_info *links_info, uint32_t link_id) {
	return 0;
}
