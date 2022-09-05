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
#ifndef _L2LV_DEBUG_H_
#define _L2LV_DEBUG_H_

#include <linux/kernel.h>
#include <linux/string.h>

#define __BASE_FILENAME__ (strrchr(__FILE__, '/') ? \
			   strrchr(__FILE__, '/') + 1 : \
			   __FILE__)
#define MAX_STRING_LEN 20
#ifdef CONFIG_L2LV_TEST
#define TEST_MSG(fmt, ...) pr_info(fmt "\n", ##__VA_ARGS__);
#else
#define TEST_MSG(fmt, ...)
#endif

#ifdef CONFIG_L2LV_DEBUG
#define DEBUG_MSG(fmt, ...) pr_debug("||<=DEBG=>|| %20s:%5d:%20s: " fmt "\n", \
		__BASE_FILENAME__, __LINE__, __func__, ##__VA_ARGS__);
#else
#define DEBUG_MSG(fmt, ...)
#endif

#endif
