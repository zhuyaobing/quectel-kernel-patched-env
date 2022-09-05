/****************************************************************************
 *
 * File:
 *     meamp.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     upper layer of the AMP part of the Management Entity.
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

#ifndef __MEAMP_H
#define __MEAMP_H

#include "osapi.h"
#include "bttypes.h"
#include "sys/ampcontrol.h"
#include "me.h"
#include "l2capagent.h"
#include "sys/a2me.h"
#include "ampmgr.h"

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * MEAMP_Init()
 *
 *     Initialize the AMP System and AMP controllers. 
 *
 * Parameters:
 *     void
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - AMP initialized successfully.
 *
 *     BT_STATUS_FAILED - AMP failed to initialize.
 */
BtStatus MEAMP_Init(void);

/*---------------------------------------------------------------------------
 * MEAMP_Deinit()
 *
 *     Deinitialize the AMP system. 
 *
 * Parameters:
 *     void
 *
 * Returns:      
 *     void
 *
 */
void MEAMP_Deinit(void);

#if NUM_AMPS > 0
/*---------------------------------------------------------------------------
 * MEAMP_AddController()
 *
 *     Add and initialize a controller to MEAMP. If the controller returns 
 *     BT_STATUS_SUCCESS when initialized it is added to the "active" list and
 *     given a controller ID. If the controller init() returns 
 *     BT_STATUS_NOT_READY the controller is added to the "pending" list. MEAMP
 *     will keep on that list until it reports success or failure in being
 *     initialized. If the controller init() returns BT_STATUS_FAILED it is
 *     placed on the disabled list and given a "disable" ID.
 *
 * Parameters:
 *     controller - pointer to controller
 *
 * Returns:      
 *     void
 *
 */
void MEAMP_AddController(AmpController *controller);

/*---------------------------------------------------------------------------
 * MEAMP_RemoveController()
 *
 *     Remove the controller from MEAMP and deinitialize it. If the controller
 *     was on the "active" list then handlers registered with the AMPMGR will
 *     receive the AMPCRLE_INIT_STATUS event.
 *
 * Parameters:
 *     controller - pointer to the controller
 *
 * Returns:      
 *     void
 *
 */
void MEAMP_RemoveController(AmpController *controller);

/*---------------------------------------------------------------------------
 * MEAMP_DisableController()
 *
 *     Cause the controller to be put on the "disabled" list. When a controller
 *     is on the "disabled" list it cannot be used and is not reported to
 *     remote devices. The controller is informed that is disabled so it will
 *     stop responding to events from remote devices. It will receive a "disabled"
 *     ID. It current "active" ID will returned to the pool of active IDs and may
 *     assigned to another controller. If disabling the controller is successful
 *     all handlers registered with the AMPMGR will receive the AMPCRLE_INIT_STATUS
 *     event.
 *
 * Parameters:
 *     controller - pointer to the controller
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - operation completed successfully
 *
 *     BT_STATUS_PENDING - operation has been started. All handler registered 
 *         with AMPMGR will receive the AMPCRLE_INIT_STATUS event when the
 *         operation is complete.
 *
 *     BT_STATUS_RESTRICTED - operation failed because the controller is already
 *         in the disabled state or in the process of becoming disabled.
 *
 *     BT_STATUS_FAILED - operation failed because the controller is still
 *         active (has physical links with other devices).
 *
 */
BtStatus MEAMP_DisableController(AmpController *controller);

/*---------------------------------------------------------------------------
 * MEAMP_EnableController()
 *
 *     Cause a disabled controller to be put on the "active" list. The
 *     controller is told to enable itself. If the operation is successful
 *     all handlers registered with AMPMGR will receive the AMPCRLE_INIT_STATUS
 *     event.
 *
 *
 * Parameters:
 *     controller - pointer to the controller
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - operation completed successfully
 *
 *     BT_STATUS_PENDING - operation has been started. All handler registered 
 *         with AMPMGR will receive the AMPCRLE_INIT_STATUS event when the
 *         operation is complete.
 *
 *     BT_STATUS_RESTRICTED - operation failed because the controller is already
 *         in the active state or in the process of becoming enabled.
 *
 *     BT_STATUS_FAILED - operation failed because the controller could not
 *         be enabled. 
 */
