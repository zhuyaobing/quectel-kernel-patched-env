/***************************************************************************
 *
 * File:
 *     eventmgr.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Types, defines, and prototypes for OS access to the Event Manager.
 *
 * Copyright 1999-2017 OpenSynergy GmbH.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of OpenSynergy GmbH.
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of OpenSynergy GmbH.
 *
 * Use of this work is governed by a license granted by OpenSynergy GmbH. 
 * This work contains confidential and proprietary information of
 * OpenSynergy GmbH. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __EVENTMGR_H
#define __EVENTMGR_H

#include "osapi.h"
#include "utils.h"
#if BT_STACK == XA_ENABLED
#include "bttypes.h"
#endif /* BT_STACK == XA_ENABLED */

#if BT_STACK == XA_DISABLED
/* If the Bluetooth stack is not enabled, this module is responsible
 * for its own priority settings and defines.
 */
typedef U8 BtPriority;

#define BT_PRIORITY_SYSTEM              0x01        /* System only */
#define BT_PRIORITY_SIGNAL              0x02        /* 1st priority */
#define BT_PRIORITY_GUARANTEED          0x03        /* 2nd priority */
#define BT_PRIORITY_BEST_EFFORT         0x04        /* 3rd priority */

#define EVM_PROCESS_MAX_ITERATIONS      5
#define EVM_MAX_GUARANTEED_ITERATIONS   1
#endif /* BT_STACK == XA_DISABLED */

#if (IRDA_STACK == XA_ENABLED) || (BT_STACK == XA_ENABLED) || (XA_EVENTMGR == XA_ENABLED)

/*---------------------------------------------------------------------------
 * Event Manager API layer
 *
 *     The Event Manager provides a thread of execution to all stack
 *     components and manages stack initialization and deinitialization. Refer
 *     to the documentation in each function for specific instructions
 *     on how it interacts with the operating system integration layer
 *     (OS API).
 */

/* Forward reference of EvmTask.
 */
typedef struct _EvmTask EvmTask;

/* Forward reference of EvmTimer.
 */
typedef struct _EvmTimer EvmTimer;

/*---------------------------------------------------------------------------
 * Function prototype for tasks scheduled using EVM_ScheduleTask. Task
 * should perform work as quickly as possible and return. If further
 * processing needs to be done, the task can reschedule itself for
 * continued processing on the same stack context after other tasks
 * have been given a chance to execute. Only work of the specified
 * priority (task->priority) or higher should be processed during the
 * callback.
 */
typedef void (*EvmTaskFunc)(EvmTask *task);

/*---------------------------------------------------------------------------
 * Callback used to notify a stack component that the timer expired.
 */
typedef void (*EvmTimerNotify)(EvmTimer*);

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * EvmTask structure
 *
 *     Represents a stack task.  This structure is for use by
 *     internal stack components only.
 */
struct _EvmTask {

    ListEntry node; /* Used internally by EVM. */
    ListEntry rnode; /* Used internally by HCI. */

    /* The function to invoke when it is time for this task to run. */
    EvmTaskFunc func;

    /* Task-specific context value, not used by EVM. */
    void *context;

    /* All scheduled tasks at a given priority will execute to completion before tasks of a
     * lower priority are given a chance to run.
     */
    BtPriority priority;
};


/*---------------------------------------------------------------------------
 * EvmTimer structure
 *
 *     Represents a stack timer. This structure is for use by
 *     internal stack components only.
 */
struct _EvmTimer
{
    ListEntry        node;     /* Used internally by the Event Manager */
    void            *context;  /* Context area for use by callers */
    EvmTimerNotify   func;     /* Function to call when timer fires */

    /* === Internal use only === */
    TimeT            time;      /* Amount of time to wait */
    TimeT            startTime; /* System time when the timer started */
};


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * EVM_Init()
 *
 *     Called by the OS to initializes the event manager and the stack.
 *     This must be called before the first call to EVM_Process.
 *
 * Returns:
 *     TRUE - Initialization succeeded.
 *
 *     FALSE - Initialization failed.
 */
BOOL EVM_Init(void);


/*---------------------------------------------------------------------------
 * EVM_Deinit()
 *
 *     Called by the OS to deinitialize the event manager and the stack.
 *     Transports are unbound, but not deinitialized when this call
 *     is made.  If it is necessary to deinitialize the transport and/or
 *     the radio module (via HCI commands), then RMGR_RadioShutdown() should
 *     be called prior to calling OS_Deinit().
 *
 *     Do not call EVM_Process after calling this function.
 */
