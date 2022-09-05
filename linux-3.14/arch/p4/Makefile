#
# arch/p4/Makefile
#
# Copyright (C) 2002 Jeff Dike (jdike@karaya.com)
# Licensed under the GPL
#

# Symbol CONFIG_P4_SUBARCH doesn't exist before the first configuration cycle
# or when using the config options. We can safely use the x86 standard
# configuration in this case because no compilation or linking is done.
SUBARCH := $(CONFIG_P4_SUBARCH:"%"=%)
ifeq ($(SUBARCH),)
SUBARCH := x86
endif
include $(srctree)/arch/p4/Makefile-$(SUBARCH)

# defaults to /opt/pikeos
PIKEOS_PREFIX       = $(shell echo $(CONFIG_PIKEOS_PREFIX))
PIKEOS_TARGET_FILES = $(PIKEOS_PREFIX)/target/$(PIKEOS_ARCH)/$(PIKEOS_PROC)
PIKEOS_INCLUDE      = -I$(PIKEOS_TARGET_FILES)/include
PIKEOS_INCLUDE     += -I$(PIKEOS_TARGET_FILES)/driver/include
PIKEOS_NATIVE_LIBS  = libsbuf.a libvm.a libp4.a
PIKEOS_NATIVE_LIBS := $(patsubst %, arch/p4/lib/native/%, $(PIKEOS_NATIVE_LIBS))

# add special include folders for arch/ and arch-override/ at the beginning
# to support SRC1DIR/SRC2DIR
LINUXINCLUDE := $(call addinclude,include-src2) \
		$(call addinclude,include-src1) \
		$(LINUXINCLUDE)

# additional includes to the SUBARCH folders
LINUXINCLUDE += -I$(srctree)/arch/p4/arch-$(SUBARCH)/include
LINUXINCLUDE += -I$(srctree)/arch/$(SUBARCH)/include
LINUXINCLUDE += -I$(srctree)/arch/$(SUBARCH)/include/uapi
LINUXINCLUDE += -I$(srctree)/arch/$(SUBARCH)/include/generated
LINUXINCLUDE += -I$(srctree)/arch/$(SUBARCH)/include/generated/uapi

KBUILD_AFLAGS += $(KBUILD_CFLAGS)
KBUILD_CFLAGS += -D__arch_um__ -D__PikeOS__

libs-y += arch/p4/lib/
libs-y += $(PIKEOS_NATIVE_LIBS)

# first compile version.o to check PikeOS kernel version
init-y += arch/p4/init/

core-y += arch/p4/kernel/
core-y += arch/p4/mm/
core-y += arch/p4/arch-$(SUBARCH)/
core-$(CONFIG_BSP) += arch/p4/bsp/

drivers-y += arch/p4/drivers/
drivers-$(CONFIG_PCI_VIRT) += arch/p4/pci/

ifeq ($(CONFIG_DEBUGSYM),y)
KBUILD_CFLAGS       := $(subst -fomit-frame-pointer,,$(KBUILD_CFLAGS))
KBUILD_CFLAGS       += -g
endif

all: stripped

# filechk works with a single input file only
define filechk_gen-pikeos-api-version
	(set -e; \
	grep -F "#define P4_API_VERSION" | sed -e 's/P4_API_VERSION/PIKEOS_API_VERSION/g';)
endef

define filechk_gen-pikeos-vm-version
	(set -e; \
	grep -F "#define VM_VERSION" | sed -e 's/VM_VERSION/PIKEOS_VM_VERSION/g';)
endef

include/generated/pikeos_api_version.h: $(PIKEOS_TARGET_FILES)/include/kernel/p4apiversion.h FORCE
	$(call filechk,gen-pikeos-api-version)

include/generated/pikeos_vm_version.h: $(PIKEOS_TARGET_FILES)/include/core/vm_core_constants.h FORCE
	$(call filechk,gen-pikeos-vm-version)

$(PIKEOS_TARGET_FILES)/include/core/vm_core_constants.h:
$(PIKEOS_TARGET_FILES)/include/kernel/p4apiversion.h:
	$(Q)echo >&2
	$(Q)echo >&2 "  ERROR: Kernel configuration invalid."
	$(Q)echo >&2 "         PikeOS target folder missing:"
	$(Q)echo >&2 "         $(PIKEOS_TARGET_FILES)"
	$(Q)echo >&2
	$(Q)/bin/false
	
# create arch/ and arch-override/ this is used by P4Linux to force
# including the header file from outside the standard location which is
# arch/p4/include
archprepare: include/generated/pikeos_api_version.h include/generated/pikeos_vm_version.h
	$(Q)rm -rf include/arch include/arch-override \
		include-src1/arch include-src1/arch-override \
		include-src2/arch include-src2/arch-override

	$(Q)$(if $(wildcard $(srctree)), \
		mkdir -p include; \
		ln -fsn $(srctree)/arch/$(SUBARCH)/include include/arch; \
		ln -fsn $(srctree)/arch/p4/arch-$(SUBARCH)/include include/arch-override; \
	)
	$(Q)$(if $(wildcard $(SRC1DIR)), \
		mkdir -p include-src1; \
		ln -fsn $(SRC1DIR)/arch/$(SUBARCH)/include include-src1/arch; \
		ln -fsn $(SRC1DIR)/arch/p4/arch-$(SUBARCH)/include include-src1/arch-override; \
	)
	$(Q)$(if $(wildcard $(SRC2DIR)), \
		mkdir -p include-src2; \
		ln -fsn $(SRC2DIR)/arch/$(SUBARCH)/include include-src2/arch; \
		ln -fsn $(SRC2DIR)/arch/p4/arch-$(SUBARCH)/include include-src2/arch-override; \
	)

	$(Q)mkdir -p arch/p4/lib
	$(Q)ln -fsn $(PIKEOS_TARGET_FILES)/lib arch/p4/lib/native

ifneq ($(CONFIG_BSP),)
ifeq ($(SUBARCH),arm)
	$(Q)$(MAKE) $(build)=arch/$(SUBARCH)/tools include/generated/mach-types.h
endif
endif

stripped: vmlinux
	cp vmlinux linux
	$(STRIP) -R .comment -R .note linux

install: stripped

archdep: setup

archmrproper:
	rm -rf include/arch include/arch-override \
		include-src1/arch include-src1/arch-override \
		include-src2/arch include-src2/arch-override

archclean:
	find . \( -name '*.bb' -o -name '*.bbg' -o -name '*.da' \
		-o -name '*.gcov' \) -type f -print | xargs rm -f
	cd arch/p4 ; \
	for dir in $(subdirs) ; do $(MAKE) -C $$dir clean; done

export SUBARCH ELF_ARCH ELF_FORMAT HEADER_ARCH ARCH_JIFFIES

# used when building the libraries
export PIKEOS_CFLAGS PIKEOS_INCLUDE

define archhelp
  @echo '* stripped        - Stripped binary kernel image (./linux)'
  @echo '                    The unstripped image is available as ./vmlinux'
endef
