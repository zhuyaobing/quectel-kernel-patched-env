/****************************************************************************
 *
 * File:
 *     a2me.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Public types, defines, and prototypes for A2MP.
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

#ifndef __A2ME_H
#define __A2ME_H

#include "ia2me.h"
#include "l2cap.h"
#include "sys/ampcontrol.h"

#define A2ME_MIN_MTU  670
#define A2ME_MAX_CONTROLLERS 256
#define A2ME_AMP_INFO_SIZE   16

typedef struct _BtA2meContext {
    IA2me                  i;
    void                  *channel;
    ListEntry              a2meTxQueue;
    BtHandlerEx            a2meHandler;
    U16                    remoteMtu;          /* Remote devices MTU size */
    U16                    remoteExtMask;      /* Remote extended feature mask */
    U8                     remoteIdent;    /* Identifier recv'd in req */
    U8                     iClient;
    U8                     waitResponse;
    U8                     curControllerId;
    BOOL                   beenDiscovered;     /* TRUE - means discoverRsp sent so change notify allowed */
    BOOL                   discoverPerformed;  /* TRUE - means a discoverRsp has been received */
    BOOL                   notified;           /* TRUE - if a notification packet is outstanding */
    BOOL                   needNotify;         /* TRUE - if notification is needed */
    BOOL                   discoverInProgress;
    BtRemoteControllerList remoteAmps;
#if A2ME_CACHE_REMOTE_AMP_INFO == XA_ENABLED
    A2meRemoteAmpInfo      remoteAmpInfos[A2ME_MAX_REMOTE_AMP_CONTROLLERS];
#endif
    U8                     response[(NUM_AMPS + 1) * 3];
    U8                     notifyRsp[(NUM_AMPS + 1) * 3];

    /* [BLCR750] make sure this will be updated if we allow a larger MTU for A2me
     * We should make a #define for the max A2me MTU
     */
    U8                     ampAssocReq[670];  /* Storage for sending an AMP Assoc in a request */
    U8                     ampAssocRsp[670];  /* Storage for sending an AMP assoc in a response */

    /* [BLCR739]: Create a buffer pool for responses which can include ampAssocRsp, response and infoRsp */
    U8                     infoRsp[A2ME_AMP_INFO_SIZE]; /* Storage for sending AMP Info response */

} BtA2meContext;

void A2ME_Create(BtA2meContext *ctx);
void A2ME_InitFixedChannelClient(void *client);

BtStatus A2ME_DiscoverReq(BtA2meContext *ctx);
BtStatus A2ME_ChangeNotify(BtA2meContext *ctx);
BtStatus A2ME_GetAmpAssocReq(BtA2meContext *ctx, U8 ControllerId);
BtStatus A2ME_GetInfoReq(BtA2meContext *ctx, U8 RemoteControlId);

BtStatus A2ME_CreatePhysicalLinkReq(BtA2meContext *ctx, U8 LocalControlId,
                                    U8 RemoteControlId, U8 *AmpAssoc, U16 AmpAssocLen);
BtStatus A2ME_DisconnectPhysicalLinkReq(BtA2meContext *ctx, U8 LocalControlId,
                                        U8 RemoteControlId);
BtRemoteControllerList *A2ME_GetRemoteAmpControllerList(BtA2meContext *ctx);

#endif /* __A2ME_H */
