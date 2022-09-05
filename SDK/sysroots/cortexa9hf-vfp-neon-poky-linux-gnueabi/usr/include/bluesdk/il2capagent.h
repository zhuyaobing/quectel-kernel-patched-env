/****************************************************************************
 *
 * File:
 *     il2capagent.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Interface for the L2CAP agent.
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

#ifndef __IL2CAPAGENT_H
#define __IL2CAPAGENT_H

#include "l2cap.h"
#include "me.h"
#include "bttypes2.h"
#include "sys/ampphylink.h"

/*---------------------------------------------------------------------------
 * L2capAgent layer
 *
 *     An L2capAgent is used to manage an L2CAP channel. The L2capAgent takes
 *     care of creating an AMP physical link if one is to be used and also
 *     helps when an L2CAP channel needs to be moved from one physical link
 *     to another. Applications provide a controller preference that 
 *     is used by the L2capAgent to determine the physical link over which the
 *     channel will operate. A preference is also used when moving a channel.
 *     The L2capAgent will automatically move a channel from an AMP physical 
 *     link to the BR/EDR physical link if the AMP physical link is terminated
 *     from under the channel (e.g. abnormal termination due to link loss).
 *
 *     The L2capAgent module is implemented as an interface (i.e. a table of
 *     function pointers). With this design is it possible to create other
 *     L2capAgents that provide the same interface but are implemented
 *     differently and/or provide additional capabilities. By using an interface
 *     application can use different L2capAgents with out code modifications.
 *
 *     L2capAgent provides the API to the L2CAP layer and requires that an 
 *     L2capPsm structure be registered with the L2CAP layer using 
 *     L2CAP_RegisterPsm(). 
 *
 *     An L2capAgent structure must be allocated by the user in order to
 *     initiate the creation of an L2CAP channel using L2A_ConnectReq. The
 *     constructor L2CAPAGENT_Create() must be called on the L2capAgent
 *     structure before it can be used. Once constructed L2A_Init() can be 
 *     called. A system constructed L2capAgent can be allocated using
 *     L2CAPAGENT_Alloc().
 *     
 *
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * ControllerPrefType type
 *
 *     Preferences indicating which controller to use for the physical link
 *     when creating a channel using L2A_ConnectReq() or moving a channel 
 *     using L2A_MoveReq(). Only one AMP is tried so if the agent finds an AMP 
 *     controller based on the preference and flags then tries to use to it 
 *     but something fails along the way (e.g. AMP physical link fails to be
 *     established) it will not search for another AMP controller. If the 
 *     ControllerPrefFlags are not set to CPREFF_ONLY then  when an attempt
 *     to use an AMP controller fails the channel will be placed over the 
 *     BR/EDR physical link.
 */
typedef U8 ControllerPrefType;

/* Illegal Controller Preference */
#define CPREF_ILLEGAL           0

/* Create or move the channel over the BR/EDR link. The ControllerPrefFlags 
 * settings have no effect. 
 */
#define CPREF_BR_EDR            1

/* Create or move a channel over an AMP Controller. The Agent determines 
 * which AMP to use. 
 */
#define CPREF_AMP               2 

/* Create or move a channel over an AMP physical link using AMP Controllers 
 * of the type specified in "p.type". 
 */
#define CPREF_AMP_TYPE          3 

/* Create or move a channel to an AMP physical link which uses the local 
 * AMP Controller with the ID specified in "p.localId". 
 */
#define CPREF_AMP_LOCAL         4

/* Create a channel or move a channel on/to the AMP physical link using the 
 * remote AMP Controller with the ID specified in "p.remoteId". 
 */
#define CPREF_AMP_REMOTE        5

/* Create a channel or move a channel on/to the AMP physical link defined 
 * by both the local and remote AMP controllers specified in p.both.
 */
#define CPREF_AMP_LOCAL_REMOTE  6

/* End of ControllerPrefType */

/*---------------------------------------------------------------------------
 * ControllerPrefFlags type
 *
 *     Flags indicating how the physical link specified in a
 *     ControllerPreference structure should be used.
 */
typedef U8 ControllerPrefFlags;

