/* Copyright (c) 2016 OpenSynergy GmbH */
/*
 *  Copyright (C) 2006-2011, SYSGO AG
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

/* Linux headers (keep as small as possible to avoid name clashes) */
#include <linux/export.h>		/* for EXPORT_SYMBOL */
#include <linux/string.h>

#include "p4.h"
#include <asm/host_cache.h>

/* NOTE: this file isn't used on PPC */

void uk_flush_dcache_range(unsigned long start, unsigned long size)
{
	/* flushes dcache range */
	p4_flush_dcache_range(start, size);
}
EXPORT_SYMBOL(uk_flush_dcache_range);

void uk_flush_dcache_all(void)
{
	/* flushes complete dcache */
	p4_kernel_control(P4_KCTL(P4_KCTL_PSP, P4_KCTL_PSP_FLUSH_DCACHE), 0, 0, 0, 0);
}

void uk_flush_icache_range(unsigned long start, unsigned long size)
{
	/* flushes dcache range */
	/* invalidates icache range */
	p4_flush_icache_range(start, size);
}

void uk_inval_icache_all(void)
{
	/* invalidates complete icache */
	p4_kernel_control(P4_KCTL(P4_KCTL_PSP, P4_KCTL_PSP_FLUSH_ICACHE), 0, 0, 0, 0);
}
