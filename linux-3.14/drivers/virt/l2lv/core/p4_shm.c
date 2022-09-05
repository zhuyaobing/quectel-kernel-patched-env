/*
 * Copyright (C) 2013-2016 OpenSynergy GmbH
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

#include <asm/tlbflush.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/vmalloc.h>

#include <core/vm_file.h>
#include <linux/l2lv/l2lv_shm.h>

#include "vm.h"

static vm_off_t get_sizeof_shm(vm_file_desc_t vm_shm_fd)
{
	vm_off_t size;
	vm_e_t err;

	err = vm_lseek(&vm_shm_fd, 0, VM_SEEK_END, &size);
	if (err != VM_E_OK)
		size = 0;

	return size;
}

static vm_e_t do_map_shm(struct l2lv_shm *shm_obj, vm_file_desc_t vm_shm_fd,
			 void *addr_to_map, vm_off_t shm_size)
{
	vm_protect((unsigned long)addr_to_map, shm_size);
	shm_obj->kernel_vaddr = addr_to_map;
	shm_obj->size = shm_size;
	return vm_map(&vm_shm_fd, 0, shm_size, VM_MEM_ACCESS_RDWR, 0,
		      (P4_address_t) addr_to_map);
}

#define SG_SIZE 4

int l2lv_map_shm(struct l2lv_shm *shm_obj, const char *i_shm_identifier)
{
	vm_off_t shm_size;
	unsigned long addr;
	vm_e_t vm_e;
	struct vm_struct *vm_obj;
	int retval = -ENOMEM;
	vm_file_desc_t vm_shm_fd;
	int id;
	uint32_t sgsize = SG_SIZE;
	P4_sglist_t sglist[SG_SIZE];

	shm_obj->phys_addr = 0U;
	vm_e = vm_open(i_shm_identifier, VM_O_RW | VM_O_MAP, &vm_shm_fd);
	if (vm_e != VM_E_OK) {
		pr_err("error: could not open shared memory \"%s\", errorcode = %d\n",
		    i_shm_identifier, vm_e);
		return retval;
	}
	shm_size = get_sizeof_shm(vm_shm_fd);
	if (shm_size == 0) {
		pr_err("error: could not determine size of shared memory \"%s\"\n",
		    i_shm_identifier);
		goto error;
	}
	vm_obj = get_vm_area(PAGE_ALIGN(shm_size), VM_IOREMAP);
	if (!vm_obj) {
		pr_err("error: get_vm_area failed\n");
		goto error;
	}
	addr = (unsigned long)vm_obj->addr;
	if (!addr) {
		pr_err("error: could not map shared memory buffer\n");
		goto error;
	}
	vm_e = do_map_shm(shm_obj, vm_shm_fd, vm_obj->addr, shm_size);
	if (vm_e != VM_E_OK) {
		pr_err("error: could not map shared memory \"%s\" at address 0x%08lx, errorcode = %d\n",
		    i_shm_identifier, addr, vm_e);
		goto error2;
	}
	p4_mem_build_sglist(P4_TASK_MYSELF, (P4_address_t) vm_obj->addr,
			    shm_size, sglist, &sgsize);
	for (id = 0; id < sgsize; ++id) {
		if ((uint32_t) sglist[id].virt == addr)
			shm_obj->phys_addr = (uint32_t) sglist[id].phys;
	}

	(void)vm_close(&vm_shm_fd);
	return 0;

 error2:
	remove_vm_area(vm_obj->addr);
	shm_obj->kernel_vaddr = 0;
 error:
	/* no vm_unmap() available... */
	(void)vm_close(&vm_shm_fd);
	return retval;
}
EXPORT_SYMBOL(l2lv_map_shm);

void l2lv_unmap_shm(struct l2lv_shm *shm_obj)
{
	if (shm_obj->kernel_vaddr) {
		remove_vm_area(shm_obj->kernel_vaddr);
		shm_obj->kernel_vaddr = NULL;
	}
}
EXPORT_SYMBOL(l2lv_unmap_shm);

MODULE_LICENSE("LGPL");

