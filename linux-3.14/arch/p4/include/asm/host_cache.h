/*
 *  cache handling
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 * 
 *  Copyright (C) 2006-2014 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#ifndef __HOST_CACHE_H_
#define __HOST_CACHE_H_

extern void uk_flush_dcache_range(unsigned long start, unsigned long size);
extern void uk_flush_dcache_all(void);
extern void uk_flush_icache_range(unsigned long start, unsigned long size);
extern void uk_inval_icache_all(void);
#ifdef CONFIG_P4_CACHEMGR_DRIVER
extern int uk_inval_dcache_range(unsigned long start, unsigned long size);
#endif
#endif
