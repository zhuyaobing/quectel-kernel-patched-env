/* Copyright (c) 2016 OpenSynergy GmbH */
/*
 * MXC GPIO support. (c) 2008 Daniel Mack <daniel@caiaq.de>
 * Copyright 2008 Juergen Beisert, kernel@pengutronix.de
 *
 * Based on code from Freescale,
 * Copyright (C) 2004-2010 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/err.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/basic_mmio_gpio.h>
#include <linux/of.h>
#include <linux/of_device.h>
#ifdef CONFIG_P4_BSP_ARCH_MXC
#include <linux/of_irq.h>
#endif
#include <linux/module.h>
#include <asm-generic/bug.h>

#ifdef CONFIG_P4_BSP_ARCH_MXC
/*
 * The PikeOS PSP maps the MXC GPIO IRQs to IRQ 256-480. Preferably, the IRQs
 * for each bank should be read from the device tree, but hard-code the IRQs
 * for now.
 */
#define MXC_GPIO_IRQ_START 256
#endif


/* GPIO virtualization uses P4 device access */
#ifdef CONFIG_P4_VIRT_GPIO
#include <linux/kernel.h>
#include <asm/vmapi_calls.h>
#include <asm/debugout.h>
#include <p4.h>
#include <psp/dev_gpio.h>

#pragma message "Compiling with GPIO virtualization active"

static inline int p4_err_to_linux(P4_e_t p4_error) { return 2000 + (int)p4_error; }

static P4_devid_t gpio_dev = -1;
#endif /* CONFIG_P4_VIRT_GPIO */

#ifdef CONFIG_DEBUG_GPIO
#define DBG_GPIO(_f_, ...)      printk(KERN_NOTICE ">>> " _f_, ##__VA_ARGS__)
#else /* CONFIG_DEBUG_GPIO */
#define DBG_GPIO(_f_, ...)
#endif /* CONFIG_DEBUG_GPIO */

#ifdef CONFIG_P4_BSP_ARCH_MXC
extern struct irq_chip P4INT_irq_type;
#endif /* CONFIG_P4_BSP_ARCH_MXC */

enum mxc_gpio_hwtype {
	IMX1_GPIO,	/* runs on i.mx1 */
	IMX21_GPIO,	/* runs on i.mx21 and i.mx27 */
	IMX31_GPIO,	/* runs on i.mx31 */
	IMX35_GPIO,	/* runs on all other i.mx */
};

/* device type dependent stuff */
struct mxc_gpio_hwdata {
	unsigned dr_reg;
	unsigned gdir_reg;
	unsigned psr_reg;
	unsigned icr1_reg;
	unsigned icr2_reg;
	unsigned imr_reg;
	unsigned isr_reg;
	int edge_sel_reg;
	unsigned low_level;
	unsigned high_level;
	unsigned rise_edge;
	unsigned fall_edge;
};

struct mxc_gpio_port {
	struct list_head node;
	void __iomem *base;
	int irq;
	int irq_high;
	struct irq_domain *domain;
	struct bgpio_chip bgc;
	u32 both_edges;
};

static struct mxc_gpio_hwdata imx1_imx21_gpio_hwdata = {
	.dr_reg		= 0x1c,
	.gdir_reg	= 0x00,
	.psr_reg	= 0x24,
	.icr1_reg	= 0x28,
	.icr2_reg	= 0x2c,
	.imr_reg	= 0x30,
	.isr_reg	= 0x34,
	.edge_sel_reg	= -EINVAL,
	.low_level	= 0x03,
	.high_level	= 0x02,
	.rise_edge	= 0x00,
	.fall_edge	= 0x01,
};

static struct mxc_gpio_hwdata imx31_gpio_hwdata = {
	.dr_reg		= 0x00,
	.gdir_reg	= 0x04,
	.psr_reg	= 0x08,
	.icr1_reg	= 0x0c,
	.icr2_reg	= 0x10,
	.imr_reg	= 0x14,
	.isr_reg	= 0x18,
	.edge_sel_reg	= -EINVAL,
	.low_level	= 0x00,
	.high_level	= 0x01,
	.rise_edge	= 0x02,
	.fall_edge	= 0x03,
};

