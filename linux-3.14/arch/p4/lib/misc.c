/*
 *  Miscellaneous p4 function
 * 
 *  Author: Alban Tilatti <ati@sysgo.de>
 * 
 *  Copyright (C) 2013 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 */

#include "p4.h"

/*
 * uk_dev_call()
 *
 * Wrapper for call to a PikeOS kernel level device driver
 */
int uk_dev_call (unsigned int devid, unsigned int function, unsigned long arg1,
		 unsigned long arg2, unsigned long arg3, unsigned long arg4)
{
	P4_e_t err;

	err = p4_dev_call(devid, function, arg1, arg2, arg3, arg4);

	return -err;
}
