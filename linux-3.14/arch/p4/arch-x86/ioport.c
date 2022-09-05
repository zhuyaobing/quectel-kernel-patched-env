/*
 * linux/arch/p4/sys-i386/ioport.c
 *
 * IO-Port emulation for P4/Linux
 *
 * ported from linux/arch/i386/kernel/ioport.c
 *
 * This contains the io-permission bitmap code - written by obz, with changes
 * by Linus.
 *
 * 2004-01-05, azu: Started porting to P4
 */

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/stddef.h>
#include <asm/host_tasks.h>
#include <asm/mmu_context.h>

/* Set EXTENT bits starting at BASE in BITMAP to value TURN_ON. */
static void set_bitmap(unsigned long *bitmap, short base, short extent, int new_value)
{
	int mask;
	unsigned long *bitmap_base = bitmap + (base >> 5);
	unsigned short low_index = base & 0x1f;
	int length = low_index + extent;

	if (low_index != 0) {
		mask = (~0 << low_index);
		if (length < 32)
				mask &= ~(~0 << length);
		if (new_value)
			*bitmap_base++ |= mask;
		else
			*bitmap_base++ &= ~mask;
		length -= 32;
	}

	mask = (new_value ? ~0 : 0);
	while (length >= 32) {
		*bitmap_base++ = mask;
		length -= 32;
	}

	if (length > 0) {
		mask = ~(~0 << length);
		if (new_value)
			*bitmap_base++ |= mask;
		else
			*bitmap_base++ &= ~mask;
	}
}

static void map_bitmap(unsigned int task, unsigned long *bitmap)
{
	unsigned int i, j;

	for (i = 0; i < IO_BITMAP_SIZE; i++)
	{
		for (j = 0; j < 31; j++)
		{
			if (bitmap[i] & (1 << j))
				uk_task_map_ioport(task, i*32+j, 1);
		}
	}
}

/*
 * Behaviour of ioperm() and iopl()
 *
 * On a i386 machine, ioperm() sets up a bitmap for ports 0 to 0x3ff,
 * while iopl() just enables access to all ports.
 * Both are methods are orthogonal to each other, but
 * an iopl-level greather than 0 supersedes settings done in the
 * port-bitmap by ioperm().
 *
 * On the microkernel, we cannot simply set the io-permission-level.
 * Instead we map all ports to the client, like we do in ioperm().
 *
 * So we need to maintain a seperate bitmap for ioperm() ...
 *
 * Access to ports even the linux kernel cannot access are emulated:
 * port READs are emulated to "0xff", port WRITEs are skipped.
 */


/*
 * ioperm()
 *
 * inherited through execve() -> no problem
 */
int sys_ioperm(unsigned long from, unsigned long num, int turn_on)
{
	struct thread_struct *t = &current->thread;

	if ((from + num <= from) || (from + num > 0x400))
		return -EINVAL;
	if (turn_on && !capable(CAP_SYS_RAWIO))
		return -EPERM;

	/* now set or clear the relevant bits ... */
	set_bitmap(t->arch.io_bitmap, from, num, turn_on);

	/* iopl() supersedes ioperm(), no need to enable a port if iopl > 0 */
	if (t->arch.iopl == 0)
	{
		if(turn_on)
			uk_task_map_ioport(current->mm->context.taskid, from, num);
		else
			uk_task_unmap_ioport(current->mm->context.taskid, from, num);

		ioport_update_mm_state(current->mm);
	}

	return 0;
}

/*
 * iopl(level)
 *
 * level==0: "no access to IO/Ports at all"
 * level>0:  "ability to access all IO/Ports"
 *
 * inherited through execve() (OK) and fork() (problem ...)
 */
int sys_iopl(int level)
{
	struct thread_struct *t = &current->thread;

	if (level < 0 || level > 3)
		return -EINVAL;
	/* Trying to gain more privileges? */
	if (level > t->arch.iopl) {
		if (!capable(CAP_SYS_RAWIO))
			return -EPERM;
	}
	t->arch.iopl = level;

	if (level > 0)
	{
		uk_task_map_ioport(current->mm->context.taskid, 0, 0x10000);
	}
	else
	{
		/* restore mapping from bitmap (0-0x400), unmap remaining ports */
		uk_task_unmap_ioport(current->mm->context.taskid, 0, 0x10000);
		map_bitmap(current->mm->context.taskid, t->arch.io_bitmap);
	}

	ioport_update_mm_state(current->mm);

	return 0;
}

