/* Copyright (c) 2016 OpenSynergy GmbH */
/*
 *  P4VM file provider based "SHM" driver
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
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/ioctl.h>
#include <linux/vmalloc.h>
#include <linux/platform_device.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#include <asm/tlbflush.h>
#include <asm/vmapi_calls.h>
#include <asm/kernthreads.h>
#include "vmdrv_ioctl.h"


/*
 * NOTE: sherry (this driver's name) is short form of SHaRed memoRY :-)
 *
 * How the driver works:
 *
 * On the first open, the driver does the following steps:
 *   1) opens the SHM file (must have "shm:" prefix)
 *   2) stats the file
 *   3) mmaps all the content
 *   4) closes the file, the mapping remains
 * The thread is freed then and all read/write/etc stuff then works on
 * the mmapped image.
 *
 * There is no async thread involved!
 *
 * This driver REQUIRES that the SHM implementation does NOT block!
 */

#ifdef DEBUG
#define DBG(x) x
#else
#define DBG(x)
#endif



#define SHERRY_MAJOR	197	/* major number */
#define P4VM_DEV_NAME	"vmfile_shm"


static struct class *p4vm_class;

struct sherry_driverinfo
{
	/* internal states */
	int count;	/* if >0, this device is used */

	const char *filename;
	int size;
	int is_write;

	/* mmaped spaces */
	struct vm_struct *area;
	unsigned long base;
};

/* device specific attributes */
static ssize_t filename_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	const char* filename;
	struct sherry_driverinfo *info = (struct sherry_driverinfo*)(dev_get_drvdata(dev));

	/* Skip the PikeOS prefix ("shm:/") */
	filename = info->filename + 5;

	return sprintf(buf, "%s", filename);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)
static DEVICE_ATTR_RO(filename);
#else
static DEVICE_ATTR(filename, S_IRUGO, filename_show, NULL);
#endif

static ssize_t size_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct sherry_driverinfo *info = (struct sherry_driverinfo*)(dev_get_drvdata(dev));
	return sprintf(buf, "%d", info->size);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)
static DEVICE_ATTR_RO(size);
#else
static DEVICE_ATTR(size, S_IRUGO, size_show, NULL);
#endif

static struct attribute *p4vm_attrs[] = {
	&dev_attr_filename.attr,
	&dev_attr_size.attr,
	NULL,
};
ATTRIBUTE_GROUPS(p4vm);

static struct platform_driver p4vm_driver;

#define P4VM_TYPE_FP
#define P4VM_SHOW_ADDITIONAL_INFO "# <minor> <filename> <size> <use_counter>"
#define P4VM_CHECK_DEVICE
#include "p4vm_gen_drv_interface.c"

/*
 * /proc/vmfileshm output:
 *
 * 7 prov:/devname  4096 0
 * ^      ^         ^    ^
 * |      |         |    +---- Use counter
 * |      |         +-------- Size of SHM in bytes
 * |      +----------------- Name of file provider
 * +----------------------- vmfileshm<x> minor number
 *
 */
static char *p4vm_proc_additional_info(struct p4vm_list *list, char *p)
{
	struct sherry_driverinfo *priv = platform_get_drvdata(list->pdev);
	if (priv) {
		p += sprintf(p, "%d %s %d %d\n", list->minor, list->parm1, 
					 priv->size, priv->count);
	}
	else {
		/* device not yet registered (probe not called) */
		p += sprintf(p, "%d %s not initialized\n", list->minor, list->parm1);
	}

	return p;
}

/* --------------------------------------------------------------------- */

/*
 * Add and remove logic
 */

static int p4vm_check_device(int state, unsigned int minor, const char *parm1,
						   const char *parm2)
{
	if (state == P4VM_STATE_DEVICE_ADD) {
		/* only SHMs are supported by this driver! */
		if (strncmp("shm:", parm1, 4) != 0) {
			printk(KERN_ERR P4VM_DEV_NAME ": filename must start with <shm:>\n");
			return -ENXIO;
		}
	}

	return 0;
}


/* --------------------------------------------------------------------- */

static ssize_t sherry_read(struct file *file, char *buffer, size_t count, loff_t *ppos)
{
	struct sherry_driverinfo *info = file->private_data;
	ssize_t start = *ppos;

	DBG(printk("read\n"));

	/* beyond end of file? */
	if (start >= info->size)
		return 0;

	/* don't exceed beyond the end */
	if (start + count > info->size)
		count = info->size - start;

	/* copy to the user */
	if (copy_to_user(buffer, (void*)(info->base + start), count))
		return -EFAULT;

	/* update file position */
	*ppos += count;

	return count;
}

