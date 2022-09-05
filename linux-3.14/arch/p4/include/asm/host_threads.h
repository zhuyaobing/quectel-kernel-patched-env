/*
 *  ukernel thread functions
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

#ifndef __HOST_THREADS_H_
#define __HOST_THREADS_H_

/* this is as an abstractionlayer between Linux and the PikeOS API */
/* including p4.h must be avoided in Linux code */

int uk_thread_create(unsigned int threadno, const char *name, void *func,
	void *stack, void *tls, unsigned int prio, unsigned int exhandler_thread,
	unsigned long affinity,
	unsigned int arg1, void *arg2);
void uk_thread_delete(unsigned int threadno);
void uk_set_tls(void *tls);

void uk_thread_ev_signal(unsigned int threadno);
void uk_thread_ev_wait(void);
void uk_thread_ev_clear(void);

void uk_thread_sleep(unsigned long long nanosecs);
void uk_thread_sleep_ev(unsigned long long nanosecs);
void uk_thread_suspend(void);

int uk_thread_irq_attach(unsigned int irqno);
void uk_thread_irq_detach(void);
int uk_thread_irq_wait(int chain);

#define UK_ERR_OK     0 /* P4_E_OK */
#define UK_ERR_CANCEL 1 /* P4_E_CANCEL */
/* negative values:        P4_E_* errors */

int uk_thread_event_mask(unsigned int uid);
int uk_thread_event_wait(void);
int uk_thread_event_signal(unsigned int uid);

unsigned long long uk_get_time_ns(void);

void uk_thread_cancel(unsigned int threadno);


/* contructor functions for UIDs */
static inline unsigned int uk_uid_of_thread(unsigned int thread_id)
{
	extern unsigned int __pikeos_taskid;

	return (63 << 20) |				/* respart */
		   (__pikeos_taskid << 9) |	/* task */
		   thread_id;				/* thread */
}

static inline unsigned int uk_uid_of_all_threads(void)
{
	extern unsigned int __pikeos_taskid;

	return (63 << 20) |				/* respart */
		   (__pikeos_taskid << 9) |	/* task */
		   511;						/* thread */
}

/* extractor functions for UIDs */
static inline unsigned int uk_uid_get_thread(unsigned int uid)
{
	return uid & 511;
}

static inline unsigned int uk_uid_get_task(unsigned int uid)
{
	return (uid >> 9) & 2047;
}

void uk_thread_call_on_stack(void *func, void *stack, void *tls);

unsigned int uk_thread_cpu_id(void);
void uk_thread_setaffinity(unsigned int thr, unsigned long affinity);

/* for robust mutexes */
typedef struct {
	/* NOTE: keep this compatible with P4_rulock_register_t */
	void *ptr[3];
	unsigned int cnt;
} uk_rulist_t;
void uk_thread_register_rulock(uk_rulist_t *rulist);
void uk_thread_set_prio(unsigned int thr, int prio);
void uk_thread_set_my_prio(unsigned int prio);
unsigned int uk_thread_get_thread_attr(unsigned int thr_num, unsigned int *thr_state_ptr,
		unsigned int *stopping_ptr, unsigned int *cpuid_ptr, unsigned int *uid_ptr, unsigned int *priority_ptr);
unsigned int uk_thread_stop(unsigned int threadno);
unsigned int uk_thread_resume(unsigned int threadno);
unsigned int uk_thread_nbr(void);

#endif