static struct mxc_gpio_hwdata imx35_gpio_hwdata = {
	.dr_reg		= 0x00,
	.gdir_reg	= 0x04,
	.psr_reg	= 0x08,
	.icr1_reg	= 0x0c,
	.icr2_reg	= 0x10,
	.imr_reg	= 0x14,
	.isr_reg	= 0x18,
	.edge_sel_reg	= 0x1c,
	.low_level	= 0x00,
	.high_level	= 0x01,
	.rise_edge	= 0x02,
	.fall_edge	= 0x03,
};

static enum mxc_gpio_hwtype mxc_gpio_hwtype;
static struct mxc_gpio_hwdata *mxc_gpio_hwdata;

#define GPIO_DR			(mxc_gpio_hwdata->dr_reg)
#define GPIO_GDIR		(mxc_gpio_hwdata->gdir_reg)
#define GPIO_PSR		(mxc_gpio_hwdata->psr_reg)
#define GPIO_ICR1		(mxc_gpio_hwdata->icr1_reg)
#define GPIO_ICR2		(mxc_gpio_hwdata->icr2_reg)
#define GPIO_IMR		(mxc_gpio_hwdata->imr_reg)
#define GPIO_ISR		(mxc_gpio_hwdata->isr_reg)
#define GPIO_EDGE_SEL		(mxc_gpio_hwdata->edge_sel_reg)

#ifndef CONFIG_P4_VIRT_GPIO
#define GPIO_INT_LOW_LEV	(mxc_gpio_hwdata->low_level)
#define GPIO_INT_HIGH_LEV	(mxc_gpio_hwdata->high_level)
#define GPIO_INT_RISE_EDGE	(mxc_gpio_hwdata->rise_edge)
#define GPIO_INT_FALL_EDGE	(mxc_gpio_hwdata->fall_edge)
#define GPIO_INT_BOTH_EDGES	0x4
#endif /* CONFIG_P4_VIRT_GPIO */

static struct platform_device_id mxc_gpio_devtype[] = {
	{
		.name = "imx1-gpio",
		.driver_data = IMX1_GPIO,
	}, {
		.name = "imx21-gpio",
		.driver_data = IMX21_GPIO,
	}, {
		.name = "imx31-gpio",
		.driver_data = IMX31_GPIO,
	}, {
		.name = "imx35-gpio",
		.driver_data = IMX35_GPIO,
	}, {
		/* sentinel */
	}
};

static const struct of_device_id mxc_gpio_dt_ids[] = {
	{ .compatible = "fsl,imx1-gpio", .data = &mxc_gpio_devtype[IMX1_GPIO], },
	{ .compatible = "fsl,imx21-gpio", .data = &mxc_gpio_devtype[IMX21_GPIO], },
	{ .compatible = "fsl,imx31-gpio", .data = &mxc_gpio_devtype[IMX31_GPIO], },
	{ .compatible = "fsl,imx35-gpio", .data = &mxc_gpio_devtype[IMX35_GPIO], },
	{ /* sentinel */ }
};

/*
 * MX2 has one interrupt *for all* gpio ports. The list is used
 * to save the references to all ports, so that mx2_gpio_irq_handler
 * can walk through all interrupt status registers.
 */
static LIST_HEAD(mxc_gpio_ports);

/* Note: This driver assumes 32 GPIOs are handled in one register */

