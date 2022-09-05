/* DO NOT EDIT! GENERATED AUTOMATICALLY! */
/* Provide a more complete sys/time.h.

   Copyright (C) 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Paul Eggert.  */

#ifndef _gl_SYS_TIME_H
#define _gl_SYS_TIME_H

#if 1
# include "///data/jenkins/workspace/alex-platform_CI_master/build/tmp/sysroots/alexandra/usr/include/sys/time.h"
#else
# include <time.h>
#endif

#if ! 1
struct timeval
{
  time_t tv_sec;
  long int tv_usec;
};
#endif

#if 1
# undef gettimeofday
# define gettimeofday rpl_gettimeofday
int gettimeofday (struct timeval *restrict, void *restrict);
#endif

#endif /* _gl_SYS_TIME_H */
