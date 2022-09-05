/* Copyright (C) 2013-2014 OpenSynergy GmbH
 * This file is part of the OpenSynergy L2LV framework
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this file; if not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef __VBD_DEBUG_H__
#define __VBD_DEBUG_H__

#include <linux/kernel.h>

//#define DEBUG_VBD

// always printk LOG and INFO
#define LOG(fmt, args...) printk("%s:%d: " fmt, __FUNCTION__, __LINE__, ##args)
#define INFO(fmt, args...) printk(KERN_INFO fmt, ##args)

#ifdef DEBUG_VBD
#   define DBG(fmt, args...) LOG(fmt, ##args)
#	define DUMP(ptr, size) dump(__FUNCTION__, __LINE__, ptr, size)

	void dump(const char *func, int line, char *ptr, int size);
#else
#   define DBG(fmt, args...)
#	define DUMP(ptr, size)
#endif

#endif /* __VBD_DEBUG_H__ */
