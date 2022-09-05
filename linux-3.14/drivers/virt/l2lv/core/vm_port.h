/* Copyright (C) 2013-2016 OpenSynergy GmbH
 * This file is part of the OpenSynergy VM framework
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
#ifndef _VM_PORT_H_
#define _VM_PORT_H_

#include "l2lv_port_defs.h"
#include <linux/types.h>

/*
 * Open port
 * Returns 0 on success, -E<ERRNO> when error occurs.
 */
int   vm_port_open(const struct l2lv_config_port *port_config, struct vm_port **port_ret);

/*
 * Close port
 */
void  vm_port_close(struct vm_port *port_obj);

/*
 * Enable/disable port interrupts
 */
void vm_port_interrupt_enable(struct vm_port *port);
void vm_port_interrupt_disable(struct vm_port *port);

/*
 * Sending
 * Caller must guarantee to be race-free.
 * l2lv_port_get_sendbuf and l2lv_port_send must be called alternately.
 */
int   vm_port_tx_state(struct vm_port *port_obj);
void *vm_port_get_sendbuf(struct vm_port *port_obj);
void  vm_port_send(struct vm_port *port_obj, size_t msgsize);

/*
 * Receiving
 * Caller must guarantee to be race-free.
 * l2lv_port_receive must only be called when l2lv_port_rx_state returned
 *   VM_PORT_RCVQ_NOTEMPTY.
 * buf must hold at least L2LV_PORT_SIZE bytes.
 */
int   vm_port_rx_state(struct vm_port *port_obj);
void  vm_port_receive(struct vm_port *port_obj, void *buf, size_t *size);

#endif				/* _VM_PORT_H_ */
