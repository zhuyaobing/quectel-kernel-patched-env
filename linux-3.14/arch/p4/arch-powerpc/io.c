/*
 * linux/arch/p4/arch-ppc/io.c
 *
 * Board configuration stuff for P4/Linux on PowerPC 32
 *
 *
 */

#include <linux/types.h>
#include <linux/module.h>
#include <asm/io.h>
#include <asm/internals.h>
#include <asm/machdep.h>
#include <asm/host_context.h>

unsigned long isa_io_base = 0;
unsigned long isa_mem_base = 0;
unsigned long pci_dram_offset = 0;

/* our dummy machine, currently empty (no callbacks) */
struct machdep_calls ppc_md;
