/*
 * PikeOS RTC driver.
 *
 * Copyright (C) 2012 SYSGO AG
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/rtc.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <asm/vmapi_calls.h>

struct pikeos_rtc_priv {
	struct rtc_device *rtc;
	int handle;
};

static int pikeos_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	struct pikeos_rtc_priv *priv = dev_get_drvdata(dev);
	u64 t;
	int ret;

	ret = vmapi_file_ioctl(priv->handle, 1, &t, -1, sizeof(t));
	if (ret)
		return ret;

	/* convert P4_time_t (nanoseconds) to seconds */
	do_div(t, 1000000000);

	rtc_time_to_tm((unsigned long)t, tm);

	return rtc_valid_tm(tm);
}

static int pikeos_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	struct pikeos_rtc_priv *priv = dev_get_drvdata(dev);
	unsigned long time;
	u64 t;
	int ret;

	rtc_tm_to_time(tm, &time);
	t = (u64)time * 1000000000;

	ret =  vmapi_file_ioctl(priv->handle, 2, &t, sizeof(t), -1);
	return ret;
}

static struct rtc_class_ops pikeos_rtc_ops = {
	.read_time	= pikeos_rtc_read_time,
	.set_time	= pikeos_rtc_set_time,
};

static int pikeos_rtc_probe(struct platform_device *pdev)
{
	struct pikeos_rtc_priv *priv;
	int ret, handle;

	/*
	 * Use driver/VMIT open flags. Be aware that the linxu driver always
	 * needs read access, else it cannot set the time. If the vmit is
	 * configured read only, we could set set_time to NULL, but then the
	 * error code for the user applicaton is "invalid parameter", now the
	 * ioctl returns "operation not permitted" which is better.
	 */
	handle = vmapi_file_open("rtc:/", VMAPI_O_AUTO, NULL);
	if (handle < 0) {
		dev_err(&pdev->dev, "cannot open %d\n", handle);
		return -ENODEV;
	}

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv) {
		vmapi_file_close(handle);
		return -ENOMEM;
	}

	platform_set_drvdata(pdev, priv);

	priv->handle = handle;
	priv->rtc = rtc_device_register("rtc-pikeos", &pdev->dev,
				     &pikeos_rtc_ops, THIS_MODULE);
	if (IS_ERR(priv->rtc)) {
		ret = PTR_ERR(priv->rtc);
		goto out_err;
	}

	return 0;

out_err:
	vmapi_file_close(handle);
	kfree(priv);
	return ret;
}

static int pikeos_rtc_remove(struct platform_device *pdev)
{
	struct pikeos_rtc_priv *priv = platform_get_drvdata(pdev);

	rtc_device_unregister(priv->rtc);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

static struct platform_driver pikeos_driver = {
	.driver		= {
		.name	= "rtc-pikeos",
		.owner	= THIS_MODULE,
	},
	.remove		= pikeos_rtc_remove,
};

static int __init pikeos_rtc_init(void)
{
	platform_device_register_simple("rtc-pikeos", -1, NULL, 0);
	return platform_driver_probe(&pikeos_driver, pikeos_rtc_probe);
}

static void __exit pikeos_rtc_exit(void)
{
	platform_driver_unregister(&pikeos_driver);
}

module_init(pikeos_rtc_init);
module_exit(pikeos_rtc_exit);

MODULE_DESCRIPTION("PikeOS VM RTC driver");
MODULE_AUTHOR("David Engraf <david.engraf@sysgo.com>");
MODULE_LICENSE("GPL");
