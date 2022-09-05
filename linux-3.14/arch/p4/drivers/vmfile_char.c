/* Copyright (c) 2016 OpenSynergy GmbH */
/*
 *  P4VM file provider based "char" driver
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 *          David Engraf <den@sysgo.com>
 * 
 *  Copyright (C) 2006-2012 SYSGO AG
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
#include <linux/proc_fs.h>
#include <linux/poll.h>
#include <linux/ioctl.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/completion.h>
#include <linux/vmalloc.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#include <asm/vmapi_calls.h>
#include <asm/kernthreads.h>
#include <asm/host_threads.h>
#include <asm/locking.h>
#include <asm/tlbflush.h>
#include "vmdrv_ioctl.h"

#ifdef DEBUG
#define DBG(x) x
#else
#define DBG(x)
#endif


#define VMFILE_MAJOR	198	/* major number */
#define P4VM_DEV_NAME		"vmfile_char"
#define MAX_FRAMESIZE 4096

static struct class *p4vm_class;

struct vmfile_driverinfo;

typedef struct file_data_s {
	/* vmapi file handle */
	int handle;
	/* file size */
	int size;
	/* fasync interface */
	struct fasync_struct *fasync;
	/* vmfile device info */
	struct vmfile_driverinfo *info;
	int is_write;
	unsigned int vmapi_flags;
} file_data_t;

struct vmfile_driverinfo
{
	/* internal states */
	int count;	/* if >0, this device is used */
	const char *filename;
	int framesize;

	/* async thread */
	unsigned int irq;

	/* async thread state machine */
	volatile int state_active;
	struct uk_uevent irq_sleep;
	enum {
		JOB_NONE = 0, /* nothing to do, sleep */
		JOB_OPEN,     /* open the file */
		JOB_RELEASE,  /* release the file */
		JOB_READ,     /* read request */
		JOB_WRITE,    /* write request */
		JOB_LSEEK,    /* lseek request */
		JOB_EVMASK,   /* set event mask */
		JOB_EVWAIT,   /* wait for event */
		JOB_IOCTL,
		JOB_MMAP,
	} job;
	struct completion job_complete;
	spinlock_t job_lock;
	/* file handle for this i/o job */
	file_data_t *job_owner;

	/* read and write buffer */
	int error;
	int inbuf;
	unsigned char *buf;

	/* leek */
	unsigned long lseek_offset;
	int lseek_orig;

	/* event mask */
	unsigned long eventmask;

	/* event wait*/
	unsigned int preclear;
	unsigned int postclear;

	/* ioctl */
	unsigned int ioctl_cmd;
	void* ioctl_data;
	int ioctl_insize;
	int ioctl_outsize;

	/* mmaped spaces */
	struct vm_struct *area;
	unsigned long base;
};

typedef struct p4_vmfile_ioctl_s {
	uint32_t cmd;
	uint32_t insize;
	uint32_t outsize;
	uint8_t data[140-3*sizeof(uint32_t)];
} p4_vmfile_ioctl_t;

static struct platform_driver p4vm_driver;

/* use generic PikeOS driver interface */
#define P4VM_TYPE_FP
#include "p4vm_gen_drv_interface.c"

/* --------------------------------------------------------------------- */

static int vmfile_do_open(struct vmfile_driverinfo *info)
{
	int handle;
	vmapi_off_t size;
	file_data_t *file_data = info->job_owner;

	handle = vmapi_file_open4(info->filename, file_data->vmapi_flags, &size, NULL);
	if (handle >= 0) {
		DBG(printk("e-OPEN OK, filesize %d\n", size));
		/* open succeeded */
		/* romfiles etc don't have a size, so fake one */
		if (size == 0)
			size = MAX_FRAMESIZE;
		file_data->handle = handle;
		file_data->size = size;

		/*  limit the framesize to 1 page */
		if (size > MAX_FRAMESIZE)
			size = MAX_FRAMESIZE;

		info->framesize = size;

		return 0;
	}

	DBG(printk("e-OPEN error %d\n", handle));
	return handle;
}


/* --------------------------------------------------------------------- */

/*
 * async thread state machine
 *
 */


