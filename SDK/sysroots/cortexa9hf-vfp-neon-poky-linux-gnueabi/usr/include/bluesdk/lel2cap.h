/****************************************************************************
 *
 * File:
 *     lel2cap.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the upper layer of 
 *     the LE portion of Logical Link Control and Adaptation Protocol.
 * 
 * Copyright 1999-2017 OpenSynergy GmbH
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
 * OpenSynergy GmbH which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __LEL2CAP_H
#define __LEL2CAP_H

#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
#include "bttypes2.h"
#include "l2cap_a.h"

/*---------------------------------------------------------------------------
 * LE L2CAP (LEL2CAP) layer
 *
 *     The LE L2CAP layer handles the LE specific L2CAP communication that
 *     take place over LE ACL links. L2CAP is a protocol multiplexer,
 *     allowing protocols and applications to share a single device-to-
 *     device connection. The L2CAP layer also allows a higher layer protocol 
 *     to communicate with several other devices over separate "channels".
 *
 *     The LE L2CAP protocol varies from the BR/EDR L2CAP protocol in that
 *     the LE ACL is assumed to be reliable so it does not require error
 *     detection and retransmission. LE L2CAP provides credit based flow
 *     control.
 *
 *     LE L2CAP supports segmentation and reassembly, which is the process
 *     of breaking down transmit packets into an appropriate size for
 *     HCI, and reassembling received packets for higher layers. The
 *     Maximum Transmission Unit (MTU) represents the largest possible
 *     reassembled L2CAP packet.
 *
 *     LE L2CCAP share some data structures and events with the BR/EDR
 *     L2CAP API in particular it uses the L2capEventTypeEx type and
 *     the L2capEvent structure for events. The main reason to allow
 *     a single handler the ability to process both LE and BR/EDR L2CAP
 *     events.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * LeL2capConnStatus type
 *
 *     Indicates the status of a requested connection. When the L2CAP
 *     user receives the L2EVENT_CONNECT_IND event for an LE connection, it 
 *     must call
 *     L2CAP_ConnectRsp to provide one of the status codes below.
 */
typedef U16 LeL2capConnStatus;

/* The connection request is accepted. */
#define L2LECONN_ACCEPTED                             0x0000

/* Rejects the connection request because no such protocol service exists.
 * Generally, this rejection status should only be originated by L2CAP,
 * not by protocol services.
 *
 * The L2EVENT_DISCONNECTED event will be generated after the rejection
 * has been sent.
 */
#define L2LECONN_REJECT_PSM_NOT_SUPPORTED             0x0002

/* Rejects the connection request due to a resource shortage. The
 * L2EVENT_DISCONNECTED event will be generated after the rejection has
 * been sent.
 */
#define L2LECONN_REJECT_NO_RESOURCES                  0x0004

/* Rejects the connection because of insufficient authentication. */
#define L2LECONN_REJECT_INSUFFICIENT_AUTHENTICATION   0x0005

/* Rejects the connection because of insufficient authorization. */
#define L2LECONN_REJECT_INSUFFICIENT_AUTHORIZATION    0x0006

/* Rejects the connection because of insufficient authentication. */
#define L2LECONN_REJECT_INSUFFICIENT_ENCRYPT_KEY_SIZE 0x0007

/* Rejects the connection because of insufficient authentication. */
#define L2LECONN_REJECT_INSUFFICIENT_ENCRYPTION       0x0008

/* Rejects the connection because the source CID is invalid */
#define L2LECONN_REJECT_INVALID_SOURCE_CID            0x0009

/* Rejects the connection request because the source CID is already
 * allocated 
 */
#define L2LECONN_REJECT_SOURCE_CID_ALLOCATED          0x000A

/* End of LeL2capConnStatus */


/*---------------------------------------------------------------------------
 * LeL2capPsmValue type
 *
 *     Defines protocol service values for LE L2CAP. These values identify
 *     protocol services that use LE L2CAP. When creating an LeL2capPsm structure
 *     for registration with L2CAP_RegisterPsm, the "psm" field must be
 *     set to one of these values.
 *
 *     Dynamically assigned PSM values are also permitted; 
 */
typedef U16 LeL2capPsmValue;

/* PSMs of this type cannot receive a connection. Only clients establishing
 * outbound L2CAP connections can use it.
 */
#define BT_CLIENT_ONLY_LE_PSM       0x0000

/* Servers that use dynamically allocated PSMs should register using
 * this value. The LEL2CAP_RegisterPsm function will assign a dynamic
 * PSM value and return it in the "psm" field.
 */
#define BT_DYNAMIC_LE_PSM           0x0080

