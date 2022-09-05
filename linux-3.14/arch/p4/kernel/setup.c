/* Copyright (c) 2016 OpenSynergy GmbH */
/*
 * linux/arch/p4/kernel/setup.c
 *
 * Copyright (C) 2000, 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 *
 * Copyright (C) 2003 - 2009 Alex Zuepke, SYSGO AG (azu@sysgo.de)
 * Merged from various User Mode Linux files
 *
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/init.h>
#include <linux/bootmem.h>
#include <linux/console.h>
#include <linux/screen_info.h>
#include <linux/module.h>

#ifdef CONFIG_VGA_CONSOLE
#include <video/vga.h>
#endif

#include <asm/internals.h>
#include <asm/vmapi_calls.h>
#include <asm/host_threads.h>
#include <asm/host_tasks.h>
#include <asm/kernthreads.h>
#include <asm/iomem.h>
#include <asm/host_exception.h>	/* BREAKPOINT */
#include <asm/setup.h>
#include <asm/internals.h>
#include <asm/debugout.h>
#include <asm/memory.h>


/* screen information */
struct screen_info screen_info;
EXPORT_SYMBOL(screen_info);

unsigned char aux_device_present;


/* Set in early boot */
unsigned long start_vm;
EXPORT_SYMBOL(start_vm);
unsigned long consistent_start;

/* Set in early boot */
int ncpus = 1;

/* Linux command line, set in early boot */
static char __initdata command_line[COMMAND_LINE_SIZE];
/* Temp while assembling the command line, too large to keep on stack */
static char __initdata command_line_tmp[COMMAND_LINE_SIZE];

static void __init get_command_line(void)
{
	char *begin, *end;
	int remain;
	int len;
	int fd;
	int i;

	/* get command line args from VMIT */
	vmapi_get_cmdline(command_line, COMMAND_LINE_SIZE);
	if (strlen(command_line) >= COMMAND_LINE_SIZE - 1)
		goto out;

	/*
	 * check if there is a "params=" substring,
	 * and read the additional commands from the specified file
	 */
	strcpy(command_line_tmp, command_line);
	begin = strstr(command_line_tmp, "params=");
	if (!begin)
		goto out;

	/* jump over params= */
	begin += 7;

	/* terminate string at the next space */
	end = strstr(begin, " ");
	if (end)
		*end = 0;

	fd = vmapi_file_open4(begin, VMAPI_O_R, NULL, NULL);
	if (fd < 0) {
		debugout("get_command_line: could not open param file %s\n", begin);
		goto out;
	}

	remain = COMMAND_LINE_SIZE - strlen(command_line) - 2;
	len = vmapi_file_read(fd, command_line_tmp, remain);
	if (len > 0) {
		command_line_tmp[len] = 0;

		/* filter string: \n and \r */
		for (i = 0; i < len; i++)
			if (command_line_tmp[i] == '\n' || command_line_tmp[i] == '\r')
				command_line_tmp[i] = ' ';

		/* strip spaces at the end */
		while (len > 0 && command_line_tmp[len-1] == ' ') {
			command_line_tmp[len-1] = 0;
			len--;
		}

		/* append additional commands */
		strcat(command_line, " ");
		strcat(command_line, command_line_tmp);
	}
	vmapi_file_close(fd);

out:
	/* keep backup copy in boot_command_line */
	strcpy(boot_command_line, command_line);
}

/*
 * Get access rights for all interrupts:
 * The interrupt access rights are always available in PikeOS <3.0
 * For Pikeos >= 3.0, we map them at kernel startup time from the
 * partitions property set:
 *  prop:/app/<partitionname>/irqs/irq%d in the range 0 .. 511
 */
static void __init get_linux_irqs(void)
{
	char propname[96];
	int i, err;


	for (i = 0; i < 512; i++)
	{
		sprintf(propname, "prop:/app/%s/irqs/irq%d",
			vmapi_get_partname(), i);
		err = vmapi_property_map_irq(propname);
	}
}

static unsigned int get_screen_lines(void)
{
#ifdef CONFIG_VGA_CONSOLE
	unsigned int fsr;

	outb_p(0x09, VGA_CRT_IC);	/* Font size register */
	fsr = inb_p(VGA_CRT_DC);
	if ((fsr & 0xf) == 8 - 1)	/* 8 lines character size? */
		return 50;
#endif

	return 25;			/* default */
}

/*
 * setup framebuffer for VESA modes
 */