/*
 * void vmfile_thread(unsigned int irq, void *dev)
 *
 * is irq dispatcher thread and runs in P4 thread context
 *
 * NOTE: do not call any linux functions, as
 *       this code is not executed in linux context!
 */
static void vmfile_thread(unsigned int irq, void *dev)
{
	struct vmfile_driverinfo *info = dev;
	file_data_t *file_data;
	int handle;
	int err;

	while (1) {
		while (!info->state_active) {
			/* sleep until woken */
			DBG(printk("e-SLEEP active:%i\n", info->state_active));
			sleep_in_irq_thread(&info->irq_sleep);
			DBG(printk("e-AWAKE active:%i\n", info->state_active));
		}

		file_data = info->job_owner;
		handle = file_data->handle;
		err = 0;
		switch (info->job) {
		case JOB_OPEN:
			/* open the file */
			DBG(printk("e-OPEN %s\n", info->filename));
			err = vmfile_do_open(info);
			break;
		case JOB_READ:
			/* receive msg */
			DBG(printk("e-READ h%d b%p sz%d\n", handle, info->buf, info->inbuf));
			err = vmapi_file_read(handle, info->buf, info->inbuf);
			break;
		case JOB_WRITE:
			/* send msg */
			DBG(printk("e-WRITE h%d b%p sz%d\n", handle, info->buf, info->inbuf));
			err = vmapi_file_write(handle, info->buf, info->inbuf);
			break;
		case JOB_LSEEK:
			DBG(printk("e-LSEEK h%d pos0x%08lx of%d\n", handle, info->lseek_offset, info->lseek_orig));

			err = vmapi_file_lseek(handle, info->lseek_offset, info->lseek_orig);
			if (err >= 0)
				info->lseek_offset = err;
			break;
		case JOB_EVMASK:
			DBG(printk("e-EVMASK h%d msk0x%08lx\n", handle, info->eventmask));

			err = uk_thread_event_mask(info->eventmask);
			if (err > 0)
				err = -1;
			break;
		case JOB_EVWAIT:
			DBG(printk("e-EVWAIT h%d pre%d post%d\n", handle, info->preclear, info->postclear));

			// FIXME: remove info->preclear, info->postclear
			err = uk_thread_event_wait();
			if (err > 0)
				err = -1;
			break;
		case JOB_IOCTL:
			DBG(printk("e-IOCTL h%d cmd:%x data:%p insize:%i outsize:%i\n",
					handle, info->ioctl_cmd, info->ioctl_data, info->ioctl_insize, info->ioctl_outsize));
			err = vmapi_file_ioctl(handle, info->ioctl_cmd, info->ioctl_data, info->ioctl_insize, info->ioctl_outsize);
			break;
		default:
			err = 0;
			break;
		}


		DBG(if (err < 0)
			printk("e-ERR=%d\n", err);
		else
			printk("e-OK=%d\n", err);
		)

		if (err < 0) {
			info->error = err;
			info->inbuf = 0;
		} else {
			info->error = 0;
			info->inbuf = err;
		}

		/* disarm the thread before entering ISR */
		info->state_active = 0;

		/* we got an interrupt, now call the handler */
		do_IRQ(irq);
	}
}

/*
 * void vmfile_irq(int irq, void *dev, struct pt_regs *regs)
 *
 * called by do_IRQ when buffer received or sent
 */
static irqreturn_t vmfile_irq(int irq, void *dev)
{
	struct vmfile_driverinfo *info = dev;
	file_data_t *file_data = info->job_owner;

	DBG(printk("e-IRQ (job=%u)\n", info->job));

	spin_lock(&info->job_lock);

	/* wakeup and signal readers */

	/* notify fasync */
	if (file_data->fasync) {
		if (info->job == JOB_READ)
			kill_fasync(&file_data->fasync, SIGIO, POLL_IN);
		if (info->job == JOB_WRITE)
			kill_fasync(&file_data->fasync, SIGIO, POLL_OUT);
	}

	/* job done! */
	info->job_owner = NULL;
	info->job = JOB_NONE;

	/* wakeup waiting ones */
	complete(&info->job_complete);

	spin_unlock(&info->job_lock);

	return IRQ_HANDLED;
}