static int gpio_set_irq_type(struct irq_data *d, u32 type)
{
	struct irq_chip_generic *gc = irq_data_get_irq_chip_data(d);
	struct mxc_gpio_port *port = gc->private;
	u32 bit, val;
#ifndef CONFIG_P4_BSP_ARCH_MXC
	u32 gpio_idx = d->hwirq;
#else
	u32 gpio_idx = d->irq - MXC_GPIO_IRQ_START - port->bgc.gc.base;
#endif

#ifdef CONFIG_P4_VIRT_GPIO
	u32 gpio = ((d->irq) - MXC_GPIO_IRQ_START);
	P4_e_t err;
	int edge;

	switch (type) {
	case IRQ_TYPE_EDGE_RISING:
		edge = GPIO_INT_TRIGGER_RISINGEDGE;
		break;
	case IRQ_TYPE_EDGE_FALLING:
		edge = GPIO_INT_TRIGGER_FALLINGEDGE;
		break;
	case IRQ_TYPE_EDGE_BOTH:
		edge = GPIO_INT_TRIGGER_BOTHEDGES;
		break;
	case IRQ_TYPE_LEVEL_LOW:
		edge = GPIO_INT_TRIGGER_LOWLEVEL;
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		edge = GPIO_INT_TRIGGER_HIGHLEVEL;
		break;
	default:
		return -EINVAL;
    }
	err = gpio_pin_intctrl(gpio_dev, gpio / GPIO_MAX_PIN, gpio % GPIO_MAX_PIN, edge, 0);
	DBG_GPIO("GPIO PikeOS kernel device (port #%u pin #%2u: IRQ trigger %u), P4 res = %u\n",
		gpio / GPIO_MAX_PIN + 1, gpio % GPIO_MAX_PIN, edge, err);
	return (err != P4_E_OK)? -p4_err_to_linux(err): 0;
#else /* #ifdef CONFIG_P4_VIRT_GPIO */
	u32 gpio = port->bgc.gc.base + gpio_idx;
	int edge;
	void __iomem *reg = port->base;

	port->both_edges &= ~(1 << gpio_idx);
	switch (type) {
	case IRQ_TYPE_EDGE_RISING:
		edge = GPIO_INT_RISE_EDGE;
		break;
	case IRQ_TYPE_EDGE_FALLING:
		edge = GPIO_INT_FALL_EDGE;
		break;
	case IRQ_TYPE_EDGE_BOTH:
		if (GPIO_EDGE_SEL >= 0) {
			edge = GPIO_INT_BOTH_EDGES;
		} else {
			val = gpio_get_value(gpio);
			if (val) {
				edge = GPIO_INT_LOW_LEV;
				pr_debug("mxc: set GPIO %d to low trigger\n", gpio);
			} else {
				edge = GPIO_INT_HIGH_LEV;
				pr_debug("mxc: set GPIO %d to high trigger\n", gpio);
			}
			port->both_edges |= 1 << gpio_idx;
		}
		break;
	case IRQ_TYPE_LEVEL_LOW:
		edge = GPIO_INT_LOW_LEV;
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		edge = GPIO_INT_HIGH_LEV;
		break;
	default:
		return -EINVAL;
	}

	if (GPIO_EDGE_SEL >= 0) {
		val = readl(port->base + GPIO_EDGE_SEL);
		if (edge == GPIO_INT_BOTH_EDGES)
			writel(val | (1 << gpio_idx),
				port->base + GPIO_EDGE_SEL);
		else
			writel(val & ~(1 << gpio_idx),
				port->base + GPIO_EDGE_SEL);
	}

	if (edge != GPIO_INT_BOTH_EDGES) {
		reg += GPIO_ICR1 + ((gpio_idx & 0x10) >> 2); /* lower or upper register */
		bit = gpio_idx & 0xf;
		val = readl(reg) & ~(0x3 << (bit << 1));
		writel(val | (edge << (bit << 1)), reg);
	}

	writel(1 << gpio_idx, port->base + GPIO_ISR);

	return 0;
#endif /* #ifdef CONFIG_P4_VIRT_GPIO */
}

#ifndef CONFIG_P4_BSP_ARCH_MXC
static void mxc_flip_edge(struct mxc_gpio_port *port, u32 gpio)
{
	void __iomem *reg = port->base;
	u32 bit, val;
	int edge;

	reg += GPIO_ICR1 + ((gpio & 0x10) >> 2); /* lower or upper register */
	bit = gpio & 0xf;
	val = readl(reg);
	edge = (val >> (bit << 1)) & 3;
	val &= ~(0x3 << (bit << 1));
	if (edge == GPIO_INT_HIGH_LEV) {
		edge = GPIO_INT_LOW_LEV;
		pr_debug("mxc: switch GPIO %d to low trigger\n", gpio);
	} else if (edge == GPIO_INT_LOW_LEV) {
		edge = GPIO_INT_HIGH_LEV;
		pr_debug("mxc: switch GPIO %d to high trigger\n", gpio);
	} else {
		pr_err("mxc: invalid configuration for GPIO %d: %x\n",
		       gpio, edge);
		return;
	}
	writel(val | (edge << (bit << 1)), reg);
}

