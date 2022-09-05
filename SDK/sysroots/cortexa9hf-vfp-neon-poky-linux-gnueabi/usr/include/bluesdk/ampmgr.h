/****************************************************************************
 *
 * File:
 *     ampmgr.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Interface for the AMP Manager Module.
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

#ifndef __AMPMGR_H
#define __AMPMGR_H

#include "bttypes2.h"
#include "me.h"

/*---------------------------------------------------------------------------
 * AMP Manager (AMPMGR) layer
 *
 *     The AMP Manager allows applications to discover local and remote
 *     AMP controllers, get information about AMP controllers, find existing
 *     physical links involving AMP controllers, create AMP physical links
 *     and disconnect AMP physical links. 
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AmpMgrEventType type
 *
 *     The AMP Manager event types are passed to all registered handlers based
 *     on the eMask. Handlers are registered using AMPMGR_Register(). The event
 *     types are defined below. They are offsets from BTE_TYPE_BASE_AMPMGR 
 *     defined in bttypes2.h
 */
typedef BtEventTypeEx AmpMgrEventType;

/* Group: Events sent to all handlers registered with the AMPMGR. These can be 
 * masked out if  desired.
 */

/* The list of active local AMP controllers has changed. This occurs when 
 * new controllers are added or removed. "localCrlId" identifies the AMP 
 * controller. "e.status" indicates the status of the controller. 
 * BT_STATUS_SUCCESS indicates that a controller has been added to the active 
 * list. BT_STATUS_NOT_READY indicates a controller has moved from the active 
 * state (active list) to the pending list. A controller cannot be used in 
 * this state but is in the process of becoming active again. When it becomes 
 * active it will get a new ID. BT_STATUS_FAILED indicates a controller has
 * become disabled and is not usable. When the status is BT_STATUS_FAILED  
 * "localCrlId" contains the active ID of the controller and "p.disabledCrlId" 
 * contains the controller's disabled ID. Now that the controller is disabled 
 * its active ID is no longer valid and may get assigned to a controller that 
 * becomes active. Disabled controllers can be renabled using 
 * AMPMGR_EnableLocalController() passing in the disabled ID. Use 
 * AMPMGR_GetLocalAmpList() to get the  most-up-to date list of active local 
 * AMP Controllers and AMPMGR_GetDisabledAmpList() to a get a list of disabled 
 * controllers.
 */
#define AMPMGRE_CONTROLLER_CHANGE    (BTE_TYPE_BASE_AMPMGR + 0)

/* The AMP status of a local AMP controller has changed. "localCrlId" identifies 
 * the local AMP controller that has changed and "p.ampStatus" indicates the 
 * new status. Use AMPMGR_GetLocalAmpList() to get the most up-to-date list of 
 * active local AMP Controllers along with their most up-to-date AMP status.
 */
#define AMPMGRE_CONTROLLER_STATUS    (BTE_TYPE_BASE_AMPMGR + 1)

/* Status of AMPs on a remote device has changed. This event occurs when AMPs 
 * on the remote device have been added/removed or when the AMP status of a 
 * remote AMP has changed. "p.remDev" identifies the remote device on which the 
 * change has occurred. The remote device has sent a new AMP list so
 * AMPMGR_GetRemoteAmpList() should be called to get the most update to date
 * information on a remote Device's AMPs.
 */
#define AMPMGRE_REMOTE_CHANGE        (BTE_TYPE_BASE_AMPMGR + 2)

/* A request for the list of AMP controllers for a remote device has completed. 
 * "e.status" indicates whether the operation was successful or not and "remDev" 
 * points to the BtRemoteDevice structure representing the remote device. The
 * event does not contain the AMP controller list. Use AMPMGR_GetRemoteAmpList()
 * (again) to get the remote AMP controller list.
 */
#define AMPMGRE_REMOTE_AMP_LIST      (BTE_TYPE_BASE_AMPMGR + 3)

