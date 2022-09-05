/*
 *  ARM thread local storage access
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

#ifdef CONFIG_CPU_V5
/* on ARMv5, use generic hook */
#define __tls_ptr() ({	\
		unsigned long (*__tls_func)(void);	\
		__tls_func = (void*)0xffff0fe0;	\
		__tls_func();	\
	})
#else
/* v6 and v7 CPUs have a read-only register for TLS */
#define __tls_ptr() ({	\
		unsigned long __tls_ptr;	\
		__asm__ volatile("mrc	p15, 0, %0, c13, c0, 3\n" : "=r" (__tls_ptr));	\
		__tls_ptr;	\
	})
#endif

#define __tls_typed_ptr(type, off) ((type)(__tls_ptr() + (off)))

#define __tls_get_uint(off) ({	\
		unsigned int __tmp;	\
		__tmp = *__tls_typed_ptr(unsigned int *, (off));	\
		__tmp;	\
	})
#define __tls_set_uint(off, val) do {	\
		*__tls_typed_ptr(unsigned int *, (off)) = (val);	\
	} while (0)
#define __tls_set_ulong(off, val) do {	\
		*__tls_typed_ptr(unsigned long *, (off)) = (val);	\
	} while (0)
#define __tls_set_ptr(off, val) do {	\
		*__tls_typed_ptr(void **, (off)) = (val);	\
	} while (0)

#define __tls_set_ptr_in_regs(regs, ptr) do {	\
		(regs)->tls = (unsigned long)(ptr);	\
	} while (0)

#endif