/* handle 32 interrupts in one status register */
static void mxc_gpio_irq_handler(struct mxc_gpio_port *port, u32 irq_stat)
{
	while (irq_stat != 0) {
		int irqoffset = fls(irq_stat) - 1;

		if (port->both_edges & (1 << irqoffset))
			mxc_flip_edge(port, irqoffset);

		generic_handle_irq(irq_find_mapping(port->domain, irqoffset));

		irq_stat &= ~(1 << irqoffset);
	}
}

/* MX1 and MX3 has one interrupt *per* gpio port */
static void mx3_gpio_irq_handler(u32 irq, struct irq_desc *desc)
{
	u32 irq_stat;
	struct mxc_gpio_port *port = irq_get_handler_data(irq);
	struct irq_chip *chip = irq_get_chip(irq);

	chained_irq_enter(chip, desc);

	irq_stat = readl(port->base + GPIO_ISR) & readl(port->base + GPIO_IMR);

	mxc_gpio_irq_handler(port, irq_stat);

	chained_irq_exit(chip, desc);
}

/* MX2 has one interrupt *for all* gpio ports */
static void mx2_gpio_irq_handler(u32 irq, struct irq_desc *desc)
{
	u32 irq_msk, irq_stat;
	struct mxc_gpio_port *port;
	struct irq_chip *chip = irq_get_chip(irq);

	chained_irq_enter(chip, desc);

	/* walk through all interrupt status registers */
	list_for_each_entry(port, &mxc_gpio_ports, node) {
		irq_msk = readl(port->base + GPIO_IMR);
		if (!irq_msk)
			continue;

		irq_stat = readl(port->base + GPIO_ISR) & irq_msk;
		if (irq_stat)
			mxc_gpio_irq_handler(port, irq_stat);
	}
	chained_irq_exit(chip, desc);
}

/*
 * Set interrupt number "irq" in the GPIO as a wake-up source.
 * While system is running, all registered GPIO interrupts need to have
 * wake-up enabled. When system is suspended, only selected GPIO interrupts
 * need to have wake-up enabled.
 * @param  irq          interrupt source number
 * @param  enable       enable as wake-up if equal to non-zero
 * @return       This function returns 0 on success.
 */
static int gpio_set_wake_irq(struct irq_data *d, u32 enable)
{
	struct irq_chip_generic *gc = irq_data_get_irq_chip_data(d);
	struct mxc_gpio_port *port = gc->private;
	u32 gpio_idx = d->hwirq;

	if (enable) {
		if (port->irq_high && (gpio_idx >= 16))
			enable_irq_wake(port->irq_high);
		else
			enable_irq_wake(port->irq);
	} else {
		if (port->irq_high && (gpio_idx >= 16))
			disable_irq_wake(port->irq_high);
		else
			disable_irq_wake(port->irq);
	}

	return 0;
}
#endif /* CONFIG_P4_BSP_ARCH_MXC */

