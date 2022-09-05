/* Copyright (c) 2016 OpenSynergy GmbH */
/*
 *  P4VM port "char" driver
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 *          David Engraf <den@sysgo.com>
 * 
 *  Copyright (C) 2013 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/ioctl.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#include "vmport_core.h"
#include "vmdrv_ioctl.h"

#define CARRIE_MAJOR	199	/* major number */
#define CARRIE_DEV_NAME	"vmport_char"

struct carrie_driverinfo {
	/* filesystem interface */
	struct fasync_struct *fasync;
	wait_queue_head_t wait;

	unsigned int minor;

	/* port interface */
	struct vmport_core_portinfo *port;

	struct mutex lock;

	struct platform_device *pdev;
};

static struct carrie_driverinfo *carrie_driverinfo;
static struct class *p4vm_class;

/* device specific attributes */
static ssize_t portname_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct carrie_driverinfo *info = (struct carrie_driverinfo*)dev_get_drvdata(dev);
	return sprintf(buf, "%s", info->port->portname);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)
static DEVICE_ATTR_RO(portname);
#else
static DEVICE_ATTR(portname, S_IRUGO, portname_show, NULL);
#endif

static ssize_t framesize_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct carrie_driverinfo *info = (struct carrie_driverinfo*)dev_get_drvdata(dev);
	return sprintf(buf, "%d", info->port->framesize);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)
static DEVICE_ATTR_RO(framesize);
#else
static DEVICE_ATTR(framesize, S_IRUGO, framesize_show, NULL);
#endif

static struct attribute *p4vm_attrs[] = {
	&dev_attr_portname.attr,
	&dev_attr_framesize.attr,
	NULL,
};
ATTRIBUTE_GROUPS(p4vm);

/*
 * IRQ callback (called by vmport_core IRQs)
 */
static void carrie_int(void *data)
{
	struct carrie_driverinfo *info = (struct carrie_driverinfo *)data;

	/* notify fasync */
	if (info->fasync) {
		if (info->port->direction == VMPORT_CORE_DIR_RECV)
			kill_fasync(&info->fasync, SIGIO, POLL_IN);
		else
			kill_fasync(&info->fasync, SIGIO, POLL_OUT);
	}

	/* wakeup waiting ones */
	wake_up_interruptible(&info->wait);
}

/*
 * ssize_t carrie_read(struct file *file, char *buffer, size_t count, loff_t *ppos)
 *
 * read ...
 */
static ssize_t carrie_read(struct file *file, char __user *buffer, size_t count,
			   loff_t * ppos)
{
	struct carrie_driverinfo *info =
	    (struct carrie_driverinfo *)file->private_data;

	if (info->port->direction != VMPORT_CORE_DIR_RECV)
		return -EINVAL;

	return vmport_core_file_read(info->port, &info->lock, &info->wait, file,
				     buffer, count);
}

/*
 * ssize_t carrie_write(struct file *file, const char *buffer, size_t count, loff_t *ppos)
 *
 * write ...
 */
static ssize_t carrie_write(struct file *file, const char __user *buffer, size_t count,
			    loff_t * ppos)
{
	struct carrie_driverinfo *info =
	    (struct carrie_driverinfo *)file->private_data;

	if (info->port->direction != VMPORT_CORE_DIR_SEND)
		return -EINVAL;

	return vmport_core_file_write(info->port, &info->lock, &info->wait,
				      file, buffer, count);
}

/*
 * static int carrie_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
 *
 * ioctl stuff
 */
static long carrie_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct carrie_driverinfo *info =
	    (struct carrie_driverinfo *)file->private_data;
	unsigned int nb_messages, max_messages;
	int err;

	switch (cmd) {
		/* get framesize */
	case VMDRV_GET_PORT_SIZE:
		if (copy_to_user
		    ((void *)arg, &info->port->framesize, sizeof(int)))
			return -EFAULT;
		break;

		/* get direction */
	case VMDRV_GET_PORT_DIRECTION:
		if (copy_to_user
		    ((void *)arg, &info->port->direction, sizeof(int)))
			return -EFAULT;
		break;

		/* get remaining messages in port */
	case VMDRV_GET_PORT_REMAIN:
		err = vmport_core_stat_num_msg(info->port, &nb_messages, &max_messages);
		if (err != 0)
			return err;
		if (copy_to_user((void *)arg, &nb_messages, sizeof(nb_messages)))
			return -EFAULT;
		break;

		/* invalid request */
	default:
		return -ENOTTY;
	}
	return 0;
}

/*
 * unsigned int carrie_poll(struct file *file, poll_table *wait)
 *
 * "state_ready" is shred between 2 pikeos threads therefore it should be
 * protected. Nevertheless, the "select" design makes sure that they'll
 * always run alternatively: vmport_select_thread() will trigger carrie_poll()
 * while disabling itself. carrie_poll() will re-enable it.
 */
static unsigned int carrie_poll(struct file *file, poll_table * wait)
{
	struct carrie_driverinfo *info =
	    (struct carrie_driverinfo *)file->private_data;
	unsigned long flags;
	unsigned int ret = 0;

	poll_wait(file, &info->wait, wait);

	spin_lock_irqsave(&info->port->lock, flags);

	if (info->port->direction == VMPORT_CORE_DIR_RECV) {
		/* read state check: data avail? */
		if (info->port->state_ready) {
			info->port->state_ready = 0;
			ret |= POLLIN | POLLRDNORM;
		}
	} else {
		/* write state check: send buffer empty? */
		if (info->port->state_ready) {
			info->port->state_ready = 0;
			ret |= POLLOUT | POLLWRNORM;
		}
	}

	spin_unlock_irqrestore(&info->port->lock, flags);

	if (!ret)
		vmport_core_poll_start(info->port);

	return ret;
}

