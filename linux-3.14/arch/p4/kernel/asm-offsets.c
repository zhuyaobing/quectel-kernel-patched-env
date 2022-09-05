#include <linux/kbuild.h>

#if defined(CONFIG_P4_X86)

#ifdef CONFIG_X86_32
#define __SYSCALL_I386(nr, sym, compat) [nr] = 1,
#else
#define __SYSCALL_64(nr, sym, compat) [nr] = 1,
#define __SYSCALL_COMMON(nr, sym, compat) [nr] = 1,
#ifdef CONFIG_X86_X32_ABI
# define __SYSCALL_X32(nr, sym, compat) [nr] = 1,
#else
# define __SYSCALL_X32(nr, sym, compat) /* nothing */
#endif
#endif

static char syscalls[] = {
#ifdef CONFIG_X86_32
#include <asm/syscalls_32.h>
#else
#include <asm/syscalls_64.h>
#endif
};

/* workaround for a warning with -Wmissing-prototypes */
void foo(void);

void foo(void)
{
	DEFINE(__NR_syscall_max, sizeof(syscalls) - 1);
	DEFINE(NR_syscalls, sizeof(syscalls));
}

#elif defined(CONFIG_P4_PPC)

#include <linux/kernel.h>
#include <asm/vdso_datapage.h>

int main(void)
{
	/* datapage offsets for use by vdso */
	DEFINE(CFG_SYSCALL_MAP32, offsetof(struct vdso_data, syscall_map_32));

	DEFINE(STAMP_XTIME, offsetof(struct vdso_data, stamp_xtime));

	DEFINE(TSPC32_TV_SEC, offsetof(struct timespec, tv_sec));

	/* Other bits used by the vdso */
	DEFINE(CLOCK_REALTIME, CLOCK_REALTIME);
	DEFINE(CLOCK_MONOTONIC, CLOCK_MONOTONIC);
	
	return 0;
}

#endif
