/*
 *  common exception routine
 *
 *  Author: Alex Zuepke <azu@sysgo.de>
 *
 *  Copyright (C) 2003-2011 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#include "p4.h"
#include <asm/host_exception.h>
#include <asm/host_threads.h>
#include <asm/host_tasks.h>
#include <asm/debugout.h>
#include <asm/wrapper.h>
#include <asm/bug.h>
#include <linux/compiler.h>	/* for unlikely */
#include <asm/pikeos_version.h>


static noinline __noreturn void fail(const char *s, int err)
{
	/* it's not safe to call BUG() because it will generate an exception */
	//BUG();
	die_runtime(s, err);
}

/*
 * wait_for_exception
 *
 * waits for an incoming exception or pagefault
 * updates regs after the exception is caught
 *
 */
void reply_and_wait_for_exception(struct pt_regs *regs, unsigned int *uid)
{
	P4_ipc_result_t result;
	P4_message_t snd, rcv;
	P4_cc_t      cc;

	/* build receive message */
	rcv.timeout = P4_TIMEOUT_INFINITE;
	rcv.uid = P4_UID_ALL;
	rcv.buf_size = sizeof(regs->context);
	rcv.buf = &regs->context;
	rcv.map_size = 0;

	/* build send message depending on former messages */
	if (*uid != 0) {
		snd.timeout = P4_TIMEOUT_INFINITE;
		snd.uid = *uid;
		snd.buf_size = sizeof(regs->context);
		snd.buf = &regs->context;
		snd.map_size = 0;

		regs->context[HOST_EX_CODE] = P4_EX_CONTINUE;
	} else {
		snd.uid = P4_UID_INVALID;
	}

	cc = p4_ipc(&snd, &rcv, 0, &result);

	if (cc != P4_CC_OK)
		fail("reply_and_wait_for_exception: IPC error 0x%x\n", cc);
	if (result.buf_size != rcv.buf_size)
		fail("reply_and_wait_for_exception: message size mismatch %d\n", rcv.buf_size);

	/* message is ok, save UID for answer */
	*uid = result.sender;
}

#if PIKEOS_API_VERSION < 0x40004
void uk_sysemu(unsigned int taskid, struct pt_regs *regs, void (*handler)(void), unsigned long stack)
{
	P4_e_t err;

	/* wait for next exceptions and pagefault, this function will never
	 * return except on preemption or error
	 */
	err = p4_sysemu_enter(
			taskid,
			(P4_regs_t*)&regs->context,
			handler,
			(void *)stack,
			P4_PRIO_KEEP,
			0);

	if (unlikely(err != P4_E_CANCEL))
		fail("uk_sysemu: failed with error 0x%x\n", err);
}
#else
void uk_sysemu(unsigned int taskid, struct pt_regs *regs, int prio)
{
	P4_e_t err;

	/* wait for next exceptions and pagefault, this function will never
	 * return except on preemption or error
	 */
	err = p4_sysemu_enter(
			taskid,
			(P4_regs_t*)&regs->context,
			prio,
			0);
	if (unlikely(err != P4_E_CANCEL))
		fail("uk_sysemu: failed with error 0x%x\n", err);
}
#endif

void uk_sysemu_preempt(unsigned int threadid)
{
	P4_e_t err;

#if PIKEOS_API_VERSION < 0x40004
	err = p4_sysemu_preempt(threadid);
#else
	err = p4_thread_preempt(threadid, 0, 0);
#endif
	if (unlikely(err != P4_E_OK))
		fail("uk_sysemu_preempt: failed with error 0x%x", err);
}

void dummy_preempt_handler(void *regs, unsigned int oldprio)
{
	p4_thread_set_regs((P4_regs_t *)regs, oldprio);
	BUG();
}
