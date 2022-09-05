/* Copyright (C) 2016 OpenSynergy GmbH
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

#include <linux/module.h>
#include <linux/types.h>
#include "l2lv_port.h"
#include "vm_port.h"
#if defined(CONFIG_L2LV_SERVER)
#include "l2lv_local_port.h"
#endif
#include <linux/l2lv/l2lv_defs.h>

#define L2LV_PORT_INVALID (0)
#define L2LV_PORT_VM_PORT (1)
#define L2LV_PORT_LOCAL_PORT (2)

int l2lv_port_interrupt_enable(struct l2lv_port *port_obj)
{
	/*
	 * Local ports don't support disabled-interrupts state.
	 * Always return 0 while lost of interrupts and data is not possible
	 * for local ports. Although to avoid some race conditions in
	 * l2lv_port_rx_state - l2lv_port_receive (see rx_thread and
	 * l2lv_port_clear_input_port) it is better to implement
	 * disabling interrupts for local ports
	 */
	if (port_obj->mode != L2LV_PORT_VM_PORT)
		return 0;

	vm_port_interrupt_enable(port_obj->p.vm_port);
	return (l2lv_port_rx_state(port_obj) == L2LV_PORT_RCVQ_NOTEMPTY);
}

static void l2lv_port_clear_input_port(struct l2lv_port *port_obj)
{
	int size;
	char rcv_buffer[L2LV_PORT_SIZE];
	while (l2lv_port_rx_state(port_obj) == L2LV_PORT_RCVQ_NOTEMPTY) {
		pr_debug("l2lv_port_clear_input_port: Removing junk from input port\n");
		l2lv_port_receive(port_obj, rcv_buffer, &size);
	}
}

int   l2lv_port_open(struct l2lv_port *port_obj, const struct l2lv_config_port *port_config) {
	int ret;

#if defined(CONFIG_L2LV_SERVER)

	if (port_config->id[0]) {
		port_obj->mode = L2LV_PORT_VM_PORT;
		ret = vm_port_open(port_config, &port_obj->p.vm_port);
		pr_debug("l2lv_port_open: mode=L2LV_PORT_VM_PORT: %s\n", (ret ? "failed" : "success"));
	} else {
		port_obj->mode = L2LV_PORT_LOCAL_PORT;
		ret = l2lv_local_port_open(port_config, &port_obj->p.local_port);
		pr_debug("l2lv_port_open: mode=L2LV_PORT_LOCAL_PORT: %s\n", (ret ? "failed" : "success"));
	}

#else

	if (port_config->id[0]) {
		port_obj->mode = L2LV_PORT_VM_PORT;
		ret = vm_port_open(port_config, &port_obj->p.vm_port);
		pr_debug("l2lv_port_open: mode=L2LV_PORT_VM_PORT: %s\n", (ret ? "failed" : "success"));
	} else {
		port_obj->mode = L2LV_PORT_INVALID;
		pr_warn("l2lv_port_open: invalid argument (missing portname)\n");
		ret = -EINVAL;
	}

#endif
	if (!ret) {
		l2lv_port_clear_input_port(port_obj);
	}
	return ret;
}

void l2lv_port_close(struct l2lv_port *port_obj) {
	if (port_obj->mode == L2LV_PORT_VM_PORT)
		vm_port_close(port_obj->p.vm_port);
#if defined(CONFIG_L2LV_SERVER)
	else if (port_obj->mode == L2LV_PORT_LOCAL_PORT)
		l2lv_local_port_close(port_obj->p.local_port);
#endif
}

int   l2lv_port_tx_state(struct l2lv_port *port_obj) {
	int ret = L2LV_PORT_SNDQ_FULL;
	if (port_obj->mode == L2LV_PORT_VM_PORT)
		ret = vm_port_tx_state(port_obj->p.vm_port);
#if defined(CONFIG_L2LV_SERVER)
	else if (port_obj->mode == L2LV_PORT_LOCAL_PORT)
		ret = l2lv_local_port_tx_state(port_obj->p.local_port);
#endif
	return ret;
}

void *l2lv_port_get_sendbuf(struct l2lv_port *port_obj) {
	void *ret = 0;
	if (port_obj->mode == L2LV_PORT_VM_PORT)
		ret = vm_port_get_sendbuf(port_obj->p.vm_port);
#if defined(CONFIG_L2LV_SERVER)
	else if (port_obj->mode == L2LV_PORT_LOCAL_PORT)
		ret = l2lv_local_port_get_sendbuf(port_obj->p.local_port);
#endif
	return ret;
}

void  l2lv_port_send(struct l2lv_port *port_obj, size_t msgsize) {
	if (port_obj->mode == L2LV_PORT_VM_PORT)
		vm_port_send(port_obj->p.vm_port, msgsize);
#if defined(CONFIG_L2LV_SERVER)
	else if (port_obj->mode == L2LV_PORT_LOCAL_PORT)
		l2lv_local_port_send(port_obj->p.local_port, msgsize);
#endif
}

int   l2lv_port_rx_state(struct l2lv_port *port_obj){
	int ret = L2LV_PORT_RCVQ_EMPTY;
	if (port_obj->mode == L2LV_PORT_VM_PORT)
		ret = vm_port_rx_state(port_obj->p.vm_port);
#if defined(CONFIG_L2LV_SERVER)
	else if (port_obj->mode == L2LV_PORT_LOCAL_PORT)
		ret = l2lv_local_port_rx_state(port_obj->p.local_port);
#endif
	return ret;
}

void  l2lv_port_receive(struct l2lv_port *port_obj, void *buf, size_t *size) {
	if (port_obj->mode == L2LV_PORT_VM_PORT)
		vm_port_receive(port_obj->p.vm_port, buf, size);
#if defined(CONFIG_L2LV_SERVER)
	else if (port_obj->mode == L2LV_PORT_LOCAL_PORT)
		l2lv_local_port_receive(port_obj->p.local_port, buf, size);
#endif
}

MODULE_LICENSE("LGPL");
