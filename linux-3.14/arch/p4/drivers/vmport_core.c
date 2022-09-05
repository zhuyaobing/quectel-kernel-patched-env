/* Copyright (c) 2016 OpenSynergy GmbH */
/*
 *  P4VM port core driver
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 *          David Engraf <den@sysgo.com>
 * 
 *  Copyright (C) 2003-2012 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/version.h>
#include <asm/irq.h>
#include <asm/kernthreads.h>
#include <asm/vmapi_calls.h>
#include <asm/uaccess.h>
#include <asm/locking.h>
#include "vmport_core.h"

#define VMPORT_DEV_NAME		"vmport"
#define SELECT_THREAD       "vmport_select"

/*
 * global variables
 */
int vmport_core_count;		/* number of PORTs in system */
struct vmport_core_portinfo *vmport_core_portinfo;	/* port specific information */
static int select_thread_irq;

EXPORT_SYMBOL(vmport_core_count);
EXPORT_SYMBOL(vmport_core_portinfo);

/* --------------------------------------------------------------------- */

/*
 * Async thread and interrupt routines
 */

/*
 * void vmport_select_thread(unsigned int irq, void *dev)
 *
 * irq dispatcher thread and runs in P4 thread context
 *
 * NOTE: do not call any linux functions, as
 *       this code is not executed in linux context!
 */
static void vmport_select_thread(unsigned int irq, void *dev)
{
	int i, ready[vmport_core_count];
	struct vmport_core_portinfo *info;

	while (1)
	{
		vmapi_qport_select(ready);
		for (i = 0; i < vmport_core_count; i++)
		{
			if (ready[i])
			{
				info = &vmport_core_portinfo[i];
				if (info->external_buf &&
				    info->direction == VMPORT_CORE_DIR_RECV) {
					/*
					 * when the application gave us a buffer
					 * auto fill it and auto restart
					 */
					info->buf_size = vmapi_qport_receive(info->line, info->buf,
									     info->framesize);
				} else {
					/**
					* only goes from '0' to '1', poll_clear() will clear the
					* mask and therefore could go from '1' to '0'
					*/
					info->state_ready = 1;
					vmapi_qport_poll_clear(info->line);
				}

				do_IRQ(info->irq);
			}
		}
	}
}

/*
 * irqreturn_t vmport_core_irq(int irq, void *dev, struct pt_regs *regs)
 *
 * dispatches the IRQ to a registeres low-level handler
 * called by do_IRQ when buffer received or transmitted
 */
static irqreturn_t vmport_core_irq(int irq, void *dev)
{
	struct vmport_core_portinfo *info = (struct vmport_core_portinfo *)dev;

	if (info->irqfunc)
		info->irqfunc(info->irqdata);

	return IRQ_HANDLED;
}

int vmport_core_poll_start(struct vmport_core_portinfo *info)
{
	return vmapi_qport_poll_set(info->line);
}
EXPORT_SYMBOL(vmport_core_poll_start);

int vmport_core_poll_stop(struct vmport_core_portinfo *info)
{
	return vmapi_qport_poll_clear(info->line);
}
EXPORT_SYMBOL(vmport_core_poll_stop);

/*
 * void vmport_core_init_port(...)
 *
 * initialize a queuing port
 */
int vmport_core_init_port(struct vmport_core_portinfo *port,
			void (*func) (void *), void *data,
			unsigned int type, char *buf, int wakeup)
{
	unsigned long flags;
	int buf_type_external;
	int err;

	if (!vmapi_qport_check_state(port->line))
		return -ENXIO;

	if (buf != NULL) {
		/* allow driver to use own buffer (e.g. vmport_net) */
		/* TODO: check size, must be at least framesize */
		buf_type_external = 1;
	} else {
		buf = kmalloc(port->framesize, GFP_KERNEL);
		if (buf == NULL)
			return -ENOMEM;
		buf_type_external = 0;
	}

	/* update port info */
	spin_lock_irqsave(&port->lock, flags);

	if (port->count != 0) {
		if (!buf_type_external)
			kfree(buf);
		err = -EBUSY;
		goto out;
	}

	port->external_buf = buf_type_external;
	port->buf = buf;
	port->count++;
	port->irqfunc = func;
	port->irqdata = data;
	port->type = type;

	err = 0;
out:
	spin_unlock_irqrestore(&port->lock, flags);
	return err;
}
EXPORT_SYMBOL(vmport_core_init_port);

