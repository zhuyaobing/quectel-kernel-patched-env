/*
 *  Filesystem to access PikeOS Volumes
 *
 *  Author: Miroslav Rudisin <rud@sysgo.com>
 *
 *  Copyright (C) 2015 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/parser.h>
#include <linux/slab.h>
#include <linux/statfs.h>
#include <linux/crc32.h>
#include <linux/interrupt.h>
#include <asm/locking.h>
#include <asm/uaccess.h>
#include <asm/vmapi_calls.h>

#define PIKEOSFS_SUPER_MAGIC 0x11ed6cee

#define PIKEOSFS_DEFAULT_FILE_MODE 0644
#define PIKEOSFS_DEFAULT_DIR_MODE 0755

/* these features are not properly implemented yet */
#undef USE_LINKS
#undef USE_TIME

/* do not use direct VM API from syscalls */
#define VMAPI_DIRECT 0

/* defined by VM_MAX_EXT_PATHNAME_LEN; including terminating NULL */
#define VMAPI_MAX_EXT_PATHNAME_LEN	1024

/* open flags that requires granted write access to PikeOS volume */
#define WRITE_ACCESS (O_WRONLY | O_RDWR | O_CREAT | O_TRUNC | O_APPEND)

/*
 * Notice: reqmax defines index of the last item in reqs array,
 *         because there is 1 extra item allocated.
 */
#define REQ_POS_ADVANCE(fsi, counter) \
	(((fsi)->counter == (fsi)->mount_opts.reqmax) ? \
	 0 : ((fsi)->counter + 1))

struct req_entry {
	struct completion req_complete;
	/* input */
	enum {
		REQ_MOUNT,
		REQ_UMOUNT,
		REQ_FILE_STAT,
		REQ_FILE_OPEN,
		REQ_FILE_CLOSE,
		REQ_FILE_READ_AT,
		REQ_FILE_WRITE_AT,
		REQ_FILE_FTRUNCATE,
		REQ_FILE_SIZE_LSEEK,
		REQ_FILE_FSTAT,
		REQ_FILE_FSYNC,
		REQ_DIR_OPEN,
		REQ_DIR_CLOSE,
		REQ_DIR_READ_AT,
		REQ_DIR_CREATE,
		REQ_DIR_REMOVE,
		REQ_DIR_SYNC,
		REQ_UNLINK,
		REQ_RENAME,
		REQ_STATVFS
	} op;
	int fd;
	union {
		const char *s1;
		char *b;
	};
	union {
		unsigned int ui;
		const char *s2;
		vmapi_off_t sz;
		struct vmapi_file_stat *file_stat;
		struct vmapi_statvfs *statvfs;
		struct vmapi_dirent *dirent;
	};
	/* input/output */
	vmapi_off_t off;
	/* output */
	int err;
};

struct pikeosfs_mount_opts {
	bool ro;
	umode_t file_mode;
	umode_t dir_mode;
	int reqmax;
};

struct pikeosfs_fs_info {
	struct pikeosfs_mount_opts mount_opts;
	char *volname;
	int volname_len;
	struct mutex myfd_mutex;

	struct req_entry **reqs;
	volatile int req_pos_completed;
	volatile int req_pos_executed;
	volatile int req_pos_empty;

	spinlock_t req_pos_completed_lock;
	spinlock_t req_pos_empty_lock;

	/* async thread */
	unsigned int irq;
	struct uk_uevent irq_sleep;
};

static void pikeosfs_execute_req(struct pikeosfs_fs_info *fsi,
		struct req_entry *re);

struct myfd_acc {
	int fd;
	int refs;
};

struct myfd {
	struct myfd_acc rd;
	struct myfd_acc rw;
	struct hlist_node hl_fd_entry;
	struct hlist_head hl_fd_children; /* used only in the dir dentries */
	struct dentry *dentry;
};

static inline struct myfd_acc *myfd_get_acc(
		struct myfd *mf,
		unsigned flags)
{
	struct myfd_acc *ac;

	if ((flags & WRITE_ACCESS) != 0)
		ac = &mf->rw;
	else
		ac = &mf->rd;

	return ac;
}

static int myfd_get(struct dentry *dentry, unsigned flags)
{
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct myfd *mf;
	struct myfd_acc *ac;
	int fd;

	if (!dentry) {
		return -EINVAL;
	}
	mf = dentry->d_fsdata;
	if (!mf) {
		return -EINVAL;
	}

	mutex_lock(&fsi->myfd_mutex);

	ac = myfd_get_acc(mf, flags);
	if (ac->refs <= 0) {
		mutex_unlock(&fsi->myfd_mutex);
		return -EBADF;
	}
	fd = ac->fd;

	return fd;
}

static void myfd_put(struct dentry *dentry, unsigned flags)
{
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;

	mutex_unlock(&fsi->myfd_mutex);
}

static struct myfd *get_new_mf(struct dentry *dentry)
{
	struct myfd *mf;

	mf = kmalloc(sizeof(struct myfd), GFP_KERNEL);
	if (mf != NULL) {
		mf->rd.fd = -1;
		mf->rd.refs = 0;
		mf->rw.fd = -1;
		mf->rw.refs = 0;
		INIT_HLIST_NODE(&mf->hl_fd_entry);
		INIT_HLIST_HEAD(&mf->hl_fd_children);
		mf->dentry = dentry;
	}

	return mf;
}

static int myfd_open(struct dentry *dentry, unsigned flags, bool *open, int *fd)
{
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct myfd *mf;
	struct myfd_acc *ac;

	mutex_lock(&fsi->myfd_mutex);

	mf = dentry->d_fsdata;
	if (mf == NULL) {
		mf = get_new_mf(dentry);
		if (!mf) {
			mutex_unlock(&fsi->myfd_mutex);
			return -ENOMEM;
		}
		dentry->d_fsdata = mf;
	}

	if (IS_ERR(mf)) {
		mutex_unlock(&fsi->myfd_mutex);
		return PTR_ERR(mf);
	}

	ac = myfd_get_acc(mf, flags);
	ac->refs++;
	if (ac->refs == 1) {
		*open = false;
	} else {
		*fd = ac->fd;
		*open = true;
	}

	return 0;
}

static void myfd_open_finish(struct dentry *dentry, unsigned flags, int fd)
{
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct myfd *mf;
	struct myfd *mfp;
	struct myfd_acc *ac;
	struct dentry *dparent;

	mf = dentry->d_fsdata;
	/* lock for d_parent is not needed, it is guarded by myfd locks */
	dparent = dentry->d_parent;
	mfp = dparent->d_fsdata;
	BUG_ON(mf == NULL || mfp == NULL);
	if ((mf != mfp) && (hlist_unhashed(&mf->hl_fd_entry))) {
		/* add to parrent directory only once */
		hlist_add_head(&mf->hl_fd_entry, &mfp->hl_fd_children);
	}
	ac = myfd_get_acc(mf, flags);
	ac->fd = fd;
	mutex_unlock(&fsi->myfd_mutex);
}

/* returns TRUE if closing last reference to file/dir */
static bool myfd_close(struct dentry *dentry, unsigned flags, int *fd)
{
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct myfd *mf;
	struct myfd_acc *ac;

	mutex_lock(&fsi->myfd_mutex);
	mf = dentry->d_fsdata;
	ac = myfd_get_acc(mf, flags);
	*fd = ac->fd;

	/* is the last one reference? */
	return ac->refs == 1;
}

