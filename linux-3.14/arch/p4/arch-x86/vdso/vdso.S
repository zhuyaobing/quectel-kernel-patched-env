#include <linux/init.h>

__INITDATA

	.globl vdso_start, vdso_end
vdso_start:
	.incbin "arch/p4/arch-x86/vdso/vdso.so"
vdso_end:

__FINIT