/* A request for the AMP info for an AMP controller on a remote device 
 * has completed. "e.status" indicates whether the operation was successful or 
 * not. "remDev" points to the BtRemoteDevice structure identifying the remote 
 * device and "remCrlId" indicates the ID of the remote device's AMP controller. 
 * The AMP info is in "p.ampInfo".  
 */
#define AMPMGRE_GET_REMOTE_AMP_INFO   (BTE_TYPE_BASE_AMPMGR + 4)

/* Creation of an AMP physical link is complete. The "status" field indicates if 
 * creation of the physical link was successful or not. The physical link is
 * identified by three fields in the event: "remDev" identifies the remote device,
 * "remCrlId" identifies the remote AMP controller and "localCrlId" identifies the 
 * local controller. This event occurs regardless of how the physical link was 
 * created (e.g. by the remote device, calling AMPMGR_CreateAmpPhyLink(), or by 
 * an L2capAgent in creating an L2CAP channel).
 */
#define AMPMGRE_PHY_LINK_CREATE       (BTE_TYPE_BASE_AMPMGR + 5)

/* An AMP physical link had been disconnected. The physical link is
 * identified by three fields in the event: "remDev" identifies the remote device,
 * "remCrlId" identifies the remote device's AMP controller and "localCrlId" 
 * identifies the local AMP controller. The reason for the disconnect is in 
 * "p.errCode".
 */
#define AMPMGRE_PHY_LINK_DISCONNECT   (BTE_TYPE_BASE_AMPMGR + 6)

/* An AMP controller has acknowledged a command sent earlier with
 * AMPMGR_SendHciCommand(). The command sent earlier is in "p.hci.cmd".
 * "localCrlId" identifies the controller acknowledging the command.
 * 
 * If "e.status" is BT_STATUS_SUCCESS, the command was acknowledged by the
 * controller. The controller's response code is in "p.hci.errCode".
 * If the command completed with success (i.e. the controller issued
 * an HCE_COMMAND_COMPLETE with HC_STATUS_SUCCESS status code), 
 * "p.hci.evt" will contain controller-provided Return Parameters starting
 * with the one-byte "Status" code. 
 *
 * HCI commands acknowledged with HCE_COMMAND_STATUS are indicated as
 * described above. If and when the controller supplies additional events,
 * AMPMGRE_HCI_EVENT_IND events will be delivered.
 * 
 * If "e.status" contains any other value, the command could not be sent
 * and "p.hci.evt" is invalid.
 */
#define AMPMGRE_HCI_COMMAND_CNF       (BTE_TYPE_BASE_AMPMGR + 7)

/* An AMP controller has provided an event which may be of interest to the
 * AMPMGR user. "p.hci.evt" contains the event. "remDev" and "localCrlId"
 * identify the controller delivering the event.
 */
#define AMPMGRE_HCI_EVENT_IND         (BTE_TYPE_BASE_AMPMGR + 8)

/* End of AmpMgrEventType */


/*---------------------------------------------------------------------------
 * AmpMgrEventMask type
 *
 *     The AMP Manager event mask is defined below. It is used to mask out
 *     undesired events. To receive all events set the event mask to 
 *     AMPMGRE_MASK_ALL. To ignore an event AND out the event from AMPMGRE_MASK_ALL
 *     as shown in the example below.
 *
 *     mask = AMPMGR_MASK_ALL & ~AMPMGRE_MASK_CONTROLLER_STATUS;
 *
 *     Create a mask with only desired events by ORing together the values for the
 *     desired events.
 */

typedef U32 AmpMgrEventMask;