static void myfd_close_finish(struct dentry *dentry, unsigned flags)
{
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct myfd *mf;
	struct myfd_acc *ac;

	mf = dentry->d_fsdata;
	ac = myfd_get_acc(mf, flags);
	BUG_ON(ac->refs < 1);
	ac->refs--;
	if (!ac->refs) {
		ac->fd = -1;
	}
	mutex_unlock(&fsi->myfd_mutex);
}

static struct filename *get_fname(struct dentry *d)
{
	struct super_block *sb = d->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct filename *tmp;
	char *kname;
	char *name;
	int len = 0;
	struct dentry *p;

	/* lock for d_parent is not needed, it is guarded by myfd locks */
	for (p = d; !IS_ROOT(p); p = p->d_parent)
		len += p->d_name.len + 1;

	/* include terminating NULL minus one path delimiter counted above */
	if (unlikely(fsi->volname_len + len > VMAPI_MAX_EXT_PATHNAME_LEN))
		return ERR_PTR(-ENAMETOOLONG);

	tmp = __getname();
	if (unlikely(!tmp))
		return ERR_PTR(-ENOMEM);

	kname = (char *)tmp + sizeof(*tmp);
	tmp->name = kname;
	tmp->separate = false;

	strncpy(kname, fsi->volname, fsi->volname_len);
	name = &kname[fsi->volname_len - 1]; /* volname includes ':' */

	if (len == 0) {
		name[0] = ':';
		name[1] = '\0';
		return tmp;
	}

	name[len] = '\0';
	for (p = d; !IS_ROOT(p); p = p->d_parent) {
		len -= p->d_name.len;
		strncpy(&(name[len]), p->d_name.name, p->d_name.len);
		len--;
		name[len] = '/';
	}
	name[len] = ':';

	return tmp;
}

static inline void put_fname(struct dentry *d, struct filename *tmp)
{
	__putname(tmp);
}

static int helper_open(struct dentry *dentry, unsigned flags)
{
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct req_entry re;
	struct filename *tmp;
	unsigned int vmapi_flags;
	int ret;

	tmp = get_fname(dentry);
	if (IS_ERR(tmp))
		return PTR_ERR(tmp);

	if ((flags & WRITE_ACCESS) != 0)
		vmapi_flags = VMAPI_O_R | VMAPI_O_W;
	else
		vmapi_flags = VMAPI_O_R;

	if (VMAPI_DIRECT) {
		ret = vmapi_file_open(tmp->name, vmapi_flags, 0);
	} else {
		re.op = REQ_FILE_OPEN;
		re.s1 = tmp->name;
		re.ui = vmapi_flags;
		pikeosfs_execute_req(fsi, &re);
		ret = re.err;
	}

	put_fname(dentry, tmp);

	return ret;
}

static int pikeosfs_read_size(struct file *file)
{
	struct inode *inode = file->f_mapping->host;
	struct dentry *dentry = file->f_path.dentry;
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct req_entry re;
	vmapi_off_t size;
	int fd;

	fd = myfd_get(dentry, file->f_flags);
	if (fd < 0)
		return fd;
	if (VMAPI_DIRECT) {
		size = vmapi_file_lseek(fd, 0, SEEK_END);
	} else {
		re.op = REQ_FILE_SIZE_LSEEK;
		re.fd = fd;
		pikeosfs_execute_req(fsi, &re);
		size = re.off;
	}
	myfd_put(dentry, file->f_flags);

	if (size < 0)
		return (int)size;

	inode->i_size = size;

	return 0;
}

static loff_t pikeosfs_llseek(struct file *file, loff_t offset, int whence)
{
	struct inode *inode = file->f_mapping->host;
	loff_t size, ret;

	if (whence == SEEK_END) {
		ret = pikeosfs_read_size(file);
		if (ret < 0)
			return ret;
		size = inode->i_size;
	} else {
		size = 0;
	}

	ret = generic_file_llseek_size(file, offset, whence,
			inode->i_sb->s_maxbytes,
			size);

	return ret;
}

static int pikeosfs_do_release(struct dentry *dentry, unsigned flags)
{
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct req_entry re;
	bool is_last;
	int fd;
	int ret;

	is_last = myfd_close(dentry, flags, &fd);
	if (is_last) {
		if (VMAPI_DIRECT) {
			ret = vmapi_file_close(fd);
		} else {
			re.op = REQ_FILE_CLOSE;
			re.fd = fd;
			pikeosfs_execute_req(fsi, &re);
			ret = re.err;
		}
		BUG_ON(ret != 0);
		myfd_close_finish(dentry, flags);
	} else {
		if (flags & WRITE_ACCESS) {
			/* sync file if it could be modified */
			if (VMAPI_DIRECT) {
				ret = vmapi_file_fsync(fd);
			} else {
				re.op = REQ_FILE_FSYNC;
				re.fd = fd;
				pikeosfs_execute_req(fsi, &re);
				ret = re.err;
			}
		} else {
			ret = 0;
		}
		myfd_close_finish(dentry, flags);
	}

	return ret;
}

static int pikeosfs_open(struct inode *inode, struct file *file)
{
	int fd;
	struct dentry *dentry;
	int ret;
	bool open;

	dentry = file->f_path.dentry;
	ret = myfd_open(dentry, file->f_flags, &open, &fd);
	if (ret)
		return ret;

	if (!open) {
		fd = helper_open(dentry, file->f_flags);
		if (fd < 0) {
			myfd_close_finish(dentry, file->f_flags);
			return fd;
		}
	}

	myfd_open_finish(dentry, file->f_flags, fd);

	/*
	 * Notice:
	 *   Keep flags used for open, because kernel changes them afterwards.
	 *   Otherwise close is not able handle "create file for reading" case.
	 */
	file->private_data = (void *)file->f_flags;

	return ret;
}

static int pikeosfs_release(struct inode *inode, struct file *file)
{
	unsigned flags = (unsigned)file->private_data;

	return pikeosfs_do_release(file->f_path.dentry, flags);
}

static ssize_t pikeosfs_read(struct file *file, char __user *buf,
		size_t len, loff_t *ppos)
{
	struct dentry *dentry = file->f_path.dentry;
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct req_entry re;
	int fd;
	int ret;
	unsigned long off;
	int len2;
	unsigned long map_addr;
	int map_len;
	unsigned long off2;
	int len3;

	fd = myfd_get(dentry, file->f_flags);
	if (fd < 0)
		return fd;

	ret = 0;
	off = 0;

	/*
	 * OPTIMIZATION:
	 * use temporary buffer to not divide small operations into two VM calls
	 */
	if ((len <= PAGE_SIZE) &&
			(((unsigned long)buf & PAGE_MASK) !=
			 (((unsigned long)buf + len - 1) & PAGE_MASK))) {
		map_addr = __get_free_page(GFP_KERNEL);
		if (map_addr) {
			len3 = 0;
			off2 = 0;
			while (len > 0) {
				if (VMAPI_DIRECT) {
					ret = vmapi_file_read_at(fd, (char *)(map_addr + off2), len,
							(*ppos + off));
				} else {
					re.op = REQ_FILE_READ_AT;
					re.fd = fd;
					re.b = (char *)(map_addr + off2);
					re.sz = len;
					re.off = *ppos + off;
					pikeosfs_execute_req(fsi, &re);
					ret = re.off;
				}
				if (ret <= 0) {
					/* EOF or error */
					break;
				}
				off += ret;
				off2 += ret;
				len -= ret;
				len3 += ret;
			}
			copy_to_user(buf, (char *)map_addr, len3);
			free_page(map_addr);
			goto read_done;
		}
	}

	do {
		map_addr = map_cont_area((unsigned long)(buf + off), len, 1, &map_len);
		if (!map_addr) {
			ret = -EFAULT;
			break;
		}
		if (len < map_len) {
			len2 = len;
		} else {
			len2 = map_len;
		}
		off2 = 0;
		while (len2 > 0) {
			if (VMAPI_DIRECT) {
				ret = vmapi_file_read_at(fd, (char *)(map_addr + off2), len2,
						(*ppos + off));
			} else {
				re.op = REQ_FILE_READ_AT;
				re.fd = fd;
				re.b = (char *)(map_addr + off2);
				re.sz = len2;
				re.off = *ppos + off;
				pikeosfs_execute_req(fsi, &re);
				ret = re.off;
			}
			if (ret <= 0) {
				/* EOF or error */
				break;
			}
			off += ret;
			off2 += ret;
			len -= ret;
			len2 -= ret;
		}
		unmap_cont_area(map_addr, map_len);
	} while (len > 0 && ret > 0);

read_done:
	myfd_put(dentry, file->f_flags);

	*ppos += off;

	if (ret >= 0) {
		ret = off;
	}

	return ret;
}

