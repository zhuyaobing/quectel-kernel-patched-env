#include <asm-generic/vmlinux.lds.h>
#include <asm/page.h>
#include <asm/cache.h>

#ifndef CONFIG_P4_ARM
OUTPUT_FORMAT(ELF_FORMAT)
#endif

OUTPUT_ARCH(ELF_ARCH)
ENTRY(linux_main)
jiffies = ARCH_JIFFIES;

#ifdef CONFIG_P4_X86
PHDRS {
	text PT_LOAD FLAGS(5);          /* R_E */
	data PT_LOAD FLAGS(6);          /* RW_ */
#ifdef CONFIG_X86_64
	init PT_LOAD FLAGS(7);          /* RWE */
#endif
	note PT_NOTE FLAGS(0);          /* ___ */
}
#endif

SECTIONS
{
	/*. = 0x0 + SIZEOF_HEADERS;*/
	. = 0x0 + 0x20000;	/* don't use first 128K to catch NULL-ptr access */

	. = ALIGN(4096);             /* Init code and data */
	_stext = .;
	. = ALIGN(4096);

	/* used by PikeOS to detect elf header */
	.interp : { *(.interp) }

	.text : {
		/* _text used by KALLSYMS */
		_text = .;
		TEXT_TEXT
		SCHED_TEXT
		LOCK_TEXT
		KPROBES_TEXT
		*(.fixup)
		*(.stub .text.* .gnu.linkonce.t.*)
		/* .gnu.warning sections are handled specially by elf32.em.  */
		*(.gnu.warning)
		*(.got1)
		__got2_start = .;
		*(.got2)
		__got2_end = .;
	}
#ifdef CONFIG_P4_X86
		:text
#endif
	_etext = .;
	PROVIDE (_etext = .);
  
#ifdef CONFIG_P4_X86
	NOTES :text :note
#endif

	EXCEPTION_TABLE(16)
#ifdef CONFIG_P4_X86
		:text
#endif

	RO_DATA(PAGE_SIZE)

	. = ALIGN(4096);		/* Init code and data */
	__init_begin = .;
	/*
	* evil automagic to get our initial bootstack for PikeOS
	* we provide a section "._vm_stack" that contains:
	* - a pointer to the stack size
	* - a pointer to the stack itself
	*/
	_vm_stack_seg = .;
	._vm_stack ALIGN(4):
	{
#ifdef CONFIG_X86_64
		QUAD(ABSOLUTE(_vm_stack_size));
		QUAD(ABSOLUTE(_vm_stack));
#else
		LONG(ABSOLUTE(_vm_stack_size));
		LONG(ABSOLUTE(_vm_stack));
#endif
	}
#ifdef CONFIG_P4_X86
		:data
#endif

#ifdef CONFIG_P4_ARM
	.init_arm_arch_info : {
		__arch_info_begin = .;
			*(.arch.info.init)
		__arch_info_end = .;
	}
#endif

	INIT_TEXT_SECTION(0)
#ifdef CONFIG_X86_64
		:init
#endif

	.init.setup : AT(ADDR(.init.setup) - LOAD_OFFSET) {
		INIT_SETUP(16)
	}

	/* initcall section */
	.initcall.init : {
		INIT_CALLS
	}

	.con_initcall.init : AT(ADDR(.con_initcall.init) - LOAD_OFFSET) {
		CON_INITCALL
	}

	SECURITY_INIT
	. = ALIGN(4);
#ifdef CONFIG_P4_X86
	__alt_instructions = .;
	.altinstructions : AT(ADDR(.altinstructions) - LOAD_OFFSET) {
		*(.altinstructions)
	}
	__alt_instructions_end = .;
	.altinstr_replacement : AT(ADDR(.altinstr_replacement) - LOAD_OFFSET) {
		*(.altinstr_replacement)
	}
#endif

#ifdef CONFIG_P4_X86
	/*
	 * struct iommu_table_entry entries are injected in this section.
	 * It is an array of IOMMUs which during run time gets sorted depending
	 * on its dependency order. After rootfs_initcall is complete
	 * this section can be safely removed.
	 */
	.iommu_table : AT(ADDR(.iommu_table) - LOAD_OFFSET) {
		__iommu_table = .;
		*(.iommu_table)
		__iommu_table_end = .;
	}
	. = ALIGN(8);
#endif

	/* FIXME: SMP: adjust cache line size! */
	PERCPU_SECTION(32)

	/* .exit.text is discard at runtime, not link time, to deal with references
	from .altinstructions and .eh_frame */
	.exit.text : AT(ADDR(.exit.text) - LOAD_OFFSET) { *(.exit.text) }
	.exit.data : AT(ADDR(.exit.data) - LOAD_OFFSET) { *(.exit.data) }
  
	.init.ramfs : AT(ADDR(.init.ramfs) - LOAD_OFFSET) {
		INIT_RAM_FS
	}

	/* freed after init ends here */
	. = ALIGN(PAGE_SIZE);
	__init_end = .;

#ifdef CONFIG_P4_X86
	/*
	 * smp_locks might be freed after init
	 * start/end must be page aligned
	 */
	. = ALIGN(PAGE_SIZE);
	.smp_locks : AT(ADDR(.smp_locks) - LOAD_OFFSET) {
		__smp_locks = .;
		*(.smp_locks)
		. = ALIGN(PAGE_SIZE);
		__smp_locks_end = .;
	}
#endif

	init.data : { INIT_DATA }
	.data : {
		/* Start of data section */
		_sdata = .;

		INIT_TASK_DATA(KERNEL_STACK_SIZE);		/* init_task */
		CACHELINE_ALIGNED_DATA(L1_CACHE_BYTES);
		DATA_DATA
		CONSTRUCTORS
	}
#ifdef CONFIG_P4_X86
		:data
#endif

	_edata = .;
	PROVIDE (edata = .);

	BSS_SECTION(0, 0, 0)
	_end = .;
	PROVIDE (end = .);

	STABS_DEBUG
	DWARF_DEBUG

	/* Sections to be discarded */
	DISCARDS

#ifdef CONFIG_P4_ARM
	/* CONFIG_SMP_ON_UP not available on P4, always discard this section */
	/DISCARD/ : {
		*(.alt.smp.init)
	}
#endif
}
