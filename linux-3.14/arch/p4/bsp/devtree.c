/*
 * Copyright (C) 2015 OpenSynergy GmbH.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/of.h>
#include <linux/of_fdt.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/vmapi_calls.h>

void __init dump_machine_table(void)
{
	const struct machine_desc *p;

	vmapi_cprintf("Available machine support:\n\nID (hex)\tNAME\n");
	for_each_machine_desc(p)
		vmapi_cprintf("%08x\t%s\n", p->nr, p->name);

	vmapi_cprintf("\nPlease check your kernel config and/or bootloader.\n");

	/* can't use cpu_relax() here as it may require MMU setup */;
	while (true)
		;
}

static const void * __init arch_get_next_mach(const char *const **match)
{
	static const struct machine_desc *mdesc = __arch_info_begin;
	const struct machine_desc *m = mdesc;

	if (m >= __arch_info_end)
		return NULL;

	mdesc++;
	*match = m->dt_compat;
	return m;
}

const struct machine_desc * __init setup_machine_fdt(void *dt_addr)
{
	const struct machine_desc *mdesc, *mdesc_best = NULL;

	if (!dt_addr || !early_init_dt_scan(dt_addr))
		return NULL;

	mdesc = of_flat_dt_match_machine(mdesc_best, arch_get_next_mach);

	if (!mdesc) {
		const char *prop;
		long size;
		unsigned long dt_root;

		vmapi_cprintf("\nError: unrecognized/unsupported "
			    "device tree compatible list:\n[ ");

		dt_root = of_get_flat_dt_root();
		prop = of_get_flat_dt_prop(dt_root, "compatible", &size);
		while (size > 0) {
			vmapi_cprintf("'%s' ", prop);
			size -= strlen(prop) + 1;
			prop += strlen(prop) + 1;
		}

		dump_machine_table(); /* does not return */
	}

	/* Change machine number to match the mdesc we're using */
	__machine_arch_type = mdesc->nr;

	return mdesc;
}
