/*
 * linux/arch/p4/arch-arm/iomap.c - ARM ioport functions
 *
 * 2009-09-22, David Engraf <den@sysgo.com>
 */

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/ioport.h>

#include <asm/io.h>

#ifdef __io
void __iomem *ioport_map(unsigned long port, unsigned int nr)
{
	return __io(port);
}
EXPORT_SYMBOL(ioport_map);

void ioport_unmap(void __iomem *addr)
{
}
EXPORT_SYMBOL(ioport_unmap);
#endif
