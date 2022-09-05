/*
 *  P4VM generic driver interface
 * 
 *  Copyright (C) 2011, SYSGO AG
 * 
 *  Author: David Engraf <den@sysgo.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

/*
 * This interface handles adding/removing of dynamic PikeOS drivers as well
 * as the proc interface. The purpose is to remove the code for handling
 * multiple devices from the driver itself and use the linux platform driver
 * registration interface.
 *
 * Driver concept:
 * - module/driver load
 * - register driver in the linux system
 * - new device gets attached (write to proc interface):
 *   - generic driver interface will add the platform device and call the driver
 *     probe function
 *   - driver probe function should create private structure and initialize
 *     variables
 * - device removal (write to proc interface)
 *   - generic driver interface will call the driver remove function
 *   - remove function should destroy private structure
 */

struct p4vm_list
{
	struct list_head list;

	unsigned int minor;
	const char *parm1;
	const char *parm2;

	struct platform_device *pdev;
};

/*
 * global variables
 */
static LIST_HEAD(p4vm_devlist);
static DEFINE_MUTEX(p4vm_devlistlock);	/* protects driver_info ... */

/*
 * Define P4VM_CHECK_DEVICE if you need to make additional checks before adding
 * or removing the device (e.g. vmfile_shm).
 */
#ifdef P4VM_CHECK_DEVICE
#define P4VM_STATE_DEVICE_ADD		0
#define P4VM_STATE_DEVICE_REMOVE	1
static int p4vm_check_device(int state, unsigned int minor, const char *parm1,
						   const char *parm2);
#endif

/*
 * Define P4VM_SHOW_ADDITIONAL_INFO if you want to show additional informations
 * when the user reads the proc interface.
 */
#ifdef P4VM_SHOW_ADDITIONAL_INFO
static char *p4vm_proc_additional_info(struct p4vm_list *list, char *p);
#endif

/*
 * ADD and REMOVE interfaces
 */

/*
 * Add the interface "P4VM_DEV_NAME<minor>", but do not open the file. The 
 * function uses the linux platform device registration concept and calls the
 * driver probe function registered for P4VM_DEV_NAME.
 */
static int p4vm_add_interface(unsigned int minor, const char *parm1, 
							const char *parm2)
{
	struct resource res[2];
	struct p4vm_list *list;
#ifdef P4VM_CHECK_DEVICE
	int err;
#endif

	/* now check if <num> is not used yet */
	mutex_lock(&p4vm_devlistlock);
	list_for_each_entry(list, &p4vm_devlist, list)
	{
		if (list->minor == minor)
		{
			mutex_unlock(&p4vm_devlistlock);
			return -EBUSY;
		}
	}
	mutex_unlock(&p4vm_devlistlock);

#ifdef P4VM_CHECK_DEVICE
	/* driver wants to make an additional check */
	err = p4vm_check_device(P4VM_STATE_DEVICE_ADD, minor, parm1, parm2);
	if (err) return err;
#endif

	list = kzalloc(sizeof(*list), GFP_KERNEL);
	if (list == NULL)
		return -ENOMEM;
	
	list->minor = minor;
	list->parm1 = parm1;
	list->parm2 = parm2;

	memset(&res, 0, sizeof(res));
	res[0].name  = (const char*)list->parm1;
	res[0].flags = IORESOURCE_BUS;
	if (parm2) {
		res[1].name  = (const char*)list->parm2;
		res[1].flags = IORESOURCE_BUS;
	}

	list->pdev = platform_device_register_simple(P4VM_DEV_NAME, list->minor,
						     res, parm2 ? 2 : 1);
	if (IS_ERR(list->pdev)) {
		kfree(list);
		return -1;
	}

	mutex_lock(&p4vm_devlistlock);
	list_add_tail(&list->list, &p4vm_devlist);
	mutex_unlock(&p4vm_devlistlock);

	return 0;
}

/*
 * Removes interface "P4VM_DEV_NAME<minor>". This function will call the remove
 * callback from the registered driver which should destroy the handle.
 */
