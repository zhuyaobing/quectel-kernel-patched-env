/****************************************************************************
 *
 * File:
 *     l2signal.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Public types, defines, and prototypes for L2CAP Connectionless.
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

#ifndef __L2SIGNAL_H
#define __L2SIGNAL_H

#include "bttypes2.h"

/* -- L2CAP Signaling Context Structure ------------------------------------------------
 * 
 * This structure provides the context and state information for the signaling channel.
 */
typedef struct _BtL2SignalContext {
    void           *channel;
    U8              iClient;
    BtHandlerEx     signalHandler;
} BtL2SignalContext;


void L2SIGNAL_Create(BtL2SignalContext *ctx);
void L2SIGNAL_InitFixedChannelClient(void *Client);

#endif /* __L2SIGNAL_H */
