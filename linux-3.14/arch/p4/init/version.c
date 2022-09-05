/*
 * PikeOS version check
 * NOTE: This file should be the first file getting compiled
 */

#include <vm.h>

#ifndef P4_API_VERSION
#error "P4_API_VERSION not defined, please check PIKEOS_PREFIX"
#endif

/* keep in sync with version check in linux/arch/p4/kernel/setup.c */
#if P4_API_VERSION < 0x30005
#error "PikeOS versions < 3.4 are not supported"
#endif

#ifndef CONFIG_PIKEOS_DISABLE_VERSION_CHECK
/* check for PikeOS versions known to work (can be disabled by the customer) */

#if P4_API_VERSION > 0x40007
#error "PikeOS API version not supported, you can disable this check by selecting CONFIG_PIKEOS_DISABLE_VERSION_CHECK."
#endif

#if VM_VERSION < 20 || VM_VERSION > 38
#error "PikeOS VM version not supported, you can disable this check by selecting CONFIG_PIKEOS_DISABLE_VERSION_CHECK."
#endif

#endif

