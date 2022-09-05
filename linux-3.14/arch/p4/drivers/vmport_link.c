/*
 *  P4VM port "NO-tty" linked driver
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 *          David Engraf <den@sysgo.com>
 * 
 *  Copyright (C) 2006-2011 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/vmapi_calls.h>
#include "vmport_core.h"
#include "vmdrv_ioctl.h"

#ifdef DEBUG
#define DBG(x) x
#else
#define DBG(x)
#endif

/*
 * This driver links two ports (like the tty driver does), but doesn't
 * filter input and output through the TTY layer, hence the name "notty".
 *
 * The driver appears as /dev/vmportlink/<minor-number> in the filesystem.
 * Configuration can be done in /dev/vmportlink
 */

#define NOTTY_MAJOR		196
#define P4VM_DEV_NAME	"vmport_link"	/* device name */


struct notty_driverinfo
{
	/* filesystem interface */
	struct fasync_struct *fasync;
	wait_queue_head_t read_wait;
	wait_queue_head_t write_wait;

	/* internal states */
	int count;	/* if >0, this line is used */

	struct mutex lock;

	/* port interface */
	struct vmport_core_portinfo *iport;
	struct vmport_core_portinfo *oport;
};

/*
 * global variables
 */
static struct class *vm_class;
static struct platform_driver p4vm_driver;

#define P4VM_CHECK_DEVICE
#define P4VM_SHOW_ADDITIONAL_INFO "# <minor> <RX_port> <TX_port> <use_counter>"
#include "p4vm_gen_drv_interface.c"

/* --------------------------------------------------------------------- */

/*
 * Add and remove link logic
 */
static int p4vm_check_device(int state, unsigned int minor, const char *parm1,
						   const char *parm2)
{
	int i;
	struct vmport_core_portinfo *iport, *oport, *port;
	
	if (state == P4VM_STATE_DEVICE_ADD) {
		/* search for ports ... */
		iport = NULL;
		oport = NULL;

		for (i = 0; i < vmport_core_count; i++)
		{
			port = &vmport_core_portinfo[i];
			if (strcmp(port->portname, parm1) == 0 ||
				strcmp(port->portname, parm2) == 0)
			{
				if (port->direction == VMPORT_CORE_DIR_RECV)
					iport = port;
				else if (port->direction == VMPORT_CORE_DIR_SEND)
					oport = port;
			}
		}

		if (iport == NULL || oport == NULL)
			return -ENOENT;
	}

	return 0;
}

/* --------------------------------------------------------------------- */

/*
 * /proc filesystem related code
 */
static char *p4vm_proc_additional_info(struct p4vm_list *list, char *p)
{
	struct notty_driverinfo *priv = platform_get_drvdata(list->pdev);

	if (priv) {
		p += sprintf(p, "%d %s %s %d\n", list->minor, priv->iport->portname, 
					 priv->oport->portname, priv->count);
	}
	else {
		/* device not yet registered (probe not called) */
		p += sprintf(p, "%d not initialized\n", list->minor);
	}

	return p;

}

/* --------------------------------------------------------------------- */

/*
 * IRQ callback (called by vmport_core IRQs)
 */
static void notty_rx_int(void *data)
{
	struct notty_driverinfo *info = (struct notty_driverinfo *) data;

	/* notify fasync */
	if (info->fasync)
		kill_fasync(&info->fasync, SIGIO, POLL_IN);

	/* wakeup waiting ones */
	wake_up_interruptible(&info->read_wait);
}

static void notty_tx_int(void *data)
{
	struct notty_driverinfo *info = (struct notty_driverinfo *) data;

	/* notify fasync */
	if (info->fasync)
		kill_fasync(&info->fasync, SIGIO, POLL_OUT);

	/* wakeup waiting ones */
	wake_up_interruptible(&info->write_wait);
}


/* --------------------------------------------------------------------- */

/*
 * int notty_open(struct inode *inode, struct file *file)
 *
 * open file request: clear buffers, arm IRQ threads, ...
 */