/*
 * void vmport_core_exit_port(...)
 *
 * destroy queuing port
 */
void vmport_core_exit_port(struct vmport_core_portinfo *port)
{
	unsigned long flags;

	spin_lock_irqsave(&port->lock, flags);

	port->count--;
	port->irqfunc = 0;
	port->type = VMPORT_CORE_TYPE_NONE;
	if (port->external_buf)
		port->external_buf = 0;
	else
		kfree(port->buf);
	port->buf = NULL;

	spin_unlock_irqrestore(&port->lock, flags);
}
EXPORT_SYMBOL(vmport_core_exit_port);

/*
 * void vmport_core_file_read(...)
 *
 * generic read function for port drivers
 */
int vmport_core_file_read(struct vmport_core_portinfo *iport,
			struct mutex *lock, wait_queue_head_t *wait,
			struct file *file, char __user *buffer, size_t count)
{
	int ret;
	unsigned int nb_messages, max_messages;
	ssize_t err;

	if (0 == count)
		return 0;

	/*
	 *  Check if the buffer is big enough for the data.
	 *  Data is always read as one message from the qport of size framesize.
	 */
	if (iport->framesize > count)
		return -EINVAL;

	/** check if data available */
	ret = vmapi_qport_stat_num_msg(iport->line, &nb_messages, &max_messages);
	if (ret) {
		/** could not get status */
		err = ret;
	}
	else if (nb_messages > 0) {
		/** data available */
		err = 0;
	}
	else if (file->f_flags & O_NONBLOCK) {
		/** no data but client does not want to wait */
		err = -EAGAIN;
	}
	else
	{
		/** no data but client wants to wait */
		vmapi_qport_poll_set(iport->line);

		err = wait_event_interruptible(*wait, iport->state_ready);
		iport->state_ready = 0;
	}
	
	/** no error = message available to read, we can block to read */
	if (!err) {
		err = vmapi_qport_receive(iport->line, iport->buf, iport->framesize);
		if (err > 0) {
			if (copy_to_user(buffer, iport->buf, err))
				err = -EFAULT;
		}
	}

	return err;
}
EXPORT_SYMBOL(vmport_core_file_read);

/*
 * void vmport_core_file_write(...)
 *
 * generic write function for port drivers
 */
int vmport_core_file_write(struct vmport_core_portinfo *oport,
			struct mutex *lock, wait_queue_head_t *wait,
			struct file *file, const char __user *buffer,
			size_t count)
{
	int ret;
	unsigned int nb_messages, max_messages;
	ssize_t err;

	if (0 == count)
		return 0;

	/** check if buffer size fits */
	if (count > oport->framesize)
		return -EMSGSIZE;

	/** check if data available */
	ret = vmapi_qport_stat_num_msg(oport->line, &nb_messages, &max_messages);
	if (ret) {
		/** could not get status */
		err = ret;
	}
	else if (nb_messages > 0) {
		/** free space available */
		err = 0;
	}
	else if (file->f_flags & O_NONBLOCK) {
		/** no free space but client does not want to wait */
		err = -EAGAIN;
	}
	else
	{
		/** no data but client wants to wait */
		vmapi_qport_poll_set(oport->line);

		err = wait_event_interruptible(*wait, oport->state_ready);
		oport->state_ready = 0;
	}
	
	/** no error = free space available, we can block to write */
	if (0 == err) {
		if (copy_from_user(oport->buf, buffer, count))
			err = -EFAULT;
		else
			err = vmapi_qport_send(oport->line, oport->buf, count);
	}

	return err;
}
EXPORT_SYMBOL(vmport_core_file_write);

/*
 * query number of unread / free messages in a port
 */