#define AMPMGRE_MASK_CONTROLLER_CHANGE    0x00000001
#define AMPMGRE_MASK_CONTROLLER_STATUS    0x00000002
#define AMPMGRE_MASK_REMOTE_CHANGE        0x00000004
#define AMPMGRE_MASK_REMOTE_AMP_LIST      0x00000008
#define AMPMGRE_MASK_REMOTE_AMP_INFO      0x00000010
#define AMPMGRE_MASK_PHY_LINK_CREATE      0x00000020
#define AMPMGRE_MASK_PHY_LINK_DISCONNECT  0x00000040
#define AMPMGRE_MASK_HCI_COMMAND_CNF      0x00000080
#define AMPMGRE_MASK_HCI_EVENT_IND        0x00000100
#define AMPMGRE_MASK_ALL                  0xFFFFFFFF

/* End of AmpMgrEventMask */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AmpMgrEvent structure
 *
 *     Used to pass information from the AMP Manager to applications. Not
 *     all fields are valid at any given time. The type of event determines 
 *     which fields are valid. This structure inherits from BtEventEx. 
 */
typedef struct _AmpMgrEvent {
    BtEventEx       e;
    BtRemoteDevice *remDev;
    U8              remCrlId;
    U8              localCrlId;

    union {
        BtRemoteAmpInfo ampInfo;
        BtAmpStatus     ampStatus;
        U8              disabledCrlId;
        HcStatus        errCode;
        struct {
            HcStatus        errCode;
            HciCommand     *cmd;
            const HciEvent *evt;
        } hci;
    } p;

} AmpMgrEvent;

/*---------------------------------------------------------------------------
 * AmpMgrHandler structure
 *
 *     An AMP Manager handler is registered with the AMP Manager to receive 
 *     events. This structure inherits from BtHandlerEx. The eMask field
 *     allows the application receive only the desired while "blocking" 
 *     unwanted events.
 */

typedef struct _AmpMgrHandler {
    BtHandlerEx  h;
    AmpMgrEventMask eMask;
} AmpMgrHandler;

/*-----------------------------------------------------------------------
 * BtControllerEntry structure
 *
 *     An entry in a controller list
 */  
typedef struct _BtControllerEntry {
    U8                  id;
    BtControllerType    type;
    BtAmpStatus         status;
} BtControllerEntry;

/*-----------------------------------------------------------------------
 * BtRemoteControllerList structure
 *
 *     List of remote AMP Controllers
 */  
typedef struct _BtRemoteControllerList {
    U8                numControllers;
    BtControllerEntry controllers[A2ME_MAX_REMOTE_AMP_CONTROLLERS];
} BtRemoteControllerList;

/*-----------------------------------------------------------------------
 * BtLocalControllerList structure
 *
 *     List of local AMP controllers
 */  
typedef struct _BtLocalControllerList {
    U8                numControllers;
    BtControllerEntry controllers[(NUM_AMPS > 0) ? NUM_AMPS : 1];
} BtLocalControllerList;

/*-----------------------------------------------------------------------
 * AmpMgrPlinkReg structure
 *
 *     Structure for registering with AMP Physical links
 */  
typedef struct _AmpMgrPlinkReg {
    ListEntry   node;
} AmpMgrPlinkReg;

/****************************************************************************
 *
 *  Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AMPMGR_Register()
 *
 *     Register a handler with the AMP Manager. The handler will be called back
 *     on AMP Manager events. Only events that are set in handler.eMask
 *     will be received. AmpMgrEventMask types can be ORed together. To receive
 *     all events set handler.eMask to AMPMGRE_MASK_ALL.
 *
 * Parameters:
 *     handler - Pointer to handler with fields initialized (especially eMask and
 *         h.callback). 
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - request succeeded. 
 *
 *     BT_STATUS_INVALID_PARM - h.callback in handler is 0 (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - request failed because the handler is already 
 *         registered or there are no local AMP Controllers.
 *
 */
BtStatus AMPMGR_Register(AmpMgrHandler *handler);