/* --------------------------------------------------------------------- */

/*
 * ssize_t vmfile_read(struct file *file, char *buffer, size_t count, loff_t *ppos)
 *
 * read ...
 */
static ssize_t vmfile_read(struct file *file, char *buffer, size_t count, loff_t *ppos)
{
	file_data_t *file_data = file->private_data;
	struct vmfile_driverinfo *info = file_data->info;
	unsigned long flags;
	int err;

	if (count == 0)
		return 0;

	/* non-blocking reading not supported by driver */
	if (file->f_flags & O_NONBLOCK)
		return -EAGAIN;

	/* check if there is another pending operation */
	err = -EIO;
	spin_lock_irqsave(&info->job_lock, flags);
	if (info->job != JOB_NONE)
		goto out_release_lock;

	/* check handle, device must be open! */
	if (file_data->handle < 0)
		goto out_release_lock;

	/* we must have a buffer */
	if (!info->buf)
		goto out_release_lock;

	/* limit read to framesize byte */
	if (count > info->framesize)
		count = info->framesize;

	/* clear completion state */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,13,0)
	reinit_completion(&info->job_complete);
#else
	INIT_COMPLETION(info->job_complete);
#endif

	/* start the job! */
	info->inbuf = count;
	info->job = JOB_READ;
	info->job_owner = file_data;
	info->state_active = 1;
	spin_unlock_irqrestore(&info->job_lock, flags);

	wake_irq_thread(&info->irq_sleep);

	/* wait for read request to complete */
	wait_for_completion(&info->job_complete);

	/* we expect data to be available ... and the RX thread is sleeping */
	if (info->error) {
		err = -EIO;
	} else {
		err = info->inbuf;
		DBG(printk("r-%d\n", err));
		if (copy_to_user(buffer, info->buf, info->inbuf)) {
			err = -EFAULT;
		}
	}
	return err;

out_release_lock:
	spin_unlock_irqrestore(&info->job_lock, flags);

	return err;
}

/* --------------------------------------------------------------------- */

/*
 * ssize_t vmfile_write()
 *
 * write to device
 */
static ssize_t vmfile_write(struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
	file_data_t *file_data = file->private_data;
	struct vmfile_driverinfo *info = file_data->info;
	unsigned long flags;
	int err;

	if (count == 0)
		return 0;

	/* non-blocking writing not supported by driver */
	if (file->f_flags & O_NONBLOCK)
		return -EAGAIN;

	/* check if there is another pending operation */
	err = -EIO;
	spin_lock_irqsave(&info->job_lock, flags);
	if (info->job != JOB_NONE)
		goto out_release_lock;

	/* check handle, device must be open! */
	if (file_data->handle < 0)
		goto out_release_lock;

	/* we must have a buffer */
	if (!info->buf)
		goto out_release_lock;

	/* limit write to framesize byte */
	if (count > info->framesize)
		count = info->framesize;

	/* fetch message */
	if (copy_from_user(info->buf, buffer, count)) {
		err = -EFAULT;
		goto out_release_lock;
	}

	/* clear completion state */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,13,0)
	reinit_completion(&info->job_complete);
#else
	INIT_COMPLETION(info->job_complete);
#endif

	/* start the job! */
	info->inbuf = count;
	info->job = JOB_WRITE;
	info->job_owner = file_data;
	info->state_active = 1;
	spin_unlock_irqrestore(&info->job_lock, flags);

	wake_irq_thread(&info->irq_sleep);

	/* wait for write request to complete */
	wait_for_completion(&info->job_complete);

	/* error: real errors or zero writes */
	if (info->error || info->inbuf == 0) {
		/* map untranslated VM_E_xxx to -EIO */
		if (info->error < -1000)
			err = -EIO;
		else
			err = info->error;
	} else {
		err = info->inbuf;
	}

	return err;

out_release_lock:
	spin_unlock_irqrestore(&info->job_lock, flags);
	return err;
}

/* --------------------------------------------------------------------- */

