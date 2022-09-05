/*
 * Copyright (C) 2013-2016 OpenSynergy GmbH
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

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/l2lv/l2lv_defs.h>

#include "vm_port.h"

#include <asm/vmapi_calls.h>
#include <vmport_core.h>

struct vm_port {
	struct vmport_core_portinfo *iport;
	struct vmport_core_portinfo *oport;
	unsigned char sendbuf[L2LV_PORT_SIZE];
};

static struct vmport_core_portinfo *p4_get_qport(const char *portname,
					         uint32_t portsize,
						 int direction)
{
	struct vmport_core_portinfo *port = NULL;
	unsigned int j;

	for (j = 0; j < vmport_core_count; ++j)	{
		port = &vmport_core_portinfo[j];

		if (strcmp(port->portname, portname) == 0
		    && port->framesize == portsize
		    && port->direction == direction)
			break;

		port = NULL;
	}

	return port;
}

int vm_port_open(const struct l2lv_config_port *port_config,
		 struct vm_port **port_ret)
{
	int err = -EINVAL;
	struct vm_port *port;
	const char *ip_name = port_config->id[L2LV_PORT_RECEIVE];
	const char *op_name = port_config->id[L2LV_PORT_SEND];

	port = kzalloc(sizeof(*port), GFP_KERNEL);
	if (!port)
		return -ENOMEM;

	/* Initialize receiver port */

	port->iport = p4_get_qport(ip_name, L2LV_PORT_SIZE,
				   VMPORT_CORE_DIR_RECV);
	if (!port->iport) {
		pr_err("Cannot find receiver port %s of size %d\n",
			ip_name, L2LV_PORT_SIZE);
		goto error;
	}

	err = vmport_core_init_port(port->iport, port_config->rcv_isr,
				    port_config->rcv_user,
				    VMPORT_CORE_TYPE_LINK, NULL, 1);
	if (err) {
		pr_err("Failed to initialize receiver port: %s\n", ip_name);
		goto error;
	}

	/* Initialize sender port */

	port->oport = p4_get_qport(op_name, L2LV_PORT_SIZE,
				   VMPORT_CORE_DIR_SEND);
	if (!port->oport) {
		pr_err("Cannot find sender port %s of size %d\n",
			ip_name, L2LV_PORT_SIZE);
		goto error;
	}

	err = vmport_core_init_port(port->oport, NULL, NULL,
				    VMPORT_CORE_TYPE_LINK, NULL, 0);
	if (err) {
		pr_err("Failed to initialize sender port: %s\n", op_name);
		goto error;
	}

	*port_ret = port;

	return err;

error:
	if (port) {
		if (port->iport) {
			vmport_core_poll_stop(port->iport);
			vmport_core_exit_port(port->iport);
		}
		if (port->oport)
			vmport_core_exit_port(port->oport);
		kfree(port);
	}
	return err;
}

void vm_port_close(struct vm_port *port)
{
	if (port->oport)
		vmport_core_exit_port(port->oport);

	if (port->iport) {
		vmport_core_poll_stop(port->iport);
		vmport_core_exit_port(port->iport);
	}

	kfree(port);
}

void vm_port_interrupt_enable(struct vm_port *port)
{
	vmport_core_poll_start(port->iport);
}

void vm_port_interrupt_disable(struct vm_port *port)
{
	vmport_core_poll_stop(port->iport);
}

int vm_port_tx_state(struct vm_port *port)
{
	unsigned int nb_messages;
	unsigned int max_messages;

	vmapi_qport_stat_num_msg(port->oport->line, &nb_messages,
				 &max_messages);

	if (nb_messages == 0)
		return L2LV_PORT_SNDQ_FULL;
	else
		return L2LV_PORT_SNDQ_NOTFULL;
}

void *vm_port_get_sendbuf(struct vm_port *port)
{
	return port->sendbuf;
}

void vm_port_send(struct vm_port *port, size_t size)
{
	vmapi_qport_send(port->oport->line, port->sendbuf, size);
}

int vm_port_rx_state(struct vm_port *port)
{
	unsigned int nb_messages;
	unsigned int max_messages;

	vmapi_qport_stat_num_msg(port->iport->line, &nb_messages,
				 &max_messages);

	if (nb_messages == 0)
		return L2LV_PORT_RCVQ_EMPTY;
	else
		return L2LV_PORT_RCVQ_NOTEMPTY;
}

/*
 * Receive queuing port message from the input port
 *
 * The caller must guarantee to be race-free. l2lv_port_receive() must only
 * be called when l2lv_port_rx_state() returned VM_PORT_RCVQ_NOTEMPTY. The
 * size of the buffer at *buf must be at least L2LV_PORT_SIZE bytes.
 */
void vm_port_receive(struct vm_port *port, void *buf, size_t *size)
{
	int count;

	count = vmapi_qport_receive(port->iport->line, buf, L2LV_PORT_SIZE);
	if (count < 0)
		pr_warn("Failed to receive from port %s\n",
			port->iport->portname);

	*size = count;
}
