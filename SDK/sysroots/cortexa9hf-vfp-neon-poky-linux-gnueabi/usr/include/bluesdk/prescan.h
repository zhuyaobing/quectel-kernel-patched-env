/****************************************************************************
 *
 * File:
 *     prescan.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Prescan interface definition
 * 
 * Copyright 2010-2017 OpenSynergy GmbH.
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

#ifndef __PRESCAN_H
#define __PRESCAN_H

#include "bttypes2.h"

/*---------------------------------------------------------------------------
 * Prescan layer
 *
 *     The Blue SDK supports a number of different modules where
 *     protocol data may be exchanged. In some cases it is helpful for
 *     a layer to expose its data flow so that it can be monitored, 
 *     displayed, or even modified at runtime. This module allows different
 *     data "producers" to notify interested "consumers" regarding the data
 *     it exchanges with its lower layers.
 *
 *     The entire prescan layer can be enabled/disabled using BT_PRESCAN.
 */

/*---------------------------------------------------------------------------
 * PrescanEventType type
 *
 *     Defines the types of events that a consumer may receive on behalf
 *     of event producers. Extends BtEventTypeEx.
 */
typedef BtEventTypeEx PrescanEventType;

/* The producer is about to deliver outgoing data to its transport layer. 
 */
#define PRESCAN_EVENT_TX        (BTE_TYPE_BASE_PRESCAN + 0x01)

/* The producer has received notification from its transport layer that
 * its transmission is now complete.
 */
#define PRESCAN_EVENT_TX_DONE   (BTE_TYPE_BASE_PRESCAN + 0x02)

/* The producer has just received data from its transport layer and is
 * about to process it.
 */
#define PRESCAN_EVENT_RX        (BTE_TYPE_BASE_PRESCAN + 0x04)

/* The producer is done handling received data, and is about to free it
 * other otherwise indicate it back to the transport layer that provided
 * it.
 */
#define PRESCAN_EVENT_RX_DONE   (BTE_TYPE_BASE_PRESCAN + 0x08)

/* End of PrescanEventType */

/*---------------------------------------------------------------------------
 * PrescanProducer type
 *
 *     Defines the types of producers that deliver information for review
 *     by consumers.
 */
typedef U32 PrescanProducer;

/* Identifies the producer of commands and events within the HCI layer of a
 * BR/EDR radio.
 * 
 * When receiving events for this producer, event.producer will be set
 * to 0 (because there is only one BR/EDR entity in the system). For TX and
 * TX_DONE events (HCI Commands), events.data will contain an HciPacket
 * pointer. For RX and RX_DONE events (HCI Events), events.data will contain
 * an HciBuffer pointer.
 *
 * If a consumer inhibits a TX event, the packet should eventually be 
 * replaced using HCI_PacketSent.
 * 
 * TX_DONE events should not be inhibited unless they correspond to a TX packet
 * provided by the consumer rather than the actual transport.
 *
 * RX and RX_DONE events, if inhibited, must take care of deallocating
 * the packet just as HCI would have.
 */
#define PRESCAN_BREDR_CMDEVT   0x00000100


/* Identifies the producer of ACL and SCO data within the HCI layer of a
 * BR/EDR radio. Otherwise identical to PRESCAN_BREDR_CMDEVT.
 *
 * Note that in some cases it will be more convenient to trap data coming
 * from the phylink producer instead of this one.
 */
#define PRESCAN_BREDR_DATA     0x00000200

/* Identifies an AMP phy link as the producer. Only actual L2CAP
 * data sent over an AMP is produced with this type. For production of 
 * AMP-HCI commands and events, see PRESCAN_AMPHCI_CMDEVT.
 *
 * The event.producer provided will always be the AmpPhyLink pointer
 * itself.
 *
 * In PRESCAN_EVENT_TX events, event.data will contain a BtPacket pointer.
 * It should be assumed that the time of the event, the controller is 
 * about to deliver the TX packet.
 *
 * In PRESCAN_EVENT_TX_DONE events, a BtPacket pointer is provided and
 * the controller has just indicated completion of delivery. event.status
 * will contain the completion status reported by the controller.
 *
 * PRESCAN_EVENT_RX indicates data which has just arrived from the
 * controller for processing by the system. event.data will contain a 
 * pointer to a LinkEvent of type LINK_EVENT_LOGLINK_RECEIVE.
 *
 * PRESCAN_EVENT_RX_DONE events are not generated.
 */
