#ifndef __UM_MUTEX_H
#define __UM_MUTEX_H

#ifdef CONFIG_P4_X86
/* x86 mutex stuff is optimized and written in assembler.
   The default x86 kernel is compiled with regparm=3 which
   is not supported by P4 and thus we have to switch to the
   generic version like um. */
#include <asm-generic/mutex-dec.h>
#else
#include "arch/asm/mutex.h"
#endif

#endif
