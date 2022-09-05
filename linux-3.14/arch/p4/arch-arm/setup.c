/*
 * linux/arch/p4/arch-arm/setup.c - ARM specific setup
 *
 * 2009-09-14, David Engraf <den@sysgo.com>
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/utsname.h>
#include <linux/module.h>
#include <asm/string.h>
#include <asm/internals.h>
#include <asm/host_tasks.h>
#include <asm/elf.h>
#include <asm/cacheflush.h>
#include <asm/debugout.h>
#include <asm/system_info.h>
#include "signal.h"

unsigned int elf_hwcap;
EXPORT_SYMBOL(elf_hwcap);

char elf_platform[ELF_PLATFORM_SIZE];
EXPORT_SYMBOL(elf_platform);

#define ENDIANNESS		'l'

static void __init check_vector_page(void)
{
	int ret;
	/*
	 * Check signal return handlers in the vector page
	 */
	ret = memcmp((void *)(KERN_SIGRETURN_CODE),
	             sigreturn_codes, sizeof(sigreturn_codes));
	ret |= memcmp((void *)(KERN_RESTART_CODE),
	              syscall_restart_code, sizeof(syscall_restart_code));

	if (ret != 0)
		die_early("Linux signal return handlers mismatch");
}

void __init setup_sub_arch(void)
{
	unsigned int regnum;

	sprintf(init_utsname()->machine, "armv%d%c", __LINUX_ARM_ARCH__, ENDIANNESS);
	sprintf(elf_platform, "v%d%c", __LINUX_ARM_ARCH__, ENDIANNESS);
	
	elf_hwcap = HWCAP_SWP|HWCAP_HALF|HWCAP_THUMB|HWCAP_FAST_MULT|HWCAP_EDSP;
#ifndef CONFIG_ARM_THUMB
	elf_hwcap &= ~HWCAP_THUMB;
#endif

	/* VFP caps */
	regnum = uk_task_kinfo_reg_size() / sizeof(int);
	if (regnum > 20) {
		/* VFP registers available, we have at least ARMv6 */
		elf_hwcap |= HWCAP_VFP;
		/* with full registers we assume ARMv7 with NEON */
		if (regnum == HOST_FRAME_SIZE)
			elf_hwcap |= HWCAP_VFPv3 | HWCAP_NEON;
	}

	check_vector_page();
}

int cpu_architecture(void)
{
#if __LINUX_ARM_ARCH__ == 6
	return CPU_ARCH_ARMv6;
#elif __LINUX_ARM_ARCH__ == 7
	return CPU_ARCH_ARMv7;
#else
#error Unknown ARM architecture version, please adapt here
#endif
}
