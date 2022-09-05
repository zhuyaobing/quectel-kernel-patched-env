/*
 *  prctl handling
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

#include <linux/sched.h>
#include <asm/prctl.h>

long do_arch_prctl(struct task_struct *task, int code, unsigned long addr)
{
	int ret = 0;

	switch (code) {
	case ARCH_SET_GS:
		if (addr >= TASK_SIZE_OF(task))
			return -EPERM;
		task->thread.regs.context[HOST_GS_BASE] = addr;
		break;
	case ARCH_SET_FS:
		/* Not strictly needed for fs, but do it for symmetry
		   with gs */
		if (addr >= TASK_SIZE_OF(task))
			return -EPERM;
		task->thread.regs.context[HOST_FS_BASE] = addr;
		break;
	case ARCH_GET_FS:
		ret = put_user(task->thread.regs.context[HOST_FS_BASE],
			       (unsigned long __user *)addr);
		break;
	case ARCH_GET_GS:
		ret = put_user(task->thread.regs.context[HOST_GS_BASE],
			       (unsigned long __user *)addr);
		break;

	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

long sys_arch_prctl(int code, unsigned long addr)
{
	return do_arch_prctl(current, code, addr);
}
