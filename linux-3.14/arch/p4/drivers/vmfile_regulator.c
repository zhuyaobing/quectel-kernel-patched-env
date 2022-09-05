/*
 * Copyright (C) 2008-2016 OpenSynergy GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/slab.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/mfd/syscon.h>
#include <linux/regulator/machine.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/regmap.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/of_regulator.h>

#include <linux/string.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <vm.h>
#include <asm/vmapi_calls.h>
#include <drv/drv_pow_reg.h>

static vm_file_desc_t vreg_fd;
#define PREG_FP_FILE "powreg_fp:/regulator"

/* enumerate supported voltages */
int p4_reg_list_voltage (struct regulator_dev *reg, unsigned selector)
{
	int ret = -EINVAL;
	struct p4_regulator vreg;
	vm_e_t rc;

	strcpy(vreg.ioctl.info_in.name, reg->desc->name);
	vreg.ioctl.info_in.function_num = PREG_LIST_VOLTAGE;
	vreg.ioctl.info_in.args.list_voltage.selector = selector;

	rc = vm_ioctl(&vreg_fd, VM_IOC_INOUT(0, struct p4_regulator_ioctl_in, struct p4_regulator_ioctl_out), &vreg);

	if(rc != VM_E_OK){
		printk("%s: vm_ioctl()failed with rc = 0x%02x\n", __func__, rc);
		return ret;
	}

	ret = vreg.ioctl.info_out.return_value;

	return ret;
};

/* get/set regulator voltage */
int p4_reg_set_voltage (struct regulator_dev *reg, int min_uV, int max_uV,
		    unsigned *selector)
{
	int ret = -EINVAL;
	struct p4_regulator vreg;
	vm_e_t rc;

	strcpy(vreg.ioctl.info_in.name, reg->desc->name);
	vreg.ioctl.info_in.function_num = PREG_SET_VOLTAGE;
	vreg.ioctl.info_in.args.set_voltage.min_uV = min_uV;
	vreg.ioctl.info_in.args.set_voltage.max_uV = max_uV;

	rc = vm_ioctl(&vreg_fd, VM_IOC_INOUT(0, struct p4_regulator_ioctl_in, struct p4_regulator_ioctl_out), &vreg);

	if(rc != VM_E_OK){
		printk("%s: vm_ioctl()failed with rc = 0x%02x\n", __func__, rc);
		return ret;
	}

	*selector = vreg.ioctl.info_out.selector;
	ret = vreg.ioctl.info_out.return_value;

	return ret;
};

int p4_reg_get_voltage (struct regulator_dev *reg)
{
	int ret = -EINVAL;
	struct p4_regulator vreg;
	vm_e_t rc;

	strcpy(vreg.ioctl.info_in.name, reg->desc->name);
	vreg.ioctl.info_in.function_num = PREG_GET_VOLTAGE;

	rc = vm_ioctl(&vreg_fd, VM_IOC_INOUT(0, struct p4_regulator_ioctl_in, struct p4_regulator_ioctl_out), &vreg);

	if(rc != VM_E_OK){
		printk("%s: vm_ioctl()failed with rc = 0x%02x\n", __func__, rc);
		return ret;
	}

	ret = vreg.ioctl.info_out.return_value;

	return ret;
};
/* get/set regulator current  */
int p4_reg_set_current_limit (struct regulator_dev *reg,
			 int min_uA, int max_uA)
{
	int ret = -EINVAL;
	struct p4_regulator vreg;
	vm_e_t rc;

	strcpy(vreg.ioctl.info_in.name, reg->desc->name);
	vreg.ioctl.info_in.function_num = PREG_SET_CURRENT_LIMIT;
	vreg.ioctl.info_in.args.set_current_limit.min_uA = min_uA;
	vreg.ioctl.info_in.args.set_current_limit.max_uA = max_uA;

	rc = vm_ioctl(&vreg_fd, VM_IOC_INOUT(0, struct p4_regulator_ioctl_in, struct p4_regulator_ioctl_out), &vreg);

	if(rc != VM_E_OK){
		printk("%s: vm_ioctl()failed with rc = 0x%02x\n", __func__, rc);
		return ret;
	}

	ret = vreg.ioctl.info_out.return_value;

	return ret;
};