/* No flags are set so if the AMP specified by a ControllerPreference is unable
 * to be used (e.g. physical link cannot be created) then the channel is 
 * created over the BR/EDR physical link. In the case of a move operation the 
 * channel remains on the current physical link if the AMP physical link cannot 
 * be created.
 */
#define CPREFF_NONE             0x00

/* If set it indicates that the AMP physical link specified by
 * ControllerPreference is to be used and if it cannot be used for any reason
 * the channel creation will fail. In the case of a move operation the channel
 * will remain on the current physical link if the AMP physical link cannot
 * be created.
 */
#define CPREFF_ONLY             0x01

/* If set it indicates that the AMP physical link specified by the 
 * ControllerPreference is used only if the physical link already exists.
 * If the physical link does not exist then the BR/EDR physical link is used. 
 * If CPREFF_ONLY is also set and the physical link does not exist the channel 
 * creation fails. In the case of a move operation the channel will remain on 
 * the current physical link if the AMP physical link does not already exist 
 * regardless of the setting for CPREFF_ONLY.
 */
#define CPREFF_PHY_EXISTS       0x02

/* If set it indicates that the channel should be created on the BR/EDR 
 * physical link first and when an AMP physical link the channel will be 
 * moved to the AMP physical link. If the CPREFF_ONLY flag is set and the 
 * AMP physical link is not able to be created, the channel will remain on 
 * the BR/EDR link. If the CPREF_PHY_EXISTS flag is set and the AMP physical
 * link does not exist the channel will remain on the BR/EDR link. In other 
 * words, if the channel is created over BR/EDR and the AMP physical link
 * cannot be created for any reason it will remain on the BR/EDR physical 
 * link. It will not be disconnected. This flag is ignored for a move 
 * operation.
 */
#define CPREFF_START_BR_EDR     0x04

/* If set, indicates the channel should not be moved by the channel manager
 * to a different physical link based on the L2ACHANMGR_TX_DATA event. This
 * is a hint from the channel indicating it may have periodic idle and full
 * throughput states over the life of the channel. This hint should be
 * considered when determining the physical link for the channel to reduce
 * the overhead of moving the channel back and forth between different
 * physical links as its data rate changes. This flag is considered a
 * hint only and the channel manager may move the channel based on other
 * throughput and power requirements of the system.
 */
#define CPREFF_NO_IDLE_DATA     0x08

/* End of ControllerPrefFlags */

/*---------------------------------------------------------------------------
 * L2capAgent type
 *
 *    Forward reference to L2capAgent structure defined in l2capagent.h.
 */
typedef struct _L2capAgent L2capAgent;

/* End of L2capAgent type */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

#if NUM_AMPS > 0 
/*---------------------------------------------------------------------------
 * ControllerPreference 
 *
 *     Defines AMP Controller preference to be used when creating or moving
 *     a channel. 
 */
typedef struct _ControllerPreference {
    ControllerPrefType    pref;
    ControllerPrefFlags   flags;

    /* The following are based on the pref */
    union {
        BtControllerType type;
        U8               localId;
        U8               remoteId;
        struct {
            U8 localId;
            U8 remoteId;
        } both;
    } p;
} ControllerPreference;
#endif /* NUM_AMPS > 0  */


/****************************************************************************
 *
 * Interface Functions for interface iL2capAgent
 *
 ****************************************************************************/





#if NUM_AMPS > 0 
/*---------------------------------------------------------------------------
 * L2A_DisconnectReq()
 *
 *     Requests the disconnection of the specified L2CAP channel.
 *
 * Parameters:
 *     agent - Pointer to object containing the L2CAP Agent interface
 *
 * Returns:      
 *     BT_STATUS_PENDING - The disconnect request has been initiated
 *         successfully. When the disconnect is complete, the
 *         L2EVENT_DISCONNECTED event is provided to the L2CAP user
 *         by calling the handler in the L2capPsm structure.
 * 
 *     BT_STATUS_IN_PROGRESS - The channel is already disconnecting
 *
 *     BT_STATUS_FAILED - The channel is invalid or could not be
 *         disconnected. A channel in the middle of being created
 *         cannot be disconnected.
 *
 *     BT_STATUS_NO_CONNECTION - The link associated with this 
 *         channel is no longer active.
 */
