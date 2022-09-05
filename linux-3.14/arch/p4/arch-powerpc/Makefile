obj-y := bugs.o setjmp.o checksum.o ksyms.o ptrace.o \
	syscalls.o sysrq.o bugs.o misc.o \
	setup.o tls.o string.o \
	cache.o signal.o align.o io.o

subarch-obj-$(CONFIG_MODULES)	+= kernel/module.o kernel/module_32.o

subarch-obj-y += kernel/syscalls.o kernel/vdso.o lib/feature-fixups.o \
	lib/code-patching.o lib/crtsavres.o

obj-y += vdso32/

obj-$(CONFIG_PCI) += pci.o

include $(srctree)/scripts/Kbuild.include
include $(srctree)/arch/p4/scripts/Makefile.rules