int p4_reg_get_current_limit (struct regulator_dev *reg)
{
	int ret = -EINVAL;
	struct p4_regulator vreg;
	vm_e_t rc;

	strcpy(vreg.ioctl.info_in.name, reg->desc->name);
	vreg.ioctl.info_in.function_num = PREG_GET_CURRENT_LIMIT;

	rc = vm_ioctl(&vreg_fd, VM_IOC_INOUT(0, struct p4_regulator_ioctl_in, struct p4_regulator_ioctl_out), &vreg);

	if(rc != VM_E_OK){
		printk("%s: vm_ioctl()failed with rc = 0x%02x\n", __func__, rc);
		return ret;
	}

	ret = vreg.ioctl.info_out.return_value;

	return ret;
};

/* enable/disable regulator */
int p4_reg_enable (struct regulator_dev *reg)
{
	int ret = -EINVAL;
	struct p4_regulator vreg;
	vm_e_t rc;

	strcpy(vreg.ioctl.info_in.name, reg->desc->name);
	vreg.ioctl.info_in.function_num = PREG_ENABLE;

	rc = vm_ioctl(&vreg_fd, VM_IOC_INOUT(0, struct p4_regulator_ioctl_in, struct p4_regulator_ioctl_out), &vreg);

	if(rc != VM_E_OK){
		printk("%s: vm_ioctl()failed with rc = 0x%02x\n", __func__, rc);
		return ret;
	}

	ret = vreg.ioctl.info_out.return_value;

	return ret;
};
int p4_reg_disable (struct regulator_dev *reg)
{
	int ret = -EINVAL;
	struct p4_regulator vreg;
	vm_e_t rc;

	strcpy(vreg.ioctl.info_in.name, reg->desc->name);
	vreg.ioctl.info_in.function_num = PREG_DISABLE;

	rc = vm_ioctl(&vreg_fd, VM_IOC_INOUT(0, struct p4_regulator_ioctl_in, struct p4_regulator_ioctl_out), &vreg);

	if(rc != VM_E_OK){
		printk("%s: vm_ioctl()failed with rc = 0x%02x\n", __func__, rc);
		return ret;
	}

	ret = vreg.ioctl.info_out.return_value;

	return ret;
};
int p4_reg_is_enabled (struct regulator_dev *reg)
{
	int ret = -EINVAL;
	struct p4_regulator vreg;
	vm_e_t rc;

	strcpy(vreg.ioctl.info_in.name, reg->desc->name);
	vreg.ioctl.info_in.function_num = PREG_IS_ENABLED;

	rc = vm_ioctl(&vreg_fd, VM_IOC_INOUT(0, struct p4_regulator_ioctl_in, struct p4_regulator_ioctl_out), &vreg);

	if(rc != VM_E_OK){
		printk("%s: vm_ioctl()failed with rc = 0x%02x\n", __func__, rc);
		return ret;
	}

	ret = vreg.ioctl.info_out.return_value;

	return ret;
};
/* get/set regulator operating mode (defined in regulator.h) */
int p4_reg_set_mode (struct regulator_dev *reg, unsigned int mode)
{
	int ret = -EINVAL;
	struct p4_regulator vreg;
	vm_e_t rc;

	strcpy(vreg.ioctl.info_in.name, reg->desc->name);
	vreg.ioctl.info_in.function_num = PREG_SET_MODE;
	vreg.ioctl.info_in.args.set_mode.mode = mode;

	rc = vm_ioctl(&vreg_fd, VM_IOC_INOUT(0, struct p4_regulator_ioctl_in, struct p4_regulator_ioctl_out), &vreg);

	if(rc != VM_E_OK){
		printk("%s: vm_ioctl()failed with rc = 0x%02x\n", __func__, rc);
		return ret;
	}

	ret = vreg.ioctl.info_out.return_value;

	return ret;
};
unsigned int p4_reg_get_mode (struct regulator_dev *reg)
{
	int ret = -EINVAL;
	struct p4_regulator vreg;
	vm_e_t rc;

	strcpy(vreg.ioctl.info_in.name, reg->desc->name);
	vreg.ioctl.info_in.function_num = PREG_GET_MODE;

	rc = vm_ioctl(&vreg_fd, VM_IOC_INOUT(0, struct p4_regulator_ioctl_in, struct p4_regulator_ioctl_out), &vreg);

	if(rc != VM_E_OK){
		printk("%s: vm_ioctl()failed with rc = 0x%02x\n", __func__, rc);
		return ret;
	}

	ret = vreg.ioctl.info_out.return_value;

	return ret;
};
/* Time taken to enable or set voltage on the regulator */
int p4_reg_enable_time (struct regulator_dev *reg)
{
	int ret = -EINVAL;
	struct p4_regulator vreg;
	vm_e_t rc;

	strcpy(vreg.ioctl.info_in.name, reg->desc->name);
	vreg.ioctl.info_in.function_num = PREG_GET_ENABLE_TIME;

	rc = vm_ioctl(&vreg_fd, VM_IOC_INOUT(0, struct p4_regulator_ioctl_in, struct p4_regulator_ioctl_out), &vreg);

	if(rc != VM_E_OK){
		printk("%s: vm_ioctl()failed with rc = 0x%02x\n", __func__, rc);
		return ret;
	}

	ret = vreg.ioctl.info_out.return_value;

	return ret;
};
int p4_reg_set_voltage_time_sel (struct regulator_dev *reg,
			     unsigned int old_selector,
			     unsigned int new_selector){
	int ret = -EINVAL;
	struct p4_regulator vreg;
	vm_e_t rc;

	strcpy(vreg.ioctl.info_in.name, reg->desc->name);
	vreg.ioctl.info_in.function_num = PREG_SET_VOLTAGE_TIME_SEL;
	vreg.ioctl.info_in.args.set_voltage_time_sel.old_selector = old_selector;
	vreg.ioctl.info_in.args.set_voltage_time_sel.new_selector = new_selector;

	rc = vm_ioctl(&vreg_fd, VM_IOC_INOUT(0, struct p4_regulator_ioctl_in, struct p4_regulator_ioctl_out), &vreg);

	if(rc != VM_E_OK){
		printk("%s: vm_ioctl()failed with rc = 0x%02x\n", __func__, rc);
		return ret;
	}

	ret = vreg.ioctl.info_out.return_value;

	return ret;
};

