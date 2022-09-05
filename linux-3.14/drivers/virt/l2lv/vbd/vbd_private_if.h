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
#ifndef _VBD_PRIVATE_IF_H_
#define _VBD_PRIVATE_IF_H_

#include <linux/l2lv/l2lv_defs.h>
#include <linux/types.h>
#include <linux/completion.h>

typedef struct vbd_msg_header {
    struct l2lv_msg_header  header;
    int                     vbd_id;
} vbd_msg_header_t;

/* Request messages: client -> server */
#define VBD_TRANSFER_REQ     (L2LV_MSGT_VBD_START + 0)
#define VBD_CAPACITY_REQ     (L2LV_MSGT_VBD_START + 1)
#define VBD_SYNC_REQ         (L2LV_MSGT_VBD_START + 2)
#define VBD_IOCTL_REQ        (L2LV_MSGT_VBD_START + 3)
#define VBD_DISCARD_REQ      (L2LV_MSGT_VBD_START + 4)

#if (L2LV_MSGT_VBD_RANGE < 9)
#error "Invalid range of message types for virtual block device"
#endif

typedef struct vbd_tx_req_msg {
    vbd_msg_header_t     header;
    unsigned int            flags;
    unsigned long           size;
    unsigned long long      offset;
    unsigned long           buf_offs;
} vbd_tx_req_msg_t;

typedef struct vbd_capa_req_msg {
    vbd_msg_header_t     header;
} vbd_capa_req_msg_t;

typedef struct vbd_ioctl_msg {
    vbd_msg_header_t     header;
    fmode_t                 mode;
    unsigned int            cmd;
} vbd_ioctl_msg_t;

typedef struct vbd_discard_req_msg {
	vbd_msg_header_t	header;
	sector_t sector;
	sector_t nr_sects;
	gfp_t gfp_mask;
	unsigned long flags;
	struct completion *rpc_done_ptr;
	int *ret_ptr;
} vbd_discard_req_msg_t;

/* Acknowledge messages: server -> client */
#define VBD_TRANSFER_ACK     (L2LV_MSGT_VBD_START + 5)
#define VBD_SYNC_ACK         (L2LV_MSGT_VBD_START + 6)
#define VBD_IOCTL_ACK        (L2LV_MSGT_VBD_START + 7)
#define VBD_CAPACITY_ACK     (L2LV_MSGT_VBD_START + 8)
#define VBD_DISCARD_ACK      (L2LV_MSGT_VBD_START + 9)

typedef struct vbd_tx_ack_msg {
    vbd_msg_header_t     header;
    int                  retval;
    struct completion *rpc_done_ptr;
    int *result_ptr;
} vbd_tx_ack_msg_t;

typedef struct vbd_sync_ack_msg {
    vbd_msg_header_t     header;
    int                  retval;
} vbd_sync_ack_msg_t;

typedef struct vbd_capa_ack_msg {
    vbd_msg_header_t     header;
    unsigned long long      capacity; // block device capacity in sectors
} vbd_capa_ack_msg_t;

typedef struct vbd_ioctl_ack_msg {
    vbd_msg_header_t     header;
    int                  retval;
} vbd_ioctl_ack_msg_t;

typedef struct vbd_discard_ack_msg {
	vbd_msg_header_t	header;
	int			retval;
	struct completion *rpc_done_ptr;
	int *ret_ptr;
} vbd_discard_ack_msg_t;


#endif /* _VBD_PRIVATE_IF_H_ */