static void __init mxc_gpio_init_gc(struct mxc_gpio_port *port, int irq_base)
{
	struct irq_chip_generic *gc;
	struct irq_chip_type *ct;

	gc = irq_alloc_generic_chip("gpio-mxc", 1, irq_base,
				    port->base, handle_level_irq);
	gc->private = port;

	ct = gc->chip_types;
#ifndef CONFIG_P4_BSP_ARCH_MXC
	ct->chip.irq_ack = irq_gc_ack_set_bit;
	ct->chip.irq_mask = irq_gc_mask_clr_bit;
	ct->chip.irq_unmask = irq_gc_mask_set_bit;
#else
	ct->chip.irq_startup = P4INT_irq_type.irq_startup;
	ct->chip.irq_shutdown = P4INT_irq_type.irq_shutdown;
	ct->chip.irq_enable = P4INT_irq_type.irq_enable;
	ct->chip.irq_disable = P4INT_irq_type.irq_disable;
	ct->chip.irq_ack = P4INT_irq_type.irq_ack;
	ct->chip.irq_mask = P4INT_irq_type.irq_mask;
	ct->chip.irq_unmask = P4INT_irq_type.irq_unmask;
#endif
	ct->chip.irq_set_type = gpio_set_irq_type;
#ifndef CONFIG_P4_BSP_ARCH_MXC
	ct->chip.irq_set_wake = gpio_set_wake_irq;
#endif
	ct->regs.ack = GPIO_ISR;
	ct->regs.mask = GPIO_IMR;

	irq_setup_generic_chip(gc, IRQ_MSK(32), IRQ_GC_INIT_NESTED_LOCK,
			       IRQ_NOREQUEST, 0);
}

static void mxc_gpio_get_hw(struct platform_device *pdev)
{
	const struct of_device_id *of_id =
			of_match_device(mxc_gpio_dt_ids, &pdev->dev);
	enum mxc_gpio_hwtype hwtype;

	if (of_id)
		pdev->id_entry = of_id->data;
	hwtype = pdev->id_entry->driver_data;

	if (mxc_gpio_hwtype) {
		/*
		 * The driver works with a reasonable presupposition,
		 * that is all gpio ports must be the same type when
		 * running on one soc.
		 */
		BUG_ON(mxc_gpio_hwtype != hwtype);
		return;
	}

	if (hwtype == IMX35_GPIO)
		mxc_gpio_hwdata = &imx35_gpio_hwdata;
	else if (hwtype == IMX31_GPIO)
		mxc_gpio_hwdata = &imx31_gpio_hwdata;
	else
		mxc_gpio_hwdata = &imx1_imx21_gpio_hwdata;

	mxc_gpio_hwtype = hwtype;
}

static int mxc_gpio_to_irq(struct gpio_chip *gc, unsigned offset)
{
	struct bgpio_chip *bgc = to_bgpio_chip(gc);
	struct mxc_gpio_port *port =
		container_of(bgc, struct mxc_gpio_port, bgc);

#ifdef CONFIG_P4_BSP_ARCH_MXC
	offset += MXC_GPIO_IRQ_START + gc->base;
#endif /* CONFIG_P4_BSP_ARCH_MXC */

	return irq_find_mapping(port->domain, offset);
}

#ifdef CONFIG_P4_BSP_ARCH_MXC
int p4_mxc_gpio_irq_xlate(struct irq_domain *d,
			  struct device_node *controller,
			  const u32 *intspec, unsigned int intsize,
			  unsigned long *out_hwirq, unsigned int *out_type)
{
	int irq_base;

	if (intsize < 2)
		return -EINVAL;

	irq_base = MXC_GPIO_IRQ_START
		+ of_alias_get_id(controller, "gpio") * 32;

	*out_hwirq = irq_base + intspec[0];
	*out_type = intspec[1];

	return 0;
}
#endif

#ifdef CONFIG_P4_VIRT_GPIO
static void mxc_gpio_set(struct gpio_chip *chip, unsigned offset, int value)
{
	P4_e_t err;
	P4_uint32_t lvl = (value & 1)? GPIO_LVL_HIGH: GPIO_LVL_LOW;
	DBG_GPIO("%s \n", __func__);
	err = gpio_pin_write(gpio_dev, chip->base / GPIO_MAX_PIN, offset, lvl);
	DBG_GPIO("GPIO PikeOS kernel device (port #%u pin #%2u set to %u), P4 res = %u\n",
	chip->base / GPIO_MAX_PIN + 1, offset, lvl, err);
	if (err < 0) die_runtime("### GPIO set failed with error #%d!", err);
}

