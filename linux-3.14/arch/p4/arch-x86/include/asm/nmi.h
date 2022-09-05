#ifndef _ASM_P4_X86_NMI_H
#define _ASM_P4_X86_NMI_H

/* PikeOS doesn't support non-maskable interrupts */
#define stop_nmi() do { } while (0)
#define restart_nmi() do { } while (0)

#endif
