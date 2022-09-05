#
# drivers/p4/Makefile
#
# Copyright (C) 2004-2006 SYSGO AG
# Copyright (C) 2016 OpenSynergy GmbH
#

obj-$(CONFIG_P4VM_PORT)	 += vmport_core.o
obj-$(CONFIG_P4VM_PORT_CHAR)	 += vmport_char.o
obj-$(CONFIG_P4VM_PORT_TTY)	 += vmport_tty.o
obj-$(CONFIG_P4VM_PORT_LINK)	 += vmport_link.o
obj-$(CONFIG_P4VM_PORT_NET)	 += vmport_net.o
obj-$(CONFIG_P4VM_FILE_CHAR)	 += vmfile_char.o
obj-$(CONFIG_P4VM_FILE_TTY)	 += vmfile_tty.o
obj-$(CONFIG_P4VM_FILE_NET)	 += vmfile_net.o
obj-$(CONFIG_P4VM_FILE_SHM)	 += vmfile_shm.o
obj-$(CONFIG_P4VM_PARTCTRL)	 += partctrl.o
obj-$(CONFIG_EARLY_PRINTK)	+= ssw_console.o
obj-$(CONFIG_P4VM_SPORT)	 += vmsport.o
obj-$(CONFIG_P4VM_FB)		+= vmfile_fb.o
obj-$(CONFIG_P4VM_FILE_SHM_FB)	+= vmfile_shm_fb.o
obj-$(CONFIG_P4VM_RTC)		+= rtc-pikeos.o 
obj-$(CONFIG_P4VM_PORT_INPUT)	+= vmport_input.o
obj-$(CONFIG_P4VM_CLOCK_TREE)	+= vmfile_clk/
obj-$(CONFIG_P4VM_REGULATOR)    += vmfile_regulator.o

ccflags-y += $(PIKEOS_INCLUDE) $(PIKEOS_CFLAGS)
