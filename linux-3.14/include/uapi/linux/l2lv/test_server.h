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
#ifndef __TEST_SERVER_H__
#define __TEST_SERVER_H__

#ifdef __KERNEL__
#include <linux/ioctl.h>
#else
#include <sys/ioctl.h>
#endif

#include <linux/l2lv/l2lv.h>

/* Return server channel state to userspace */
#define TEST_IOC_GET_STATE	_IOR('Z', 1, unsigned int *)

/* Setup links */
#define TEST_IOC_SET_LINK	_IOW('Z', 2, struct test_set_link *)

#define STATE_UNAVAIL	0
#define STATE_OFFLINE	1
#define STATE_READY	2

struct test_set_link {
	int link_id;
	char shm_name[L2LV_MAX_SHM_NAMELEN];
	char ip_name[L2LV_MAX_PORT_NAMELEN];
	char op_name[L2LV_MAX_PORT_NAMELEN];
	int access;
};

#endif