static ssize_t sherry_write(struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
	struct sherry_driverinfo *info = file->private_data;
	ssize_t start = *ppos;

	DBG(printk("write\n"));

	/* allowed to write? */
	if (!info->is_write)
		return -EACCES;

	/* beyond end of file? */
	if (start >= info->size)
		return -EFBIG;

	/* don't exceed beyond the end */
	if (start + count > info->size)
		count = info->size - start;

	/* copy from the user */
	if (copy_from_user((void*)(info->base + start), buffer, count))
		return -EFAULT;

	/* update file position */
	*ppos += count;

	return count;
}

static loff_t sherry_lseek(struct file * file, loff_t offset, int orig)
{
	struct sherry_driverinfo *info = file->private_data;
	loff_t newfpos;

	DBG(printk("lseek\n"));

	newfpos = file->f_pos;
	switch (orig) {
	case 0:	/* SEEK_SET */
		newfpos = offset;
		break;
	case 1:	/* SEEK_CUR */
		newfpos += offset;
		break;
	case 2:	/* SEEK_END */
		newfpos = info->size + offset;
		break;
	default:
		return -EINVAL;
	}
	
	/* this should also catch integer overflow (SEEK_END & offset==MAXSINT) */
	if (newfpos < 0 || newfpos >= info->size)
		return -EINVAL;

	file->f_pos = newfpos;
	return newfpos;
}

static int sherry_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct sherry_driverinfo *info = file->private_data;
	int err;
	unsigned long size;
	unsigned long offs;
	int need_write;

	/* check sizes and offset */
	size = vma->vm_end - vma->vm_start;
	offs = vma->vm_pgoff;
	need_write = !!(pgprot_val(vma->vm_page_prot) & _PAGE_RW);

	DBG(printk("mmap: req. size 0x%08x, offs 0x%08x, need_write %d, base 0x%08x, rsize 0x%08x\n", size, offs, need_write, info->base, info->size));

	if ((size > info->size) || (size + offs > info->size)) {
		return -EAGAIN;
	}

	if (need_write && !info->is_write) {
		return -EACCES;
	}

	/* map to user */
	err = remap_pfn_range(vma, vma->vm_start,
			    (info->base + offs) >> PAGE_SHIFT,
			    vma->vm_end-vma->vm_start,
			    vma->vm_page_prot);

	return err;
}

/* --------------------------------------------------------------------- */

/*
 * static int sherry_ioctl(...)
 *
 * ioctl stuff
 */
static long sherry_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct sherry_driverinfo *info = file->private_data;

	switch (cmd)
	{
	case VMDRV_GET_FILE_SIZE:
		if (copy_to_user((void *)arg, &info->size, sizeof(int)))
			return -EFAULT;
		break;

	default:
		return -ENOTTY;
	}
	return 0;
}

/* --------------------------------------------------------------------- */

/*
 * int sherry_open(struct inode *inode, struct file *file)
 *
 * open file request:
 *  - open file, get size
 *  - mmap into kernel
 *  - close file again
 *  - done :-)
 */
static int sherry_open(struct inode *inode, struct file *file)
{
	struct sherry_driverinfo *info;
	unsigned int minor = iminor(inode);
	int err = 0;
	int handle;
	struct p4vm_list *list;
	unsigned int flags;
	vmapi_off_t size_o;

	info = NULL;
	mutex_lock(&p4vm_devlistlock);
	list_for_each_entry(list, &p4vm_devlist, list) {
		if (list->minor == minor) {
			info = platform_get_drvdata(list->pdev);
			break;
		}
	}
	mutex_unlock(&p4vm_devlistlock);

	if (!info) {
		DBG(printk("open1\n"));
		return -ENXIO;
	}

	if (info->count > 0) {
		/* device is already open and ready to use */
		DBG(printk("open2\n"));

		/* update port info */
		file->private_data = info;
		info->count++;

		return 0;
	}

	/* Converte UNIX flags to VMAPI flags */
	switch (file->f_flags & O_ACCMODE) {
		case O_WRONLY:
			flags = VMAPI_O_W;
			info->is_write = 1;
			break;
		case O_RDONLY:
			flags = VMAPI_O_R;
			info->is_write = 0;
			break;
		case O_RDWR:
			flags = VMAPI_O_W | VMAPI_O_R;
			info->is_write = 1;
			break;
		default:
			return -EINVAL;
	}

	flags = flags | VMAPI_O_MAP;

	/* open - the real job */
	DBG(printk("e-OPEN '%s'\n", info->filename));
	err = vmapi_file_open4(info->filename, flags, &size_o, NULL);
	if (err < 0) {
		DBG(printk("e-OPEN failed, error %d\n", err));
		return err;
	}
	info->size = size_o;

	handle = err;
	DBG(printk("e-filesize %d, is_write %d\n", info->size,
		   info->is_write));

	err = -ENOMEM;
	if (info->size > 0) {
		/* get some mmap space */
		info->area = get_vm_area(info->size, VM_IOREMAP);
		if (!info->area) {
			DBG(printk("unable to get virt memory space\n"));
			err = -ENOMEM;
			goto out_close;
		}

		/* request mapping */
		info->base = (unsigned long)info->area->addr;
		DBG(printk("base address: 0x%08x\n", info->base));
		/* protect memory against TLB flushes */
		vm_protect(info->base, info->size);
		err = vmapi_file_fmap(handle, 0, info->size, info->base, flags);
		if (err < 0) {
			DBG(printk("e-OPEN fmap failed, error %d\n", err));
			remove_vm_area(info->area->addr);
			goto out_close;
		}

		/* done! */
		file->private_data = info;
		info->count++;
		
		err = 0;	/* OK */
	}

out_close:
	DBG(printk("e-CLOSE h%d\n", handle));
	vmapi_file_close(handle);

	return err;
}