#define BT_FIRST_FIXED_LE_PSM       0x0001
#define BT_LAST_FIXED_LE_PSM        0x007F
#define BT_FIRST_DYNAMIC_LE_PSM     0x0080
#define BT_LAST_DYNAMIC_LE_PSM      0x00FF

/* End of LeL2capPsmValue */

/*---------------------------------------------------------------------------
 * LeL2capCallback type
 *
 *     A function of this type is called to indicate LE L2CAP events to
 *     a protocol service. When LE L2CAP calls this function, the channel 
 *     information is contained in the LeL2capEvent object passed as a parameter. 
 *
 *     Protocol services provide the callback function's address to
 *     LE L2CAP by calling the LEL2CAP_RegisterPsm function.
 */
typedef void (*LeL2capCallback)(void *ctx, L2capEvent *event);

/* End of LeL2capCallback */

/*---------------------------------------------------------------------------
 * LE L2CAP Security Id
 *
 * Security ID for L2CAP access.
 */
#define LESEC_L2CAP_ID    (LeProtocolId)LEL2Cap_SecAccessRsp

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * LeL2capPsm structure
 *
 *     Represents a protocol service that uses LE L2CAP. Any higher-layer
 *     protocol or application that will use LE L2CAP's connection-oriented
 *     services must allocate and completely fill out this structure.
 *     The structure is then provided to LE L2CAP by calling 
 *     LEL2CAP_RegisterPsm.
 *
 */
typedef struct _LeL2capPsm
{
    /* LeL2capPsm inherit from BtHandlerEx. The user must fill in the 
     * h.callback and may fill in h.context. The h.node field is used 
     * internally by LeL2cap and must not be modified. 
     *
     * The LEL2CAP layer will call h.callback whenever an event occurs
     * and expects the callback function to be of the type LeL2capCallback. 
     */
    BtHandlerEx         h; /* Inherit */

    /* Identifier for the protocol service. */
    LeL2capPsmValue     psm;
} LeL2capPsm;

/*---------------------------------------------------------------------------
 * LeL2capFlowParms structure
 *
 *     Specifies the LE L2CAP flow control parameters for the a channel.
 */
