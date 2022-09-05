#ifndef __P4_SMP_H
#define __P4_SMP_H

#ifndef __ASSEMBLY__

#ifdef CONFIG_SMP

#include <linux/bitops.h>
#include <linux/cpumask.h>
#include <asm/current.h>

#define raw_smp_processor_id()  (current_thread_info()->cpu)	/* Linux way */
#define hard_smp_processor_id() (kthread_cpu())		/* PikeOS native way */

extern void arch_send_call_function_single_ipi(int cpu);
extern void arch_send_call_function_ipi_mask(const struct cpumask *mask);

extern void local_timer_setup(unsigned int cpu);

#ifdef CONFIG_HOTPLUG_CPU
extern int __cpu_disable(void);
extern void __cpu_die(unsigned int cpu);
extern void cpu_die(void);
#endif

#else

#define hard_smp_processor_id() 0

#endif

#endif /* __ASSEMBLY__ */

#endif
