/*
 * linux/arch/p4/kernel/test.c
 *
 * Copyright (C) 2011 AlexZuepke, SYSGO AG (azu@sysgo.de)
 *
 * builtin tests
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stringify.h>
#include <asm/debugout.h>

#define assert(cond) do { if(!(cond)) die_early("do_builtin_test failed: " __FILE__ ":" __stringify(__LINE__) ":" __stringify(cond)); } while (0)


static __init void test_hweight(void)
{
	int w1, w2, w3, w4, w5, w6, w7;

	w1 = __arch_hweight32(0x00000000);
	w2 = __arch_hweight32(0x00000001);
	w3 = __arch_hweight32(0x00000002);
	w4 = __arch_hweight32(0x00000003);
	w5 = __arch_hweight32(0x000000ff);
	w6 = __arch_hweight32(0xffffffff);
	w7 = __arch_hweight32(0x50a0c001);
	assert(w1 == 0);
	assert(w2 == 1);
	assert(w3 == 1);
	assert(w4 == 2);
	assert(w5 == 8);
	assert(w6 == 32);
	assert(w7 == 7);
}

void __init do_builtin_test(void)
{
	printk("performing builtin tests: ");
	test_hweight();
	printk("done\n");
}
