/*
 * Copyright (C) 2000 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include <asm/unistd.h>
#include <asm/host_context.h>

typedef long syscall_handler_t(struct pt_regs);

extern syscall_handler_t *sys_call_table[];

#ifdef CONFIG_X86_32

#define EXECUTE_SYSCALL(syscall, regs) \
	((long (*)(struct syscall_args)) (*sys_call_table[syscall]))(SYSCALL_ARGS(regs))

#else

#define EXECUTE_SYSCALL(syscall, regs) \
	(((long (*)(long, long, long, long, long, long)) \
	  (*sys_call_table[syscall]))(UPT_SYSCALL_ARG1(regs), \
		 		      UPT_SYSCALL_ARG2(regs), \
				      UPT_SYSCALL_ARG3(regs), \
				      UPT_SYSCALL_ARG4(regs), \
				      UPT_SYSCALL_ARG5(regs), \
				      UPT_SYSCALL_ARG6(regs)))

#endif
