/*
 * Copyright (C) 2002 - 2007 Jeff Dike (jdike@{addtoit,linux.intel}.com)
 * Licensed under the GPL
 *
 * ported from linux/arch/um/kernel/skas/uaccess.c
 * 20060606, azu@sysgo.de
 * 20110621, azu@sysgo.de: sync'd with UM of Linux 2.6.34
 */

#include <linux/err.h>
#include <linux/highmem.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <asm/current.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/uaccess.h>
#include <asm/internals.h>
#include <asm/cacheflush.h>

static
pte_t *virt_to_pte(struct mm_struct *mm, unsigned long addr)
{
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;

	if (mm == NULL)
		return NULL;

	pgd = pgd_offset(mm, addr);
	if (!pgd_present(*pgd))
		return NULL;

	pud = pud_offset(pgd, addr);
	if (!pud_present(*pud))
		return NULL;

	pmd = pmd_offset(pud, addr);
	if (!pmd_present(*pmd))
		return NULL;

	return pte_offset_kernel(pmd, addr);
}

static pte_t *maybe_map(unsigned long virt, int is_write)
{
	pte_t *pte = virt_to_pte(current->mm, virt);
	int err, dummy_code;

	if ((pte == NULL) || !pte_present(*pte) ||
	    (is_write && !pte_write(*pte))) {
		err = handle_page_fault(virt, is_write ? HOST_PF_WRITE : 0, 1, &dummy_code);
		if (err)
			return NULL;
		pte = virt_to_pte(current->mm, virt);
	}
	if (!pte_present(*pte))
		pte = NULL;

	return pte;
}

static int do_op_one_page(unsigned long addr, int len, int is_write,
		 int (*op)(unsigned long addr, int len, void *arg), void *arg)
{
	struct page *page;
	pte_t *pte;
	int n;

	pte = maybe_map(addr, is_write);
	if (pte == NULL)
		return -1;

	page = pte_page(*pte);
	addr = (unsigned long) kmap_atomic(page) +
		(addr & ~PAGE_MASK);

	n = (*op)(addr, len, arg);

	kunmap_atomic((void *)addr);

	return n;
}

/*
 * map_cont_area() tries to map maximum continuous memory area
 * from the start of specified user address.
 */
unsigned long map_cont_area(unsigned long addr, int len, int is_write, int *map_len)
{
	int size, remain;
	pte_t *pte;
	struct page *page;
	unsigned long maddr_page;
	unsigned long maddr_first;
	unsigned long maddr_last;

	maddr_first = 0;
	
	size = min((addr & PAGE_MASK) + PAGE_SIZE - addr, (unsigned long) len);
	remain = len;

	while (remain > 0) {
		pte = maybe_map(addr, is_write);
		if (pte == NULL)
			break;

		page = pte_page(*pte);
		maddr_page = (unsigned long)kmap(page);

		if (maddr_first == 0) {
			maddr_first = maddr_page + (addr & ~PAGE_MASK);
		} else if (maddr_page != maddr_last + PAGE_SIZE) {
			kunmap((void *)maddr_page);
			break;
		}

		addr += size;
		remain -= size;
		maddr_last = maddr_page;
	}

	if (maddr_first != 0)
		*map_len = size;

	return maddr_first;
}

void unmap_cont_area(unsigned long addr, int len)
{
	unsigned long last;

	last = addr + len - 1;
	addr &= PAGE_MASK;
	while (addr <= last) {
		kunmap((void *)addr);
		addr += PAGE_SIZE;
	}
}

static int buffer_op(unsigned long addr, int len, int is_write,
		     int (*op)(unsigned long, int, void *), void *arg)
{
	int size, remain, n;

	size = min(PAGE_ALIGN(addr) - addr, (unsigned long) len);
	remain = len;

	n = do_op_one_page(addr, size, is_write, op, arg);
	if (n != 0) {
		remain = (n < 0 ? remain : 0);
		goto out;
	}

	addr += size;
	remain -= size;
	if (remain == 0)
		goto out;

	while (addr < ((addr + remain) & PAGE_MASK)) {
		n = do_op_one_page(addr, PAGE_SIZE, is_write, op, arg);
		if (n != 0) {
			remain = (n < 0 ? remain : 0);
			goto out;
		}

		addr += PAGE_SIZE;
		remain -= PAGE_SIZE;
	}
	if (remain == 0)
		goto out;

	n = do_op_one_page(addr, remain, is_write, op, arg);
	if (n != 0) {
		remain = (n < 0 ? remain : 0);
		goto out;
	}

	return 0;
 out:
	return remain;
}

static int copy_chunk_from_user(unsigned long from, int len, void *arg)
{
	unsigned long *to_ptr = arg, to = *to_ptr;

	memcpy((void *) to, (void *) from, len);
	*to_ptr += len;
	return 0;
}

