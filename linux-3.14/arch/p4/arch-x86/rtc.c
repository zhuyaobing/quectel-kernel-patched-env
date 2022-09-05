/*
 * RTC related functions
 * Copyright (c) 2012, David Engraf, SYSGO AG <den@sysgo.com>
 *
 *  derived from arch/x86/kernel/rtc.c and reduced to the read/write functions
 *  for the RTC driver
 */
#include <linux/platform_device.h>
#include <linux/mc146818rtc.h>
#include <linux/acpi.h>
#include <linux/bcd.h>
#include <linux/pnp.h>
#include <linux/of.h>

#include <asm/x86_init.h>
#include <asm/time.h>

#ifdef CONFIG_X86_32
/*
 * This is a special lock that is owned by the CPU and holds the index
 * register we are working with.  It is required for NMI access to the
 * CMOS/RTC registers.  See include/asm-i386/mc146818rtc.h for details.
 */
volatile unsigned long cmos_lock;
EXPORT_SYMBOL(cmos_lock);
#endif /* CONFIG_X86_32 */

/* For two digit years assume time is always after that */
#define CMOS_YEARS_OFFS 2000

DEFINE_SPINLOCK(rtc_lock);
EXPORT_SYMBOL(rtc_lock);

/* Routines for accessing the CMOS RAM/RTC. */
unsigned char rtc_cmos_read(unsigned char addr)
{
	unsigned char val;

	lock_cmos_prefix(addr);
	outb(addr, RTC_PORT(0));
	val = inb(RTC_PORT(1));
	lock_cmos_suffix(addr);

	return val;
}
EXPORT_SYMBOL(rtc_cmos_read);

void rtc_cmos_write(unsigned char val, unsigned char addr)
{
	lock_cmos_prefix(addr);
	outb(addr, RTC_PORT(0));
	outb(val, RTC_PORT(1));
	lock_cmos_suffix(addr);
}
EXPORT_SYMBOL(rtc_cmos_write);
