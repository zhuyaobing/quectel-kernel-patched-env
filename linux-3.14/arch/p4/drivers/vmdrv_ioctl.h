/*
 *  ioctl()s of P4 related drivers
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 * 
 *  Copyright (C) 2006-2009 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 */

#ifndef _VMDRV_IOCTL_H_
#define _VMDRV_IOCTL_H_

/* IOCTLs to retrieve information of the PikeOS related drivers */

#define VMDRV_MAGIC			0x42

#define VMDRV_GET_PORT_SIZE		_IOR(VMDRV_MAGIC, 0, unsigned int)
#define VMDRV_GET_PORT_DIRECTION	_IOR(VMDRV_MAGIC, 1, unsigned int)
#define VMDRV_GET_PORT_SIZE2		_IOR(VMDRV_MAGIC, 2, unsigned int)
#define VMDRV_GET_FILE_SIZE		_IOR(VMDRV_MAGIC, 3, unsigned int)
#define VMDRV_GET_EVENT_MASK		_IOR(VMDRV_MAGIC, 4, unsigned long)
#define VMDRV_SET_EVENT_MASK		_IOW(VMDRV_MAGIC, 5, unsigned long)
#define VMDRV_EVENT_WAIT		_IOW(VMDRV_MAGIC, 6, unsigned long)
#define VMDRV_GET_UID			_IOW(VMDRV_MAGIC, 7, unsigned long)
#define VMDRV_GET_PORT_REMAIN		_IOR(VMDRV_MAGIC, 8, unsigned int)
#define VMDRV_IOCTL			_IOWR(VMDRV_MAGIC, 0x12, uint8_t[140])

/* input flags for EVENT_WAIT */
#define VMDRV_EVENT_PRECLEAR		1
#define VMDRV_EVENT_POSTCLEAR		2

#ifndef __KERNEL__
/* maintained for historic reasons */
#define VMPORT_CORE_DIR_RECV		0
#define VMPORT_CORE_DIR_SEND		1

#define VMPORT_CORE_IOCTL_GET_FRAMESIZE	VMDRV_GET_PORT_SIZE
#define VMPORT_CORE_IOCTL_GET_DIRECTION	VMDRV_GET_PORT_DIRECTION
#endif /* __KERNEL__ */

#endif /* _VMDRV_IOCTL_H_ */
