obj-y := fault.o process.o syscall.o bugs.o signal.o setjmp.o tls.o setup.o \
			sysrq.o ptrace.o pci-dma.o iomap.o entry-common.o traps.o \
			cache.o armksyms.o vmregion.o

obj-$(CONFIG_ALIGNMENT_TRAP)	+= alignment.o
obj-$(CONFIG_P4_GENERIC_OUTER_CACHE)	+= outer_cache.o

subarch-obj-y = lib/changebit.o lib/csumipv6.o lib/csumpartial.o \
		lib/csumpartialcopy.o lib/clearbit.o \
		lib/delay.o lib/delay-loop.o lib/findbit.o lib/memchr.o lib/memcpy.o \
		lib/memmove.o lib/memset.o lib/memzero.o lib/setbit.o \
		lib/strchr.o lib/strrchr.o \
		lib/testchangebit.o lib/testclearbit.o lib/testsetbit.o \
		lib/ashldi3.o lib/ashrdi3.o lib/lshrdi3.o lib/muldi3.o \
		lib/ucmpdi2.o lib/lib1funcs.o lib/div64.o \
		lib/io-readsb.o lib/io-writesb.o lib/io-readsl.o lib/io-writesl.o \
		lib/io-readsw-armv4.o lib/io-writesw-armv4.o \
		kernel/io.o kernel/sys_arm.o kernel/stacktrace.o kernel/return_address.o
subarch-obj-$(CONFIG_MODULES) += kernel/module.o

subarch-obj-$(CONFIG_COMMON_CLKDEV) += common/clkdev.o

include $(srctree)/scripts/Kbuild.include
include $(srctree)/arch/p4/scripts/Makefile.rules
