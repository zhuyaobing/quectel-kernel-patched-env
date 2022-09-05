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
#ifndef __VBD_SERVER_H__
#define __VBD_SERVER_H__

#ifndef __KERNEL__
#include <stdint.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#endif

#include <linux/l2lv/l2lv.h>

#define VBD_MAX_PART_NAMELEN 64

typedef struct vbd_setlink_t {
	int  link_id;
	int  vbd_id;							   /* id of vbd (/dev/vbd[0-9]) */
	char shm_name [L2LV_MAX_SHM_NAMELEN];  /* shared memory filename */
	char ip_name  [L2LV_MAX_PORT_NAMELEN]; /* name of input port */
	char op_name  [L2LV_MAX_PORT_NAMELEN]; /* name of output port */
	char part_name[VBD_MAX_PART_NAMELEN];   /* name of partition */
	int  access;
} _vbd_setlink_t;

#define VBD_IOC_INIT		_IOWR('I', 1, uint32_t)
#define VBD_IOC_SET_LINK	_IOWR('S', 1, _vbd_setlink_t)
#define VBD_IOC_REMOVE_LINK _IOWR('R', 1, uint32_t)

#endif /* __VBD_SERVER_H__ */
