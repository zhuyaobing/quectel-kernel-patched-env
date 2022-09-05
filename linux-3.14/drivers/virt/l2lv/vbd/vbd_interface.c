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

#include <linux/syscalls.h>
#include <linux/module.h>
#include "vbd_interface.h"

long vbd_sys_ioctl(char *file_name)
{
	mm_segment_t old_fs = get_fs();
	int fd;
	int ret = 0;

	set_fs(KERNEL_DS);
	fd = sys_open(file_name, O_RDWR, 0);

	if (IS_ERR_VALUE(fd)){
		set_fs(old_fs);
		return fd;
	}

	ret = sys_ioctl(fd, BLKFLSBUF, 0);

	sys_close(fd);
	set_fs(old_fs);

	return ret;
}
EXPORT_SYMBOL_GPL(vbd_sys_ioctl);