BtStatus L2A_DisconnectReq(L2capAgent *agent);
#define L2A_DisconnectReq(agent) (agent)->i.disconnectReq(agent)
typedef BtStatus (*L2capAgentDiscReq)(L2capAgent *agent);

/*---------------------------------------------------------------------------
 * L2A_ConnectReq()
 *
 *     Requests the establishment of an L2CAP channel to the specified
 *     remote device. If this function returns BT_STATUS_PENDING, the 
 *     request was successfully initiated. Connection status events will be 
 *     sent to the handler in the registered L2capPsm structure.
 *
 * Parameters:
 *     agent - Pointer to object containing the L2CAP Agent interface. The
 *         caller of this function is required to allocate the L2capAgent
 *         object and free when the L2capAgent is disconnected 
 *         (L2EVENT_DISCONNECTED event is sent to the handler). 
 *
 *     psm - PSM value of protocol to connect to on the remote device. 
 *
 *     remDev - The remote device to connect with.
 *
 * Returns:      
 *     BT_STATUS_PENDING - The connection process has been successfully
 *         initialized. When the connection process is complete, the
 *         handler in the L2capPsm structure will receive either the
 *         L2EVENT_CONNECTED or L2EVENT_DISCONNECTED L2capEventType.
 *
 *     BT_STATUS_FAILED - Protocol linkModes mismatch or the Controller
 *         preference could not be met.  
 * 
 *     BT_STATUS_NO_RESOURCES - L2CAP could not allocate a channel
 *         structure to initiate the connection.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link to the specified device
 *         is not active.
 *
 *     BT_STATUS_RESTRICTED - The PSM is not authorized to establish
 *         outbound connections.
 *
 *     BT_STATUS_IN_USE - Agent is already in use.
 *
 *     BT_STATUS_SECURITY - Operation failed because the correct security
 *         settings are not in place for using an AMP and the CPREFF_ONLY
 *         flag setting was set.
 *
 */
BtStatus L2A_ConnectReq(L2capAgent *agent, L2capPsmValue psm, BtRemoteDevice *remDev);
#define L2A_ConnectReq(agent, psm, remDev) (agent)->i.connectReq(agent, psm, remDev)
typedef BtStatus (*L2capAgentConReqFunc)(L2capAgent *agent, L2capPsmValue psm, 
                                         BtRemoteDevice *RemDev);


/*---------------------------------------------------------------------------
 * L2A_ConnectRsp()
 *
 *     Used to indicate the response to a connection request from a remote
 *     device. L2EVENT_CONNECT_IND event (L2capEventType) is sent to the 
 *     handler in the L2capPsm structure to signal an incoming connection.
 *
 * Parameters:
 *     agent - Pointer to object containing the L2CAP Agent interface. This 
 *         L2capAgent was allocated by the stack and will be freed by the stack.
 *         The l2capAgent pointer is available in the L2EVENT_CONNECT_IND 
 *         event.
 *
 *     status - Response code used in the response to the remote device.
 *         This value may indicate success, pending, or rejection. 
 * 
 *     settings - Pointer to the channel-specific connection settings. This
 *         parameter may be zero if the settings in the L2capPsm structure 
 *         are acceptable.  
 *
 * Returns:
 *     BT_STATUS_PENDING - The connection response has been initiated
 *         successfully. One of the following events will be sent to the 
 *         handler in the L2capPsm structure to indicate completion:
 *
 *         L2EVENT_CONNECTED, if the connection was established successfully.
 *
 *         L2EVENT_DISCONNECTED, if the connection failed. Check the callback
 *         info's "status" parameter for the exact reason.
 *
 *         L2EVENT_COMPLETE, when the remote device has been informed of the
 *         L2CONN_PENDING connection status. L2A_ConnectRsp must
 *         be called a second time.
 *
 *     BT_STATUS_FAILED - Invalid channel ID (XA_ERROR_CHECK only), or
 *         L2A_ConnectRsp was called at the wrong time.
 *
 *     BT_STATUS_IN_PROGRESS - A response is already in progress. Wait for
 *         L2EVENT_COMPLETE before calling L2A_ConnectRsp.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this 
 *         channel is no longer active.
 */