static ssize_t pikeosfs_write(struct file *file, const char __user *buf,
		size_t len, loff_t *ppos)
{
	struct dentry *dentry = file->f_path.dentry;
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct req_entry re;
	int fd;
	int ret;
	unsigned long off;
	int len2;
	unsigned long map_addr;
	int map_len;
	unsigned long off2;

	fd = myfd_get(dentry, file->f_flags);
	if (fd < 0) {
		return fd;
	}

	if ((file->f_flags & O_APPEND) != 0) {
		if (VMAPI_DIRECT) {
			*ppos = vmapi_file_lseek(fd, 0, SEEK_END);
		} else {
			re.op = REQ_FILE_SIZE_LSEEK;
			re.fd = fd;
			pikeosfs_execute_req(fsi, &re);
			*ppos = re.off;
		}
		if (*ppos < 0) {
			return *ppos;
		}
	}

	ret = 0;
	off = 0;

	/*
	 * OPTIMIZATION:
	 * use temporary buffer to not divide small operations into two VM calls
	 */
	if ((len <= PAGE_SIZE) &&
			(((unsigned long)buf & PAGE_MASK) !=
			 (((unsigned long)buf + len - 1) & PAGE_MASK))) {
		map_addr = __get_free_page(GFP_KERNEL);
		if (map_addr) {
			copy_from_user((char *)map_addr, buf, len);
			off2 = 0;
			while (len > 0) {
				if (VMAPI_DIRECT) {
					ret = vmapi_file_write_at(fd, (char *)(map_addr + off2), len,
							(*ppos + off));
				} else {
					re.op = REQ_FILE_WRITE_AT;
					re.fd = fd;
					re.b = (char *)(map_addr + off2);
					re.sz = len;
					re.off = *ppos + off;
					pikeosfs_execute_req(fsi, &re);
					ret = re.off;
				}
				if (ret <= 0) {
					/* EOF or error */
					break;
				}
				off += ret;
				off2 += ret;
				len -= ret;
			}
			free_page(map_addr);
			goto write_done;
		}
	}

	do {
		map_addr = map_cont_area((unsigned long)(buf + off), len, 0, &map_len);
		if (!map_addr) {
			ret = -EFAULT;
			break;
		}
		if (len < map_len) {
			len2 = len;
		} else {
			len2 = map_len;
		}
		off2 = 0;
		while (len2 > 0) {
			if (VMAPI_DIRECT) {
				ret = vmapi_file_write_at(fd, (char *)(map_addr + off2), len2,
						(*ppos + off));
			} else {
				re.op = REQ_FILE_WRITE_AT;
				re.fd = fd;
				re.b = (char *)(map_addr + off2);
				re.sz = len2;
				re.off = *ppos + off;
				pikeosfs_execute_req(fsi, &re);
				ret = re.off;
			}
			if (ret <= 0) {
				/* EOF or error */
				break;
			}
			off += ret;
			off2 += ret;
			len -= ret;
			len2 -= ret;
		}
		unmap_cont_area(map_addr, map_len);
	} while (len > 0 && ret > 0);

write_done:
	myfd_put(dentry, file->f_flags);

	*ppos += off;

	if (ret >= 0) {
		ret = off;
	}

	return ret;
}

static int pikeosfs_fsync(struct file *file, loff_t start, loff_t end,
		int datasync)
{
	struct dentry *dentry = file->f_path.dentry;
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct req_entry re;
	int fd;
	int ret;

	fd = myfd_get(dentry, file->f_flags);
	if (fd < 0)
		return fd;
	if (VMAPI_DIRECT) {
		ret = vmapi_file_fsync(fd);
	} else {
		re.op = REQ_FILE_FSYNC;
		re.fd = fd;
		pikeosfs_execute_req(fsi, &re);
		ret = re.err;
	}
	myfd_put(dentry, file->f_flags);

	return ret;
}

static const struct file_operations pikeosfs_file_operations = {
	.open		= pikeosfs_open,
	.release		= pikeosfs_release,
	.read		= pikeosfs_read,
	.write		= pikeosfs_write,
	.fsync		= pikeosfs_fsync,
	.llseek		= pikeosfs_llseek,
};

static int pikeosfs_getattr(struct vfsmount *mnt, struct dentry *dentry,
		struct kstat *stat)
{
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct filename *tmp;
	struct vmapi_file_stat fattr;
	struct req_entry re;
	int result;

	generic_fillattr(dentry->d_inode, stat);

	tmp = get_fname(dentry);
	if (IS_ERR(tmp))
		return PTR_ERR(tmp);

	if (VMAPI_DIRECT) {
		result = vmapi_file_stat(tmp->name, &fattr);
	} else {
		re.op = REQ_FILE_STAT;
		re.s1 = tmp->name;
		re.file_stat = &fattr;
		pikeosfs_execute_req(fsi, &re);
		result = re.err;
	}
	put_fname(dentry, tmp);

	if (result)
		return result;

	/* update size */
	stat->size = fattr.size;
	stat->blocks = (fattr.size + 511) / 512;

	/* enforce mode according to the mount flags */
	fattr.mode &= ~S_IRWXUGO;
	if ((fattr.mode & S_IFDIR) != 0)
		stat->mode = fattr.mode | fsi->mount_opts.dir_mode;
	else
		stat->mode = fattr.mode | fsi->mount_opts.file_mode;

	return 0;
}

static int pikeosfs_setattr(struct dentry *dentry, struct iattr *attr)
{
	struct inode *inode = dentry->d_inode;
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct req_entry re;
	int error;
	bool open;
	int fd;

	error = inode_change_ok(inode, attr);
	if (error)
		return error;

	if (attr->ia_valid & ATTR_SIZE) {
		error = inode_newsize_ok(inode, attr->ia_size);
		if (error)
			return error;

		error = myfd_open(dentry, O_RDWR, &open, &fd);
		if (error)
			return error;

		if (!open) {
			error = helper_open(dentry, O_RDWR);
			if (error < 0) {
				myfd_close_finish(dentry, O_RDWR);
				return error;
			}
			fd = error;
		}

		if (VMAPI_DIRECT) {
			error = vmapi_file_ftruncate(fd, attr->ia_size);
		} else {
			re.op = REQ_FILE_FTRUNCATE;
			re.fd = fd;
			re.off = attr->ia_size;
			pikeosfs_execute_req(fsi, &re);
			error = re.err;
		}

		if (!open) {
			if (VMAPI_DIRECT) {
				(void)vmapi_file_close(fd);
			} else {
				re.op = REQ_FILE_CLOSE;
				re.fd = fd;
				pikeosfs_execute_req(fsi, &re);
				(void)re.err;
			}
			myfd_close_finish(dentry, O_RDWR);
		} else {
			/*
			 * Notice:
			 *   the lock was held during VMAPI call, so this call
			 *   will not become the last user of FD,
			 *   so we can have "last_closed" set to false
			 */
			myfd_close_finish(dentry, O_RDWR);
		}

		if (error)
			return error;
	}

	setattr_copy(inode, attr);
	mark_inode_dirty(inode);

	return 0;
}

