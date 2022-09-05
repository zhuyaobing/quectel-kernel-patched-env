/*
 * linux/arch/p4/arch-arm/sysrq.c - ARM dump registers
 *
 * 2009-09-22, David Engraf <den@sysgo.com>
 */

#include <linux/kernel.h>
#include <linux/smp.h>
#include <linux/sched.h>
#include <asm/ptrace.h>

void show_regs(struct pt_regs * regs)
{
	printk(KERN_INFO "#### r0:   %08lx r1:  %08lx r2:   %08lx r3:    %08lx\n", regs->context[HOST_R0], regs->context[HOST_R1], regs->context[HOST_R2], regs->context[HOST_R3]);
	printk(KERN_INFO "#### r4:   %08lx r5:  %08lx r6:   %08lx r7:    %08lx\n", regs->context[HOST_R4], regs->context[HOST_R5], regs->context[HOST_R6], regs->context[HOST_R7]);
	printk(KERN_INFO "#### r8:   %08lx r9:  %08lx r10:  %08lx r11:   %08lx\n", regs->context[HOST_R8], regs->context[HOST_R9], regs->context[HOST_R10], regs->context[HOST_R11]);
	printk(KERN_INFO "#### r12:  %08lx r13: %08lx r14:  %08lx r15:   %08lx\n", regs->context[HOST_R12], regs->context[HOST_R13], regs->context[HOST_R14], regs->context[HOST_R15]);
	printk(KERN_INFO "#### cpsr: %08lx tls :%08lx fault:%08lx excode:%08lx\n", regs->context[HOST_CPSR],regs->context[HOST_TLS],regs->context[HOST_FAULT], regs->context[HOST_EX_CODE]);
	printk(KERN_INFO "#### epc:  %08lx sp:  %08lx arch1:%08lx arch2 :%08lx\n", regs->context[HOST_EPC], regs->context[HOST_SP],  regs->context[HOST_ARCH1], regs->context[HOST_ARCH2]);
}
