#
# arch/p4/lib/Makefile
#
# Copyright (c) 2003-2013, Alex Zuepke, Sysgo AG <azu at sysgo dot de>
#

lib-y := threads.o tasks.o exception.o vmapi_calls.o iomem.o locking.o misc.o

ifneq ($(CONFIG_P4_PPC), y)
lib-y += cache.o
endif

lib-$(CONFIG_P4_CACHEMGR_DRIVER)	+= cachemgr.o

# use PikeOS includes and flags
EXTRA_CFLAGS += $(PIKEOS_INCLUDE) $(PIKEOS_CFLAGS)