static const struct inode_operations pikeosfs_file_inode_operations = {
	.setattr	= pikeosfs_setattr,
	.getattr	= pikeosfs_getattr,
};

static int pikeosfs_dir_open(struct inode *inode, struct file *file)
{
	struct dentry *dentry = file->f_path.dentry;
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct filename *tmp;
	struct req_entry re;
	int fd;
	int ret;
	bool open;

	tmp = get_fname(dentry);
	if (IS_ERR(tmp))
		return PTR_ERR(tmp);

	ret = myfd_open(dentry, file->f_flags, &open, &fd);
	if (ret == 0) {
		if (!open) {
			if (VMAPI_DIRECT) {
				fd = vmapi_dir_open(tmp->name);
			} else {
				re.op = REQ_DIR_OPEN;
				re.s1 = tmp->name;
				pikeosfs_execute_req(fsi, &re);
				fd = re.err;
			}
			if (fd < 0) {
				myfd_close_finish(dentry, file->f_flags);
				put_fname(dentry, tmp);
				return fd;
			}
		}
		myfd_open_finish(dentry, file->f_flags, fd);
	}
	/*
	 * Notice:
	 *   Keep flags used for open, because kernel changes them afterwards.
	 *   Otherwise close is not able handle "create file for reading" case.
	 */
	file->private_data = (void *)file->f_flags;

	put_fname(dentry, tmp);

	return ret;
}

static int pikeosfs_dir_close(struct inode *inode, struct file *file)
{
	struct dentry *dentry = file->f_path.dentry;
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct req_entry re;
	unsigned flags;
	bool is_last;
	int fd;
	int ret;

	flags = (unsigned)file->private_data;
	is_last = myfd_close(dentry, flags, &fd);
	if (is_last) {
		if (VMAPI_DIRECT) {
			ret = vmapi_dir_close(fd);
		} else {
			re.op = REQ_DIR_CLOSE;
			re.fd = fd;
			pikeosfs_execute_req(fsi, &re);
			ret = re.err;
		}
		BUG_ON(ret != 0);
		myfd_close_finish(dentry, flags);
	} else {
		ret = 0;
		myfd_close_finish(dentry, flags);
	}

	return ret;
}

static loff_t pikeosfs_dir_lseek(struct file *file, loff_t offset, int whence)
{
	struct dentry *dentry = file->f_path.dentry;

	if (whence != 0)
		return -EINVAL;

	mutex_lock(&dentry->d_inode->i_mutex);
	file->f_pos = offset;
	mutex_unlock(&dentry->d_inode->i_mutex);

	return 0;
}

static int pikeosfs_readdir(struct file *file, struct dir_context *ctx)
{
	struct dentry *dentry = file->f_path.dentry;
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct req_entry re;
	int fd;
	vmapi_off_t pos;
	struct vmapi_dirent vd;
	unsigned char dt;
	int err = 0;

	if (!dir_emit_dots(file, ctx))
		return 0;

	/* get vmapi descriptor of opened directory */
	fd = myfd_get(dentry, file->f_flags);
	if (fd < 0)
		return fd;

	/* get posisiton without offset for dots entries */
	pos = ctx->pos - 2;

	for (;;) {
		vd.name[0]=0;
		if (VMAPI_DIRECT) {
			err = vmapi_dir_read_at(fd, &pos, &vd);
		} else {
			re.op = REQ_DIR_READ_AT;
			re.fd = fd;
			re.off = pos;
			re.dirent = &vd;
			pikeosfs_execute_req(fsi, &re);
			pos = re.off;
			err = re.err;
		}
		if (err < 0)
			break;

		/* check for end of directory */
		if (vd.name[0] == 0)
			break;

		switch (vd.type) {
			case VMAPI_FILE_T_DIR:
				dt = DT_DIR;
				break;
			case VMAPI_FILE_T_REGULAR:
				dt = DT_REG;
				break;
			default:
				dt = DT_UNKNOWN;
				break;
		}

		/*
		 * Notice:
		 *   Directory entries are emitted with the inode number 1.
		 *   This allows readdir to display entries and we do not need to
		 *   generate and maintain inode numbers for unused entries.
		 */
		if (!dir_emit(ctx, vd.name, strlen(vd.name), 1, dt))
			break;

		/* update position with offset for dots */
		ctx->pos = pos + 2;
	}

	myfd_put(dentry, file->f_flags);

	return err;
}

static int pikeosfs_dir_sync(struct file *file, loff_t start, loff_t end,
		int datasync)
{
	struct dentry *dentry = file->f_path.dentry;
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct req_entry re;
	int fd;
	int ret;

	fd = myfd_get(dentry, file->f_flags);
	if (fd < 0)
		return fd;
	if (VMAPI_DIRECT) {
		ret = vmapi_dir_sync(fd);
	} else {
		re.op = REQ_DIR_SYNC;
		re.fd = fd;
		pikeosfs_execute_req(fsi, &re);
		ret = re.err;
	}
	myfd_put(dentry, file->f_flags);

	return ret;
}

static const struct file_operations pikeosfs_dir_operations = {
	.open		= pikeosfs_dir_open,
	.release	= pikeosfs_dir_close,
	.llseek		= pikeosfs_dir_lseek,
	.read		= generic_read_dir, /* returns -EISDIR */
	.iterate	= pikeosfs_readdir,
	.fsync		= pikeosfs_dir_sync,
};

static const struct inode_operations pikeosfs_dir_inode_operations;

static struct inode *pikeosfs_get_inode(struct super_block *sb,
				const struct inode *dir, umode_t mode, unsigned long ino)
{
	struct inode *inode;

	/* only files and directories are supported */
	if (((mode & S_IFMT) & ~(S_IFREG | S_IFDIR)) != 0)
		return ERR_PTR(-EINVAL);

	inode = new_inode(sb);
	if (!inode)
		return ERR_PTR(-ENOMEM);

	inode->i_ino = ino;

	inode_init_owner(inode, dir, mode);
#ifdef USE_TIME
	inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
#endif
	switch (mode & S_IFMT) {
		case S_IFREG:
			inode->i_op = &pikeosfs_file_inode_operations;
			inode->i_fop = &pikeosfs_file_operations;
			break;
		case S_IFDIR:
			inc_nlink(inode);
			inode->i_op = &pikeosfs_dir_inode_operations;
			inode->i_fop = &pikeosfs_dir_operations;
			break;
	}

	return inode;
}

