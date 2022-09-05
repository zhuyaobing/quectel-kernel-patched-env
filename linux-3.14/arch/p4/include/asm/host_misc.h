/*
 *  Miscellaneous p4 function
 * 
 *  Copyright (C) 2013 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 */

#ifndef __HOST_MISC_H_
#define __HOST_MISC_H_

int uk_dev_call (unsigned int devid, unsigned int function, unsigned long arg1,
		 unsigned long arg2, unsigned long arg3, unsigned long arg4);

#endif /* ! __HOST_MISC_H_ */
