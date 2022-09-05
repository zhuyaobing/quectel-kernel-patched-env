/****************************************************************************
 *
 * File:
 *     phylink.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     logical link in the Management Entity.
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

#ifndef __PHYLINK_H
#define __PHYLINK_H

#include "me.h"
#include "sys/iphylink.h"
#include "sys/loglink.h"

/*---------------------------------------------------------------------------
 * BtPhysicalLink structure
 *
 *     Manages Physical Link.
 */
struct _BtPhysicalLink
{
    ListEntry       node; /* Used internally by the stack (Controller) */
    iPhysicalLink   ipl;
    iLink           il;
    
    BtRemoteDevice *remDev;
    ListEntry       lLinks;             /* List of Logical Links. */

    /* There is one Logical link per BR/EDR/LE Physical link  and one Best
     * Effort link per AMP physical link
     */
    BtLogicalLink   lLink;

    /* Tx Queues. */
    ListEntry       txSignaling;
    ListEntry       txGuaranteed;
    ListEntry       txBestEffort;

    /* List of hciBuffers for the PDU being received */
    ListEntry       rxHciBuffList;

    /* Priority tokens. */
    BtPriorityToken priSignaling;
    BtPriorityToken priGuaranteed;
    BtPriorityToken priBestEffort;
    BtPriorityToken priIncoming;

    BOOL            okToSend;
    U16             cid;
    U16             rxLenBlh; /* Rx buffer length in Basic L2CAP header */
    U16             rxLen;
    U8              numRxBuffs;
    U8              maxRxBuffs;
    U8              maxRxHeader; /* Used to get data packet header into first RxBuff */

    /* The following is of L2capRxFrag type. If set to L2CAP_FRAG_ENABLED it indicates that
     * current channel is for BASIC mode (including connectionless) and supports fragmentation.
     */
    U8              rxFrag;

    void           *channel;
    HciBuffer      *hciBuff;
    U8              rxState1;

    /* Controller Type cache. */
    BtControllerType    crlType;
};


/****************************************************************************
 *
 * Inteface and Helper Functions for the BtPhysicalLink class
 *
 ****************************************************************************/

/* Redefine this function as a macro */
#define PHYLINK_SetLinkModes(p,l) (p)->remDev->linkModes = l
#define PHYLINK_ExtFeatures(p) (p)->remDev->extendedFeatures
#define PHYLINK_LinkModes(p) (p)->remDev->linkModes
#define PHYLINK_SetExtFeatures(p,f) (p)->remDev->extendedFeatures = f

void PHYLINK_Create(BtPhysicalLink *Plink);
BtStatus PHYLINK_Init(BtPhysicalLink *ctx,BtRemoteDevice *remDev);
void PHYLINK_ReceiveData(BtPhysicalLink *ctx, void *HciBuff, BOOL FirstPacket);
BtPacket *PHYLINK_TxReady(BtPhysicalLink *ctx, BtPriorityToken *PriToken, HciHandle Broadcast);
void PHYLINK_TxDone(BtPhysicalLink *ctx, BtPacket *Packet, BtStatus PacketStatus);
void PHYLINK_Close(BtPhysicalLink *ctx, U8 ErrCode, BOOL closeChannels);
void PHYLINK_CloseChannels(BtPhysicalLink *ctx, U8 ErrCode, BOOL closeFixed);
BtLinkMode PHYLINK_Mode(BtPhysicalLink *ctx);
BtLinkState PHYLINK_State(BtPhysicalLink *ctx);
U16 PHYLINK_AddChannel(BtPhysicalLink *ctx);
U16 PHYLINK_DelChannel(BtPhysicalLink *ctx);
BOOL PHYLINK_IsTxQueue(BtPhysicalLink *ctx);
BOOL PHYLINK_IsPktOnTxQueue(BtPhysicalLink *ctx, BtPacket *Packet);
void PHYLINK_QueueTxPacket(BtPhysicalLink *ctx, BtPacket *Packet);
BtPacket *PHYLINK_DequeueTxPacket(BtPhysicalLink *ctx, U16 ChannelId);
void PHYLINK_RequestToSend(BtPhysicalLink *ctx);
void *PHYLINK_MapToLlink(BtPhysicalLink *ctx, U16 ChannelId, struct _L2CAP_BaseChannel **channel);
BtStatus PHYLINK_SetFlushTimeout(BtPhysicalLink *ctx, U16 ChannelId, U16 FlushTimeout);
void PHYLINK_SetEfsId (BtPhysicalLink *plink, ExtFlowSpec *efs, BOOL bestEffort);
BOOL PHYLINK_UseLockstep(BtPhysicalLink *ctx);
BtLogicalLink *PHYLINK_AllocLink(BtPhysicalLink *Plink, BOOL isBestEffort);
BtStatus PHYLINK_ForceDisconnect(BtPhysicalLink *Plink);
BtStatus PHYLINK_GetIdAndType(BtPhysicalLink *ctx, U8 *pId, BtControllerType *pType);
BOOL PHYLINK_FreeLink(BtPhysicalLink *Plink, BtLogicalLink *Llink);

void PHYLINK_InsertLogLink(BtPhysicalLink *ctx, void *llink);
BOOL PHYLINK_IsKeyChanged(BtPhysicalLink *Plink);
U16 PlinkAppendBufferSafe(BtPhysicalLink *Plink, U8 *buffer, U16 buffLen);


#endif /* __PHYLINK_H */

