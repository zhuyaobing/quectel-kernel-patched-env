/*
 *  TLS handling
 *
 *  Author: David Engraf <david.engraf@sysgo.com>
 *
 *  Copyright (C) 2014 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <uapi/asm/prctl.h>

long do_arch_prctl(struct task_struct *task, int code, unsigned long addr);

void arch_clear_tls(struct task_struct *tsk)
{
	tsk->thread.regs.context[HOST_FS_BASE] = 0;
	tsk->thread.regs.context[HOST_GS_BASE] = 0;
}

int arch_copy_tls(struct task_struct *t, struct pt_regs *regs)
{
	return do_arch_prctl(t, ARCH_SET_FS, regs->context[HOST_R8]);
}
