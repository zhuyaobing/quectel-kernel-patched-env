/*
 *  P4VM generic input driver based on a queuing port
 * 
 *  Author: David Engraf <den@sysgo.com>
 * 
 *  Copyright (C) 2012 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 */

/*
 * This driver uses a single RX queuing port, named input_rx and receives
 * transparent linux input codes.
 *
 * Note:
 *  Supported events are configured in the probe function and must be adapted
 *  to the underlaying hardware.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/vmapi_calls.h>
#include "vmport_core.h"
#include "vmdrv_ioctl.h"

#define P4VM_DEV_NAME	"vmportinput"	/* device name */

struct vminput_driverinfo
{
	/* port interface */
	struct vmport_core_portinfo *iport;

	struct input_dev *input;
};

/* --------------------------------------------------------------------- */

/*
 * IRQ callback (called by vmport_core IRQs)
 */
static void vminput_rx_int(void *data)
{
	struct vminput_driverinfo *info = (struct vminput_driverinfo *) data;
	int len, i;
	struct input_event levt;

	len = vmapi_qport_receive(info->iport->line, info->iport->buf, info->iport->framesize);
	if (len <= 0) {
		dev_err(&info->input->dev, "Error reading: %d\n", len);
		return;
	}

	dev_dbg(&info->input->dev, "Received %d Bytes\n", len);

	for (i = 0; i < len;) {
		if (len - i < sizeof(struct input_event)) {
			dev_warn(&info->input->dev, "Remaining bytes incomplete (left %d)\n",
				len - i);
			break;
		}

		/* copy bytes to structure to not generate alignment traps */
		memcpy(&levt, &info->iport->buf[i], sizeof(levt));

		if (levt.type != EV_ABS && levt.type != EV_REL &&
			levt.type != EV_KEY && levt.type != EV_SYN) {
			dev_warn(&info->input->dev, "Unknown input type: 0x%X (pos %d)\n",
				levt.type, i);
			i++;
			continue;
		}
		input_event(info->input, levt.type, levt.code, levt.value);
		dev_err(&info->input->dev, "Type: 0x%X, Code: 0x%X, Value: 0x%X\n",
			levt.type, levt.code, levt.value);
		i += sizeof(struct input_event);
	}

	vmport_core_poll_start(info->iport);
}

/* --------------------------------------------------------------------- */

static int vminput_probe(struct platform_device *pdev)
{
	struct vminput_driverinfo *info;
	struct vmport_core_portinfo *port = NULL;
	int i, err;

	for (i = 0; i < vmport_core_count; i++)
	{
		port = &vmport_core_portinfo[i];
		if (strcmp(port->portname, "input_rx") == 0) {
			if (port->direction == VMPORT_CORE_DIR_RECV)
				break;
		}
	}
	if (i == vmport_core_count) {
		dev_info(&pdev->dev, "No queueing port found\n");
		return -ENODEV;
	}

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->iport = port;
	info->input = input_allocate_device();
	if (!info->input) {
		dev_err(&pdev->dev, "Failed to allocate input device.\n");
		err = -ENOMEM;
		goto error;
	}

	info->input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
	info->input->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);
	info->input->absbit[0] =
	    BIT_MASK(ABS_X) | BIT_MASK(ABS_Y) | BIT_MASK(ABS_PRESSURE);

#define X_AXIS_MIN      0
#define X_AXIS_MAX      0x3ff
#define Y_AXIS_MIN      0
#define Y_AXIS_MAX      0x3ff
#define PRESSURE_MIN    0
#define PRESSURE_MAX    0x3ff
	input_set_abs_params(info->input, ABS_X, X_AXIS_MIN, X_AXIS_MAX, 0, 0);
	input_set_abs_params(info->input, ABS_Y, Y_AXIS_MIN, Y_AXIS_MAX, 0, 0);
	input_set_abs_params(info->input, ABS_PRESSURE, PRESSURE_MIN, PRESSURE_MAX, 0, 0);

	info->input->name = pdev->name;
	info->input->id.bustype = BUS_VIRTUAL;
	info->input->dev.parent = &pdev->dev;

	err = vmport_core_init_port(info->iport, vminput_rx_int, info,
				    VMPORT_CORE_TYPE_LINK, NULL, 1);
	if (err) {
		dev_err(&pdev->dev, "Failed to initialize port (%d)\n", err);
		input_free_device(info->input);
		goto error;
	}
	platform_set_drvdata(pdev, info);

	err = input_register_device(info->input);
	if (err) {
		input_free_device(info->input);
		goto error;
	}
	vmport_core_poll_start(info->iport);

	return 0;

error:
	kfree(info);
	return err;
}

static int vminput_remove(struct platform_device *pdev)
{
	struct vminput_driverinfo *info = platform_get_drvdata(pdev);

	if (info) {
		vmport_core_exit_port(info->iport);
		input_free_device(info->input);
		kfree(info);
	}

	return 0;
}

static struct platform_driver vminput_driver = {
	.driver		= {
		.name	= P4VM_DEV_NAME,
		.owner	= THIS_MODULE,
	},
	.probe  = vminput_probe,
	.remove = vminput_remove,
};


/*
 * int vminput_init(void)
 *
 * Initialize and register driver
 */
static int __init vminput_init(void)
{
	int ret;

	ret = platform_driver_register(&vminput_driver);
	if (!ret) {
		platform_device_register_simple(P4VM_DEV_NAME, -1, NULL, 0);
	}

	return ret;
}

/*
 * void vminput_exit(void)
 *
 * Shutdown driver
 */
static void __exit vminput_exit(void)
{
	platform_driver_unregister(&vminput_driver);
}

module_init(vminput_init);
module_exit(vminput_exit);

MODULE_AUTHOR("David Engraf <den@sysgo.com>");
MODULE_DESCRIPTION("PikeOS Communication Ports (PCP) input driver");
MODULE_LICENSE("GPL");

