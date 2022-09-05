/*
 * linux/arch/p4/arch-ppc/setup.c
 *
 * arch specific setup
 *
 * 2004jul06, azu@sysgo.de
 * some stuff taken from linux/arch/ppc/kernel/setup.c
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/utsname.h>
#include <linux/module.h>
#include <asm/cputable.h>
#include <asm/string.h>
#include <asm/cache.h>
#include <asm/internals.h>
#include <asm/io.h>

/* cur_cpu_spec must be a pointer, so declare the struct as static. This
 * will also make sure that the elements in the struct like cpu_features are
 * set to 0.
 */
static struct cpu_spec cur_cpu_spec_temp;

struct cpu_spec* cur_cpu_spec = &cur_cpu_spec_temp;
EXPORT_SYMBOL(cur_cpu_spec);

/*
 * These are used in binfmt_elf.c to put aux entries on the stack
 * for each elf executable being started.
 */
int dcache_bsize;
int icache_bsize;
int ucache_bsize;

/* Variables required to store legacy IO irq routing */
int of_i8042_kbd_irq;
EXPORT_SYMBOL_GPL(of_i8042_kbd_irq);
int of_i8042_aux_irq;
EXPORT_SYMBOL_GPL(of_i8042_aux_irq);

/*
 * Just a stub for now.
 * Needed by various drivers. PPC only.
 * For a working Linux implementation check arch/powerpc/kernel/setup-common.c
 */
int check_legacy_ioport(unsigned long base_port)
{
	int ret = -ENODEV;
	
	switch(base_port) {
		case I8042_DATA_REG:
			printk(KERN_INFO "i8042 not supported on PikeOS yet!\n");
			break;
		
		default:
			printk(KERN_INFO "FIXME: check_legacy_ioport() not supported on PikeOS yet!\n");
			break;
	}

	return ret;
}
EXPORT_SYMBOL(check_legacy_ioport);

void setup_sub_arch(void)
{
	strcpy(init_utsname()->machine, "ppc");

	/* split I and D caches on other archs - 601 not supported yet */
	icache_bsize = dcache_bsize = L1_CACHE_BYTES;
	ucache_bsize = 0;
}

