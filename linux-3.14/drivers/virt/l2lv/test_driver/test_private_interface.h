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
#ifndef __TEST_DRIVER_H__
#define __TEST_DRIVER_H__

#include <linux/l2lv/l2lv_defs.h>

/* Simple message just to test that a client can send to the server */
#define TEST_SEND_MSG_REQ		(L2LV_MSGT_TEST_START + 0)

/* Acknowledgement that shows that the server received the client message */
#define TEST_SEND_MSG_ACK		(L2LV_MSGT_TEST_START + 1)

/* Send data over the shared memory */
#define TEST_SEND_DATA_SHM_REQ		(L2LV_MSGT_TEST_START + 2)

/* Acknowledge that the server received the correct data in shared memory */
#define TEST_SEND_DATA_SHM_ACK		(L2LV_MSGT_TEST_START + 3)

/*
 * Sends a message on which the server will not reply so client stays
 * in request_data state. In the test the server will then be restarted
 * which leads to a server_init message on which the client should switch
 * to offline. Just necessary to ensure this transition works.
 */
#define TEST_SEND_DATA_TO_OFFLINE_REQ	(L2LV_MSGT_TEST_START + 4)
#endif
