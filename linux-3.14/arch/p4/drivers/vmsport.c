/* Copyright (c) 2016 OpenSynergy GmbH */
/*
 *  P4VM sampling port driver
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
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
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/kernthreads.h>
#include <asm/vmapi_calls.h>
#include "vmsport_ioctl.h"

#define SPORTY_MAJOR	195	/* major number */
#define SPORTY_DEV_NAME		"vmsport"

struct sporty_info {
	/* linux specific */
	unsigned int minor;
	spinlock_t lock;
	unsigned int count;

	/* port specific */
	int istx;
	char *name;
	int framesize;
	unsigned long long refresh;	/* refresh rate in nanoseconds */
	unsigned char *buf;	/* buffer in kernel space to hold messages */
	/* validity flag is ignored here */
};

/*
 * global variables
 */
unsigned int sporty_count;	/* number of sampling ports in system */
struct sporty_info *sporty_info;	/* port specific information */
static struct class *p4vm_class;

/* --------------------------------------------------------------------- */

/* device specific attributes */
static ssize_t portname_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct sporty_info *info = (struct sporty_info*)dev_get_drvdata(dev);
	return sprintf(buf, "%s", info->name);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)
static DEVICE_ATTR_RO(portname);
#else
static DEVICE_ATTR(portname, S_IRUGO, portname_show, NULL);
#endif

static ssize_t framesize_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct sporty_info *info = (struct sporty_info*)dev_get_drvdata(dev);
	return sprintf(buf, "%d", info->framesize);
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
 * Proc filesystem related code
 */

/*
 * prints some info
 */
static ssize_t config_show(struct device_driver *driver, char *buf)
{
	struct sporty_info *info;
	unsigned long flags;
	int i;
	char *p;

	p = buf;

	p += sprintf(p,
		     "# <id> <portname> <direction> <portsize> <use_counter> <refresh>\n");

	for (i = 0; i < sporty_count; i++) {
		info = &sporty_info[i];

		spin_lock_irqsave(&info->lock, flags);

		/* states as strings  */
		p += sprintf(p, "%d %s ", i, info->name);

		if (info->istx)
			p += sprintf(p, "SOURCE");
		else
			p += sprintf(p, "DESTINATION");

		p += sprintf(p, " %d %d %llu\n", info->framesize, info->count,
			     info->refresh);

		spin_unlock_irqrestore(&info->lock, flags);
	}

	return strlen(buf);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)
static DRIVER_ATTR_RO(config);
#else
static DRIVER_ATTR(config, S_IRUGO, config_show, NULL);
#endif

/* --------------------------------------------------------------------- */

static int sporty_read(struct sporty_info *info, _sport_ioc_t * p)
{
	unsigned int v;
	int size;

	if (info->istx)
		return -EINVAL;

	if (p->sport_op.read.buf_size < info->framesize)
		return -EMSGSIZE;

	size = vmapi_sport_receive(info->minor, info->buf, info->framesize, &v);
	p->sport_op.read.read_size = size;
	p->sport_op.read.validity = v;

	if (size < 0)
		return size;
	else if (size == 0)	/* VM_E_EMPTY case */
		return -ENODATA;

	if (copy_to_user(p->sport_op.read.buf, info->buf, size))
		return -EFAULT;

	return 0;
}

static int sporty_write(struct sporty_info *info, _sport_ioc_t * p)
{
	int size;

	if (!info->istx)
		return -EINVAL;

	if (p->sport_op.write.buf_size > info->framesize)
		return -EMSGSIZE;

	if (copy_from_user
	    (info->buf, p->sport_op.write.buf, p->sport_op.write.buf_size))
		return -EFAULT;

	size = vmapi_sport_send(info->minor, info->buf,
			     p->sport_op.write.buf_size);
	return size;
}

static int sporty_pstat(struct sporty_info *info, _sport_ioc_t * p)
{
	int istx;
	int msg_size;
	char *portname;
	unsigned long long refresh;
	unsigned int validity;
	int err;

	err =
	    vmapi_sport_stat(info->minor, &istx, &msg_size, &portname, &refresh,
			     &validity);
	if (err < 0)
		return -EIO;

	/* fill in stat struct */
	if (istx)
		p->sport_op.pstat.direction = SPORT_SOURCE;
	else
		p->sport_op.pstat.direction = SPORT_DESTINATION;

	p->sport_op.pstat.msg_size = msg_size;

	p->sport_op.pstat.refresh = ns_to_timespec(refresh);
	p->sport_op.pstat.validity = validity;

	return 0;
}

/* --------------------------------------------------------------------- */

/*
 * static int sporty_ioctl()
 *
 * ioctl stuff
 */
static long sporty_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct sporty_info *info = file->private_data;
	_sport_ioc_t rq;
	int err;

	switch (cmd) {
	case SPORT_READ:
		if (copy_from_user(&rq, (void *)arg, sizeof(rq)))
			return -EFAULT;
		err = sporty_read(info, &rq);
		if (copy_to_user((void *)arg, &rq, sizeof(rq)))
			return -EFAULT;
		return err;

	case SPORT_WRITE:
		if (copy_from_user(&rq, (void *)arg, sizeof(rq)))
			return -EFAULT;
		return sporty_write(info, &rq);

	case SPORT_PSTAT:
		err = sporty_pstat(info, &rq);
		if (copy_to_user((void *)arg, &rq, sizeof(rq)))
			return -EFAULT;
		return err;
	}

	/* invalid request */
	return -ENOTTY;
}

/* --------------------------------------------------------------------- */

/*
 * int sporty_open()
 *
 * open file request
 */
