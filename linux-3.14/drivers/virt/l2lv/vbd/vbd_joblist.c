/*  vbd/vbd_joblist.c
 *
 *  Copyright (C) 2014 OpenSynergy GmbH
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

#include <asm/uaccess.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/syscalls.h>

#include "vbd_joblist.h"

static void lock_joblist(l2lv_block_joblist_t *list){
    mutex_lock(&list->lock);
}

static void unlock_joblist(l2lv_block_joblist_t *list){
    mutex_unlock(&list->lock);
}

static void on_transfer_req(struct l2lv_channel *channel, vbd_tx_req_msg_t* req_message, vbd_info *vbd_info) {
    vbd_tx_ack_msg_t *ack_message;
    unsigned long        size                = req_message->size;
    unsigned long long   offset          = req_message->offset;
    mm_segment_t         old_fs              = get_fs();
    struct file         *filp                  = NULL;
    int                  ret = -EINVAL;

    if (req_message->buf_offs + size > channel->shm.size){
        LOG("error: request exceeds buffer\n");
        LOG("\t%s: buf_offs=0x%lX\n", vbd_info->part_name, req_message->buf_offs);
        goto send_ack;
    }
    if ((req_message->buf_offs & 0x1FFFF) != 0){
        LOG("error: strange buffer offset\n");
        LOG("\t%s: buf_offs=0x%lX\n", vbd_info->part_name, req_message->buf_offs);
        goto send_ack;
    }
    if (IS_ERR(vbd_info)) {
        goto send_ack;
    }
    set_fs(KERNEL_DS);
    filp = filp_open(vbd_info->part_name, O_RDWR, 0);
    if (!IS_ERR(filp)) {
        ret = 0;
        if (req_message->flags & VBD_WRITE) {
            ret = vfs_write(filp, channel->shm.kernel_vaddr + req_message->buf_offs, size, &offset);
        } else {
            ret = vfs_read(filp, channel->shm.kernel_vaddr + req_message->buf_offs, size, &offset);
        }
        DBG("%s %d of %lu bytes at offset 0x%X\n", (req_message->flags & VBD_WRITE ? "Wrote" : "Read"), ret, size, offset);
        DBG("%s: buf_offs=0x%lX\n", vbd_info->part_name, req_message->buf_offs);
        if (ret != size) {
            ret = -EAGAIN;
        }
        if (req_message->flags & VBD_FLUSH_CACHE) {
            vfs_fsync(filp, 0);
        }
        filp_close(filp, NULL);
    }
    set_fs(old_fs);

send_ack:
    //DBG("Sending TRANSFER_ACK\n");
    ack_message                              = (vbd_tx_ack_msg_t *)l2lv_get_send_buf(channel->link);
    ack_message->header.header.message_type  = VBD_TRANSFER_ACK;
    ack_message->header.vbd_id                = req_message->header.vbd_id;
    ack_message->retval                      = ret;
    l2lv_send(channel->link, L2LV_CHANNEL_VBD, sizeof(vbd_tx_ack_msg_t), 0);
    return;
}

static void on_vbd_sync_req(struct l2lv_channel *channel, vbd_capa_req_msg_t *req_message, vbd_info *vbd_info) {
    vbd_sync_ack_msg_t *ack_message;
    mm_segment_t old_fs              = get_fs();
    struct file *filp                  = NULL;
    int ret = 0;

    set_fs(KERNEL_DS);
    filp = filp_open(vbd_info->part_name, O_RDWR, 0);
    if (!IS_ERR(filp)) {
        vfs_fsync(filp, 0);
        filp_close(filp, NULL);
    } else {
        ret = -EINVAL;
    }
    set_fs(old_fs);
    ack_message                              = (vbd_sync_ack_msg_t *)l2lv_get_send_buf(channel->link);
    ack_message->header.header.message_type  = VBD_SYNC_ACK;
    ack_message->header.vbd_id                = req_message->header.vbd_id;
    ack_message->retval                      = ret;
    l2lv_send(channel->link, L2LV_CHANNEL_VBD, sizeof(vbd_sync_ack_msg_t), false);
    return;
}


static int block_joblist_thread(void *arg){
    l2lv_block_joblist_t *list = arg;
    for (;;){
        l2lv_block_job_t *job = 0;
        wait_event(list->waitq, kthread_should_stop() || !list_empty(&list->list));
        if (kthread_should_stop()){
            break;
        }
        lock_joblist(list);
        if (!list_empty(&list->list)){
            job = list_first_entry(&list->list, l2lv_block_job_t, node);
            list_del(&job->node);
        }
        unlock_joblist(list);
        if (job != 0){
            switch (job->req_msg.hdr.message_type){
            case VBD_TRANSFER_REQ:
                on_transfer_req(job->channel, &job->req_msg.tx_msg, job->info);
                break;
            case VBD_SYNC_REQ:
                on_vbd_sync_req(job->channel, &job->req_msg.sync_msg, job->info);
                break;
            default:
                ;
            }
            lock_joblist(list);
            list_add(&job->node, &list->freelist);
            unlock_joblist(list);
        }
    }
    return 0;
}



void l2lv_block_joblist_init   (l2lv_block_joblist_t *list){
    uint32_t i;
    INIT_LIST_HEAD(&list->list);
    INIT_LIST_HEAD(&list->freelist);
    for (i=0; i<L2LV_BLOCK_MAX_JOBS; ++i){
        list_add(&list->jobs[i].node, &list->freelist);
    }
    mutex_init(&list->lock);
    init_waitqueue_head(&list->waitq);
    list->thread = kthread_run(block_joblist_thread, list, "block_joblist");
}
EXPORT_SYMBOL(l2lv_block_joblist_init);

void l2lv_block_joblist_free   (l2lv_block_joblist_t *list){
    (void)kthread_stop(list->thread);
    wake_up(&list->waitq);
}
EXPORT_SYMBOL(l2lv_block_joblist_free);

int  l2lv_block_joblist_add_req(l2lv_block_joblist_t *list, struct l2lv_channel *channel, struct l2lv_msg_header *req_message, vbd_info *info){
    l2lv_block_job_t     *job;
    if (list_empty(&list->freelist)){
        LOG("out of free list elements, loose information...\n");
        return -ENOMEM;
    }
    lock_joblist(list);
    job = list_first_entry(&list->freelist, l2lv_block_job_t, node);
    list_del(&job->node);

    job->channel = channel;
    switch (req_message->message_type){
    case VBD_TRANSFER_REQ:
        job->req_msg.tx_msg = *((vbd_tx_req_msg_t*)req_message);
        break;
    case VBD_SYNC_REQ:
        job->req_msg.sync_msg = *((vbd_capa_req_msg_t*)req_message);
        break;
    default:
        ;
    }
    job->info    = info;

    list_add_tail(&job->node, &list->list);
    unlock_joblist(list);
    wake_up(&list->waitq);
    return 0;
}
EXPORT_SYMBOL(l2lv_block_joblist_add_req);


