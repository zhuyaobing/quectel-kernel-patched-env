/*
 *  PikeOS VM abstraction layer
 *
 *  Author: Alex Zuepke <azu@sysgo.de>
 *
 *  Copyright (C) 2003-2015 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 *  Changelog:
 *  - PIKEOS_VM_VERSION >= 34 (PikeOS 4.0) introduced combined VM_ and P4_E_
 *    error codes.
 */

/* PikeOS headers */
#include <vm.h>
#include <vm_io_sbuf.h>			/* for libsbuf */

/* the PikeOS definition collides with a header file on x86 */
#undef __unused

/* Linux headers (keep as small as possible to avoid name clashes) */
#include <linux/export.h>		/* for EXPORT_SYMBOL */
#include <linux/string.h>

#include <uapi/asm-generic/errno.h>	/* for linux error codes */
#include <uapi/asm-generic/termios.h>	/* for TIOCM_ */
#include <uapi/linux/ethtool.h>		/* SPEED_ */
#include <uapi/linux/stat.h>		/* S_IFDIR */

#include <asm/pikeos_version.h>
#include <asm/termbits.h>
#include <asm/vmapi_calls.h>
#include <asm/locking.h>
#include <asm/wrapper.h>
#include <asm/debugout.h>
#include <asm/smp.h>

#ifdef CONFIG_PCI_VIRT
#include <linux/pci.h>
#include <asm/tlbflush.h>
#define VM_CLIENT_TYPE_EXT
#include <vm_pci.h>

#ifndef VM_PCI_API_VERSION
/* PikeOS 3.4 uses same version as 3.5 */
#define VM_PCI_API_VERSION 2
#endif

#endif

#ifdef VMAPI_TRACE
#define TLOG(f, ...) pr_debug("%s" f, __func__, __VA_ARGS__)
#else
#define TLOG(f, ...)
#endif

#if PIKEOS_DRV_API_DDK
#include <drv_ser_api_types.h>
#include <drv_net_api_types.h>
#else
#include <drv/drv_ser_api.h>
#include <drv/drv_net_api.h>
#endif

/*
 * local definitions
 */
#define FS_UNUSED	0
#define FS_ALLOC	1
#define FS_OPEN		2

#define P4_ADDR_T  void*
#define VMPORT_SOURCE         VM_PORT_SOURCE
#define VMPORT_DESTINATION    VM_PORT_DESTINATION
#define VMPORT_CONNECTED      VM_PORT_CONNECTED
#define VMPORT_OPENED         VM_PORT_CREATE

/* verify VMAPI defines with VM ones */
#if VMAPI_NAME_LEN != VM_NAME_LEN
#error Please adapt VMAPI_NAME_LEN to VM_NAME_LEN
#endif

/* offset on which P4 errors will be mapped */
#define CONV_ERROR_OFFSET 1000

/* default number of open files and directories */
#define MAX_FILES_DEFAULT	64
#if PIKEOS_VM_VERSION >= 34
#define MAX_DIRS_DEFAULT	64
#endif

int p4_error_to_linux(vm_e_t vm_error)
{
	switch (vm_error) {
#if PIKEOS_VM_VERSION >= 34
	case P4_E_OK:
		return 0;
	case P4_E_PERM:
		return -EPERM;
	case P4_E_NOENT:
		return -ENOENT;
	case P4_E_IO:
		return -EIO;
	case P4_E_TIMEOUT:
		return -EAGAIN;
	case P4_E_LIMIT:
		return -ENOSPC;
	case P4_E_EXIST:
		return -EEXIST;
	case P4_E_NAME:
		return -ENAMETOOLONG;
	case P4_E_OOFILE:
		return -EMFILE;
	case P4_E_INVAL:
		/* this error has various meanings; handle it locally */
		return -(CONV_ERROR_OFFSET + vm_error);
#else
	case VM_E_OK:
		return 0;
	case VM_E_PERM:
		return -EPERM;
	case VM_E_NO_ENT:
		return -ENOENT;
	case VM_E_IO:
		return -EIO;
	case VM_E_AGAIN:
		return -EAGAIN;
#endif
	default:
		return -(CONV_ERROR_OFFSET + vm_error);
	}
}

/*
 * global variables
 */

int vmapi_max_files;
#if PIKEOS_VM_VERSION >= 34
int vmapi_max_dirs;
#endif

/*
 * local variables
 */

/* number of queueing ports in the system */
static int max_qports;
/* handles for open queueing ports */
static vm_port_desc_t *qport_handles;

/* sampling port handles */
static int max_sports;
static vm_port_desc_t *sport_handles;

/* handles for open files */
int vmapi_max_files;
static vm_file_desc_t *file_handles;
static int *file_states;
static struct uk_mutex file_lock;     /* protects files local variables ... */

#if PIKEOS_VM_VERSION >= 34
/* handles for open dirs */
int vmapi_max_dirs;
static vm_dir_t *dir_handles;
static int *dir_states;
static struct uk_mutex dir_lock;     /* protects dirs local variables ... */
#endif

/* handles for SHM-Bufs */
static drv_sbuf_desc_t sbuf_handles[VMAPI_MAX_SHM];
static int sbuf_states[VMAPI_MAX_SHM];
/* protect global vars for concurrent access */
static struct uk_mutex sbuf_lock;

static char partition_name[VMAPI_NAME_LEN];
static char process_name[VMAPI_NAME_LEN];

/* --------------------------------------------------------------------- */

/*
 * initialize VMAPI library
 */
void vmapi_init(void)
{
	vm_init();
}

/*
 * All vm_xxx functions may block. Using PikeOS UP is not critical because the
 * calling thread goes into blocking mode and the CPU is scheduled to the
 * partition deamon or system software (higher priority).
 *
 * In PikeOS SMP mode, it may happen that the current thread is not running on
 * core 0, thus the current thread is getting blocked and another thread on the
 * same core may start (e.g. linux main CPU thread). This *is* critical because
 * linux thinks it is running in an interrupt/atomic context and cannot run the
 * main CPU thread. In this case linux complains about scheduling while atomic
 * or produces other warnings.
 *
 * On the other side, these functions are safe to call from any PikeOS context.
 *
 * Possible modes:
 *
 *	if (kthread_is_irq()) {
 *		IRQ thread, check wheater we are in linux or PikeOS mode
 *
 *		in_irq: not possible to detect the correct mode, even in a
 *		        PikeOS thread the structure is correctly set
 *
 *		arch_irqs_disabled: is safe to use because
 *		                    arch_local_irq_lock is called when
 *		                    entering do_IRQ to serialize incoming
 *		                    interrupts
 *
 *		if (arch_irqs_disabled())
 *			bad: linux mode
 *		else
 *			good: PikeOS mode
 *	} else if (kthread_thread_id() == 0) {
 *		good: boot (pre start_kernel)
 *		bad: exception handler TODO
 *	} else {
 *		per CPU main threads
 *		check if we're in atomic context (e.g. hard/soft IRQ)
 *		or interrupts are disabled
 *		if (in_atomic() == 0 && irqs_disabled() == 0)
 *			good: preemption possible
 *		else
 *			good: IRQs already locked
 *	}
 */

/*
 * This macro can be used in all cases, even directly after calling vmapi_init()
 * because kthread_native_cpu_num() will return 0 until kthread_init() has been
 * called.
 */
#define VMAPI_CALL_ENTER() \
	do { \
		if (kthread_native_cpu_num() == 0) \
			break; \
		if (kthread_is_irq() && arch_irqs_disabled()) \
			p4_thread_stop(kthread_main_thread(hard_smp_processor_id())); \
	} while (0)

#define VMAPI_CALL_EXIT() \
	do { \
		if (kthread_native_cpu_num() == 0) \
			break; \
		if (kthread_is_irq() && arch_irqs_disabled()) \
			p4_thread_resume(kthread_main_thread(hard_smp_processor_id())); \
	} while (0)

/* --------------------------------------------------------------------- */

/*
 * initialize VMAPI library
 */
void __init vmapi_fs_init(void)
{
	char proppath[VM_NAME_LEN + 32];
	vm_file_desc_t fd;
	P4_uint32_t num;
	vm_e_t err;

	vmapi_max_files = MAX_FILES_DEFAULT;
#if PIKEOS_VM_VERSION >= 34
	vmapi_max_dirs = MAX_DIRS_DEFAULT;
#endif

	sprintf(proppath, "prop:app/%s",
#if PIKEOS_VM_VERSION >= 34
		vmapi_get_process_name()
#else
		/* < PikeOS 4.0 */
		vmapi_get_partname()
#endif
		);

	VMAPI_CALL_ENTER();
	err = vm_open(proppath, VM_O_RD, &fd);
	if (err == VM_E_OK) {
		err = VM_PROP_READ_UINT32(&fd, "max_open_files", &num);
		if (err == VM_E_OK)
			vmapi_max_files = num;

#if PIKEOS_VM_VERSION >= 34
		err = VM_PROP_READ_UINT32(&fd, "max_open_dirs", &num);
		if (err == VM_E_OK)
			vmapi_max_dirs = num;
#endif

		vm_close(&fd);
	}
	VMAPI_CALL_EXIT();

	file_handles = wrapped_malloc(vmapi_max_files * sizeof(file_handles[0]));
	file_states = wrapped_malloc(vmapi_max_files * sizeof(file_states[0]));
	if (!file_handles || !file_states)
		goto nomem;

#if PIKEOS_VM_VERSION >= 34
	dir_handles = wrapped_malloc(vmapi_max_dirs * sizeof(dir_handles[0]));
	dir_states = wrapped_malloc(vmapi_max_dirs * sizeof(dir_states[0]));
	if (!dir_handles || !dir_states)
		goto nomem;
#endif

	return;

nomem:
	die_early("unable to allocate memory for PikeOS FS access");
}

/*
 * initialize: count number of queueing ports
 */
void __init vmapi_qport_init(void)
{
	vm_qport_stat_t stat;
	P4_uint32_t flags;
	vm_e_t err;
	int i;

	/*
	 * iterate all queueing ports:
	 * get number, alloc data, get status
	 */
	VMAPI_CALL_ENTER();
	for (i = 0; i < VM_QPORT_MAXPORTS; i++) {
		err = vm_qport_iterate(i, &stat);
		if (err != VM_E_OK)
			break;
	}
	VMAPI_CALL_EXIT();
	max_qports = i;

	if (max_qports == 0)
		return;

	qport_handles = wrapped_malloc(max_qports * sizeof(vm_port_desc_t));
	if (!qport_handles)
		die_early("vmapi_qport_init(): malloc failed");

	for (i = 0; i < max_qports; i++) {
		VMAPI_CALL_ENTER();
		err = vm_qport_iterate(i, &stat);
		VMAPI_CALL_EXIT();

		if (stat.state == VMPORT_CONNECTED) {
			if (stat.direction == VMPORT_SOURCE)
				flags = VMPORT_SOURCE;
			else
				flags = VMPORT_DESTINATION;

			VMAPI_CALL_ENTER();
			err = vm_qport_open(stat.name, flags, &qport_handles[i]);
			VMAPI_CALL_EXIT();
			if (err != VM_E_OK)
				die_early("vmapi_qport_init(): couldn't open port");
		}
	}
}

/*
 * return number of ports in system
 */
int vmapi_qport_get_count(void)
{
	return max_qports;
}
EXPORT_SYMBOL(vmapi_qport_get_count);