int copy_from_user(void *to, const void __user *from, int n)
{
	if (segment_eq(get_fs(), KERNEL_DS)) {
		memcpy(to, (__force void*)from, n);
		return 0;
	}

	return access_ok(VERIFY_READ, from, n) ?
	       buffer_op((unsigned long) from, n, 0, copy_chunk_from_user, &to):
	       n;
}
EXPORT_SYMBOL(copy_from_user);

static int copy_chunk_to_user(unsigned long to, int len, void *arg)
{
	unsigned long *from_ptr = arg, from = *from_ptr;

	memcpy((void *) to, (void *) from, len);
	*from_ptr += len;
	return 0;
}

int copy_to_user(void __user *to, const void *from, int n)
{
	if (segment_eq(get_fs(), KERNEL_DS)) {
		memcpy((__force void *) to, from, n);
		return 0;
	}

	return access_ok(VERIFY_WRITE, to, n) ?
	       buffer_op((unsigned long) to, n, 1, copy_chunk_to_user, &from) :
	       n;
}
EXPORT_SYMBOL(copy_to_user);

static int strncpy_chunk_from_user(unsigned long from, int len, void *arg)
{
	char **to_ptr = arg, *to = *to_ptr;
	int n;

	strncpy(to, (void *) from, len);
	n = strnlen(to, len);
	*to_ptr += n;

	if (n < len)
	        return 1;
	return 0;
}

int strncpy_from_user(char *dst, const char __user *src, int count)
{
	int n;
	char *ptr = dst;

	if (segment_eq(get_fs(), KERNEL_DS)) {
		strncpy(dst, (__force void *) src, count);
		return strnlen(dst, count);
	}

	if (!access_ok(VERIFY_READ, src, 1))
		return -EFAULT;

	n = buffer_op((unsigned long) src, count, 0, strncpy_chunk_from_user,
		      &ptr);
	if (n != 0)
		return -EFAULT;
	return strnlen(dst, count);
}
EXPORT_SYMBOL(strncpy_from_user);

static int clear_chunk(unsigned long addr, int len, void *unused)
{
	memset((void *) addr, 0, len);
	return 0;
}

int __clear_user(void __user *mem, int len)
{
	return buffer_op((unsigned long) mem, len, 1, clear_chunk, NULL);
}

int clear_user(void __user *mem, int len)
{
	if (segment_eq(get_fs(), KERNEL_DS)) {
		memset((__force void*)mem, 0, len);
		return 0;
	}

	return access_ok(VERIFY_WRITE, mem, len) ?
	       buffer_op((unsigned long) mem, len, 1, clear_chunk, NULL) : len;
}
EXPORT_SYMBOL(clear_user);

static int strnlen_chunk(unsigned long str, int len, void *arg)
{
	int *len_ptr = arg, n;

	n = strnlen((void *) str, len);
	*len_ptr += n;

	if (n < len)
		return 1;
	return 0;
}

int strnlen_user(const void __user *str, int len)
{
	int count = 0, n;

	if (segment_eq(get_fs(), KERNEL_DS))
		return strnlen((__force char*)str, len) + 1;

	n = buffer_op((unsigned long) str, len, 0, strnlen_chunk, &count);
	if (n == 0)
		return count + 1;
	return -EFAULT;
}
EXPORT_SYMBOL(strnlen_user);

/* cache flush handlers for I-cache synchronization */
static int flush_icache_chunk(unsigned long addr, int len, void *arg)
{
	__flush_icache_range(addr, addr + len);
	return 0;
}

void flush_icache_range_user(unsigned long __user start, unsigned long __user end)
{
	int len = end - start;

	if (segment_eq(get_fs(), KERNEL_DS)) {
		__flush_icache_range(start, end);
		return;
	}

	if (access_ok(VERIFY_READ, start, len))
		buffer_op(start, len, 0, flush_icache_chunk, NULL);
}

#ifdef __arm__

/* cache flush handlers for D-cache DMA ops on user space addresses */
static int flush_dcache_chunk(unsigned long addr, int len, void *arg)
{
	__flush_dcache_range(addr, addr + len);
	return 0;
}

void flush_dcache_range_user(unsigned long __user start, unsigned long __user end)
{
	int len = end - start;

	if (segment_eq(get_fs(), KERNEL_DS)) {
		__flush_dcache_range(start, end);
		return;
	}

	if (access_ok(VERIFY_READ, start, len))
		buffer_op(start, len, 0, flush_dcache_chunk, NULL);
}

#endif

/* get in-kernel address of futex variable in current user space */
void *__futex_in_kernel_uaddr(void *uaddr)
{
	unsigned long base, offs;
	struct page *page;
	pte_t *pte;

	if ((unsigned long)uaddr & (sizeof(unsigned long)-1))
		return NULL;	/* alignment error */

	pte = virt_to_pte(current->mm, (unsigned long)uaddr);
	if ((pte == NULL) || !pte_present(*pte) || !pte_write(*pte))
		return NULL;

	page = pte_page(*pte);
	base = (unsigned long)pfn_to_phys(page_to_pfn(page));
	offs = (unsigned long)uaddr & (PAGE_SIZE-1);

	return (void*)(base + offs);
}
