/*
 *  Defines and types to use virtual framebuffer
 * 
 *  Copyright (C) 2003, SYSGO AG
 * 
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#ifndef _VIRTUAL_FB_H
#define _VIRTUAL_FB_H

#define VM_IOC_PARAM_IN(n) (0x00008000u | (((n) & 0x7fu) << 8))
#define VM_IOC_CMD(cmd_id) (((cmd_id) & 0x0000ffffu) << 16)
#define VM_IOC_VOID_OUT 0u

#define VM_IOC_IN(cmd_id, type)                          \
                     (  VM_IOC_CMD(cmd_id)               \
                      | VM_IOC_PARAM_IN(sizeof(type))    \
                      | VM_IOC_VOID_OUT)

#define FBFP_IOCTL_COPYAREA	VM_IOC_IN(0x80, struct fb_copyarea)
#define FBFP_IOCTL_FILLRECT VM_IOC_IN(0x81, struct fb_fillrect)

/**
 * Framebuffer infos
 */
struct virtual_fb_info {

/** Keep this part synchronize with fbinfo struct from psp */	
	/* framebuffer mode */
	uint32_t	width;
	uint32_t	height;
	uint32_t	bpp;
	uint32_t	linelen;
	uint32_t	addr;
	uint32_t	size;
	uint32_t	flags;
	uint32_t	reserved;

	/* reg, green, blue, reserved position and sizes */
	uint8_t		r_size;
	uint8_t		r_pos;
	uint8_t		g_size;
	uint8_t		g_pos;
	uint8_t		b_size;
	uint8_t		b_pos;
	uint8_t		x_size;
	uint8_t		x_pos;

/** end of psp fbinfo struct */

	/* is acceleration supported */
	uint8_t		accel;
};

#endif /* _VIRTUAL_FB_H_ */
