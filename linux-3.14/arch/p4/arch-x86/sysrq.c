#include <linux/kernel.h>
#include <linux/smp.h>
#include <linux/sched.h>
#include <asm/ptrace.h>

extern void show_trace(unsigned long *stack);

void show_regs(struct pt_regs *regs)
{
#ifdef CONFIG_X86_32
	printk(KERN_INFO "#### eax:%08lx  ebx:%08lx  ecx:%08lx  edx:%08lx\n", regs->context[HOST_EAX], regs->context[HOST_EBX], regs->context[HOST_ECX], regs->context[HOST_EDX]);
	printk(KERN_INFO "#### esi:%08lx  edi:%08lx  ebp:%08lx  esp:%08lx\n", regs->context[HOST_ESI], regs->context[HOST_EDI], regs->context[HOST_EBP], regs->context[HOST_SP]);
	printk(KERN_INFO "#### eip:%08lx  elf:%08lx  cs:%04lx  ds:%04lx  es:%04lx  ss:%04lx\n", regs->context[HOST_IP], regs->context[HOST_EFLAGS], regs->context[HOST_CS], regs->context[HOST_DS], regs->context[HOST_ES], regs->context[HOST_SS]);
#else
	printk(KERN_INFO "#### rax:%016lx  rbx:%016lx  rcx:%016lx rdx:%016lx\n", regs->context[HOST_RAX], regs->context[HOST_RBX], regs->context[HOST_RCX], regs->context[HOST_RDX]);
	printk(KERN_INFO "#### rsi:%016lx  rdi:%016lx  rbp:%016lx r8 :%016lx\n", regs->context[HOST_RSI], regs->context[HOST_RDI], regs->context[HOST_RBP], regs->context[HOST_R8]);
	printk(KERN_INFO "#### r9 :%016lx  r10:%016lx  r11:%016lx r12:%016lx\n", regs->context[HOST_R9] , regs->context[HOST_R10], regs->context[HOST_R11], regs->context[HOST_R12]);
	printk(KERN_INFO "#### r13:%016lx  r14:%016lx  r15:%016lx  ...\n", regs->context[HOST_R13], regs->context[HOST_R14], regs->context[HOST_R15]);
	printk(KERN_INFO "#### rip:%016lx  cs :%016lx  rsp:%016lx rflags:%08lx\n", regs->context[HOST_RIP], regs->context[HOST_CS], regs->context[HOST_RSP], regs->context[HOST_RFLAGS]);
	printk(KERN_INFO "#### ss :%016lx  fs :%016lx  gs :%016lx\n", regs->context[HOST_SS], regs->context[HOST_FS_BASE], regs->context[HOST_GS_BASE]);
#endif
}
