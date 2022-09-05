/****************************************************************************
 *
 * File:
 *     l2capagent_i.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Internal types, defines, and prototypes for accessing the used in 
 *     implementing the L2capAgent.
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

#ifndef __L2CAPAGENT_I_H
#define __L2CAPAGENT_I_H

#include "il2capagent.h"
#include "bttypes2.h"
#include "me.h" /* For BtRemoteDevice */
#include "sys/ampcontrol.h"
#include "sys/phylink.h"
#include "sec.h"

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * L2capAgentState type
 *
 */
typedef U8 L2capAgentState;

#define AGENTSTATE_CLOSED           0
#define AGENTSTATE_WAIT_SECURITY    1
#define AGENTSTATE_FIND_AMP         2
#define AGENTSTATE_WAIT_FIND_AMP    3
#define AGENTSTATE_CREATE_AMP_PHY   4
#define AGENTSTATE_WAIT_AMP_PHY     5
#define AGENTSTATE_CREATE_CHAN      6
#define AGENTSTATE_WAIT_CHANNEL     7
#define AGENTSTATE_CONNECTED        8
#define AGENTSTATE_WAIT_CHAN_RSP    9
#define AGENTSTATE_MOVE_CHAN        10
#define AGENTSTATE_WAIT_MOVE        11
#define AGENTSTATE_MOVE_AMP_PHY     12
#define AGENTSTATE_CREATE_AGAIN     13
#define AGENTSTATE_WAIT_TIMER       14
#define AGENTSTATE_DISC_AMP_PHY     15

/* End of L2capAgentState */

/*---------------------------------------------------------------------------
 * L2capAgentCMState type
 *
 *    Used to track the channel manager. The order matters since the state
 *    is incremented.
 *
 */
typedef U8 L2capAgentCMState;

#define AGENTCMSTATE_CONREQ_START   0
#define AGENTCMSTATE_CONREQ_START2  1
#define AGENTCMSTATE_CONNECTED      2
#define AGENTCMSTATE_AUTOMOVE       3

/* End of L2capAgentCMState */

/*---------------------------------------------------------------------------
 * L2capAgent callback
 *
 *     Used to pass events to the agent
 */

typedef void (*L2capAgentCallback)(L2capAgent *agent, BtEventEx *event);

/*---------------------------------------------------------------------------
 * L2capAgentFactory
 *
 *     Function of this type is called to return L2capAgent objects. The
 *     returned object will be constructed but not initialized.
 */
typedef L2capAgent *(*L2capAgentFactory)(void);


/****************************************************************************
 *
 * Inteface and Helper Functions for the L2capAgent class
 *
 ****************************************************************************/

#if NUM_AMPS > 0 
BtStatus L2CAPAGENT_Init(L2capAgent *ctx, L2capPsm *protocol,
                         const L2capConnSettings *settings, ControllerPreference *pref);
BtStatus L2CAPAGENT_TxDataLen(L2capAgent *ctx, U32 TotalLen);
#else
BtStatus L2CAPAGENT_InitNA(L2capAgent *ctx, L2capPsm *protocol,
                         const L2capConnSettings *settings);
#endif

BtStatus L2CAPAGENT_ConnectReq(L2capAgent *ctx, L2capPsmValue psm, 
                               BtRemoteDevice *RemDev);
BtStatus L2CAPAGENT_ConnectRsp(L2capAgent *ctx, L2capConnStatus status,
                               const L2capConnSettings *settings);
BtStatus L2CAPAGENT_DiscReq(L2capAgent *ctx);
BtStatus L2CAPAGENT_SendData(L2capAgent *ctx, BtPacket *packet);
L2capLinkMode L2CAPAGENT_GetConnectionMode(L2capAgent *ctx);
U16 L2CAPAGENT_GetTxMtu(L2capAgent *ctx);
void L2CAPAGENT_Free(L2capAgent *ctx);
BOOL L2CAPAGENT_IsPhyLinkAssociated(L2capAgent *ctx, AmpPhyLink *plink);
BtServiceId L2CAPAGENT_SetServiceId(L2capAgent *ctx, BtServiceId serviceId);
BtServiceId L2CAPAGENT_GetServiceId(L2capAgent *ctx);

#if NUM_AMPS > 0 
BtStatus L2CAPAGENT_GetPhyLinkDesc(L2capAgent *agent, BtAmpPhyLinkDesc *desc);
BtStatus L2CAPAGENT_ConnectReqChanMgr(L2capAgent *ctx, ControllerPreference *pref, L2capConnSettings *settings);
BtStatus L2CAPAGENT_MoveReq(L2capAgent *ctx, ControllerPreference *pref);
BtStatus L2CAPAGENT_AutoMoveReq(L2capAgent *ctx);
#endif

#if L2CAP_NUM_ENHANCED_CHANNELS > 0 
BtStatus L2CAPAGENT_SetLocalBusy(L2capAgent *ctx, BOOL Set);
#endif

#endif /* __L2CAPAGENT_I_H  */