/*---------------------------------------------------------------------------
 * AMPMGR_Unregister()
 *
 *     Unregisters a handler from the AMP Manager. The handler will receive
 *     no further events.
 *
 * Parameters:
 *     handler - Pointer to a registered handler.
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - request succeeded. 
 *
 *     BT_STATUS_FAILED - request failed because the handler was not 
 *         registered or there are no local AMP controllers.
 *
 */
BtStatus AMPMGR_Unregister(AmpMgrHandler *handler);

/*---------------------------------------------------------------------------
 * AMPMGR_GetNumLocalAmps()
 *
 *     Return the number of local AMP Controllers that can be used (active
 *     AMP Controllers). AMPs that are in the process of being initialized or 
 *     are disabled are not included.
 *
 * Parameters:
 *     void
 *
 * Returns:      
 *     Number of local AMP Controllers 
 */
U8 AMPMGR_GetNumLocalAmps(void);

/*---------------------------------------------------------------------------
 * AMPMGR_DisableLocalController()
 *
 *     Disable the controller specified with the "active" ID. Use 
 *     AMPMGR_GetLocalAmpList to get a list of the local (active) controllers. 
 *     When a controller becomes disabled its "active" ID is no longer valid. 
 *     It will be assigned a new "disabled" ID. 
 *
 * Parameters:
 *     activeId - The active ID of the local controller. 
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - operation was successful. All the registered handlers
 *         will be called with AMPMGRE_CONTROLLER_CHANGE. This may occur
 *         before this function returns.
 *
 *     BT_STATUS_FAILED - operation failed because the controller was active.
 *         A controller is active if it has at least one phylink allocated or 
 *         is in the middle of performing an operation (e.g. the controller 
 *         has issued a command to the radio and is waiting for a response 
 *         that will result in an event). 
 *
 *     BT_STATUS_NOT_FOUND - operation failed because the activeId does
 *         not correspond to an active controller.
 */
BtStatus AMPMGR_DisableLocalController(U8 activeId);

/*---------------------------------------------------------------------------
 * AMPMGR_EnableLocalController()
 *
 *     Enable the controller specified with the "disabled" ID. Use 
 *     AMPMGR_GetDisabledAmpList to get a list of the disabled controllers. 
 *
 * Parameters:
 *     disabledId - The disabled ID of the local controller. 
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - operation was successful. All the registered handlers
 *         will be called with AMPMGRE_CONTROLLER_CHANGE. This may occur
 *         before this function returns.
 *
 *     BT_STATUS_PENDING - operation was started successfully. The result of the
 *         operation will be sent to all registered handlers via the
 *         AMPMGRE_CONTROLLER_CHANGE event. If the controller is enabled
 *         successfully it will receive a new "active" ID.
 *
 *     BT_STATUS_FAILED - operation failed possibly because the controller was
 *         unable to initialize its transport.
 *
 *     BT_STATUS_NOT_FOUND - operation failed because the disabledId does
 *         not correspond to a disabled controller.
 */
BtStatus AMPMGR_EnableLocalController(U8 disabledId);

/*---------------------------------------------------------------------------
 * AMPMGR_GetLocalAmpList()
 *
 *     Get a list of local AMP controllers. The list will be copied into the
 *     memory pointed to by list.
 *
 * Parameters:
 *     list - pointer to a BtLocalControllerList which will receive the
 *            list of controllers.
 *
 * Returns:      
 *     void
 */
void AMPMGR_GetLocalAmpList(BtLocalControllerList *list);

/*---------------------------------------------------------------------------
 * AMPMGR_GetDisabledAmpList()
 *
 *     Get a list of the disabled AMP controllers. The Controller list will be 
  *    copied into the memory pointed to by list.
 *
 * Parameters:
 *     list - pointer to a BtLocalControllerList structure which will receive 
 *         the list of controllers.
 *
 * Returns:      
 *     void
 */
void AMPMGR_GetDisabledAmpList(BtLocalControllerList *list);