static loff_t vmfile_lseek(struct file * file, loff_t offset, int orig)
{
	file_data_t *file_data = file->private_data;
	struct vmfile_driverinfo *info = file_data->info;
	unsigned long flags;
	loff_t err;

	/* check "whence" */
	if (orig < 0 || orig > 2)
		return -EINVAL;

	err = -EAGAIN;
	/* check for other pending operations */
	spin_lock_irqsave(&info->job_lock, flags);
	if (info->job != JOB_NONE)
		goto out_release_lock;

	/* check handle, device must be open! */
	if (file_data->handle < 0)
		goto out_release_lock;

	info->lseek_offset = offset;
	info->lseek_orig = orig;

	/* clear completion state */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,13,0)
	reinit_completion(&info->job_complete);
#else
	INIT_COMPLETION(info->job_complete);
#endif

	/* start the job! */
	info->job = JOB_LSEEK;
	info->job_owner = file_data;
	info->state_active = 1;
	spin_unlock_irqrestore(&info->job_lock, flags);

	wake_irq_thread(&info->irq_sleep);

	/* wait for seek request to complete */
	wait_for_completion(&info->job_complete);

	if (info->error)
		return err;

	/* return new offset */
	err = info->lseek_offset;
	return err;

out_release_lock:
	spin_unlock_irqrestore(&info->job_lock, flags);
	return err;
}

/* --------------------------------------------------------------------- */

/*
 * int vmfile_fasync(int fd, struct file *file, int mode)
 *
 * common fasync stuff ... don't change
 */
static int vmfile_fasync(int fd, struct file *file, int mode)
{
	file_data_t *file_data = file->private_data;
	int retval;

	retval = fasync_helper(fd, file, mode, &file_data->fasync);
	if (retval < 0)
		return retval;
	return 0;
}

/* --------------------------------------------------------------------- */

/*
 * IOCTL helpers
 */
static int vmfile_set_eventmask(file_data_t *file_data, unsigned long newmask)
{
	struct vmfile_driverinfo *info = file_data->info;
	unsigned long flags;
	int err = -EAGAIN;

	/* check for other pending operations */
	spin_lock_irqsave(&info->job_lock, flags);
	if (info->job != JOB_NONE)
		goto out_release_lock;

	/* check handle, device must be open! */
	if (file_data->handle < 0)
		goto out_release_lock;

	info->eventmask = newmask;

	/* clear completion state */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,13,0)
	reinit_completion(&info->job_complete);
#else
	INIT_COMPLETION(info->job_complete);
#endif

	/* start the job! */
	info->job = JOB_EVMASK;
	info->job_owner = file_data;
	info->state_active = 1;
	spin_unlock_irqrestore(&info->job_lock, flags);

	wake_irq_thread(&info->irq_sleep);

	/* wait for request to complete */
	wait_for_completion(&info->job_complete);

	if (info->error)
		err = -EPERM;
	else
		err = 0;
	return err;

out_release_lock:
	spin_unlock_irqrestore(&info->job_lock, flags);
	return err;
}

static int vmfile_event_wait(file_data_t *file_data, unsigned long wflags)
{
	struct vmfile_driverinfo *info = file_data->info;
	unsigned long flags;
	int err = -EAGAIN;

	/* check for other pending operations */
	spin_lock_irqsave(&info->job_lock, flags);
	if (info->job != JOB_NONE)
		goto out_release_lock;

	/* check handle, device must be open! */
	if (file_data->handle < 0)
		goto out_release_lock;

	info->preclear = wflags & VMDRV_EVENT_PRECLEAR;
	info->postclear = wflags & VMDRV_EVENT_POSTCLEAR;

	/* clear completion state */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,13,0)
	reinit_completion(&info->job_complete);
#else
	INIT_COMPLETION(info->job_complete);
#endif

	/* start the job! */
	info->job = JOB_EVWAIT;
	info->job_owner = file_data;
	info->state_active = 1;
	spin_unlock_irqrestore(&info->job_lock, flags);

	wake_irq_thread(&info->irq_sleep);

	/* wait for request to complete */
	wait_for_completion(&info->job_complete);

	if (info->error)
		err = -EPERM;
	else
		err = 0;
	return err;