BtStatus L2A_ConnectRsp(L2capAgent *agent, L2capConnStatus status,
                        const L2capConnSettings *settings);
#define L2A_ConnectRsp(agent, status, settings) (agent)->i.connectRsp(agent, status, settings)
typedef BtStatus (*L2capAgentConRspFunc)(L2capAgent *agent, L2capConnStatus status,
                                         const L2capConnSettings *settings);

/*---------------------------------------------------------------------------
 * L2A_Init()
 *
 *     Initialize the L2CAP Agent object and associate it with the given 
 *     L2capPsm structure. 
 *
 * Parameters:
 *     agent - Pointer to object containing the L2CAP Agent interface
 *
 *     protocol - Pointer to a registered L2capPsm structure. The "handler" 
 *         field and the "serviceId" field of the L2capPsm structure must be
 *         set with appropriate values. The application may supply its own 
 *         agent allocator by assigning it to the "agentFactory" field of 
 *         the L2capPsm structure or it can use the system default allocator 
 *         by setting the agentFactor field to 0.
 *
 *     settings - Pointer to the channel-specific connection settings. This
 *         parameter may be zero if the settings in the L2capPsm structure 
 *         are acceptable.  
 *
 *     pref - Pointer to Controller preference. If set to 0 then a default is
 *         used. For ConnectRequest the default is to create the channel over
 *         the BR/EDR phy link. For ConnectResponse the preference is ignored.
 *         During ConnectRequest the agent calls the L2capAgent Preference Manager
 *         (L2APREFMGR) to provide a preference based on the "serviceId" field 
 *         in the protocol structure. The "serviceId" field may tell L2APREFMGR
 *         to use the preference that already exists in the agent.
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - request succeeded. 
 *
 *     BT_STATUS_INVALID_PARM - Protocol specified is not registered
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - request failed.
 *
 */
BtStatus L2A_Init(L2capAgent *agent, L2capPsm *protocol,
                L2capConnSettings *settings, ControllerPreference *pref);
#define L2A_Init(agent, protocol, settings, pref) \
        (agent)->i.init(agent, protocol, settings, pref)
typedef BtStatus (*L2capAgentInitFunc)(L2capAgent *agent, L2capPsm *protocol,
                                       const L2capConnSettings *settings, 
                                       ControllerPreference *pref);


/*---------------------------------------------------------------------------
 * L2A_MoveReq()
 *
 *     Requests that the L2CAP channel managed by the agent be moved from
 *     the current physical link to the physical link identified in pref.
 *     If the new physical link does not exist it will be created.
 *
 * Parameters:
 *     agent - Pointer to object containing the L2CAP Agent interface
 *
 *     pref - Pointer to controller preference used in choosing the 
 *         destination physical link for the channel.
 *
 * Returns:      
 *     BT_STATUS_PENDING - The move request has been initiated
 *         successfully. When the move is complete, the
 *         L2EVENT_MOVE_CHAN_CNF event is provided to the L2CAP user.
 * 
 *     BT_STATUS_IN_PROGRESS - The channel is already in the process
 *          of being moved or created.
 *
 *     BT_STATUS_INVALID_PARM - The pref.pref value is invalid. (error
 *         checking only).
 *
 *     BT_STATUS_FAILED - The channel is invalid or could not be
 *         moved.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this 
 *         channel is no longer active.
 */
BtStatus L2A_MoveReq(L2capAgent *agent, ControllerPreference *pref);
#define L2A_MoveReq(agent, pref) (agent)->i.moveReq(agent, pref)
typedef BtStatus (*L2capAgentMoveReq)(L2capAgent *agent, ControllerPreference *pref);

/*---------------------------------------------------------------------------
 * L2A_AutoMoveReq()
 *
 *     Requests that the L2CAP channel managed by the agent be automatically
 *     moved because the current physical link is being disconnected. The new
 *     link is obtained via the channel manager or defaults to BR/EDR. 
 *
 * Parameters:
 *     agent - Pointer to object containing the L2CAP Agent interface
 *
 * Returns:      
 *     BT_STATUS_PENDING - The move request has been initiated
 *         successfully. When the move is complete, the
 *         L2EVENT_MOVE_CHAN_CNF event is provided to the L2CAP user.
 * 
 *     BT_STATUS_IN_PROGRESS - The channel is already in the process
 *          of being moved or created.
 *
 *     BT_STATUS_FAILED - The channel is invalid or could not be
 *         moved.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this 
 *         channel is no longer active.
 */