/*---------------------------------------------------------------------------
 * AMPMGR_GetRemoteAmpList()
 *
 *     Get a list of remote AMP controllers from the remote device identified
 *     by remDev. The list will be copied into the memory pointed to by list
 *     if the remote AMP list is available from the local cache. If the remote
 *     AMP list must requested from the remote device BT_STATUS_PENDING will
 *     be returned and the AMPMGRE_REMOTE_AMP_LIST event will sent when 
 *     the request completes. AMPMGR_GetRemoteAmpList() must be called again 
 *     to retrieve the remote AMP list.
 *
 * Parameters:
 *     remDev - pointer to BtRemoteDevice structure identifying the remote 
 *         device from which the list of AMP controllers is desired.
 *
 *     list - pointer to a BtRemoteControllerList which will receive the
 *         list of controllers.
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - operation was successful and the remote device's
 *         AMP Controller list has been copied into the memory pointed to by 
 *         list.
 *
 *     BT_STATUS_PENDING - operation is pending. All registered handlers will
 *         be called with the AMPMGRE_REMOTE_AMP_LIST event when the
 *         operation completes.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remDev is 0 or
 *         the list is 0 or the remDev is not for a BR/EDR link.
 *
 *     BT_STATUS_FAILED - operation failed
 */
BtStatus AMPMGR_GetRemoteAmpList(BtRemoteDevice *remDev, BtRemoteControllerList *list);

/*---------------------------------------------------------------------------
 * AMPMGR_GetLocalAmpInfo()
 *
 *     Get the AMP Info for a local AMP Controller. 
 *
 * Parameters:
 *     localId - ID of the local AMP Controller 
 *
 *     ampInfo - pointer to a BtAmpInfo structure which is to receive the
 *         local AMP Info
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - operation was successful
 *
 *     BT_STATUS_NOT_FOUND - operation failed because localId is invalid.
 */
BtStatus AMPMGR_GetLocalAmpInfo(U8 localId, BtAmpInfo *ampInfo);

/*---------------------------------------------------------------------------
 * AMPMGR_GetRemoteAmpInfo()
 *
 *     Get AMP Info for a remote AMP Controller on the remote device identified
 *     by remDev. 
 *
 * Parameters:
 *     remdev - pointer to remote device structure
 *
 *     ampId - ID of the remote device's AMP Controller 
 *
 *     ampInfo - pointer to a BtAmpInfo structure which is to receive the
 *         remote AMP Info if the operation is successful.
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - operation was successful and the AMP info has
 *         been copied into the memory pointed to by ampInfo
 *
 *     BT_STATUS_PENDING - operation is pending and AMP info has not been
 *         copied. Registered handlers will be called with 
 *         AMPMGRE_GET_REMOTE_AMP_INFO event when the operation completes.
 *
 *     BT_STATUS_RESTRICTED - operation failed because remote AMP discovery
 *         has not been successfully performed (AMPMGR_GetRemoteAmpList()).
 *
 *     BT_STATUS_FAILED - operation failed because there is no remote AMP
 *         controller with the given ampId.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remDev is 0 or
 *         the ampId is invalid or the ampInfo is 0 or the remDev is not for
 *         a BR/EDR link.
 */
BtStatus AMPMGR_GetRemoteAmpInfo(BtRemoteDevice *remDev, U8 ampId, 
                                 BtRemoteAmpInfo *ampInfo);

