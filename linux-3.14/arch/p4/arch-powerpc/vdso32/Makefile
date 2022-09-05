# den@sysgo.com: copied from arch/powerpc/kernel/vdso32/ and modified to use the
#                source files from the native directory and store the object
#                files into arch/p4

# List of files in the vdso, has to be asm only for now

obj-vdso32-$(CONFIG_PPC64) = getcpu.o
obj-vdso32 = sigtramp.o gettimeofday.o datapage.o cacheflush.o note.o \
		$(obj-vdso32-y)

# Build rules

ifeq ($(CONFIG_PPC32),y)
CROSS32CC := $(CC)
endif

targets := $(obj-vdso32) vdso32.so vdso32.so.dbg
obj-vdso32 := $(addprefix $(obj)/, $(obj-vdso32))

GCOV_PROFILE := n

ccflags-y := -shared -fno-common -fno-builtin
ccflags-y += -nostdlib -Wl,-soname=linux-vdso32.so.1 \
		$(call cc-ldoption, -Wl$(comma)--hash-style=sysv)
asflags-y := -D__VDSO32__ -s

obj-y += vdso32_wrapper.o
extra-y += vdso32.lds
CPPFLAGS_vdso32.lds += -P -C -Upowerpc

# Force dependency (incbin is bad)
$(obj)/vdso32_wrapper.o : $(obj)/vdso32.so

# special rule for here, our sources are located in a different folder
$(obj)/%.o: $(src)/../../../powerpc/kernel/vdso32/%.S FORCE

# link rule for the .so file, .lds has to be first
$(obj)/vdso32.so.dbg: $(src)/vdso32.lds $(obj-vdso32)
	$(call if_changed,vdso32ld)

$(obj)/vdso32.lds: $(src)/../../../powerpc/kernel/vdso32/vdso32.lds.S FORCE
	$(call if_changed_dep,cpp_lds_S)

# strip rule for the .so file
$(obj)/%.so: OBJCOPYFLAGS := -S
$(obj)/%.so: $(obj)/%.so.dbg FORCE
	$(call if_changed,objcopy)

# assembly rules for the .S files
$(obj-vdso32): $(obj)/%.o: $(src)/../../../powerpc/kernel/vdso32/%.S
	$(call if_changed_dep,vdso32as)

# actual build commands
quiet_cmd_vdso32ld = VDSO32L $@
      cmd_vdso32ld = $(CROSS32CC) $(c_flags) -Wl,-T $^ -o $@
quiet_cmd_vdso32as = VDSO32A $@
      cmd_vdso32as = $(CROSS32CC) $(a_flags) -c -o $@ $<

# install commands for the unstripped file
quiet_cmd_vdso_install = INSTALL $@
      cmd_vdso_install = cp $(obj)/$@.dbg $(MODLIB)/vdso/$@

vdso32.so: $(obj)/vdso32.so.dbg
	@mkdir -p $(MODLIB)/vdso
	$(call cmd,vdso_install)

vdso_install: vdso32.so
