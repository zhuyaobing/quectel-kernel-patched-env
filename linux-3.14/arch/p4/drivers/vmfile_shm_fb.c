/*
 *  PikeOS Virtual Framebuffer driver based on shared memory
 * 
 *  Author: David Engraf <den@sysgo.com>
 * 
 *  Copyright (C) 2012 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 */

/*
 * This driver mapes PikeOS shared memory and uses it as framebuffer. The first
 * page is reserved for the screen informations and must be configured prior
 * loading this driver.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/vmalloc.h>
#include <asm/vmapi_calls.h>
#include <asm/tlbflush.h>

#include "virtual-fb.h"

#define P4VM_DEV_NAME	"vmfile_shm_fb"     /* device name */
#define P4VM_SHARED_MEM	"shm:/framebuffer"

MODULE_AUTHOR("David Engraf <den@sysgo.com>");
MODULE_DESCRIPTION("PikeOS Virtual Framebuffer driver based on shared memory");
MODULE_LICENSE("GPL");

/* --------------------------------------------------------------------- */

static struct fb_var_screeninfo vmfb_defined __initdata = {
	.activate	= FB_ACTIVATE_NOW,
	.height		= -1,
	.width		= -1,
	.right_margin	= 32,
	.upper_margin	= 16,
	.lower_margin	= 4,
	.vsync_len	= 4,
	.vmode		= FB_VMODE_NONINTERLACED,
};

static struct fb_fix_screeninfo vmfb_fix __initdata = {
	.id	= "PIKEOS SHM VFB",
	.type	= FB_TYPE_PACKED_PIXELS,
	.accel	= FB_ACCEL_NONE,
};

struct vmfb_par {
	u32 pseudo_pal[256];
};

static char vmfb_path[64];

/* --------------------------------------------------------------------- */

static int vmfb_setcolreg(unsigned regno, unsigned red, unsigned green,
			    unsigned blue, unsigned transp,
			    struct fb_info *info)
{
	int err = 0;

	/*
	 *  Set a single color register. The values supplied are
	 *  already rounded down to the hardware's capabilities
	 *  (according to the entries in the `var' structure). Return
	 *  != 0 for invalid regno.
	 */
	
	if (regno >= info->cmap.len)
		return 1;

	if (info->var.bits_per_pixel == 8)
		err = EINVAL; /** not supported */
	else if (regno < 16) {
		switch (info->var.bits_per_pixel) {
		case 16:
			if (info->var.red.offset == 10) {
				/* 1:5:5:5 */
				((u32*) (info->pseudo_palette))[regno] =
					((red   & 0xf800) >>  1) |
					((green & 0xf800) >>  6) |
					((blue  & 0xf800) >> 11);
			} else {
				/* 0:5:6:5 */
				((u32*) (info->pseudo_palette))[regno] =
					((red   & 0xf800)      ) |
					((green & 0xfc00) >>  5) |
					((blue  & 0xf800) >> 11);
			}
			break;
		case 24:
		case 32:
			red   >>= 8;
			green >>= 8;
			blue  >>= 8;
			((u32 *)(info->pseudo_palette))[regno] =
				(red   << info->var.red.offset)   |
				(green << info->var.green.offset) |
				(blue  << info->var.blue.offset);
			break;
		}
	}

	return err;
}

static int vmfb_mmap(struct fb_info *info, struct vm_area_struct *vma)
{
	int err;
	unsigned long size;
	unsigned long offs;
	int need_write;

	/* check sizes and offset */
	size = vma->vm_end - vma->vm_start;
	offs = vma->vm_pgoff << PAGE_SHIFT;
	need_write = !!(pgprot_val(vma->vm_page_prot) & _PAGE_RW);

	//printk("mmap: req. size 0x%08x, offs 0x%08x, need_write %d, base 0x%08x, rsize 0x%08x\n", size, offs, need_write, info->base, info->size);

	if (size + offs > info->fix.smem_len) {
		return -EINVAL;
	}

	/*if (need_write && !info->is_write) {
		return -EACCES;
	}*/

	/* map to user */
	err = remap_pfn_range(vma, vma->vm_start,
			    ((unsigned long)info->screen_base + offs) >> PAGE_SHIFT,
			    vma->vm_end-vma->vm_start,
			    vma->vm_page_prot);

	return err;
}

static struct fb_ops vmfb_ops = {
	.owner		= THIS_MODULE,
	.fb_setcolreg	= vmfb_setcolreg,
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
	.fb_mmap	= vmfb_mmap,
};

