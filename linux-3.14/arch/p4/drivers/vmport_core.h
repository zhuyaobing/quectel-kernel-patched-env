/*
 *  P4VM port core driver
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 * 
 *  Copyright (C) 2003-2012 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 */

#ifndef _VMPORT_CORE_H_
#define _VMPORT_CORE_H_

#ifdef __KERNEL__
#define VMPORT_CORE_TYPE_NONE	0
#define VMPORT_CORE_TYPE_TTY		1
#define VMPORT_CORE_TYPE_FILE	2
#define VMPORT_CORE_TYPE_NET		3
#define VMPORT_CORE_TYPE_LINK		4

#define VMPORT_CORE_DIR_RECV		0
#define VMPORT_CORE_DIR_SEND		1

struct vmport_core_portinfo
{
	/* driver type - low level */
	unsigned int type;
	unsigned int direction;
	spinlock_t lock;
	unsigned int line;
	unsigned int irq;
	unsigned int count;
	char *portname;
	void (*irqfunc)(void *);	/* irq callback func */
	void *irqdata;	/* irq callback data */

	/* states part */
	volatile int state_ready;
	int error;

	/* buffer */
	int framesize;
	unsigned char *buf;
	size_t buf_size;
	int external_buf;
};

/* exported by core driver */
extern int vmport_core_count;	/* number of PORTS in system */
extern struct vmport_core_portinfo *vmport_core_portinfo;	/* port specific information */

extern int vmport_core_init_port(struct vmport_core_portinfo *port,
								  void (*func)(void *), void *data,
								  unsigned int type, char *buf, int wakeup);
extern void vmport_core_exit_port(struct vmport_core_portinfo *port);

extern int vmport_core_file_read(struct vmport_core_portinfo *iport,
				 struct mutex *lock, wait_queue_head_t *wait,
				 struct file *file, char __user *buffer,
				 size_t count);
extern int vmport_core_file_write(struct vmport_core_portinfo *oport,
				  struct mutex *lock, wait_queue_head_t *wait,
				  struct file *file, const char __user *buffer,
				  size_t count);
extern int vmport_core_stat_num_msg(struct vmport_core_portinfo *port,
									unsigned int *nb_messages,
									unsigned int *max_messages);
extern int vmport_core_poll_start(struct vmport_core_portinfo *port);
extern int vmport_core_poll_stop(struct vmport_core_portinfo *port);


#endif /* __KERNEL__ */

#endif /* _VMPORT_CORE_H_ */