static int pikeosfs_mkdir(struct inode * dir, struct dentry * dentry,
		umode_t mode)
{
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct filename *tmp;
	struct req_entry re;
	int error;
	struct vmapi_file_stat fattr;
	unsigned long ino;
	struct inode *inode;

	tmp = get_fname(dentry);
	if (IS_ERR(tmp))
		return PTR_ERR(tmp);

	if (VMAPI_DIRECT) {
		error = vmapi_dir_create(tmp->name);
	} else {
		re.op = REQ_DIR_CREATE;
		re.s1 = tmp->name;
		pikeosfs_execute_req(fsi, &re);
		error = re.err;
	}

	ino = 0;
	if (error == 0) {
		if (VMAPI_DIRECT) {
			error = vmapi_file_stat(tmp->name, &fattr);
		} else {
			re.op = REQ_FILE_STAT;
			re.s1 = tmp->name;
			re.file_stat = &fattr;
			pikeosfs_execute_req(fsi, &re);
			error = re.err;
		}
		if (error == 0) {
			if ((fattr.mode & S_IFDIR) != 0)
				ino = fattr.inode;
			else
				error = -EEXIST;
		}
	}

	if (error == 0) {
		inode = pikeosfs_get_inode(dir->i_sb, dir, S_IFDIR | mode, ino);
		if (!IS_ERR(inode)) {
			d_instantiate(dentry, inode);
			error = 0;
		} else {
			error = PTR_ERR(inode);
		}
#ifdef USE_LINKS
		inc_nlink(dir);
#endif
	}

	put_fname(dentry, tmp);

	return error;
}

static struct dentry *pikeosfs_do_lookup(struct filename *tmp,
		struct inode *dir, struct dentry *dentry)
{
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct vmapi_file_stat fattr;
	int result;
	struct dentry *dparent;
	struct inode *inode;
	struct myfd *mfp;
	struct myfd *mf;
	struct req_entry re;

	/* lock for d_parent is not needed, it is guarded by myfd locks */
	dparent = dentry->d_parent;
	if (dparent == NULL || dparent == dentry)
		return ERR_PTR(-EINVAL);
	else if (dparent->d_fsdata == NULL)
		return ERR_PTR(-EINVAL);
	else {
		struct dentry *d_new = NULL;
		const struct qstr * const n_self = &dentry->d_name;

		mfp = dparent->d_fsdata;

		spin_lock(&dentry->d_lock);
		hlist_for_each_entry(mf, &mfp->hl_fd_children, hl_fd_entry) {
			struct dentry *d_oth = mf->dentry;
			if (d_oth != NULL) {
				const struct qstr * const n_oth = &d_oth->d_name;

				if ((n_self->len == n_oth->len) &&
						(strcmp(n_self->name, n_oth->name) == 0)) {
					d_new = d_oth;
					dget(d_new);
					break;
				}
			}
		}
		spin_unlock(&dentry->d_lock);
		if (d_new != NULL)
			return d_new;
	}

	if (VMAPI_DIRECT) {
		result = vmapi_file_stat(tmp->name, &fattr);
	} else {
		re.op = REQ_FILE_STAT;
		re.s1 = tmp->name;
		re.file_stat = &fattr;
		pikeosfs_execute_req(fsi, &re);
		result = re.err;
	}
	if (result < 0) {
		d_add(dentry, NULL);
		if (result == -ENOENT)
			return NULL;
		else
			return ERR_PTR(result);
	}

	/* enforce mode according to the mount flags */
	fattr.mode &= ~S_IRWXUGO;
	if ((fattr.mode & S_IFDIR) != 0) {
		if (dentry->d_fsdata)
			return ERR_PTR(-EINVAL);
		mf = get_new_mf(dentry);
		if (!mf)
			return ERR_PTR(-ENOMEM);
		fattr.mode |= fsi->mount_opts.dir_mode;
	} else
		fattr.mode |= fsi->mount_opts.file_mode;

	inode = pikeosfs_get_inode(dir->i_sb, dir, fattr.mode, fattr.inode);
	if (IS_ERR(inode)) {
		kfree(mf);
		return ERR_PTR(PTR_ERR(inode));
	}

	if ((fattr.mode & S_IFDIR) != 0) {
		dentry->d_fsdata = mf; /* mf was allocated above */
		hlist_add_head(&mf->hl_fd_entry, &mfp->hl_fd_children);
	}

	return d_splice_alias(inode, dentry);
}

static struct dentry *pikeosfs_lookup(struct inode *dir, struct dentry *dentry,
		unsigned int flags)
{
	struct filename *tmp;
	struct dentry *res;

	tmp = get_fname(dentry);
	if (IS_ERR(tmp))
		return (struct dentry *)tmp;

	res = pikeosfs_do_lookup(tmp, dir, dentry);

	put_fname(dentry, tmp);

	return res;
}

static int pikeosfs_atomic_open(struct inode *dir, struct dentry *dentry,
		struct file *file, unsigned flags,
		umode_t mode, int *opened)
{
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	int err, fd;
	struct dentry *res = NULL;
	struct filename *tmp;
	bool open;
	unsigned long ino;
	struct vmapi_file_stat fattr;
	struct inode *inode;
	unsigned int vmflags;
	struct req_entry re;

	tmp = get_fname(dentry);
	if (IS_ERR(tmp)) {
		err = PTR_ERR(tmp);
		goto pao_exit;
	}

	if (d_unhashed(dentry)) {
		res = pikeosfs_do_lookup(tmp, dir, dentry);
		if (IS_ERR(res)) {
			err = PTR_ERR(res);
			res = NULL; /* for dput() */
			goto pao_free_name;
		}
		if (res)
			dentry = res;
	}

	if (!(flags & O_CREAT)) {
		/* handle this with .open unless new file is requested */
		/* Notice: this also serves atomic open on directories */
		err = finish_no_open(file, 0);
		goto pao_free_name;
	}

	if (dentry->d_inode &&
			((!S_ISREG(dentry->d_inode->i_mode)) || (flags & O_EXCL))) {
		err = -EEXIST;
		goto pao_free_name;
	}

	err = myfd_open(dentry, flags, &open, &fd);
	if (err != 0)
		goto pao_free_name;

	vmflags = VMAPI_O_R | VMAPI_O_W | VMAPI_O_CREAT;
	if (flags & O_EXCL) {
		vmflags |= VMAPI_O_EXCL;
		if (open) {
			err = -EEXIST;
			goto pao_free_myfd;
		}
	}

	if (VMAPI_DIRECT) {
		err = vmapi_file_open(tmp->name, vmflags, 0);
	} else {
		re.op = REQ_FILE_OPEN;
		re.s1 = tmp->name;
		re.ui = vmflags;
		pikeosfs_execute_req(fsi, &re);
		err = re.err;
	}
	if (err < 0)
		goto pao_free_myfd;

	fd = err;

	/* resolve inode number of new file */
	if (VMAPI_DIRECT) {
		err = vmapi_file_fstat(fd, &fattr);
	} else {
		re.op = REQ_FILE_FSTAT;
		re.fd = fd;
		re.file_stat = &fattr;
		pikeosfs_execute_req(fsi, &re);
		err = re.err;
	}
	if (err || ((fattr.mode & S_IFREG) == 0)) {
		err = -EEXIST;
		goto pao_free_vmfd;
	}
	ino = fattr.inode;

	if (!dentry->d_inode) {
		inode = pikeosfs_get_inode(dir->i_sb, dir, mode, ino);
		if (IS_ERR(inode)) {
			err = PTR_ERR(inode);
			goto pao_free_vmfd;
		}
		d_instantiate(dentry, inode);
	}

	err = finish_open(file, dentry, generic_file_open, opened);
	if (err)
		goto pao_free_vmfd;

	myfd_open_finish(dentry, flags, fd);
	/*
	 * Notice:
	 *   Keep flags used for open, because kernel changes them afterwards.
	 *   Otherwise close is not able handle "create file for reading" case.
	 */
	file->private_data = (void *)flags;
	*opened |= FILE_CREATED;

	put_fname(dentry, tmp);
	dput(res);
	return err;

pao_free_vmfd:
	if (VMAPI_DIRECT) {
		(void)vmapi_file_close(fd);
	} else {
		re.op = REQ_FILE_CLOSE;
		re.fd = fd;
		pikeosfs_execute_req(fsi, &re);
		(void)re.err;
	}
pao_free_myfd:
	myfd_close_finish(dentry, flags);
pao_free_name:
	put_fname(dentry, tmp);
pao_exit:
	dput(res);
	return err;
}

