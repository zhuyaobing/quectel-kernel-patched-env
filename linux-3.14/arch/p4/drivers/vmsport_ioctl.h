/*
 *  Data Structure for Sampling Port Access
 * 
 *  Copyright (C) 2006 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 * 20061130, azu@sysgo.de: taken from POSIX sys/sport.h
 *
 */

#ifndef _VMSPORT_IOCTL_H_
#define _VMSPORT_IOCTL_H_

#ifndef __KERNEL__
#include <stdint.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#endif

/* Hold in sync with vm_port_types.h, vm_sport_msg_validity_t */
enum _sport_validity {
	SPORT_EMPTY          = 1,
	SPORT_VALID          = 2,
	SPORT_INVALID        = 3,
	SPORT_NOT_APPLICABLE = 4,
};

enum _sport_directcion {
	SPORT_SOURCE,
	SPORT_DESTINATION
};

/** @brief
 *    Data type for ioctl() operations on sampling ports.
 *
 */
typedef struct {
	union {
		struct {
			void *buf;
			size_t buf_size;
			size_t read_size;
			enum _sport_validity validity;
		} read;
		struct {
			void *buf;
			size_t buf_size;
		} write;
		struct {
			/** Port direction. */
			enum _sport_directcion direction;
			/** Port's configured message size. */
			size_t msg_size;
			/** Port's configured refresh period. */
			struct timespec refresh;
			/** */
			enum _sport_validity validity;
		} pstat;
	} sport_op;
} _sport_ioc_t;

#define SPORT_READ		_IOWR('S', 1, _sport_ioc_t)
#define SPORT_WRITE		_IOW('S', 2, _sport_ioc_t)
#define SPORT_PSTAT		_IOR('S', 3, _sport_ioc_t)

/*
 * ioctl() errors:
 *
 * EFAULT:
 *      copy in/out problem, bad pointers
 * EINVAL:
 *      read on source port or write on destination port
 * EMSGSIZE:
 *      buffer too small for read or too large for write
 * ENODATA:
 *      EMPTY error for read operation
 * EIO:
 *      Other errors during vm_sport_*() call
 */

#endif /* _VMSPORT_IOCTL_H_ */
