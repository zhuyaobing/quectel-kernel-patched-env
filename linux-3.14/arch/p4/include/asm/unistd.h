/*
 * Copyright (C) 2000 - 2004  Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#ifndef _P4_UNISTD_H_
#define _P4_UNISTD_H_

#ifdef __KERNEL__
/* We get __ARCH_WANT_OLD_STAT and __ARCH_WANT_STAT64 from the base arch */
#define __ARCH_WANT_OLD_READDIR
#define __ARCH_WANT_SYS_ALARM
#define __ARCH_WANT_SYS_GETHOSTNAME
#define __ARCH_WANT_SYS_PAUSE
#define __ARCH_WANT_SYS_SGETMASK
#define __ARCH_WANT_SYS_SIGNAL
#define __ARCH_WANT_SYS_TIME
#define __ARCH_WANT_SYS_UTIME
#define __ARCH_WANT_SYS_WAITPID
#define __ARCH_WANT_SYS_SOCKETCALL
#define __ARCH_WANT_SYS_FADVISE64
#define __ARCH_WANT_SYS_GETPGRP
#define __ARCH_WANT_SYS_LLSEEK
#define __ARCH_WANT_SYS_NICE
#define __ARCH_WANT_SYS_OLD_GETRLIMIT
#define __ARCH_WANT_SYS_OLDUMOUNT
#define __ARCH_WANT_SYS_SIGPENDING
#define __ARCH_WANT_SYS_SIGPROCMASK
#define __ARCH_WANT_SYS_RT_SIGACTION
#define __ARCH_WANT_SYS_RT_SIGSUSPEND
#endif

#include "arch/asm/unistd.h"

#ifdef CONFIG_P4_PPC
#undef cond_syscall
#define cond_syscall(x) asm(".weak\t" #x "\n\t.set\t" #x ",sys_ni_syscall")

#undef __ARCH_WANT_SYS_RT_SIGSUSPEND
#undef __ARCH_WANT_COMPAT_SYS_RT_SIGSUSPEND

#endif

#ifdef CONFIG_P4_ARM
#undef __ARCH_WANT_SYS_RT_SIGSUSPEND
#endif

#endif /* _UM_UNISTD_H_*/