/*
 * int carrie_fasync(int fd, struct file *file, int mode)
 *
 * common fasync stuff ... don't change
 */
static int carrie_fasync(int fd, struct file *file, int mode)
{
	struct carrie_driverinfo *info =
	    (struct carrie_driverinfo *)file->private_data;
	int retval;

	retval = fasync_helper(fd, file, mode, &info->fasync);
	if (retval < 0)
		return retval;
	return 0;
}

/*
 * int carrie_open(struct inode *inode, struct file *file)
 *
 * open file request: clear buffers, arm IRQ thread, ...
 */
static int carrie_open(struct inode *inode, struct file *file)
{
	struct carrie_driverinfo *info;
	unsigned int minor = iminor(inode);
	int err;

	if (minor >= vmport_core_count)
		return -ENODEV;

	info = &carrie_driverinfo[minor];

	mutex_lock(&info->lock);

	err = vmport_core_init_port(info->port, carrie_int, info,
				    VMPORT_CORE_TYPE_FILE, NULL,
				    info->port->direction ==
				    VMPORT_CORE_DIR_RECV ? 1 : 0);
	if (err)
		goto out_release;

	file->private_data = info;

      out_release:
	mutex_unlock(&info->lock);
	return err;
}

/*
 * int carrie_close(struct inode *inode, struct file *file)
 *
 * close file request: disarm IRQ thread (if possible)
 */
static int carrie_close(struct inode *inode, struct file *file)
{
	struct carrie_driverinfo *info =
	    (struct carrie_driverinfo *)file->private_data;

	mutex_lock(&info->lock);
	vmport_core_exit_port(info->port);
	mutex_unlock(&info->lock);

	vmport_core_poll_stop(info->port);
	return 0;
}

/*
 * file ops
 */
static struct file_operations carrie_fops = {
      owner:THIS_MODULE,
      read:carrie_read,
      write:carrie_write,
      poll:carrie_poll,
      unlocked_ioctl:carrie_ioctl,
      open:carrie_open,
      release:carrie_close,
      fasync:carrie_fasync,
};

static int carrie_probe(struct platform_device *pdev)
{
	struct carrie_driverinfo *info;

	info = &carrie_driverinfo[pdev->id];

	init_waitqueue_head(&info->wait);
	mutex_init(&info->lock);
	info->minor = pdev->id;
	info->port = &vmport_core_portinfo[pdev->id];	/* assoc with port */

	device_create(p4vm_class, NULL, MKDEV(CARRIE_MAJOR, pdev->id), info,
		      CARRIE_DEV_NAME "%u", pdev->id);

	return 0;
}

static int carrie_remove(struct platform_device *pdev)
{
	struct carrie_driverinfo *info;

	info = &carrie_driverinfo[pdev->id];

	mutex_destroy(&info->lock);
	device_destroy(p4vm_class, MKDEV(CARRIE_MAJOR, pdev->id));
	return 0;
}

static struct platform_driver carrie_driver = {
	.driver = {
		   .name = CARRIE_DEV_NAME,
		   .owner = THIS_MODULE,
		   },
	.remove = carrie_remove,
};

/*
 * int carrie_init(void)
 *
 * Initialize and register driver
 */
static int __init carrie_init(void)
{
	int i, ret;

	if (!vmport_core_count) {
		printk(KERN_ERR CARRIE_DEV_NAME
		       ": no queuing ports in the vmit configured\n");
		return -ENODEV;
	}

	/* register driver */
	ret = register_chrdev(CARRIE_MAJOR, CARRIE_DEV_NAME, &carrie_fops);
	if (ret != 0) {
		printk(KERN_ERR CARRIE_DEV_NAME
		       ": could not register major number, error %d\n", ret);
		return ret;
	}

	p4vm_class = class_create(THIS_MODULE, CARRIE_DEV_NAME);
	p4vm_class->dev_groups = p4vm_groups;

	carrie_driverinfo =
	    kzalloc(vmport_core_count * sizeof(struct carrie_driverinfo),
		    GFP_KERNEL);
	if (!carrie_driverinfo)
		return -ENOMEM;

	for (i = 0; i < vmport_core_count; i++) {
		carrie_driverinfo[i].pdev =
		    platform_device_register_simple(CARRIE_DEV_NAME, i, NULL,
						    0);
		if (IS_ERR(carrie_driverinfo[i].pdev)) {
			ret = PTR_ERR(carrie_driverinfo[i].pdev);
			break;
		}
	}

	if (!ret) {
		/* not hotpluggable, use simple probe interface */
		ret = platform_driver_probe(&carrie_driver, carrie_probe);
	}

	printk(KERN_INFO CARRIE_DEV_NAME ": driver registered as major %d\n",
	       CARRIE_MAJOR);
	return ret;
}

/*
 * void carrie_exit(void)
 *
 * Shutdown driver
 */
static void __exit carrie_exit(void)
{
	int i;

	unregister_chrdev(CARRIE_MAJOR, CARRIE_DEV_NAME);

	for (i = 0; i < vmport_core_count; i++)
		platform_device_unregister(carrie_driverinfo[i].pdev);

	platform_driver_unregister(&carrie_driver);

	class_destroy(p4vm_class);

	kfree(carrie_driverinfo);
}

module_init(carrie_init);
module_exit(carrie_exit);

MODULE_AUTHOR("Alex Zuepke <azu@sysgo.de>");
MODULE_DESCRIPTION("PikeOS Communication Ports (PCP) raw char driver");
MODULE_LICENSE("GPL");
