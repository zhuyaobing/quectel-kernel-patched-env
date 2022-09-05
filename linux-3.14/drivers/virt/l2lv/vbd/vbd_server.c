/*  vbd/vbd_server.c
 *
 *  Copyright (C) 2013-2014 OpenSynergy GmbH
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Virtual block device driver
 */

#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/poll.h>
#include <linux/proc_fs.h>
#include <linux/random.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/tty.h>
#include <linux/types.h>
#include <linux/vmalloc.h>

#include <linux/l2lv/l2lv_server_links.h>
#include <linux/l2lv/vbd_server.h>
#include "vbd_debug.h"
#include "vbd_link.h"

#define VBD_DEV_NAME  "vbd"

MODULE_LICENSE("GPL");

struct vbd_driverinfo {
	int		count;  /* if >0, this device is used (and cannot be opened again) */
	vbd_links_info	links_info;
	struct mutex	lock;
};

static int vbd_major = 0;

static struct class *vm_class;
static struct vbd_driverinfo g_info;

static long vbd_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	struct vbd_driverinfo *info = file->private_data;
	long					  ret = 0;

	while (!mutex_trylock(&info->lock)) {
		arch_mutex_cpu_relax();
	}
	switch (cmd) {
	case VBD_IOC_INIT:

		break;
	case VBD_IOC_SET_LINK:
		{
			_vbd_setlink_t set_link;
			if (copy_from_user(&set_link, (const void*)arg, sizeof(_vbd_setlink_t))) {
				ret = -EFAULT;
				goto out;
			}
			DBG("VBD_IOC_SET_LINK(%d)\n", set_link.link_id);
			if (vbd_set_link(&info->links_info, &set_link)) {
				ret = -EINVAL;
				goto out;
			}
		}
		break;
	case VBD_IOC_REMOVE_LINK:

		break;

	default:
		LOG("invalid ioctl command 0x%x\n", cmd);
		ret = -EINVAL;
	}

out:
	mutex_unlock(&info->lock);
	return ret;
}

static int vbd_open(struct inode *inode, struct file *file) {
	struct vbd_driverinfo *info = file->private_data = &g_info;
	int					   err = 0;

	// Allow only one instance for now...
	// TODO: discuss how to protect against userspace fork()
	while (!mutex_trylock(&info->lock)){
		arch_mutex_cpu_relax();
	}
	if (info->count > 0) {
		LOG("error: device already opened\n");
		err = -EBUSY;
		goto error;
	}
	++info->count;
	mutex_unlock(&info->lock);

	vbd_init_links(&info->links_info);
	return 0;

error:
	return err;
}

static int vbd_release(struct inode *inode, struct file *file) {
	struct vbd_driverinfo *info = file->private_data = &g_info;
	LOG("closing\n");
	vbd_destroy_links(&info->links_info);
	while (!mutex_trylock(&info->lock)){
		arch_mutex_cpu_relax();
	}
	--info->count;
	mutex_unlock(&info->lock);
	return 0;
}

static struct file_operations vbd_fops = {
	owner:          THIS_MODULE,
	unlocked_ioctl: vbd_ioctl,
	open:           vbd_open,
	release:        vbd_release,
};

static int vbd_probe(struct platform_device *pdev) {
	struct vbd_driverinfo *info = &g_info;
	struct device *dev;
	memset(info, 0, sizeof(*info));
	mutex_init(&info->lock);
	dev = device_create(vm_class, 0, MKDEV(vbd_major, pdev->id), 0, VBD_DEV_NAME);
	if (IS_ERR(dev)) {
		/* Not fatal */
		LOG("Unable to create device %s; errno = %ld\n", VBD_DEV_NAME, PTR_ERR(dev));
	}
	return 0;
}

static int vbd_remove(struct platform_device *pdev) {
	device_destroy(vm_class, MKDEV(vbd_major, pdev->id));
	return 0;
}

static struct platform_driver vbd_driver = {
	.driver	= {
		.name  = VBD_DEV_NAME,
		.owner = THIS_MODULE,
	},
	.probe  = vbd_probe,
	.remove = vbd_remove,
};

static struct platform_device *vbd_device;

static int __init vbd_init(void) {
	int ret = register_chrdev(vbd_major, VBD_DEV_NAME, &vbd_fops);
	if (ret > 0) {
		vbd_major = ret;
		ret = 0;
	} else {
		LOG("could register major number %d, error %d\n", vbd_major, ret);
		return -ENODEV;
	}
	vm_class = class_create(THIS_MODULE, VBD_DEV_NAME);
	if (!vm_class) {
		LOG("could not create class\n");
		ret = -ENOMEM;
		goto err_class_create;
	}
	ret = platform_driver_register(&vbd_driver);
	if (ret != 0) {
		LOG("could not register platform driver\n");
		ret = -ENOMEM;
		goto err_dev;
	}
	vbd_device = platform_device_alloc(VBD_DEV_NAME, 0);
	if (!vbd_device) {
		ret = -ENOMEM;
		LOG("could not allocate platform device\n");
		goto err_dev_alloc;
	}
	ret = platform_device_add(vbd_device);
	if (ret != 0) {
		ret = -ENOMEM;
		LOG("could not add platform device\n");
		goto err_dev_add;
	}
	/* Force the shdrv links get initialised: */
	(void) l2lv_server_get_links();
	printk(KERN_INFO VBD_DEV_NAME ": driver registered as major %d\n", vbd_major);
	return 0;

err_dev_add:
	platform_device_unregister(vbd_device);
err_dev_alloc:
	platform_driver_unregister(&vbd_driver);
err_dev:
	class_destroy(vm_class);
err_class_create:
	unregister_chrdev(vbd_major, VBD_DEV_NAME);
	return ret;
}

static void __exit vbd_exit(void) {
	platform_device_unregister(vbd_device);
	platform_driver_unregister(&vbd_driver);
	class_destroy(vm_class);
	unregister_chrdev(vbd_major, VBD_DEV_NAME);
}

module_init(vbd_init);
module_exit(vbd_exit);