static char tmpname[VM_NAME_LEN];

/*
 * helper function: return some port status (constant attributes)
 *
 */
int vmapi_qport_stat(int portnum, int *isoutgoing, int *framesize, char **portname)
{
	vm_qport_stat_t stat;
	vm_e_t err;
	int i;

	if (portnum < 0 || portnum >= max_qports)
		return -EBADF;	/* error: port outer range  */

	VMAPI_CALL_ENTER();
	err = vm_qport_iterate(portnum, &stat);
	VMAPI_CALL_EXIT();
	if (err)
		return p4_error_to_linux(err);	/* error: getqport failed */

	*isoutgoing = (stat.direction == VMPORT_SOURCE) ? 1 : 0;
	*framesize = stat.max_msg_size;
	for (i = 0; i < sizeof(tmpname); i++)
		tmpname[i] = ((char *)stat.name)[i];
	*portname = tmpname;

	return 0; /* OK */
}
EXPORT_SYMBOL(vmapi_qport_stat);

/*
 * helper function: return number of unread/free messages (port runtime into)
 *
 * - destination port: number of messages which can be read until empty.
 * - source port: number of messages which can be written until full.
 */
int vmapi_qport_stat_num_msg(int portnum, unsigned int *nb_messages, unsigned int *max_messages)
{
	vm_qport_stat_t stat;
	vm_e_t err;

	if (portnum < 0 || portnum >= max_qports)
		return -EBADF;	/* error: port outer range  */

	VMAPI_CALL_ENTER();
	err = vm_qport_iterate(portnum, &stat);
	VMAPI_CALL_EXIT();
	if (err)
		return p4_error_to_linux(err);	/* error: getqport failed */

	*nb_messages = stat.nb_messages;
	*max_messages = stat.max_nb_messages;
	return 0; /* OK */
}
EXPORT_SYMBOL(vmapi_qport_stat_num_msg);

/*
 * helper function: return if the queuing port is connected or not
 *
 * - portnum: port ID
 */
int vmapi_qport_check_state(int portnum)
{
	vm_qport_stat_t stat;
	vm_e_t err;

	if (portnum < 0 || portnum >= max_qports)
		return -EBADF;	/* error: port outer range  */

	VMAPI_CALL_ENTER();
	err = vm_qport_iterate(portnum, &stat);
	VMAPI_CALL_EXIT();
	if (err)
		return p4_error_to_linux(err);	/* error: getqport failed */

	return (stat.state == VMPORT_OPENED);
}
EXPORT_SYMBOL(vmapi_qport_check_state);

/*
 * helper function: receive something
 */
int vmapi_qport_receive(int portnum, void *buf, int count)
{
	vm_e_t err;
	P4_size_t size;

	if (portnum < 0 || portnum >= max_qports)
		return -EBADF;	/* error: port outer range  */

	VMAPI_CALL_ENTER();
	err = vm_qport_read(&qport_handles[portnum], buf, count,
		       P4_TIMEOUT_INFINITE, &size);
	VMAPI_CALL_EXIT();

	if (err)
		return p4_error_to_linux(err);	/* some error */

	return size;
}
EXPORT_SYMBOL(vmapi_qport_receive);

/*
 * helper function: send something
 */
int vmapi_qport_send(int portnum, const void *buf, int count)
{
	vm_e_t err;

	if (portnum < 0 || portnum >= max_qports)
		return -EBADF;	/* error: port outer range  */

	VMAPI_CALL_ENTER();
	err = vm_qport_write(&qport_handles[portnum], buf, count,
		       P4_TIMEOUT_INFINITE);
	VMAPI_CALL_EXIT();

	if (err)
		return p4_error_to_linux(err);	/* some error */

	return count;
}
EXPORT_SYMBOL(vmapi_qport_send);

/*
 * helper function: add queuing port to select()
 */
int vmapi_qport_poll_set(int portnum)
{
	vm_e_t err;
	vm_qport_bitmap_t clear_mask, set_mask, out_mask;

	if (portnum < 0 || portnum >= max_qports)
		return -EBADF;	/* error: port outer range  */

	VM_QPORT_CLEAR_SET(&clear_mask);
	VM_QPORT_CLEAR_SET(&set_mask);
	VM_QPORT_SET_BIT(&set_mask, &qport_handles[portnum]);

	VMAPI_CALL_ENTER();
	err = vm_qport_mask(&clear_mask, &set_mask, &out_mask);
	VMAPI_CALL_EXIT();

	return p4_error_to_linux(err);
}
EXPORT_SYMBOL(vmapi_qport_poll_set);

/*
 * helper function: remove queuing port from select()
 */
int vmapi_qport_poll_clear(int portnum)
{
	vm_e_t err;
	vm_qport_bitmap_t clear_mask, set_mask, out_mask;

	if (portnum < 0 || portnum >= max_qports)
		return -EBADF;	/* error: port outer range  */

	VM_QPORT_CLEAR_SET(&clear_mask);
	VM_QPORT_SET_BIT(&clear_mask, &qport_handles[portnum]);
	VM_QPORT_CLEAR_SET(&set_mask);

	VMAPI_CALL_ENTER();
	err = vm_qport_mask(&clear_mask, &set_mask, &out_mask);
	VMAPI_CALL_EXIT();

	return p4_error_to_linux(err);
}
EXPORT_SYMBOL(vmapi_qport_poll_clear);

/*
 * helper function: blocking call, return ready qports
 */
int vmapi_qport_select(int *states)
{
	int i;
	vm_e_t err;
	vm_qport_bitmap_t clear_mask, set_mask, out_mask;

	VM_QPORT_CLEAR_SET(&clear_mask);
	VM_QPORT_CLEAR_SET(&set_mask);
	VMAPI_CALL_ENTER();
	err =  vm_qport_select(&clear_mask, &set_mask, &out_mask);
	VMAPI_CALL_EXIT();
	if (err)
		return p4_error_to_linux(err);	/* some error */

	for (i = 0; i < max_qports; i++)
		states[i] = VM_QPORT_GET_BIT(&out_mask, &qport_handles[i]);

	return 0;
}
EXPORT_SYMBOL(vmapi_qport_select);

/* --------------------------------------------------------------------- */

/*
 * helper function: reboot linux or other partition
 */
int vmapi_partition_reboot(int part)
{
	vm_e_t err;

	VMAPI_CALL_ENTER();
	err = vm_part_set_mode(part, VM_PART_MODE_COLD_START, 0, 1, 0);
	VMAPI_CALL_EXIT();

	return p4_error_to_linux(err);
}
EXPORT_SYMBOL(vmapi_partition_reboot);

/*
 * helper function: shutdown linux or other partition
 */
int vmapi_partition_shutdown(int part)
{
	vm_e_t err;

	VMAPI_CALL_ENTER();
	err = vm_part_set_mode(part, VM_PART_MODE_IDLE, 0, 1, 0);
	VMAPI_CALL_EXIT();

	return p4_error_to_linux(err);
}
EXPORT_SYMBOL(vmapi_partition_shutdown);

/*
 * helper function: raise partition error or
 *   shutdown our partition if SSW doesn't support this feature
 */
void vmapi_partition_error(int isiniterror, const char *msg)
{
#if PIKEOS_VM_VERSION >= 34
	P4_hm_error_t error;
#else
	vm_hm_error_identifier_t error_identifier;
#endif

	/* no strlen in this context! */
	unsigned len = 0;
	const char *c = msg;

	while (*c++)
		len++;

	if (isiniterror) {
#if PIKEOS_VM_VERSION >= 34
		error.domain = P4_HM_DOMAIN_PART_INIT;
#else
		error_identifier = VM_HM_EI_PART_INIT_ERROR;
#endif
	} else {
#if PIKEOS_VM_VERSION >= 34
		error.domain = P4_HM_DOMAIN_PART;
#else
		error_identifier = VM_HM_EI_APP_ERROR;
#endif
	}

#if PIKEOS_VM_VERSION >= 34
	error.type = P4_HM_TYPE_TRAP;
	error.id = P4_TRAP_SEG;
	error.msg = msg;
	error.size = len;
#endif

	/* raise HM error */
	VMAPI_CALL_ENTER();
#if PIKEOS_VM_VERSION >= 34
	p4_hm_inject(p4_my_uid(), &error, 0);
#else
	vm_hm_raise_error(error_identifier, VM_HM_ERR_MSG_T_CUSTOM, msg, len);
#endif
	VMAPI_CALL_EXIT();

	/* if HM not supported: partition shutdown */
	vmapi_partition_shutdown(-1);
	/* may return */
	for (;;)
		;
}
EXPORT_SYMBOL(vmapi_partition_error);

/*
 * helper function: reboot the whole system
 */
int vmapi_target_reboot(void)
{
	vm_e_t err;

	VMAPI_CALL_ENTER();
	err = vm_target_reset(VM_TARGET_REBOOT);
	VMAPI_CALL_EXIT();

	return p4_error_to_linux(err);
}
EXPORT_SYMBOL(vmapi_target_reboot);

/*
 * helper function: shutdown the whole system
 */
int vmapi_target_shutdown(void)
{
	vm_e_t err;

	VMAPI_CALL_ENTER();
	err = vm_target_reset(VM_TARGET_HALT);
	VMAPI_CALL_EXIT();

	return p4_error_to_linux(err);
}
EXPORT_SYMBOL(vmapi_target_shutdown);

/*
 * helper function: shutdown the whole system
 */
int vmapi_target_power_off(void)
{
	vm_e_t err;

	VMAPI_CALL_ENTER();
	err = vm_target_reset(VM_TARGET_POWER_OFF);
	VMAPI_CALL_EXIT();

	return p4_error_to_linux(err);
}
EXPORT_SYMBOL(vmapi_target_power_off);

/*
 * helper function: change scheduling scheme
 */
int vmapi_set_time_sched_scheme(const char *scheme)
{
	vm_e_t err;

	VMAPI_CALL_ENTER();
#if PIKEOS_VM_VERSION >= 34
	{
		vm_tsched_id_t id;
		err = vm_tsched_lookup(scheme, &id);
		if (err == P4_E_OK)
			err = vm_tsched_change(id, 0);
	}
#else
	/* < PikeOS 4.0 */
	err = vm_set_time_sched_scheme(scheme);
#endif
	VMAPI_CALL_EXIT();

	return p4_error_to_linux(err);
}
EXPORT_SYMBOL(vmapi_set_time_sched_scheme);

/* --------------------------------------------------------------------- */

/*
 * get commandline of our partition
 */
void vmapi_get_cmdline(char *buf, unsigned int maxsize)
{
	vm_e_t err;

	VMAPI_CALL_ENTER();
	err = vm_cmd_line(buf, maxsize);
	VMAPI_CALL_EXIT();
	/* a truncated command line string is considered to be OK. */
	if (err != VM_E_OK && err != VM_E_TRUNC)
		die_early("vmapi_get_cmdline(): couldn't get command line");
}

/*
 * helper function: print formated strings on PikeOS
 *                  system console and use linux format
 *                  function
 */
void vmapi_cprintf(const char *fmt, ...)
{
	char buffer[256];
	va_list ap;

	va_start(ap, fmt);
	/*
	 * Use already implemented and better supported linux format function.
	 * This also reduces the kernelsize about 8KB when vm_cprintf is no
	 * longer used at other locations.
	 */
	vsnprintf(buffer, sizeof(buffer), fmt, ap);
	va_end(ap);

	VMAPI_CALL_ENTER();
	vm_cputs(buffer);
	VMAPI_CALL_EXIT();
}

