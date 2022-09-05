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
#include <linux/kernel.h>

#include "vbd_debug.h"

void dump(const char *func, int line, char *ptr, int size) {
	int i, j;
	printk("===[DUMP from %s:%d]===\n", func, line);
	for (i=0; i<size; i+=8)
	{
		printk("    ");
		for (j=0;j<8; j++)
		{
			if (i+j >= size) {
				printk("\n");
				break;
			}
			printk("%#.8x ", ptr[i+j]);
		}
		printk("\n");
	}
}
