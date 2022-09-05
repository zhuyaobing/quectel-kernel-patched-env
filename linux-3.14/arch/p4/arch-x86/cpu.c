/*
 * linux/arch/p4/arch-x86/cpu.c
 *
 * Simplified CPU detection, taken from arch/x86/kernel/cpu/common.c
 * This covers the minimum setup we need to handle alternative instructions
 *
 * 2011oct19, azu@sysgo.de
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/utsname.h>
#include <asm/string.h>
#include <asm/internals.h>
#include <asm/elf.h>
#include <asm/cpufeature.h>
#include <asm/asm.h>

static void __cpuinit get_cpu_cap(struct cpuinfo_x86 *c)
{
	u32 tfms, xlvl;
	u32 ebx;

	/* Intel-defined flags: level 0x00000001 */
	if (c->cpuid_level >= 0x00000001) {
		u32 capability, excap;

		cpuid(0x00000001, &tfms, &ebx, &excap, &capability);
		c->x86_capability[0] = capability;
		c->x86_capability[4] = excap;
	}

	/* Additional Intel-defined flags: level 0x00000007 */
	if (c->cpuid_level >= 0x00000007) {
		u32 eax, ebx, ecx, edx;

		cpuid_count(0x00000007, 0, &eax, &ebx, &ecx, &edx);

		c->x86_capability[9] = ebx;
	}

	/* AMD-defined flags: level 0x80000001 */
	xlvl = cpuid_eax(0x80000000);
	c->extended_cpuid_level = xlvl;

	if ((xlvl & 0xffff0000) == 0x80000000) {
		if (xlvl >= 0x80000001) {
			c->x86_capability[1] = cpuid_edx(0x80000001);
			c->x86_capability[6] = cpuid_ecx(0x80000001);
		}
	}

	// we don't need any scattered features for P4Linux
	//init_scattered_cpuid_features(c);
}

void __cpuinit detect_x86_cpu(void)
{
	struct cpuinfo_x86 *c = &boot_cpu_data;
	unsigned int *vendor_id_int;
	unsigned int eax, ebx, ecx, edx;
	unsigned int i;

	c->x86_clflush_size = 32;

	for (i = 0; i < NCAPINTS; i++)
		c->x86_capability[i] = 0;

	/* simple CPU detection scheme: we do a maybe-faulting CPUID
	 * and take a look at the processor version! */
	__asm__ __volatile__(
		"1:	cpuid\n"
		"2:\n"
		".section .fixup,\"ax\"\n"
		"3:	xorl %%eax,%%eax\n"
		"	jmp 2b\n"
		".previous\n"
		_ASM_EXTABLE(1b,3b)
		: "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
		: "0" (1), "c"(0));

	if (!eax)
		panic("CPU with CPUID instruction required!");

	c->x86 = (eax >> 8) & 0xf;
	c->x86_model = (eax >> 4) & 0xf;
	c->x86_mask = (eax >> 0) & 0xf;

	c->x86_capability[0] = edx;

	if (edx & (1<<19)) {
		c->x86_clflush_size = ((ebx >> 8) & 0xff) * 8;
	}

	/* get vendor string */
	cpuid(0, &eax, &ebx, &ecx, &edx);

	c->cpuid_level = eax;
	vendor_id_int = (unsigned int *) &c->x86_vendor_id;
	vendor_id_int[0] = ebx;
	vendor_id_int[1] = edx;
	vendor_id_int[2] = ecx;
	vendor_id_int[3] = 0;	/* string terminator */
	c->extended_cpuid_level = 0;

	/* currently limited to Intel and AMD CPUs. Add more when necessary */
	if (!strcmp(c->x86_vendor_id, "GenuineIntel")) {
		c->x86_vendor = X86_VENDOR_INTEL;
	} else if (!strcmp(c->x86_vendor_id, "AuthenticAMD")) {
		c->x86_vendor = X86_VENDOR_AMD;
	} else {
		c->x86_vendor = X86_VENDOR_UNKNOWN;
	}

	get_cpu_cap(c);

	printk("detected x86 CPU: %s, family %d model %d revision %d\n",
		c->x86_vendor_id, c->x86, c->x86_model, c->x86_mask);
#ifdef CONFIG_X86_32
	if (cpu_has_pae && cpu_has_nx) {
		/* PikeOS logic: when PAE and NX is available, NX is active */
		printk(KERN_INFO "NX (Execute Disable) protection: active\n");
	}
#endif
}