static void __init setup_screen_info(void)
{
	unsigned int lines;

	lines = get_screen_lines();

	/* setup for VGA (may be overwritten when VESA is configured) */
	screen_info.orig_y = lines - 1;
	screen_info.orig_video_cols = 80;
	screen_info.orig_video_lines = lines;
	screen_info.orig_video_isVGA = VIDEO_TYPE_VGAC;
	screen_info.orig_video_points = 16;

#if defined(CONFIG_FB_VESA) && defined(CONFIG_P4_X86)
	/* check for already running VESA framebuffer */
	{
		struct fb_info {
			/* framebuffer mode */
			u32	width;
			u32	height;
			u32	bpp;
			u32	linelen;
			u32	addr;
			u32	size;
			u32	flags;
			u32	reserved;

			/* reg, green, blue, reserved position and sizes */
			u8	r_size;
			u8	r_pos;
			u8	g_size;
			u8	g_pos;
			u8	b_size;
			u8	b_pos;
			u8	x_size;
			u8	x_pos;
		} *fbi;

		fbi = try_phys_to_virt(0x600);
		/* not mapped */
		if (!fbi) {
			vmapi_cprintf("P4Linux: VESA settings not mapped (0x0)\n");
			return;
		}

		/* not initialized */
		if (fbi->width == 0) {
			vmapi_cprintf("P4Linux: VESA not initialized (PSP_VESA_MODE)\n");
			return;
		}

		if (iomem_find_io(fbi->addr, fbi->size) == 0) {
			vmapi_cprintf("P4Linux: VESA video memory not mapped (0x%x, size 0x%x)\n",
				      fbi->addr, fbi->size);
			return;
		}

		/* copy values */
		screen_info.orig_video_isVGA = VIDEO_TYPE_VLFB;

		screen_info.lfb_width      = fbi->width;
		screen_info.lfb_height     = fbi->height;
		screen_info.lfb_depth      = fbi->bpp;
		screen_info.lfb_base       = fbi->addr;
		screen_info.lfb_size       = fbi->size >> 16;
		screen_info.lfb_linelength = fbi->linelen;

		screen_info.red_size       = fbi->r_size;
		screen_info.red_pos        = fbi->r_pos;
		screen_info.green_size     = fbi->g_size;
		screen_info.green_pos      = fbi->g_pos;
		screen_info.blue_size      = fbi->b_size;
		screen_info.blue_pos       = fbi->b_pos;
		screen_info.rsvd_size      = fbi->x_size;
		screen_info.rsvd_pos       = fbi->x_pos;

		vmapi_cprintf("P4Linux: Using PikeOS VESA configuration: %dx%dx%d, addr 0x%x\n",
			screen_info.lfb_width, screen_info.lfb_height,
			screen_info.lfb_depth, screen_info.lfb_base);
	}

	{
		unsigned long xsize, ysize, xoffs, yoffs;
		unsigned long offs;
		char *options;

		/* check if there is a "vesapart=" substring */
		options = strstr(command_line, "vesapart=");
		if (!options)
			return;
		options += 9;

		/* option string is formatted <xsize>x<ysize>@<xoffs>x<yoffs> */
		xsize = simple_strtoul(options, &options, 10);
		if (*options++ != 'x')
			return;
		ysize = simple_strtoul(options, &options, 10);
		if (*options++ != '@')
			return;
		xoffs = simple_strtoul(options, &options, 10);
		if (*options++ != 'x')
			return;
		yoffs = simple_strtoul(options, &options, 10);

		offs = (yoffs * screen_info.lfb_linelength) + (xoffs * (screen_info.lfb_depth / 8));

#if 0
		debugout("vesapart: desired video mode: %dx%d@%dx%d\n", xsize, ysize, xoffs, yoffs);
		debugout("vesapart: lfb_width=%d\n", screen_info.lfb_width);
		debugout("vesapart: lfb_height=%d\n", screen_info.lfb_height);
		debugout("vesapart: lfb_depth=%d\n", screen_info.lfb_depth);
		debugout("vesapart: lfb_linelength=%d\n", screen_info.lfb_linelength);
		debugout("vesapart: offset=%d\n", offs);
#endif

		/* adjust start of frame buffer, width and height */
		screen_info.lfb_base += offs;
		screen_info.lfb_size -= offs;
		screen_info.lfb_width = xsize;
		screen_info.lfb_height = ysize;
	}
#endif
}

void __init linux_main(void)
{
	unsigned int uk_version;

#ifdef CONFIG_INITIAL_BRKPT
	/* initial breakpoint */
	BREAKPOINT();
#endif

	/* init VM API library */
	vmapi_init();

	/* version check */
	/* keep in sync with version check in linux/arch/p4/kernel/version.c */
	uk_version = uk_task_kinfo_api_version();
	if (uk_version < 0x30005)
		die_early("PikeOS versions < 3.3 are not supported");

	uk_task_init();

	/* define current thread */
	early_init_process_setup();

	/* enable locking */
	kthreads_init();

	/*
	 * parse param tables for usable RAM
	 * and setup boot mem allocator
	 */
	iomem_init();
	setup_boot_memory();

	/*
	 * allocate IO-memory lookup tables
	 */
	iomem_fill_table(&consistent_start);
	/* iomem_dump_table(); */

	/* initialize file subsystem */
	vmapi_fs_init();

	/* initialize queueing port subsystem */
	vmapi_qport_init();
	vmapi_sport_init();

	mmu_task_init();

	get_command_line();

	get_linux_irqs();

	setup_screen_info();

	start_kernel_thread();

	/* become exception handler for all linux kernel threads */

	/* The PikeOS kernel starts us with FPU enabled, and we use
	 * this function below to disable the FPU. arch_dojmp() cannot do this.
	 */
	kthreads_call_on_new_stack(kernel_exception_handler);
	/* never returns */
}


void __init setup_arch(char **cmdline_p)
{
	parse_early_param();

	setup_sub_arch();

#ifdef CONFIG_BSP
	/* init BSP (if any) */
	bsp_init();
#endif

#ifdef CONFIG_BUILTIN_TEST
	do_builtin_test();
#endif

	/*
	 * vmalloc area starts behind consistent memory area
	 */
	start_vm = consistent_start + consistent_size;

	paging_init();
	*cmdline_p = command_line;

#ifdef CONFIG_VT
#if defined(CONFIG_VGA_CONSOLE)
	conswitchp = &vga_con;
#elif defined(CONFIG_DUMMY_CONSOLE)
	conswitchp = &dummy_con;
#endif
#endif

}
