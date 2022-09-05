/*
 *  PikeOS VM abstraction layer
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 * 
 *  Copyright (C) 2003-2014 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 *  Version 2003-12-16, azu: initial
 *  Version 2005-08-03, azu: merged vmport.c and vmfile.c info vmapi_calls.c
 */
#ifndef __VMAPI_CALLS_H_
#define __VMAPI_CALLS_H_

#include <asm/pikeos_version.h>

/* maximum number of files, dynamically allocated */
#define VMAPI_MAX_FILES	vmapi_max_files
#if PIKEOS_VM_VERSION >= 34
/* maximum number of directories, dynamically allocated */
#define VMAPI_MAX_DIRS	vmapi_max_dirs
#endif
/* maximum number of sbuf handles, statically allocated */
#define VMAPI_MAX_SHM		64

/* file open flags */
#define VMAPI_O_AUTO	0x0
#define VMAPI_O_R		0x1
#define VMAPI_O_W		0x2
#define VMAPI_O_MAP		0x4
#if PIKEOS_VM_VERSION >= 34
#define VMAPI_O_CREAT		0x8
#define VMAPI_O_EXCL		0x10
#endif

/* equivalent to VM_NAME_LEN */
#define VMAPI_NAME_LEN		(32u)
#if PIKEOS_VM_VERSION >= 34
#define VMAPI_EXT_NAME_LEN 256
#endif

#define VMAPI_FILE_T_DIR 0
#define VMAPI_FILE_T_REGULAR 1
#define VMAPI_FILE_T_PROPERTY 2
#define VMAPI_FILE_T_DEVICE 3
#define VMAPI_FILE_T_UNKNOWN 4

/* matches PikeOS vm_off_t */
#if PIKEOS_VM_VERSION >= 34
typedef long long vmapi_off_t;
#else
typedef long vmapi_off_t;
#endif

struct vmapi_file_stat {
	unsigned int mode;
	unsigned long inode;
	vmapi_off_t size;
	unsigned long blksize;
	unsigned long blkunit;
};

#if PIKEOS_VM_VERSION >= 34
struct vmapi_statvfs {
	unsigned long f_bsize;
	unsigned long f_blocks;
	unsigned long f_bfree;
	unsigned long f_files;
	unsigned long f_ffree;
	unsigned long f_namemax;
	unsigned long max_atomic_size;
};

struct vmapi_dirent {
	int type;
	char name[VMAPI_EXT_NAME_LEN];
};
#endif

extern int vmapi_max_files;
#if PIKEOS_VM_VERSION >= 34
extern int vmapi_max_dirs;
#endif

/* initialize VMAPI library */
void vmapi_init(void);

/* queueing port related functions */
void vmapi_qport_init(void);
int vmapi_qport_get_count(void);
int vmapi_qport_stat(int portnum, int *isoutgoing, int *framesize, char **portname);
int vmapi_qport_stat_num_msg(int portnum, unsigned int *nb_messages, unsigned int *max_messages);
int vmapi_qport_check_state(int portnum);
int vmapi_qport_receive(int portnum, void *buf, int count);
int vmapi_qport_send(int portnum, const void *buf, int count);
int vmapi_qport_poll_set(int portnum);
int vmapi_qport_poll_clear(int portnum);
int vmapi_qport_select(int *states);

/* VM related functions */
int  vmapi_partition_reboot(int part);
int  vmapi_partition_shutdown(int part);
void vmapi_partition_error(int isiniterror, const char *msg) __attribute__((noreturn));
int  vmapi_target_reboot(void);
int  vmapi_target_shutdown(void);
int  vmapi_target_power_off(void);
int  vmapi_set_time_sched_scheme(const char *scheme);
void vmapi_get_cmdline(char *buf, unsigned int maxsize);
void vmapi_cprintf(const char *fmt, ...);

/* file related functions */
void vmapi_fs_init(void);
int vmapi_file_stat(const char *name, struct vmapi_file_stat *status);
int vmapi_file_open(const char *name, unsigned int oflags, vmapi_off_t *size);
int vmapi_file_open4(const char *name, unsigned int oflags, vmapi_off_t *size, int *is_write);
vmapi_off_t vmapi_file_size(int handle);
int vmapi_file_fstat(int handle, struct vmapi_file_stat *vmapi_status);
int vmapi_file_close(int handle);
vmapi_off_t vmapi_file_read(int handle, void *buf, vmapi_off_t size);
vmapi_off_t vmapi_file_read_at(int handle, void *buf, vmapi_off_t size, vmapi_off_t offset);
vmapi_off_t vmapi_file_write_at(int handle, const void *buf, vmapi_off_t size, vmapi_off_t offset);
vmapi_off_t vmapi_file_write(int handle, const void *buf, vmapi_off_t size);
vmapi_off_t vmapi_file_lseek(int handle, vmapi_off_t offset, int whence);
int vmapi_file_fmap(int handle, vmapi_off_t foffset, vmapi_off_t size, unsigned long destaddr, unsigned int vmapi_flags);
int vmapi_file_ioctl(int handle, unsigned int cmd, void *data, int in, int out);

