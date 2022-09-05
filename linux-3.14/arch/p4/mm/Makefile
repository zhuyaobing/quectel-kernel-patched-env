#
# arch/p4/mm/Makefile
#
# Copyright (c) 2003-2006, Alex Zuepke, Sysgo RTS AG <azu at sysgo dot de>
#

obj-y := extable.o init.o ioremap.o tlb.o pageattr.o

ifeq ($(CONFIG_P4_TASK_OVERCOMMIT),y)
obj-y += mmu-overcommit.o
else
obj-y += mmu-static.o
endif
