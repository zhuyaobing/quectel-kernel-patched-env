/* Copyright (C) 2013-2016 OpenSynergy GmbH
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
#ifndef _L2LV_SHM_H_
#define _L2LV_SHM_H_

struct l2lv_shm {
	void *kernel_vaddr;
	unsigned long size;
	phys_addr_t phys_addr;
};

/*
 * Map shared memory to virtual kernel address
 */
int l2lv_map_shm(struct l2lv_shm *shm_obj, const char *i_shm_identifier);

/*
 * Cleanup shared memory map object
 */
void l2lv_unmap_shm(struct l2lv_shm *shm_obj);

#endif				/* _L2LV_SHM_H_ */
