/*
 *  Simple printf output, for debugging only
 * 
 *  Copyright (C) 2003-2014 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 * 
 *  2003-03-31, azu: prevent UML from using user space libs
 *  2006-06-19, azu: replaced with macros calling vm_cprintf()
 *  2009-12-11, den: call vmapi_cprintf wrapper instead of vm_cprintf
 */

#ifndef __DEBUGOUT_H__
#define __DEBUGOUT_H__

#include <asm/vmapi_calls.h>

#define debugout(a...)	\
	do {	\
		vmapi_cprintf(a);	\
		vmapi_cprintf("\n");	\
	} while (0)

#define die_early(a...)	\
	do {	\
		vmapi_cprintf("die_early: ");	\
		vmapi_cprintf(a);	\
		vmapi_cprintf("\n");	\
		vmapi_partition_error(1, "init error");	\
	} while (0)

#define die_runtime(a...)	\
	do {	\
		vmapi_cprintf("die_runtime: ");	\
		vmapi_cprintf(a);	\
		vmapi_cprintf("\n");	\
		vmapi_partition_error(0, "runtime error");	\
	} while (0)

#endif /*  __DEBUGOUT_H__ */

