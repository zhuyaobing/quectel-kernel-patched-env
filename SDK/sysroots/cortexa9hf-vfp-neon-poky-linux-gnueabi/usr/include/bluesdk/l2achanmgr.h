/****************************************************************************
 *
 * File:
 *     l2achanmgr.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Definition of the API used by the stack to access a system-specific
 *     L2capAgent Preference Manager.
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

#ifndef __L2ACHANMGR_H
#define __L2ACHANMGR_H

#include "bttypes2.h"
#include "l2capagent.h"

#if NUM_AMPS > 0
/*---------------------------------------------------------------------------
 * L2capAgent Channel Manager API layer
 *
 *     An L2capAgent Channel Manager is a system-specific entity that is called 
 *     by L2capAgents when channels are created, disconnected and moved. The
 *     Channel Manager is given the ability to change the L2capAgent preferences
 *     and L2CAP connection settings used when a channel is created. 
 * 
 *     The core concept behind the L2capAgent Channel Manager is matching
 *     a "Service ID" stored in the L2capAgent with a preference and setting 
 *     information stored in the Channel Manager. The L2capAgent Channel 
 *     Manager is a system-specific module and therefore, the "Service ID" is 
 *     also system-specific. The idea is that profiles may have specific 
 *     preferences and/or settings when running on a specific device and the 
 *     device may allow users the ability to set preferences and/or settings 
 *     for individual profiles or globally. 
 *
 *     The L2capAgent Channel Manager API is defined in such a way as to
 *     allow maximum flexibility in defining the features and capabilities
 *     of the Channel Manager. The "Service ID" is defined as a void *
 *     pointer (BtServiceId in file bttypes2.h) giving it the flexibility to
 *     point to a complex structure or be a simple value.
 *
 *     A Channel Manager registers a handler with the stack and receives 
 *     events. The stack assumes there is only one Channel Manager. The 
 *     interaction between L2capAgents and the Channel Manager allows the 
 *     Channel Manager to perform elaborate schemes such as evaluating existing 
 *     channels along with the new channel and moving existing channels around 
 *     to make room for the new channel when a new channel is being created. 
 *     It also allows the Channel Manager to track changes to the channels. 
 *     The details of an L2capAgent Channel Manager's behavior are left to 
 *     the specific implementation.
 *
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * L2ACHANMGR_MOVE_THRESHOLD_SIZE constant
 *     The minimum size of a data transfer to be considered for an AMP
 *     controller. The MOVE_THRESHOLD_SIZE is specified in bytes.
 */
#ifndef L2ACHANMGR_MOVE_THRESHOLD_SIZE
#define L2ACHANMGR_MOVE_THRESHOLD_SIZE  10000
#endif /* L2ACHANMGR_MOVE_THRESHOLD_SIZE */

/*---------------------------------------------------------------------------
 * L2ACHANMGR_MOVE_THRESHOLD_RATE constant
 *     The minimum rate of a data transfer to be considered for an AMP
 *     controller. The MOVE_THRESHOLD_RATE is specified in bits per second.
 */
#ifndef L2ACHANMGR_MOVE_THRESHOLD_RATE
#define L2ACHANMGR_MOVE_THRESHOLD_RATE  1000000
#endif /* L2ACHANMGR_MOVE_THRESHOLD_RATE */


/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * L2aChanMgrEventType type
 *
 *     The L2capAgent Channel Manager event types are passed to the registered
 *     Channel Manager. They are offsets from BTE_TYPE_BASE_CHANMGR 
 *     defined in bttypes2.h
 */
typedef BtEventTypeEx L2aChanMgrEventType;

/* Group: Events sent to the registered Channel Manager. */

/* An L2capAgent is in the process of performing a Connect Request operation.
 * The security is verified and in place but the L2capAgent has not yet 
 * determined which physical link to use. The "p.req" field contains a pointer 
 * to the L2capAgent and the preferences and settings being used. The Channel 
 * Manager must call agent->i.connectReqChanMgr to cause the Connect Request 
 * operation to continue. See il2capagent.h for information on the 
 * connectReqChanMgr function.
 */
#define L2ACHANMGR_CONNECT_REQ      (BTE_TYPE_BASE_CHANMGR + 0)

