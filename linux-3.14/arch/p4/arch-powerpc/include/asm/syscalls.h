/*
 * Copyright (C) 2000 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include <asm/unistd.h>
#include <asm/host_context.h>

typedef long syscall_handler_t(long, long, long, long, long, long);

#define EXECUTE_SYSCALL(syscall, regs) \
	sys_call_table[(syscall)]((regs)->context[HOST_R3], \
                                  (regs)->context[HOST_R4], \
                                  (regs)->context[HOST_R5], \
                                  (regs)->context[HOST_R6], \
                                  (regs)->context[HOST_R7], \
                                  (regs)->context[HOST_R8])
