/****************************************************************************
 *
 * File:
 *     lesignal.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Public types, defines, and prototypes for LE Signaling.
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

#ifndef __LE_SIGNAL_H
#define __LE_SIGNAL_H

#include "bttypes2.h"
#include "sys/l2cap_i.h"

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

BtStatus LESIGNAL_SendConnParmUpdateReq(BtRemoteDevice *link, MeLeUpdateLinkParms *Parms);
BtStatus LESIGNAL_SendConnParmUpdateRsp(MeLeRemoteDevice *remDev, BOOL accept);
void LESIGNAL_SendReject(BtLeSignalContext *ctx, U8 ident, U8 reason);
BtStatus LESIGNAL_WaitForRemoteFeatures(BtRemoteDevice *link, MeLeUpdateLinkParms *Parms);

#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
BtStatus LESIGNAL_SendLeCreditBasedConReq(LEL2CAP_DynamicChannel *newChannel);
BtStatus LESIGNAL_SendLeCreditBasedConRsp(LEL2CAP_DynamicChannel *DChannel);
BtStatus LESIGNAL_SendLeFlowControlCredit(LEL2CAP_DynamicChannel *DChannel, U16 Credits);
BtStatus L2SIGNAL_SendLeCreditBasedDiscReq(LEL2CAP_DynamicChannel *DChannel);
BtStatus L2SIGNAL_SendLeCreditBasedDiscRsp(LEL2CAP_DynamicChannel *DChannel);
#endif /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */

#endif /* __LE_SIGNAL_H */
