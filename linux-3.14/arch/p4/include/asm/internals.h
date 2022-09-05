/*
 * Various internal functions, taken from *util.h files
 *
 * Copyright (C) 2000, 2001, 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 *
 * Merged by azu@sysgo
 */

#ifndef __INTERNALS_H__
#define __INTERNALS_H__

#include <asm/host_context.h>

extern int ncpus;
extern int bootmem_retired;

extern void do_signal(struct pt_regs *regs);
extern void user_segv(struct pt_regs *regs);
extern int handle_page_fault(unsigned long address, unsigned long error_code,
			     int is_user, int *code_out);

extern void paging_init(void);
extern void syscall_trace(struct pt_regs *, int);

extern void userspace(struct pt_regs *regs);
extern void test_and_wakeup_userspace(unsigned int cpu);
extern void kern_idle(unsigned int cpu);
extern void arch_handle_syscall(struct pt_regs *regs);

extern void setup_sub_arch(void);

extern void early_init_process_setup(void);

#ifdef CONFIG_BSP
extern void bsp_init(void);
extern void bsp_time_init(void);
#endif

#ifdef CONFIG_BUILTIN_TEST
extern void do_builtin_test(void);
#endif

extern unsigned long _stext, _etext, _sdata, _edata, __bss_start, _end;
extern void setup_boot_memory(void);
extern void start_kernel_thread(void);

extern void linux_main(void);

extern int arch_handle_exception(struct pt_regs *regs);
extern int fixup_exception(unsigned long address, struct pt_regs *regs);

/* ptrace */
struct task_struct;
extern void set_single_step(struct task_struct *child);
extern void clear_single_step(struct task_struct *child);

/* tlb handling */
extern void force_flush_all(void);

/* kmapper */
extern void kernel_exception_handler(void);

/* mmu */
extern void mmu_task_init(void);

#endif /* __INTERNALS_H__ */