BtStatus L2A_AutoMoveReq(L2capAgent *agent);
#define L2A_AutoMoveReq(agent) (agent)->i.autoMoveReq(agent)
typedef BtStatus (*L2capAgentAutoMoveReq)(L2capAgent *agent);

/*---------------------------------------------------------------------------
 * L2A_SendData()
 *
 *      Sends data on the specified channel.
 *
 * Parameters:
 *     agent - Pointer to object containing the L2CAP Agent interface
 * 
 *     packet - Data packet describing data to transmit. The caller
 *         must initialize all necessary BtPacket fields, including
 *         "data", "dataLen", "flags", "ulpContext", "tail", "tailLen",
 *         "header" and "headerLen" as described below.
 *
 *         The "data" pointer and "dataLen" length indicate the payload
 *         data to be transmitted by L2CAP.
 *
 *         The "header" array is filled with higher-layer protocol
 *         header data. This array is filled back-to-front so that
 *         protocols may fill in data as the packet is handed down
 *         the protocol stack. "headerLen" indicates the number of
 *         valid header bytes at the end of the "header" array.
 *
 *         The "ulpContext" field may be filled with a protocol-specific
 *         value. L2CAP and lower layers ignore this field.
 *
 *         The "flags" field may be set as described by the BtPacketFlags
 *         type.
 *
 *         The "tail" pointer may point to a data buffer that is appended
 *         to the end of the packet (after "data"). The field "tailLen"
 *         is set to the number of bytes pointed to by "tail". If this field
 *         is used, the packet flag BTP_FLAG_TAIL must be set.        
 * 
 *         The packet's total size ("headerLen" plus "dataLen" plus
 *         "tailLen", if present) must be less than or equal to
 *         the remote device's L2CAP MTU. For group channels,
 *         the total size should typically be kept below 670 bytes. 
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
BtStatus L2A_SendData(L2capAgent *agent, BtPacket *packet);
#define L2A_SendData(agent, packet) (agent)->i.sendData(agent, packet)
typedef BtStatus (*L2capAgentSendData)(L2capAgent *agent, BtPacket *packet);

/*---------------------------------------------------------------------------
 * L2A_SetLocalBusy()
 *
 *     Sets the busy mode of the connected ERTM channel.
 *
 * Parameters:
 *     agent - Pointer to object containing the L2CAP Agent interface
 *
 *     Set - When true, set busy. When false, clear busy.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Retransmissions have been disabled.
 *
 *     BT_STATUS_FAILED - The channel is not configured properly
 *                        or is in wrong state for this operation.
 */
BtStatus L2A_SetLocalBusy(L2capAgent *agent, BOOL Set);
#define L2A_SetLocalBusy(agent, Set) (agent)->i.setLocalBusy(agent, Set)
typedef BtStatus (*L2capAgentSetLocalBusyFunc)(L2capAgent *agent, BOOL Set);

/*---------------------------------------------------------------------------
 * L2A_GetConnectionMode()
 *
 *     Retrieves the link mode of the connected channel.
 *
 * Parameters:
 *     agent - Pointer to object containing the L2CAP Agent interface
 *
 * Returns:
 *     Link Mode - The L2capLinkMode selected during channel configuration.
 *
 *     0 - If the channel does not exist or is not connected (XA_ERROR_CHECK only).
 */
L2capLinkMode L2A_GetConnectionMode(L2capAgent *agent);
#define L2A_GetConnectionMode(agent) (agent)->i.getConnectionMode(agent)
typedef L2capLinkMode (*L2capAgentGetConnModeFunc)(L2capAgent *agent);

/*---------------------------------------------------------------------------
 * L2A_GetTxMtu()
 *
 *     Retrieves the transmit MTU of the connected channel.
 *
 * Parameters:
 *     agent - Pointer to object containing the L2CAP Agent interface
 *
 * Returns:
 *     Size - The maximum transmission unit size in bytes.
 *
 *     0 - If the channel does not exist or is not connected (XA_ERROR_CHECK only).
 */
