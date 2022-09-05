/****************************************************************************
 *
 * File:
 *     l2cap_a.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     upper layer of the Logical Link Control and Adaptation Protocol
 *     using the "new" method.
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

#ifndef __L2CAP_A_H
#define __L2CAP_A_H

#include "bttypes2.h"
#include "l2capagent.h"


/*---------------------------------------------------------------------------
 * L2CAP Extended Events (L2CAPEXT) layer
 *
 *     The L2CAP Extended Events layer defines L2CAP events using the
 *     BtEventEx format. These events report status and connection progress
 *     to the L2capAgent and new entities that use the BtEventEx event
 *     structure format.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * L2capEventTypeEx type
 *
 *     Indicates the type of an L2CAP callback's event during an
 *     L2capCallbackEx function call. These events are delivered in the
 *     L2capEvent structure's "e.type" field. The legacy event parms
 *     is delivered in the L2capEvent structure's "info" field.
 *
 *     Depending on the event, a specific response may be required or
 *     certain fields in the L2capEvent structure may be valid or
 *     invalid. The valid fields and required responses are described
 *     for each event below.
 */
typedef U32 L2capEventTypeEx;

/* A remote device has requested a new L2CAP connection (channel). The
 * L2CAP user must call L2CAP_ConnectRsp to confirm or deny the connection 
 * request. 
 */
#define L2EVENTEX_CONNECT_IND      (BTE_TYPE_BASE_L2CAP + L2EVENT_CONNECT_IND)

/* The channel is connected and ready to exchange data. This event occurs
 *  regardless of which device initiated the connection.
 */
#define L2EVENTEX_CONNECTED        (BTE_TYPE_BASE_L2CAP + L2EVENT_CONNECTED)

/* The remote device requested that its channel be disconnected. L2CAP
 * will respond with a disconnection response, and send the DISCONNECTED 
 * event when the disconnection process is complete. 
 */
#define L2EVENTEX_DISCON_IND       (BTE_TYPE_BASE_L2CAP + L2EVENT_DISCON_IND)

/* The channel is disconnected. The callback's "status" field contains
 * the reason for the disconnection. 
 */
#define L2EVENTEX_DISCONNECTED     (BTE_TYPE_BASE_L2CAP + L2EVENT_DISCONNECTED)

/* Channel data has been received. The callback "ptrs.data" field
 * contains the received data, and "dataLen" indicates the data length. 
 */
#define L2EVENTEX_DATA_IND         (BTE_TYPE_BASE_L2CAP + L2EVENT_DATA_IND)

/* A data packet has been sent. The application may re-use or deallocate
 * the packet in the "ptrs.packet" field. The status of the transmit is 
 * provided in the "status" field of the callback parameters structure.
 */
#define L2EVENTEX_PACKET_HANDLED   (BTE_TYPE_BASE_L2CAP + L2EVENT_PACKET_HANDLED)

/* A request is complete. Different L2CAP operations may generate a
 * COMPLETE event, depending on configuration options. (See L2CAP_Ping, 
 * L2CAP_GetInfo, and L2CAP_ConnectRsp for more information.) 
 *
 * During this event, the "status" field contains the results of the
 * request.
 */
#define L2EVENTEX_COMPLETE         (BTE_TYPE_BASE_L2CAP + L2EVENT_COMPLETE)

/* A Quality-of-Service violation was detected. It is up to the
 * application to take any action based on this event.
 */
#define L2EVENTEX_QOS_VIOLATION    (BTE_TYPE_BASE_L2CAP + L2EVENT_QOS_VIOLATION)

/* The Channel configuration parameters are being negotiated. This event
 * is generated when a connected channel enters renegotiation. No data can
 * be sent while a channel is negotiating its configuration. 
 */
#define L2EVENTEX_CONFIGURING      (BTE_TYPE_BASE_L2CAP + L2EVENT_CONFIGURING)

/* If a channel undergoes configuration renegotiation (as indicated
 * by the L2EVENT_CONFIGURING event) this event will be indicated
 * once the configuration negotiation is complete. This event is roughly
 * equivalent to L2EVENT_CONNECTED, and indicates that data transmission
 * can now be resumed.
 */
#define L2EVENTEX_RECONFIGURED     (BTE_TYPE_BASE_L2CAP + L2EVENT_RECONFIGURED)

