/***************************************************************************
 *
 * File:
 *     ring.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Header file for ring buffer management (ring.c)
 *
 * Created:
 *     February 29, 2000
 *
 * Copyright 2000-2017 OpenSynergy GmbH.
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

#ifndef _RING_H
#define _RING_H

#include "xatypes.h"


/****************************************************************************
 *
 * Ring Buffer Management layer
 *
 * Ring Buffer Management is a collection of utilities for managing 
 * circular - or ring - buffers. It is targeted for those platforms that
 * do not have their own ring buffer libraries.
 *
 ****************************************************************************/


/****************************************************************************
 *
 * Configuration Constants
 *
 ****************************************************************************/



/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/



/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/
                       
/*---------------------------------------------------------------------------
 * RingBuf structure
 *      This structure contains all fields required to manipulate a singe
 *      ring buffer. All fields are private and should not be modified.
 */
typedef struct {
    /* === Internal use only === */
    U8 *start;          /* 1st byte in ring buffer */
    U8 *end;            /* last byte in ring buffer + 1 */
    U8 *data;           /* 1st byte of date to be removed */
    U8 *free;           /* 1st byte of free space */
    U32 dataLen;        /* data bytes currently in buffer */
} RingBuf;



/****************************************************************************
 *
 * Function Prototypes
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * RING_BufInit()
 * Initialize a ring buffer structure. This function must be called before
 * any other ring buffer API.
 *
 * Parameters:
 *      ring - pointer to RingBuf struct
 *      buf  - allocated buffer which will become the ring buffer
 *      len  - length of buf
 */
void RING_BufInit(RingBuf *ring, char buf[], U32 len);


/*---------------------------------------------------------------------------
 * RING_BufFlush() 
 *      Clear data from a ring buffer.
 *
 * Parameters:
 *      Pointer to RingBuf struct.
 */
void RING_BufFlush(RingBuf *ring);


/*---------------------------------------------------------------------------
 * RING_WriteData()
 * Copy data into the ring buffer from caller's buffer.
 * If there is not enough free space to copy the caller's entire buffer,
 * no data at all is copied.
 *
 * Parameters
 *      ring - pointer to RingBuf struct
 *      buf  - caller's destination buffer
 *      len  - max amount to copy
 *  Returns: 
 *          actual amount copied to buf
 */
U32 RING_WriteData(RingBuf *ring, char buf[], U32 len);


/*---------------------------------------------------------------------------
 * RING_ReadData()
 * Move data from the ring buffer to caller's buffer.
 * Once the data is copied, the space in the ring buffer is considered
 * free.
 *
 * Parameters: 
 *      ring - pointer to RingBuf struct
 *      buf  - caller's destination buffer
 *      len  - max amount to copy
 *  Returns: actual amount copied to buf
 */
U32 RING_ReadData(RingBuf *ring, char buf[], U32 len);


/*---------------------------------------------------------------------------
 * RING_Size()
 * Determine the size in bytes of a ring buffer.
 *
 * Parameters:
 *      ring - pointer to RingBuf struct
 *                  
 *  Returns: size in bytes of the buffer
 */
U32 RING_Size(RingBuf *ring);
#define RING_Size(ring)          ((ring)->end - (ring)->start)


/*---------------------------------------------------------------------------
 * RING_FreeSpace()
 * Determine the free space in the ring buffer.
 *
 * Parameters:
 *      ring - pointer to RingBuf struct
 *                  
 *  Returns: 
 *      amount of free space in the buffer
 */
U32 RING_FreeSpace(RingBuf *ring);
#define RING_FreeSpace(ring)     ((U32)(RING_Size(ring) - (ring)->dataLen))


/*---------------------------------------------------------------------------
 * RING_DataLen()
 * Determine length of data in the ring buffer.
 * The value returned reflects the entire length including wrap-around.
 *
 * Parameters:
 *      ring - pointer to RingBuf struct
 *
 *  Returns: 
 *      amount of data in the buffer
 */
U32 RING_DataLen(RingBuf *ring);
#define RING_DataLen(ring)       (ring)->dataLen
    

/*---------------------------------------------------------------------------
 * RING_ContigDataLen()
 * Determine length of contiguous data in the ring buffer.
 *
 * Parameters:
 *      ring - pointer to RingBuf struct
 *
 *  Returns: 
 *      amount of contiguous data in the buffer
 */
U32 RING_ContigDataLen(RingBuf *ring);
#define RING_ContigDataLen(ring) ((ring)->dataLen > (U32)((ring)->end - (ring)->data) \
                                 ? (U32)((ring)->end - (ring)->data) : (ring)->dataLen)

/*---------------------------------------------------------------------------
 * RING_GetDataPtr() 
 *      Get a pointer to the data and the number of bytes. The number of
 *      bytes is for contiguous data only. This function might be used
 *      by a routine that did not have it's own buffer yet wanted access
 *      to data in the ring buffer.
 *
 * Parameters:
 *      ring - pointer to RingBuf struct
 *      data - address of character pointer
 *      len  - address of an int
 *
 * Returns:
 *      *data is updated to point to the beginning of data; *len is updated
 *      with the number of bytes of data. *len is zero if no data is 
 *      available. 
 */
void RING_GetDataPtr(RingBuf *ring, char **data, U32 *len);


/*---------------------------------------------------------------------------
 * RING_BufDelete() 
 *      Deletes bytes from the buffer. Bytes are removed starting with
 *      the oldest or "first in" data.
 *
 * Parameters:
 *      ring - pointer to ring buffer structure
 *      len  - bytes to delete; if len > existing bytes, existing bytes
 *             are removed
 */
void RING_BufDelete(RingBuf *ring, U32 len);


#endif /* _RING_H */