static int pikeosfs_unlink(struct inode *dir, struct dentry *dentry)
{
	struct inode *inode = dentry->d_inode;
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct filename *tmp;
	struct req_entry re;
	int err;

	tmp = get_fname(dentry);
	if (IS_ERR(tmp))
		return PTR_ERR(tmp);

	/*
	 * Notice:
	 *  Deleting of open file is allowed, because myfd in the dentry will
	 *  be kept until file is open.
	 */

	if (VMAPI_DIRECT) {
		err = vmapi_unlink(tmp->name);
	} else {
		re.op = REQ_UNLINK;
		re.s1 = tmp->name;
		pikeosfs_execute_req(fsi, &re);
		err = re.err;
	}
	if ((err == 0) || (err == -ENOENT)) {
		drop_nlink(inode);
	}

	put_fname(dentry, tmp);

	return err;
}

static int pikeosfs_rmdir(struct inode *dir, struct dentry *dentry)
{
	struct inode *inode = dentry->d_inode;
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct filename *tmp;
	struct req_entry re;
	int err;

	tmp = get_fname(dentry);
	if (IS_ERR(tmp))
		return PTR_ERR(tmp);

	if (VMAPI_DIRECT) {
		err = vmapi_dir_remove(tmp->name);
	} else {
		re.op = REQ_DIR_REMOVE;
		re.s1 = tmp->name;
		pikeosfs_execute_req(fsi, &re);
		err = re.err;
	}
	if ((err == 0) || (err == -ENOENT)) {
		drop_nlink(inode);
	}

	put_fname(dentry, tmp);

	return err;
}

static int pikeosfs_rename(struct inode *old_dir, struct dentry *old_dentry,
		struct inode *new_dir, struct dentry *new_dentry)
{
	struct super_block *sb = old_dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct myfd *old_mf;
	struct myfd *new_mf;
	int they_are_dirs = S_ISDIR(old_dentry->d_inode->i_mode);
	struct filename *old_tmp;
	struct filename *new_tmp;
	int err;
	struct req_entry re;

	old_tmp = get_fname(old_dentry);
	if (IS_ERR(old_tmp))
		return PTR_ERR(old_tmp);

	new_tmp = get_fname(new_dentry);
	if (IS_ERR(new_tmp)) {
		final_putname(old_tmp);
		return PTR_ERR(new_tmp);
	}

	/*
	 * Notice:
	 *  Rename of open file, open dir or dir with open subentries is not
	 *  allowed. It would require better management of myfd data.
	 *  For example if 'a' and 'b' are open files and renaming 'a' to 'b'
	 *  would result in two different myfd in the single dentry.
	 *  Also myfd in the old dentry needs to be kept (or redirected),
	 *  because files that are already open have link only to original dentry.
	 */

	mutex_lock(&fsi->myfd_mutex);

	err = 0;
	old_mf = old_dentry->d_fsdata;
	if (old_mf &&
			(!hlist_empty(&old_mf->hl_fd_children) ||
			 (old_mf->rd.refs != 0) ||
			 (old_mf->rw.refs != 0)))
		err = -EBUSY;
	new_mf = new_dentry->d_fsdata;
	if (new_mf &&
			(!hlist_empty(&new_mf->hl_fd_children) ||
			 (new_mf->rd.refs != 0) ||
			 (new_mf->rw.refs != 0)))
		err = -EBUSY;
	if (err == 0) {
		if (VMAPI_DIRECT) {
			err = vmapi_rename(old_tmp->name, new_tmp->name);
		} else {
			re.op = REQ_RENAME;
			re.s1 = old_tmp->name;
			re.s2 = new_tmp->name;
			pikeosfs_execute_req(fsi, &re);
			err = re.err;
		}
	}
	if (err == 0) {
		if (new_dentry->d_inode) {
			if (they_are_dirs) {
#ifdef USE_LINKS
				drop_nlink(new_dentry->d_inode);
				drop_nlink(old_dir);
#endif
			}
		} else if (they_are_dirs) {
#ifdef USE_LINKS
			/* rename in same dir (for example) */
			drop_nlink(old_dir);
			inc_nlink(new_dir);
#endif
		}
	}

	mutex_unlock(&fsi->myfd_mutex);

	final_putname(old_tmp);
	final_putname(new_tmp);

	return err;
}

static const struct inode_operations pikeosfs_dir_inode_operations = {
	.atomic_open	= pikeosfs_atomic_open,
	.create		= NULL,
	.lookup		= pikeosfs_lookup,
	.unlink		= pikeosfs_unlink,
	.mkdir		= pikeosfs_mkdir,
	.rmdir		= pikeosfs_rmdir,
	.mknod		= NULL,
	.rename		= pikeosfs_rename,
};

/*
 * Notice:
 *   This will not be called if dentry_delete or drop_inode returns 1.
 */
static int pikeosfs_dentry_revalidate(struct dentry *dentry, unsigned int flags)
{
	return 1;
}

/*
 * Notice:
 *   If d_delete is not defined, then all dentries are kept in cache and
 *   before further use they are revalidated.
 */
static int pikeosfs_dentry_delete(const struct dentry *dentry)
{
	struct myfd *mf;

	mf = dentry->d_fsdata;
	if (mf != NULL) {
		/* do not delete used dentries */
		if (mf->rd.refs != 0 || mf->rw.refs != 0)
			return 0;
		if (!hlist_empty(&mf->hl_fd_children))
			return 0;
	}

	return 1;
}

static void pikeosfs_dentry_release(struct dentry *dentry)
{
	struct myfd *mf;

	mf = dentry->d_fsdata;
	if (mf != NULL) {
		if (mf->rd.refs != 0 || mf->rw.refs != 0)
			pr_err("d_release: pikeos data in dentry is still in use!!!\n");
		if (!hlist_empty(&mf->hl_fd_children))
			pr_err("d_release: dentry children still exist!!!\n");
		hlist_del_init(&mf->hl_fd_entry);
		kfree(mf);
		dentry->d_fsdata = NULL;
	}
}

static const struct dentry_operations pikeosfs_dentry_ops = {
	.d_revalidate = pikeosfs_dentry_revalidate,
	.d_delete = pikeosfs_dentry_delete,
	.d_release = pikeosfs_dentry_release,
};