/* --------------------------------------------------------------------- */

static int vmapi_xlate_file_stat(
		vm_file_stat_t *status, struct vmapi_file_stat *vmapi_status)
{
	umode_t mode;

	switch (status->type) {
	case VM_FILE_T_DIR:
		mode = S_IFDIR | 0755;
		break;
	case VM_FILE_T_REGULAR:
		mode = S_IFREG | 0644;
		break;
	default:
		return -ENOENT;
	}

	vmapi_status->mode = mode;
	vmapi_status->size = status->size;
	vmapi_status->blksize = status->blksize;
#if PIKEOS_VM_VERSION >= 34
	vmapi_status->blkunit = status->blkunit;
	vmapi_status->inode = status->inode;
#else
	vmapi_status->blkunit = 0;
	vmapi_status->inode =  0;
#endif

	return 0;
}

int vmapi_file_stat(const char *name, struct vmapi_file_stat *vmapi_status)
{
	vm_e_t err;
	vm_file_stat_t status;

	VMAPI_CALL_ENTER();
	err = vm_stat(name, &status);
#ifdef VMAPI_TRACE
	if (strstr(name, "pci:") != name) {
		TLOG("(%s): ret=%#x\n", name, err);
	}
#endif
	VMAPI_CALL_EXIT();
	switch (err) {
	case VM_E_OK:
		return vmapi_xlate_file_stat(&status, vmapi_status);
#if PIKEOS_VM_VERSION >= 34
	case P4_E_INVAL:
		return -ENAMETOOLONG;
#endif
	default:
		return p4_error_to_linux(err);
	}
}
EXPORT_SYMBOL(vmapi_file_stat);

/*
 * Open a VM file
 *
 * NOTE: first try to open as "rw", then as "r"
 *
 * FIXME: azu@sysgo.de: ignore EXEC for now
 */
int vmapi_file_open4(const char *name, unsigned int oflags, vmapi_off_t *size, int *is_write)
{
	vm_e_t err;
	int i;
	vm_file_stat_t status;
	int ret;
	P4_uint32_t flags;

	/* find a free filedescriptor */
	uk_mutex_lock(&file_lock, kthread_uid());
	for (i = 0; i < VMAPI_MAX_FILES; i++) {
		if (file_states[i] == FS_UNUSED) {
			file_states[i] = FS_ALLOC;
			break;
		}
	}
	uk_mutex_unlock(&file_lock, kthread_uid());

	if (i == VMAPI_MAX_FILES) {
		/* no free filedescriptor ... */
		ret = -EMFILE;
		goto out;
	}

	if (oflags != 0) {
#if PIKEOS_VM_VERSION >= 34
		flags = 0;
#else
		/* < PikeOS 4.0 */
		flags = VM_O_STAT;
#endif

		if (oflags & VMAPI_O_R)
			flags |= VM_O_RD;
		if (oflags & VMAPI_O_W)
			flags |= VM_O_WR;
		if (oflags & VMAPI_O_MAP)
			flags |= VM_O_MAP;
#if PIKEOS_VM_VERSION >= 34
		if (oflags & VMAPI_O_CREAT)
			flags |= VM_O_CREAT;
		if (oflags & VMAPI_O_EXCL)
			flags |= VM_O_EXCL;
#endif
	} else {
		VMAPI_CALL_ENTER();
		err = vm_stat(name, &status);
		VMAPI_CALL_EXIT();

		switch (err) {
		case VM_E_OK:
			/* automatic mode - use minimum of driver & vmit */
			flags = status.fflags & status.vflags;
			break;
		case VM_E_NO_SUPP:
			/* maybe driver has no stat implemented (yet)? */
			/* override flags in this case */
			flags = 0;
			if (oflags & VMAPI_O_R)
				flags |= VM_O_RD;
			if (oflags & VMAPI_O_W)
				flags |= VM_O_WR;
			if (oflags & VMAPI_O_MAP)
				flags |= VM_O_MAP;
			break;
#if PIKEOS_VM_VERSION >= 34
		case P4_E_INVAL:
			ret = -ENAMETOOLONG;
			goto out_free_fh;
		case P4_E_NOCONTAINER:
			ret = -ENOTDIR;
			goto out_free_fh;
#endif
		default:
			/* file does not exists or something */
			ret = p4_error_to_linux(err);
			goto out_free_fh;
		}
	}

	VMAPI_CALL_ENTER();
	err = vm_open(name, flags, &file_handles[i]);
#ifdef VMAPI_TRACE
	if (strstr(name, "pci:") != name) {
		TLOG("(%s, %#x): ret=%#x -> %d\n", name, flags, err, i);
	}
#endif
	VMAPI_CALL_EXIT();
	switch (err) {
	case VM_E_OK:
		break;
#if PIKEOS_VM_VERSION >= 34
	case P4_E_INVAL:
		ret = -ENAMETOOLONG;
		goto out_free_fh;
	case P4_E_NOCONTAINER:
		ret = -ENOTDIR;
		goto out_free_fh;
	case P4_E_MISMATCH:
		ret = -EISDIR;
		goto out_free_fh;
#endif
	default:
		/* open failed */
		ret = p4_error_to_linux(err);
		goto out_free_fh;
	}

	file_states[i] = FS_OPEN;
	if (is_write)
		*is_write = ((flags & VM_O_WR) == VM_O_WR);
	ret = i;
	if (size)
		*size = vmapi_file_size(i);

	goto out;

out_free_fh:
	file_states[i] = FS_UNUSED;

out:
	return ret;
}
EXPORT_SYMBOL(vmapi_file_open4);

/* compat wrapper */
int vmapi_file_open(const char *name, unsigned int oflags, vmapi_off_t *size)
{
	return vmapi_file_open4(name, oflags, size, 0);
}
EXPORT_SYMBOL(vmapi_file_open);

/*
 * Return size of a file
 */
vmapi_off_t vmapi_file_size(int handle)
{
	vm_file_stat_t status;
	vm_e_t err;

#ifdef VMAPI_TRACE
	TLOG("(%d)\n", handle);
#endif
	if (handle < 0 || handle >= VMAPI_MAX_FILES)
		return -EBADF;

	if (file_states[handle] != FS_OPEN)
		return -EBADFD;

	VMAPI_CALL_ENTER();
	err = vm_fstat(&file_handles[handle], &status);
	VMAPI_CALL_EXIT();
	if (err != 0) {
		/* vm_fstat() failed with an error ... need to fake a file size
		 * in case the file provider doesn't implemement fstat()
		 */
		return 0;
	}

	return status.size;
}
EXPORT_SYMBOL(vmapi_file_size);

int vmapi_file_fstat(int handle, struct vmapi_file_stat *vmapi_status)
{
	vm_file_stat_t status;
	vm_e_t err;

#ifdef VMAPI_TRACE
	TLOG("(%d)\n", handle);
#endif
	if (handle < 0 || handle >= VMAPI_MAX_FILES)
		return -EBADF;

	if (file_states[handle] != FS_OPEN)
		return -EBADFD;

	VMAPI_CALL_ENTER();
	err = vm_fstat(&file_handles[handle], &status);
	VMAPI_CALL_EXIT();

	if (err)
		return p4_error_to_linux(err);

	return vmapi_xlate_file_stat(&status, vmapi_status);
}
EXPORT_SYMBOL(vmapi_file_fstat);

/*
 * Close a VM file
 */
int vmapi_file_close(int handle)
{
	vm_e_t err;

#ifdef VMAPI_TRACE
	TLOG("(%d)\n", handle);
#endif
	if (handle < 0 || handle >= VMAPI_MAX_FILES)
		return -EBADF;

	uk_mutex_lock(&file_lock, kthread_uid());
	if (file_states[handle] != FS_OPEN) {
		uk_mutex_unlock(&file_lock, kthread_uid());
		return -EBADFD;
	}
	file_states[handle] = FS_ALLOC;         /* mark it invalid */
	uk_mutex_unlock(&file_lock, kthread_uid());

	VMAPI_CALL_ENTER();
	err = vm_close(&file_handles[handle]);
	VMAPI_CALL_EXIT();
	/* error code ignored here! */

	file_states[handle] = FS_UNUSED;		/* and now re-use-able */

	return 0;
}
EXPORT_SYMBOL(vmapi_file_close);

/*
 * Read from file
 * return <0 for errors, otherwise bytes read
 */
vmapi_off_t vmapi_file_read(int handle, void *buf, vmapi_off_t size)
{
	vm_e_t err;
	P4_size_t read;

#ifdef VMAPI_TRACE
	if (handle != 0) /* MUXA */
		TLOG("(%d)\n", handle);
#endif
	if (handle < 0 || handle >= VMAPI_MAX_FILES || size < 0)
		return -EBADF;

	if (file_states[handle] != FS_OPEN)
		return -EBADFD;

	VMAPI_CALL_ENTER();
	err = vm_read(&file_handles[handle], buf, size, &read);
	VMAPI_CALL_EXIT();
	if (err)
		return p4_error_to_linux(err);

	return read;
}
EXPORT_SYMBOL(vmapi_file_read);

/*
 * Read from file
 * return <0 for errors, otherwise bytes read
 */
vmapi_off_t vmapi_file_read_at(int handle, void *buf, vmapi_off_t size, vmapi_off_t offset)
{
	vm_e_t err;
	P4_size_t read;

#ifdef VMAPI_TRACE
	TLOG("(%d)\n", handle);
#endif
	if (handle < 0 || handle >= VMAPI_MAX_FILES || size < 0 || offset < 0)
		return -EBADF;

	if (file_states[handle] != FS_OPEN)
		return -EBADFD;

	VMAPI_CALL_ENTER();
	err = vm_read_at(&file_handles[handle], buf, size, offset, &read);
	VMAPI_CALL_EXIT();
	if (err)
		return p4_error_to_linux(err);

	return read;
}
EXPORT_SYMBOL(vmapi_file_read_at);

/*
 * Write to file
 * return <0 for errors, otherwise bytes written
 */
vmapi_off_t vmapi_file_write(int handle, const void *buf, vmapi_off_t size)
{
	vm_e_t err;
	P4_size_t written;

#ifdef VMAPI_TRACE
	if (handle != 0) /* MUXA */
		TLOG("(%d)\n", handle);
#endif
	if (handle < 0 || handle >= VMAPI_MAX_FILES || size < 0)
		return -EBADF;

	if (file_states[handle] != FS_OPEN)
		return -EBADFD;

	VMAPI_CALL_ENTER();
	err = vm_write(&file_handles[handle], buf, size, &written);
	VMAPI_CALL_EXIT();

	if (err)
		return p4_error_to_linux(err);

	return written;
}
EXPORT_SYMBOL(vmapi_file_write);

/*
 * Write to file
 * return <0 for errors, otherwise bytes written
 */
vmapi_off_t vmapi_file_write_at(int handle, const void *buf, vmapi_off_t size, vmapi_off_t offset)
{
	vm_e_t err;
	P4_size_t written;

#ifdef VMAPI_TRACE
	TLOG("(%d)\n", handle);
#endif
	if (handle < 0 || handle >= VMAPI_MAX_FILES || size < 0 || offset < 0)
		return -EBADF;

	if (file_states[handle] != FS_OPEN)
		return -EBADFD;

	VMAPI_CALL_ENTER();
	err = vm_write_at(&file_handles[handle], buf, size, offset, &written);
	VMAPI_CALL_EXIT();

	if (err)
		return p4_error_to_linux(err);

	return written;
}
EXPORT_SYMBOL(vmapi_file_write_at);

