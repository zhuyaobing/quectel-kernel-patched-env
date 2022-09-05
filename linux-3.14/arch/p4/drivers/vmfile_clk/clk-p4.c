/*
 * Copyright (C) 2015-2016 OpenSynergy GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#define pr_fmt(fmt) "p4-clk: " fmt

#include <linux/clk-provider.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/string.h>
#include <linux/kernel.h>

#include <vm.h>
#include <asm/vmapi_calls.h>

#include <drv/drv_clock.h>

#include <clk.h>

/*
 * DOC: P4 shared clock client clock with gating support
 *
 * This clock forwards all requests to the P4 clock driver server.
 *
 * Traits of this clock:
 * rate - always request rate from server (no cache)
 * parent - no parent support. All p4 clocks are root clocks
 */

struct clk_p4 {
	struct clk_hw hw;
	vm_file_desc_t *fd;
};

#define to_clk_p4(_hw) container_of(_hw, struct clk_p4, hw)

/*
 * Issue P4 IOCTL command function for the clock with the given clk_name
 *
 * Helper function for setting up the IOCTL data and translating PikeOS error
 * codes to Linux error codes.
 */
static int clk_p4_op(struct p4_clk *data, vm_file_desc_t *fd,
		     const char *clk_name, int function)
{
	vm_e_t err;
	size_t len;
	unsigned int cmd;
	struct p4_clk_ioctl_in *clk_info = &data->ioctl.info_in;

	cmd = VM_IOC_INOUT(0, struct p4_clk_ioctl_in, struct p4_clk_ioctl_out);

	clk_info->function_num = function;
	len = strlcpy(clk_info->name, clk_name, CLK_NAME_LEN);
	if (len > CLK_NAME_LEN - 1)
		pr_warn_once("clock name %s too long: truncated to %d characters",
			     clk_name, CLK_NAME_LEN - 1);

	err = vm_ioctl(fd, cmd, data);
	if (err == VM_E_INVAL_PARAM) {
		return -EINVAL;
	} else if (err == VM_E_PERM) {
		return -EPERM;
	} else if (err != VM_E_OK) {
		pr_err("IOCTL for clock %s failed: err = %d\n", clk_name, err);
		return err;
	}

	return 0;
}

static int clk_p4_prepare(struct clk_hw *hw)
{
	int err;
	struct p4_clk data;
	struct clk_p4 *clk = to_clk_p4(hw);
	const char *clk_name = __clk_get_name(hw->clk);

	pr_debug("%s(%s)\n", __func__, clk_name);

	err = clk_p4_op(&data, clk->fd, clk_name, CLK_PREPARE);
	if (err < 0) {
		pr_err("failed to prepare clock %s\n", clk_name);
		return err;
	}

	/* p4 driver casts original int return value to unsigned long */
	err = (int) data.ioctl.info_out.args.par;
	if (err < 0)
		pr_err("imx_clk_fp failed to prepare clock %s\n", clk_name);

	return err;
}

static void clk_p4_unprepare(struct clk_hw *hw)
{
	int err;
	struct p4_clk data;
	struct clk_p4 *clk = to_clk_p4(hw);
	const char *clk_name = __clk_get_name(hw->clk);

	pr_debug("%s(%s)\n", __func__, clk_name);

	err = clk_p4_op(&data, clk->fd, clk_name, CLK_UNPREPARE);
	if (err < 0)
		pr_warn("failed to unprepare clock %s\n", clk_name);
}

static int clk_p4_enable(struct clk_hw *hw)
{
	int err;
	struct p4_clk data;
	struct clk_p4 *clk = to_clk_p4(hw);
	const char *clk_name = __clk_get_name(hw->clk);

	pr_debug("%s(%s)\n", __func__, clk_name);

	err = clk_p4_op(&data, clk->fd, clk_name, CLK_ENABLE);
	if (err < 0) {
		pr_err("failed to enable clock %s\n", clk_name);
		return err;
	}

	/* p4 driver casts original int return value to unsigned long */
	err = (int) data.ioctl.info_out.args.par;
	if (err < 0)
		pr_err("imx_clk_fp failed to enable clock %s\n", clk_name);

	return err;
}