static int pikeosfs_statfs(struct dentry *dentry, struct kstatfs *buf)
{
	struct super_block *sb = dentry->d_sb;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct vmapi_statvfs statvfs;
	struct req_entry re;
	int err;

	if (VMAPI_DIRECT) {
		err = vmapi_statvfs(fsi->volname, &statvfs);
	} else {
		re.op = REQ_STATVFS;
		re.s1 = fsi->volname;
		re.statvfs = &statvfs;
		pikeosfs_execute_req(fsi, &re);
		err = re.err;
	}
	if (err)
		return err;

	buf->f_type = sb->s_magic;
	buf->f_bsize = statvfs.f_bsize;
	buf->f_frsize = statvfs.max_atomic_size;
	buf->f_blocks = statvfs.f_blocks;
	buf->f_bfree = statvfs.f_bfree;
	buf->f_bavail = statvfs.f_bfree;
	buf->f_files = statvfs.f_files;
	buf->f_ffree = statvfs.f_ffree;
	buf->f_namelen = statvfs.f_namemax;
	buf->f_fsid.val[0] = (u32)crc32_le(0, fsi->volname,
			fsi->volname_len/2);
	buf->f_fsid.val[1] = (u32)crc32_le(0, fsi->volname,
			fsi->volname_len);

	return 0;
}

static inline int pikeosfs_drop_inode(struct inode *inode)
{
	struct myfd *mf;
	struct dentry *dentry;
	struct hlist_node *tmp;

	hlist_for_each_entry_safe(dentry, tmp, &inode->i_dentry, d_alias) {
		mf = dentry->d_fsdata;
		if (mf && (!hlist_empty(&mf->hl_fd_children) ||
					mf->rd.refs != 0 || mf->rw.refs != 0)) {
			return 0;
		}
	}

	return generic_drop_inode(inode);
}

/* queue filesystem request for execution and wait until is completed */
static void pikeosfs_execute_req(struct pikeosfs_fs_info *fsi,
		struct req_entry *re)
{
	init_completion(&re->req_complete);
	/* serialize access to req_pos_empty */
	spin_lock(&fsi->req_pos_empty_lock);
	if (fsi->req_pos_empty != fsi->req_pos_completed) {
		fsi->reqs[fsi->req_pos_empty] = re;
		fsi->req_pos_empty = REQ_POS_ADVANCE(fsi, req_pos_empty);
		spin_unlock(&fsi->req_pos_empty_lock);
		wake_irq_thread(&fsi->irq_sleep);
		wait_for_completion(&re->req_complete);
	} else {
		spin_unlock(&fsi->req_pos_empty_lock);
		/* set error into both output arguments */
		re->err = -EAGAIN;
		re->off = -EAGAIN;
	}
}

/* filesystem worker running in the PikeOS context */
static void pikeosfs_worker(unsigned int irq, void *dev)
{
	struct pikeosfs_fs_info *fsi = dev;
	struct req_entry *re;
	int next;

	for (;;) {
		next = REQ_POS_ADVANCE(fsi, req_pos_executed);
		barrier();
		while (next == fsi->req_pos_empty) {
			/* wait on request for execution */
			sleep_in_irq_thread(&fsi->irq_sleep);
			barrier();
		}
		re = fsi->reqs[next];
		switch (re->op) {
		case REQ_MOUNT:
			re->err = vmapi_mount(re->s1, re->ui);
			break;
		case REQ_UMOUNT:
			re->err = vmapi_umount(re->s1);
			break;
		case REQ_FILE_STAT:
			re->err = vmapi_file_stat(re->s1, re->file_stat);
			break;
		case REQ_FILE_OPEN:
			re->err = vmapi_file_open(re->s1, re->ui, 0);
			break;
		case REQ_FILE_CLOSE:
			re->err = vmapi_file_close(re->fd);
			break;
		case REQ_FILE_READ_AT:
			re->off = vmapi_file_read_at(re->fd, re->b, re->sz, re->off);
			break;
		case REQ_FILE_WRITE_AT:
			re->off = vmapi_file_write_at(re->fd, re->b, re->sz, re->off);
			break;
		case REQ_FILE_FTRUNCATE:
			re->err = vmapi_file_ftruncate(re->fd, re->off);
			break;
		case REQ_FILE_SIZE_LSEEK:
			re->off = vmapi_file_lseek(re->fd, 0, SEEK_END);
			break;
		case REQ_FILE_FSTAT:
			re->err = vmapi_file_fstat(re->fd, re->file_stat);
			break;
		case REQ_FILE_FSYNC:
			re->err = vmapi_file_fsync(re->fd);
			break;
		case REQ_DIR_OPEN:
			re->err = vmapi_dir_open(re->s1);
			break;
		case REQ_DIR_CLOSE:
			re->err = vmapi_dir_close(re->fd);
			break;
		case REQ_DIR_READ_AT:
			re->err = vmapi_dir_read_at(re->fd, &re->off, re->dirent);
			break;
		case REQ_DIR_CREATE:
			re->err = vmapi_dir_create(re->s1);
			break;
		case REQ_DIR_REMOVE:
			re->err = vmapi_dir_remove(re->s1);
			break;
		case REQ_DIR_SYNC:
			re->err = vmapi_dir_sync(re->fd);
			break;
		case REQ_UNLINK:
			re->err = vmapi_unlink(re->s1);
			break;
		case REQ_RENAME:
			re->err = vmapi_rename(re->s1, re->s2);
			break;
		case REQ_STATVFS:
			re->err = vmapi_statvfs(re->s1, re->statvfs);
			break;
		default:
			/* set error into both output arguments */
			re->err = -EBADRQC;
			re->off = -EBADRQC;
			break;
		}
		fsi->req_pos_executed = next;
		barrier();
		/* request result is ready, call the IRQ handler */
		do_IRQ(irq);
	}
}

/* wakes up all completed requests on the interrupt */
static irqreturn_t pikeosfs_irq(int irq, void *dev)
{
	struct pikeosfs_fs_info *fsi = dev;
	int next;
	struct completion *c;

	spin_lock(&fsi->req_pos_completed_lock);
	while (fsi->req_pos_completed != fsi->req_pos_executed) {
		next = REQ_POS_ADVANCE(fsi, req_pos_completed);
		c = &fsi->reqs[next]->req_complete;
		/* free position of current request in the queue */
		fsi->req_pos_completed = next;
		spin_unlock(&fsi->req_pos_completed_lock);

		/* wakeup waiting one */
		complete(c);

		spin_lock(&fsi->req_pos_completed_lock);
	}
	spin_unlock(&fsi->req_pos_completed_lock);

	return IRQ_HANDLED;
}

static const struct super_operations pikeosfs_super_ops = {
	.statfs		= pikeosfs_statfs,
	.drop_inode	= pikeosfs_drop_inode,
	.show_options	= generic_show_options,
};

enum {
	Opt_ro,
	Opt_file_mode,
	Opt_dir_mode,
	Opt_reqmax,
	Opt_err
};

static const match_table_t tokens = {
	{Opt_ro, "ro"},
	{Opt_file_mode, "file_mode=%o"},
	{Opt_dir_mode, "dir_mode=%o"},
	{Opt_reqmax, "reqmax=%o"},
	{Opt_err, NULL}
};

