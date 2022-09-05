/* Copyright (c) 2016 OpenSynergy GmbH */
/*
 * linux/arch/p4/include/asm/memory.h
 *
 * memory layout for P4 Linux
 *
 * PikeOS uses a 2G/2G memory split between the PikeOS kernel and the partition,
 * thus the Linux kernel can access the first 2 GB virtual address space only.
 * The Linux kernel uses distinct 2 GB of virtual address space for kernel and
 * user space. That means the Linux kernel cannot directly access user space
 * memory, like on a native Linux system. This results in a small performance
 * impact due to TLB flushes when entering the kernel space from an application.
 * On the other side the remaing 2 GB doesn't need further splits and the kernel
 * as well as the user space can access the whole address space.
 *
 * written by Alex Zuepke, SYSGO AG <azu at sysgo dot de>
 */

#ifndef __P4_MEMORY_H__
#define __P4_MEMORY_H__

#include <asm-generic/sizes.h>

#ifndef __ASSEMBLY__
extern unsigned long kseg_phys_offset;
extern unsigned long start_vm;
extern unsigned long consistent_start;
extern unsigned long consistent_size;
#endif

/* Kernel virtual address space, equivalent to P4_MEM_USR_BASE and
 * P4_MEM_USR_END */
#define KSEG		0x00000000
#define KSEG_BASE	0x00020000

#ifdef CONFIG_64BIT
#define KSEG_END	0x7ffffffff000
#else
#define KSEG_END	0x80000000
#endif

#define PAGE_OFFSET	(KSEG)
#define VMALLOC_OFFSET	(SZ_1M)
#define VMALLOC_START	(start_vm+VMALLOC_OFFSET)
#define VMALLOC_END	(KSEG_END)

#define MODULES_VADDR	VMALLOC_START
#define MODULES_END		VMALLOC_END
#define MODULES_LEN		(MODULES_VADDR - MODULES_END)

#ifdef CONFIG_P4_ARM
#define arch_is_coherent()              0

#include <linux/const.h>
#include <asm/sizes.h>

/*
 * Allow for constants defined here to be used from assembly code
 * by prepending the UL suffix only with actual C code compilation.
 */
#define UL(x) _AC(x, UL)

#endif

#endif /* __P4_MEMORY_H__ */