static void clk_p4_disable(struct clk_hw *hw)
{
	int err;
	struct p4_clk data;
	struct clk_p4 *clk = to_clk_p4(hw);
	const char *clk_name = __clk_get_name(hw->clk);

	pr_debug("%s(%s)\n", __func__, clk_name);

	err = clk_p4_op(&data, clk->fd, clk_name, CLK_DISABLE);
	if (err < 0)
		pr_warn("failed to disable clock %s\n", clk_name);
}

static unsigned long clk_p4_recalc_rate(struct clk_hw *hw,
					unsigned long parent_rate)
{
	int err;
	struct p4_clk data;
	struct clk_p4 *clk = to_clk_p4(hw);
	const char *clk_name = __clk_get_name(hw->clk);

	pr_debug("%s(%s, %lu)\n", __func__, clk_name, parent_rate);

	err = clk_p4_op(&data, clk->fd, clk_name, CLK_GET_RATE);
	if (err < 0) {
		pr_err("failed to get rate of clock %s\n", clk_name);
		return 0;
	}

	return data.ioctl.info_out.args.par;
}

static long clk_p4_round_rate(struct clk_hw *hw, unsigned long rate,
			      unsigned long *parent_rate)
{
	int err;
	struct p4_clk data;
	struct clk_p4 *clk = to_clk_p4(hw);
	const char *clk_name = __clk_get_name(hw->clk);

	pr_debug("%s(%s, %lu, %lu)\n", __func__, clk_name, rate, *parent_rate);

	data.ioctl.info_in.args.par = rate;

	err = clk_p4_op(&data, clk->fd, clk_name, CLK_ROUND_RATE);
	if (err < 0) {
		pr_err("failed to round rate of clock %s\n", clk_name);
		return 0;
	}

	/* p4 driver casts original long return value to unsigned long */
	return (long) data.ioctl.info_out.args.par;
}

static int clk_p4_set_rate(struct clk_hw *hw, unsigned long rate,
			   unsigned long parent_rate)
{
	int err;
	struct p4_clk data;
	struct clk_p4 *clk = to_clk_p4(hw);
	const char *clk_name = __clk_get_name(hw->clk);

	pr_debug("%s(%s, %lu, %lu)\n", __func__, clk_name, rate, parent_rate);

	data.ioctl.info_in.args.par = rate;

	err = clk_p4_op(&data, clk->fd, clk_name, CLK_SET_RATE);
	if (err < 0) {
		pr_err("failed to set rate of clock %s\n", clk_name);
		return err;
	}

	/* p4 driver casts original int return value to unsigned long */
	err = (int) data.ioctl.info_out.args.par;
	if (err < 0) {
		pr_err("imx_clk_fp failed to set rate of clock %s\n", clk_name);
		return err;
	}

	return 0;
}

const struct clk_ops clk_p4_ops = {
	.prepare = clk_p4_prepare,
	.unprepare = clk_p4_unprepare,
	.enable = clk_p4_enable,
	.disable = clk_p4_disable,
	.recalc_rate = clk_p4_recalc_rate,
	.round_rate = clk_p4_round_rate,
	.set_rate = clk_p4_set_rate,
};
EXPORT_SYMBOL_GPL(clk_p4_ops);

/**
 * p4_register_clk - register a P4 virtualized clock with the clock framework
 *
 * @dev: device registering this clock
 * @name: name of this clock
 * @parent_names: names of clock's parent (NULL if root clock)
 * @num_parents: number of clock's parents (0 if root clock)
 * @vclk_fd: opened file descriptor of file provider
 * @flags: framework-specific flags
 */
struct clk *p4_register_clk(struct device *dev, const char *name,
			    const char **parent_names, int num_parents,
			    vm_file_desc_t *vclk_fd, unsigned long flags)
{
	struct clk_p4 *clk_p4;
	struct clk *clk;
	struct clk_init_data init;

	/* allocate the gate */
	clk_p4 = kzalloc(sizeof(*clk_p4), GFP_KERNEL);
	if (!clk_p4) {
		pr_err("%s: could not allocate p4 clk\n", __func__);
		return ERR_PTR(-ENOMEM);
	}

	init.name = name;
	init.ops = &clk_p4_ops;
	init.flags = flags | CLK_GET_RATE_NOCACHE | CLK_IS_ROOT;
	init.parent_names = parent_names;
	init.num_parents = num_parents;

	clk_p4->hw.init = &init;

	clk_p4->fd = vclk_fd;

	clk = clk_register(dev, &clk_p4->hw);

	if (IS_ERR(clk))
		kfree(clk_p4);

	return clk;
}