int p4_reg_get_status(struct regulator_dev *reg)
{
	int ret = -EINVAL;
	struct p4_regulator vreg;
	vm_e_t rc;

	strcpy(vreg.ioctl.info_in.name, reg->desc->name);
	vreg.ioctl.info_in.function_num = PREG_GET_STATUS;

	rc = vm_ioctl(&vreg_fd, VM_IOC_INOUT(0, struct p4_regulator_ioctl_in, struct p4_regulator_ioctl_out), &vreg);

	if(rc != VM_E_OK){
		printk("%s: vm_ioctl()failed with rc = 0x%02x\n", __func__, rc);
		return ret;
	}

	ret = vreg.ioctl.info_out.return_value;

	return ret;
};

static struct regulator_ops p4_reg_ops = {
	.list_voltage = p4_reg_list_voltage,
	.set_voltage = p4_reg_set_voltage,
	.get_voltage = p4_reg_get_voltage,
	.set_current_limit = p4_reg_set_current_limit,
	.get_current_limit = p4_reg_get_current_limit,
	.enable = p4_reg_enable,
	.disable = p4_reg_disable,
	.is_enabled = p4_reg_is_enabled,
	.set_mode = p4_reg_set_mode,
	.get_mode = p4_reg_get_mode,
	.enable_time = p4_reg_enable_time,
	.set_voltage_time_sel = p4_reg_set_voltage_time_sel,
	.get_status = p4_reg_get_status,
};

#define PREG_PROPERTY_ENTRY "prop:/preg/part_0/"

#define PART_NUM_LOCATION (sizeof(PREG_PROPERTY_ENTRY) - sizeof("0/"))

#define NUM_OF_REGULATORS_PATH "num_of_regulators"

#define PREG_PATH "reg_00"
#define PREG_NUM_LOCATION (sizeof(PREG_PATH) - sizeof("00"))

#define SUPPLY_REG_NAME_PATH "/supply_reg"

#define CONSUMER_SUPPLY_PATH "/consumer_supply_00"
#define CONSUMER_SUPPLY_NUM_LOCATION (PREG_NUM_LOCATION + sizeof("00") - sizeof(char) + sizeof(CONSUMER_SUPPLY_PATH) - sizeof("00"))