static void p4vm_remove_interface(unsigned int minor)
{
	struct p4vm_list *list;

	/* search list of devices for the one */
	mutex_lock(&p4vm_devlistlock);
	/* scan the list ...*/
	list_for_each_entry(list, &p4vm_devlist, list)
	{
		if (list->minor == minor) {
#ifdef P4VM_CHECK_DEVICE
			if (p4vm_check_device(P4VM_STATE_DEVICE_REMOVE, minor, list->parm1,
				list->parm2)) {
				/* device removal not allowed */
				mutex_unlock(&p4vm_devlistlock);
				return;
			}
#endif

			platform_device_unregister(list->pdev);
			list_del(&list->list);
			kfree(list);
			mutex_unlock(&p4vm_devlistlock);
			return;
		}
	}
	mutex_unlock(&p4vm_devlistlock);

	printk(KERN_INFO P4VM_DEV_NAME ": cannot remove device (id %d not found)\n", 
		   minor);
}

/*
 * accepts two/three strings to add and remove devices:
 *
 * "5"               - remove device 5
 * "1 prov:/devname" - register "prov:/devname"
 * "2 rxport txport" - register device 2 with rx/tx port
 */
#define TMPSIZE 128
static ssize_t config_store(struct device_driver *driver, const char *buf,
			    size_t count)
{
	char *parm1;
	char *parm2;

	unsigned int minor;
	int ret, err;

	if (!count)
		return -EINVAL;

	if (count > TMPSIZE)
		count = TMPSIZE;

	ret = - ENOMEM;

	/* allocate temp buffers */
	parm1 = kmalloc(TMPSIZE+1, GFP_KERNEL);
#ifdef P4VM_TYPE_FP
	parm2 = NULL;
#else
	parm2 = kmalloc(TMPSIZE+1, GFP_KERNEL);
	if (!parm2)
		goto free;
#endif
	if (!parm1)
		goto free;

#ifdef P4VM_TYPE_FP
	ret = sscanf(buf, "%u %s\n", &minor, parm1);
	if (ret == 2)
#else
	ret = sscanf(buf, "%u %s %s\n", &minor, parm1, parm2);
	if (ret == 3)
#endif
	{
		err = p4vm_add_interface(minor, parm1, parm2);
		if (err) {
			printk(KERN_ERR P4VM_DEV_NAME ": failed to add inferface "
				   P4VM_DEV_NAME "%d using %s %s; error %d\n", minor, 
				   parm1, parm2 ? parm2 : "", err);
		}
		else {
			ret = count;
			goto done;
		}
	}
	else if (ret == 1)
		p4vm_remove_interface(minor);

	ret = count;

free:
	kfree(parm1);
	kfree(parm2);

done:
	return ret;
}

static ssize_t config_show(struct device_driver *driver, char *buf)
{
	struct p4vm_list *list;
	char *p = buf;

#ifdef P4VM_SHOW_ADDITIONAL_INFO
	p += sprintf(p, P4VM_SHOW_ADDITIONAL_INFO "\n");
#else
	p += sprintf(p, "# <id/minor> <filename>\n");
#endif

	/* print list of devices */
	mutex_lock(&p4vm_devlistlock);
	list_for_each_entry(list, &p4vm_devlist, list)
	{
#ifdef P4VM_SHOW_ADDITIONAL_INFO
		p = p4vm_proc_additional_info(list, p);
#else
		p += sprintf(p, "%d %s\n", list->minor, list->parm1);
#endif
	}
	mutex_unlock(&p4vm_devlistlock);

	return strlen(buf);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)
static DRIVER_ATTR_RW(config);
#else
static DRIVER_ATTR(config, (S_IWUSR | S_IRUGO), config_show, config_store);
#endif

/*
 * init generic interface
 */
static int p4vm_driver_init(void)
{
	return driver_create_file(&p4vm_driver.driver, &driver_attr_config);
}

/*
 * exit generic interface (please call it before removing the driver)
 */
static void p4vm_driver_exit(void)
{
	struct p4vm_list *list, *temp;

	driver_remove_file(&p4vm_driver.driver, &driver_attr_config);

	/* remove the devices here, this is not mandatory because linux 
	 * automatically calls the remove function for all registered devices but
	 * we have to hold the device list in sync with the linux platform subsystem
	 * e.g. remove and reload the module driver while a device is connected
	 */
	list_for_each_entry_safe(list, temp, &p4vm_devlist, list) {
		p4vm_remove_interface(list->minor);
	}
}
