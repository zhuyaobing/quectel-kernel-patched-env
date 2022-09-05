/***************************************************************************
 *
 * File:
 *     userheap.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     A compact, portable heap manager.  
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

#ifndef __USERHEAP_H__
#define __USERHEAP_H__

#include "osapi.h"

/*---------------------------------------------------------------------------
 * User Heap API layer
 *
 *     Provides behavior for a compact and portable heap manager.  This heap
 *     manager allows the creation of a user-defined heap which can have 
 *     memory blocks allocated, reallocated, and freed as part of this heap.
 *     Additionally, multiple user-defined heaps can be created if needed.
 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * UH_STATISTICS constant
 *
 *     When enabled, collects heap statistics which can be output using
 *     UH_DumpStatistics(). XA_DEBUG must be enabled to use this feature.
 */
#ifndef UH_STATISTICS
#define UH_STATISTICS XA_DEBUG
#endif
#if (UH_STATISTICS == XA_ENABLED) && (XA_DEBUG != XA_ENABLED)
#error "UH_STATISTICS requires XA_DEBUG!"
#endif

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * UH_CreateHeap()
 *
 *     Initializes a heap for use.
 *
 * Parameters:
 *     buffer - A buffer of memory to be used as a user heap. This memory
 *         must not be modified or released while in use.
 *
 *     len - Length of heap.
 */
void UH_CreateHeap(U8 *buffer, U32 len);

/*---------------------------------------------------------------------------
 * UH_Alloc()
 *
 *     Allocates a block of memory from a user heap.
 *
 * Parameters:
 *     heap - Starting pointer for the heap (must have been initialized with
 *         UH_CreateHeap prior to this call).
 *
 *     size - Amount of memory required.
 *
 * Returns:
 *     An allocated block of memory of the specified size. Returns "0" if 
 *     insufficient memory is available.
 */
void *UH_Alloc(U8 *heap, U32 size);
#if XA_DEBUG == XA_ENABLED
#define UH_Alloc(heap, size) UhAlloc(heap, size, __FILE__, __LINE__)
#else /* XA_DEBUG == XA_ENABLED */
#define UH_Alloc(heap, size) UhAlloc(heap, size, 0, 0)
#endif /* XA_DEBUG == XA_ENABLED */


/*---------------------------------------------------------------------------
 * UH_AllocClear()
 *
 *     Allocates a block of memory from a user heap and fills it with 0s.
 *
 * Parameters:
 *     heap - Starting pointer for the heap (must have been initialized with
 *         UH_CreateHeap prior to this call).
 *
 *     size - Amount of memory required.
 *
 * Returns:
 *     An allocated block of memory of the specified size. Returns "0" if 
 *     insufficient memory is available.
 */
void *UH_AllocClear(U8 *heap, U32 size);
#if XA_DEBUG == XA_ENABLED
#define UH_AllocClear(heap, size) UhAllocClear(heap, size, __FILE__, __LINE__)
#else /* XA_DEBUG == XA_ENABLED */
#define UH_AllocClear(heap, size) UhAllocClear(heap, size, 0, 0)
#endif /* XA_DEBUG == XA_ENABLED */

/* Macro for simplifying allocation of a structure by typename*/
#define UH_AllocClearStruct(heap, structName) \
    (structName *)UH_AllocClear(heap, sizeof(structName))

/*---------------------------------------------------------------------------
 * UH_ReAlloc()
 *
 *     Reallocates memory in the user heap. This alters the size of an
 *     allocated block. This function may also modify the location of
 *     the block being realloc'ed if necessary.
 *
 * Parameters:
 *     heap - Starting pointer for the heap (must have been initialized with
 *         UH_CreateHeap prior to this call).
 * 
 *     oldBlock - Pointer to memory block pointer. If successful, the 
 *         reallocated block will contain all of the data contained in
 *         oldBlock (truncated if size is smaller than the previous
 *         allocation). If oldBlock points to 0 then ReAlloc attempts to
 *         allocate an entirely new block. 
 *
 *     size - The new size required for the reallocated block. May be more
 *         than or less than the formerly allocated block's size. If zero,
 *         the oldBlock pointer (if any) is deallocated and set to 0.
 *
 * Returns:
 *     TRUE - oldBlock was successfully reallocated, and now is filled
 *         with a pointer to a memory buffer containing all of the data 
 *         previously pointed to, if any, up to "size" bytes.
 *
 *     FALSE - insufficient memory was available to complete the operation.
 *         and oldBlock is left unmodified.
 *     
 */
BOOL UH_ReAlloc(U8 *heap, void **oldBlock, U32 size);
#if XA_DEBUG == XA_ENABLED
#define UH_ReAlloc(heap, oldBlock, size) UhReAlloc(heap, oldBlock, size, __FILE__, __LINE__)
#else /* XA_DEBUG == XA_ENABLED */
#define UH_ReAlloc(heap, oldBlock, size) UhReAlloc(heap, oldBlock, size, 0, 0)
#endif /* XA_DEBUG == XA_ENABLED */

/*---------------------------------------------------------------------------
 * UH_Free()
 *
 *     Frees a previously allocated memory block.
 *
 * Parameters:
 *     block - A pointer to memory previously indicated by UH_Alloc or
 *         UH_ReAlloc.
 */
void UH_Free(void *block);


#if XA_DEBUG == XA_ENABLED
/*---------------------------------------------------------------------------
 * UH_CountAllocs()
 *
 *     Counts the number of current allocations on the heap.
 *
 * Parameters:
 *     heap - Starting pointer for the heap (must have been initialized with
 *         UH_CreateHeap prior to this call).
 *
 * Returns:
 *     The number of current allocations on the heap or 0 if no allocations
 *     exist.
 */
U32 UH_CountAllocs(U8 *heap);

/*---------------------------------------------------------------------------
 * UH_DumpAllocs()
 *
 *     Prints out complete information about heap contents using Report(),
 *     including allocating file/line and the size of each allocation.
 *
 * Parameters:
 *     heap - Starting pointer for the heap (must have been initialized with
 *         UH_CreateHeap prior to this call).
 */
void UH_DumpAllocs(U8 *heap);

/*---------------------------------------------------------------------------
 * UH_ReportHeap
 *
 *     Dumps the complete contents of the heap.
 *
 * Parameters:
 *     
 *     heap - Starting pointer for the heap (must have been initialized with
 *         UH_CreateHeap prior to this call).
 */
void UH_ReportHeap(U8 *heap);

#endif /* XA_DEBUG == XA_ENABLED */

#if UH_STATISTICS == XA_ENABLED
/*---------------------------------------------------------------------------
 * UH_DumpStatistics()
 *
 *     Report() on usage of the heap.
 *
 * Parameters:
 *     heap - Starting pointer for the heap (must have been initialized with
 *         UH_CreateHeap prior to this call).
 */
void UH_DumpStatistics(U8 *heap);
#endif /* UH_STATISTICS == XA_ENABLED */

void *UhAlloc(U8 *heap, U32 size, const char *file, U16 line);
void *UhAllocClear(U8 *heap, U32 size, const char *file, U16 line);
BOOL UhReAlloc(U8 *heap, void **oldBlock, U32 size, const char *file, U16 line);

#endif /* __USERHEAP_H__ */