static int sporty_open(struct inode *inode, struct file *file)
{
	struct sporty_info *info;
	unsigned int minor = iminor(inode);
	unsigned long flags;
	int err;

	if (minor >= sporty_count)
		return -ENODEV;

	if (!vmapi_sport_check_state(minor))
		return -ENXIO;

	info = &sporty_info[minor];

	spin_lock_irqsave(&info->lock, flags);

	err = -EBUSY;
	if (info->count != 0)
		goto out;

	file->private_data = info;
	info->count++;
	err = 0;

      out:
	spin_unlock_irqrestore(&info->lock, flags);

	return err;
}

/* --------------------------------------------------------------------- */

/*
 * int sporty_close()
 *
 * close file request
 */
static int sporty_close(struct inode *inode, struct file *file)
{
	struct sporty_info *info = file->private_data;
	unsigned long flags;

	spin_lock_irqsave(&info->lock, flags);
	info->count--;
	spin_unlock_irqrestore(&info->lock, flags);

	return 0;
}

/* --------------------------------------------------------------------- */

/*
 * file ops
 */
static struct file_operations sporty_fops = {
      owner:THIS_MODULE,
      unlocked_ioctl:sporty_ioctl,
      open:sporty_open,
      release:sporty_close,
};

/* --------------------------------------------------------------------- */

/*
 * int sporty_free_resources(void)
 *
 * Free resources used by driver
 */
static void sporty_free_resources(void)
{
	struct sporty_info *info;
	int i;

	for (i = 0; i < sporty_count; i++) {
		info = &sporty_info[i];

		/* free associated memory */
		kfree(info->name);
		kfree(info->buf);
	}

	/* unregister device and procfs entries */
	unregister_chrdev(SPORTY_MAJOR, SPORTY_DEV_NAME);

	kfree(sporty_info);
	sporty_count = 0;
}

/* --------------------------------------------------------------------- */

/*
 * Module initialization
 */
static struct platform_driver p4vm_driver = {
	.driver		= {
		.name	= SPORTY_DEV_NAME,
		.owner	= THIS_MODULE,
	},
};

/*
 * Initialize and register driver
 */
static int __init sporty_init(void)
{
	struct sporty_info *info;
	unsigned int i;
	int ret;

	sporty_count = vmapi_sport_get_count();

	/* allocate */
	sporty_info =
	    kzalloc(sporty_count * sizeof(struct sporty_info), GFP_KERNEL);
	if (!sporty_info)
		return -ENOMEM;

	/* register driver */
	ret = register_chrdev(SPORTY_MAJOR, SPORTY_DEV_NAME, &sporty_fops);
	if (ret != 0) {
		printk(KERN_ERR SPORTY_DEV_NAME
		       ": could register major number, error %d\n", ret);
		sporty_free_resources();
		return -ENODEV;
	}

	p4vm_class = class_create(THIS_MODULE, SPORTY_DEV_NAME);
	p4vm_class->dev_groups = p4vm_groups;

	/* collecting port info, allocate some buffers, create device nodes */
	for (i = 0; i < sporty_count; i++) {
		char *name;
		int portnamesize;

		info = &sporty_info[i];

		if (vmapi_sport_stat
		    (i, &info->istx, &info->framesize, &name, &info->refresh,
		     NULL) < 0) {
			printk(KERN_ERR SPORTY_DEV_NAME
			       ": could not get port info for sport %d\n", i);
			sporty_free_resources();
			return -ENODEV;
		}

		printk(KERN_INFO SPORTY_DEV_NAME
		       ": found port %s, size %d, %s, refresh %llu ns\n", name,
		       info->framesize, info->istx ? "tx" : "rx",
		       info->refresh);

		/* check and allocate buffer */
		if (info->framesize <= 0) {
			printk(KERN_ERR SPORTY_DEV_NAME
			       ": unsupported framesize %d\n", info->framesize);
			sporty_free_resources();
			return -ENODEV;
		}

		/* alloc portname + buf */
		portnamesize = strlen(name) + 1;
		info->name = kmalloc(portnamesize, GFP_KERNEL);
		info->buf = kmalloc(info->framesize, GFP_KERNEL);
		if (!info->name || !info->buf) {
			printk(KERN_ERR SPORTY_DEV_NAME
			       ": memory allocation failed\n");
			sporty_free_resources();
			return -ENOMEM;
		}

		/* save information */
		spin_lock_init(&info->lock);
		info->minor = i;
		strncpy(info->name, name, portnamesize);

		device_create(p4vm_class, NULL, MKDEV(SPORTY_MAJOR, i), info,
			      SPORTY_DEV_NAME "%u", i);
	
		platform_device_register_simple(SPORTY_DEV_NAME, 0, NULL, 0);
	}

	platform_driver_register(&p4vm_driver);
	ret = driver_create_file(&p4vm_driver.driver, &driver_attr_config);

	if (!ret) {
		printk(KERN_INFO SPORTY_DEV_NAME ": driver registered as major %d\n",
		       SPORTY_MAJOR);
	}

	return ret;
}
/*
 * void sporty_exit(void)
 *
 * Shutdown driver
 */
static void __exit sporty_exit(void)
{
	unsigned int i;

	driver_remove_file(&p4vm_driver.driver, &driver_attr_config);
	platform_driver_unregister(&p4vm_driver);

	for (i = 0; i < sporty_count; i++) {
		device_destroy(p4vm_class, MKDEV(SPORTY_MAJOR, i));
	}
	class_destroy(p4vm_class);
	sporty_free_resources();
}

module_init(sporty_init);
module_exit(sporty_exit);

MODULE_AUTHOR("Alex Zuepke <azu@sysgo.de>");
MODULE_DESCRIPTION("PikeOS Sampling Ports driver");
MODULE_LICENSE("GPL");
