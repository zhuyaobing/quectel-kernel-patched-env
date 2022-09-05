/***************************************************************************
 *
 * File:
 *     rxbuff.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file contains receive buffer structures for the HCI transport.
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

#ifndef __RXBUFF_H
#define __RXBUFF_H

#include "bttypes.h"
#include "hcitrans.h"

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*-------------------------------------------------------------------------
 * TRANS_MAX_ACL_BUFF_SIZE constant
 *
 *     Defines the amount of memory to be allocated by the transport layer
 *     for each ACL receive buffer.
 *
 *     This value is based on HCI_ACL_DATA_SIZE. It should not be modified
 *     directly.
 */
#define TRANS_MAX_ACL_BUFF_SIZE  (HCI_ACL_DATA_SIZE + 4)

/*-------------------------------------------------------------------------
 * TRANS_NUM_ACL_BUFFERS constant
 * 
 *     Defines the number of buffers to be allocated by the transport
 *     layer for receiving ACL data.
 *
 *     This value is based on HCI_NUM_ACL_BUFFERS and should not be modified
 *     directly.
 */
#define TRANS_NUM_ACL_BUFFERS  HCI_NUM_ACL_BUFFERS

/*-------------------------------------------------------------------------
 * TRANS_MAX_SCO_BUFF_SIZE constant
 *
 *     Defines the amount of memory to be allocated by the transport layer
 *     for each SCO receive buffer.
 *
 *     This value is based on HCI_SCO_DATA_SIZE and should not be modified
 *     directly.
 */
#define TRANS_MAX_SCO_BUFF_SIZE  (HCI_SCO_DATA_SIZE + 3)

/*-------------------------------------------------------------------------
 * TRANS_NUM_SCO_BUFFERS constant
 *
 *     Defines the number of buffers to be allocated by the transport layer
 *     for receiving SCO data.
 *
 *     This value is based on HCI_NUM_SCO_BUFFERS and should not be modified
 *     directly.
 */
#define TRANS_NUM_SCO_BUFFERS  HCI_NUM_SCO_BUFFERS

/*-------------------------------------------------------------------------
 * TRANS_EVENT_BUFF_SIZE constant
 *
 *     Defines the amount of memory to be allocated by the transport layer
 *     for each HCI event buffer.
 */
#define TRANS_EVENT_BUFF_SIZE  257     /* 255 bytes of data + 2 byte header */

/*-------------------------------------------------------------------------
 * RXBUFF_TOTAL_BLOCK_SIZE constant
 *
 *     Defines the total number of HCI ACL buffers that can be reserved
 *     using RXBUFF_ReserveBlock(). 
 */
#define RXBUFF_TOTAL_BLOCK_SIZE (HCI_NUM_ACL_BUFFERS_ENHANCED + HCI_NUM_ACL_BUFFERS_LE)

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/* Receive buffer context
 * 
 * Contains receive buffers and management lists. 
 */
typedef struct _BtRxbuffContext {
    
    /* Lists and Queues */
#if NUM_SCO_CONNS != 0
    ListEntry   rxSCOFreeList;
#endif /* NUM_SCO_CONNS != 0 */
    ListEntry   rxACLFreeList;
    ListEntry   rxEvtFreeList;

    /* RX buff structures */
#if (NUM_SCO_CONNS != 0) && (TRANS_NUM_SCO_BUFFERS > 0)
    HciBuffer rxSCOBuffers[TRANS_NUM_SCO_BUFFERS];  
#endif /* (NUM_SCO_CONNS != 0) && (TRANS_NUM_SCO_BUFFERS > 0) */
    HciBuffer rxACLBuffers[TRANS_NUM_ACL_BUFFERS];  
    HciBuffer rxEvtBuffers[HCI_NUM_EVENTS];  

    /* Allocated memory for Rx data */
#if (NUM_SCO_CONNS != 0) && (TRANS_NUM_SCO_BUFFERS > 0)
    U8  rxSCOBuffer[TRANS_NUM_SCO_BUFFERS * TRANS_MAX_SCO_BUFF_SIZE]; 
#endif /* (NUM_SCO_CONNS != 0) && (TRANS_NUM_SCO_BUFFERS > 0) */
    U8  rxACLBuffer[TRANS_NUM_ACL_BUFFERS * TRANS_MAX_ACL_BUFF_SIZE]; 
    U8  rxEvtBuffer[HCI_NUM_EVENTS * TRANS_EVENT_BUFF_SIZE];

#if ((L2CAP_NUM_ENHANCED_CHANNELS > 0) || (LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0))
    ListEntry blockList;
    U32       numBlocksUsed;
#endif /* ((L2CAP_NUM_ENHANCED_CHANNELS > 0) || (LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0)) */

} BtRxbuffContext;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

