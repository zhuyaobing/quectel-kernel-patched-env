#ifndef _ASM_P4_BUG_H
#define _ASM_P4_BUG_H

#if defined(CONFIG_P4_ARM)  || defined(CONFIG_P4_X86) || defined(CONFIG_P4_PPC)
/* use native version */

#if defined(CONFIG_X86_64) && defined(CONFIG_MODULES) && defined(MODULE)
/* Native BUG() is not supported in modules on X86_64 because they are compiled
 * with -mcmodel=large thus the relative BUG() optimization does not work,
 * fallback to the generic version. */
#include <asm-generic/bug.h>
#else
#include <arch/asm/bug.h>
#endif

#else
/* Please check how the native achitecture implements BUG(). P4Linux supports
 * the GENERIC_BUG interface and it should be used if it's possible. Please
 * check kmapper.c:kernel_exception_handler.
 */
#error Check for native BUG implementation
#endif

#endif /* _ASM_P4_BUG_H */
