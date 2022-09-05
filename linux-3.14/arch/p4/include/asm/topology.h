#ifndef _ASM_P4_TOPOLOGY_H
#define _ASM_P4_TOPOLOGY_H

#include <asm-generic/topology.h>

#ifdef CONFIG_P4_X86
struct pci_bus;
void x86_pci_root_bus_resources(int bus, struct list_head *resources);

static inline int get_mp_bus_to_node(int busnum)
{
	return 0;
}
static inline void set_mp_bus_to_node(int busnum, int node)
{
}
#endif

#endif
