obj-y := init.o
obj-$(CONFIG_PCI_MSI) += msi.o

EXTRA_CFLAGS += $(PIKEOS_INCLUDE)

ifeq ($(CONFIG_PCI_DEBUG),y)
EXTRA_CFLAGS += -DDEBUG
endif
