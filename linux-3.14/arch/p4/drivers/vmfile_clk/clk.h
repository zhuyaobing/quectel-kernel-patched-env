#ifndef __VMFILE_CLK_CLK_H
#define __VMFILE_CLK_CLK_H
/*
 * Copyright (C) 2016 OpenSynergy GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <vm.h>

#include <linux/clk-provider.h>

struct clk *p4_register_clk(struct device *dev, const char *name,
			    const char **parent_names, int num_parents,
			    vm_file_desc_t *vclk_fd, unsigned long flags);
#endif