U16 L2A_GetTxMtu(L2capAgent *agent);
#define L2A_GetTxMtu(agent) (agent)->i.getTxMtu(agent)
typedef U16 (*L2capAgentGetTxMtuFunc)(L2capAgent *agent);

/*---------------------------------------------------------------------------
 * L2A_Free()
 *
 *     Frees the agent using the appropriate free function. This function
 *     should be "overloaded" to match the method of allocation.
 *
 * Parameters:
 *     agent - Pointer to object containing the L2CAP Agent interface
 *
 * Returns:
 *     void 
 */
void L2A_Free(L2capAgent *agent);
#define L2A_Free(agent) (agent)->i.free(agent)
typedef void (*L2capAgentFreeFunc)(L2capAgent *agent);

/*---------------------------------------------------------------------------
 * L2A_GetPhylinkDesc()
 *
 *     Retrieves the physical link descriptor of the physical link over
 *     which the channel operators.  
 *
 * Parameters:
 *     agent - Pointer to object containing the L2CAP Agent interface
 *
 *     desc - pointer to memory to receive the physical link descriptor.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - physical link descriptor is copied to memory 
 *         pointed to by desc. 
 *
 *     BT_STATUS_NO_CONNECTION - there currently is no channel.
 */
BtStatus L2A_GetPhyLinkDesc(L2capAgent *agent, BtAmpPhyLinkDesc *desc);
#define L2A_GetPhyLinkDesc(agent, desc) (agent)->i.getPhyLinkDesc(agent,desc)
typedef BtStatus (*L2capAgentGetPhyDescFunc)(L2capAgent *agent, BtAmpPhyLinkDesc *desc);

/*---------------------------------------------------------------------------
 * L2A_TxDataLen()
 *
 *     Indicate the amount of data transferred by the channel associated with
 *     the agent since the last indication. The channel may be moved to a
 *     different physical controller within this event.
 *
 * Parameters:
 *     agent - Pointer to object containing the L2CAP Agent interface
 * 
 *     TotalLen - The amount of data transferred on the channel since the
 *         previous call to L2A_TxData().
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The data information was recorded and no
 *         move request was initiated.
 * 
 *     BT_STATUS_PENDING - The move request has been initiated
 *         successfully. When the move is complete, the
 *         L2EVENT_MOVE_CHAN_CNF event is provided to the L2CAP user.
 * 
 *     BT_STATUS_IN_PROGRESS - The channel is already in the process
 *          of being moved or created.
 *
 *     BT_STATUS_FAILED - The channel is invalid or could not be
 *         moved.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this 
 *         channel is no longer active.
 */
BtStatus L2A_TxDataLen(L2capAgent *agent, U32 TotalLen);
#define L2A_TxDataLen(agent, TotalLen) (agent)->i.txDataLen(agent, TotalLen)
typedef BtStatus (*L2capAgentTxDataLen)(L2capAgent *agent, U32 TotalLen);

/*---------------------------------------------------------------------------
 * L2A_ConnectReqChanMgr()
 *
 *     This function is intended to be called by the L2capAgent Channel
 *     Manager to request the establishment of an L2CAP channel. 
 *
 * Parameters:
 *     agent - Pointer to object containing the L2CAP Agent interface.
 *
 *     pref - Pointer to preferences that will be used instead of the current
 *         preferences stored in the L2capAgent. If "pref" is set to 0 then
 *         the current preferences will be used.
 *
 *     settings - Pointer to L2CAP Connection settings that will be used
 *         instead of the settings currently stored in the L2capAgent. If
 *         "settings" is set to 0 then the current connection settings will
 *         be used.
 *
 * Returns:      
 *     BT_STATUS_PENDING - The connection process has been successfully
 *         initialized. When the connection process is complete, the
 *         handler in the L2capPsm structure will receive either the
 *         L2EVENT_CONNECTED or L2EVENT_DISCONNECTED L2capEventType.
 *
 *     BT_STATUS_FAILED - Protocol linkModes mismatch or the Controller
 *         preference could not be met.  
 * 
 *     BT_STATUS_NO_RESOURCES - L2CAP could not allocate a channel
 *         structure to initiate the connection.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link to the specified device
 *         is not active.
 *
 *     BT_STATUS_RESTRICTED - The PSM is not authorized to establish
 *         outbound connections.
 *
 *     BT_STATUS_IN_USE - Agent is already in use.
 *
 *     BT_STATUS_SECURITY - Operation failed because the correct security
 *         settings are not in place for using an AMP and the CPREFF_ONLY
 *         flag setting was set.
 *
 *     BT_STATUS_NOT_READY - Agent is in the wrong state for this call (i.e.
 *         this function was called when the agent was not attempting a connect
 *         request).
 */
