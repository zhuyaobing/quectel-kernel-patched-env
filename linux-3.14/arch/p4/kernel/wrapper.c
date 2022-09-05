/*
 *  Wrapper against Linux
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 * 
 *  Copyright (C) 2004-2015 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 *  Version 2004-01-27, azu: initial
 */

#include <linux/init.h>
#include <linux/bootmem.h>
#include <asm/memory.h>
#include <asm/internals.h>

#if PIKEOS_VM_VERSION >= 34
/* Used by libvm and not available because we do not use libstand. An alias
 * would be possible as well, but we don't want to modify the generic part of
 * the kernel.
 */
int _st_snprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buf, size, fmt, args);
	va_end(args);

	return i;
}
#endif

void __init *wrapped_malloc(unsigned long size)
{
	void *b;
	
	b = alloc_bootmem(size);
	if (b)
		memset(b, 0, size);
	return b;
}