out_release_lock:
	spin_unlock_irqrestore(&info->job_lock, flags);
	return err;
}

static int vmfile_ioctl_helper(file_data_t *file_data, char __user* data)
{
	struct vmfile_driverinfo *info = file_data->info;
	p4_vmfile_ioctl_t iodata;
	unsigned long flags;
	int err = -EAGAIN;

	/* check for other pending operations */
	spin_lock_irqsave(&info->job_lock, flags);
	if (info->job != JOB_NONE)
		goto out_release_lock;

	/* check handle, device must be open! */
	if (file_data->handle < 0)
		goto out_release_lock;

	if (copy_from_user(&iodata, data, sizeof(iodata))) {
		err = -EFAULT;
		goto out_release_lock;
	}

	err = 0;

	info->ioctl_cmd = iodata.cmd;
	info->ioctl_insize = iodata.insize ? iodata.insize : -1;
	info->ioctl_outsize = iodata.outsize ? iodata.outsize : -1;
	info->ioctl_data = iodata.data;

	/* clear completion state */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,13,0)
	reinit_completion(&info->job_complete);
#else
	INIT_COMPLETION(info->job_complete);
#endif

	info->job = JOB_IOCTL;
	info->job_owner = file_data;
	info->state_active = 1;
	spin_unlock_irqrestore(&info->job_lock, flags);

	wake_irq_thread(&info->irq_sleep);

	/* wait for request to complete */
	wait_for_completion(&info->job_complete);

	if (info->error) {
		err = -EPERM;
	} else
	if (iodata.outsize != 0) {
		if (copy_to_user(data, &iodata, sizeof(iodata))) {
			err = -EFAULT;
		}
	}

	return err;

out_release_lock:
	spin_unlock_irqrestore(&info->job_lock, flags);
	return err;
}

/*
 * static int vmfile_ioctl(...)
 *
 * ioctl stuff
 */
static long vmfile_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	file_data_t *file_data = file->private_data;
	struct vmfile_driverinfo *info = file_data->info;
	unsigned long val;

	switch (cmd)
	{
	case VMDRV_GET_FILE_SIZE:
		if (copy_to_user((void *)arg, &file_data->size, sizeof(int)))
			return -EFAULT;
		break;

	case VMDRV_GET_EVENT_MASK:
		if (copy_to_user((void *)arg, &info->eventmask, sizeof(long)))
			return -EFAULT;
		break;

	case VMDRV_SET_EVENT_MASK:
		if (copy_from_user(&val, (void *)arg, sizeof(long)))
			return -EFAULT;
		return vmfile_set_eventmask(file_data, val);

	case VMDRV_EVENT_WAIT:
		if (copy_from_user(&val, (void *)arg, sizeof(long)))
			return -EFAULT;
		return vmfile_event_wait(file_data, val);

	case VMDRV_GET_UID:
		val = get_irq_uid(info->irq);
		if (copy_to_user((void *)arg, &val, sizeof(int)))
			return -EFAULT;
		break;

	case VMDRV_IOCTL:
		return vmfile_ioctl_helper(file_data, (char __user*)arg);
		break;

	default:
		return -ENOTTY;
	}
	return 0;
}

/* --------------------------------------------------------------------- */

/*
 * int vmfile_open(struct inode *inode, struct file *file)
 *
 * open file request: clear buffers, arm IRQ thread, ...
 */
