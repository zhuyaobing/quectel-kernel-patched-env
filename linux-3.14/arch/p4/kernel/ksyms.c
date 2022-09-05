/*
 * Copyright (C) 2001, 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include <linux/module.h>
#include <linux/string.h>
#include <linux/spinlock.h>
#include <linux/pci.h>
#include <asm/current.h>
#include <asm/delay.h>
#include <asm/irq.h>
#include <asm/processor.h>
#include <asm/unistd.h>
#include <asm/pgalloc.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/io.h>
#include <asm/internals.h>
#include <asm/uaccess.h>
#include <asm/host_threads.h>
#include <asm/locking.h>

EXPORT_SYMBOL(empty_zero_page);

EXPORT_SYMBOL(cpu_data);

EXPORT_SYMBOL(uk_uevent_clear_pending);
EXPORT_SYMBOL(uk_uevent_is_waiting);
EXPORT_SYMBOL(uk_uevent_wait);
EXPORT_SYMBOL(uk_uevent_wait_timeout);
EXPORT_SYMBOL(uk_uevent_wait_timeout_abs);
EXPORT_SYMBOL(uk_uevent_signal);

#ifdef CONFIG_PCI
EXPORT_SYMBOL(pci_alloc_consistent);
EXPORT_SYMBOL(pci_free_consistent);
#endif

/* must be exported here because tasks.c cannot include linux/module.h 
 * due to name claches in function names between PikeOS <-> Linux
 */
extern unsigned int __pikeos_taskid;
EXPORT_SYMBOL(__pikeos_taskid);
EXPORT_SYMBOL(uk_thread_cpu_id);
EXPORT_SYMBOL(uk_thread_setaffinity);
#ifndef CONFIG_P4_PPC
EXPORT_SYMBOL(uk_flush_dcache_all);
#endif

#ifdef CONFIG_P4_PPC
EXPORT_SYMBOL(strncmp);
#endif
