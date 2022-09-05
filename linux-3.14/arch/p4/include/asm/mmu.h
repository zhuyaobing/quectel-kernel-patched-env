#ifndef __P4_MMU_H_
#define __P4_MMU_H_

typedef struct {
	unsigned int taskid;
	void *task;
#ifdef CONFIG_P4_PPC
	unsigned long vdso_base;
#endif
} mm_context_t;

#endif
