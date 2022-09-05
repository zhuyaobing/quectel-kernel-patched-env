/*
 * Copyright (c) 2012-2016 OpenSynergy GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/cache.h>
#include <linux/clk-provider.h>
#include <linux/clocksource.h>
#include <linux/dma-mapping.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/irqchip.h>
#include <linux/irqdomain.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/of_fdt.h>
#include <linux/of_platform.h>
#include <asm/cputype.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/smp_plat.h>
#include <asm/vmapi_calls.h>

#include "devtree.h"

unsigned int system_rev;
EXPORT_SYMBOL(system_rev);

unsigned int system_serial_low;
EXPORT_SYMBOL(system_serial_low);

unsigned int system_serial_high;
EXPORT_SYMBOL(system_serial_high);

const char *machine_name;
EXPORT_SYMBOL(machine_name);

unsigned int __machine_arch_type __read_mostly;
EXPORT_SYMBOL(__machine_arch_type);

const struct machine_desc *machine_desc __initdata;

/* arch/arm/kernel/setup.c */
u32 __cpu_logical_map[NR_CPUS] = { [0 ... NR_CPUS-1] = MPIDR_INVALID };

static int gic_irq_domain_xlate(struct irq_domain *d,
				struct device_node *controller,
				const u32 *intspec, unsigned int intsize,
				unsigned long *out_hwirq,
				unsigned int *out_type);

void __init bsp_init(void)
{
	void *dt_addr;
	const struct machine_desc *mdesc;

	system_serial_low = system_serial_high = 0;
	system_rev = 0;

	dt_addr = initial_boot_params;

	mdesc = setup_machine_fdt(dt_addr);
	if (!mdesc) {
		/* panic not supported at this stage */
		vmapi_cprintf("P4Linux ERROR: No machine descriptor\n");
		while (1)
			;
	}

	machine_desc = mdesc;
	machine_name = machine_desc->name;

	/* Same consistent DMA size as in Linux 3.0.35 i.MX6q BSP */
	init_consistent_dma_size(184 * SZ_1M);
}


#ifdef CONFIG_GPIO_MXC
int p4_mxc_gpio_irq_xlate(struct irq_domain *d,
			  struct device_node *controller,
			  const u32 *intspec, unsigned int intsize,
			  unsigned long *out_hwirq, unsigned int *out_type);
#else
static inline int p4_mxc_gpio_irq_xlate(struct irq_domain *d,
		struct device_node *controller,
		const u32 *intspec, unsigned int intsize,
		unsigned long *out_hwirq, unsigned int *out_type)
{
	return gic_irq_domain_xlate(d, controller, intspec, intsize,
				    out_hwirq, out_type);
}
#endif

int bsp_irq_xlate(struct irq_domain *d,
		  struct device_node *controller,
		  const u32 *intspec, unsigned int intsize,
		  unsigned long *out_hwirq, unsigned int *out_type)
{
	if (of_device_is_compatible(controller, "fsl,imx35-gpio")) {
		return p4_mxc_gpio_irq_xlate(d, controller, intspec, intsize,
					     out_hwirq, out_type);
	}

	return gic_irq_domain_xlate(d, controller, intspec, intsize,
			out_hwirq, out_type);
}

void __init bsp_time_init(void)
{
	/* Copied from arch/arm/kernel/time.c: time_init() */
	if (machine_desc->init_time) {
		machine_desc->init_time();
	} else {
#ifdef CONFIG_COMMON_CLK
		of_clk_init(NULL);
#endif
		clocksource_of_init();
	}
}

/* arch/arm/kernel/setup.c */
static int __init customize_machine(void)
{
	/*
	 * customizes platform devices, or adds new ones
	 * On DT based machines, we fall back to populating the
	 * machine from the device tree, if no callback is provided,
	 * otherwise we would always need an init_machine callback.
	 */
	if (machine_desc->init_machine)
		machine_desc->init_machine();
#ifdef CONFIG_OF
	else
		of_platform_populate(NULL, of_default_bus_match_table,
					NULL, NULL);
#endif
	return 0;
}
arch_initcall(customize_machine);

int __init arch_early_irq_init(void)
{
	/* Copied from arch/arm/kernel/irq.c: init_IRQ() */
	if (IS_ENABLED(CONFIG_OF) && !machine_desc->init_irq)
		irqchip_init();
	else
		machine_desc->init_irq();

	return 0;
}

/* arch/arm/kernel/setup.c */
static int __init init_machine_late(void)
{
	if (machine_desc->init_late)
		machine_desc->init_late();
	return 0;
}
late_initcall(init_machine_late);

/* drivers/irqchip/irq-gic.c */
static int gic_irq_domain_xlate(struct irq_domain *d,
				struct device_node *controller,
				const u32 *intspec, unsigned int intsize,
				unsigned long *out_hwirq,
				unsigned int *out_type)
{
	if (d->of_node != controller)
		return -EINVAL;
	if (intsize < 3)
		return -EINVAL;

	/* Get the interrupt number and add 16 to skip over SGIs */
	*out_hwirq = intspec[1] + 16;

	/* For SPIs, we need to add 16 more to get the GIC irq ID number */
	if (!intspec[0])
		*out_hwirq += 16;

	*out_type = intspec[2] & IRQ_TYPE_SENSE_MASK;
	return 0;
}

/* arch/arm/include/asm/smp.h */
struct smp_operations {
	/* Stub */
};

/* arch/arm/mach-imx/platsmp.c */
struct smp_operations imx_smp_ops __initdata = {
	/* Stub */
};

/* arch/arm/mach-imx/platsmp.c */
void __init imx_scu_map_io(void)
{
	pr_debug("%s: Ignore SCU initialization\n", __func__);
}

/* arch/arm/mach-imx/system.c */
void mxc_restart(char mode, const char *cmd)
{
	pr_err("%s: Ignore system restart\n", __func__);
}

/* arch/arm/mach-imx/system.c */
void __init mxc_arch_reset_init_dt(void)
{
	pr_debug("%s: Ignore watchdog initialization\n", __func__);
}

/* arch/arm/mach-imx/time.c */
void __init mxc_timer_init(void __iomem *base, int irq)
{
	pr_debug("%s: Ignore timer initialization\n", __func__);
}

/* arch/arm/mach-imx/time.c */
void __init mxc_timer_init_dt(struct device_node *np)
{
	pr_debug("%s: Ignore timer initialization\n", __func__);
}

/* arch/arm/mach-imx/gpc.c: */
void __init imx_gpc_init(void)
{
	pr_debug("%s: Ignore GPC initialization\n", __func__);
}

/* arch/arm/mach-imx/pm-imx6.c */
void __init imx6_pm_map_io(void)
{
	pr_debug("%s: Ignore power management initialization\n", __func__);
}

/* arch/arm/mach-imx/pm-imx6.c: */
int imx6q_set_lpm(int arg)
{
	pr_debug("%s: Ignore LPM setting\n", __func__);
	return 0;
}

/* arch/arm/mach-imx/pm-imx6.c: */
void __init imx6q_pm_init(void)
{
	pr_debug("%s: Ignore PM initialization\n", __func__);
}

/* arch/arm/mach-imx/pm-imx6.c: */
void __init imx6dl_pm_init(void)
{
	pr_debug("%s: Ignore PM initialization\n", __func__);
}

/* arch/arm/mach-imx/pm-imx6.c: */
void __init imx6q_pm_set_ccm_base(void __iomem *base)
{
	pr_debug("%s: Ignore PM initialization\n", __func__);
}
