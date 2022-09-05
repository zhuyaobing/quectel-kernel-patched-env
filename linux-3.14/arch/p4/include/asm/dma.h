#ifndef __P4_DMA_H
#define __P4_DMA_H

#include "asm/io.h"

/* on i386, this is PAGE_OFFSET + 0x10000000 */
#define MAX_DMA_ADDRESS (0x10000000)

/* on PPC and ARM, ISA style DMA isn't supported at all, but we have to define this */
#if defined (CONFIG_P4_PPC) || defined (CONFIG_P4_ARM)
#define ISA_DMA_THRESHOLD	(~0UL)
#endif

/*
 * We don't support ISA style DMA at all, so we don't set MAX_DMA_CHANNELS,
 * and do not define request_dma() and free_dma() at all.
 */

#ifdef CONFIG_PCI
extern int isa_dma_bridge_buggy;
#else
#define isa_dma_bridge_buggy   (0)
#endif


#endif