#define PRESCAN_AMPPHYLINK     0x00000400

/* Identifies an HCI-style AMP Controller as the producer. Only HCI
 * commands and events are included. (Channel data is delivered at the phylink
 * layer via the PRESCAN_AMPPHYLINK producer type.)
 *
 * The event.producer provided is the AmpHciController context, which
 * can also be treated as an AmpController.
 *
 * PRESCAN_EVENT_TX events are produced when the HCI AMP Controller is 
 * just about to deliver a packet to its lower-layer transport. "event.data"
 * refers to an HciPacket being delivered. 
 *
 * PRESCAN_EVENT_TX_DONE events are produced when the HCI AMP Controller's
 * transport is handling TX completion as indicated by the lower-layer
 * transport. "event.data" will be the same HciPacket provided earlier with
 * PRESCAN_EVENT_TX.
 *
 * PRESCAN_EVENT_RX events contain an HciBuffer as received by the transport.
 *
 * PRESCAN_EVENT_RX_DONE events are not generated. */
#define PRESCAN_AMPHCI_CMDEVT  0x00000800

/* For future expansion */
#define PRESCAN_L2CAP          0x00002000

/* For future expansion. */
#define PRESCAN_OBEX           0x00004000

/* End of PrescanProducer */

/*---------------------------------------------------------------------------
 * PrescanPriority type
 *
 *     Defines a priority for prescan consumers. Higher priority consumers 
 *     will run before any lower-priority ones. 
 *   
 *     Prescans that modify or inhibit prescan events should be higher
 *     priority than prescans that do not. This will prevent lower level
 *     prescans from reviewing or logging invalid data.
 */
typedef U8 PrescanPriority;
#define PRESCAN_PRIORITY_HIGH 255
#define PRESCAN_PRIORITY_MEDIUM 128
#define PRESCAN_PRIORITY_LOW  0
/* End of PrescanPriority */

/*---------------------------------------------------------------------------
 * PrescanConsumer struct
 *
 *     Context structure used to manage a consumer of events. Consumers 
 *     initialize this structure and supply it to PRESCAN_Register.
 */
typedef struct _PrescanConsumer {
    /* Prepare the "context" and "callback" fields of this structure before
     * registration
     */
    BtHandlerEx handler;

    /* === Internal use only === */
    PrescanProducer producers;
    PrescanEventType events;
    PrescanPriority priority;
} PrescanConsumer;

/*---------------------------------------------------------------------------
 * PrescanEvent struct
 *
 *     Describes an event for processing by a consumer.
 */
typedef struct _PrescanEvent {
    BtEventEx event;
    /* Event output parameter; FALSE on input and set by consumer to TRUE if
     * event is ‘inhibited’. Inhibited events will not be further acted upon 
     * by the producer, nor will they be delivered to any other consumers.
     */
    BOOL inhibit;

    /* Single bit set to indicate the producer of this event */
    PrescanProducer producerType;

    /* Actual pointer to the producer’s context if any, as defined by
     * producerType
     */
    void *producer;

    /* Pointer to data. The data type is as defined by producerType
     * and event.type
     */
    void *data;

} PrescanEvent;

/* Callback type for prescanning. Can be safely cast to BtCallbackEx.
 */
typedef void (*PrescanCallback)(void *ctx, PrescanEvent *e);

/*---------------------------------------------------------------------------
 * PRESCAN_Init()
 *
 *     Called at system startup to initialize the prescan system.
 *
 * Requires:
 *     BT_PRESCAN enabled.
 *
 */
void PRESCAN_Init(void);

