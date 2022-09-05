#include "asm/errno.h"

int sys_modify_ldt(int func, void *ptr, unsigned long bytecount)
{
	/* not implemented */
	return -ENOSYS;
}
