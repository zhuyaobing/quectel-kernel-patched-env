/*
 *  P4VM virtual framebuffer based on PikeOS vfb file provider
 * 
 *  Author: Bertrand Marquis <bma@sysgo.fr>
 *          David Engraf <den@sysgo.com>
 * 
 *  Copyright (C) 2012 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <asm/vmapi_calls.h>

#include "virtual-fb.h"

#define P4VM_DEV_NAME	"vmfile_fb"     /* device name */

MODULE_AUTHOR("Bertrand Marquis <bma@sysgo.fr>");
MODULE_DESCRIPTION("PikeOS Virtual Framebuffer driver");
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
	.id	= "PIKEOS VFB",
	.type	= FB_TYPE_PACKED_PIXELS,
	.accel	= FB_ACCEL_NONE,
};

struct vmfb_par {
	int handle;
	u32 pseudo_pal[256];
};

/* --------------------------------------------------------------------- */

static void vmfb_fillrect(struct fb_info *info, const struct fb_fillrect *rect)
{
	int rc;
	struct vmfb_par *vmfb_par;

	vmfb_par = info->par;
	rc = vmapi_file_ioctl(vmfb_par->handle, 
				FBFP_IOCTL_FILLRECT, (void *)rect, -1, -1);
	if ( rc != 0)
	{
		dev_err(info->dev, "Error during ioctl: %d\n", rc);
		cfb_fillrect(info, rect);
	}
}

static void vmfb_copyarea(struct fb_info *info, const struct fb_copyarea *region)
{
	int rc;
	struct vmfb_par *vmfb_par;

	vmfb_par = info->par;
	rc =  vmapi_file_ioctl(vmfb_par->handle, 
				FBFP_IOCTL_COPYAREA, (void *)region, -1, -1);
	if (rc != 0)
	{
		dev_err(info->dev, "Error during ioctl: %d\n", rc);
		cfb_copyarea(info, region);
	}
}

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

static struct fb_ops vmfb_ops = {
	.owner		= THIS_MODULE,
	.fb_setcolreg	= vmfb_setcolreg,
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
};

static int vmfb_probe(struct platform_device *dev)
{
	int err;
	unsigned int size_vmode;
	unsigned int size_remap;
	unsigned int size_total;
	char devname[32];
	struct fb_info *info;
	struct vmfb_par *vmfb_par;
	struct virtual_fb_info p4fbinfo;

	info = framebuffer_alloc(sizeof(vmfb_par), &dev->dev);
	if (!info) {
		return -ENOMEM;
	}
	vmfb_par = info->par;

	snprintf(devname, sizeof(devname), "vfb:/%d/fb", dev->id);

	vmfb_par->handle = vmapi_file_open4(devname, VMAPI_O_R, NULL, NULL);
	if (vmfb_par->handle < 0)
	{
		dev_err(&dev->dev, "Error opening %s (%d)\n", devname, vmfb_par->handle);
		framebuffer_release(info);
		return -ENODEV;
	}

	/** read the p4fb struct from driver */
	err = vmapi_file_read(vmfb_par->handle, &p4fbinfo, sizeof(struct virtual_fb_info));

	if (err != sizeof(struct virtual_fb_info)) {
		dev_err(&dev->dev, "Error when reading fbinfo\n");
		err = -EIO;
		goto error_close;
	}

	if (p4fbinfo.bpp == 0) {
		/** for debug, only keyboard activated */
		return 0;
	}

	if (p4fbinfo.bpp <= 8) {
		dev_err(&dev->dev, "8 bit mode not supported\n");
		err=-ENODEV;
		goto error_close;
	}

	vmfb_fix.smem_start = p4fbinfo.addr;
	vmfb_defined.bits_per_pixel = p4fbinfo.bpp;
	if (15 == vmfb_defined.bits_per_pixel)
		vmfb_defined.bits_per_pixel = 16;
	vmfb_defined.xres = p4fbinfo.width;
	vmfb_defined.yres = p4fbinfo.height;
	vmfb_fix.line_length = p4fbinfo.linelen;
	vmfb_fix.visual   = FB_VISUAL_TRUECOLOR;

	/*   size_vmode -- that is the amount of memory needed for the
	 *                 used video mode, i.e. the minimum amount of
	 *                 memory we need. */
	size_vmode = vmfb_defined.yres * vmfb_fix.line_length;

	/*   size_total -- all video memory we have. Used for mtrr
	 *                 entries, ressource allocation and bounds
	 *                 checking. */
	size_total = p4fbinfo.size & 0x10000;
	if (size_total < size_vmode)
		size_total = size_vmode;

	/*   size_remap -- the amount of video memory we are going to
	 *                 use for vesafb.  With modern cards it is no
	 *                 option to simply use size_total as that
	 *                 wastes plenty of kernel address space. */
	size_remap  = size_vmode * 2;
	if (size_remap < size_vmode)
		size_remap = size_vmode;
	if (size_remap > size_total)
		size_remap = size_total;
	vmfb_fix.smem_len = size_remap;

	if (!request_mem_region(vmfb_fix.smem_start, size_total, "vfb")) {
		dev_err(&dev->dev, "cannot reserve video memory at 0x%lx\n",
			vmfb_fix.smem_start);
		/* We cannot make this fatal. Sometimes this comes from magic
		   spaces our resource handlers simply don't know about */
	}

	info->pseudo_palette = vmfb_par->pseudo_pal;

	info->screen_base = ioremap(vmfb_fix.smem_start, vmfb_fix.smem_len);
	if (!info->screen_base) {
		dev_err(&dev->dev,
			"abort, cannot ioremap video memory 0x%x @ 0x%lx\n",
			vmfb_fix.smem_len, vmfb_fix.smem_start);
		err = -EIO;
		goto error_release;
	}

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
	if (p4fbinfo.accel != 0)
	{
		vmfb_ops.fb_fillrect = vmfb_fillrect;
		vmfb_ops.fb_copyarea = vmfb_copyarea;
		info->flags = FBINFO_DEFAULT | FBINFO_HWACCEL_COPYAREA | FBINFO_HWACCEL_FILLRECT;
	}
	else
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
	       "fb%d: framebuffer at 0x%lx, mapped to 0x%p, using %dk, total %dk\n",
	       info->node,
	       vmfb_fix.smem_start, info->screen_base,
	       size_remap/1024, size_total/1024);
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
	printk(KERN_INFO "fb%d: %s frame buffer device (accel %d)\n",
	       info->node, info->fix.id, p4fbinfo.accel);
	return 0;

error_release:
	release_mem_region(vmfb_fix.smem_start, size_total);
error_close:
	vmapi_file_close(vmfb_par->handle);
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
		vmapi_file_close(vmfb_par->handle);
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
 * passed (video=vmfile_fb:0).
 */
static void __init vmfb_setup(char *options)
{
	int minor;
	char *n;

	if (!options)
		return;

	/* get first substring: minor(int) */
	minor = simple_strtol(options, &n, 0);

	if (n == options || *n != '\0')
		return;

	/* mark bootargs as valid */
	vmfb_boot_minor = minor;
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