/*
 * lseek in file
 * return <0 for errors, otherwise current position
 */
vmapi_off_t vmapi_file_lseek(int handle, vmapi_off_t offset, int whence)
{
	vm_e_t err;
	vm_off_t new_offset;
	vm_origin_t origin;

#ifdef VMAPI_TRACE
	TLOG("(%d)\n", handle);
#endif
	if (handle < 0 || handle >= VMAPI_MAX_FILES || offset < 0)
		return -EBADF;

	if (file_states[handle] != FS_OPEN)
		return -EBADFD;

	if (whence == 0)
		origin = VM_SEEK_SET;
	else if (whence == 1)
		origin = VM_SEEK_CUR;
	else
		origin = VM_SEEK_END;

	VMAPI_CALL_ENTER();
	err = vm_lseek(&file_handles[handle], offset, origin, &new_offset);
	VMAPI_CALL_EXIT();
	if (err)
		return p4_error_to_linux(err);

	return new_offset;
}
EXPORT_SYMBOL(vmapi_file_lseek);

#if PIKEOS_VM_VERSION >= 34

/*
 * Sync file
 * return <0 for errors, otherwise 0
 */
int vmapi_file_fsync(int handle)
{
	vm_e_t err;

#ifdef VMAPI_TRACE
	TLOG("(%d)\n", handle);
#endif
	if (handle < 0 || handle >= VMAPI_MAX_FILES)
		return -EBADF;

	if (file_states[handle] != FS_OPEN)
		return -EBADFD;

	VMAPI_CALL_ENTER();
	err = vm_fsync(&file_handles[handle]);
	VMAPI_CALL_EXIT();
	if (err)
		return p4_error_to_linux(err);

	return 0;
}
EXPORT_SYMBOL(vmapi_file_fsync);

/*
 * Truncate file
 * return <0 for errors, otherwise new size
 */
vmapi_off_t vmapi_file_ftruncate(int handle, vmapi_off_t length)
{
	vm_e_t err;

#ifdef VMAPI_TRACE
	TLOG("(%d)\n", handle);
#endif
	if (handle < 0 || handle >= VMAPI_MAX_FILES)
		return -EBADF;

	if (file_states[handle] != FS_OPEN)
		return -EBADFD;

	VMAPI_CALL_ENTER();
	err = vm_ftruncate(&file_handles[handle], length);
	VMAPI_CALL_EXIT();
	if (err)
		return p4_error_to_linux(err);

	return length;
}
EXPORT_SYMBOL(vmapi_file_ftruncate);

#endif

/*
 * Map part of a file to memory
 * return <0 for errors, otherwise zero
 */
int vmapi_file_fmap(int handle, vmapi_off_t foffset, vmapi_off_t size, unsigned long destaddr, unsigned int vmapi_flags)
{
	vm_e_t err;
	vm_memory_access_mode_t prot;

#ifdef VMAPI_TRACE
	TLOG("(%d)\n", handle);
#endif
	if (handle < 0 || handle >= VMAPI_MAX_FILES || size < 0)
		return -EBADF;

	if (file_states[handle] != FS_OPEN)
		return -EBADFD;

	/* Convert VMAPI flags to the P4 VM memory access flags */
	if (vmapi_flags & VMAPI_O_R) {
		if (vmapi_flags & VMAPI_O_W)
			prot = VM_MEM_ACCESS_RDWR;
		else
			prot = VM_MEM_ACCESS_READ;
	} else if (vmapi_flags & VMAPI_O_W) {
		prot = VM_MEM_ACCESS_WRITE;
	} else {
		/* Default value */
		prot = VM_MEM_ACCESS_READ;
	}

	VMAPI_CALL_ENTER();
	err = vm_map(&file_handles[handle], foffset, size, prot, 0, destaddr);
	VMAPI_CALL_EXIT();
	if (err)
		return p4_error_to_linux(err);

	return 0;
}
EXPORT_SYMBOL(vmapi_file_fmap);

/*
 * Nasty IOCTLs ...
 * return <0 for errors, otherwise 0
 *
 * NOTE: untested etc, cmd must be shifted etc
 * If in == -1 and out == -1, cmd is untouched
 */
int vmapi_file_ioctl(int handle, unsigned int cmd, void *data, int in, int out)
{
	vm_e_t err;
	P4_uint32_t newcmd;

#ifdef VMAPI_TRACE
	TLOG("(%d)\n", handle);
#endif
	if (handle < 0 || handle >= VMAPI_MAX_FILES)
		return -EBADF;

	if (file_states[handle] != FS_OPEN)
		return -EBADFD;

	if (in == -1 && out == -1) {
		/* unmodified IOCTL */
		VMAPI_CALL_ENTER();
		err = vm_ioctl(&file_handles[handle], cmd, data);
		VMAPI_CALL_EXIT();
	} else {
		newcmd = VM_IOC_CMD(cmd);
		if (in == -1)
			newcmd |= VM_IOC_VOID_IN;
		else
			newcmd |= VM_IOC_PARAM_IN(in);

		if (out == -1)
			newcmd |= VM_IOC_VOID_OUT;
		else
			newcmd |= VM_IOC_PARAM_OUT(out);

		/* build IOCTL command word */
		VMAPI_CALL_ENTER();
		err = vm_ioctl(&file_handles[handle], newcmd, data);
		VMAPI_CALL_EXIT();
	}
	if (err)
		return p4_error_to_linux(err);

	return 0;
}
EXPORT_SYMBOL(vmapi_file_ioctl);

#if PIKEOS_VM_VERSION >= 34

int vmapi_unlink(const char *name)
{
	vm_e_t err;

#ifdef VMAPI_TRACE
	TLOG("(%s)\n", name);
#endif
	VMAPI_CALL_ENTER();
	err = vm_unlink(name);
	VMAPI_CALL_EXIT();

	switch (err) {
	case P4_E_OK:
		return 0;
	case P4_E_INVAL:
		return -ENAMETOOLONG;
	case P4_E_NOCONTAINER:
		return -ENOTDIR;
	case P4_E_MISMATCH:
		return -EISDIR;
	default:
		return p4_error_to_linux(err);
	}
}
EXPORT_SYMBOL(vmapi_unlink);

int vmapi_rename(const char *old_path, const char *new_path)
{
	vm_e_t err;

#ifdef VMAPI_TRACE
	TLOG("(%s, %s)\n", old_path, new_path);
#endif
	VMAPI_CALL_ENTER();
	err = vm_rename(old_path, new_path);
	VMAPI_CALL_EXIT();
	switch (err) {
	case P4_E_OK:
		return 0;
	case P4_E_INVAL:
		return -ENAMETOOLONG;
	case P4_E_STATE:
		return -ENOTEMPTY;
	case P4_E_NOCONTAINER:
		return -ENOTDIR;
	case P4_E_MISMATCH:
		return -EISDIR;
	default:
		return p4_error_to_linux(err);
	}
}
EXPORT_SYMBOL(vmapi_rename);

int vmapi_statvfs(const char *path, struct vmapi_statvfs *vmapi_statvfs)
{
	vm_e_t err;
	vm_statvfs_t statvfs;

#ifdef VMAPI_TRACE
	TLOG("(%s)\n", path);
#endif
	VMAPI_CALL_ENTER();
	err = vm_statvfs(path, &statvfs);
	VMAPI_CALL_EXIT();
	switch (err) {
	case P4_E_OK:
		vmapi_statvfs->f_bsize = statvfs.f_bsize;
		vmapi_statvfs->f_blocks = statvfs.f_blocks;
		vmapi_statvfs->f_bfree = statvfs.f_bfree;
		vmapi_statvfs->f_files = statvfs.f_files;
		vmapi_statvfs->f_ffree = statvfs.f_ffree;
		vmapi_statvfs->f_namemax = statvfs.f_namemax;

		/* TODO
		 * f_fsid
		 * f_flag
		 * max_atomic_size
		 * vol_type
		 */
		return 0;
	case P4_E_INVAL:
		return -ENAMETOOLONG;
	case P4_E_NOCONTAINER:
		return -ENOTDIR;
	default:
		return p4_error_to_linux(err);
	}
}
EXPORT_SYMBOL(vmapi_statvfs);

int vmapi_dir_create(const char *path)
{
	vm_e_t err;

#ifdef VMAPI_TRACE
	TLOG("(%s)\n", path);
#endif
	VMAPI_CALL_ENTER();
	err = vm_dir_create(path);
	VMAPI_CALL_EXIT();
	switch (err) {
	case P4_E_OK:
		return 0;
	case P4_E_INVAL:
		return -ENAMETOOLONG;
	case P4_E_NOCONTAINER:
		return -ENOTDIR;
	default:
		return p4_error_to_linux(err);
	}
}
EXPORT_SYMBOL(vmapi_dir_create);

int vmapi_dir_remove(const char *path)
{
	vm_e_t err;

#ifdef VMAPI_TRACE
	TLOG("(%s)\n", path);
#endif
	VMAPI_CALL_ENTER();
	err = vm_dir_remove(path);
	VMAPI_CALL_EXIT();
	switch (err) {
	case P4_E_OK:
		return 0;
	case P4_E_INVAL:
		return -ENAMETOOLONG;
	case P4_E_STATE:
		return -ENOTEMPTY;
	case P4_E_NOCONTAINER:
		return -ENOTDIR;
	default:
		return p4_error_to_linux(err);
	}
}
EXPORT_SYMBOL(vmapi_dir_remove);

int vmapi_dir_open(const char *name)
{
	vm_e_t err;
	int i;
	int ret;

#ifdef VMAPI_TRACE
	TLOG("(%s)\n", name);
#endif
	/* find a free dirdescriptor */
	uk_mutex_lock(&dir_lock, kthread_uid());
	for (i = 0; i < VMAPI_MAX_DIRS; i++) {
		if (dir_states[i] == FS_UNUSED) {
			dir_states[i] = FS_ALLOC;
			break;
		}
	}
	uk_mutex_unlock(&dir_lock, kthread_uid());

	if (i == VMAPI_MAX_DIRS) {
		/* no free dirdescriptor ... */
		ret = -EMFILE;
		goto out;
	}

	VMAPI_CALL_ENTER();
	err = vm_dir_open(name, &dir_handles[i]);
	VMAPI_CALL_EXIT();

	if (err) {
		/* on error, free entry */
		dir_states[i] = FS_UNUSED;
	}

	switch (err) {
	case P4_E_OK:
		dir_states[i] = FS_OPEN;
		ret = i;
		break;
	case P4_E_INVAL:
		ret = -ENAMETOOLONG;
		break;
	case P4_E_NOCONTAINER:
		ret = -ENOTDIR;
		break;
	default:
		ret = p4_error_to_linux(err);
		break;
	}

out:
#ifdef VMAPI_TRACE
	TLOG("(%s) = %d\n", name, ret);
#endif
	return ret;
}
EXPORT_SYMBOL(vmapi_dir_open);

