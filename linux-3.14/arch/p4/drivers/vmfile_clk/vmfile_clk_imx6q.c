/*
 * Copyright (C) 2016 OpenSynergy GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/clk-provider.h>
#include <linux/clkdev.h>

#include <vm.h>
#include <asm/vmapi_calls.h>

#include <drv/drv_clock.h>

#include "clk.h"

/* i.MX6 specific clock bindings. Device tree setup needs this... */
#include <dt-bindings/clock/imx6qdl-clock.h>

#define FILE_NAME_DEFAULT "imx_clk_fp:/vclock"

/* PikeOS file descriptor of the virtual clock driver */
static vm_file_desc_t vclk_fd;

static struct clk *clk[IMX6QDL_CLK_END];
static struct clk_onecell_data clk_data;

static void __init p4_clocks_init(struct device_node *np)
{
	vm_e_t p4_err;
	int error;
	const char *filename;

	error = of_property_read_string(np, "provider-filename", &filename);
	if (error < 0) {
		pr_warn("could not read clock file provider name from device tree: Using %s",
			FILE_NAME_DEFAULT);
		filename = FILE_NAME_DEFAULT;
	}

	p4_err = vm_open(filename, VM_O_RW, &vclk_fd);
	if (p4_err != VM_E_OK) {
		pr_err("could not open %s: Clock tree virtualization not available\n",
			filename);
		return;
	}

	pr_info("Use clock tree virtualization with %s\n", filename);

	clk[IMX6QDL_CLK_USDHC1] = p4_register_clk(NULL, "usdhc1", NULL, 0, &vclk_fd, 0);
	clk[IMX6QDL_CLK_USDHC2] = p4_register_clk(NULL, "usdhc2", NULL, 0, &vclk_fd, 0);
	clk[IMX6QDL_CLK_USDHC3] = p4_register_clk(NULL, "usdhc3", NULL, 0, &vclk_fd, 0);
	clk[IMX6QDL_CLK_USDHC4] = p4_register_clk(NULL, "usdhc4", NULL, 0, &vclk_fd, 0);

	clk[IMX6QDL_CLK_I2C1] = p4_register_clk(NULL, "i2c1", NULL, 0, &vclk_fd, 0);
	clk[IMX6QDL_CLK_I2C2] = p4_register_clk(NULL, "i2c2", NULL, 0, &vclk_fd, 0);
	clk[IMX6QDL_CLK_I2C3] = p4_register_clk(NULL, "i2c3", NULL, 0, &vclk_fd, 0);

	clk[IMX6QDL_CLK_USBPHY1] = p4_register_clk(NULL, "usbphy1", NULL, 0, &vclk_fd, 0);
	clk[IMX6QDL_CLK_USBPHY2] = p4_register_clk(NULL, "usbphy2", NULL, 0, &vclk_fd, 0);

	clk[IMX6QDL_CLK_USBOH3] = p4_register_clk(NULL, "usboh3", NULL, 0, &vclk_fd, 0);

	clk[IMX6QDL_CLK_UART_IPG] = p4_register_clk(NULL, "uart_ipg", NULL, 0, &vclk_fd, 0);
	clk[IMX6QDL_CLK_UART_SERIAL] = p4_register_clk(NULL, "uart_serial", NULL, 0, &vclk_fd, 0);

	clk[IMX6QDL_CLK_ENET] = p4_register_clk(NULL, "enet", NULL, 0, &vclk_fd, 0);
	clk[IMX6QDL_CLK_ENET_REF] = p4_register_clk(NULL, "enet_ref", NULL, 0, &vclk_fd, 0);

	clk[IMX6QDL_CLK_ECSPI3] = p4_register_clk(NULL, "ecspi3", NULL, 0, &vclk_fd, 0);

	clk_data.clks = clk;
	clk_data.clk_num = ARRAY_SIZE(clk);
	of_clk_add_provider(np, of_clk_src_onecell_get, &clk_data);
}
CLK_OF_DECLARE(imx6q, "opsy,p4-clk-imx6q", p4_clocks_init);