static int p4_anatop_regulator_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct device_node *anatop_np;
	struct regulator_desc *p4_reg_desc;
	struct regulator_dev *rdev;
	struct regulator_init_data *p4_reg_initdata;
	struct regulator_config config = { };
	int ret = 0;
	int err;
	vm_file_desc_t prop_fd;
	vm_partition_stat_t partinfo;

	char prop_path[64];
	char p4_reg_name[32];
	unsigned int j, temp;
	struct regulator_consumer_supply *p4_reg_consumers;
	
	p4_reg_initdata = of_get_regulator_init_data(dev, np);

	anatop_np = of_get_parent(np);
	if (!anatop_np)
		return -ENODEV;
	of_node_put(anatop_np);
	p4_reg_desc = kzalloc(sizeof(struct regulator_desc), GFP_KERNEL);

	if (!p4_reg_desc){
		pr_err("Allocating memory for p4_reg_desc filed\n");
	}

	p4_reg_desc->id = -1,
	p4_reg_desc->type = REGULATOR_VOLTAGE,
	p4_reg_desc->owner = THIS_MODULE,
	p4_reg_desc->ops = &p4_reg_ops,
	p4_reg_desc->name = of_get_property(np, "regulator-name", NULL);


	err = vm_part_pstat(VM_RESPART_MYSELF, &partinfo);
	if (err != VM_E_OK)
		return -ENODEV;

	prop_path[0] = '\0';

	strcpy(prop_path, PREG_PROPERTY_ENTRY);

	prop_path[PART_NUM_LOCATION] += partinfo.id - 1;

	err = vm_open(prop_path, VM_O_RD, &prop_fd);

	if (err == VM_E_OK) {

		unsigned int i;
		unsigned int num_of_regulators = 0;

		strcpy(prop_path, NUM_OF_REGULATORS_PATH);

		ret = vm_prop_read(&prop_fd, prop_path, VM_PROP_T_UINT32,
				   sizeof(P4_uint32_t), &num_of_regulators, NULL, NULL);
		if(ret)
			pr_debug("%s: Enable to read %s ret %d\n", __func__, prop_path, ret);

		for(i = 0; i < num_of_regulators; i++){


			strcpy(prop_path, PREG_PATH);

			prop_path[PREG_NUM_LOCATION + 0] += ((i+1)%100)/10;
			prop_path[PREG_NUM_LOCATION + 1] += (i+1)%10;
			strcat(prop_path, SUPPLY_REG_NAME_PATH);

			ret = vm_prop_read(&prop_fd, prop_path, VM_PROP_T_STRING,
						   PREG_NAME_LEN, p4_reg_name, NULL, NULL);

			if(ret)
				pr_debug("%s: Enable to read %s ret %d\n", __func__, prop_path, ret);
			if(!strcmp(p4_reg_name,p4_reg_desc->name))
				break;
		}
	}




			p4_reg_initdata = kzalloc(sizeof(struct regulator_init_data), GFP_KERNEL);

			if (!p4_reg_initdata)
				pr_err("Allocating memory for p4_reg_initdata filed\n");

			p4_reg_initdata->constraints.name = p4_reg_desc->name;

			prop_path[PREG_NUM_LOCATION + 2] = '\0';
			strcat(prop_path, "/regulation_constraints/min_uV");

			ret = vm_prop_read(&prop_fd, prop_path, VM_PROP_T_UINT32,
				   sizeof(P4_uint32_t), &p4_reg_initdata->constraints.min_uV, NULL, NULL);
			if(ret)
				pr_debug("%s: Enable to read %s\n", __func__, prop_path);

			prop_path[PREG_NUM_LOCATION + 2] = '\0';
			strcat(prop_path, "/regulation_constraints/max_uV");

			ret = vm_prop_read(&prop_fd, prop_path, VM_PROP_T_UINT32,
				   sizeof(P4_uint32_t), &p4_reg_initdata->constraints.max_uV, NULL, NULL);
			if(ret)
				pr_debug("%s: Enable to read %s\n", __func__, prop_path);

			prop_path[PREG_NUM_LOCATION + 2] = '\0';
			strcat(prop_path, "/regulation_constraints/uV_offset");

			ret = vm_prop_read(&prop_fd, prop_path, VM_PROP_T_UINT32,
				   sizeof(P4_uint32_t), &p4_reg_initdata->constraints.uV_offset, NULL, NULL);
			if(ret)
				pr_debug("%s: Enable to read %s\n", __func__, prop_path);

			prop_path[PREG_NUM_LOCATION + 2] = '\0';
			strcat(prop_path, "/regulation_constraints/min_uA");

			ret = vm_prop_read(&prop_fd, prop_path, VM_PROP_T_UINT32,
				   sizeof(P4_uint32_t), &p4_reg_initdata->constraints.min_uA, NULL, NULL);
			if(ret)
				pr_debug("%s: Enable to read %s\n", __func__, prop_path);

			prop_path[PREG_NUM_LOCATION + 2] = '\0';
			strcat(prop_path, "/regulation_constraints/max_uA");

			ret = vm_prop_read(&prop_fd, prop_path, VM_PROP_T_UINT32,
				   sizeof(P4_uint32_t), &p4_reg_initdata->constraints.max_uA, NULL, NULL);
			if(ret)
				pr_debug("%s: Enable to read %s\n", __func__, prop_path);

			prop_path[PREG_NUM_LOCATION + 2] = '\0';
			strcat(prop_path, "/regulation_constraints/valid_modes_mask");

			ret = vm_prop_read(&prop_fd, prop_path, VM_PROP_T_UINT32,
				   sizeof(P4_uint32_t), &p4_reg_initdata->constraints.valid_modes_mask, NULL, NULL);
			if(ret)
				pr_debug("%s: Enable to read %s\n", __func__, prop_path);

			prop_path[PREG_NUM_LOCATION + 2] = '\0';
			strcat(prop_path, "/regulation_constraints/valid_ops_mask");

			ret = vm_prop_read(&prop_fd, prop_path, VM_PROP_T_UINT32,
				   sizeof(P4_uint32_t), &p4_reg_initdata->constraints.valid_ops_mask, NULL, NULL);
			if(ret)
				pr_debug("%s: Enable to read %s\n", __func__, prop_path);

			prop_path[PREG_NUM_LOCATION + 2] = '\0';
			strcat(prop_path, "/regulation_constraints/always_on");

			ret = vm_prop_read(&prop_fd, prop_path, VM_PROP_T_UINT32,
				   sizeof(P4_uint32_t), &temp, NULL, NULL);
			if(ret)
				pr_debug("%s: Enable to read %s\n", __func__, prop_path);
			p4_reg_initdata->constraints.always_on = temp;

			prop_path[PREG_NUM_LOCATION + 2] = '\0';
			strcat(prop_path, "/regulation_constraints/boot_on");

			vm_prop_read(&prop_fd, prop_path, VM_PROP_T_UINT32,
				   sizeof(P4_uint32_t), &temp, NULL, NULL);

			p4_reg_initdata->constraints.boot_on = temp;

			prop_path[PREG_NUM_LOCATION + 2] = '\0';
			strcat(prop_path, "/regulation_constraints/apply_uV");

			vm_prop_read(&prop_fd, prop_path, VM_PROP_T_UINT32,
				   sizeof(P4_uint32_t), &temp, NULL, NULL);

			p4_reg_initdata->constraints.apply_uV = temp;

			prop_path[PREG_NUM_LOCATION + 2] = '\0';
			strcat(prop_path, "/regulation_constraints/input_uV");

			vm_prop_read(&prop_fd, prop_path, VM_PROP_T_UINT32,
				   sizeof(P4_uint32_t), &p4_reg_initdata->constraints.input_uV, NULL, NULL);

			prop_path[PREG_NUM_LOCATION + 2] = '\0';
			strcat(prop_path, "/regulation_constraints/state_disk");

			vm_prop_read(&prop_fd, prop_path, VM_PROP_T_UINT32,
				   sizeof(P4_uint32_t), &p4_reg_initdata->constraints.state_disk, NULL, NULL);

			prop_path[PREG_NUM_LOCATION + 2] = '\0';
			strcat(prop_path, "/regulation_constraints/state_mem");

			vm_prop_read(&prop_fd, prop_path, VM_PROP_T_UINT32,
				   sizeof(P4_uint32_t), &p4_reg_initdata->constraints.state_mem, NULL, NULL);

			prop_path[PREG_NUM_LOCATION + 2] = '\0';
			strcat(prop_path, "/regulation_constraints/state_standby");

			vm_prop_read(&prop_fd, prop_path, VM_PROP_T_UINT32,
				   sizeof(P4_uint32_t), &p4_reg_initdata->constraints.state_standby, NULL, NULL);

			prop_path[PREG_NUM_LOCATION + 2] = '\0';
			strcat(prop_path, "/regulation_constraints/initial_state");

			vm_prop_read(&prop_fd, prop_path, VM_PROP_T_UINT32,
				   sizeof(P4_uint32_t), &p4_reg_initdata->constraints.initial_state, NULL, NULL);

			prop_path[PREG_NUM_LOCATION + 2] = '\0';
			strcat(prop_path, "/regulation_constraints/initial_mode");

			vm_prop_read(&prop_fd, prop_path, VM_PROP_T_UINT32,
				   sizeof(P4_uint32_t), &p4_reg_initdata->constraints.initial_mode, NULL, NULL);

			prop_path[PREG_NUM_LOCATION + 2] = '\0';
			strcat(prop_path, "/num_consumer_supplies");

			ret = vm_prop_read(&prop_fd, prop_path, VM_PROP_T_UINT32,
				   sizeof(P4_uint32_t), &p4_reg_initdata->num_consumer_supplies, NULL, NULL);
			if(ret)
				pr_debug("%s: Enable to read %s\n", __func__, prop_path);

			p4_reg_consumers = kzalloc(sizeof(struct regulator_init_data) * p4_reg_initdata->num_consumer_supplies, GFP_KERNEL);

			if (!p4_reg_consumers)
				pr_err("Allocating memory for p4_reg_consumers filed\n");

			p4_reg_initdata->consumer_supplies = p4_reg_consumers;

			for(j = 0; j < p4_reg_initdata->num_consumer_supplies; j++){

				prop_path[PREG_NUM_LOCATION + 2] = '\0';
				strcat(prop_path, CONSUMER_SUPPLY_PATH);

				prop_path[CONSUMER_SUPPLY_NUM_LOCATION + 0] += ((j+1)%100)/10;
				prop_path[CONSUMER_SUPPLY_NUM_LOCATION + 1] += (j+1)%10;

				strcat(prop_path, "/dev_name");
				ret = vm_prop_read(&prop_fd, prop_path, VM_PROP_T_STRING,
						   PREG_NAME_LEN, p4_reg_name, NULL, NULL);

				if(ret)
					pr_debug("%s: Enable to read %s\n", __func__, prop_path);

				p4_reg_consumers->dev_name = kstrdup(p4_reg_name, GFP_KERNEL);

				prop_path[CONSUMER_SUPPLY_NUM_LOCATION + 2] = '\0';

				strcat(prop_path, "/supply");
				ret = vm_prop_read(&prop_fd, prop_path, VM_PROP_T_STRING,
						   PREG_NAME_LEN, p4_reg_name, NULL, NULL);
				if(ret)
					pr_debug("%s: Enable to read %s\n", __func__, prop_path);

				p4_reg_consumers->supply = kstrdup(p4_reg_name, GFP_KERNEL);

				p4_reg_consumers++;
			}


	/* Only 3p0, 2p5, and 1p1 has enable bit */
	p4_reg_desc->min_uV = p4_reg_initdata->constraints.min_uV;
	p4_reg_desc->uV_step = 25000;

	config.dev = &pdev->dev;
	config.init_data = p4_reg_initdata;
	config.driver_data = NULL;
	config.of_node = pdev->dev.of_node;

	p4_reg_desc->ops = &p4_reg_ops;
	/* register regulator */
	rdev = devm_regulator_register(dev, p4_reg_desc, &config);
	if (IS_ERR(rdev)) {
		dev_err(dev, "failed to register %s\n",
			p4_reg_desc->name);
		return PTR_ERR(rdev);
	}

	pr_err("dev handle=%p\n",dev);
	platform_set_drvdata(pdev, rdev);

	return 0;

}

static struct of_device_id of_anatop_regulator_match_tbl[] = {
	{ .compatible = "fsl,anatop-regulator", },
	{ /* end */ }
};

static struct platform_driver p4_anatop_regulator_driver = {
	.driver = {
		.name	= "p4_anatop_regulator",
		.owner  = THIS_MODULE,
		.of_match_table = of_anatop_regulator_match_tbl,
	},
	.probe	= p4_anatop_regulator_probe,
};

static int __init regulator_p4_reg_init(void)
{
	vm_e_t rc;

	rc =  vm_open(PREG_FP_FILE, VM_O_RW, &vreg_fd);

	if(rc != VM_E_OK)
		pr_err("%s: vm_open() for %s failed with rc = 0x%02x", __func__, PREG_FP_FILE, rc);

	return platform_driver_register(&p4_anatop_regulator_driver);

}
postcore_initcall(regulator_p4_reg_init);

MODULE_DESCRIPTION("P4 voltage regulator");
MODULE_LICENSE("GPL");




