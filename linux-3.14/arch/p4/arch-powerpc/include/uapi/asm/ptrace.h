#ifndef _UAPI_ASM_P4_PPC_PTRACE_H
#define _UAPI_ASM_P4_PPC_PTRACE_H

#ifndef __KERNEL__
/* included by user space, use correct declaration */
#include "arch/uapi/asm/ptrace.h"
#else
/* use native ptrace.h but redefine pt_regs as sys_pt_regs */
#define pt_regs sys_pt_regs
#include "arch/uapi/asm/ptrace.h"
#undef pt_regs

/* pt_regs definition */
#include <asm/host_context.h>
#endif

#endif