static int vmfile_open(struct inode *inode, struct file *file)
{
	file_data_t *file_data;
	struct vmfile_driverinfo *info;
	unsigned int minor = iminor(inode);
	int err = 0;
	struct p4vm_list *list;
	unsigned int flags;

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
		err = -ENXIO;
		goto out;
	}

	file_data = kzalloc(sizeof(file_data_t), GFP_KERNEL);
	if (file_data == NULL) {
		err = -ENOMEM;
		goto out;
	}

	if (info->count == 0) {
		info->buf = kmalloc(MAX_FRAMESIZE, GFP_KERNEL);
		if (!info->buf) {
			err = -ENOMEM;
			goto out;
		}
		/* startup async thread */
		info->irq = get_free_async_irq();
		if (!info->irq) {
			printk(KERN_ERR P4VM_DEV_NAME ": unable to get free irqs\n");
			kfree(info->buf);
			err = -EAGAIN;
			goto out;
		}

		err = request_irq(info->irq, vmfile_irq, 0, "vmfile_raw", info);
		if (err) {
			printk(KERN_ERR P4VM_DEV_NAME ": unable to request read-IRQ %d\n", info->irq);
			release_async_irq(info->irq);
			kfree(info->buf);
			goto out;
		}

		err = enable_irq_thread(info->irq, "vmfile_char_rxtx", vmfile_thread, info);
		if (err) {
			err = -ENOMEM;
			printk(KERN_ERR P4VM_DEV_NAME ": could not start async IRQ thread\n");
			free_irq(info->irq, info);
			release_async_irq(info->irq);
			kfree(info->buf);
			goto out;
		}
	}

	file_data->info = info;
	file->private_data = file_data;

	/* Converte UNIX flags to VMAPI flags */
	switch (file->f_flags & O_ACCMODE) {
		case O_WRONLY:
			flags = VMAPI_O_W;
			file_data->is_write = 1;
			break;
		case O_RDONLY:
			flags = VMAPI_O_R;
			file_data->is_write = 0;
			break;
		case O_RDWR:
			flags = VMAPI_O_W | VMAPI_O_R;
			file_data->is_write = 1;
			break;
		default:
			return -EINVAL;
	}

	flags = flags | VMAPI_O_MAP;
	file_data->vmapi_flags = flags;

	/* disable event reception for this thread */
	info->eventmask = -1UL;

	/* async open */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,13,0)
	reinit_completion(&info->job_complete);
#else
	INIT_COMPLETION(info->job_complete);
#endif
	info->job = JOB_OPEN;
	info->job_owner = file_data;
	info->state_active = 1;
	wake_irq_thread(&info->irq_sleep);

	wait_for_completion(&info->job_complete);

	DBG(printk("open done - handle=%d\n", file_data->handle));

	/* in case of error, kill the thread again */
	if (info->error) {
		err = info->error;
		/* make sure that the irq thread is sleeping */
		while (!uk_uevent_is_waiting(&info->irq_sleep))
			cpu_relax();

		disable_irq_thread(info->irq);
		free_irq(info->irq, info);
		release_async_irq(info->irq);
		kfree(info->buf);
		kfree(file_data);
	} else {
		/* update port info */
		info->count++;
	}

out:
	return err;
}

/* --------------------------------------------------------------------- */

/*
 * int vmfile_close(struct inode *inode, struct file *file)
 *
 * close file request: disarm IRQ thread (if possible)
 */
static int vmfile_close(struct inode *inode, struct file *file)
{
	file_data_t *file_data = file->private_data;
	struct vmfile_driverinfo *info = file_data->info;

	if (file_data->handle >= 0) {
		vmapi_file_close(file_data->handle);
	}
	kfree(file_data);

	info->count--;
	if (info->count == 0) {
		/* disable IRQ thread */
		info->state_active = 0;

		/* make sure that the irq thread is sleeping */
		while (!uk_uevent_is_waiting(&info->irq_sleep))
			cpu_relax();

		disable_irq_thread(info->irq);
		free_irq(info->irq, info);
		release_async_irq(info->irq);
		kfree(info->buf);
	}

	return 0;
}