static int mxc_gpio_get(struct gpio_chip *chip, unsigned offset)
{
	P4_e_t err;
	int psr;
	DBG_GPIO("%s \n", __func__);
	err = gpio_pin_read(gpio_dev, chip->base / GPIO_MAX_PIN, offset, 0, (P4_uint32_t *)&psr);
	DBG_GPIO("GPIO PikeOS kernel device (port #%u pin #%2u read %d), P4 res = %u\n",
	chip->base / GPIO_MAX_PIN + 1, offset, psr, err);
	if (err < 0) die_runtime("### GPIO get failed with error #%d!", err);
	return psr;
}

static int mxc_gpio_direction_output(struct gpio_chip *chip,
                     unsigned offset, int value)
{
	P4_e_t err;
	P4_uint32_t lvl = (value & 1)? GPIO_LVL_HIGH: GPIO_LVL_LOW;
	DBG_GPIO("%s \n", __func__);
	err = gpio_pin_setdir(gpio_dev, chip->base / GPIO_MAX_PIN, offset, GPIO_DIR_OUTPUT, lvl);
	DBG_GPIO("GPIO PikeOS kernel device (port #%u pin #%2u to output), P4 res = %u\n",
	chip->base / GPIO_MAX_PIN + 1, offset, err);
	return (err != P4_E_OK)? -p4_err_to_linux(err): 0;
}

static int mxc_gpio_direction_input(struct gpio_chip *chip, unsigned offset)
{
	P4_e_t err;
	DBG_GPIO("%s \n", __func__);
	err = gpio_pin_setdir(gpio_dev, chip->base / GPIO_MAX_PIN, offset, GPIO_DIR_INPUT, 0);
	DBG_GPIO("GPIO PikeOS kernel device (port #%u pin #%2u to  input), P4 res = %u\n",
	chip->base / GPIO_MAX_PIN + 1, offset, err);
	return (err != P4_E_OK)? -p4_err_to_linux(err): 0;
}
/* This function has been implemented to memic the original bgpio_init() function to set 
   GPIO access functions by the suported PikeOS PSP function */
int bgpio_p4_init(struct bgpio_chip *bgc, struct device *dev)
{
	int ret;
	char gpio_device_name[VMAPI_NAME_LEN];
	sprintf(gpio_device_name, "prop:drv/gpio/%s", vmapi_get_partname());
	DBG_GPIO("Accessing GPIO device %s\n", gpio_device_name);
	ret = vmapi_property_device_grant(gpio_device_name, (int *)&gpio_dev);
	DBG_GPIO("GPIO PikeOS kernel device #%d\n", gpio_dev);
	if (ret < 0) die_early("### GPIO device access failed with error #%d!", ret);

	bgc->gc.dev = dev;
	bgc->gc.label = dev_name(dev);
	bgc->gc.base = -1;
	bgc->bits = GPIO_MAX_PIN;  /* Hardcoded for now */
	bgc->gc.ngpio = bgc->bits;
	bgc->gc.set = mxc_gpio_set;
	bgc->gc.get = mxc_gpio_get;
	bgc->gc.direction_output = mxc_gpio_direction_output;
	bgc->gc.direction_input = mxc_gpio_direction_input;

	return ret;
}

#endif /* #ifdef CONFIG_P4_VIRT_GPIO  */
static int mxc_gpio_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct mxc_gpio_port *port;
	struct resource *iores;
	int irq_base;
	int err;

	mxc_gpio_get_hw(pdev);

	port = devm_kzalloc(&pdev->dev, sizeof(*port), GFP_KERNEL);
	if (!port)
		return -ENOMEM;

#ifndef CONFIG_P4_VIRT_GPIO
	iores = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	port->base = devm_ioremap_resource(&pdev->dev, iores);
	if (IS_ERR(port->base))
		return PTR_ERR(port->base);
#endif /* #ifndef CONFIG_P4_VIRT_GPIO */

