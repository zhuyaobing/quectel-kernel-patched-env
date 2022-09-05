/****************************************************************************
 *
 * File:
 *     loglink.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
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

#ifndef __LOGLINK_H
#define __LOGLINK_H

#include "utils.h"
#include "sys/iphylink.h"
#include "extflowspec.h"

/*---------------------------------------------------------------------------
 * BtLogicalLink structure
 *
 *     Manages Logical Link.
 */
typedef struct _BtLogicalLink
{
    ListEntry       node;    /* Used internally by the stack. */
    ListEntry       cNode;   /* Used internally by the controller */

    BtPhysicalLink *pLink;
    ListEntry       channels;
    ListEntry       pending;
    BtLinkState     state;
    BtLinkState     pstate;    /* State used internally by the physical link */
    U16             hciHandle; /* 12-bit handle used by HCI to identify the link */
    U16             refCount;
    U16             flowUsage; /* Used internally by the controller */

#if L2CAP_NUM_ENHANCED_CHANNELS > 0 
    ExtFlowSpecPair efsPair;
    ExtFlowSpecPair origEfsPair;
#endif

} BtLogicalLink;

struct _L2CAP_BaseChannel;

void LOGLINK_Create(BtLogicalLink *Llink);
BtStatus LOGLINK_Init(BtLogicalLink *ctx, BtPhysicalLink *plink);
BOOL LOGLINK_IsLink(BtLogicalLink *ctx, BtLogicalLink *ctx2);
void *LOGLINK_MapCidToChannel(BtLogicalLink *Llink, U16 ChannelId);
void LOGLINK_Close(BtLogicalLink *ctx, BtErrorCode discReason);
void LOGLINK_CloseChannels(BtLogicalLink *ctx, BtErrorCode discReason);
void LOGLINK_QueueTxPacket(BtLogicalLink *ctx, BtPacket *Packet);
U16 LOGLINK_AddChannel(BtLogicalLink *ctx, struct _L2CAP_BaseChannel *Channel);
U16 LOGLINK_DelChannel(BtLogicalLink *ctx, struct _L2CAP_BaseChannel *Channel);
BOOL LOGLINK_GenNewAggregate(BtLogicalLink *Llink);

#endif /* __LOGLINK_H */