/* --------------------------------------------------------------------- */

/*
 * int sherry_release(struct inode *inode, struct file *file)
 *
 * close file request:
 *   unmap mapped file and
 */
static int sherry_release(struct inode *inode, struct file *file)
{
	struct sherry_driverinfo *info = file->private_data;

	DBG(printk("close\n"));

	info->count--;

	/* further cleanup? need to remove the mapping! */
	if (info->count == 0) {
		remove_vm_area(info->area->addr);
	}

	return 0;
}


/* --------------------------------------------------------------------- */

/*
 * file ops
 */
static struct file_operations sherry_fops = {
	owner:		THIS_MODULE,
	read:		sherry_read,
	write:		sherry_write,
	mmap:		sherry_mmap,
	llseek:		sherry_lseek,
	unlocked_ioctl:	sherry_ioctl,
	open:		sherry_open,
	release:	sherry_release,
};

static int sherry_probe(struct platform_device *pdev)
{
	struct sherry_driverinfo *info;
	struct resource *res;

	res = platform_get_resource(pdev, IORESOURCE_BUS, 0);
	if (!res) {
		dev_warn(&pdev->dev, "Resource not set\n");
		return -EINVAL;
	}

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	/* alloc space for filename */
	info->filename = res->name;

	device_create(p4vm_class, NULL, MKDEV(SHERRY_MAJOR, pdev->id), info,
			P4VM_DEV_NAME "%u", pdev->id);

	platform_set_drvdata(pdev, info);

	return 0;
}

static int sherry_remove(struct platform_device *pdev)
{
	struct sherry_driverinfo *info = platform_get_drvdata(pdev);

	kfree(info);

	device_destroy(p4vm_class, MKDEV(SHERRY_MAJOR, pdev->id));
	return 0;
}

static struct platform_driver p4vm_driver = {
	.driver		= {
		.name	= P4VM_DEV_NAME,
		.owner	= THIS_MODULE,
	},
	.probe  = sherry_probe,
	.remove = sherry_remove,
};

/*
 * int sherry_init(void)
 *
 * Initialize and register driver
 */
static int __init sherry_init(void)
{
	int ret;

	/* register driver */
	ret = register_chrdev(SHERRY_MAJOR, P4VM_DEV_NAME, &sherry_fops);
	if (ret != 0) {
		printk(KERN_ERR P4VM_DEV_NAME ": could register major number %d, error %d\n", SHERRY_MAJOR, ret);
		return -ENODEV;
	}

	p4vm_class = class_create(THIS_MODULE, P4VM_DEV_NAME);
	p4vm_class->dev_groups = p4vm_groups;

	platform_driver_register(&p4vm_driver);
	p4vm_driver_init();

	printk(KERN_INFO P4VM_DEV_NAME ": driver registered as major %d\n", SHERRY_MAJOR);

	return 0;
}

/*
 * void sherry_exit(void)
 *
 * Shutdown driver, remove proc and devfs related entries
 */
static void __exit sherry_exit(void)
{
	p4vm_driver_exit();

	unregister_chrdev(SHERRY_MAJOR, P4VM_DEV_NAME);
	class_destroy(p4vm_class);
	platform_driver_unregister(&p4vm_driver);
}

module_init(sherry_init);
module_exit(sherry_exit);

MODULE_AUTHOR("Alex Zuepke <azu@sysgo.de>");
MODULE_DESCRIPTION("PikeOS File Provider (client) SHM char driver");
MODULE_LICENSE("GPL");
