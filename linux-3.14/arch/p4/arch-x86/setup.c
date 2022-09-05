/*
 * linux/arch/p4/sys-i386/setup.c
 *
 * arch specific setup
 *
 * 2004jul06, azu@sysgo.de
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/utsname.h>
#include <linux/module.h>
#include <asm/string.h>
#include <asm/internals.h>
#include <asm/elf.h>
#include <asm/host_tasks.h>


/* we do not support the vsyscall stuff on PikeOS, but
 * provide these symbols to keep the code compatible with UM
 */
unsigned long vsyscall_ehdr;
unsigned long vsyscall_end;
unsigned long __kernel_vsyscall;
long elf_aux_hwcap;
char *elf_aux_platform;

/* For PCI or other memory-mapped resources */
unsigned long pci_mem_start = 0xaeedbabe;
EXPORT_SYMBOL(pci_mem_start);

#ifdef CONFIG_64BIT
/* used by UM VDSO */
unsigned long task_size = 0x7fc0000000;
EXPORT_SYMBOL(task_size);
#endif

static void __init setup_pci_mem_start(void)
{
	/* PR#2804-elinos: azu@sysgo.de:
	 * For PCI setup, we need to know the upper RAM limit of the kernel.
	 * Unfortunately, the kernel does not offer such a feature, so
	 * we take the amount of available RAM and add additional 64 MB
	 * to be safe.
	 */
	pci_mem_start = uk_global_total() * PAGE_SIZE + 0x04000000;
}

void __init e820_reserve_resources_late(void)
{
	/* called by the x86 pci layer on initialization, do nothing on PikeOS */
}

int generic_access_phys(struct vm_area_struct *vma, unsigned long addr,
			void *buf, int len, int write)
{
	printk(KERN_ERR "%s not supported\n", __func__);
	return -EINVAL;
}

void __init setup_sub_arch(void)
{
#ifndef CONFIG_64BIT
	unsigned int family;
#endif

	detect_x86_cpu();

#ifdef CONFIG_64BIT
	strcpy(init_utsname()->machine, "x86_64");
#else
	/* extract processor family */
	family = boot_cpu_data.x86;
	if (family > 6)
		family = 6;
	strcpy(init_utsname()->machine, "i386");
	init_utsname()->machine[1] = '0' + family;
#endif

	elf_aux_platform = init_utsname()->machine;
	elf_aux_hwcap = boot_cpu_data.x86_capability[0];

	setup_pci_mem_start();
}