#if PIKEOS_VM_VERSION >= 34
int vmapi_file_fsync(int handle);
vmapi_off_t vmapi_file_ftruncate(int handle, vmapi_off_t length);

int vmapi_unlink(const char *name);
int vmapi_rename(const char *old_path, const char *new_path);
int vmapi_statvfs(const char *name, struct vmapi_statvfs *statvfs);

int vmapi_dir_create(const char *path);
int vmapi_dir_remove(const char *path);
int vmapi_dir_open(const char *name);
int vmapi_dir_read_at(int handle, vmapi_off_t *pos, struct vmapi_dirent *dirent);
int vmapi_dir_close(int handle);
int vmapi_dir_sync(int handle);

int vmapi_mount(const char *prefix, unsigned int vmapi_mflags);
int vmapi_umount(const char *prefix);
#endif

void vmapi_delay(unsigned int s);

/* sampling port related functions */
void vmapi_sport_init(void);
int vmapi_sport_get_count(void);
int vmapi_sport_stat(int portnum, int *isoutgoing, int *framesize, char **portname, unsigned long long *refresh, unsigned int *validity);
int vmapi_sport_check_state(int portnum);
int vmapi_sport_receive(int portnum, void *buf, int count, unsigned int *validity);
int vmapi_sport_send(int portnum, const void *buf, int count);

/* access property API */
int vmapi_property_read(const char *path, void *buf, unsigned int size);
int vmapi_property_map_irq(const char *path);
const char *vmapi_get_partname(void);
const char *vmapi_get_process_name(void);
int vmapi_property_device_grant(const char* path, unsigned int* device_id);

/* Termios to P4 serial FP */
int vmapi_set_termios(int handle, unsigned c_cflag);
int vmapi_get_termios(int handle, unsigned *c_cflag);

int vmapi_set_mctrl(int handle, unsigned short mctrl);
int vmapi_get_mctrl(int handle, unsigned short *mctrl);
int vmapi_get_mstat(int handle, unsigned short *status);

/* retrieve nic status */
int vmapi_get_eth_link_stat(int handle, int *linkup, int *speed);
int vmapi_get_eth_mac(int handle, unsigned char *mac);

#ifdef CONFIG_PCI_VIRT
struct resource;
struct msix_entry;
struct pci_dev;

extern int vmapi_pci_read_config(int handle, unsigned int where,
	unsigned int size, unsigned int *value);
extern int vmapi_pci_write_config(int handle, unsigned int where,
	unsigned int size, unsigned int value);
extern int vmapi_pci_get_irq(int handle);
extern int vmapi_pci_get_info(int handle, int *ctrl_id, int *bus_id,
    unsigned int *devfn);
extern int vmapi_pci_map_resource(int handle, int bar, struct resource *res,
    unsigned long *virt);
extern int vmapi_pci_request_irq(int handle, struct pci_dev *pdev);
extern int vmapi_pci_iterate(int handle, char *name, unsigned int size);
extern int vmapi_pci_msi_enable(int handle, unsigned int *base, int nvec);
extern int vmapi_pci_msi_disable(int handle);
extern int vmapi_pci_msix_enable(int handle, struct msix_entry *entries, int nvec);
extern int vmapi_pci_msix_disable(int handle);
extern int vmapi_pci_msi_msix_stat(int handle, unsigned int *msix_avail, unsigned int *msi_avail);
#endif

/* libsbuf API wrapper */
int vmapi_io_sbuf_init(int handle, unsigned int *size);
int vmapi_io_sbuf_map(int handle, unsigned long vaddr);
int vmapi_io_sbuf_write(int shm, const void *buf, int count, int noblock);
int vmapi_io_sbuf_read(int shm, void *buf, int count, int noblock);
int vmapi_io_sbuf_close(int shm);

/* misc/extensions */
int vmapi_get_vmit_mem_region_by_name(const char *name, unsigned long *phys_addr, unsigned int *phys_size);

#endif
