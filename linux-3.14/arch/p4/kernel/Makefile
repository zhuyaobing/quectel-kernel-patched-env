# to build the linkerscript from .S-file
CPPFLAGS_vmlinux.lds = \
	-U"$(ELF_ARCH)" \
	-DARCH_JIFFIES=$(ARCH_JIFFIES) \
	-DELF_ARCH=$(ELF_ARCH) \
	-DELF_FORMAT="$(ELF_FORMAT)" $(CPP_MODE-y) \
	-DKERNEL_STACK_SIZE=8192 -P -C

extra-y := vmlinux.lds

obj-y := \
	cpu.o irq.o ksyms.o kmapper.o initrd.o \
	userspace.o process.o \
	kernthreads.o sysrq.o \
	irqflags.o \
	time.o traps.o setup.o wrapper.o uaccess.o reboot.o

obj-$(CONFIG_GENERIC_PTRACE)	+= ptrace_gen.o
obj-$(CONFIG_GENERIC_SIGNAL)	+= signal.o
obj-$(CONFIG_SMP)		+= smp.o
obj-$(CONFIG_BUILTIN_TEST)	+= test.o
obj-$(CONFIG_OF_EARLY_FLATTREE)	+= devtree.o