/*---------------------------------------------------------------------------
 * AMPMGR_FindPhyLinksForRemDev()
 *
 *     Find all the AMP physical links associated with the given remote 
 *     device. Up to size number of physical link descriptors are copied 
 *     into the memory pointed to by desc if desc is not 0. The total 
 *     number of physical links associated with the given remote device is 
 *     copied into the memory pointed to by total if total is not 0. Note
 *     that this function can be used to get the total number of physical 
 *     links associated with a remote device without returning an array of 
 *     descriptors by setting size or desc to 0. It then can be called 
 *     again with an array that can hold the total size. 
 *
 * Parameters:
 *     remdev - pointer to remote device
 *
 *     size - number of items in the array pointed to by desc. If the size
 *         is 0 then no physical link descriptors are returned.
 *
 *     desc - array of BtAmpPhyLinkDesc structures to receive AMP physical 
 *         link descriptors. If the pointer is 0 no physical link descriptors
 *         are returned.
 *
 *     total - pointer to memory to receive the total number of physical
 *         links associated with the remote device. If the pointer is 0 the 
 *         total is not returned.
 *
 * Returns:      
 *     Number of AMP phylinks actually returned in desc.
 */

U16 AMPMGR_FindPhyLinksForRemDev(BtRemoteDevice *remDev, U16 size, 
                                 BtAmpPhyLinkDesc desc[], U16 *total);

/*---------------------------------------------------------------------------
 * AMPMGR_FindPhyLinks()
 *
 *     Find all the AMP physical links that exist. Up to size number of 
 *     physical link descriptors are copied into the memory pointed to by 
 *     desc if desc is not 0. The total number of physical links that exist
 *     is copied into the memory pointed to by total if total is not 0. Note 
 *     that this function can be used to get the total number of physical 
 *     links that exist without returning an array of descriptors by setting 
 *     size or desc to 0. It then can be called again with an array that can 
 *     hold the total size. 
 *
 * Parameters:
 *     size - number of items in the array pointed to by desc. If the size
 *         is 0 then no physical link descriptors are returned.
 *
 *     desc - array of BtAmpPhyLinkDesc structures to receive AMP physical 
 *         link descriptors. If the pointer is 0 no physical plink descriptors 
 *         are returned.
 *
 *     total - pointer to memory to receive the total number of physical 
 *         available. If the pointer is 0 the total is not returned.
 *
 * Returns:      
 *     Number of AMP phylinks returned in desc.
 */
U16 AMPMGR_FindPhyLinks(U16 size, BtAmpPhyLinkDesc desc[], U16 *total);

/*---------------------------------------------------------------------------
 * AMPMGR_InitPlinkReg()
 *
 *     Initialize the physical link registration structure. 
 *
 * Parameters:
 *     reg - pointer to a physical link registration structure 
 *
 * Returns:      
 *     void 
 */
void AMPMGR_InitPlinkReg(AmpMgrPlinkReg *reg);
#define AMPMGR_InitPlinkReg(reg) InitializeListEntry(&((reg)->node))

/*---------------------------------------------------------------------------
 * AMPMGR_IsPlinkRegInUse()
 *
 *     Return TRUE if the physical link registration structure is currently
 *     associated with a physical link (in use). Return FALSE if it is not 
 *     in use and available.
 *
 * Parameters:
 *     reg - pointer to a physical link registration structure 
 *
 * Returns:      
 *     void 
 */
void AMPMGR_IsPlinkRegInUse(AmpMgrPlinkReg *reg);
#define AMPMGR_IsPlinkRegInUse(reg) IsNodeConnected(&((reg)->node))

