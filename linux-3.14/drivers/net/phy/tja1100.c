/*
 * Driver for NXP TJA1100 PHY
 *
 * Author: Maciej Szymanski <maciej.szymanski@opensynergy.com>
 *
 * Copyright (c) 2017 OpenSynergy GmbH
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mii.h>
#include <linux/phy.h>
#include <linux/of.h>

#define PHY_ID_TJA1100        0x0180dc48

#define EXT_CTRL_REG          0x11
#define CONFIG_EN             0x2

#define SMI_NXP_CONF_REG_1    0x12
#define MASTER_SLAVE          0xf
#define MII_MODE              0x8
#define MII_MODE_MASK        (0x3 << MII_MODE)
#define MII_MODE_MII          0
#define MII_MODE_RMII_50MHZ   1
#define MII_MODE_RMII_25MHZ   2
#define MII_MODE_REVERSE_MII  3

#define COMMSTAT_REG          0x17
#define COMMSTAT_LINK_UP     (0x1 << 15)


#define TJA1100_ESTATUS_100_T1FULL	0x0080

MODULE_DESCRIPTION("NXP TJA1100 driver");
MODULE_AUTHOR("Maciej Szymanski <maciej.szymanski@opensynergy.com>");
MODULE_LICENSE("GPL");

static int tja1100_config_init(struct phy_device *phydev)
{
	bool cfg_changed = false;
	int conf_reg_1;
	int val;
	int err;
	u32 features;
	features = SUPPORTED_MII;

	conf_reg_1 = phy_read(phydev, SMI_NXP_CONF_REG_1);

	/*
	 * There are PCBs with a missing connection to ground. So the pull-down
	 * resistor for CONFIG2 can't do it's job. Therefor we need this hack.
	 * There are two different configurations, with and without a switch.
	 * Which variant we have is determined by CONFIG3, high bit in MII_MODE.
	 */
	if(((conf_reg_1 & MII_MODE_MASK) >> MII_MODE) > 1) {
		pr_info("TJA1100: RMII mode (25MHz XTAL).\n");
		conf_reg_1 &= ~MII_MODE_MASK;
		conf_reg_1 |= (MII_MODE_RMII_25MHZ << MII_MODE);
		cfg_changed = true;
	} else {
		pr_info("TJA1100: RMII mode (50MHz REFCLK_IN).\n");
	}

	/* If neither slave/master is set in device tree, the config pin
	 * setting will be used 
	*/
	if (of_property_read_bool(phydev->dev.of_node, "slave")) {
		pr_info("TJA1100: Setting slave mode.\n");
		conf_reg_1 &= ~(1 << MASTER_SLAVE);
		cfg_changed = true;
	}

	if (of_property_read_bool(phydev->dev.of_node, "master")) {
		pr_info("TJA1100: Setting master mode.\n");
		conf_reg_1 |= (1 << MASTER_SLAVE);
		cfg_changed = true;
	}

	if(cfg_changed) {
		/* To allow writing to SMI_NXP_CONF_REG_1, the CONFIG_EN bit
		 * in the extended control register must be set.
		 */
		val = phy_read(phydev, EXT_CTRL_REG);
		val |= (1 << CONFIG_EN);
		err = phy_write(phydev, EXT_CTRL_REG, val);

		err = phy_write(phydev, SMI_NXP_CONF_REG_1, conf_reg_1);

		val &= ~(1 << CONFIG_EN);
		err = phy_write(phydev, EXT_CTRL_REG, val);
	}

	val = phy_read(phydev, MII_BMSR);
	if (val < 0)
		return val;

	if (val & BMSR_100FULL)
		features |= SUPPORTED_100baseT_Full;
	if (val & BMSR_100HALF)
		features |= SUPPORTED_100baseT_Half;
	if (val & BMSR_10FULL)
		features |= SUPPORTED_10baseT_Full;
	if (val & BMSR_10HALF)
		features |= SUPPORTED_10baseT_Half;

	if (val & BMSR_ESTATEN) {
		val = phy_read(phydev, MII_ESTATUS);
		if (val < 0)
			return val;

		if (val & ESTATUS_1000_TFULL)
			features |= SUPPORTED_1000baseT_Full;
		if (val & ESTATUS_1000_THALF)
			features |= SUPPORTED_1000baseT_Half;
		if (val & TJA1100_ESTATUS_100_T1FULL)
			features |= SUPPORTED_100baseT_Full;
	}

	phydev->supported = features;
	phydev->advertising = features;
	phydev->autoneg = AUTONEG_DISABLE;

	return 0;
}

static ssize_t sysfs_link_status_show(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	int linkup = 0;
	struct phy_device *phydev = container_of(dev, struct phy_device, dev);

	if (phydev) {
		linkup = phy_read(phydev, COMMSTAT_REG) & COMMSTAT_LINK_UP;
	}

	return scnprintf(buf, PAGE_SIZE, "%s\n", linkup ? "up" : "down");
}

static DEVICE_ATTR(link_status, S_IRUSR, sysfs_link_status_show, NULL);

static int tja1100_probe(struct phy_device *phydev)
{
	int err = 0;

	if (of_property_read_bool(phydev->dev.of_node, "standalone")) {
		/* Initialize the phy in case it should be operated in standalone
		 * mode. config_init would not be called otherwise
		 */
		err = phydev->drv->config_init(phydev);
		if (err < 0)
			return err;

		/* register sysfs files */
		err = device_create_file(&phydev->dev, &dev_attr_link_status);
		if (err) {
			dev_err(&phydev->dev, "sysfs file creation failed\n");
			return -ENOMEM;
		}
	}

	return err;
}

static void tja1100_remove(struct phy_device *phydev)
{
	device_remove_file(&phydev->dev, &dev_attr_link_status);
}

static struct phy_driver tja1100_driver = {
	.phy_id        = PHY_ID_TJA1100,
	.name          = "NXP TJA1100 BroadR-Reach PHY",
	.phy_id_mask   = 0xfffffff0,
	.features      = 0,
	.probe         = tja1100_probe,
	.remove        = tja1100_remove,
	.config_init   = tja1100_config_init,
	.config_aneg   = genphy_config_aneg,
	.read_status   = genphy_read_status,
	.suspend       = genphy_suspend,
	.resume        = genphy_resume,
	.driver        = { .owner = THIS_MODULE,},
};

static int __init tja1100_init(void)
{
	return phy_driver_register(&tja1100_driver);
}

static void __exit tja1100_exit(void)
{
	phy_driver_unregister(&tja1100_driver);
}

module_init(tja1100_init);
module_exit(tja1100_exit);

static struct mdio_device_id __maybe_unused tja1100_tbl[] = {
	{ PHY_ID_TJA1100, 0xfffffff0 },
	{ }
};

MODULE_DEVICE_TABLE(mdio, tja1100_tbl);