int vmport_core_stat_num_msg(struct vmport_core_portinfo *port,
			     unsigned int *nb_messages, unsigned int *max_messages)
{
	int err;

	/* NOTE: this can safely be called from Linux context (never blocks) */
	err = vmapi_qport_stat_num_msg(port->line, nb_messages, max_messages);

	return err;
}
EXPORT_SYMBOL(vmport_core_stat_num_msg);

/* --------------------------------------------------------------------- */

/*
 * sysfs filesystem related code
 */

/*
 * int vmport_core_proc_read(...)
 *
 * prints some nice proc info
 */
static ssize_t config_show(struct device_driver *driver, char *buf)
{
	struct vmport_core_portinfo *info;
	unsigned long flags;
	int len, i;
	char *p;

	p = buf;
	len = 0;

	p += sprintf(p,
		     "# <id> <portname> <direction> <portsize> <use_counter> <type>\n");

	for (i = 0; i < vmport_core_count; i++) {
		info = &vmport_core_portinfo[i];

		spin_lock_irqsave(&info->lock, flags);

		/* states as strings  */
		p += sprintf(p, "%d %s ", i, info->portname);

		if (info->direction == VMPORT_CORE_DIR_RECV)
			p += sprintf(p, "RX");
		else
			p += sprintf(p, "TX");

		p += sprintf(p, " %d %d ", info->framesize, info->count);

		switch (info->type) {
		case VMPORT_CORE_TYPE_NONE:
			p += sprintf(p, "NONE");
			break;
		case VMPORT_CORE_TYPE_TTY:
			p += sprintf(p, "TTY");
			break;
		case VMPORT_CORE_TYPE_FILE:
			p += sprintf(p, "FILE");
			break;
		case VMPORT_CORE_TYPE_NET:
			p += sprintf(p, "NET");
			break;
		case VMPORT_CORE_TYPE_LINK:
			p += sprintf(p, "LINK");
			break;
		default:
			p += sprintf(p, "%d", info->type);
			break;
		}
		p += sprintf(p, "\n");

		spin_unlock_irqrestore(&info->lock, flags);
	}

	return strlen(buf);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)
static DRIVER_ATTR_RO(config);
#else
static DRIVER_ATTR(config, S_IRUGO, config_show, NULL);
#endif

/*
 * int vmport_core_free_resources(void)
 *
 * Free resources used by driver
 */
static void vmport_core_free_resources(void)
{
	struct vmport_core_portinfo *info;
	unsigned long flags;
	int i;

	if (select_thread_irq) {
		/* we cannot delete the select thread because it is blocked in
		 * the PSSW, see PR#7607-p4
		 */
		BUG();

		disable_irq_thread(select_thread_irq);
		/* don't call free_irq, this interrupt is not registered in the
		 * linux interrupt context
		 */
		release_async_irq(select_thread_irq);
		select_thread_irq = 0;
	}

	for (i = 0; i < vmport_core_count; i++) {
		info = &vmport_core_portinfo[i];

		spin_lock_irqsave(&info->lock, flags);
		if (info->irq)
			free_irq(info->irq, 0);
		spin_unlock_irqrestore(&info->lock, flags);

		/* free associated memory */
		kfree(info->portname);
		kfree(info->buf);
	}

	kfree(vmport_core_portinfo);
	vmport_core_count = 0;
}

/* --------------------------------------------------------------------- */

/*
 * Module initialization
 */

static struct platform_driver vmport_core_driver = {
	.driver = {
		   .name = VMPORT_DEV_NAME,
		   .owner = THIS_MODULE,
		   },
};

/*
 * int vmport_core_init(void)
 *
 * Initialize and register driver
 */
