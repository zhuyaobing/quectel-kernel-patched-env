/* Copyright (C) 2013-2016 OpenSynergy GmbH
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
#ifndef _L2LV_PORT_DEFS_H_
#define _L2LV_PORT_DEFS_H_

/* Opaque type to represent a full-duplex port */
struct vm_port;
struct l2lv_local_port;

/* Index of port-id in configuration: */
#define L2LV_PORT_RECEIVE (0)
#define L2LV_PORT_SEND (1)
#define L2LV_PORT_FULLDUPLEX (0)

struct l2lv_config_port {
	const char *id[2];			/* receive ([0]) and send ([1]) port-ids */
	void (*rcv_isr)(void *);	/* receive indication ISR */
	void *rcv_user;				/* user pointer passed to the rcv_isr */
};

#define L2LV_PORT_SNDQ_NOTFULL (0)
#define L2LV_PORT_SNDQ_FULL (1)

#define L2LV_PORT_RCVQ_NOTEMPTY (0)
#define L2LV_PORT_RCVQ_EMPTY (1)

#endif				/* _L2LV_PORT_DEFS_H_ */
