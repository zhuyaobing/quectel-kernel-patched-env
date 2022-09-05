/*
 *  PikeOS thread abstraction layer
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

#ifndef __P4_KTHREADS_H__
#define __P4_KTHREADS_H__

#include <linux/stddef.h>
#include <linux/threads.h>
#include <asm/pikeos_version.h>
#include <asm/archtls.h>
#include <asm/host_threads.h>
#include <asm/host_tasks.h>
#include <asm/pikeos_version.h>

/*
 * thread IDs
 * - the initial thread becomes the kernel exception handler (KPAGER)
 * - then come the Linux main threads
 * - the rest is free for interrupts
 */
#define THREAD_KPAGER 0
#define THREAD_MAIN 1
#define THREAD_IRQ (THREAD_MAIN + NR_CPUS)

/*
 * The kthread table contains various P4 related attributes.
 * Parts accessible through thread local storage are presented below.
 */
struct kthread_tls {
	/* format required by PikeOS API */
#if PIKEOS_API_VERSION < 0x40003
	/* PikeOS 3.5 and earlier */
	struct kthread_tls *tls_ptr;	/* addr to TLS */
	unsigned long uid;	/* thread UID */
	void *rulist;	/* mutex robust list */
	unsigned long errno;	/* thread local error (unused) */
#else
	/* PikeOS 4.0 and later */
	struct kthread_tls *tls_ptr;	/* addr to TLS */
	unsigned int uid;	/* thread UID */
	char padding[4];	/* uprio, kprio, cpuid, padding */

	/* embedded rulock data */
	void *rulock_trying;
	void *rulock_list_next;
	void *rulock_list_prev;
	unsigned int rulock_count;

	unsigned int errno;	/* thread local error (unused) */

#if PIKEOS_API_VERSION >= 0x40004
	void *preempt_handler;
	void *preempt_stack;
	void *preempt_regs;
	unsigned long preempt_flags;
	void *sysemu_handler;
	void *sysemu_stack;
	void *sysemu_regs;
	unsigned long sysemu_flags;
	void *except_handler;
	void *except_stack;
	void *except_regs;
	unsigned long except_flags;
#endif
#endif

	unsigned int thread_id;	/* thread ID */
#ifdef CONFIG_SMP
	unsigned int cpu;	/* associated CPU */
	unsigned int new_cpu;	/* CPU to migrate to */
#endif
	int grab_lock;	/* thread tries to disable interrupts */
};

static inline unsigned int kthread_thread_id(void)
{
	return __tls_get_uint(offsetof(struct kthread_tls, thread_id));
}

static inline unsigned int kthread_uid(void)
{
	return __tls_get_uint(offsetof(struct kthread_tls, uid));
}

static inline unsigned int kthread_cpu(void)
{
	unsigned int cpu = 0;

#ifdef CONFIG_SMP
	cpu = __tls_get_uint(offsetof(struct kthread_tls, cpu));
#endif

	return cpu;
}

static inline void kthread_migrate_irq_thread_lazily(void)
{
#ifdef CONFIG_SMP
	void do_migrate_irq_thread(void);
	unsigned int this_cpu;
	unsigned int new_cpu;

	this_cpu = __tls_get_uint(offsetof(struct kthread_tls, cpu));
	new_cpu = __tls_get_uint(offsetof(struct kthread_tls, new_cpu));
	if (this_cpu != new_cpu) {
		do_migrate_irq_thread();
	}
#endif
}

static inline void kthread_set_lock(int state)
{
	__tls_set_uint(offsetof(struct kthread_tls, grab_lock), state);
}

static inline int kthread_get_lock(void)
{
	return __tls_get_uint(offsetof(struct kthread_tls, grab_lock));
}

unsigned int kthread_native_cpu_num(void);
unsigned int kthread_native_cpu_id(unsigned int cpu);

void kthreads_init(void);
void kthreads_call_on_new_stack(void *func);
void kthreads_boot_cpu(unsigned int cpu, void *func, void *stack);

static inline unsigned int kthread_main_thread(unsigned int cpu)
{
	return THREAD_MAIN + cpu;
}

static inline int kthread_is_irq(void)
{
	return kthread_thread_id() >= THREAD_IRQ;
}

int enable_irq_thread(unsigned int irq, const char *name, void (*func)(unsigned int, void*), void *arg);
int enable_irq_console_thread(unsigned int irq, const char *name, void (*func)(unsigned int, void*), void *arg);
void disable_irq_thread(unsigned int irq);

void cancel_irq_thread(unsigned int irq);
void sleep_in_irq_thread(void *event);
void wake_irq_thread(void *event);

#ifdef CONFIG_SMP
void bind_irq_thread(unsigned int irq, unsigned int cpu);
void migrate_irq_thread(unsigned int irq, unsigned int cpu);
#else
static inline void bind_irq_thread(unsigned int irq, unsigned int cpu) { }
static inline void migrate_irq_thread(unsigned int irq, unsigned int cpu) { }
#endif

// returns 0 on failure
unsigned int get_free_async_irq(void);
void release_async_irq(unsigned int irq);

unsigned int get_irq_threadid(unsigned int irq);
unsigned int get_irq_uid(unsigned int irq);

/* not defined here, but necessary for async threads */
void do_IRQ(unsigned int irq);

/* priorities */
#define PRIO_CRITICAL	0
#define PRIO_IRQ_CONSOLE -1
#define PRIO_KPAGER     -2
#define PRIO_IRQ        -3
#define PRIO_MAIN       -4

#define PRIO_RANGE  4 /* required priority range */

static inline void enter_critical_prio(void)
{
	uk_thread_set_my_prio(uk_task_mcp() + PRIO_CRITICAL);
}

static inline void leave_critical_prio(void)
{
	uk_thread_set_my_prio(uk_task_mcp() + PRIO_MAIN);
}
#endif
