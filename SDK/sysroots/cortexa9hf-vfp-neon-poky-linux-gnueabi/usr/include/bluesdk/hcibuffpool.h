/****************************************************************************
 *
 * File:
 *     hcibuffpool.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Public types, defines, and prototypes for HciBufferPool.
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

#ifndef __HBPOOL_H
#define __HBPOOL_H

#include "sys/hci.h"
#include "bttypes2.h"

/*---------------------------------------------------------------------------
 * HCI Buffer Pool component
 *
 *     The HciBufferPool class provides the ability manage a pool of HciBuffer
 *     structures used to hold received packets from a Controller. 
 *
 *      Note that L2CAP reserves blocks of HciBuffer structures for Enhanced
 *      channels by calling RXBUFF_ReserveBlock(). In order for the block
 *      reservations to work correctly HCI Buffer Pools used by AMP
 *      controllers must provide the same number of HciBuffer structures
 *      that are provided by RXBUFF for Enhanced channels. The proper number
 *      is HCI_NUM_ACL_BUFFERS_ENHANCED + NUM_BT_DEVICES.
 *
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HBPoolEventType type
 *
 *     The HciBufferPool event types are defined below. They
 *     are offsets from BTE_TYPE_BASE_HBPOOL define in bttypes2.h.
 */
typedef BtEventTypeEx HBPoolEventType;

/** An HciBuffer has become available to be allocated. This event is
 * sent whenever an HciBuffer is returned to the pool (freed).  
 */
#define HBPOOLE_BUFF_AVAIL       (BTE_TYPE_BASE_HBPOOL + 0) 

/* End of HBPoolEventType type */

/*---------------------------------------------------------------------------
 * HciBuffAccessFunc type
 *
 *     Prototype of function used to gain and release exclusive access
 *     to buffer pool when freeing an HciBuffer. ctx is a pointer
 *     to the object managing the pool. For example the AmpController
 *     object or AMP transport depending on which object manages the pool.
 *     The ctx pointer is passed in the handler (handler->context).
 *
 */
typedef void (*HciBuffAccessFunc)(void *ctx);
/* End of HciBuffAccessFunc */


/*---------------------------------------------------------------------------
 * HciBuffDynamicAllocate type
 *
 *     Prototype of function used to dynamically allocate memory for 
 *     HCI buffers. Returns 0 if the allocation could not be made.
 */
typedef void *(*HciBuffDynamicAllocate)(void *ctx, U32 size);
/* End of HciBuffDynamicAllocate */

/*---------------------------------------------------------------------------
 * HciBuffDynamicFree type
 *
 *     Prototype of function used to dynamically free memory previously
 *     allocated by HciBuffDynamicAllocate.
 */
typedef void (*HciBuffDynamicFree)(void *ctx, void *toFree);
/* End of HciBuffDynamicFree */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HciBufferAllocation structure
 * 
 *     Holds the memory that will be managed by an HciBufferPool. It has
 *     fields defining expected buffer sizes, and either pointers to the
 *     actual memory or to functions that allocate memory as needed.
 */
typedef struct _HciBufferAllocation {
    /* Number of HciBuffer structures in the hciBuffArray or 0 if using
     * dynamic allocation.
     */
    U16        num; 

    /* Size of the "buffer" field of each HciBuffer in the array.
     * If using dynamic allocation this must contain the maximum buffer
     * size that will ever be allocated.
     */
    U32        size;

    /* HciBuffer array. Memory allocated must be num * sizeof(HciBuffer)
     * in size. Ignored if "num" is zero.
     */
    HciBuffer *hciBuffArray; 
 
    /* Pointer to buffer from which "buffer" field will be allocated this
     * memory must be num * size octets. Ignored if "num" is zero.
     */
    U8        *dataPtr; 

    /* Function to call to allocate memory dynamically. This function
     * is only used if "num" is 0.
     */
    HciBuffDynamicAllocate alloc;
    
    /* Function to call to free previously allocated memory. This function
     * is only used if "num" is 0.
     */
    HciBuffDynamicFree free;

} HciBufferAllocation;

/*---------------------------------------------------------------------------
 * HciBufferPool structure
 * 
 *     Class for managing a pool of HciBuffer structures. Its contents
 *     are only for use by HBPOOL_ functions.
 */
typedef struct _HciBufferPool {
    /* Pointer to structure containing HciBuffers to be managed */
    HciBufferAllocation  hbAlloc;

    /* List of free HciBuffer structures */
    ListEntry            freeList;

    /* Pointer to handler to call whenever an HciBuffer is returned to the pool
     * via a call to HBPOOL_Free or HciBuffer->freeBuff().
     */
    BtHandlerEx         *handler;

    /* Indicates if stop/resume function will be called. Both have to be set to
     * non-0 value.
     */
    BOOL useHardware;

    /* Function to stop hardware when the HciBuffer free function is called */
    HciBuffAccessFunc    stopHardware;

    /* Function to resume hardware after the HciBuffer free function has
     * freed the buffer
     */
    HciBuffAccessFunc    resumeHardware;

    /* Number of buffers outstanding (allocated but not freed) */
    U32 outCount;

} HciBufferPool;

