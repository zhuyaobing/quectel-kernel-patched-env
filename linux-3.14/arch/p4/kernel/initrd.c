/*
 *  Initrd Setup
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 * 
 *  Copyright (C) 2005-2009 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 *  Version 2005-08-04, azu: initial
 */

#include <linux/init.h>
#include <linux/bootmem.h>
#include <linux/initrd.h>
#include <linux/mm.h>
#include <asm/types.h>
#include <asm/memory.h>
#include <asm/vmapi_calls.h>
#include <asm/pikeos_version.h>

#if defined(CONFIG_BLK_DEV_INITRD) || defined(CONFIG_EXTRACT_ROOTFS)

static int __init setup_initrd(char *str)
{
	int err;
	int fd;
	vmapi_off_t fsize;
	vmapi_off_t read;
	unsigned long size;
	char *data;
	char *initrd_name;
	char temp[VMAPI_NAME_LEN+12];

	err = -1;
	fd = -1;

	if (strcmp(str, "auto") == 0) {
		/* auto detetion */
		snprintf(temp, sizeof(temp), "rfs:/%s.initrd",
#if PIKEOS_VM_VERSION >= 34
			 vmapi_get_process_name()
#else
			 /* < PikeOS 4.0 */
			 vmapi_get_partname()
#endif
			 );
		initrd_name = temp;
	} else
		initrd_name = str;

	fd = vmapi_file_open4(initrd_name, VMAPI_O_R, &fsize, NULL);
	if (fd < 0) {
		if (initrd_name == str)
			pr_err("Failed to open initrd <%s>\n", initrd_name);
		else
			pr_err("No initrd found, tried <%s>\n", initrd_name);
		return -1;
	}

	if (fsize <= 0) {
		if (fsize == 0)
			pr_err("setup_initrd: size of file %s is zero, ingoring initrd\n",
			       initrd_name);
		else
			pr_err("setup_initrd: failed to get size of file %s\n",
			       initrd_name);
		goto close;
	}

	if (fsize > ULONG_MAX) {
		pr_err("setup_initrd: size of file %s too big\n", initrd_name);
		goto close;
	}
	size = fsize;

	pr_info("loading initial ramdisk from file %s, size %lu bytes\n",
		initrd_name, size);

	/*
	 * As pages are freed page-by-page the initrd's end must be aligned or
	 * the next allocations on the same page get screwed up while untarring
	 */
	data = alloc_bootmem_pages(PAGE_ALIGN(size));
	if (!data) {
		pr_err("setup_initrd: not enough memory to load initrd!\n");
		goto close;
	}

	initrd_start = (unsigned long)data;
	initrd_end = initrd_start + PAGE_ALIGN(size);

	while (size > 0) {
		read = vmapi_file_read(fd, data, size);

		if (read < 0) {
			pr_err("setup_initrd: error while reading the initrd\n");
			goto close;
		}
		data += read;
		size -= read;
	}

	err = 0;

close:
	vmapi_file_close(fd);

	return err;
}

__setup("initrd=", setup_initrd);

#endif
