#include <linux/kernel.h>
#include <linux/smp.h>
#include <linux/sched.h>
#include <asm/ptrace.h>

extern void show_trace(unsigned long *stack);

void show_regs(struct pt_regs *regs)
{
	printk(KERN_INFO "####  r0:%08lx   r1:%08lx   r2:%08lx   r3:%08lx\n", regs->context[HOST_R0], regs->context[HOST_R1], regs->context[HOST_R2], regs->context[HOST_R3]);
	printk(KERN_INFO "####  r4:%08lx   r5:%08lx   r6:%08lx   r7:%08lx\n", regs->context[HOST_R4], regs->context[HOST_R5], regs->context[HOST_R6], regs->context[HOST_R7]);
	printk(KERN_INFO "####  r8:%08lx   r9:%08lx  r10:%08lx  r11:%08lx\n", regs->context[HOST_R8], regs->context[HOST_R9], regs->context[HOST_R10], regs->context[HOST_R11]);
	printk(KERN_INFO "#### r12:%08lx  r13:%08lx  r14:%08lx  r15:%08lx\n", regs->context[HOST_R12], regs->context[HOST_R13], regs->context[HOST_R14], regs->context[HOST_R15]);
	printk(KERN_INFO "#### r16:%08lx  r17:%08lx  r18:%08lx  r19:%08lx\n", regs->context[HOST_R16], regs->context[HOST_R17], regs->context[HOST_R18], regs->context[HOST_R19]);
	printk(KERN_INFO "#### r20:%08lx  r21:%08lx  r22:%08lx  r23:%08lx\n", regs->context[HOST_R20], regs->context[HOST_R21], regs->context[HOST_R22], regs->context[HOST_R23]);
	printk(KERN_INFO "#### r24:%08lx  r25:%08lx  r26:%08lx  r27:%08lx\n", regs->context[HOST_R24], regs->context[HOST_R25], regs->context[HOST_R26], regs->context[HOST_R27]);
	printk(KERN_INFO "#### r28:%08lx  r29:%08lx  r30:%08lx  r31:%08lx\n", regs->context[HOST_R28], regs->context[HOST_R29], regs->context[HOST_R30], regs->context[HOST_R31]);
	printk(KERN_INFO "####  cr:%08lx  xer:%08lx   lr:%08lx  ctr:%08lx\n", regs->context[HOST_CR], regs->context[HOST_XER], regs->context[HOST_LR], regs->context[HOST_CTR]);
	printk(KERN_INFO "#### dar:%08lx  trp:%08lx  msr:%08lx\n", regs->context[HOST_DAR], regs->context[HOST_TRAP], regs->context[HOST_SRR1]);
	printk(KERN_INFO "####  pc:%08lx dsisr:%08lx\n", regs->context[HOST_SRR0], regs->context[HOST_DSISR]);
}
