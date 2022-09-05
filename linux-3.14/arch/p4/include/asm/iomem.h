/*
 *  IO memory (and ports) management
 * 
 *  Copyright (C) 2014 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 *  Version 2004-01-26, azu: initial
 *
 */

#ifndef __IOMEM_H_
#define __IOMEM_H_

extern void iomem_init(void);
extern int iomem_find_bootram(unsigned long *start, unsigned long *size, unsigned long *phys);
extern void iomem_fill_table(unsigned long *highest_mapping);
extern unsigned long __iomem_find_io(unsigned long phys_addr, unsigned long size, unsigned int *cache_mode);
#define iomem_find_io(phys_addr, size) __iomem_find_io((phys_addr), (size), NULL)

#ifdef CONFIG_PCI_VIRT
extern unsigned long p4_pci_find_bar(unsigned long bar, unsigned long size);
extern int p4_pci_is_bar_mapping(unsigned long virt);
#endif

#endif /* __IOMEM_H_ */
