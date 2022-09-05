/*
 *  Partition control driver
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
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/version.h>
#include <asm/vmapi_calls.h>

MODULE_AUTHOR("Alex Zuepke <azu@sysgo.de>");
MODULE_DESCRIPTION("PikeOS partition control driver");
MODULE_LICENSE("GPL");

#define PC_DEV_NAME	"partctrl"		/* device name */

/*
 * /proc/partctrl accepts strings to reboot and shutdown partitions
 * or the whole system:
 *
 * "shutdown 0"      - shutdown target
 * "shutdown 7"      - shutdown partition 7 (1..N)
 * "reboot 0"        - reboot target
 * "reboot 3"        - reboot partition 3 (1..N)
 * "sched SCHED1"    - switch to scheduling scheme SCHED1
 */
#define TMPSIZE 128
static ssize_t command_store(struct device_driver *driver, const char *buf,
			     size_t count)
{
	char *command;
	unsigned int id;
	int ret;

	if (!count)
		return -EINVAL;

	if (count > TMPSIZE)
		count = TMPSIZE;

	/* allocate temp buffers */
	ret = -ENOMEM;
	command = kmalloc(TMPSIZE+1, GFP_KERNEL);
	if (!command)
		goto free;

	ret = sscanf(buf, "%s %u\n", command, &id);
	if (ret == 2) {
		/* activate device */
		if (strncmp(command, "shutdown", TMPSIZE) == 0) {
			if (id == 0)
				ret = vmapi_target_shutdown();
			else
				ret = vmapi_partition_shutdown(id);
		} else if (strncmp(command, "poweroff", TMPSIZE) == 0) {
			if (id == 0)
				ret = vmapi_target_power_off();
			else
				ret = vmapi_partition_shutdown(id);
		} else if (strncmp(command, "reboot", TMPSIZE) == 0) {
			if (id == 0)
				ret = vmapi_target_reboot();
			else
				ret = vmapi_partition_reboot(id);
		} else
			goto err_unknown;
	} else {
		ret = sscanf(buf, "sched %s\n", command);
		if (ret == 1)
			ret = vmapi_set_time_sched_scheme(command);
		else
			goto err_unknown;
	}

	if (ret)
		printk(KERN_ERR PC_DEV_NAME ": Error executing command %d\n", ret);
	ret = count;
	goto free;

err_unknown:
	printk(KERN_ERR PC_DEV_NAME ": Error unknown or invalid command, supported:\n");
	printk(KERN_ERR "          shutdown <id>, poweroff <id>, reboot <id>, sched\n");
	ret = -EINVAL;

free:
	kfree(command);
	return ret;
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)
static DRIVER_ATTR_WO(command);
#else
static DRIVER_ATTR(command, S_IWUSR, NULL, command_store);
#endif

static struct platform_driver pc_driver = {
	.driver = {
		   .name = PC_DEV_NAME,
		   .owner = THIS_MODULE,
		   },
};

/*
 * int pc_init(void)
 *
 * Initialize and register driver
 */
static int __init pc_init(void)
{
	int ret;

	platform_device_register_simple(PC_DEV_NAME, 0, NULL, 0);
	ret = platform_driver_register(&pc_driver);
	ret |= driver_create_file(&pc_driver.driver, &driver_attr_command);
	if (!ret)
		printk(KERN_INFO PC_DEV_NAME ": driver registered\n");

	return ret;
}

/*
 * void pc_exit(void)
 *
 * Shutdown driver, remove proc and devfs related entries
 */
static void __exit pc_exit(void)
{
	driver_remove_file(&pc_driver.driver, &driver_attr_command);
	platform_driver_unregister(&pc_driver);
}

module_init(pc_init);
module_exit(pc_exit);