/* The Flush Timeout command was sent to HCI. The "flushTimeout" field
 * indicates the flush timeout value used. The BTEVENT_HCI_COMMAND_SENT 
 * event will confirm success or failure of this command. 
 */
#define L2EVENTEX_FLUSH_TIMEOUT    (BTE_TYPE_BASE_L2CAP + L2EVENT_FLUSH_TIMEOUT)

/* A Move Channel operation has completed. This event occurs regardless
 *  of which device initiated the connection. The Event "status" field
 *  indicates the result of the Move Channel operation.
 */
#define L2EVENTEX_MOVE_CHAN_CNF    (BTE_TYPE_BASE_L2CAP + L2EVENT_MOVE_CHAN_CNF)

/* Advises the upper layer of the optimal channel SDU size to achieve
 *  overall system QoS requirements. If the channel is an Enhanced mode
 *  channel, this event notifies the upper layer an automatic MPS size
 *  adjustment has occurred. In this case, the upper layer should adjust
 *  its SDU size to better utilize the current MPS size on the Enhanced
 *  mode channel. If the channel is in Basic Mode, the upper layer must
 *  adjust its SDU size to the size indicated in this event in order to
 *  maintain system QoS. The new SDU/MPS size is indicated in the
 *  "dataLen" field of the event parameters.
 * 
 */
#define L2EVENTEX_ADJUST_SDU_SIZE  (BTE_TYPE_BASE_L2CAP + L2EVENT_ADJUST_SDU_SIZE)

/* Group: The following events occur only on L2CAP Enhanced mode channels
 *  with flowParms.fragmentsEnabled set to TRUE and BASIC mode channels with 
 *  rxFragments set to L2CAP_FRAG_ENABLED in the registered L2capPsm structure.
 *
 */

/* Fragmented channel data has been received. The callback
 * "ptrs.data" field contains the received data, "dataLen" field
 * indicates the data length of this fragment, and "sduLen" 
 * field indicates total length of all fragments. 
 */
#define L2EVENTEX_FRAGMENT_IND     (BTE_TYPE_BASE_L2CAP + L2EVENT_FRAGMENT_IND)

/* The last fragment of fragmented channel data has been
 * received. The callback "ptrs.data" field contains the 
 * received data, "dataLen" field indicates the data length of 
 * this fragment, and "sduLen" field indicates total length of 
 * all fragments. 
 */
#define L2EVENTEX_LAST_FRAG_IND    (BTE_TYPE_BASE_L2CAP + L2EVENT_LAST_FRAG_IND)

#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
/* A credit packet has been received on an LE L2CAP Dyanmic Channel.
 *  The callback "ptrs.rcvdTxCredits" field contains the credit value received.
 */
#define L2EVENTEX_CREDIT_IND       (BTE_TYPE_BASE_L2CAP + L2EVENT_CREDIT_IND)
#endif /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */

/* End of L2capEventTypeEx */

/*---------------------------------------------------------------------------
 * L2capEvent structure
 *
 *     When L2CAP generates events, these events are passed to the registered
 *     callback function. L2capEvent inherits from BtEvent. Parameters are 
 *     based on the event type e.type. 
 *
 */
typedef struct _L2capEvent {
    BtEventEx           e;      /* Inherit */

    /* Completion status, based on the event type. Possible types are
     * L2capDiscReason for L2EVENT_DISCONNECTED events or BtStatus for
     * most others.
     */ 
    U16                 status;

    /* The Channel ID of the L2CAP Channel  */
    L2capChannelId      cid;

    /* Agent is used when this event is passed to the agent's client. The
     * client is identified with its L2capChannelId indicated within the
     * "agent->cid" parameter.
     */
    L2capAgent         *agent;

    /* Provides information to the L2CAP user about an L2capCallback event. */
    L2capCallbackParms *info;

} L2capEvent;

/*---------------------------------------------------------------------------
 * L2capCallbackEx type
 *
 *     A function of this type is called to indicate L2CAP events to
 *     a protocol service. When L2CAP calls this function, the channel 
 *     information is contained in the L2capEvent object passed as a parameter. 
 *
 *     Protocol services provide the callback function's address to
 *     L2CAP by calling the L2CAP_RegisterPsm function.
 */
typedef void (*L2capCallbackEx)(void *ctx, L2capEvent *event);


#endif /* __L2CAP_A_H */