BtStatus L2A_ConnectReqChanMgr(L2capAgent *agent, ControllerPreference *pref,
                               L2capConnSettings *settings);
#define L2A_ConnectReqChanMgr(agent,pref,settings) (agent)->i.connectReqChanMgr(agent,pref,settings)
typedef BtStatus (*L2capAgentConReqChanMgrFunc)(L2capAgent *agent,
                                                ControllerPreference *pref,
                                                L2capConnSettings *settings);

/*---------------------------------------------------------------------------
 * L2A_SetServiceId()
 *
 *     Set the service ID to a new value and return the old value.
 *
 * Parameters:
 *     agent - Pointer to object containing the L2CAP Agent interface
 *
 *     serviceId - the new service ID
 *
 * Returns:
 *     servicId - The Service ID
 *
 */
BtServiceId L2A_SetServiceId(L2capAgent *agent, BtServiceId serviceId);
#define L2A_SetServiceId(agent, serviceId) (agent)->i.setServiceId(agent, serviceId)
typedef BtServiceId (*L2capAgentSetServiceIdFunc)(L2capAgent *agent, BtServiceId serviceId);

/*---------------------------------------------------------------------------
 * L2A_GetServiceId()
 *
 *     Return the Agents ServiceId.
 *
 * Parameters:
 *     agent - Pointer to object containing the L2CAP Agent interface
 *
 * Returns:
 *     servicId - The Service ID
 *
 */
BtServiceId L2A_GetServiceId(L2capAgent *agent);
#define L2A_GetServiceId(agent) (agent)->i.getServiceId(agent)
typedef BtServiceId (*L2capAgentGetServiceIdFunc)(L2capAgent *agent);

/*---------------------------------------------------------------------------
 * L2A_IsPhyLinkAssociated - ** For Internal use only **
 *
 *     Return TRUE if the physical link is associated with this agent. 
 *
 * Parameters:
 *     agent - Pointer to object containing the L2CAP Agent interface
 *
 *     plink - Pointer to AMP physical link.
 *
 * Returns:
 *     TRUE if agent is associated with this physical link FALSE otherwise. 
 */
typedef BOOL (*L2capIsPhyLinkAssocFunc)(L2capAgent *agent, /* AmpPhyLink */ void *plink);

/*---------------------------------------------------------------------------
 * IL2capAgent structure
 *
 *    L2capAgent interface. This table can be included in any structure
 *    that wants to provide an L2capAgent interface.
 *
 */
typedef struct _IL2capAgent {

    L2capAgentDiscReq           disconnectReq;
    L2capAgentSendData          sendData;
    L2capAgentInitFunc          init;
    L2capAgentConReqFunc        connectReq;
    L2capAgentConRspFunc        connectRsp;
    L2capAgentMoveReq           moveReq;
    L2capAgentAutoMoveReq       autoMoveReq;
    L2capAgentConReqChanMgrFunc connectReqChanMgr;
    L2capAgentSetServiceIdFunc  setServiceId;
    L2capAgentGetServiceIdFunc  getServiceId;
    L2capAgentTxDataLen         txDataLen;
    L2capAgentGetPhyDescFunc    getPhyLinkDesc;
    L2capAgentSetLocalBusyFunc  setLocalBusy;
    L2capAgentGetConnModeFunc   getConnectionMode;
    L2capAgentGetTxMtuFunc      getTxMtu;
    L2capAgentFreeFunc          free;

    /* For internal use only */
    L2capIsPhyLinkAssocFunc     isPhyLinkAssociated;

} IL2capAgent;

#endif /* NUM_AMPS > 0 */

#endif /* __IL2CAPAGENT_H */