/*---------------------------------------------------------------------------
 * AMPMGR_CreateAmpPhyLink()
 *
 *     Create an AMP physical link. If the physical link already exists then
 *     the caller is registered with it. The physical link will not be
 *     disconnected under normal operation until all clients (including 
 *     logical links created using L2capAgents) have requested to be 
 *     disconnected from it. Events associated with physical links are sent 
 *     to all the handlers registered with the AMPMGR via AMPMGR_Register().
 *
 * Parameters:
 *     reg - pointer to a registration structure that associates the caller with
 *         the physical link. This structure must be initialized by calling
 *         AMPMGR_InitPlinkReg(). This structure is owned by the stack until the
 *         AMPMGR_DisconnectPhyLink is called or the physical link is
 *         disconnected (AMPMGRE_PHY_LINK_DISCONNECT is sent to all registered
 *         handlers when a physical link is disconnected). 
 *
 *     remDev - pointer to remote device
 *
 *     localAmpId - ID of the local AMP controller to be involved in the 
 *         physical link. 
 *
 *     remoteAmpId - ID of the remote AMP controller on the remDev to be
 *         involved in the physical link
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - operation was successful. The caller is registered
 *         with a physical link that is already connected or in the process
 *         of being connected.
 *
 *     BT_STATUS_PENDING - operation is pending. Registered handlers will
 *         be called with AMPMGRE_PHY_LINK_CREATE when operation completes.
 *
 *     BT_STATUS_FAILED - operation failed because there is no local and/or
 *         remote AMP controller with the given ID.
 *
 *     BT_STATUS_FAIL_SECURITY - operation failed because proper security is
 *        not in place (e.g. Secure Simple Pairing has not been performed) 
 *
 *     BT_STATUS_IN_USED - operation failed because reg is already in use or
 *        not initialized properly.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because reg is 0,  remDev is 
 *     0, localAmpId is 0 or remoteAmpId is 0 or the remDev is not for a 
 *     BR/EDR link.
 */

BtStatus AMPMGR_CreateAmpPhyLink(AmpMgrPlinkReg *reg, BtRemoteDevice *remDev,
                                 U8 localAmpId, U8 remoteAmpId);

/*---------------------------------------------------------------------------
 * AMPMGR_DisconnectAmpPhyLink()
 *
 *     Unregister the client and disconnect the AMP physical link if there 
 *     are no other clients or logical links associated with the physical link.
 *     Events associated with physical links are sent to all the handlers 
 *     registered with the AMPMGR.
 *
 * Parameters:
 *     reg- pointer to a registration structure that was used in 
 *         AMPMGR_CreateAmpPhyLink(). 
 *
 *     remDev - pointer to remote device
 *
 *     localAmpId - ID of the local AMP controller involved in the physical 
 *         link. 
 *
 *     remoteAmpId - ID of the remote AMP controller on the remote device
 *         involved in the physical link
 *
 * Returns:      
 *     BT_STATUS_PENDING - The client has been successfully removed and a 
 *         disconnect request has been initiated successfully. Registered 
 *         handlers will be called with AMPMGRE_PHY_LINK_DISCONNECT when 
 *         the operation completes. The exception is if the disconnect is 
 *         called on a physical link that has just started to come up (i.e. 
 *         the disconnect is called before the AMPMGRE_PHY_LINK_CREATE is given. 
 *         There are two possible results: handlers will be called with an 
 *         AMPMGRE_PHY_LINK_CREATE event with failed status and no
 *         AMPMGR_PHY_LINK_DISCONNECT event or handlers will be called with
 *         both an AMPMGRE_PHY_LINK_CREATE event with failed status and 
 *         an AMPMGR_PHY_LINK_DISCONNECT event (in either order). 
 *
 *     BT_STATUS_SUCCESS - The client has been successfully removed but the
 *         physical link is not being disconnected because it has other
 *         clients or logical links. 
 *
 *     BT_STATUS_NOT_FOUND - The physical link identified by remDev,
 *         localAmpId and remoteAmpId is not found. 
 *
 *     BT_STATUS_FAILED - The client has been removed but the physical link 
 *         disconnect failed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because reg is 0, 
 *         remDev is 0, localAmpId is 0, remoteAmpId is 0 or reg
 *         is not associated with the physical link or the remDev is 
 *         not for a BR/EDR link.
 */

BtStatus AMPMGR_DisconnectAmpPhyLink(AmpMgrPlinkReg *reg, BtRemoteDevice *remDev,
                                     U8 localAmpId, U8 remoteAmpId);