static int pikeosfs_parse_options(char *data, struct pikeosfs_mount_opts *opts)
{
	substring_t args[MAX_OPT_ARGS];
	int option;
	int token;
	char *p;

	opts->ro = false;
	opts->file_mode = PIKEOSFS_DEFAULT_FILE_MODE;
	opts->dir_mode = PIKEOSFS_DEFAULT_DIR_MODE;
	opts->reqmax = -1;
	
	while ((p = strsep(&data, ",")) != NULL) {
		if (!*p)
			continue;

		token = match_token(p, tokens, args);
		switch (token) {
		case Opt_ro:
			opts->ro = true;
			break;
		case Opt_file_mode:
			if (match_octal(&args[0], &option))
				return -EINVAL;
			opts->file_mode = option & S_IRWXUGO;
			break;
		case Opt_dir_mode:
			if (match_octal(&args[0], &option))
				return -EINVAL;
			opts->dir_mode = option & S_IRWXUGO;
			break;
		case Opt_reqmax:
			if (match_int(&args[0], &option))
				return -EINVAL;
			opts->reqmax = option;
			break;
		default:
			printk(KERN_ERR "pikeosfs: unsupported mount option %s\n", p);
			return -EINVAL;
		}
	}

	if (opts->reqmax < 1) {
		printk(KERN_ERR "pikeosfs: missing mount option reqmax >= 1\n");
		return -EINVAL;
	}

	return 0;
}

static int pikeosfs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *inode;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;

	strncpy(sb->s_id, fsi->volname, sizeof(sb->s_id));
	/* Notice: PikeOS Volume provider does not provide maximum file size yet */
	sb->s_maxbytes		= MAX_LFS_FILESIZE;
	sb->s_blocksize		= PAGE_CACHE_SIZE;
	sb->s_blocksize_bits	= PAGE_CACHE_SHIFT;
	sb->s_magic		= PIKEOSFS_SUPER_MAGIC;
	sb->s_op		= &pikeosfs_super_ops;
	sb->s_d_op		= &pikeosfs_dentry_ops;
	sb->s_time_gran		= 1;

	inode = pikeosfs_get_inode(sb, NULL, S_IFDIR | fsi->mount_opts.dir_mode, 0);
	if (IS_ERR(inode))
		return PTR_ERR(inode);

	sb->s_root = d_make_root(inode);
	if (!sb->s_root)
		return -ENOMEM;

	sb->s_root->d_fsdata = get_new_mf(sb->s_root);
	if (!sb->s_root->d_fsdata)
		return -ENOMEM;

	return 0;
}

static struct dentry *pikeosfs_mount(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *data)
{
	int error;
	struct super_block *s;
	struct pikeosfs_fs_info *fsi;
	struct req_entry re;
	char *voldel;
	unsigned int mount_flags;
	char tname[VMAPI_NAME_LEN + 16];

	voldel = strchr(dev_name, ':');
	if ((voldel == NULL) || (voldel == dev_name) || (voldel[1] != 0)) {
		/* allow only volume name with double colon as an argument */
		error = -EINVAL;
		goto err;
	}

	fsi = kzalloc(sizeof(struct pikeosfs_fs_info), GFP_KERNEL);
	if (fsi == NULL) {
		error = -ENOMEM;
		goto err;
	}

	error = pikeosfs_parse_options(data, &fsi->mount_opts);
	if (error)
		goto err_free_fsi;

	fsi->volname = kstrndup(dev_name, voldel - dev_name + 1, GFP_KERNEL);
	if (fsi->volname == NULL) {
		error = -ENOMEM;
		goto err_free_fsi;
	}
	fsi->volname_len = strlen(fsi->volname);

	mutex_init(&fsi->myfd_mutex);

	fsi->reqs = kmalloc((fsi->mount_opts.reqmax + 1) *
							sizeof(struct req_entry),
						GFP_KERNEL);
	if (!fsi->reqs) {
		error = -ENOMEM;
		goto err_free_name;
	}
	fsi->req_pos_completed = 0;
	fsi->req_pos_executed = 0;
	fsi->req_pos_empty = 1;
	spin_lock_init(&fsi->req_pos_completed_lock);
	spin_lock_init(&fsi->req_pos_empty_lock);

	/* startup PikeOS worker thread */
	fsi->irq = get_free_async_irq();
	if (!fsi->irq) {
		error = -EAGAIN;
		printk(KERN_ERR "pikeosfs: unable to get free irqs\n");
		goto err_free_name;
	}

	snprintf(tname, sizeof(tname), "pfs_irq_%s", fsi->volname);
	error = request_irq(fsi->irq, pikeosfs_irq, 0, tname, fsi);
	if (error) {
		printk(KERN_ERR "pikeosfs: unable to request read-IRQ %d\n", fsi->irq);
		goto err_release_async_irq;
	}

	snprintf(tname, sizeof(tname), "pfs_wrk_%s", fsi->volname);
	error = enable_irq_thread(fsi->irq, tname, pikeosfs_worker, fsi);
	if (error) {
		printk(KERN_ERR "pikeosfs: could not start async IRQ thread\n");
		goto err_free_irq;
	}

	mount_flags = VMAPI_O_R;
	if (!fsi->mount_opts.ro)
		mount_flags |= VMAPI_O_W | VMAPI_O_CREAT | VMAPI_O_EXCL;

	if (VMAPI_DIRECT) {
		error = vmapi_mount(fsi->volname, mount_flags);
	} else {
		re.op = REQ_MOUNT;
		re.s1 = fsi->volname;
		re.ui = mount_flags;
		pikeosfs_execute_req(fsi, &re);
		error = re.err;
	}
	if (error)
		goto err_disable_worker;

	s = sget(fs_type, NULL, set_anon_super, flags, NULL);
	if (IS_ERR(s)) {
		error = PTR_ERR(s);
		goto err_disable_worker;
	}

	s->s_fs_info = fsi;
	save_mount_options(s, data);

	error = pikeosfs_fill_super(s, data, flags & MS_SILENT ? 1 : 0);
	if (error) {
		deactivate_locked_super(s);
		goto err_disable_worker;
	}
	s->s_flags |= MS_ACTIVE;
	return dget(s->s_root);

err_disable_worker:
	/* make sure that the worker thread is sleeping */
	while (!uk_uevent_is_waiting(&fsi->irq_sleep))
		cpu_relax();
	disable_irq_thread(fsi->irq);
err_free_irq:
	free_irq(fsi->irq, fsi);
err_release_async_irq:
	release_async_irq(fsi->irq);
err_free_name:
	kfree(fsi->volname);
err_free_fsi:
	kfree(fsi);
err:
	return ERR_PTR(error);
}

static void pikeosfs_kill_sb(struct super_block *sb)
{
	int err;
	struct pikeosfs_fs_info *fsi = sb->s_fs_info;
	struct req_entry re;

	if (VMAPI_DIRECT) {
		err = vmapi_umount(fsi->volname);
	} else {
		re.op = REQ_UMOUNT;
		re.s1 = fsi->volname;
		pikeosfs_execute_req(fsi, &re);
		err = re.err;
	}
	if (err)
		pr_err("pikeosfs_umount: err=%d\n", err);

	/* make sure that the worker thread is sleeping */
	while (!uk_uevent_is_waiting(&fsi->irq_sleep))
		cpu_relax();
	disable_irq_thread(fsi->irq);
	free_irq(fsi->irq, fsi);
	release_async_irq(fsi->irq);
	kill_anon_super(sb);
	kfree(fsi->volname);
	kfree(fsi);
}

static struct file_system_type pikeosfs_fs_type = {
	.name		= "pikeosfs",
	.mount		= pikeosfs_mount,
	.kill_sb	= pikeosfs_kill_sb,
	.fs_flags	= FS_USERNS_MOUNT,
};

int __init init_pikeosfs_fs(void)
{
	static unsigned long once;
	int err;

	if (test_and_set_bit(0, &once))
		return 0;

	err = register_filesystem(&pikeosfs_fs_type);

	return err;
}
module_init(init_pikeosfs_fs)
