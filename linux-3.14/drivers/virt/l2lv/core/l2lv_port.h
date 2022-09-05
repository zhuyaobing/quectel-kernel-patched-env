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
#ifndef _L2LV_PORT_H_
#define _L2LV_PORT_H_

#include "l2lv_port_defs.h"

struct l2lv_port {
	int mode;
	union {
		struct vm_port *vm_port;
		struct l2lv_local_port *local_port;
	} p;
};

/*
 * Open port
 * Returns 0 on success, -E<ERRNO> when error occurs.
 */
int   l2lv_port_open(struct l2lv_port *port_obj, const struct l2lv_config_port *port_config);

/*
 * Close port
 */
void  l2lv_port_close(struct l2lv_port *port_obj);

/*
 * Sending
 * Caller must guarantee to be race-free.
 * l2lv_port_get_sendbuf and l2lv_port_send must be called alternately.
 */
int   l2lv_port_tx_state(struct l2lv_port *port_obj);
void *l2lv_port_get_sendbuf(struct l2lv_port *port_obj);
void  l2lv_port_send(struct l2lv_port *port_obj, size_t msgsize);

/*
 * Receiving
 * Caller must guarantee to be race-free.
 * l2lv_port_receive must only be called when l2lv_port_rx_state returned
 *   L2LV_PORT_RCVQ_NOTEMPTY.
 * buf must hold at least L2LV_PORT_SIZE bytes.
 */
int   l2lv_port_rx_state(struct l2lv_port *port_obj);
void  l2lv_port_receive(struct l2lv_port *port_obj, void *buf, size_t *size);

/*
 * Enables reception of interrupts
 * return true if port is not empty
 * in case of local port it always returns 0 while current implementation
 * of local ports doesn't support disabled-interrupts state
 */
int l2lv_port_interrupt_enable(struct l2lv_port *port_obj);

#endif				/* _L2LV_PORT_H_ */
