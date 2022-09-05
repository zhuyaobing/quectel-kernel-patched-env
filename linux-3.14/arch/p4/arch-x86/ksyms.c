#include "linux/module.h"
#include "asm/uaccess.h"
#include "asm/checksum.h"

/* Networking helper routines. */
EXPORT_SYMBOL(csum_partial);

#ifndef CONFIG_P4_X86_AMD64
EXPORT_SYMBOL(strstr);
#endif

#ifdef CONFIG_FUNCTION_TRACER
/* mcount is defined in assembly */
EXPORT_SYMBOL(mcount);
#endif

/*
 * Note, this is a prototype to get at the symbol for
 * the export, but dont use it from C code, it is used
 * by assembly code and is not using C calling convention!
 */
#ifndef CONFIG_X86_CMPXCHG64
extern void cmpxchg8b_emu(void);
EXPORT_SYMBOL(cmpxchg8b_emu);
#endif

#ifdef CONFIG_SMP

/* required for SMP */

extern void __write_lock_failed(rwlock_t *rw);
EXPORT_SYMBOL(__write_lock_failed);

extern void __read_lock_failed(rwlock_t *rw);
EXPORT_SYMBOL(__read_lock_failed);

#endif

#ifdef CONFIG_P4_X86_AMD64

/*
 * Export string functions. We normally rely on gcc builtin for most of these,
 * but gcc sometimes decides not to inline them.
 */
#undef memcpy
#undef memset
#undef memmove

extern void *memset(void *, int, __kernel_size_t);
extern void *memcpy(void *, const void *, __kernel_size_t);
extern void *__memcpy(void *, const void *, __kernel_size_t);

EXPORT_SYMBOL(memset);
EXPORT_SYMBOL(memcpy);
EXPORT_SYMBOL(__memcpy);
EXPORT_SYMBOL(memmove);

#endif