static int notty_open(struct inode *inode, struct file *file)
{
	struct notty_driverinfo *info;
	unsigned int minor = iminor(inode);
	int err;
	struct p4vm_list *list;

	DBG(printk("b-OPEN(minor %d)\n", minor));

	info = NULL;
	mutex_lock(&p4vm_devlistlock);
	list_for_each_entry(list, &p4vm_devlist, list) {
		if (list->minor == minor) {
			info = platform_get_drvdata(list->pdev);
			break;
		}
	}
	mutex_unlock(&p4vm_devlistlock);

	if (!info)
		return -ENXIO;

	mutex_lock(&info->lock);

	/* don't allow opening more than once! */
	if (info->count) {
		DBG(printk("b-OPEN Error: device opened more than once\n"));
		err = -EBUSY;
		goto out_release;
	}

	err = vmport_core_init_port(info->iport, notty_rx_int, info, 
						  VMPORT_CORE_TYPE_LINK, NULL, 1);
	if (err)
		goto out_release;
	
	err = vmport_core_init_port(info->oport, notty_tx_int, info, 
								VMPORT_CORE_TYPE_LINK, NULL, 0);
	if (err) {
		vmport_core_exit_port(info->iport);
		goto out_release;
	}

	info->count++;
	file->private_data = info;

out_release:
	mutex_unlock(&info->lock);

	DBG(printk("b-OPEN returns %d\n", err));
	return err;
}

/*
 * int notty_release(struct inode *inode, struct file *file)
 *
 * release file request: disarm IRQ threads (if possible)
 */
static int notty_release(struct inode *inode, struct file *file)
{
	struct notty_driverinfo *info = (struct notty_driverinfo *)file->private_data;

	DBG(printk("b-RELEASE\n"));

	mutex_lock(&info->lock);

	info->count--;

	vmport_core_exit_port(info->iport);
	vmport_core_exit_port(info->oport);

	mutex_unlock(&info->lock);

	return 0;
}


/* --------------------------------------------------------------------- */

/*
 * ssize_t notty_read(struct file *file, char *buffer, size_t count, loff_t *ppos)
 *
 * read ...
 */
static ssize_t notty_read(struct file *file, char __user *buffer, size_t count,
			  loff_t *ppos)
{
	struct notty_driverinfo *info = (struct notty_driverinfo *)file->private_data;

	return vmport_core_file_read(info->iport, &info->lock, &info->read_wait,
								 file, buffer, count);
}


/* --------------------------------------------------------------------- */

/*
 * ssize_t notty_write(struct file *file, const char *buffer, size_t count, loff_t *ppos)
 *
 * write ...
 */
static ssize_t notty_write(struct file *file, const char __user *buffer,
			   size_t count, loff_t *ppos)
{
	struct notty_driverinfo *info = (struct notty_driverinfo *)file->private_data;

	return vmport_core_file_write(info->oport, &info->lock, &info->write_wait,
								  file, buffer, count);
}


/* --------------------------------------------------------------------- */

/*
 * int notty_fasync(int fd, struct file *file, int mode)
 *
 * common fasync stuff ... don't change
 */
static int notty_fasync(int fd, struct file *file, int mode)
{
	struct notty_driverinfo *info = (struct notty_driverinfo *)file->private_data;
	int retval;

	retval = fasync_helper(fd, file, mode, &info->fasync);
	if (retval < 0)
		return retval;
	return 0;
}

/* --------------------------------------------------------------------- */

/*
 * static int notty_ioctl(...)
 *
 * ioctl stuff
 */
static long notty_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct notty_driverinfo *info = file->private_data;

	switch (cmd)
	{
	/* get framesize of the RX port */
	case VMDRV_GET_PORT_SIZE:
		if (copy_to_user((void *)arg, &info->iport->framesize, sizeof(int)))
			return -EFAULT;
		break;

	/* get framesize of the TX port */
	case VMDRV_GET_PORT_SIZE2:
		if (copy_to_user((void *)arg, &info->oport->framesize, sizeof(int)))
			return -EFAULT;
		break;

	/* invalid request*/
	default:
		return -ENOTTY;
	}
	return 0;
}

