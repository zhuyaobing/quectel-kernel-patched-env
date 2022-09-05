/*
 * linux/arch/p4/kernel/reboot.c
 *
 * Copyright (C) 2006-2009 SYSGO AG (azu@sysgo.de)
 *
 * reboot handling
 */

#include <linux/kernel.h>
#include <linux/reboot.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#include <asm/vmapi_calls.h>

#define DEV_NAME "part_reboot"

static int reboot_target;
void (*pm_power_off)(void);

/*
 * specify "reboot=target" or "reboot=system" at boot cmdline
 * to reboot the whole system on a linux reboot / shutdown
 */
static int __init reboot_setup(char *str)
{
	if ((strncmp(str, "target", 6) == 0) || (strncmp(str, "system", 6) == 0))
		reboot_target = 1;
	return 1;
}

__setup("reboot=", reboot_setup);

/*
 *   write "target" or "system" to reboot the whole system,
 *   write "partition" to reboot the partition only
 */
static ssize_t config_store(struct device_driver *driver, const char *buf,
			    size_t count)
{
	if (!count)
		return -EINVAL;

	if ((strncmp(buf, "target", 6) == 0) || (strncmp(buf, "system", 6) == 0))
		reboot_target = 1;
	else if (strncmp(buf, "partition", 9) == 0)
		reboot_target = 0;
	else
		printk(KERN_ERR DEV_NAME ": Error invalid argument\n");

	return count;
}

/*
 *   returns "target" or "partition" which identify reboot mode
 */
static ssize_t config_show(struct device_driver *driver, char *buf)
{
	strcpy(buf, reboot_target ? "target\n" : "partition\n");
	return strlen(buf);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)
static DRIVER_ATTR_RW(config);
#else
static DRIVER_ATTR(config, (S_IWUSR | S_IRUGO), config_show, config_store);
#endif

static struct platform_driver reboot_driver = {
	.driver = {
		   .name = DEV_NAME,
		   .owner = THIS_MODULE,
		   },
};

/*
 * register sysfs interface
 */
static int __init reboot_init(void)
{
	platform_device_register_simple(DEV_NAME, 0, NULL, 0);
	platform_driver_register(&reboot_driver);
	return driver_create_file(&reboot_driver.driver, &driver_attr_config);
}

module_init(reboot_init);


void machine_restart(char * __unused)
{
	/* try to reboot whole system (only if run in service partition) */
	if (reboot_target)
		vmapi_target_reboot();
	vmapi_partition_reboot(-1);
	/* never returns */
}

void machine_power_off(void)
{
	/* try to power off whole system (only if run in service partition) */
	if (reboot_target)
		vmapi_target_power_off();
	vmapi_partition_shutdown(-1);
	/* never returns */
}

void machine_halt(void)
{
	/* try to shutdown whole system (only if run in service partition) */
	if (reboot_target)
		vmapi_target_shutdown();
	vmapi_partition_shutdown(-1);
	/* never returns */
}
