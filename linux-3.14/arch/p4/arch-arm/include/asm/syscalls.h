/*
 * Copyright (C) 2000 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include <asm/unistd.h>
#include <asm/host_context.h>

typedef long syscall_handler_t(struct pt_regs);

extern syscall_handler_t *sys_call_table[];

#define EXECUTE_SYSCALL(syscall, regs) \
	((long (*)(struct syscall_args)) (*sys_call_table[syscall]))(SYSCALL_ARGS(regs))
