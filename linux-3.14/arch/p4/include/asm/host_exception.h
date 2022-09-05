/*
 *  common exception routine
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 * 
 *  Copyright (C) 2003-2014 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#ifndef __EXCEPTION_H_
#define __EXCEPTION_H_

/* this should be seen as an abstractionlayer between linux and the p4-library */
/* because including p4.h is very problematic and should be avoided */

#include <asm/host_context.h>
#include <asm/pikeos_version.h>

extern void reply_and_wait_for_exception(struct pt_regs *regs, unsigned int *uid);
#if PIKEOS_API_VERSION < 0x40004
extern void uk_sysemu(unsigned int taskid, struct pt_regs *regs, void (*handler)(void), unsigned long stack);
#else
extern void uk_sysemu(unsigned int taskid, struct pt_regs *regs, int prio);
#endif
extern void uk_sysemu_preempt(unsigned int threadid);
void dummy_preempt_handler(void *regs, unsigned int oldprio);

#if defined(CONFIG_P4_X86)
#define BREAKPOINT()	__asm__("int $3")
#elif defined(CONFIG_P4_PPC)
#define BREAKPOINT()	__asm__(".long 0x7d821009")
#elif defined(CONFIG_P4_ARM)
#define BREAKPOINT()	__asm__("bkpt #12")
#else
#error define architecture breakpoint
#endif

#endif /* __EXCEPTION_H_ */

