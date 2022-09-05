#ifndef __P4_MMU_CONTEXT_H
#define __P4_MMU_CONTEXT_H

struct mm_struct;
struct task_struct;
#include <asm-generic/mm_hooks.h>

#define get_mmu_context(task)	do { } while(0)
#define activate_context(tsk)	do { } while(0)

#define deactivate_mm(tsk, mm)	do { } while (0)

extern int init_new_context(struct task_struct *task, struct mm_struct *mm);

extern void destroy_context(struct mm_struct *mm);

extern void switch_mm(struct mm_struct *prev, struct mm_struct *next,
		struct task_struct *tsk);

extern void activate_mm(struct mm_struct *old, struct mm_struct *new);

extern void enter_lazy_tlb(struct mm_struct *mm, struct task_struct *tsk);

/* FIXME: SMP: non-standard */
extern void flush_context(void);

/* special for overcommit mode, used internally */
extern void flush_for_kmem(void);

/* x86 specific hooks for IO port handling */
extern int ioport_get_mm_state(struct mm_struct *mm);
extern void ioport_update_mm_state(struct mm_struct *mm);

#endif