int vmapi_dir_read_at(int handle, vmapi_off_t *pos, struct vmapi_dirent *dirent)
{
	vm_e_t err;
	vm_dirent_t de;
	vm_off_t vm_pos;
	int ret;

#ifdef VMAPI_TRACE
	TLOG("(%d)\n", handle);
#endif
	if (handle < 0 || handle >= VMAPI_MAX_DIRS)
		return -EBADF;

	if (dir_states[handle] != FS_OPEN)
		return -EBADFD;

	VMAPI_CALL_ENTER();
	vm_pos = *pos;
	err = vm_dir_read_at(&dir_handles[handle], &vm_pos, &de);
	*pos = vm_pos;
	VMAPI_CALL_EXIT();

	if (err) {
		/* dir read failed */
		ret = p4_error_to_linux(err);
	} else {
#if (VMAPI_EXT_NAME_LEN != VM_EXT_NAME_LEN)
#error VMAPI name length does not match
#endif

		switch (de.type) {
		case VM_FILE_T_DIR:
			dirent->type = VMAPI_FILE_T_DIR;
			break;
		case VM_FILE_T_REGULAR:
			dirent->type = VMAPI_FILE_T_REGULAR;
			break;
		case VM_FILE_T_PROPERTY:
			dirent->type = VMAPI_FILE_T_PROPERTY;
			break;
		case VM_FILE_T_DEVICE:
			dirent->type = VMAPI_FILE_T_DEVICE;
			break;
		default:
			dirent->type = VMAPI_FILE_T_UNKNOWN;
			break;
		}
		strncpy(dirent->name, de.name, sizeof(dirent->name));
		ret = 0;
	}

	return ret;
}

int vmapi_dir_close(int handle)
{
	vm_e_t err;

#ifdef VMAPI_TRACE
	TLOG("(%d)\n", handle);
#endif
	if (handle < 0 || handle >= VMAPI_MAX_DIRS)
		return -EBADF;

	uk_mutex_lock(&dir_lock, kthread_uid());
	if (dir_states[handle] != FS_OPEN) {
		uk_mutex_unlock(&dir_lock, kthread_uid());
		return -EBADFD;
	}
	dir_states[handle] = FS_ALLOC;         /* mark it invalid */
	uk_mutex_unlock(&dir_lock, kthread_uid());

	VMAPI_CALL_ENTER();
	err = vm_dir_close(&dir_handles[handle]);
	VMAPI_CALL_EXIT();
	/* error code ignored here! */

	dir_states[handle] = FS_UNUSED;		/* and now re-use-able */

	return 0;
}
EXPORT_SYMBOL(vmapi_dir_close);

int vmapi_dir_sync(int handle)
{
	vm_e_t err;

#ifdef VMAPI_TRACE
	TLOG("(%d)\n", handle);
#endif
	if (handle < 0 || handle >= VMAPI_MAX_DIRS)
		return -EBADF;

	VMAPI_CALL_ENTER();
	err = vm_dir_sync(&dir_handles[handle]);
	VMAPI_CALL_EXIT();
	/* error code ignored here! */

	return 0;
}
EXPORT_SYMBOL(vmapi_dir_sync);

int vmapi_mount(const char *prefix, unsigned int vmapi_mflags)
{
	vm_e_t err;
	int ret;
	unsigned int mflags = 0;

#ifdef VMAPI_TRACE
	TLOG("(%s)\n", prefix);
#endif
	if (vmapi_mflags & VMAPI_O_R)
		mflags |= VM_O_RD;
	if (vmapi_mflags  & VMAPI_O_W)
		mflags |= VM_O_WR;
	if (vmapi_mflags  & VMAPI_O_CREAT)
		mflags |= VM_O_CREAT;
	if (vmapi_mflags  & VMAPI_O_EXCL)
		mflags |= VM_O_EXCL;

	VMAPI_CALL_ENTER();
	err = vm_mount(prefix, mflags);
	VMAPI_CALL_EXIT();

	switch (err) {
	case P4_E_INVAL:
		ret = -ENAMETOOLONG;
		break;
	case P4_E_OK:
	default:
		ret = p4_error_to_linux(err);
		break;
	}

	return ret;
}
EXPORT_SYMBOL(vmapi_mount);

int vmapi_umount(const char *prefix)
{
	vm_e_t err;
	int ret;

#ifdef VMAPI_TRACE
	TLOG("(%s)\n", prefix);
#endif
	VMAPI_CALL_ENTER();
	err = vm_umount(prefix);
	VMAPI_CALL_EXIT();

	switch (err) {
	case P4_E_INVAL:
		ret = -ENAMETOOLONG;
		break;
	case P4_E_OK:
	default:
		ret = p4_error_to_linux(err);
		break;
	}

	return ret;
}
EXPORT_SYMBOL(vmapi_umount);

#endif /* PIKEOS_VM_VERSION >= 34 */

/* --------------------------------------------------------------------- */

typedef struct {
	int bd;
	int bd_mask;
} baud_conv_t;

static baud_conv_t baud_conv_table[] = {
	{     0,      B0},
	{    50,     B50},
	{    75,     B75},
	{   110,    B110},
	{   134,    B134},
	{   150,    B150},
	{   200,    B200},
	{   300,    B300},
	{   600,    B600},
	{  1200,   B1200},
	{  1800,   B1800},
	{  2400,   B2400},
	{  4800,   B4800},
	{  9600,   B9600},
	{ 19200,  B19200},
	{ 38400,  B38400},
	{ 57600,  B57600 | CBAUDEX},
	{115200,  B115200 | CBAUDEX},
	{230400,  B230400 | CBAUDEX},
	{460800,  B460800 | CBAUDEX},
	{500000,  B500000 | CBAUDEX},
	{576000,  B576000 | CBAUDEX},
	{921600,  B921600 | CBAUDEX},
	{1000000, B1000000 | CBAUDEX},
	{1152000, B1152000 | CBAUDEX},
	{1500000, B1500000 | CBAUDEX},
	{2000000, B2000000 | CBAUDEX},
	{2500000, B2500000 | CBAUDEX},
	{3000000, B3000000 | CBAUDEX},
	{3500000, B3500000 | CBAUDEX},
	{4000000, B4000000 | CBAUDEX},
};

#define BAUD_TABLE_SIZE (sizeof(baud_conv_table)/sizeof(baud_conv_table[0]))

/*
 * Translate <struct termios>->c_cflag to P4 serial FP ioctl and use
 * vmapi_file_ioctl(cmd=DRV_SER_IOCTL_SET_COMM) to update serial
 * line parameters.
 *
 * return <0 for errors, otherwise 0
 */
int vmapi_set_termios(int handle, unsigned c_cflag)
{
	int i;
	int bd = c_cflag & CBAUD;

#if PIKEOS_DRV_API_DDK
	drv_ser_comm_cmd_t ioc;

	ioc.baud_rate = 0;
	for (i = 0; i < BAUD_TABLE_SIZE; i++) {

		if (baud_conv_table[i].bd_mask == bd) {
			ioc.baud_rate = baud_conv_table[i].bd;
			break;
		}
	}

	if (i >= BAUD_TABLE_SIZE)
		return -1;

	if (c_cflag & PARENB) {
		if (c_cflag & PARODD)
			ioc.parity = DRV_SER_PARITY_ODD;
		else
			ioc.parity = DRV_SER_PARITY_EVEN;
	} else
		ioc.parity = DRV_SER_PARITY_NONE;

	ioc.flow_cntrl = DRV_SER_FC_NONE;
	switch (c_cflag & CSIZE) {
	case CS5:
		ioc.data_bits = 5;
		break;
	case CS6:
		ioc.data_bits = 6;
		break;
	case CS7:
		ioc.data_bits = 7;
		break;
	case CS8:
	default:
		ioc.data_bits = 8;
		break;
	}
	ioc.start_bits = 1;
	if (c_cflag & CSTOPB)
		ioc.stop_bits = DRV_SER_STOP_BITS_2;
	else
		ioc.stop_bits = DRV_SER_STOP_BITS_1;

	if (c_cflag & CRTSCTS)
		ioc.flow_cntrl = DRV_SER_FC_HW;

	return vmapi_file_ioctl(handle, DRV_SER_IOCTL_SET_COMM, &ioc, -1, -1);
#else
	drv_ser_comm_cfg_t ioc;

	ioc.baud = 0;
	for (i = 0; i < BAUD_TABLE_SIZE; i++) {

		if (baud_conv_table[i].bd_mask == bd) {
			ioc.baud = baud_conv_table[i].bd;
			break;
		}
	}

	if (i >= BAUD_TABLE_SIZE)
		return -1;

	if (c_cflag & PARENB) {
		if (c_cflag & PARODD)
			ioc.parity = DRV_SER_PARITY_ODD;
		else
			ioc.parity = DRV_SER_PARITY_EVEN;
	} else
#if PIKEOS_VM_VERSION >= 34
		ioc.parity = DRV_SER_PARITY_NONE;
#else
		ioc.parity = DRV_SER_PARITY_NO;
#endif

	ioc.flow = DRV_SER_FC_NONE;
	switch (c_cflag & CSIZE) {
	case CS5:
		ioc.data = 5;
		break;
	case CS6:
		ioc.data = 6;
		break;
	case CS7:
		ioc.data = 7;
		break;
	case CS8:
	default:
		ioc.data = 8;
		break;
	}
	ioc.start = 0;
	if (c_cflag & CSTOPB)
		ioc.stop = 2;
	else
		ioc.stop = 1;

	if (c_cflag & CRTSCTS)
		ioc.flow = DRV_SER_FC_HW;

	return vmapi_file_ioctl(handle, DRV_SER_IOCTL_SET_COMM, &ioc, -1, -1);
#endif
}
EXPORT_SYMBOL(vmapi_set_termios);

/*
 * use vmapi_file_ioctl(cmd=DRV_SER_IOCTL_STAT) to get serial
 * line parameters from serial FP, then
 * translate P4 serial FP ioctl to <struct termios>->c_cflag.
 *
 * return <0 for errors, otherwise 0
 */
int vmapi_get_termios(int handle, unsigned *c_cflag)
{
	int err, i;
	unsigned cf;

#if PIKEOS_DRV_API_DDK
	drv_ser_comm_cmd_t  ioc;

	err = vmapi_file_ioctl(handle, DRV_SER_IOCTL_GET_COMM, &ioc, -1, -1);
	if (!err) {

		cf = CREAD | HUPCL | CLOCAL;

		/*
		 * Construct a cflag setting.
		 */
		for (i = 0; i < BAUD_TABLE_SIZE; i++) {
			if (ioc.baud_rate <= baud_conv_table[i].bd) {
				cf |= baud_conv_table[i].bd_mask;
				break;
			}
		}

		switch (ioc.data_bits) {
		case 5:
			cf |= CS5;
			break;
		case 6:
			cf |= CS6;
			break;
		case 7:
			cf |= CS7;
			break;
		case 8:
		default:
			cf |= CS8;
			break;
		}

		if (ioc.parity == DRV_SER_PARITY_ODD)
			cf |= PARODD;
		else if (ioc.parity == DRV_SER_PARITY_EVEN)
			cf |= PARENB;

		if (ioc.stop_bits == DRV_SER_STOP_BITS_2)
			cf |= CSTOPB;

		if (ioc.flow_cntrl == DRV_SER_FC_HW)
			cf |= CRTSCTS;

		*c_cflag = cf;
	}
#else
	drv_ser_ioctl_stat_t stat;
	drv_ser_comm_cfg_t  *ioc;

	err = vmapi_file_ioctl(handle, DRV_SER_IOCTL_STAT, &stat, -1, -1);
	if (!err) {

		ioc = &stat.stat.comm_cfg;
		cf = CREAD | HUPCL | CLOCAL;

		/*
		 * Construct a cflag setting.
		 */
		for (i = 0; i < BAUD_TABLE_SIZE; i++) {
			if (ioc->baud <= baud_conv_table[i].bd) {
				cf |= baud_conv_table[i].bd_mask;
				break;
			}
		}

		switch (ioc->data) {
		case 5:
			cf |= CS5;
			break;
		case 6:
			cf |= CS6;
			break;
		case 7:
			cf |= CS7;
			break;
		case 8:
		default:
			cf |= CS8;
			break;
		}

		if (ioc->parity == DRV_SER_PARITY_ODD)
			cf |= PARODD;
		else if (ioc->parity == DRV_SER_PARITY_EVEN)
			cf |= PARENB;

		if (ioc->stop > 1)
			cf |= CSTOPB;

		if (ioc->flow == DRV_SER_FC_HW)
			cf |= CRTSCTS;

		*c_cflag = cf;
	}
#endif

	return err;
}
EXPORT_SYMBOL(vmapi_get_termios);

