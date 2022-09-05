#ifndef __UM_TLB_H
#define __UM_TLB_H

#if defined(CONFIG_P4_ARM) || defined(CONFIG_P4_PPC) || defined(CONFIG_P4_X86)
#define tlb_start_vma(tlb, vma) do { } while (0)
#define tlb_end_vma(tlb, vma) do { } while (0)
#define __tlb_remove_tlb_entry(tlb, ptep, address) do { } while (0)
#define tlb_flush(tlb) flush_tlb_mm((tlb)->mm)

#include <asm-generic/tlb.h>
#else
#error Please check
#include <arch/asm/tlb.h>
#endif

#endif
