/*
 * arch/p4/kernel/devtree.c
 *
 * Copyright (C) 2014-2015 SYSGO AG (jho@sysgo.com)
 *
 * Device Tree Setup
 */

#include <linux/init.h>
#include <linux/bootmem.h>
#include <linux/mm.h>
#include <linux/of.h>
#include <linux/of_fdt.h>
#include <linux/debugfs.h>
#include <linux/version.h>
#include <asm/types.h>
#include <asm/memory.h>
#include <asm/vmapi_calls.h>

void * __init early_init_dt_alloc_memory_arch(u64 size, u64 align)
{
	return __alloc_bootmem(size, align, __pa(MAX_DMA_ADDRESS));
}

void __init early_init_dt_scan_chosen_arch(unsigned long node)
{
	BUG();
}

void __init early_init_dt_add_memory_arch(u64 base, u64 size)
{
	BUG();
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)
void __init early_init_dt_setup_initrd_arch(u64 start, u64 end)
#else
void __init early_init_dt_setup_initrd_arch(unsigned long start,
					    unsigned long end)
#endif
{
}

static void *__init devtree_from_rom(char *path)
{
	unsigned char *dtb_start, *dtb_end;
	int fd, read, size;
	unsigned char *blob;

	fd = vmapi_file_open4(path, VMAPI_O_R, &size, 0);
	if (fd < 0) {
		pr_err("p4dtb: failed to open device tree blob <%s>: err=%d\n", path, fd);
		return 0;
	}

	if (size <= 8) {
		if (size >= 0)
			pr_err("p4dtb: size of file %s is %d, ingoring dtb\n",
			       path, size);
		else
			pr_err("p4dtb: failed to get size of file %s\n",
			       path);
		goto error;
	}

	pr_info("p4dtb: loading device tree from file %s, size %d bytes\n",
		path, size);

	blob = alloc_bootmem_pages(PAGE_ALIGN(size));
	if (!blob) {
		pr_err("p4dtb: not enough memory to load dtb!\n");
		goto error;
	}

	dtb_start = blob;
	dtb_end = dtb_start + PAGE_ALIGN(size);

	while (size > 0) {
		read = vmapi_file_read(fd, blob, size);

		if (read < 0) {
			pr_err("p4dtb: error while reading the dtb\n");
			dtb_start = 0;
			break;
		}
		blob += read;
		size -= read;
	}
	vmapi_file_close(fd);
	return dtb_start;
error:
	vmapi_file_close(fd);
	return 0;
}


static int __init setup_dtb(char *path)
{
	struct boot_param_header *dtb;

	dtb = devtree_from_rom(path);
	if (dtb == 0){
		panic("No device tree found at %s\n", path);
	}

	initial_boot_params = dtb;
	unflatten_device_tree();
	return 0;
}

/* load before setup_arch */
early_param("dtb", setup_dtb);
