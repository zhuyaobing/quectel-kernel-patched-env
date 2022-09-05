#ifndef __P4_CHECKSUM_H
#define __P4_CHECKSUM_H

#ifdef CONFIG_P4_X86

/* The UM version uses a good version of copy_from/to_user */
#include "../../../x86/um/asm/checksum.h"

#else

#include <arch-override/asm/checksum.h>

/*
	This function must remain here, because the architecure specific one
	is using optimized assembler code with exceptions.
*/
static __inline__
__wsum csum_partial_copy_from_user(const void __user *src, void *dst, int len, __wsum sum, int *err_ptr)
{
	if (copy_from_user(dst, src, len)) {
		*err_ptr = -EFAULT;
		return (__force __wsum)-1;
	}

	return csum_partial(dst, len, sum);
}

#endif /* !CONFIG_P4_X86 */
#endif /* __P4_CHECKSUM_H */
