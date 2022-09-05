/****************************************************************************
 *
 * File:
 *     ia2me.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
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

#ifndef __IA2ME_H
#define __IA2ME_H

#include "bttypes2.h"
#include "me.h"
#include "ampmgr.h"

/* -- A2ME Events -------------------------------------------------------
 * A2meEventType
 *
 *     A2ME Events indicated to the layer above. The last byte of these
 *     events match the opcodes in the A2ME Request/Response packets.
 */
typedef U32 A2meEventType;

#define A2ME_EVENT_COMMAND_REJ          (BTE_TYPE_BASE_A2ME + 0x01)
#define A2ME_EVENT_DISCOVER_REQ         (BTE_TYPE_BASE_A2ME + 0x02)

/* Indicate that a discover response was received. The p.data field
 * contains a pointer to the start of the remote controller list.
 * The save controller list can be accessed via
 * remdev->a2meContext->remoteAmpControllerList. This is the list
 * used by MEAMP and L2CAP Agent when creating a phylink to
 * to a remote AMP.
 */
#define A2ME_EVENT_DISCOVER_RSP         (BTE_TYPE_BASE_A2ME + 0x03)
#define A2ME_EVENT_CHANGE_NOTIFY        (BTE_TYPE_BASE_A2ME + 0x04)
#define A2ME_EVENT_CHANGE_RSP           (BTE_TYPE_BASE_A2ME + 0x05)
#define A2ME_EVENT_GET_INFO_REQ         (BTE_TYPE_BASE_A2ME + 0x06)
#define A2ME_EVENT_GET_INFO_RSP         (BTE_TYPE_BASE_A2ME + 0x07)
#define A2ME_EVENT_GET_AMP_ASSOC_REQ    (BTE_TYPE_BASE_A2ME + 0x08)
#define A2ME_EVENT_GET_AMP_ASSOC_RSP    (BTE_TYPE_BASE_A2ME + 0x09)
#define A2ME_EVENT_CREATE_PHY_REQ       (BTE_TYPE_BASE_A2ME + 0x0A)
#define A2ME_EVENT_CREATE_PHY_RSP       (BTE_TYPE_BASE_A2ME + 0x0B)
#define A2ME_EVENT_DISC_PHY_REQ         (BTE_TYPE_BASE_A2ME + 0x0C)
#define A2ME_EVENT_DISC_PHY_RSP         (BTE_TYPE_BASE_A2ME + 0x0D)

/* End of A2meEventType */

/*-----------------------------------------------------------------------
 * A2meRemoteAmpInfo structure
 *
 *     Holds AMP Info for remote AMPs. AMP info is only cached if
 *     A2ME_CACHE_REMOTE_AMP_INFO is set XA_ENALBED.
 */  
typedef struct _A2meRemoteAmpInfo {
    U8               id;   /* Since 0 is invalid a 0 indcates the entry is empty */
    BtRemoteAmpInfo  ampInfo;
} A2meRemoteAmpInfo;

/*-----------------------------------------------------------------------
 * A2meEvent structure
 *
 *     Provides information to the A2ME user about an A2meCallback event.
 */  
typedef struct _A2meEvent
{
    /* A2ME upper layer event. Always valid. */
    BtEventEx       e;

    BtRemoteDevice *remDev;
    
    /* One or both are valid for some events */
    U8              localCrlId;
    U8              remoteCrlId;

    /* Status returned in A2ME response packets. Valid for the given
     * events: A2ME_EVENT_GET_INFO_RSP, A2ME_EVENT_GET_AMP_ASSOC_RSP,
     * A2ME_EVENT_CREATE_PHY_RSP, A2ME_EVENT_DISC_PHY_RSP 
     */
    U8              rspStatus;

    /* Parameters */
    union {
         /* Pointer to amp assoc */
        struct {
            U16  len;
            U8  *ptr;
        } ampAssoc;

        /* Pointer to general data */
        struct {
            U16 len;
            U8 *ptr;
        } data;

        /* Pointer to AMP info */
        BtRemoteAmpInfo *ampInfo;
    } p;

} A2meEvent;

/****************************************************************************
 *
 * Interface Functions for interface iA2me
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * A2meCallback type
 *
 *     A function of this type is called to indicate A2ME events to
 *     the layer above.
 */
typedef void (*A2meCallback)(void *ctx, A2meEvent *Event);

/* End of A2meCallback */

/*---------------------------------------------------------------------------
 * A2ME_DiscoverReq()
 *
 *     Request the list of AMP Controllers from the remote device. 
 *
 * Parameters:
 *     ctx - Pointer to the A2ME context.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation was successfully started. Completion
 *         is indicated by an A2ME_EVENT_DISCOVER_RSP event to the
 *         registered callback.
 *
 *     BT_STATUS_FAILED - The A2ME channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this
 *         A2ME channel is no longer active.
 *
 *     BT_STATUS_NO_RESOURCES - No TxPackets available to send the request.
 */
typedef BtStatus (*A2meDiscoverReqFunc)(void *ctx);

/*---------------------------------------------------------------------------
 * A2ME_ChangeNotify()
 *
 *     Indicate that the status of a local AMP controller or the list of local
 *     AMP controllers has changed. 
 *
 * Parameters:
 *     ctx - Pointer to the A2ME context.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation was successfully started. Completion
 *         is indicated by an A2ME_EVENT_CHANGE_NOTIFY event to the
 *         registered callback.
 *
 *     BT_STATUS_FAILED - The A2ME channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this
 *         A2ME channel is no longer active.
 *
 *     BT_STATUS_RESTRICTED - notifcation is not allowed because the remote
 *         device has not yet sent a discovery Request or the local controller
 *         does not support Secure Simple Pairing.
 */