static int vmfile_mmap(struct file *file, struct vm_area_struct *vma)
{
	file_data_t *file_data = file->private_data;
	struct vmfile_driverinfo *info = file_data->info;
	int err;
	unsigned long size;
	unsigned long offs;
	int need_write;

	/* check sizes and offset */
	size = vma->vm_end - vma->vm_start;
	offs = vma->vm_pgoff;
	need_write = !!(pgprot_val(vma->vm_page_prot) & _PAGE_RW);

	DBG(printk("mmap: req. size 0x%08lx, offs 0x%08lx, need_write %d, base 0x%08lx, rsize 0x%08x\n",
			size, offs, need_write, info->base, file_data->size));

	if ((size > file_data->size) || (size + offs > file_data->size)) {
		return -EAGAIN;
	}

	if (need_write && !file_data->is_write) {
		return -EACCES;
	}

	err = 0;
	if (info->area == NULL) {
		/* get some mmap space */
		info->area = get_vm_area(file_data->size, VM_IOREMAP);
		if (!info->area) {
			DBG(printk("unable to get virt memory space\n"));
			err = -ENOMEM;
			goto out;
		}

		/* request mapping */
		info->base = (unsigned long)info->area->addr;
		DBG(printk("base address: 0x%08lx\n", info->base));
		/* protect memory against TLB flushes */
		vm_protect(info->base, file_data->size);
		err = vmapi_file_fmap(file_data->handle, 0,
				file_data->size, info->base, file_data->vmapi_flags);
		if (err < 0) {
			DBG(printk("e-OPEN fmap failed, error %d\n", err));
			remove_vm_area(info->area->addr);
			goto out;
		}
	}

	if (err == 0) {
		/* map to user */
		err = remap_pfn_range(vma, vma->vm_start,
					(info->base + offs) >> PAGE_SHIFT,
					vma->vm_end-vma->vm_start,
					vma->vm_page_prot);
	}

out:
	return err;
}



/* --------------------------------------------------------------------- */

/*
 * file ops
 */
static struct file_operations vmfile_fops = {
	owner:		THIS_MODULE,
	read:		vmfile_read,
	write:		vmfile_write,
	mmap:		vmfile_mmap,
	llseek:		vmfile_lseek,
	fasync:		vmfile_fasync,
	unlocked_ioctl:	vmfile_ioctl,
	open:		vmfile_open,
	release:	vmfile_close,
};


static int vmfile_probe(struct platform_device *pdev)
{
	struct vmfile_driverinfo *info;
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
	init_completion(&info->job_complete);
	spin_lock_init(&info->job_lock);
	
	device_create(p4vm_class, NULL, MKDEV(VMFILE_MAJOR, pdev->id), NULL, P4VM_DEV_NAME "%u", pdev->id);

	platform_set_drvdata(pdev, info);

	return 0;
}

static int vmfile_remove(struct platform_device *pdev)
{
	struct vmfile_driverinfo *info = platform_get_drvdata(pdev);

	kfree(info);

	device_destroy(p4vm_class, MKDEV(VMFILE_MAJOR, pdev->id));
	return 0;
}

static struct platform_driver p4vm_driver = {
	.driver		= {
		.name	= P4VM_DEV_NAME,
		.owner	= THIS_MODULE,
	},
	.probe  = vmfile_probe,
	.remove = vmfile_remove,
};

/*
 * int vmfile_init(void)
 *
 * Initialize and register driver
 */
static int __init vmfile_init(void)
{
	int ret;

	/* register driver */
	ret = register_chrdev(VMFILE_MAJOR, P4VM_DEV_NAME, &vmfile_fops);
	if (ret != 0) {
		printk(KERN_ERR P4VM_DEV_NAME ": could register major number %d, error %d\n", VMFILE_MAJOR, ret);
		return -ENODEV;
	}

	p4vm_class = class_create(THIS_MODULE, P4VM_DEV_NAME);

	ret = platform_driver_register(&p4vm_driver);
	if (ret)
		printk(KERN_INFO P4VM_DEV_NAME ": driver registration failed: %d\n", ret);
	else
		printk(KERN_INFO P4VM_DEV_NAME ": driver registered as major %d\n", VMFILE_MAJOR);

	p4vm_driver_init();

	return ret;
}

/*
 * void vmfile_exit(void)
 *
 * Shutdown driver, remove proc and devfs related entries
 */
static void __exit vmfile_exit(void)
{
	p4vm_driver_exit();

	unregister_chrdev(VMFILE_MAJOR, P4VM_DEV_NAME);
	platform_driver_unregister(&p4vm_driver);
	class_destroy(p4vm_class);
}

module_init(vmfile_init);
module_exit(vmfile_exit);

MODULE_AUTHOR("Alex Zuepke <azu@sysgo.de>");
MODULE_DESCRIPTION("PikeOS File Provider (client) raw char driver");
MODULE_LICENSE("GPL");
