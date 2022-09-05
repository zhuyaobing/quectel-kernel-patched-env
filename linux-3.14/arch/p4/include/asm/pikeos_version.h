/*
 *  Copyright (C) 2014 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#ifndef __PIKEOS_VERSION_H_
#define __PIKEOS_VERSION_H_

#include <generated/pikeos_api_version.h>
#include <generated/pikeos_vm_version.h>

/* Use new driver ioctls from DDK API */
#define PIKEOS_DRV_API_DDK (PIKEOS_API_VERSION >= 0x40004)

#endif