/*---------------------------------------------------------------------------
 * PRESCAN_Register()
 *
 *     Registers a prescan consumer to receive events from producers.
 *
 * Requires:
 *     BT_PRESCAN enabled.
 *
 * Parameters:
 *     pc - An initialized PrescanConsumer struct containing context memory
 *         for use while the consumer is registered. If pc is already
 *         registered this call is ignored.
 *
 *     producerMask - Mask for this consumer consisting of all desired
 *        PrescanProducer types bitwise-OR'ed together. Can be changed later
 *        with PRESCAN_SetMask. An event must match both the producerMask
 *        and the eventMask or the consumer will not hear about it.
 *
 *     eventMask - Mask for the types of producer events of interest, 
 *        consisting of all important events bitwise-OR'ed together. Can be
 *        changed later with PRESCAN_SetMask. An event must match both
 *        the producerMask and the eventMask or the consumer will not hear
 *        about it.
 *
 *     priority - The priority for this prescan consumer. Cannot be changed
 *         after registration.
 *
 */
void PRESCAN_Register(PrescanConsumer *pc, PrescanProducer producerMask,
                      PrescanEventType eventMask,
                      PrescanPriority priority);
#if BT_PRESCAN != XA_ENABLED
#define PRESCAN_Register(a,b,c,d) 0
#endif /* BT_PRESCAN != XA_ENABLED */

/*---------------------------------------------------------------------------
 * PRESCAN_Deregister()
 *
 *     Deregisters a prescan consumer so that it will receive no further
 *     events.
 *
 * Requires:
 *     BT_PRESCAN enabled.
 *
 * Parameters:
 *     pc - A previously registered consumer. If not already registered,
 *         this call is ignored.
 */
void PRESCAN_Deregister(PrescanConsumer *pc);
#if BT_PRESCAN != XA_ENABLED
#define PRESCAN_Deregister(a) (0)
#endif /* BT_PRESCAN != XA_ENABLED */

/*---------------------------------------------------------------------------
 * PRESCAN_SetMask()
 *
 *     Updates the set of producers for which this consumer has an interest.
 *
 * Requires:
 *     BT_PRESCAN enabled.
 *
 * Parameters:
 *     pc - A previously registered consumer. If not already registered,
 *         this call is ignored.
 *
 *     producerMask - New bitwise mask of producers of interest. See
 *         PRESCAN_Register.
 *
 *     eventMask - New bitwise mask of producers of interest. See
 *         PRESCAN_Register.
 *        
 */
void PRESCAN_SetMask(PrescanConsumer *pc, PrescanProducer mask,
                     PrescanEventType eventMask);
#if BT_PRESCAN != XA_ENABLED
#define PRESCAN_SetMask(a,b,c) 0
#endif /* BT_PRESCAN != XA_ENABLED */

/*---------------------------------------------------------------------------
 * PRESCAN_Produce()
 *
 *     Producers invoke this to notify all registered and interested 
 *     consumers. The event contains everything necessary to understand the
 *     transport data processing that is about to occur.
 *
 *     It is assumed that PRESCAN_Produce() is called during normal
 *     stack execution context (i.e. during OS_LockStack()).
 *
 * Requires:
 *     BT_PRESCAN enabled.
 *
 * Parameters:
 *     type - Event type.
 *
 *     producerType - Indicates the originator of the event.
 *
 *     producerContext - A pointer to producer-specific context memory.
 *         For example, an AMP physical layer event points back to the
 *         originating AmpPhyLink.
 *
 *     data - A pointer to a data structure representing the data for the 
 *         event. For example, a BR/EDR HCI layer TX event is represented
 *         as an HciPacket.
 *
 *     status - Status associated with the event, if any.
 *
 * Returns:
 *     FALSE - The event was observed and perhaps even modified but the 
 *         producer should continue to process it normally.
 *
 *     TRUE - One of the prescan consumers "handled" the event and therefore
 *         requests that the event should be prevented from being sent
 *         anywhere further.
 */
BOOL PRESCAN_Produce(PrescanEventType type, PrescanProducer producerType,
                     void *producerContext, void *data, BtStatus status);
#if BT_PRESCAN != XA_ENABLED
/* If prescans are disabled, nothing can consumes the event */
#define PRESCAN_Produce(a,b,c,d,e) 0
#endif /* BT_PRESCAN != XA_ENABLED */

#endif /* __PRESCAN_H */
