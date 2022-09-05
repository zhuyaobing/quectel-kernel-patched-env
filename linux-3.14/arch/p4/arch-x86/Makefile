ifeq ($(CONFIG_X86_32),y)
	BITS := 32
else
	BITS := 64
endif

obj-y := bugs.o fault.o ksyms.o pageattr.o ptrace.o \
	setup.o signal.o syscall.o sysrq.o bugs.o \
	cpu.o ioport.o ldt.o irq_32.o x86_init.o
obj-$(CONFIG_X86_32) += mem_32.o setjmp.o tls.o sys_call_table.o
obj-$(CONFIG_X86_64) += mem_64.o tls_64.o sys_call_table_64.o syscalls_64.o

obj-$(CONFIG_PCI) += pci.o
obj-$(CONFIG_RTC) += rtc.o


subarch-obj-y = lib/rwsem.o lib/rwlock.o kernel/pci-nommu.o kernel/pci-dma.o \
	kernel/pci-iommu_table.o kernel/alternative.o kernel/sysfb.o \
	kernel/io_delay.o lib/memcpy_$(BITS).o um/delay.o

ifeq ($(CONFIG_X86_32),y)
subarch-obj-y += lib/atomic64_32.o lib/atomic64_cx8_32.o \
	lib/strstr_32.o lib/string_32.o um/checksum_32.o

ifneq ($(CONFIG_X86_CMPXCHG64),y)
subarch-obj-y += lib/cmpxchg8b_emu.o lib/atomic64_386_32.o
endif

else
subarch-obj-y += \
	lib/memmove_64.o lib/memset_64.o um/setjmp_64.o lib/cmpxchg16b_emu.o \
	lib/csum-partial_64.o ../um/kernel/syscall.o

obj-y += vdso/

endif

subarch-obj-$(CONFIG_PCI_REAL) += pci/init.o pci/legacy.o
subarch-obj-$(CONFIG_PCI) += pci/direct.o pci/fixup.o \
	pci/irq.o pci/bus_numa.o pci/common.o pci/i386.o pci/bus_numa.o

subarch-obj-$(CONFIG_MODULES)	+= kernel/module.o

include $(srctree)/arch/p4/scripts/Makefile.rules
