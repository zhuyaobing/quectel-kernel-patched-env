/*
 *  x86 thread local storage access
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 * 
 *  Copyright (C) 2011 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#ifndef __ARCHTLS_H__
#define __ARCHTLS_H__

#define __tls_get_uint(off) ({	\
		unsigned int __tmp;	\
		__asm__ volatile("movl %%fs:%c1, %0\n" : "=r" (__tmp) : "i"(off));	\
		__tmp;	\
	})

#define __tls_set_uint(off, val)	\
		__asm__ volatile("movl %0, %%fs:%c1\n" : : "r" (val), "i"(off))

#ifdef __x86_64__ 
#define __tls_set_ulong(off, val)	\
		__asm__ volatile("movq %0, %%fs:%c1\n" : : "r" (val), "i"(off))
#define __tls_set_ptr __tls_set_ulong
#else
#define __tls_set_ulong __tls_set_uint
#define __tls_set_ptr __tls_set_uint
#endif

#define __tls_set_ptr_in_regs(regs, ptr)	\
		p4_thread_tls_register((regs), (ptr))

#endif
