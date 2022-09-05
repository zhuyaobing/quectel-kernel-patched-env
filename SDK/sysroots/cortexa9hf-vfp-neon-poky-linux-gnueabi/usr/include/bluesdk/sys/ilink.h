/****************************************************************************
 *
 * File:
 *     ilink.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
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

#ifndef __ILINK_H
#define __ILINK_H

#include "bttypes2.h"
#include "extflowspec.h"
#include "sys/hci.h"

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/* -- -------------------------------------------------------------------
 * LinkEventType
 *
 *     Link Events are indicated to L2CAP for events from Logical link
 *     physical links. 
 */
typedef U32 LinkEventType;

#define LINK_EVENT_LOGLINK_CREATE       (BTE_TYPE_BASE_LINK + 1)
#define LINK_EVENT_LOGLINK_DISC         (BTE_TYPE_BASE_LINK + 2)
#define LINK_EVENT_LOGLINK_MOD_FLOW     (BTE_TYPE_BASE_LINK + 3)
#define LINK_EVENT_LOGLINK_RECEIVE      (BTE_TYPE_BASE_LINK + 4)
#define LINK_EVENT_LOGLINK_TX_DONE      (BTE_TYPE_BASE_LINK + 5)
#define LINK_EVENT_LOGLINK_CH_CLOSE     (BTE_TYPE_BASE_LINK + 6)
#define LINK_EVENT_LOGLINK_ACL_UP       (BTE_TYPE_BASE_LINK + 7)
#define LINK_EVENT_LOGLINK_LE_UP        (BTE_TYPE_BASE_LINK + 8)
#define LINK_EVENT_LOGLINK_LE_UP2       (BTE_TYPE_BASE_LINK + 9)

/* End of LinkEventType */


/*-----------------------------------------------------------------------
 * LinkEvent structure
 *
 *     Provides information to L2CAP about an Link event.
 */  
typedef struct _LinkEvent
{
    /* L2CAP Link event. Always valid. */
    BtEventEx   e;

    HcStatus    errCode;

    /* Reason for the physical link Disconnect. This field is useful only for
     * events LINK_EVENT_LOGLINK_CH_CLOSE and  LINK_EVENT_LOGLINK_DISC 
     * when they are sent with status BT_STATUS_NO_CONNECTION because the phylink
     * went down 
     */
    HcStatus    phyDiscReason;

    void       *pLink;  /* Pointer to a BtPhysicalLink object */
    void       *lLink;  /* Pointer to a BtLogicalLink object */


    union {
        /* Received HCI data: LINK_EVENT_LOGLINK_RECEIVE */
        struct {
            U16        channelId;
            HciBuffer *hciBuff;
            BOOL       firstPacket;
        } hci;

        /* Sent BtPacket: LINK_EVENT_LOGLINK_TX_DONE */
        BtPacket  *packet;

        /* Channel Close: LINK_EVENT_LOGLINK_CH_CLOSE */
        void *channel;

        /* Disconnect reason code: LINK_EVENT_LOGLINK_DISC */
        HcStatus reason;
    } p;

} LinkEvent;

/*---------------------------------------------------------------------------
 * LinkCallback type
 *
 *     A function of this type is called to indicate L2CAP link events to
 *     L2CAP from physical and logical link objects.
 */
typedef void (*LinkCallback)(void *ctx, LinkEvent *Event);

/* End of LinkCallback */


/*---------------------------------------------------------------------------
 * Prototype:     MELINK_RegisterHandler()
 *
 * Descriptions:  Register the handler to receive events from Physical and 
 *                logical links. 
 *
 * Parameters:    Pointer to handler
 *
 * Returns:       Pointer old handler 
 */
