#include "linux/module.h"
#include "linux/in6.h"
#include "linux/rwsem.h"
#include "asm/byteorder.h"
#include "asm/uaccess.h"
#include "asm/checksum.h"
#include "asm/errno.h"
#include "asm/cacheflush.h"
#include "asm/delay.h"
#include "asm/io.h"
#include "asm/cacheflush.h"
#include "asm/page.h"
#include "asm/pci.h"

extern void __ide_mm_insw(void __iomem *port, void *addr, u32 count);
extern void __ide_mm_outsw(void __iomem *port, void *addr, u32 count);
extern void __ide_mm_insl(void __iomem *port, void *addr, u32 count);
extern void __ide_mm_outsl(void __iomem *port, void *addr, u32 count);

long long __ashrdi3(long long, int);
long long __ashldi3(long long, int);
long long __lshrdi3(long long, int);

/* misc.S */
EXPORT_SYMBOL(__delay);
EXPORT_SYMBOL(_insb);
EXPORT_SYMBOL(_outsb);
EXPORT_SYMBOL(_insw);
EXPORT_SYMBOL(_outsw);
EXPORT_SYMBOL(_insl);
EXPORT_SYMBOL(_outsl);
EXPORT_SYMBOL(__ide_mm_insw);
EXPORT_SYMBOL(_insw_ns);
EXPORT_SYMBOL(__ide_mm_outsw);
EXPORT_SYMBOL(_outsw_ns);
EXPORT_SYMBOL(__ide_mm_insl);
EXPORT_SYMBOL(_insl_ns);
EXPORT_SYMBOL(__ide_mm_outsl);
EXPORT_SYMBOL(_outsl_ns);
EXPORT_SYMBOL(__ashrdi3);
EXPORT_SYMBOL(__ashldi3);
EXPORT_SYMBOL(__lshrdi3);

/* cache flushing stuff */
EXPORT_SYMBOL(flush_dcache_range);
EXPORT_SYMBOL(flush_instruction_cache);

/* string.S */
EXPORT_SYMBOL(strcpy);
EXPORT_SYMBOL(strncpy);
EXPORT_SYMBOL(strcat);
EXPORT_SYMBOL(strcmp);
EXPORT_SYMBOL(strlen);
EXPORT_SYMBOL(cacheable_memzero);
EXPORT_SYMBOL(memset);
EXPORT_SYMBOL(memmove);
EXPORT_SYMBOL(memcpy);
EXPORT_SYMBOL(memcmp);
EXPORT_SYMBOL(memchr);

/* checksum.S */
EXPORT_SYMBOL(ip_fast_csum);
EXPORT_SYMBOL(csum_tcpudp_magic);
EXPORT_SYMBOL(csum_partial);
EXPORT_SYMBOL(csum_partial_copy_generic);

#ifdef CONFIG_PCI
EXPORT_SYMBOL(isa_io_base);
EXPORT_SYMBOL(isa_mem_base);
EXPORT_SYMBOL(pci_dram_offset);
EXPORT_SYMBOL(pci_alloc_consistent);
EXPORT_SYMBOL(pci_free_consistent);
EXPORT_SYMBOL(pci_bus_io_base);
EXPORT_SYMBOL(pci_bus_io_base_phys);
EXPORT_SYMBOL(pci_bus_mem_base_phys);
EXPORT_SYMBOL(pci_bus_to_hose);
EXPORT_SYMBOL(pci_resource_to_bus);
EXPORT_SYMBOL(pci_phys_to_bus);
EXPORT_SYMBOL(pci_bus_to_phys);
#endif /* CONFIG_PCI */

#ifdef CONFIG_NOT_COHERENT_CACHE
extern void flush_dcache_all(void);
EXPORT_SYMBOL(flush_dcache_all);
#endif