/* --------------------------------------------------------------------- */

int vmapi_set_mctrl(int handle, unsigned short mctrl)
{
#if PIKEOS_DRV_API_DDK
	drv_ser_set_signal_cmd_t sv;
	drv_ser_get_attr_cmd_t attr;

	sv.assert_mask = 0;
	sv.deassert_mask = 0;
	if (mctrl & TIOCM_RTS)
		sv.assert_mask |= DRV_SER_SIGNAL_RTS;
	if (mctrl & TIOCM_DTR)
		sv.assert_mask |= DRV_SER_SIGNAL_DTR;
	if (mctrl & TIOCM_OUT1)
		sv.assert_mask |= DRV_SER_SIGNAL_OUT1;
	if (mctrl & TIOCM_OUT2)
		sv.assert_mask |= DRV_SER_SIGNAL_OUT2;
	if (mctrl & TIOCM_LOOP)
		sv.assert_mask |= DRV_SER_SIGNAL_LOOP;

	if (vmapi_file_ioctl(handle, DRV_SER_IOCTL_GET_ATTR, &attr, -1, -1) == 0) {
		/* Set only supported signals, if indicated by the driver */
		sv.assert_mask &= attr.signals_settable;
		sv.deassert_mask &= attr.signals_settable;
	}

	return vmapi_file_ioctl(handle, DRV_SER_IOCTL_SET_SIGNAL, &sv, -1, -1);
#else
	drv_ser_ioctl_set_mctrl_t sv;

	sv.mctrl = 0;
	if (mctrl & TIOCM_RTS)
		sv.mctrl |= DRV_SER_MCR_RTS;
	if (mctrl & TIOCM_DTR)
		sv.mctrl |= DRV_SER_MCR_DTR;
	if (mctrl & TIOCM_OUT1)
		sv.mctrl |= DRV_SER_MCR_OUT1;
	if (mctrl & TIOCM_OUT2)
		sv.mctrl |= DRV_SER_MCR_OUT2;
	if (mctrl & TIOCM_LOOP)
		sv.mctrl |= DRV_SER_MCR_LOOP;

	return vmapi_file_ioctl(handle, DRV_SER_IOCTL_SET_MCTRL, &sv, -1, -1);
#endif
}
EXPORT_SYMBOL(vmapi_set_mctrl);

int vmapi_get_mctrl(int handle, unsigned short *mctrl)
{
	unsigned short tmp = 0;
	int e = 0;

#if PIKEOS_DRV_API_DDK
	drv_ser_get_signal_cmd_t gv;

	e = vmapi_file_ioctl(handle, DRV_SER_IOCTL_GET_SIGNAL, &gv, -1, -1);
	if (!e) {
		if (gv.signal & DRV_SER_SIGNAL_RTS)
			tmp |= TIOCM_RTS;
		if (gv.signal & DRV_SER_SIGNAL_DTR)
			tmp |= TIOCM_DTR;
		if (gv.signal & DRV_SER_SIGNAL_OUT1)
			tmp |= TIOCM_OUT1;
		if (gv.signal & DRV_SER_SIGNAL_OUT2)
			tmp |= TIOCM_OUT2;
		if (gv.signal & DRV_SER_SIGNAL_LOOP)
			tmp |= TIOCM_LOOP;

		*mctrl = tmp;
	}
#else
#ifdef DRV_SER_IOCTL_GET_MCTRL
	drv_ser_ioctl_get_mctrl_t gv;

	e = vmapi_file_ioctl(handle, DRV_SER_IOCTL_GET_MCTRL, &gv, -1, -1);
	if (!e) {
		if (gv.mctrl & DRV_SER_MCR_RTS)
			tmp |= TIOCM_RTS;
		if (gv.mctrl & DRV_SER_MCR_DTR)
			tmp |= TIOCM_DTR;
		if (gv.mctrl & DRV_SER_MCR_OUT1)
			tmp |= TIOCM_OUT1;
		if (gv.mctrl & DRV_SER_MCR_OUT2)
			tmp |= TIOCM_OUT2;
		if (gv.mctrl & DRV_SER_MCR_LOOP)
			tmp |= TIOCM_LOOP;

		*mctrl = tmp;
	}
	return e;
#else
	*mctrl = TIOCM_RTS;
#endif
#endif

	return e;
}
EXPORT_SYMBOL(vmapi_get_mctrl);

int vmapi_get_mstat(int handle, unsigned short *status)
{
	unsigned short tmp = 0;
	int e;

#if PIKEOS_DRV_API_DDK
	drv_ser_get_signal_cmd_t gv;

	e = vmapi_file_ioctl(handle, DRV_SER_IOCTL_GET_SIGNAL, &gv, -1, -1);
	if (!e) {
		if (gv.signal & DRV_SER_SIGNAL_DCD)
			tmp |= TIOCM_CAR;
		if (gv.signal & DRV_SER_SIGNAL_RI)
			tmp |= TIOCM_RNG;
		if (gv.signal & DRV_SER_SIGNAL_DSR)
			tmp |= TIOCM_DSR;
		if (gv.signal & DRV_SER_SIGNAL_CTS)
			tmp |= TIOCM_CTS;

		*status = tmp;
	}
#else
	drv_ser_ioctl_get_mstat_t gv;

	e = vmapi_file_ioctl(handle, DRV_SER_IOCTL_GET_MSTAT, &gv, -1, -1);
	if (!e) {
		if (gv.mstat & DRV_SER_MSR_DCD)
			tmp |= TIOCM_CAR;
		if (gv.mstat & DRV_SER_MSR_RI)
			tmp |= TIOCM_RNG;
		if (gv.mstat & DRV_SER_MSR_DSR)
			tmp |= TIOCM_DSR;
		if (gv.mstat & DRV_SER_MSR_CTS)
			tmp |= TIOCM_CTS;

		*status = tmp;
	}
#endif

	return e;
}
EXPORT_SYMBOL(vmapi_get_mstat);

/* --------------------------------------------------------------------- */

int vmapi_get_eth_link_stat(int handle, int *linkup, int *speed)
{
	int e = -EINVAL;

#if defined(DRV_NET_IOCTL_GET_STAT) || defined(DRV_NET_IOCTL_GET_LINK_STAT)
#if PIKEOS_DRV_API_DDK
	drv_net_get_link_stat_cmd_t stat;
	e = vmapi_file_ioctl(handle, DRV_NET_IOCTL_GET_LINK_STAT, &stat, -1, -1);
#else
	drv_net_ioctl_get_stat_t stat;
	e = vmapi_file_ioctl(handle, DRV_NET_IOCTL_GET_STAT, &stat, -1, -1);
#endif

	if (!e) {

		*linkup = 0;
		if (stat.stat == DRV_NET_LINK_UP)
			*linkup = 1;

		if (stat.speed == DRV_NET_LINK_SPEED_100)
			*speed  = SPEED_100;
		else if (stat.speed == DRV_NET_LINK_SPEED_1000)
			*speed  = SPEED_1000;
		else if (stat.speed == DRV_NET_LINK_SPEED_10000)
			*speed  = SPEED_10000;
		else
			*speed  = SPEED_10;
	}
#endif

	return e;
}
EXPORT_SYMBOL(vmapi_get_eth_link_stat);

int vmapi_get_eth_mac(int handle, unsigned char *mac)
{
	int e;

#if PIKEOS_DRV_API_DDK
	drv_net_mac_cmd_t data;

	e = vmapi_file_ioctl(handle, DRV_NET_IOCTL_GET_MAC, &data, -1, -1);
	if (!e)
		memcpy(mac, data.addr, sizeof(data.addr));
#else
	drv_net_ioctl_get_mac_t data;

	e = vmapi_file_ioctl(handle, DRV_NET_IOCTL_GET_MAC, &data, -1, -1);
	if (!e)
		memcpy(mac, data.mac, sizeof(data.mac));
#endif

	return e;
}
EXPORT_SYMBOL(vmapi_get_eth_mac);

/* --------------------------------------------------------------------- */

/*
 * Delay routine, sleeps <s> seconds
 */
void vmapi_delay(unsigned int s)
{
	p4_sleep(P4_SEC(s));
}
EXPORT_SYMBOL(vmapi_delay);

/* --------------------------------------------------------------------- */

/*
 * initialize: count number of sampling ports
 */
void __init vmapi_sport_init(void)
{
	vm_sport_stat_t stat;
	P4_uint32_t flags;
	vm_e_t err;
	int i;

	/*
	 * iterate all sampling ports:
	 * get number, alloc data, get status
	 */
	VMAPI_CALL_ENTER();
	for (i = 0; i < VM_SPORT_MAXPORTS; i++) {
		err = vm_sport_iterate(i, &stat);
		if (err != VM_E_OK)
			break;
	}
	VMAPI_CALL_EXIT();
	max_sports = i;

	if (max_sports == 0)
		return;

	sport_handles = wrapped_malloc(max_sports * sizeof(vm_port_desc_t));
	if (!sport_handles)
		die_early("vmapi_sport_init(): malloc failed");

	for (i = 0; i < max_sports; i++) {
		VMAPI_CALL_ENTER();
		err = vm_sport_iterate(i, &stat);
		VMAPI_CALL_EXIT();

		if (stat.state == VMPORT_CONNECTED) {
			if (stat.direction == VMPORT_SOURCE)
				flags = VMPORT_SOURCE;
			else
				flags = VMPORT_DESTINATION;

			VMAPI_CALL_ENTER();
			err = vm_sport_open(stat.name, flags,
				       &sport_handles[i]);
			VMAPI_CALL_EXIT();
			if (err != VM_E_OK)
				die_early("vmapi_sport_init(): couldn't open port");
		}
	}
}

/*
 * return number of ports in system
 */
int vmapi_sport_get_count(void)
{
	return max_sports;
}
EXPORT_SYMBOL(vmapi_sport_get_count);

/*
 * helper function: return some port status
 *
 * FIXME: azu@sysgo, 2005-08-03: does not handle unconneted ports yet!
 */