#if ((L2CAP_NUM_ENHANCED_CHANNELS > 0) || (LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0))
/*---------------------------------------------------------------------------
 * RXBUFF_ReserveBlock()
 *
 *      Reserve a block of HciBuffer structures. This is intended to be used
 *      by L2CAP to reserve buffers for ERTM and Streaming mode.
 * 
 *      Note that if AMPs are enabled they get HciBuffer structures from
 *      an HCI Buffer Pool and not from RXBUFF. In order for the block
 *      reservations to work correctly HCI Buffer Pools used by AMP
 *      controllers must provide the same number of HciBuffer structures
 *      that are provided by RXBUFF for Enhanced channels. The proper number
 *      is HCI_NUM_ACL_BUFFERS_ENHANCED + NUM_BT_DEVICES.
 *
 * Parameters:
 *      block - Pointer to an HciBufferBlock structure. 
 *
 *      size - Number of HciBuffer structure desired in the block. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Block is reserved successfully. 
 *
 *     BT_STATUS_NO_RESOURCES - unable to reserve block of desired size.
 *         RXBUFF_BlockSizeAvail() can be called to determine the size
 *         available.
 *
 *     BT_STATUS_IN_USE - block is already being used.
 *
 *     BT_STATUS_FAILED - Reservation failed because block and/or size is 0
 *
 */
BtStatus RXBUFF_ReserveBlock(HciBufferBlock *block, U16 size);

/*---------------------------------------------------------------------------
 * RXBUFF_BlockSizeAvail()
 *
 *      Returns the number of HciBuffer structures available to be reserved. 
 *
 * Parameters:
 *      none  
 *
 * Returns:
 *     Number available to be reserved 
 *
 */
U32 RXBUFF_BlockSizeAvail(void);

/*---------------------------------------------------------------------------
 * RXBUFF_FreeBlock()
 *
 *      Free a block of HciBuffer structures. This is intended to be used
 *      by L2CAP to free blocks reserved for ERTM and Streaming mode.
 *
 * Parameters:
 *      block - Pointer to an HciBufferBlock structure. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Block is freed successfully. 
 *
 *     BT_STATUS_NOT_FOUND - Free failed because block is not found.
 *
 *     BT_STATUS_FAILED - Reservation failed because block is 0
 *
 */
BtStatus RXBUFF_FreeBlock(HciBufferBlock *block);

#endif /* ((L2CAP_NUM_ENHANCED_CHANNELS > 0) || (LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0)) */

/* Internal references to buffer allocation routines. */
void RXBUFF_Init(void);
void RXBUFF_Deinit(void);
BtStatus RXBUFF_Alloc(RxBuffHandle *rxBuffhandle, U16 len, U8 type);
void RXBUFF_Free(RxBuffHandle rxBuffhandle);
BtStatus RXBUFF_SetLength(RxBuffHandle rxBuffHandle, U16 len);
void RXBUFF_FreeBuff(HciBuffer *hbuff);

#endif /* __RXBUFF_H */


