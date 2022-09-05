/* Copyright (c) 2016 OpenSynergy GmbH */
/*
 * Copyright (C) 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#ifndef __UM_THREAD_INFO_H
#define __UM_THREAD_INFO_H

#ifndef __ASSEMBLY__

#include <asm/kernthreads.h>
#include <asm/page.h>

/* we use 8K stacks on PikeOS */
#define THREAD_SIZE_ORDER 1
#define THREAD_SIZE ((1 << THREAD_SIZE_ORDER) * PAGE_SIZE)

#include <asm/types.h>
#include <asm/processor.h>
#include <asm/uaccess.h>

typedef struct {
	unsigned long seg;
} mm_segment_t;

struct thread_info {
	struct task_struct	*task;		/* main task structure */
	struct exec_domain	*exec_domain;	/* execution domain */
	unsigned long		flags;		/* low level flags */
	__u32			cpu;		/* current CPU */
	int			preempt_count;  /* 0 => preemptable,
						   <0 => BUG */
	mm_segment_t		addr_limit;	/* thread address space:
					 	   0-0xBFFFFFFF for user
						   0-0xFFFFFFFF for kernel */
	struct restart_block    restart_block;
};

#define INIT_THREAD_INFO(tsk)			\
{						\
	.task =		&tsk,			\
	.exec_domain =	&default_exec_domain,	\
	.flags =		0,		\
	.cpu =		0,			\
	.preempt_count = INIT_PREEMPT_COUNT,	\
	.addr_limit =	KERNEL_DS,		\
	.restart_block =  {			\
		.fn =  do_no_restart_syscall,	\
	},					\
}

#define init_thread_info	(init_thread_union.thread_info)
#define init_stack		(init_thread_union.stack)

/* how to get the thread information struct from C:
 * On PikeOS, we keep the current thread info in a pointer,
 * because thread magics don't work when we are in an interrupt
 * handler which is in fact another PikeOS thread ...
 */
extern struct thread_info *__current_thread[];
static inline struct thread_info *current_thread_info(void)
{
	return __current_thread[kthread_cpu()];
}

#endif	//__ASSEMBLY__

#define TIF_SYSCALL_TRACE	0	/* syscall trace active */
#define TIF_SIGPENDING		1	/* signal pending */
#define TIF_NEED_RESCHED	2	/* rescheduling necessary */
#define TIF_NOTIFY_RESUME	3	/* callback before returning to user */
#define TIF_RESTART_BLOCK 	4
#define TIF_MEMDIE	 	5
#define TIF_SYSCALL_AUDIT	6
#define TIF_RESTORE_SIGMASK	7
#define TIF_SINGLESTEP		8
#define TIF_KERNEL_TRACE	9	/* kernel trace active */
#define TIF_NOERROR		12
#define TIF_FREEZE		16	/* is freezing for suspend */

#define _TIF_SYSCALL_TRACE	(1 << TIF_SYSCALL_TRACE)
#define _TIF_SIGPENDING		(1 << TIF_SIGPENDING)
#define _TIF_NEED_RESCHED	(1 << TIF_NEED_RESCHED)
#define _TIF_NOTIFY_RESUME	(1 << TIF_NOTIFY_RESUME)
#define _TIF_KERNEL_TRACE	(1 << TIF_KERNEL_TRACE)
#define _TIF_MEMDIE		(1 << TIF_MEMDIE)
#define _TIF_SYSCALL_AUDIT	(1 << TIF_SYSCALL_AUDIT)
#define _TIF_RESTORE_SIGMASK	(1 << TIF_RESTORE_SIGMASK)
#define _TIF_FREEZE		(1 << TIF_FREEZE)

#endif