/*---------------------------------------------------------------------------
 * AMPMGR_ForceDisconnectAmpPhyLink()
 *
 *     Force the disconnect of the AMP physical link even if there are other 
 *     clients and logical links associated with the physical link.
 *     Events associated with physical links are sent to all the handlers 
 *     registered with the AMPMGR.
 *
 * Parameters:
 *     remDev - pointer to remote device
 *
 *     localAmpId - ID of the local AMP controller involved in the physical 
 *         link. 
 *
 *     remoteAmpId - ID of the remote AMP controller on the remote device
 *         involved in the physical link
 *
 * Returns:      
 *     BT_STATUS_PENDING - A disconnect request has been initiated successfully. 
 *         Registered  handlers will be called with AMPMGRE_PHY_LINK_DISCONNECT 
 *         when the operation completes. The exception is if the disconnect is 
 *         called on a physical link that has just started to come up (i.e. 
 *         the disconnect is called before the AMPMGRE_PHY_LINK_CREATE is given. 
 *         There are two possible results: handlers will be called with an 
 *         AMPMGRE_PHY_LINK_CREATE event with failed status and no
 *         AMPMGR_PHY_LINK_DISCONNECT event or handlers will be called with
 *         both an AMPMGRE_PHY_LINK_CREATE event with failed status and 
 *         an AMPMGR_PHY_LINK_DISCONNECT event (in either order). 
 *
 *     BT_STATUS_NOT_FOUND - The physical link identified by remDev,
 *         localAmpId and remoteAmpId is not found. 
 *
 *     BT_STATUS_FAILED - The physical link disconnect failed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because remDev is 0, 
 *         localAmpId is 0, remoteAmpId is 0 or reg is not associated with the 
 *         physical link or the remDev is not for a BR/EDR link.
 */

BtStatus AMPMGR_ForceDisconnectAmpPhyLink(BtRemoteDevice *remDev,
                                          U8 localAmpId, U8 remoteAmpId);

/*---------------------------------------------------------------------------
 * AMPMGR_SendHciCommand()
 *
 *     Sends an HCI command to the specified AMP controller.
 *
 * Parameters:
 *     cmd - Command to send
 *
 *     localAmpId - ID of the local AMP controller to receive the command.
 *
 * Returns:      
 *     BT_STATUS_PENDING - The command has been delivered to the AMP 
 *         controller and must not be modified. Completion of the command 
 *         will arrive in the AMPMGRE_HCI_COMMAND_CNF event.
 *
 *     BT_STATUS_FAILED - The command could not be sent.
 *
 *     BT_STATUS_NOT_FOUND - localAmpId does not correspond to a local 
 *         controller.
 *
 *     BT_STATUS_INVALID_PARM - A parameter was invalid (cmd or localAmpId
 *         is 0)
 */
BtStatus AMPMGR_SendHciCommand(HciCommand *cmd, U8 localAmpId);

/*---------------------------------------------------------------------------
 * AMPMGR_GetPhyHandle()
 *
 *     Retrieves the physical link handle corresponding to a local/remote
 *     AMP pair. This handle may be used when constructing HCI commands
 *     (see AMPMGR_SendCommand) or processing HCI events (see 
 *     AMPMGRE_HCI_EVENT_IND).
 *
 * Parameters:
 *     remDev - Remote device which hosts the physical link.
 *
 *     localAmpId - ID of the local AMP controller involved in the physical 
 *         link. 
 *
 *     remoteAmpId - ID of the remote AMP controller on the remote device
 *         involved in the physical link
 *
 * Returns:      
 *     Physical link handle, or 0 if the remDev or either ID are invalid or
 *     do not correspond to an open physical link.
 */
U8 AMPMGR_GetPhyHandle(BtRemoteDevice *remDev, U8 localAmpId, U8 remoteAmpId);

/****************************************************************************
 *
 * Internal functions
 *
 ****************************************************************************/

void AMPMGR_Init(void);
void AMPMGR_HandleEvents(BtEventEx *event);

#endif  /* __AMPMGR_H  */