typedef struct _LeL2capFlowParms
{
    /* The maximum SDU size that the local channel can receive. This value must
     * be 23 or greater.
     */
    U16               maxSdu;

    /* The maximum PDU size the local channel can receive.  This value must be
     * 23 or greater.
     */
    U16               maxPdu;

    /* LE L2CAP uses a credit based flow control mechanism. Initial credits 
     * are sent in the LE L2CAP connect request/response packets. The following 
     * field is used to indicate the initial credits. If the initial credits are 
     * 0 then the remote device cannot send data until it is granted credits. 
     * Credits can be sent to the remote device by calling LEL2CAP_AdvanceCredits().
     */
    U16               initialCredits;
} LeL2capFlowParms;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * LEL2CAP_RegisterPsm()
 *
 *     Registers a PSM identifier with LE L2CAP. The protocol can register
 *     an outgoing connection (by assigning protocol->psm to 
 *     BT_CLIENT_ONLY_LE_PSM), an incoming connection for a fixed PSM 
 *     (by assigning the applicable fixed protocol->psm value), or an 
 *     incoming connection for the next available dynamic PSM (by assigning 
 *     protocol->psm to BT_DYNAMIC_LE_PSM). The outgoing connection PSM
 *     can initiate connections and data transfer, while the incoming
 *     connection PSM's can receive connection requests and data.
 *
 * Parameters:
 *     protocol - Pointer to the LeL2capPsm structure to be registered.
 *         This structure is owned by LE L2CAP until it is successfully
 *         deregistered. All fields in the structure must be initialized
 *         before this function is called.  In the case of a dynamic PSM
 *         the psm field will be assigned to the next available dynamic
 *         PSM value.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - PSM was successfully registered with LE L2CAP.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter value: protocol is 0, 
 *          protocol->h.callback is 0, protocol->psm is outside the legal
 *          range. (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_USE - The protocol is already registered.
 */
BtStatus LEL2CAP_RegisterPsm(LeL2capPsm *protocol);

/*---------------------------------------------------------------------------
 * LEL2CAP_DeregisterPsm()
 *
 *     Deregisters a registered protocol from LEL2CAP.
 *
 * Parameters:
 *     protocol - Pointer to LeL2capPsm structure previously registered with
 *         the L2CAP_RegisterPsm function call.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The protocol has been deregistered.
 *
 *     BT_STATUS_INVALID_PARM - The protocol parameter was invalid (XA_ERROR_CHECK
 *         only).
 *
 *     BT_STATUS_NOT_FOUND - The protocol was not previously registered.
 *
 *     BT_STATUS_BUSY - The protocol has one or more active channels and
 *         cannot be deregistered.
 */
BtStatus LEL2CAP_DeregisterPsm(LeL2capPsm *protocol);

/*---------------------------------------------------------------------------
 * LEL2CAP_ConnectReq()
 *
 *     Requests the establishment of an LE L2CAP channel to the specified
 *     remote protocol service and device. If this function returns
 *     BT_STATUS_PENDING, the request was successfully initiated.
 *     Connection status events will be sent to the initiating
 *     LeL2capPsm's h.callback function.
 *
 * Parameters:
 *     protocol - Pointer to a registered LeL2capPsm structure.
 *
 *     psm - PSM value of protocol to connect to on the remote device.
 *
 *     remDevice - The LE remote device to connect with.
 *
 *     parms - Pointer to the channel-specific flow parameters. All 
 *         settings must be initialized.
 *
 *     localCid - Pointer to a channel ID. If this function returns
 *         BT_STATUS_PENDING, this parameter is filled with a channel
 *         ID assigned to this connection.
 *
 * Returns:
 *     BT_STATUS_PENDING - The connection process has been successfully
 *         started. When the connection process is complete, the
 *         protocol service's callback will receive either the
 *         L2EVENTEX_CONNECTED or L2EVENTEX_DISCONNECTED L2capEventType.
 *
 *     BT_STATUS_INVALID_PARM - Parameters are invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - ACL is not an LE link.
 *
 *     BT_STATUS_NO_RESOURCES - LE L2CAP could not allocate a channel
 *         structure to initiate the connection.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link to the specified device
 *         is not active.
 *
 *     BT_STATUS_NOT_FOUND - The PSM is not registered. 
 *
 *     BT_STATUS_RESTRICTED - The PSM is not authorized to establish
 *         outbound connections.
 *
 *     BT_STATUS_NOT_SUPPORTED - The PSM is not authorized to establish
 *         outbound connection because a security record exists requiring
 *         secure connections but the remote device does not support it.
 */
BtStatus LEL2CAP_ConnectReq(LeL2capPsm *protocol, L2capPsmValue psm, 
                            BtRemoteDevice *remDev, LeL2capFlowParms *parms, 
                            L2capChannelId *localCid);


/*---------------------------------------------------------------------------
 * LEL2CAP_ConnectRsp()
 *
 *     Indicates the response to a connection request (L2EVENTEX_CONNECT_IND
 *     L2capEventType) from a remote device.
 *
 * Parameters:
 *     cid - Channel ID assigned by LEL2CAP to this connection. This
 *         ID must match the one provided by the L2EVENTEX_CONNECT_IND event
 *         callback.
 *
 *     status - Response code used in the response to the remote device.
 *         This value may indicate success, or rejection.
 *
 *     parms - Pointer to the channel-specific flow parameters. All 
 *         settings must be initialized.
 *
 * Returns:
 *     BT_STATUS_PENDING - The connection response packet has been initiated
 *         successfully. One of the following events will arrive to
 *         indicate completion:
 *
 *         L2EVENTEX_CONNECTED, if the connection was established successfully.
 *
 *         L2EVENTEX_DISCONNECTED, if the connection failed. Check the callback
 *         info's "status" parameter for the exact reason.
 *
 *     BT_STATUS_FAILED - Invalid channel ID (XA_ERROR_CHECK only), or
 *         LEL2CAP_ConnectRsp was called at the wrong time.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this
 *         channel is no longer active.
 */
BtStatus LEL2CAP_ConnectRsp(L2capChannelId cid, LeL2capConnStatus connStatus,
                            LeL2capFlowParms *parms);


/*---------------------------------------------------------------------------
 * LEL2CAP_DisconnectReq()
 *
 *     Requests the disconnection of the specified LEL2CAP channel.
 *
 * Parameters:
 *     cid - The channel to disconnect.
 *
 * Returns:
 *     BT_STATUS_PENDING - The disconnect request has been initiated
 *         successfully. When the disconnect is complete, the
 *         L2EVENTEX_DISCONNECTED event is provided to the LEL2CAP user.
 *
 *     BT_STATUS_IN_PROGRESS - The channel is already disconnecting
 *         (XA_ERROR_CHECK Only).
 *
 *     BT_STATUS_FAILED - The channel is invalid or could not be
 *         disconnected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this
 *         channel is no longer active.
 */
BtStatus LEL2CAP_DisconnectReq(L2capChannelId cid);

/*---------------------------------------------------------------------------
 * LEL2CAP_SendData()
 *
 *      Sends data on the specified channel. The maximum size SDU that can
 *      be sent to a remote device is contained in the info->maxRemoteSdu 
 *      field in the L2capEvent sent in a L2EVENTEX_CONNECTED event.
 *
 * Parameters:
 *      cid - Channel to send the data over. The channel must be connected.
 *
 *      packet - Data packet describing data to transmit. The caller
 *          must initialize all necessary BtPacket fields, including
 *          "data", "dataLen", "flags", "ulpContext", "tail", "tailLen",
 *          "header" and "headerLen" as described below.
 *
 *          The "data" pointer and "dataLen" length indicate the payload
 *          data to be transmitted by LEL2CAP.
 *
 *          The "header" array is filled with higher-layer protocol
 *          header data. This array is filled back-to-front so that
 *          protocols may fill in data as the packet is handed down
 *          the protocol stack. "headerLen" indicates the number of
 *          valid header bytes at the end of the "header" array.
 *
 *          The "ulpContext" field may be filled with a protocol-specific
 *          value. L2CAP and lower layers ignore this field.
 *
 *          The "flags" field may be set as described by the BtPacketFlags
 *          type.
 *
 *          The "tail" pointer may point to a data buffer that is appended
 *          to the end of the packet (after "data"). The field "tailLen"
 *          is set to the number of bytes pointed to by "tail". If this field
 *          is used, the packet flag BTP_FLAG_TAIL must be set.
 *
 *          The packet's total size ("headerLen" plus "dataLen" plus
 *          "tailLen", if present) must be less than or equal to
 *          the remote device's maximum SDU.
 *
 * Returns:
 *     BT_STATUS_PENDING - Packet was successfully queued for transmission on
 *         the channel. The completion of the send request is indicated by an
 *         L2EVENT_PACKET_HANDLED event.
 *
 *     BT_STATUS_FAILED - The channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this
 *         channel is no longer active.
 */
BtStatus LEL2CAP_SendData(L2capChannelId cid, BtPacket *packet);

/*---------------------------------------------------------------------------
 * LEL2CAP_AdvanceCredits()
 *
 *      Advanced credit to the remote device on the specified channel. The
 *      remote device cannot send packets unless it has credit. Each fragment 
 *      received via an L2EVENTEX_FRAGMENT_IND or L2EVENTEX_LAST_FRAG_IND 
 *      consumes a credit. The actual number of credits left after receiving 
 *      data is contained in the "info.remainingCredits" field of the 
 *      L2capEvent event.
 *
 * Parameters:
 *      cid - Channel to which credits are advanced. The channel must be connected.
 *
 *      credits - number of credits to advance. Credits are accumulative which means
 *          the remote device adds these credits to the unused credits previously
 *          advanced. The value cannot be 0. 
 *
 * Returns:
 *     BT_STATUS_PENDING - The LEL2CAP layer has successfully received the credits
 *         and will send them to the remote device as soon as possible. 
 *
 *     BT_STATUS_FAILED - The channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this
 *         channel is no longer active.
 *
 *     BT_STATUS_INVALID_PARM - credit value is 0 (error check only).
 */
BtStatus LEL2CAP_AdvanceCredits(L2capChannelId cid, U16 credits);

/*---------------------------------------------------------------------------
 * LEL2CAP_GetMaxTxSizes()
 *
 *     Retrieves the transmit SDU (MTU) and PDU (MPS) sizes of the connected channel. 
 *     Do not use this API for fixed channels.
 *
 * Parameters:
 *     cid - Channel ID to query.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The LEL2CAP layer has successfully retrieve the SDU
 *         and PDU values.
 *
 *     BT_STATUS_FAILED - The channel is invalid or the channel is not open.
 *
 *     BT_STATUS_INVALID_PARM - Provided Channel Id is invalid (error check only).
 */
BtStatus LEL2CAP_GetMaxTxSizes(L2capChannelId cid, U16 *sdu, U16 *pdu);

/*---------------------------------------------------------------------------
 * LEL2CAP_GetTxCredits()
 *
 *     Retrieves the current amount of credits available for transmit
 *     PDU's.
 *
 * Parameters:
 *     cid - Channel ID to query.
 *
 * Returns:
 *     The number of available transmit credits.
 */
U16 LEL2CAP_GetTxCredits(L2capChannelId cid);

/* For LESEC_L2CAP_ID */
void LEL2Cap_SecAccessRsp(void *ctx, BtEventEx *event);

#endif /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */

#endif /* __LEL2CAP_H */