static int vmfb_probe(struct platform_device *dev)
{
	int err;
	unsigned int size_total;
	struct fb_info *info;
	struct vmfb_par *vmfb_par;
	struct virtual_fb_info p4fbinfo;
	struct vm_struct *area;
	int handle;
	vmapi_off_t size_o;

	info = framebuffer_alloc(sizeof(vmfb_par), &dev->dev);
	if (!info) {
		return -ENOMEM;
	}
	vmfb_par = info->par;

	if (vmfb_path[0] == '\0')
		strlcpy(vmfb_path, P4VM_SHARED_MEM, sizeof(vmfb_path));

	handle = vmapi_file_open4(vmfb_path, VMAPI_O_R|VMAPI_O_W|VMAPI_O_MAP, &size_o, NULL);
	if (handle < 0)
	{
		dev_err(&dev->dev, "Error opening %s (%d)\n", vmfb_path,
			handle);
		framebuffer_release(info);
		return -ENODEV;
	}
	size_total = size_o;

	/* read the p4fb struct from driver (first PAGE) */
	err = vmapi_file_read(handle, &p4fbinfo, sizeof(struct virtual_fb_info));

	if (err != sizeof(struct virtual_fb_info)) {
		dev_err(&dev->dev, "Error when reading fbinfo\n");
		err = -EIO;
		goto error_close;
	}

	if (p4fbinfo.bpp <= 8) {
		dev_err(&dev->dev, "8 bit mode not supported\n");
		err=-ENODEV;
		goto error_close;
	}

	/* first page is reserved for header, exclude from total size */
	size_total -= PAGE_SIZE;
	if (size_total < p4fbinfo.linelen * p4fbinfo.height) {
		dev_err(&dev->dev, "size of shared memory too small for dimension %d x %d (%u, minimum %d)\n",
			p4fbinfo.width, p4fbinfo.height, size_total,
			p4fbinfo.linelen * p4fbinfo.height);
		err=-ENODEV;
		goto error_close;
	}
	else if (size_total > PAGE_ALIGN(p4fbinfo.linelen * p4fbinfo.height)) {
		/* shared memory buffer too large */
		size_total = PAGE_ALIGN(p4fbinfo.linelen * p4fbinfo.height);
	}

	/* get some mmap space */
	area = get_vm_area(size_total, VM_IOREMAP);
	if (!area) {
		dev_err(&dev->dev, "Unable to get virtual memory space\n");
		err = -ENOMEM;
		goto error_close;
	}

	/* request mapping */
	vmfb_fix.smem_start = (unsigned long)area->addr;
	vmfb_fix.smem_len = size_total;

	/* protect memory against TLB flushes */
	vm_protect(vmfb_fix.smem_start, size_total);

	/* map the shared memory to the framebuffer area (ignore header) */
	err = vmapi_file_fmap(handle, PAGE_SIZE, size_total, vmfb_fix.smem_start,
			VMAPI_O_R|VMAPI_O_W|VMAPI_O_MAP);
	if (err < 0) {
		dev_err(&dev->dev, "fmap failed, error %d\n", err);
		remove_vm_area(area->addr);
		goto error_close;
	}

	vmfb_defined.bits_per_pixel = p4fbinfo.bpp;
	if (15 == vmfb_defined.bits_per_pixel)
		vmfb_defined.bits_per_pixel = 16;
	vmfb_defined.xres = p4fbinfo.width;
	vmfb_defined.yres = p4fbinfo.height;
	vmfb_fix.line_length = p4fbinfo.linelen;
	vmfb_fix.visual   = FB_VISUAL_TRUECOLOR;

	if (!request_mem_region(vmfb_fix.smem_start, size_total, "vfb")) {
		dev_err(&dev->dev, "cannot reserve video memory at 0x%lx\n",
			vmfb_fix.smem_start);
		/* We cannot make this fatal. Sometimes this comes from magic
		   spaces our resource handlers simply don't know about */
	}

	info->pseudo_palette = vmfb_par->pseudo_pal;

	info->screen_base = (char*)vmfb_fix.smem_start;

	vmfb_defined.xres_virtual = vmfb_defined.xres;
	vmfb_defined.yres_virtual = vmfb_fix.smem_len / vmfb_fix.line_length;
	vmfb_defined.yres_virtual = vmfb_defined.yres;

	/* some dummy values for timing to make fbset happy */
	vmfb_defined.pixclock     = 10000000 / vmfb_defined.xres * 1000 / vmfb_defined.yres;
	vmfb_defined.left_margin  = (vmfb_defined.xres / 8) & 0xf8;
	vmfb_defined.hsync_len    = (vmfb_defined.xres / 8) & 0xf8;

	vmfb_defined.red.offset    = p4fbinfo.r_pos;
	vmfb_defined.red.length    = p4fbinfo.r_size;
	vmfb_defined.green.offset  = p4fbinfo.g_pos;
	vmfb_defined.green.length  = p4fbinfo.g_size;
	vmfb_defined.blue.offset   = p4fbinfo.b_pos;
	vmfb_defined.blue.length   = p4fbinfo.b_size;
	vmfb_defined.transp.offset = p4fbinfo.x_pos;
	vmfb_defined.transp.length = p4fbinfo.x_size;

	if (vmfb_defined.bits_per_pixel <= 8) {
		vmfb_defined.red.length =
		vmfb_defined.green.length =
		vmfb_defined.blue.length =
		vmfb_defined.bits_per_pixel;
	}

	info->var = vmfb_defined;
	info->fix = vmfb_fix;
	info->flags = FBINFO_DEFAULT | FBINFO_HWACCEL_DISABLED;

	info->fbops = &vmfb_ops;

	info->fbops->fb_pan_display = NULL;

	if (fb_alloc_cmap(&info->cmap, 256, 0) < 0) {
		err = -ENOMEM;
		goto error_release;
	}
	if (register_framebuffer(info)<0) {
		err = -EINVAL;
		fb_dealloc_cmap(&info->cmap);
		goto error_release;
	}
	platform_set_drvdata(dev, info);

	/* print some information */
	printk(KERN_INFO
	       "fb%d: framebuffer at 0x%lx, mapped to 0x%p, using %dk\n",
	       info->node,
	       vmfb_fix.smem_start, info->screen_base, size_total/1024);
	printk(KERN_INFO "fb%d: mode is %dx%dx%d, linelength=%d\n", info->node,
	       vmfb_defined.xres, vmfb_defined.yres, vmfb_defined.bits_per_pixel,
	       vmfb_fix.line_length);
	printk(KERN_INFO "fb%d: %s: "
	       "size=%d:%d:%d:%d, shift=%d:%d:%d:%d\n",
	       info->node,
	       "Truecolor",
	       p4fbinfo.x_size,
	       p4fbinfo.r_size,
	       p4fbinfo.g_size,
	       p4fbinfo.b_size,
	       p4fbinfo.x_pos,
	       p4fbinfo.r_pos,
	       p4fbinfo.g_pos,
	       p4fbinfo.b_pos);
	printk(KERN_INFO "fb%d: %s frame buffer device\n",
	       info->node, info->fix.id);

	vmapi_file_close(handle);
	return 0;

error_release:
	release_mem_region(vmfb_fix.smem_start, size_total);
error_close:
	vmapi_file_close(handle);
	framebuffer_release(info);

	return err;
}

