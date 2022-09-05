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
#ifndef _L2LV_DEFS_H_
#define _L2LV_DEFS_H_

#define CLIENT_SYNC_OPEN_ACK_TIMEOUT_MS	(100)

/* Maximum number of clients per virtual device */
#define L2LV_MAX_CLIENTS        (8)

/* Maximum number of channels per link */
#define L2LV_MAX_CHANNEL        (8)

/* Links */
#define L2LV_LINK_ID_INVALID (~0)
#define L2LV_LINK_ID_LOCAL   (~1)

#define L2LV_LOCAL_MSGQ_SIZE (128)

/* Changes must be synchronized with vmit.xml */
#define L2LV_PORT_SIZE        (256)

/* Channels */
#define L2LV_CHANNEL_VBD         (0x0FE)
#define L2LV_CHANNEL_INPUT       (0x0FF)
#define L2LV_CHANNEL_SHGFX_FB    (0x100)
#define L2LV_CHANNEL_SHGPU       (0x101)
#define L2LV_CHANNEL_SHGPU_EVENT (0x102)
#define L2LV_CHANNEL_VSCULL	     (0x103)
#define L2LV_CHANNEL_TEST	     (0x104)
#define L2LV_CHANNEL_DRM         (0x105)
/*
 * Message type namespaces.
 *
 * Message types are device driver specific, so define ranges only.
 */
#define L2LV_MSGT_START           (1024)
#define L2LV_MSGT_SYNC            (0)
#define L2LV_MSGT_VFB_START       (L2LV_MSGT_START)
#define L2LV_MSGT_VFB_RANGE       (16)

#define L2LV_MSGT_VBD_START       (L2LV_MSGT_VFB_START + \
				       L2LV_MSGT_VFB_RANGE)
#define L2LV_MSGT_VBD_RANGE       (16)

#define L2LV_MSGT_VGPU_START      (L2LV_MSGT_VBD_START + \
				       L2LV_MSGT_VBD_RANGE)
#define L2LV_MSGT_VGPU_RANGE      (32)

#define L2LV_MSGT_VSCULL_START    (L2LV_MSGT_VGPU_START + \
				       L2LV_MSGT_VGPU_RANGE)
#define L2LV_MSGT_VSCULL_RANGE    (16)

#define L2LV_MSGT_TEST_START      (L2LV_MSGT_VSCULL_START + \
				       L2LV_MSGT_VSCULL_RANGE)
#define L2LV_MSGT_TEST_RANGE      (8)

#define L2LV_MSGT_VDRM_START      (L2LV_MSGT_TEST_START + \
				       L2LV_MSGT_TEST_RANGE)
#define L2LV_MSGT_VDRM_RANGE      (32)

/* Synchronization events */
#define L2LV_SYNC_OPEN_REQ      (1)
#define L2LV_SYNC_OPEN_ACK      (2)
#define L2LV_SYNC_DATA_REQ      (3)
#define L2LV_SYNC_DATA_ACK      (4)
#define L2LV_SYNC_SERVER_INIT   (5)

/* Server states */
#define L2LV_SERVER_STATE_UNAVAIL (1)
#define L2LV_SERVER_STATE_OFFLINE (2)
#define L2LV_SERVER_STATE_READY   (3)

/* Client states */
#define L2LV_CLIENT_STATE_UNAVAIL   (6)
#define L2LV_CLIENT_STATE_OFFLINE   (7)
#define L2LV_CLIENT_STATE_READY     (8)
#define L2LV_CLIENT_STATE_REQ_DATA  (9)

#define L2LV_STATE_UNINITIALIZED  (121)

struct l2lv_msg_header {
	/* TODO: add CRC, when safety requirements appear */
	unsigned int channel_id;
	unsigned int channel_size; /* TODO: rename to message_size */
	unsigned int message_type;
};

struct l2lv_sync_msg {
	struct l2lv_msg_header header;
	unsigned int event;
};

#endif