int vmapi_sport_stat(
	int portnum,
	int *isoutgoing,
	int *framesize,
	char **portname,
	unsigned long long *refresh,
	unsigned int *validity)
{
	vm_sport_stat_t stat;
	vm_e_t err;
	int i;

	if (portnum < 0 || portnum >= max_sports)
		return -EBADF;	/* error: port outer range  */

	VMAPI_CALL_ENTER();
	err = vm_sport_iterate(portnum, &stat);
	VMAPI_CALL_EXIT();
	if (err)
		return p4_error_to_linux(err);	/* error: getsport failed */

	*isoutgoing = (stat.direction == VMPORT_SOURCE) ? 1 : 0;
	*framesize = stat.max_msg_size;
	*refresh = stat.refresh_period; /* nanoseconds */
	if (validity)
		*validity = stat.last_msg_validity;
	for (i = 0; i < sizeof(tmpname); i++)
		tmpname[i] = ((char *)stat.name)[i];
	*portname = tmpname;

	return 0; /* OK */
}
EXPORT_SYMBOL(vmapi_sport_stat);

/*
 * helper function: return if the sampling port is connected or not
 *
 * - portnum: port ID
 */
int vmapi_sport_check_state(int portnum)
{
	vm_sport_stat_t stat;
	vm_e_t err;

	if (portnum < 0 || portnum >= max_sports)
		return -EBADF;	/* error: port outer range  */

	VMAPI_CALL_ENTER();
	err = vm_sport_iterate(portnum, &stat);
	VMAPI_CALL_EXIT();
	if (err)
		return p4_error_to_linux(err);	/* error: getsport failed */

	return (stat.state == VMPORT_OPENED);
}
EXPORT_SYMBOL(vmapi_sport_check_state);

/*
 * helper function: receive something
 */
int vmapi_sport_receive(int portnum, void *buf, int count, unsigned int *validity)
{
	vm_sport_msg_validity_t v;
	vm_e_t err;
	P4_size_t size;

	if (portnum < 0 || portnum >= max_sports)
		return -EBADF;	/* error: port outer range  */

	VMAPI_CALL_ENTER();
	err = vm_sport_read(&sport_handles[portnum], buf, count, &size, &v);
	VMAPI_CALL_EXIT();

	if (validity)
		*validity = v;

#if PIKEOS_VM_VERSION >= 34
	if (err == P4_E_TIMEOUT)
#else
	if (err == VM_E_EMPTY)
#endif
		return 0;

	if (err)
		return p4_error_to_linux(err);	/* some error */

	return size;
}
EXPORT_SYMBOL(vmapi_sport_receive);

/*
 * helper function: send something
 */
int vmapi_sport_send(int portnum, const void *buf, int count)
{
	vm_e_t err;

	if (portnum < 0 || portnum >= max_sports)
		return -EBADF;	/* error: port outer range  */

	VMAPI_CALL_ENTER();
	err = vm_sport_write(&sport_handles[portnum], buf, count);
	VMAPI_CALL_EXIT();
	if (err)
		return p4_error_to_linux(err);	/* some error */

	return count;
}
EXPORT_SYMBOL(vmapi_sport_send);

/*
 * access the property API
 */
int vmapi_property_read(const char *path, void *buf, unsigned int size)
{
	vm_file_desc_t fd;
	P4_size_t readsize;
	vm_e_t err;

	VMAPI_CALL_ENTER();
	err = vm_open(path, VM_O_RD, &fd);
	if (err == VM_E_OK) {
		err = vm_prop_read(&fd, "", VM_PROP_T_ANY, size, buf,
			       0, &readsize);
		vm_close(&fd);
	}
	VMAPI_CALL_EXIT();

	if (err)
		return p4_error_to_linux(err);

	return readsize;
}

/*
 * get partition name
 */
const char *vmapi_get_partname(void)
{
	if (partition_name[0] == 0) {
		vm_partition_stat_t partinfo;
		vm_e_t err;

		VMAPI_CALL_ENTER();
		err = vm_part_pstat(VM_RESPART_MYSELF, &partinfo);
		VMAPI_CALL_EXIT();
		if (err)
			die_early("vm_part_pstat returned error");

		strcpy(partition_name, partinfo.name);
	}

	return partition_name;
}

/*
 * get process name
 */
const char *vmapi_get_process_name(void)
{
	if (process_name[0] == 0) {
		vm_procinfo_t procinfo;
		vm_e_t err;

		VMAPI_CALL_ENTER();
		err = vm_procinfo(VM_RESPART_MYSELF, VM_PROC_MYSELF, &procinfo);
		VMAPI_CALL_EXIT();
		if (err)
			die_early("vm_procinfo returned error");

		strcpy(process_name, procinfo.appname);
	}

	return process_name;
}

/*
 * grant access to a device
 */
int vmapi_property_device_grant(const char *path, unsigned int *device_id)
{
	vm_file_desc_t fd;
	vm_e_t err;

	VMAPI_CALL_ENTER();
	err = vm_open(path, VM_O_RD | VM_O_MAP, &fd);
	if (err == VM_E_OK) {
		err = vm_prop_dev_grant(&fd, "", (P4_devid_t *)(device_id));
		vm_close(&fd);
	}
	VMAPI_CALL_EXIT();

	return p4_error_to_linux(err);
}

/*
 * get access to all interrupts
 */
int vmapi_property_map_irq(const char *path)
{
	vm_file_desc_t fd;
	vm_e_t err;
	P4_intid_t irq;

	VMAPI_CALL_ENTER();
	err = vm_open(path, VM_O_RD | VM_O_MAP, &fd);
	if (err == VM_E_OK) {
		err = vm_prop_int_grant(&fd, "", &irq);
		vm_close(&fd);
	}
	VMAPI_CALL_EXIT();

	if (err)
		return p4_error_to_linux(err);

	return irq;
}

#ifdef CONFIG_PCI_VIRT
int vmapi_pci_read_config(int handle, unsigned int where,
			  unsigned int size, unsigned int *value)
{
	vm_pci_rd_config32_cmd_t cmd32;
	vm_pci_rd_config16_cmd_t cmd16;
	vm_pci_rd_config8_cmd_t cmd8;
	int err;

	/** size coming from PCI_OP_READ */
	switch (size) {
	case 1:
		cmd8.reg = where;
		err = vmapi_file_ioctl(handle,
			VM_PCI_IOCTL_RD_CONFIG8, &cmd8, -1, -1);
		*value = (unsigned int)cmd8.byte;
		break;
	case 2:
		cmd16.reg = where;
		err = vmapi_file_ioctl(handle,
			VM_PCI_IOCTL_RD_CONFIG16, &cmd16, -1, -1);
		*value = (unsigned int)cmd16.word;
		break;
	case 4:
		cmd32.reg = where;
		err = vmapi_file_ioctl(handle,
			VM_PCI_IOCTL_RD_CONFIG32, &cmd32, -1, -1);
		*value = (unsigned int)cmd32.dword;
		break;
	default:
		return -EINVAL;
	}

	return err;
}

int vmapi_pci_write_config(int handle, unsigned int where,
			   unsigned int size, unsigned int value)
{
	vm_pci_wr_config32_cmd_t cmd32;
	vm_pci_wr_config16_cmd_t cmd16;
	vm_pci_wr_config8_cmd_t cmd8;
	int err;

	/** size coming from PCI_OP_WRITE */
	switch (size) {
	case 1:
		cmd8.reg = where;
		cmd8.byte = (P4_uint8_t)(value & 0xFF);
		err = vmapi_file_ioctl(handle,
			VM_PCI_IOCTL_WR_CONFIG8, &cmd8, -1, -1);
		break;
	case 2:
		cmd16.reg = where;
		cmd16.word = (P4_uint16_t)(value & 0xFFFF);
		err = vmapi_file_ioctl(handle,
			VM_PCI_IOCTL_WR_CONFIG16, &cmd16, -1, -1);
		break;
	case 4:
		cmd32.reg = where;
		cmd32.dword = value;
		err = vmapi_file_ioctl(handle,
			VM_PCI_IOCTL_WR_CONFIG32, &cmd32, -1, -1);
		break;
	default:
		return -EINVAL;
	}

	return err;
}

int vmapi_pci_get_irq(int handle)
{
	vm_pci_irq_cmd_t cmd;
	int err;

	err = vmapi_file_ioctl(handle, VM_PCI_IOCTL_GET_IRQ, &cmd, -1, -1);
	if (err)
		return err;

	return cmd.irq;
}

int vmapi_pci_get_info(int handle, int *ctrl_id, int *bus_id,
		       unsigned int *devfn)
{
	int err;
	P4_pci_config_addr_t addr;
	vm_pci_dev_t dev;
	vm_pci_get_dev_cmd_t cmd;

	cmd.dev = &dev;
	err = vmapi_file_ioctl(handle, VM_PCI_IOCTL_GET_DEV, &cmd, -1, -1);
	if (!err) {
		addr = dev.config_addr;
		*ctrl_id = P4_PCI_CONFIG_DOM(addr);
		*bus_id = P4_PCI_CONFIG_BUS(addr);
		*devfn = PCI_DEVFN(P4_PCI_CONFIG_DEV(addr), P4_PCI_CONFIG_FN(addr));
	} else {
		*ctrl_id = -1;
		*bus_id = -1;
		*devfn = (unsigned int)-1;
	}
	return err;
}

int vmapi_pci_map_resource(int handle, int bar, struct resource *res,
			   unsigned long *virt)
{
	P4_size_t size;
	vm_pci_map_res_cmd_t cmd;
	struct vm_struct *area;

	/** P4 enum type is the same as the Linux one (1:1), no cast needed */
	cmd.id = bar;
	if (res->flags & IORESOURCE_IO) {
		cmd.vaddr = 0;
		cmd.size = 0;
		cmd.access = 0;
	} else {
		size = roundup(resource_size(res), PAGE_SIZE);
		area = get_vm_area(size, VM_IOREMAP);
		cmd.vaddr = (P4_address_t)area->addr;
		vm_protect(cmd.vaddr, size);
		cmd.size = size;
		/*
		* @TODO
		* Set any arch specific flags
		*/
		cmd.access = P4_M_UPDATE | P4_M_READ | P4_M_WRITE | P4_M_C_UPDATE |
			P4_M_C_COHERENCY | P4_M_C_UNCACHED;
	}
	*virt = cmd.vaddr;

	return vmapi_file_ioctl(handle, VM_PCI_IOCTL_MAP_RES, &cmd, -1, -1);
}