/* An L2CAP channel has been created. This event occurs both for incoming channels
 * as well as those initiated by the local device. The "p.desc" field contains
 * the L2capChannelDesc for the channel. Note that the profile/application is
 * sent the L2CAP event corresponding to the channel creation before the Channel
 * Manager receives the event.
 */
#define L2ACHANMGR_CONNECTED        (BTE_TYPE_BASE_CHANMGR + 1)

/* An L2CAP channel has been disconnected or a connect request has failed. 
 * The "p.desc" field contains the L2capChannelDesc for the channel. The 
 * p.desc.agent identifies the agent, p.desc.mode specifies the mode that 
 * was in affect, and p.desc.efsPair specifies the flow spec pair that was 
 * in affect for the channel. Note that the profile/application is sent
 * the L2CAP event corresponding to the channel disconnect before the Channel
 * Manager receives the event.
 */
#define L2ACHANMGR_DISCONNECTED     (BTE_TYPE_BASE_CHANMGR + 2)

/* An L2CAP channel move attempt has completed. The "p.desc" field contains the 
 * L2capChannelDesc for the channel and the "e.status" field indicates the
 * status of the move (success or failure). Note that the profile/application is
 * sent the L2CAP event corresponding to the channel move before the Channel
 * Manager receives the event.
 */
#define L2ACHANMGR_MOVED            (BTE_TYPE_BASE_CHANMGR + 3)

/* The physical link for channel is in the process of being disconnected due to
 * link supervision timeout. The Channel manager is given a chance to move
 * the channel to another physical link or the channel will be disconnected.
 * The channel manager must initiate the move using agent->i.moveReq() before 
 * returning from the callback or the channel will be disconnected. The p.agent 
 * field contains a pointer to the L2capAgent for the channel. 
 */
#define L2ACHANMGR_AUTOMOVE         (BTE_TYPE_BASE_CHANMGR + 4)

/* L2CAP Agent periodically indicates this event while the channel is
 * transferring data. The channel manager may decide to move the channel
 * to a different physical controller within this event using
 * agent->i.moveReq(). The p.data.agent field contains a pointer to the
 * L2capAgent for the channel. The p.data.len field contains the length of
 * data transferred since the last event.
 */
#define L2ACHANMGR_TX_DATA_LEN       (BTE_TYPE_BASE_CHANMGR + 5)

/* End of L2aChanMgrEventType */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * L2aChanMgrEvent structure
 *
 *     Used to pass information to the L2capAgent Channel Manager. Not
 *     all fields are valid at any given time. The type of event determines 
 *     which fields are valid. This structure inherits from BtEventEx. 
 */
typedef struct _L2aChanMgrEvent {
    BtEventEx       e;

    union {
        /* This parameter is used for the L2ACHANMGR_CONNECT_REQ event */
        struct {
            L2capAgent          *agent;
            
            /* Settings agent is currently using */
            L2capConnSettings    settings;

            /* Preferences the agent is currently using. */
            ControllerPreference pref;
        } req;

        /* This parameter is used for the L2ACHANMGR_AUTOMOVE event */
        L2capAgent         *agent;

        /* This parameter is used for the L2ACHANMGR_TX_DATA_LEN event */
        struct {
            L2capAgent     *agent;

            /* Total length of data transferred since last event */
            U32             len;
        } data;

        /* This parameter is used for all events except the 
         * L2ACHANMGR_CONNECT_REQ and L2ACHANMGR_AUTOMOVE events
         */
        L2capChannelDesc    desc;
    } p;

} L2aChanMgrEvent;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * L2ACHANMGR_Register()
 *
 *     Register an L2capAgent Channel Manager handler with the stack. The
 *     handler will be called with events of type L2aChanMgrEvent when 
 *     channels are created, disconnected and moved. 
 *
 * Parameters:
 *     handler - pointer to handler.
 *
 * Returns:
 *     old handler or 0 if a handler was not previously registered.
 */
BtHandlerEx *L2ACHANMGR_Register(BtHandlerEx *handler);

#endif /* NUM_AMPS > 0 */
#endif /* __L2APREFMGR_H */ 