static int __init vmport_core_init(void)
{
	struct vmport_core_portinfo *info;
	int i, ret;

	vmport_core_count = vmapi_qport_get_count();

	/* allocate */
	vmport_core_portinfo =
	    kzalloc(vmport_core_count * sizeof(struct vmport_core_portinfo),
		    GFP_KERNEL);
	if (!vmport_core_portinfo)
		return -ENOMEM;

	/* each port ... safe part */
	for (i = 0; i < vmport_core_count; i++) {
		info = &vmport_core_portinfo[i];

		/* low level stuff */
		info->type = VMPORT_CORE_TYPE_NONE;
		spin_lock_init(&info->lock);
		info->line = i;
	}

	/* now we can call vmport_core_free_resources() for cleanup */

	/* unsafe part: collecting port info and allocate some buffers */
	for (i = 0; i < vmport_core_count; i++) {
		char *portname;
		int portnamesize;
		int isoutgoing;
		int irq;

		info = &vmport_core_portinfo[i];

		if (vmapi_qport_stat
		    (i, &isoutgoing, &info->framesize, &portname) < 0) {
			printk(KERN_ERR VMPORT_DEV_NAME
			       ": could not get port info for port %d\n", i);
			vmport_core_free_resources();
			return -ENODEV;
		}

		printk(KERN_INFO VMPORT_DEV_NAME
		       ": found port %s, size %d, %s\n", portname,
		       info->framesize, isoutgoing ? "tx" : "rx");
		/* portname */
		portnamesize = strlen(portname) + 1;
		info->portname = kmalloc(portnamesize, GFP_KERNEL);
		if (!info->portname) {
			printk(KERN_ERR VMPORT_DEV_NAME
			       ": memory allocation failed, %d bytes\n",
			       portnamesize);
			vmport_core_free_resources();
			return -ENOMEM;
		}

		/* check and allocate buffer */
		if (info->framesize <= 0) {
			printk(KERN_ERR VMPORT_DEV_NAME
			       ": unsupported framesize %d\n", info->framesize);
			vmport_core_free_resources();
			return -ENODEV;
		}

		/* port direction */
		strncpy(info->portname, portname, portnamesize);
		if (!isoutgoing)
			info->direction = VMPORT_CORE_DIR_RECV;
		else
			info->direction = VMPORT_CORE_DIR_SEND;

		irq = get_free_async_irq();

		/* allocate and enable IRQs */
		if (!irq || request_irq(irq, vmport_core_irq, 0, info->portname, info)) {
			printk(KERN_ERR VMPORT_DEV_NAME ": unable to request IRQ %d\n", irq);
			vmport_core_free_resources();
			return -ENODEV;
		}
		info->irq = irq;
		platform_device_register_simple(VMPORT_DEV_NAME, i, NULL, 0);
	}

	select_thread_irq = get_free_async_irq();
	if (!select_thread_irq) {
		vmport_core_free_resources();
		return -ENODEV;
	}

	/* don't call request_irq for select_thread_irq because the thread
	 * directly calls interrupt info->irq and doesn't need it's own
	 * linux interrupt handler
	 */
	
	if (enable_irq_thread(select_thread_irq, SELECT_THREAD, vmport_select_thread, vmport_core_portinfo)) {
		printk(KERN_ERR VMPORT_DEV_NAME ": unable to start async select thread\n");

		/* don't disable the irq thread, since it wasn't enabled... */
		release_async_irq(select_thread_irq);
		select_thread_irq = 0;
		vmport_core_free_resources();
		return -ENODEV;
	}

	ret = platform_driver_register(&vmport_core_driver);
	if (ret)
		return ret;

	return driver_create_file(&vmport_core_driver.driver, &driver_attr_config);
}

/*
 * void vmport_core_exit(void)
 *
 * Shutdown driver
 */
static void __exit vmport_core_exit(void)
{
	vmport_core_free_resources();
	driver_remove_file(&vmport_core_driver.driver, &driver_attr_config);
}

/* 
 * initialize the vmport core as early as possible so that other drivers 
 * like vmport_tty can use this ports for a console
 */
core_initcall(vmport_core_init);
module_exit(vmport_core_exit);

MODULE_AUTHOR("Alex Zuepke <azu@sysgo.de>");
MODULE_DESCRIPTION("PikeOS Communication Ports (PCP) core driver");
MODULE_LICENSE("GPL");