BtHandlerEx *MELINK_RegisterHandler(BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * LINK::Mode()
 *
 *     Return the mode (BtLinkMode) of the link.
 *
 * Parameters:
 *     ctx - Pointer to object containing the Link interface
 *
 * Returns:      
 *     BtLinkMode 
 *
 */
typedef BtLinkMode (*BtLinkModeFunc)(void *ctx);

/*---------------------------------------------------------------------------
 * LINK::State()
 *
 *     Return the state (BtLinkState) of the link.
 *
 * Parameters:
 *     ctx - Pointer to object containing the Link interface
 *
 * Returns:      
 *     BtLinkState 
 *
 */
typedef BtLinkState (*BtLinkStateFunc)(void *ctx);

/*---------------------------------------------------------------------------
 * LINK::IsTxQueue()
 *
 *     Returns FALSE if the transmit queue(s) for the link are empty otherwise
 *     it returns TRUE.
 *
 * Parameters:
 *     ctx - Pointer to object containing the Link interface
 *
 * Returns:      
 *     FALSE - transmit queue(s) are empty
 *
 *     TRUE - transmit queue(s) are not empty
 *
 */
typedef BOOL (*BtLinkIsTxQueueFunc)(void *ctx);

/*---------------------------------------------------------------------------
 * LINK::IsPktOnTxQueue()
 *
 *     Return TRUE if Packet is on the link's no the transmit queue otherwise 
 *     return FALSE.
 *
 * Parameters:
 *     ctx - Pointer to object containing the Link interface
 *
 *     Packet - Pointer to transmit packet.
 *
 * Returns:      
 *     TRUE - Packet is on the link's transmit queue
 *
 *     FALSE - Packet is not on the link's transmit queue
 *
 */
typedef BOOL (*BtLinkIsPktOnTxQueueFunc)(void *ctx, BtPacket *Packet);

/*---------------------------------------------------------------------------
 * LINK::QueueTxPacket()
 *
 *     Put Packet onto the link's transmit queue.
 *
 * Parameters:
 *     ctx - Pointer to object containing the Link interface
 *
 *     Packet - Pointer to transmit packet.
 *
 * Returns:      
 *     void 
 *
 */
typedef void (*BtLinkQueueTxPacketFunc)(void *ctx, BtPacket *Packet);

/*---------------------------------------------------------------------------
 * LINK::DequeueTxPacket()
 *
 *     Remove a transmit packet for the L2CAP channel with channel ID
 *     equal to ChannelId from the link's transmit queue. 
 * Parameters:
 *     ctx - Pointer to object containing the Link interface
 *
 *     ChannelId - L2CAP channel ID
 *
 * Returns:      
 *     Pointer to the packet removed or 0 if no packets are on the queue for
 *     the given L2CAP channel.
 *
 */
typedef BtPacket * (*BtLinkDequeueTxPacketFunc)(void *ctx, U16 ChannelId);

/*---------------------------------------------------------------------------
 * LINK::RequestToSend()
 *
 *     Tell the layer below that the link has packets to send.
 *
 * Parameters:
 *     ctx - Pointer to object containing the Link interface
 *
 * Returns:      
 *     void 
 *
 */
typedef void (*BtLinkRequestToSendFunc)(void *ctx);

/*---------------------------------------------------------------------------
 * LINK::SetFlushTimeout()
 *
 *     Set the flush timeout for the L2CAP channel identified by ChannelId on
 *     on the link.
 *
 * Parameters:
 *     ctx - Pointer to object containing the Link interface
 *
 *     ChannId - ID of the L2CAP channel
 *
 *     FlushTimeout - flush timeout value
 *
 * Returns:      
 *     BT_STATUS_PENDING - operation has started successfully
 *
 *     BT_STATUS_IN_USE - operation is deferred because a set flush timeout
 *         operation is in progress
 *
 *     BT_STATUS_NOT_SUPPORTED - operation is not supported by the underlying
 *         controller.
 *
 */
typedef BtStatus (*BtLinkSetFlushTimeoutFunc)(void *ctx, U16 ChannelId, U16 FlushTimeout);


/*---------------------------------------------------------------------------
 * iLink Interface structure
 *
 *     Logical Link Interface.
 */
typedef struct _iLink
{
    BtLinkModeFunc              mode;
    BtLinkStateFunc             state;
    BtLinkIsTxQueueFunc         isTxQueue;
    BtLinkIsPktOnTxQueueFunc    isOnTxQueue;
    BtLinkQueueTxPacketFunc     txQueue;
    BtLinkDequeueTxPacketFunc   txDequeue;
    BtLinkRequestToSendFunc     requestToSend;
    BtLinkSetFlushTimeoutFunc   setFlushTimeout;

} iLink;

#endif /* __ILINK_H */