BtStatus MEAMP_EnableController(AmpController *controller);


/*---------------------------------------------------------------------------
 * MEAMP_GetNextLocalController()
 *
 *     This function is an interator for getting local controllers. To get
 *     a pointer to the first controller set ampc to 0. To get the next 
 *     controller set ampc to the value returned from the last call. 
 *     When there are no more controllers 0 is returned.
 *
 *     The list could change in between calls so this function should be called 
 *     in a loop with the stack locked or by a processing running on the
 *     stack thread.
 *
 * Parameters:
 *     ampc - pointer to context used by the iterator.
 *
 * Returns:      
 *     Pointer to next local AMP controller or 0 
 *
 */
AmpController *MEAMP_GetNextLocalController(AmpController *ampc);

/*---------------------------------------------------------------------------
 * MEAMP_MEAMP_GetNextDisabledController()
 *
 *     This function is an interator for getting disabled controllers. To get
 *     a pointer to the first controller set ampc to 0. To get the next 
 *     controller set ampc to the value returned from the last call. 
 *     When there are no more controllers 0 is returned.
 *
 *     The list could change in between calls so this function should be called 
 *     in a loop with the stack locked or by a processing running on the
 *     stack thread.
 *
 * Parameters:
 *     ampc - pointer to context used by the iterator.
 *
 * Returns:      
 *     Pointer to next disabled AMP controller or 0 
 *
 */
AmpController *MEAMP_GetNextDisabledController(AmpController *ampc);

/*---------------------------------------------------------------------------
 * MEAMP_GetRemoteControllerList()
 *
 *     Return a pointer to the remote controller list for the remote device
 *     specified by the remDev. 
 *
 * Parameters:
 *     remDev - Pointer to remote device structure identifying the remote
 *         device. 
 *
 *     list - pointer to memory to receive the Remote Controller list
 *         pointer. 
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - list returned successfully. 
 *
 *     BT_STATUS_PENDING -  list not returned. A list discover operation has
 *         been started. All agents waiting for the list will be notified.
 *
 *     BT_STATUS_FAILED - unable to return a list.
 *
 */
BtStatus MEAMP_GetRemoteControllerList(BrEdrRemoteDevice *remDev, 
                                       BtRemoteControllerList **list);

/*---------------------------------------------------------------------------
 * MEAMP_GetRemoteAmpInfo()
 *
 *     Copy the AMP info into memory pointed to by ampInfo for the remote
 *     AMP controller identified by ampId for the remote device identified
 *     by remDev. If a discovery of remote AMPs has not been performed then 
 *     return BT_STATUS_RESTRICTED. AMP Info can only be obtained if the remote 
 *     AMP list has first been obtained for the remote device.
 *
 * Parameters:
 *     remDev - Pointer to remote device structure identifying the remote
 *         device. 
 *
 *     ampId - ID of the remote AMP controller
 *
 *     ampInfo - pointer to BtAmpInfo structure to received AMP info.
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - operation was successful
 *
 *     BT_STATUS_PENDING - operation is pending. Registered handlers will
 *         be called when operation completes.
 *
 *     BT_STATUS_RESTRICTED - operation failed because remote AMP discovery
 *         has not been performed
 *
 *     BT_STATUS_FAILED - operation failed.
 */
BtStatus MEAMP_GetRemoteAmpInfo(BrEdrRemoteDevice *remDev, U8 ampId, 
                                BtRemoteAmpInfo *ampInfo);

/*---------------------------------------------------------------------------
 * MEAMP_FindNextPhyLink()
 *
 *     Return a pointer to an AmpPhyLink object that is operating  with 
 *     the given Remote Device. This function can be used to retrieve all 
 *     AmpPhyLinks operating with the given Remote device by obtaining the 
 *     first one with plink = 0 then retrieving the next one using the first 
 *     plink and so forth. 
 *
 *     The phylink list could change in between calls so this call should be 
 *     called in a loop with the stack locked or by a process running on the 
 *     stack thread.
 *
 * Parameters:
 *     plink - Pointer to an AmpPhyLink object from where the search 
 *        should start. Use 0 to start at the beginning. 
 *
 *     remDev - pointer Remote Device with which the AmpPhyLink is operating
 *
 * Returns:      
 *     Pointer to an AmpPhyLink that is currently operating with the Remote
 *     device or 0 if none are found. Note that the AmpPhyLink maybe in the
 *     process of being connected. 
 *
 */