void EVM_Deinit(void);


/*---------------------------------------------------------------------------
 * EVM_Process()
 *
 *     Called by the OS to give a thread of execution to the stack.
 *     Internally, this function calls OS_LockStack and OS_UnlockStack
 *     to protect the stack from reentrancy.
 */
void EVM_Process(void);


/*---------------------------------------------------------------------------
 * EVM_ScheduleTask()
 *
 *     Schedules a task, or updates a previously scheduled task, for
 *     execution during the next stack context.  This function can
 *     be called from stack context or from a high priority hardware
 *     thread/interrupt context.  Scheduled tasks will be run on the
 *     stack thread.
 *
 *     If the specified task is already scheduled, then the
 *     scheduled task will remain scheduled at the same priority,
 *     unless a higher priority is specified.  In the case that a
 *     higher priority is specified, the task will be moved to the
 *     end of the higher priority queue.
 *
 * Parameters:
 *     task - Pointer to a task structure.  The caller must
 *         initialize the 'func' and 'priority' fields, and may also
 *         initialize the 'context' field.
 */
void EVM_ScheduleTask(EvmTask *task);


/*---------------------------------------------------------------------------
 * EVM_CancelTask()
 *
 *     Cancels a previously scheduled task.  This function can be
 *     called from stack context or interrupt context.
 *
 * Parameters:
 *     task - Pointer to a task structure that was previously scheduled.
 */
void EVM_CancelTask(EvmTask *task);


/* Prototypes for EVM_StartTimer EVM_CancelTimer macros */
void EvmStartTimer(EvmTimer* timer, TimeT ticks);
void EvmCancelTimer(EvmTimer* timer);

/*---------------------------------------------------------------------------
 * EVM_StartTimer()
 *
 *     Called by stack layers to start a timer. After the specified
 *     amount of time elapses, the timer fires by calling the timer's
 *     callback function. The timer fires only once for each call to
 *     EVM_StartTimer().
 *
 *     If the timer was already running, this function resets it with a
 *     new time period.
 *
 *     Internally, timers are managed by calling OS_GetSystemTime during
 *     stack execution. When XA_MULTITASKING is enabled, this function
 *     uses OS_StartTimer, which maps to a single system timer.
 *
 * Parameters:
 *     timer - Pointer to a timer structure. The caller must
 *         initialize the func field, and may also initialize the context
 *         field if desired. When the timer fires, timer.func is called
 *         during stack execution. NOTE: The timer.func will typically check
 *         for Hold, Park or Sniff mode with ME_GetCurrentMode when called
 *         and handle these modes differently (usually restarting the timer
 *         is sufficient.)
 *
 *     ms - Number of milliseconds to wait before firing. This value
 *         is converted to system ticks using the MS_TO_TICKS macro.
 */
void EVM_StartTimer(EvmTimer* timer, TimeT ms);
#define EVM_StartTimer(timer, ms) EvmStartTimer(timer, MS_TO_TICKS(ms))

/*---------------------------------------------------------------------------
 * EVM_CancelTimer()
 *
 *     Called by the stack to cancel a timer. This prevents the timer
 *     from firing.
 *
 * Parameters:
 *     timer - Pointer to a timer structure. The timer must have been
 *         started by EVM_StartTimer and must not have fired or
 *         been cancelled already.
 */
void EVM_CancelTimer(EvmTimer* timer);
#define EVM_CancelTimer(timer) EvmCancelTimer(timer)

/*---------------------------------------------------------------------------
 * EVM_ScheduleTimer()
 *
 *     Called by the stack to cause the stack to be scheduled and this
 *     timer to fire immediately. This call may be made outside of stack
 *     context.
 *
 * Parameters:
 *     timer - Pointer to a timer structure. The timer must have been
 *         started by EVM_StartTimer and must not have fired or
 *         been cancelled.
 */
void EVM_ScheduleTimer(EvmTimer *timer);

#endif /* (IRDA_STACK == XA_ENABLED) || (BT_STACK == XA_ENABLED) || (XA_EVENTMGR == XA_ENABLED) */

#endif /* __EVENTMGR_H */