/****************************************************************************
 *
 * Function Reference 
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HBPOOL_Init()
 *
 *     Initialize the HciBufferPool instance. 
 *
 * Parameters:
 *    ctx - Pointer to an uninitialized HciBufferPool instance
 *
 *    hbAlloc - Pointer to an HciBufferAllocation describing how to get
 *        memory. Must be filled out as specified.
 *
 *    stopHardware - pointer to function that gains exclusive access
 *        to the buffer pool during a call HciBuffer::free(). If this is 0 
 *        then no protection of the buffer pool is provided during the call
 *        to HciBuffer->freeBuff().
 *
 *    resumeHardware - pointer to function that releases the exclusive access
 *        to the buffer pool during the call to HciBuffer->freeBuff(). If this
 *        is 0 then no protection of the buffer pool is provided during the
 *        call to HciBuffer->freeBuff().
 *
 *    handler - pointer to handler that will be called when an HciBuffer is 
 *        returned to the pool either by a call to HBPOOL_Free() or when 
 *        HciBuffer->freeBuff() is called. The handler is called with 
 *        the HBPOOLE_BUFF_AVAIL event and the hardware in the stopped state.
 *        The handler is not put on a list so if an implementation needs to
 *        manage more than one pool and wants the same callback to be used
 *        it can pass the same handler to both pools.
 *
 * Returns:
 *    BT_STATUS_SUCCESS - Initialization completed successfully. 
 *
 *    BT_STATUS_FAILED - Initialization failed because ctx and/or hballoc was 
 *        0 (XA_ERROR_CHECK only). 
 */
BtStatus HBPOOL_Init(HciBufferPool *ctx, HciBufferAllocation *hbAlloc,
                     HciBuffAccessFunc stopHardware, 
                     HciBuffAccessFunc resumeHardware,
                     BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * HBPOOL_Deinit()
 *
 *     Deinitialize the HciBufferPool instance, releasing all allocated
 *     resources, and returning success. 
 *
 *     This function is only necessary for dynamically-allocated pools
 *     (see HciBufferAllocation.num). In the statically-allocated case,
 *     there is nothing to release.
 *
 * Parameters:
 *    ctx - Pointer to the previously-initialized pool.
 *
 * Returns:
 *    BT_STATUS_BUSY - One or more buffers have been allocated, but have
 *        not yet been freed. HBPOOL_Init() should be called again after
 *        those buffers have been released.
 *
 *    BT_STATUS_SUCCESS - All resources have been freed and the pool is
 *        deinitialized.
 */
BtStatus HBPOOL_Deinit(HciBufferPool *ctx);

/*---------------------------------------------------------------------------
 * HBPOOL_SetHandler()
 *
 *     Set a handler that will be called when an HciBuffer is returned to
 *     the pool either by a call to HBPOOL_Free() or when HciBuffer->freeBuff() 
 *     is called. The handler will be called with the HBPOOLE_BUFF_AVAIL event
 *     and the hardware in the stopped state. This function should be called 
 *     while the hardware is in the stopped state.
 *
 * Parameters:
 *    ctx - Pointer to the HciBufferPool instance
 *
 *    handler - Pointer to handler or 0 if no event is desired.
 *
 * Returns:
 *    Pointer to old handler or 0 if no previous handler has been set.
 */
BtHandlerEx *HBPOOL_SetHandler(HciBufferPool *ctx, BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * HBPOOL_Alloc()
 *
 *     Allocate an HciBuffer from the pool. If no HciBuffers are available then
 *     return 0. The HciBuffer fields "hbpool" and "free" are set appropriately
 *     and the "flags" field is set to 0. The "len" field is set to len. This 
 *     function must be called with the hardware in the stopped state. 
 *
 * Parameters:
 *    ctx - Pointer to the HciBufferPool instance
 *
 *    len - Length of buffer to allocate. Note len cannot exceed the size
 *        set in the HciBufferAllocation (hballoc->size) passed in
 *        HBPOOL_Init().
 *
 * Returns:
 *    Pointer to an HciBuffer or 0 if none are available. 
 */
HciBuffer *HBPOOL_Alloc(HciBufferPool *ctx, U16 len);

/*---------------------------------------------------------------------------
 * HBPOOL_Free()
 *
 *     Return the HciBuffer to the pool. If the HciBuffer is already in the pool
 *     then nothing is done. Note that the handler will be called before this 
 *     function returns if the handler is non-0 and the HciBuffer is actually
 *     returned. Also the functions stopHardware and resumeHardware are not 
 *     called. This function must be called with the hardware in the stopped 
 *     state and assumes the caller has already stopped the hardware.
 *
 * Parameters:
 *    ctx - Pointer to the HciBufferPool instance
 *
 *    hciBuff - pointer to HciBuffer to free.
 *
 * Returns:
 *    void 
 */
void HBPOOL_Free(HciBufferPool *ctx, HciBuffer *hciBuff);

#endif /* __HBPOOL_H */
