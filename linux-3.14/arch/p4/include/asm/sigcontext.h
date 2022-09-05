#ifndef __P4_SIGCONTEXT_H
#define __P4_SIGCONTEXT_H

#ifdef CONFIG_P4_PPC
/*
 * This is a workaround for an ugly dependency: sigcontext (and ucontext)
 * contain pt_regs, and userspace thinks pt_regs is in native PPC/Linux format,
 * not our P4/Linux one ...
 */
#define pt_regs sys_pt_regs
#endif

#if defined(CONFIG_P4_PPC) || defined(CONFIG_P4_ARM)
#include <uapi/asm/sigcontext.h>
#else
#include <arch/asm/sigcontext.h>
#endif

#ifdef CONFIG_P4_PPC
#undef pt_regs
#endif

#endif
