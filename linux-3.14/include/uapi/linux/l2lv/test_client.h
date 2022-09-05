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
#ifndef __TEST_CLIENT_H__
#define __TEST_CLIENT_H__

#ifdef __KERNEL__
#include <linux/ioctl.h>
#else
#include <sys/ioctl.h>
#endif

#include <linux/l2lv/l2lv.h>

/*
 * should switch the client from Unavail/Uninit to offline and then to ready
 * or from offline to ready
 */
#define TEST_IOC_OPEN			_IO('A', 1)

/* should switch from unavail/uninit to offline  */
#define TEST_IOC_OPEN_REQ		_IO('A', 2)

/* returns the state of the client channel */
#define TEST_IOC_GET_STATE		_IOR('A', 3, unsigned int *)

/*
 * sends a simple message on which the channel switches to DATA_REQ and then
 * back to READY
 */
#define TEST_IOC_SEND_FAKE_DATA		_IO('A', 4)

/* closes the channel on which the state should switch to unavail */
#define TEST_IOC_CLOSE			_IO('A', 5)

/*
 * sends data over shared memory same state switches as in
 * TEST_IOC_SEND_FAKE_DATA
 */
#define TEST_IOC_SEND_SHM		_IO('A', 6)

/*
 * sends a message on which server doesn't reply. Therefore the client stays
 * in REQ_DATA state. The test will then restart the server which cause the
 * sending of a server init message on which the client should switch back to
 * OFFLINE
 */
#define TEST_IOC_REQ_DATA_TO_OFFLINE	_IO('A', 7)

#define STATE_UNAVAIL	0
#define STATE_OFFLINE	1
#define STATE_READY	2
#define STATE_REQ_STATE	3
#define STATE_REQ_DATA	4
#define STATE_UNINIT	5

struct test_link {
	struct l2lv_link *test_link;
	char shm_name[L2LV_MAX_SHM_NAMELEN];
};

#endif