typedef BtStatus (*A2meChangeNotifyFunc)(void *ctx);

/*---------------------------------------------------------------------------
 * A2ME_GetAmpAssocReq()
 *
 *     Requests the AMP Assoc of the specified Controller ID.
 *
 * Parameters:
 *     ctx - Pointer to the A2ME context.
 *
 *     ControllerId - The Controller ID of the desired AMP Assoc.
 *
 * Returns:      
 *     BT_STATUS_PENDING - Request was successfully queued for transmission.
 *         The completion of the request is indicated by an
 *         L2EVENT_PACKET_HANDLED event.
 *
 *     BT_STATUS_FAILED - The A2ME channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this
 *         A2ME channel is no longer active.
 */
typedef BtStatus (*A2meGetAmpAssocReqFunc)(void *ctx, U8 ControllerId);

/*---------------------------------------------------------------------------
 * A2ME_GetInfoReq()
 *
 *     Request AMP info related to the specified remote controller ID.
 *
 * Parameters:
 *     ctx - Pointer to the A2ME context.
 *
 *     ControllerId - The Controller ID of the requested AMP Info.
 *
 *
 * Returns:      
 *     BT_STATUS_PENDING - Request was successfully queued for transmission.
 *         The completion of the request is indicated by an
 *         L2EVENT_PACKET_HANDLED event.
 *
 *     BT_STATUS_FAILED - The A2ME channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this
 *         A2ME channel is no longer active.
 */
typedef BtStatus (*A2meGetInfoReqFunc)(void *ctx, U8 RemoteControlId);

/*---------------------------------------------------------------------------
 * A2ME_CreatePhysicalLinkReq()
 *
 *     Request creation of a physical link.
 *
 * Parameters:
 *     ctx - Pointer to the A2ME context.
 *
 *     LocalControlId - The Controller ID of the sending device.
 *
 *     RemoteControlId - The Controller ID of the receiving device.
 *
 *     AmpAssoc - The AMP Assoc structure associated with this request.
 *
 *     AmpAssocLen - The Length of the AMP Assoc structure.
 *
 * Returns:      
 *     BT_STATUS_PENDING - Request was successfully queued for transmission.
 *         The completion of the request is indicated by an
 *         L2EVENT_PACKET_HANDLED event.
 *
 *     BT_STATUS_FAILED - The A2ME channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this
 *         A2ME channel is no longer active.
 */
typedef BtStatus (*A2meCreatePhyLinkReqFunc)(void *ctx, U8 LocalControlId,
                                            U8 RemoteControlId, U8 *AmpAssoc, 
                                            U16 AmpAssocLen);

/*---------------------------------------------------------------------------
 * A2ME_DisconnectPhysicalLinkReq()
 *
 *     Request disconnection of a physical link.
 *
 * Parameters:
 *     ctx - Pointer to the A2ME context.
 *
 *     LocalControlId - The Controller ID of the sending device.
 *
 *     RemoteControlId - The Controller ID of the receiving device.
 *
 * Returns:      
 *     BT_STATUS_PENDING - Request was successfully queued for transmission.
 *         The completion of the request is indicated by an
 *         L2EVENT_PACKET_HANDLED event.
 *
 *     BT_STATUS_FAILED - The A2ME channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this
 *         A2ME channel is no longer active.
 */
typedef BtStatus (*A2meDiscPhyLinkReqFunc)(void *ctx, U8 LocalControlId,
                                           U8 RemoteControlId);

/*---------------------------------------------------------------------------
 * A2ME_GetRemoteAmpControllerList()
 *
 *     Return a pointer to the remote controller list. If this list
 *     is unavailable because it needs to be retrieved from the remote device
 *     then return 0.
 *
 * Parameters:
 *     ctx - Pointer to the A2ME context.
 *
 * Returns:      
 *     Pointer to the list or 0.
 */
typedef BtRemoteControllerList *(*A2meGetRemAmpControlListFunc)(void *ctx);

/*---------------------------------------------------------------------------
 * A2ME_GetRemoteAmpInfo()
 *
 *     Copy the AMP info into memory pointed to by ampInfo for the remote
 *     AMP controller identified by ampId. If a discovery of remote AMPs
 *     has not been performed then return BT_STATUS_RESTRICTED. AMP Info
 *     can only be obtained if the remote AMP list has first been obtained.
 *
 * Parameters:
 *     ctx - Pointer to the A2ME context.
 *
 *     ampId - ID of the remote AMP controller
 *
 *     ampInfo - pointer to BtAmpInfo structure to received AMP info.
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - operation was successful
 *
 *     BT_STATUS_PENDING - operation is pending. Registered handler will
 *         be called when operation completes.
 *
 *     BT_STATUS_RESTRICTED - operation failed because remote AMP discovery
 *         has not been performed
 *
 *     BT_STATUS_FAILED - operation failed.
 */
typedef BtStatus (*A2meGetRemAmpInfoFunc)(void *ctx, U8 ampId, BtRemoteAmpInfo *ampInfo);

/*---------------------------------------------------------------------------
 * A2me Interface structure
 *
 */
typedef struct _IA2me {
    A2meDiscoverReqFunc          discoverReq;
    A2meChangeNotifyFunc         changeNotify;
    A2meGetAmpAssocReqFunc       getAmpAssocReq;
    A2meGetInfoReqFunc           getInfoReq;
    A2meCreatePhyLinkReqFunc     createPhyLinkReq;
    A2meDiscPhyLinkReqFunc       discPhyLinkReq;
    A2meGetRemAmpControlListFunc getRemAmpControlList;
    A2meGetRemAmpInfoFunc        getRemoteAmpInfo;
} IA2me;

#endif /* __IA2ME_H */