AmpPhyLink *MEAMP_FindNextPhyLink(AmpPhyLink *plink, BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * MEAMP_FindNextPhyLinkAny()
 *
 *     Return a pointer to the next AmpPhyLink object. This function can be 
 *     used to retrieve all active AmpPhyLinks by obtaining the first one 
 *     with plink = 0 then retrieving the next one using the first plink and 
 *     so forth. 
 *
 *     The phylink list could change in between calls so this call should be 
 *     called in a loop with the stack locked or by a process running on the 
 *     stack thread.
 *
 * Parameters:
 *     plink - Pointer to an AmpPhyLink object from where the search 
 *        should start. Use 0 to start at the beginning. 
 *
 * Returns:      
 *     Pointer to an AmpPhyLink or 0 if none are found. Note that the 
 *     AmpPhyLink maybe in the process of being connected. 
 *
 */
AmpPhyLink *MEAMP_FindNextPhyLinkAny(AmpPhyLink *plink);

/*---------------------------------------------------------------------------
 * MEAMP_CreatePhyLink()
 *
 *     Create an AMP physical link to the remove device identified by RemDev,
 *     using the local controller ampc to the remote controller identified
 *     by remControlId. A pointer to the AMP physical link is copied
 *     into the memory pointed to by phyLink if phyLink is not 0. 
 *
 * Parameters:
 *     ampc - pointer to local AMP Controller
 *
 *     RemDev - pointer to BtRemoteDevice structure of remote device.
 *
 *     remControlId - Controller ID of controller on remote deivce to which
 *         connnection is to be made.
 *
 *     phyLink - pointer to memory location to receive AMP physical link
 *         pointer.
 *
 * Returns:      
 *     BT_STATUS_PENDING - operation is pending. All handlers registered 
 *         with the AMPMGR will be called with AMPMGRE_PHY_LINK_CREATE 
 *         when operation completes. All L2capAgents waiting for this physical link
 *         will be called with AMPCRLE_PHY_CREATE when the operation completes.
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because the controller
 *         has no AmpPhyLink structures available. 
 *
 *     BT_STATUS_IN_PROGRESS - operation failed a create connection is
 *        already in progress. 
 *
 *     BT_STATUS_IN_USE - operation failed because a physical link already
 *        exists to the remote device between the local controller and the
 *        remote controller. 
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because the ACL link 
 *         to the remote device is no longer active.
 *
 *     BT_STATUS_FAILED - operation failed because the remote device
 *         does not support AMP operations.
 */
BtStatus MEAMP_CreatePhyLink(AmpController *ampc, BrEdrRemoteDevice *RemDev, 
                             U8 remControlId, AmpPhyLink **phyLink);


/*---------------------------------------------------------------------------
 * MEAMP_AcceptPhyLink()
 *
 *     Accept an incoming AMP physical link request.  All handlers registered 
 *     with the AMPMGR will be called with AMPMGRE_PHY_LINK_CREATE when the
 *     operation completes. All L2capAgents waiting for this physical link will
 *     be called with AMPCRLE_PHY_CREATE when the operation completes.
 *
 * Parameters:
 *     ampc - pointer to local AMP Controller
 *
 *     RemDev - pointer to BtRemoteDevice structure of remote device.
 *
 *     remControlId - Controller ID of controller on remote device to which
 *         connnection is to be made.
 *
 *     ampAssocLen - length of the AMP Assoc from the remote device
 *
 *     ampAssoc - pointer to a buffer holding the AMP Assoc structure received
 *         from the remote device
 *
 * Returns:      
 *     BT_STATUS_PENDING - operation is pending. All handlers registered 
 *         with the AMPMGR will be called with AMPMGRE_PHY_LINK_CREATE 
 *         when operation completes. All L2capAgents waiting for this physical link
 *         will be called with AMPCRLE_PHY_CREATE when the operation completes.
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because the controller
 *         has no AmpPhyLink structures available. 
 *
 *     BT_STATUS_IN_PROGRESS - operation failed a create connection is
 *        already in progress. 
 *
 *     BT_STATUS_IN_USE - operation failed because a physical link already
 *        exists to the remote device between the local controller and the
 *        remote controller. 
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because the ACL link 
 *         to the remote device is no longer active.
 *
 *     BT_STATUS_FAILED - operation failed because the remote device
 *         does not support AMP operations.
 *
 */
BtStatus MEAMP_AcceptPhyLink(AmpController *ampc, BrEdrRemoteDevice *RemDev, 
                             U8 remControlId, U16 ampAssocLen, U8 *ampAssoc);


/*---------------------------------------------------------------------------
 * MEAMP_DisconnectPhyLink()
 *
 *     Disconnect the AMP physical link. 
 *
 * Parameters:
 *     RemDev - pointer to BtRemoteDevice structure of remote device.
 *
 *     localCrlId - Controller ID of the local controller on which
 *         connection exists.
 *
 *     remControlId - Controller ID of controller on remote device on which
 *         connnection exists.
 *
 *     client - Client to be removed from the physical link. If all clients
 *         and logical links have been removed the physical link is disconnected.
 *         If the client 0 the physical link is disonnected independent
 *         of the number clients or logical links associated with the physical
 *         link (i.e. force disconnect). 
 *
 * Returns:      
 *     BT_STATUS_PENDING - Operation has be started successfully.  All 
 *         handler registered with the AMPMGR will be called with 
 *         AMPMGRE_PHY_LINK_DISCONNECT when the operation completes. The
 *         exception is if the disconnect is called on a physical link
 *         that has just started to come up (i.e. the disconnect is called 
 *         before the AMPMGRE_PHY_LINK_CREATE is given. There are two possible 
 *         results: handlers will be called with an 
 *         AMPMGRE_PHY_LINK_CREATE event with failed status and no
 *         AMPMGR_PHY_LINK_DISCONNECT event or handlers will be called with
 *         both an AMPMGRE_PHY_LINK_CREATE event with failed status and 
 *         an AMPMGR_PHY_LINK_DISCONNECT event (in either order). 
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because the AMP physical
 *         link does not exist.
 *
 */
BtStatus MEAMP_DisconnectPhyLink(BrEdrRemoteDevice *RemDev, U8 localCrlId,
                                 U8 remControlId, ListEntry *client);

/*---------------------------------------------------------------------------
 * MEAMP_CheckForDisconnect()
 *
 *     If the AMP physical link is not busy then disconnect it. 
 *
 * Parameters:
 *     plink - pointer to AMP physical link to disconnect.
 *
 * Returns:      
 *     BT_STATUS_PENDING - Operation has be started successfully.  All 
 *         handler registered with the AMPMGR will be called with 
 *         AMPMGRE_PHY_LINK_DISCONNECT when the operation completes. 
 *
 *     BT_STATUS_NO_CONNECTION - Request failed because there is no connection
 *
 *     BT_STATUS_HCI_INIT_ERR - The lower level transport is not in an initialized
 *         state and therefore the operation cannot proceed or the controller is 
 *         disabled.
 *
 *     BT_STATUS_BUSY - operation failed because the AMP physical link is busy
 *         (i.e. it has registered clients and/or L2capAgents associated with it).
 */
BtStatus MEAMP_CheckForDisconnect(AmpPhyLink *plink);

#endif /* NUM_AMPS > 0 */

/*---------------------------------------------------------------------------
 * MEAMP_AllocateAgent()
 *
 *     Allocate a L2capAgent structure from the system pool. The constructor
 *     is called on the L2capAgent structure and the free() function is
 *     set to point to the system free function.
 *
 * Parameters:
 *     void
 *
 * Returns:      
 *     Pointer to L2capAgent structure or 0 if none are available.
 *
 */
L2capAgent *MEAMP_AllocateAgent(void);

/*---------------------------------------------------------------------------
 * MEAMP_FreeAgent(()
 *
 *     Return the L2capAgent structure back to the pool. 
 *
 * Parameters:
 *     agent - pointer to L2capAgent structure.
 *
 * Returns:      
 *     void
 *
 */
void MEAMP_FreeAgent(L2capAgent *agent);


/*---------------------------------------------------------------------------
 * MEAMP_AddAgent()
 *
 *     Supplies an agent to MEAMP. While added, MEAMP will notify the agent
 *     of physical link creation (AMPCRLE_PHY_CREATE) and controller discovery
 *     (A2ME_EVENT_DISCOVER_RSP) events.
 *
 * Parameters:
 *     agent - The agent to be managed by MEAMP. May already have been added,
 *          in which case this function has no effect.
 *
 */
void MEAMP_AddAgent(L2capAgent *agent);

/*---------------------------------------------------------------------------
 * MEAMP_RemoveAgent()
 *
 *     Removes an agent from MEAMP's management. Must be called once for
 *     every call to MEAMP_AddAgent.
 *
 * Parameters:
 *     agent - The agent to be removed from MEAMP. Safe to call even if the
 *          agent was already removed.
 *
 */
void MEAMP_RemoveAgent(L2capAgent *agent);

/*---------------------------------------------------------------------------
 * MEAMP_IsAgentInUse()
 *
 *     Indicates whether an agent is being managed by MEAMP (i.e. has been
 *     added to MEAMP).
 *
 * Parameters:
 *     agent - pointer to L2capAgent structure.
 *
 * Returns:      
 *     TRUE - agent is managed by MEAMP
 *
 *     FALSE - agent is not managed by MEMAP
 */
BOOL MEAMP_IsAgentInUse(L2capAgent *agent);

/*---------------------------------------------------------------------------
 * MEAMP_FindPhyLink()
 *
 *     Return a pointer to an AmpPhyLink object that is operating with 
 *     the given Remote Device using the given local AMP Controller. If
 *     the AmpPhyLink is not found return 0.
 *
 * Parameters:
 *     remDev - pointer Remote Device with which the AmpPhyLink is operating
 *
 *     ampC - Pointer to local AMP Controller.
 *
 * Returns:      
 *     Pointer to the AmpPhyLink if found otherwise returns 0
 *
 */
AmpPhyLink * MEAMP_FindPhyLink(BtRemoteDevice *remDev, AmpController *ampC);

/*---------------------------------------------------------------------------
 * MEAMP_FindAmp()
 *
 *     Return a pointer to the local AMP Controller with the given crlID. Return 0
 *     if the AMP Controller cannot be found.
 *
 * Parameters:
 *     crlId - ID of the AMP Controller
 *
 * Returns:      
 *     Pointer to AMP Controller or 0
 *
 */
AmpController *MEAMP_FindLocalAmp(U8 crlId);

/*---------------------------------------------------------------------------
 * MEAMP_IsSecurityOkForAmp()
 *
 *     Returns TRUE if the current security is ok to use AMPs otherwise it
 *     returns FALSE. Creating a physical link between AMP Controllers 
 *     requires that Secure Simple Pairing authentication has occurred and
 *     that encryption is enabled.
 *
 * Parameters:
 *     remDev - pointer to active Remote Device Structure.
 *
 * Returns:      
 *     TRUE - Security is ok for create an AMP physical link
 *     FALSE - otherwise.
 *
 */
BOOL MEAMP_IsSecurityOkForAmp(BrEdrRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * MEAMP_IsSspSupportedByLocalBrEdr()
 *
 *     Returns TRUE if the local BR/EDR Controller supports Secure Simple
 *     pairing otherwise it returns FALSE. If the local BR/EDR does not
 *     support secure simple pairing then the A2ME will respond to
 *     an AMP Discover Request with a Controller list showing only
 *     the BR/EDR controller. 
 *
 * Parameters:
 *
 * Returns:      
 *     TRUE - Secure Simple Pairing is supported by local BR/EDR controller
 *     FALSE - otherwise.
 *
 */
BOOL MEAMP_IsSspSupportedByLocalBrEdr(void);

#endif /* __MEMAMP_H */