int vmapi_pci_request_irq(int handle, struct pci_dev *pdev)
{
	int ret;
	vm_pci_irq_cmd_t cmd;

	ret = vmapi_file_ioctl(handle, VM_PCI_IOCTL_GET_IRQ, &cmd, -1, -1);
	if (!ret) {
		/* PikeOS > 3.4 returns P4_INTID_INVALID for invalid IRQs */
	#ifdef P4_INTID_INVALID
		if (cmd.irq != P4_INTID_INVALID) {
	#else
		if (cmd.irq > 0) {
	#endif
			/** device has a valid IRQ, grant it */
			ret = vmapi_file_ioctl(handle, VM_PCI_IOCTL_INT_GRANT,
						&cmd, -1, -1);
			if (!ret) {
				/* get the IRQ from the PCI manager */
				pdev->irq = cmd.irq;
			}
		}
	}
	return ret;
}

#ifdef VM_PCI_IOCTL_MSI_MSIX_STAT
int vmapi_pci_msi_disable(int handle)
{
	return vmapi_file_ioctl(handle, VM_PCI_IOCTL_DISABLE_MSI, NULL, -1, -1);
}

int vmapi_pci_msix_disable(int handle)
{
	return vmapi_file_ioctl(handle, VM_PCI_IOCTL_DISABLE_MSIX, NULL, -1, -1);
}

int vmapi_pci_msi_msix_stat(int handle, unsigned int *msix_avail, unsigned int *msi_avail)
{
	vm_pci_msi_msix_stat_t stat;
	int ret;

	ret = vmapi_file_ioctl(handle, VM_PCI_IOCTL_MSI_MSIX_STAT, &stat, -1, -1);

	if (msi_avail)
		*msi_avail = stat.msi_avail_irqs;
	if (msix_avail)
		*msix_avail = stat.msix_avail_irqs;
	return ret;
}

int vmapi_pci_msi_enable(int handle, unsigned int *base, int nvec)
{
	vm_pci_enable_msi_cmd_t cmd;
	int ret;

	cmd.count = nvec;

	ret = vmapi_file_ioctl(handle, VM_PCI_IOCTL_ENABLE_MSI_BLK, &cmd, -1, -1);

	if (ret == 0) {
		*base = cmd.irq_base;
#if VM_PCI_API_VERSION < 3
	} else if (ret == -(VM_E_INDICATED + CONV_ERROR_OFFSET)) {
#else
	/* P4_E_LIMIT indicates insufficient resources */
	} else if (ret == -ENOSPC) {
#endif
		/* invoke the stat and get avail */
		unsigned int avail;
		ret = vmapi_pci_msi_msix_stat(handle, NULL, &avail);
		if (ret == 0)
			ret = avail;
	} else {
		ret = -EINVAL;
	}
	return ret;
}

int vmapi_pci_msix_enable(int handle, struct msix_entry *entries, int nvec)
{
	vm_pci_enable_msix_cmd_t cmd;
	int i, ret;

	P4_pci_msix_irq_t *m = kzalloc(sizeof(P4_pci_msix_irq_t) * nvec, GFP_KERNEL);

	if (!m)
		return -ENOMEM;

	/* copy the structure to our request structure */
	for (i = 0; i < nvec; i++) {
		m[i].dirq = entries[i].entry;
		m[i].kirq = 0;
	}

	cmd.nirq = nvec;
	cmd.irqs = m;

	ret = vmapi_file_ioctl(handle,
			VM_PCI_IOCTL_ENABLE_MSIX_BLK, &cmd, -1, -1);

	if (ret == 0) {
		/* copy it to linux structure */
		for (i = 0; i < nvec; i++)
			entries[i].vector = m[i].kirq;
#if VM_PCI_API_VERSION < 3
	} else if (ret == -(VM_E_INDICATED + CONV_ERROR_OFFSET)) {
#else
	/* P4_E_LIMIT indicates insufficient resources */
	} else if (ret == -ENOSPC) {
#endif
		/* invoke the stat and get avail */
		unsigned int avail;
		ret = vmapi_pci_msi_msix_stat(handle, &avail, NULL);

		if (ret == 0)
			ret = avail;
	} else
		ret = -EINVAL;

	kfree(m);

	return ret;
}
#else /* The MSI API is not available */
int vmapi_pci_msi_disable(int handle)
{
	return -EINVAL;
}

int vmapi_pci_msix_disable(int handle)
{
	return -EINVAL;
}

int vmapi_pci_msi_msix_stat(int handle, unsigned int *msix_avail, unsigned int *msi_avail)
{
	return -EINVAL;
}

int vmapi_pci_msi_enable(int handle, unsigned int *base, int nvec)
{
	return -EINVAL;
}

int vmapi_pci_msix_enable(int handle, struct msix_entry *entries, int nvec)
{
	return -EINVAL;
}
#endif /* MSI support */
extern int vmapi_pci_iterate(int handle, char *name, unsigned int size)
{
	int ret;
	vm_pci_iterate_cmd_t cmd;

	cmd.pathname = name;
	cmd.size = (P4_size_t)size;
	ret = vmapi_file_ioctl(handle, VM_PCI_IOCTL_ITERATE, &cmd, -1, -1);
	if (!ret)
		return 0;

	return 1;
}
#endif /* CONFIG_PCI_VIRT */

/* --------------------------------------------------------------------- */

/*
 * returns:
 *     0    if everything went ok
 *    -X    a negative value to indicate a error, this can either be
 *          a errno error code or a PikeOS error
 */
int vmapi_io_sbuf_init(int handle, unsigned int *size)
{
	vm_e_t verr;
	int err;
	P4_size_t size_p;

	/* check if handle is valid */
	if (handle < 0 || handle >= VMAPI_MAX_FILES)
		return -EBADF;

	/* check if file is already open */
	if (file_states[handle] != FS_OPEN)
		return -EBADFD;

	VMAPI_CALL_ENTER();
	verr = vm_io_sbuf_init(&file_handles[handle], &size_p);
	VMAPI_CALL_EXIT();
	if (verr) {
		err = p4_error_to_linux(verr);
	} else {
		*size = size_p;
		err = 0;
	}

	return err;
}
EXPORT_SYMBOL(vmapi_io_sbuf_init);

/*
 * TODO: P4_address_t == unsigned long only on 32bit systems
 */
int vmapi_io_sbuf_map(int handle, unsigned long vaddr)
{
	vm_e_t verr;
	int count;

	/* check if handle is valid */
	if (handle < 0 || handle >= VMAPI_MAX_FILES)
		return -EBADF;

	/* check if file is already open */
	if (file_states[handle] != FS_OPEN)
		return -EBADFD;

	/* TODO: check if we are allowed to map the file */

	/* find usable sbuf descriptor
	 * uses same schema as filedescriptor wrappers
	 */
	uk_mutex_lock(&sbuf_lock, kthread_uid());
	for (count = 0; count < VMAPI_MAX_SHM; count++) {
		if (sbuf_states[count] == FS_UNUSED) {
			sbuf_states[count] = FS_ALLOC;
			break;
		}
	}
	uk_mutex_unlock(&sbuf_lock, kthread_uid());

	if (count == VMAPI_MAX_FILES)
		return -EMFILE;

	/* map SHM into virtual address space */
	VMAPI_CALL_ENTER();
	verr = vm_io_sbuf_map(&file_handles[handle], vaddr, &sbuf_handles[count]);
	VMAPI_CALL_EXIT();
	if (verr) {
		/* couldn't map - free sbuf descriptor for further use */
		sbuf_states[count] = FS_UNUSED;
		return p4_error_to_linux(verr);
	} else {
		sbuf_states[count] = FS_OPEN;
		return count;
	}
}
EXPORT_SYMBOL(vmapi_io_sbuf_map);

/*
 * may block (controled by param noblock)
 */
int vmapi_io_sbuf_write(int shm, const void *buf, int count, int noblock)
{
	vm_io_buf_id_t tx;
	P4_address_t txbuf;
	P4_size_t size;
	drv_sbuf_desc_t *sbuf_bd;

	/* check if handle is valid */
	if (shm < 0 || shm >= VMAPI_MAX_FILES)
		return -EBADF;

	/* check if file is already open */
	if (sbuf_states[shm] != FS_OPEN)
		return -EBADFD;

	sbuf_bd = &sbuf_handles[shm];

	/* get usable buffer */
	VMAPI_CALL_ENTER();
	tx = vm_io_sbuf_tx_alloc(sbuf_bd, noblock);
	VMAPI_CALL_EXIT();
	if (tx == VM_IO_BUF_ID_INVALID)
		return -EWOULDBLOCK;

	VMAPI_CALL_ENTER();
	/* get start of data area */
	txbuf = vm_io_sbuf_tx_buf_addr(sbuf_bd, tx);

	/* get maximal buffer size */
	size = vm_io_sbuf_buf_max_size(sbuf_bd);
	VMAPI_CALL_EXIT();

	/* check if message will fit */
	if (count > size)
		count = size;

	/* copy into sbuf */
	memcpy((void *)txbuf, buf, count);

	/* signal SHM-Buf we are done */
	VMAPI_CALL_ENTER();
	vm_io_sbuf_tx_ready(sbuf_bd, tx, count);
	VMAPI_CALL_EXIT();

	return count;
}
EXPORT_SYMBOL(vmapi_io_sbuf_write);

/*
 * may block (controled by param noblock)
 */
int vmapi_io_sbuf_read(int shm, void *buf, int count, int noblock)
{
	vm_io_buf_id_t rx;
	P4_address_t rxbuf;
	P4_size_t size;
	drv_sbuf_desc_t *sbuf_bd;

	/* check if handle is valid */
	if (shm < 0 || shm >= VMAPI_MAX_FILES)
		return -EBADF;

	/* check if file is already open */
	if (sbuf_states[shm] != FS_OPEN)
		return -EBADFD;

	sbuf_bd = &sbuf_handles[shm];

	VMAPI_CALL_ENTER();
	/* get usable buffer - blocking */
	rx = vm_io_sbuf_rx_get(sbuf_bd, noblock);

	/* get start of data area */
	rxbuf = vm_io_sbuf_rx_buf_addr(sbuf_bd, rx);

	/* get maximal buffer size */
	size = vm_io_sbuf_rx_buf_size(sbuf_bd, rx);
	VMAPI_CALL_EXIT();

	/* check if message will fit */
	if (count > size)
		count = size;

	/* copy from sbuf */
	memcpy(buf, (const void *)rxbuf, count);

	/* signal sbuf we are done */
	VMAPI_CALL_ENTER();
	vm_io_sbuf_rx_free(sbuf_bd, rx);
	VMAPI_CALL_EXIT();

	return count;
}
EXPORT_SYMBOL(vmapi_io_sbuf_read);

/*
 * recycles a shm handle
 * doesn't call a equivalent sbuf function
 */
int vmapi_io_sbuf_close(int shm)
{
	/* check if handle is valid */
	if (shm < 0 || shm >= VMAPI_MAX_FILES)
		return -EBADF;

	/* check if file is already open */
	if (sbuf_states[shm] != FS_OPEN)
		return -EBADFD;

	/* recycle shm handle */
	sbuf_states[shm] = FS_UNUSED;

	return 0;
}
EXPORT_SYMBOL(vmapi_io_sbuf_close);

/*
 * Retrieve physical address and size of the memory object defined in VMIT
 * resource partition of the calling task.
 */
int vmapi_get_vmit_mem_region_by_name(const char *name, unsigned long *phys_addr, unsigned int *phys_size)
{
	vm_mem_desc_t mdesc;
	vm_mem_stat_t mstat;
	vm_e_t err;

	if (!phys_addr || !phys_size)
		return -EINVAL;

	/* get memory descriptor for a given name */
	VMAPI_CALL_ENTER();
#if PIKEOS_VM_VERSION > 34
	err = vm_mem_lookup(VM_RESPART_MYSELF, name, &mdesc);
#else
	err = vm_mem_lookup(name, &mdesc);
#endif
	VMAPI_CALL_EXIT();
	if (err)
		return p4_error_to_linux(err);

	/* status information about a memory object */
	VMAPI_CALL_ENTER();
	err = vm_mem_stat(VM_RESPART_MYSELF, mdesc, &mstat);
	VMAPI_CALL_EXIT();
	if (err)
		return p4_error_to_linux(err);

	*phys_addr = mstat.paddress;
	*phys_size = mstat.size;

	return 0; /* OK */
}
EXPORT_SYMBOL(vmapi_get_vmit_mem_region_by_name);

