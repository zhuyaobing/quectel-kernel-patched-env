#ifndef _ASM_P4_X86_ASM_H
#define _ASM_P4_X86_ASM_H

#include <arch/asm/asm.h>

/* x86 has switched to a special exception handling with relative entries (check
 * commit 706276543b699d80f546e45f8b12574e7b18d952). Go back to the generic
 * handler.
 */
#undef _ASM_EXTABLE
#undef _ASM_EXTABLE_EX

/* Exception table entry */
#ifdef __ASSEMBLY__
# define _ASM_EXTABLE(from,to)					\
	.pushsection "__ex_table","a" ;				\
	_ASM_ALIGN ;						\
	_ASM_PTR from , to ;					\
	.popsection

# define _ASM_EXTABLE_EX(from,to)				\
	.pushsection "__ex_table","a" ;				\
	._ASM_ALIGN ;						\
	_ASM_PTR from , (to) - (from) ;				\
	.popsection
#else
# define _ASM_EXTABLE(from,to) \
	" .pushsection \"__ex_table\",\"a\"\n"			\
	_ASM_ALIGN "\n"						\
	_ASM_PTR #from "," #to "\n"				\
	" .previous\n"

# define _ASM_EXTABLE_EX(from,to)				\
	" .pushsection \"__ex_table\",\"a\"\n"			\
	_ASM_ALIGN "\n"						\
	 _ASM_PTR #from ",(" #to ")-(" #from ")\n"		\
	" .popsection\n"
#endif

#endif /* _ASM_P4_X86_ASM_H */