static int vmfb_remove(struct platform_device *dev)
{
	struct fb_info *info = platform_get_drvdata(dev);
	struct vmfb_par *vmfb_par;

	if (info)
	{
		vmfb_par = info->par;

		unregister_framebuffer(info);
		fb_dealloc_cmap(&info->cmap);
		framebuffer_release(info);
	}

	return 0;
}


static struct platform_driver vmfb_driver = {
	.probe	= vmfb_probe,
	.remove = vmfb_remove,
	.driver		= {
		.name	= P4VM_DEV_NAME,
	},
};

static int vmfb_boot_minor = -1;	/* -1: not used, otherwise minor */

/*
 * The virtual framebuffer driver is only enabled if the minor number is
 * passed (video=vmfile_shm_fb:0).
 */
static void __init vmfb_setup(char *options)
{
	int minor;
	char *n;

	if (!options)
		return;

	/* get first substring: minor(int) */
	minor = simple_strtol(options, &n, 0);

	if (n == options)
		return;

	/* mark bootargs as valid */
	vmfb_boot_minor = minor;

	/* no more options */
	if (*n == '\0')
		return;

	/* path is seperated by comma */
	if (*n == ',') {
		n++;
		strlcpy(vmfb_path, n, sizeof(vmfb_path));
	}
}

static int __init vmfb_init(void)
{
	int ret = 0;

#ifndef MODULE
	char *option = NULL;

	if (fb_get_options(P4VM_DEV_NAME, &option))
		return -ENODEV;
	vmfb_setup(option);
#endif

	if (vmfb_boot_minor == -1) {
		printk(KERN_INFO P4VM_DEV_NAME ": No video= boot arg, ignoring.\n");
		return -ENXIO;
	}

	ret = platform_driver_register(&vmfb_driver);
	if (!ret) {
		platform_device_register_simple(P4VM_DEV_NAME, vmfb_boot_minor, NULL, 0);
	}

	return ret;
}

static void __exit vmfb_exit(void)
{
	platform_driver_unregister(&vmfb_driver);
}

module_init(vmfb_init);
module_exit(vmfb_exit);
