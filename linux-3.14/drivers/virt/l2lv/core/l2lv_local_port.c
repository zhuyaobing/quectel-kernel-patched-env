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
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/kthread.h>
#include <linux/l2lv/l2lv_defs.h>
#include "l2lv_local_port.h"
#include "l2lv_local_msgq.h"

#define L2LV_LP_GET_SENDBUF (0)
#define L2LV_LP_SEND (1)

struct l2lv_local_port {
	struct l2lv_local_port *remote;
	void (*rcv_isr)(void *);
	void *rcv_user;
	struct l2lv_local_msgq msgq;
	int send_last_called;
};

/*
 * Open port
 * Returns 0 on success, -E<ERRNO> when error occurs.
 */
int   l2lv_local_port_open(const struct l2lv_config_port *port_config, struct l2lv_local_port **port_ret) {
	struct l2lv_local_port *info;
	int ret;

	info = kzalloc(sizeof(struct l2lv_local_port), GFP_KERNEL);
	if (!info) {
		ret = -ENOMEM;
		goto error;
	}

	info->rcv_isr = port_config->rcv_isr;
	info->rcv_user = port_config->rcv_user;
	l2lv_local_msgq_init(&info->msgq);
	info->send_last_called = L2LV_LP_SEND;

	*port_ret = info;
	return 0;

error:
	if (info)
		kfree(info);
	return ret;
}

void  l2lv_local_port_set_remote(struct l2lv_local_port *port_obj, struct l2lv_local_port *remote) {
	port_obj->remote = remote;
}

/*
 * Close port
 */
void l2lv_local_port_close(struct l2lv_local_port *port_obj) {
	kfree(port_obj);
}

/*
 * Sending
 */
int   l2lv_local_port_tx_state(struct l2lv_local_port *port_obj) {
	if (!port_obj->remote) {
		pr_warn("l2lv_local_port_tx_state: not connected.\n");
		return L2LV_PORT_SNDQ_FULL;
	}
	return l2lv_local_msgq_isfull(&port_obj->remote->msgq) ?
		L2LV_PORT_SNDQ_FULL : L2LV_PORT_SNDQ_NOTFULL;
}

void *l2lv_local_port_get_sendbuf(struct l2lv_local_port *port_obj) {
	if (port_obj->send_last_called == L2LV_LP_GET_SENDBUF) {
		panic("l2lv_port_get_sendbuf: Execution order violation\n");
	}
	if (!port_obj->remote) {
		pr_warn("l2lv_local_port_get_sendbuf: not connected.\n");
		return 0;
	}
	port_obj->send_last_called = L2LV_LP_GET_SENDBUF;
	return l2lv_local_msgq_get_buffer(&port_obj->remote->msgq);
}

void  l2lv_local_port_send(struct l2lv_local_port *port_obj, size_t msgsize) {
	if (port_obj->send_last_called == L2LV_LP_SEND) {
		panic("l2lv_port_send: Execution order violation\n");
	}
	if (msgsize > L2LV_LMSGQ_MAX_MSGSIZE) {
		panic("l2lv_port_send: Too many bytes to send\n");
	}
	port_obj->send_last_called = L2LV_LP_SEND;
	l2lv_local_msgq_push(&port_obj->remote->msgq, msgsize);
	port_obj->remote->rcv_isr(port_obj->remote->rcv_user);
}

/*
 * Receiving
 */
int   l2lv_local_port_rx_state(struct l2lv_local_port *port_obj){
	return l2lv_local_msgq_isempty(&port_obj->msgq) ?
		L2LV_PORT_RCVQ_EMPTY : L2LV_PORT_RCVQ_NOTEMPTY;
}

void  l2lv_local_port_receive(struct l2lv_local_port *port_obj, void *buf, size_t *size) {
	if (l2lv_local_msgq_isempty(&port_obj->msgq)) {
		panic("l2lv_port_receive: Try to receive from empty queue\n");
	}
	// TODO: Examine execution context (are we l2lv_local_rx_irq_thread? -> panic...)
	l2lv_local_msgq_pop(&port_obj->msgq, buf, size);
}

MODULE_LICENSE("LGPL");