/* --------------------------------------------------------------------- */

/*
 * file ops
 */
static struct file_operations notty_fops = {
	owner:		THIS_MODULE,
	read:		notty_read,
	write:		notty_write,
	//poll:		notty_poll,
	unlocked_ioctl:	notty_ioctl,
	open:		notty_open,
	release:	notty_release,
	fasync:		notty_fasync,
};

/* --------------------------------------------------------------------- */

static int notty_probe(struct platform_device *pdev)
{
	struct notty_driverinfo *info;
	struct resource *res[2];
	struct vmport_core_portinfo *port;
	int i;

	res[0] = platform_get_resource(pdev, IORESOURCE_BUS, 0);
	res[1] = platform_get_resource(pdev, IORESOURCE_BUS, 1);
	if (!res[0] || !res[1]) {
		dev_warn(&pdev->dev, "Resource not set\n");
		return -EINVAL;
	}

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	/* OK, both ports are fine */
	init_waitqueue_head(&info->read_wait);
	init_waitqueue_head(&info->write_wait);

	mutex_init(&info->lock);

	for (i = 0; i < vmport_core_count; i++)
	{
		port = &vmport_core_portinfo[i];
		if (strcmp(port->portname, res[0]->name) == 0 ||
			strcmp(port->portname, res[1]->name) == 0) {

			if (port->direction == VMPORT_CORE_DIR_RECV)
				info->iport = port;
			else if (port->direction == VMPORT_CORE_DIR_SEND)
				info->oport = port;
		}
	}

	platform_set_drvdata(pdev, info);

	device_create(vm_class, NULL, MKDEV(NOTTY_MAJOR, pdev->id), NULL, 
				  P4VM_DEV_NAME "%u", pdev->id);

	return 0;
}

static int notty_remove(struct platform_device *pdev)
{
	struct notty_driverinfo *info = platform_get_drvdata(pdev);

	mutex_destroy(&info->lock);
	kfree(info);
	device_destroy(vm_class, MKDEV(NOTTY_MAJOR, pdev->id));
	return 0;
}

static struct platform_driver p4vm_driver = {
	.driver		= {
		.name	= P4VM_DEV_NAME,
		.owner	= THIS_MODULE,
	},
	.probe  = notty_probe,
	.remove = notty_remove,
};


/*
 * int notty_init(void)
 *
 * Initialize and register driver
 */
static int __init notty_init(void)
{
	int ret;

	/* register driver */
	ret = register_chrdev(NOTTY_MAJOR, P4VM_DEV_NAME, &notty_fops);
	if (ret != 0) {
		printk(KERN_ERR P4VM_DEV_NAME ": could register major number %d, error %d\n", NOTTY_MAJOR, ret);
		return -ENODEV;
	}

	vm_class = class_create(THIS_MODULE, P4VM_DEV_NAME);

	platform_driver_register(&p4vm_driver);

	ret = p4vm_driver_init();
	if (ret) return ret;

	printk(KERN_INFO P4VM_DEV_NAME ": driver registered as major %d\n", NOTTY_MAJOR);

	return 0;
}

/*
 * void notty_exit(void)
 *
 * Shutdown driver
 */
static void __exit notty_exit(void)
{
	p4vm_driver_exit();

	unregister_chrdev(NOTTY_MAJOR, P4VM_DEV_NAME);
	platform_driver_unregister(&p4vm_driver);
	class_destroy(vm_class);
}

module_init(notty_init);
module_exit(notty_exit);

MODULE_AUTHOR("Alex Zuepke <azu@sysgo.de>");
MODULE_DESCRIPTION("PikeOS Communication Ports (PCP) NOTTY linked driver");
MODULE_LICENSE("GPL");

