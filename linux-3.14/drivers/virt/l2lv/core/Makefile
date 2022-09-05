#
# drivers/l2lv/Makefile
#
# Copyright (C) 2013 Open Synergy
#
obj-$(CONFIG_L2LV_CLIENT)   += l2lv_client.o
l2lv_client-y               += l2lv_client_link.o \
                               l2lv_link.o \
                               l2lv_port.o \
                               l2lv_joblist.o

l2lv_client-$(CONFIG_P4)    += p4_shm.o \
                               p4_port.o

obj-$(CONFIG_L2LV_SERVER)   += l2lv_server.o
l2lv_server-y               += l2lv_server_links.o \
                               l2lv_link.o \
                               l2lv_port.o \
                               l2lv_joblist.o \
                               l2lv_local_msgq.o \
                               l2lv_local_port.o

l2lv_server-$(CONFIG_P4)    += p4_shm.o \
                               p4_port.o

ccflags-$(CONFIG_P4)        += $(PIKEOS_INCLUDE) $(PIKEOS_CFLAGS) \
			       -I$(srctree)/arch/p4/drivers
