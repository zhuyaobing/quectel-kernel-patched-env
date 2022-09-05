/*
 *  Generic outer cache functions
 * 
 *  Author: David Engraf <david.engraf@sysgo.com>
 * 
 *  Copyright (C) 2013 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 */

#include <linux/module.h>
#include <asm/cacheflush.h>

struct outer_cache_fns outer_cache __read_mostly;
EXPORT_SYMBOL(outer_cache);

static void outer_cache_sync(void)
{
	int temp;

	/* the pseudo cache flush makes sure that the L2 controller is also
	 * synchronized
	 */
	uk_flush_dcache_range((unsigned long)&temp, sizeof(temp));
}

static int outer_cache_init(void)
{
	memset(&outer_cache, 0, sizeof(outer_cache));

	outer_cache.sync = outer_cache_sync;

	/* The other functions are already handled by PikeOS when using the
	 * normal cache functions.
	 */

	printk(KERN_INFO "PikeOS outer cache controller enabled\n");
	return 0;
}

arch_initcall(outer_cache_init);