#ifndef CONFIG_P4_BSP_ARCH_MXC
	port->irq_high = platform_get_irq(pdev, 1);
	port->irq = platform_get_irq(pdev, 0);
	if (port->irq < 0)
		return port->irq;

	/* disable the interrupt and clear the status */
	writel(0, port->base + GPIO_IMR);
	writel(~0, port->base + GPIO_ISR);

	if (mxc_gpio_hwtype == IMX21_GPIO) {
		/*
		 * Setup one handler for all GPIO interrupts. Actually setting
		 * the handler is needed only once, but doing it for every port
		 * is more robust and easier.
		 */
		irq_set_chained_handler(port->irq, mx2_gpio_irq_handler);
	} else {
		/* setup one handler for each entry */
		irq_set_chained_handler(port->irq, mx3_gpio_irq_handler);
		irq_set_handler_data(port->irq, port);
		if (port->irq_high > 0) {
			/* setup handler for GPIO 16 to 31 */
			irq_set_chained_handler(port->irq_high,
						mx3_gpio_irq_handler);
			irq_set_handler_data(port->irq_high, port);
		}
	}
#endif /* CONFIG_P4_BSP_ARCH_MXC */

#ifdef CONFIG_P4_VIRT_GPIO
	/* This function has been implemented to mimic the original function to set 
	GPIO access functions by the suported PikeOS PSP function */
	err = bgpio_p4_init(&port->bgc, &pdev->dev);
#else /* #ifdef CONFIG_P4_VIRT_GPIO */
	err = bgpio_init(&port->bgc, &pdev->dev, 4,
			 port->base + GPIO_PSR,
			 port->base + GPIO_DR, NULL,
			 port->base + GPIO_GDIR, NULL, 0);

#endif /* #ifdef CONFIG_P4_VIRT_GPIO */

	if (err)
		goto out_bgio;

	port->bgc.gc.to_irq = mxc_gpio_to_irq;
	port->bgc.gc.base = (pdev->id < 0) ? of_alias_get_id(np, "gpio") * 32 :
					     pdev->id * 32;

	err = gpiochip_add(&port->bgc.gc);
	if (err)
		goto out_bgpio_remove;

#ifndef CONFIG_P4_BSP_ARCH_MXC
	irq_base = irq_alloc_descs(-1, 0, 32, numa_node_id());
	if (irq_base < 0) {
		err = irq_base;
		goto out_gpiochip_remove;
	}
#else
	irq_base = MXC_GPIO_IRQ_START + port->bgc.gc.base;
#endif

#ifndef CONFIG_P4_BSP_ARCH_MXC
	port->domain = irq_domain_add_legacy(np, 32, irq_base, 0,
					     &irq_domain_simple_ops, NULL);
#else
	port->domain = irq_find_host(of_irq_find_parent(np));
#endif /* CONFIG_P4_BSP_ARCH_MXC */
	if (!port->domain) {
		err = -ENODEV;
		goto out_irqdesc_free;
	}

	/* gpio-mxc can be a generic irq chip */
	mxc_gpio_init_gc(port, irq_base);

	list_add_tail(&port->node, &mxc_gpio_ports);

	return 0;

out_irqdesc_free:
#ifndef CONFIG_P4_BSP_ARCH_MXC
	irq_free_descs(irq_base, 32);
out_gpiochip_remove:
#endif
	WARN_ON(gpiochip_remove(&port->bgc.gc) < 0);
out_bgpio_remove:
	bgpio_remove(&port->bgc);
out_bgio:
	dev_info(&pdev->dev, "%s failed with errno %d\n", __func__, err);
	return err;
}

static struct platform_driver mxc_gpio_driver = {
	.driver		= {
		.name	= "gpio-mxc",
		.owner	= THIS_MODULE,
		.of_match_table = mxc_gpio_dt_ids,
	},
	.probe		= mxc_gpio_probe,
	.id_table	= mxc_gpio_devtype,
};

static int __init gpio_mxc_init(void)
{
	return platform_driver_register(&mxc_gpio_driver);
}
postcore_initcall(gpio_mxc_init);

MODULE_AUTHOR("Freescale Semiconductor, "
	      "Daniel Mack <danielncaiaq.de>, "
	      "Juergen Beisert <kernel@pengutronix.de>");
MODULE_DESCRIPTION("Freescale MXC GPIO");
MODULE_LICENSE("GPL");
