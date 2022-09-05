/*
 * An RTC driver for SRE Alexandra
 * Copyright (C) 2019 Stoneridge Electronics AB
 * Author: Erik Ahlén <erik.ahlen@stoneridge.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/rtc.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>

struct srealex_data {
	struct rtc_device *rtc;
	void __iomem *ioaddr;
};

static int srealex_read_time(struct device *dev, struct rtc_time *tm)
{
	struct srealex_data *data = dev_get_drvdata(dev);
	u32 time = readl(data->ioaddr);

	if(time == 0)
		return -EINVAL;

	rtc_time_to_tm(time, tm);

	return 0;
}

static const struct rtc_class_ops srealex_ops = {
	.read_time = srealex_read_time,
};

static int srealex_probe(struct platform_device *pdev)
{
	struct srealex_data *data;
	struct resource *res;
	int ret;

	data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	data->ioaddr = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(data->ioaddr))
		return PTR_ERR(data->ioaddr);

	platform_set_drvdata(pdev, data);

	data->rtc = devm_rtc_device_register(&pdev->dev, pdev->name,
					&srealex_ops, THIS_MODULE);
	if (IS_ERR(data->rtc)) {
		ret = PTR_ERR(data->rtc);
		dev_err(&pdev->dev, "failed to register rtc: %d\n", ret);
		return ret;
	}

	return 0;
}

static const struct of_device_id srealex_dt_ids[] = {
	{ .compatible = "sre,rtc-alex", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, srealex_dt_ids);

static struct platform_driver srealex_rtc_driver = {
	.driver = {
		.name	= "rtc-srealex",
		.owner	= THIS_MODULE,
		.of_match_table = srealex_dt_ids,
	},
	.probe		= srealex_probe,
};
module_platform_driver(srealex_rtc_driver);

static int __init srealex_init(void)
{
	int ret = 0;
#ifndef CONFIG_OF
	struct resource res = DEFINE_RES_MEM_NAMED(0x0090006c, 4, "dat");

	ret = IS_ERR(platform_device_register_simple("rtc-srealex", -1, &res, 1));
#endif
	return ret;
}

static void __exit srealex_exit(void)
{
#ifndef CONFIG_OF
	platform_driver_unregister(&srealex_rtc_driver);
#endif
}

module_init(srealex_init);
module_exit(srealex_exit);


MODULE_AUTHOR("Erik Ahlén <erik.ahlen@stoneridge.com>");
MODULE_DESCRIPTION("RTC driver for Alexandra");
MODULE_LICENSE("GPL");
