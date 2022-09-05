/*
 * Copyright (C) 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#ifndef __UM_SIGNAL_H
#define __UM_SIGNAL_H

/* Need to kill the do_signal() declaration in the i386 signal.h */

#define do_signal do_signal_renamed
#include "arch/asm/signal.h"
#undef do_signal

#endif
