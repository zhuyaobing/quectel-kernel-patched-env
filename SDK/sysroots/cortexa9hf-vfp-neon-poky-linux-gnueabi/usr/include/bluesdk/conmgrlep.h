/****************************************************************************
 *
 * File:
 *     conmgrlep.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions for the LE Privacy capability
 *     of Connection Manager.
 *
 * Created:
 *     February22, 2012
 *
 * Copyright 2005-2017 OpenSynergy GmbH.
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

#ifndef __CONMGRLEP_H_
#define __CONMGRLEP_H_

#include "bttypes2.h"
#include "conmgr.h"
#include "mele.h"

/*---------------------------------------------------------------------------
 * Connection Manager LE Privacy API layer
 *
 *     The Connection Manager LE Privacy layer provides a set of functions
 *     for managing the LE Privacy feature. A GAP entity such as BSM makes
 *     calls to this API in order to enable/disable privacy and control other
 *     privacy capabilities. This API is only available if LE_PRIVACY is
 *     set to XA_ENBLED.
 *
 *     One important aspect of privacy is the use of identity addresses
 *     which is the public or static Bluetooth address of a device. When a
 *     device is private it will use either a resolvable or non-resolvable
 *     private address that will change over time. These private addresses are
 *     used during advertising, scanning and connecting. If two devices have a
 *     bond then it is important to be able to match the private address to
 *     the correct bonded device. This matching is known as resolving the
 *     private address. The processing of resolving a resolvable private
 *     address can be processor intensive so it is more efficient to pass
 *     an identity address around than a private address. 
 *
 *     Since the address of a device with privacy enabled will change 
 *     periodically a Central with privacy enabled should use the General 
 *     Connection procedure when initiating a connection. This ensures the 
 *     best chance of connecting to a peripheral using a private address. A 
 *     Peripheral with privacy enabled desiring a connection should use 
 *     undirected connectable mode when advertising because it ensures the 
 *     best of chance of connecting.
 *
 *     See the Blue SDK Connection Manager API layer for core connection
 *     creation and disconnection API.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

#if (LE_PRIVACY == XA_ENABLED) || (LL_PRIVACY == XA_ENABLED)
/*---------------------------------------------------------------------------
 * CmgrLePrivacyEvent structure
 *
 *     Used to pass LE privacy information from the CMGR layer to CMGR
 *     client applications.
 */
typedef struct _CmgrLePrivacyEvent {
    /* Inherit from BtEventEx. Note that "e.handler" contains a pointer
     * to the handler.
     */
    BtEventEx         e;

    /* Error code valid during the CMEVENTEX_LE_CHANGED_RPA_TIMEOUT, 
     * CMEVENTEX_LE_READ_LOCAL_RPA, CMEVENTEX_LE_READ_PEER_RPA, and
     * CMEVENTEX_LE_CLEAR_RESOLVE_LIST events.
     */
    BtErrorCode       errCode;

    union {
        /* Valid for event CMEVENTEX_LE_NEW_PRIVATE_ADDR */
        BtDevAddr     newPrivAddr;

        /* Valid for CMEVENTEX_LE_PRIVACY_COMPLETE */
        BOOL          isPrivate;

        /* Valid for CMEVENTEX_LE_CHANGED_RPA_TIMEOUT */
        U16           resolvableAddrTimeout;

        /* Valid for CMEVENTEX_LE_READ_LOCAL_RPA and CMEVENTEX_LE_READ_PEER_RPA */
        struct {
            BtDevAddr     identityAddr;
            BtDevAddr     rpa;
        } readRpa;
    } p;

} CmgrLePrivacyEvent;
#endif /* (LE_PRIVACY == XA_ENABLED) || (LL_PRIVACY == XA_ENABLED) */

#if LL_PRIVACY == XA_ENABLED
/*---------------------------------------------------------------------------
 * CmgrLeResolvingListInfo structure
 *
 *     Used to hold LE Resolving List information used for creating LE 
 *     connections.
 */
typedef struct _CmgrLeResolvingListInfo {
    /* Indicates if address resolution is enabled in the LE Controller */
    BOOL    enabled;

    /* Holds the total number of resolving list entries that can be stored 
     * in the LE controller.
     */
    U8      max;

    /* Holds the number of resolving list entries available (those are not yet
     * filled).
     */
    U8      avail;

} CmgrLeResolvingListInfo;
#endif /* LL_PRIVACY == XA_ENABLED */


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/
#if LE_PRIVACY == XA_ENABLED
 /*---------------------------------------------------------------------------
 * CMGR_RegisterPrivacyHandler()
 *
 *     Register a handler to receive privacy events.
 *
 * Parameters:
 *     handler - Handler structure to use.  This memory is allocated by the
 *         application. The handler's 'callback' field must be set.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Registration was successful.
 *
 *     BT_STATUS_IN_USE - Could not register Handler, because it is
 *         already in use.
 *
 *     BT_STATUS_INVALID_PARM - Callback function or handler are not valid.
 *         (XA_ERROR_CHECK only)
 */
BtStatus CMGR_RegisterPrivacyHandler(BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * CMGR_DeregisterPrivacyHandler()
 *
 *     Deregister a privacy handler.
 *
 * Parameters:
 *     Handler - handler structure to deregister. This handler must have
 *         been previously registered.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Deinitialization was successful.
 *
 *     BT_STATUS_NOT_FOUND - Could not deregister Handler, because it is
 *         not registered.
 *
 *     BT_STATUS_INVALID_PARM - Handler is not valid (XA_ERROR_CHECK only).
 */
BtStatus CMGR_DeregisterPrivacyHandler(BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * CMGR_EnablePrivacy()
 *
 *     Enable or disable local LE privacy. If the device is scanning and/or 
 *     advertising the operation(s) will be temporarily suspended, the 
 *     controller will be setup appropriately and the operation(s) resumed.
 *
 * Parameters:
 *     enable - TRUE causes privacy to be enabled while FALSE disables it.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation was successful.
 *
 *     BT_STATUS_PENDING - We are advertising and/or scanning so the controller
 *        must be updated in order to enable/disable privacy. When the operation 
 *        is complete a CMEVENTEX_LE_PRIVACY_COMPLETE event will be sent to all 
 *        registered privacy handlers.
 *
 *     BT_STATUS_BUSY - The privacy setting cannot be changed while Connection
 *         Manager is in the process of setting up advertising, setting up 
 *         scanning or starting a connection. These operations must complete before 
 *         privacy can be changed. Note that privacy can be changed while connected
 *         to a remote device, and while advertising and/or scanning.
 */
BtStatus CMGR_EnablePrivacy(BOOL enable);

/*---------------------------------------------------------------------------
 * CMGR_IsPrivacyEnabled()
 *
 *     Return TRUE if privacy is enable otherwise return FALSE.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     TRUE or FALSE
 *
 */
BOOL CMGR_IsPrivacyEnabled(void);

/*---------------------------------------------------------------------------
 * CMGR_GetCurRandomAddr()
 *
 *     Return the current random address set in the LE controller which
 *     corresponds the local device's private address. If privacy is disabled,
 *     then this address is not actually being used by the controller. 
 *
 * Parameters:
 *     devAddr - pointer to memory to receive the random address.
 *
 * Returns:
 *     Pointer to BtDevAddr or 0.
 *
 */
void CMGR_GetCurRandomAddr(BtDevAddr *devAddr);

/*---------------------------------------------------------------------------
 * CMGR_GetPrivateAddr()
 *
 *     Return the private address currently set in the Connection Manager. 
 *     This may not be the address currently being used by the controller but
 *     is the one that will be used the next time scanning, advertising, or
 *     connecting is started. If privacy is disabled then the 'type' fields 
 *     of the address will be set to BTADDR_INVALID.
 *
 *     Use CMGR_GetCurRandomAddr to get the address currently set in the LE
 *     Controller.
 *
 * Parameters:
 *     randomAddr - pointer to structure to receive the random address 
 *
 * Returns:
 *     void
 *
 */
void CMGR_GetPrivateAddr(BtDevAddr *randomAddr);

/*---------------------------------------------------------------------------
 * CMGR_ChangePrivateAddress()
 *
 *     Change the resolvable private address that is used when privacy
 *     is enabled. This function can be called at any time. If privacy is
 *     not enabled it will not take effect until privacy is enabled. If privacy
 *     is enabled it will not take effect until the next time scanning,
 *     advertising or connecting is started. In other words it will not affect
 *     a current operation.
 *
 *     This function will have no effect if Link Layer Privacy is supported,
 *     as there is no way to force the LE Controller to change is resolvable
 *     private address.
 *
 *     A CMEVENT_LE_NEW_PRIVATE_ADDR event will be sent to all registered privacy
 *     handlers when the address is programmed into the controller.
 *
 * Parameters:
 *     newAddr - pointer to memory to receive the new address. May be set to 0.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The address was changed successfully.
 *
 *     BT_STATUS_FAILED - The address was unable to be changed.  This will
 *        occur if Link Layer Privacy is supported.
 *
 *     BT_STATUS_INVALID_PARM - NewAddr is not valid (XA_ERROR_CHECK only).
 *
 */
BtStatus CMGR_ChangePrivateAddress(BtDevAddr *newAddr);

/*---------------------------------------------------------------------------
 * CMGR_SetPrivateAddrChangeTime()
 *
 *     Set the time in seconds used for changing the private address. This will
 *     either occur locally or in the LE Controller if Link Layer Privacy is 
 *     supported. In order for a device to be private it must periodically change 
 *     its private address. Connection Manager has a timer that will automatically 
 *     change the private address. This function allows the user to set the timer's 
 *     value. The minimum time allowed is 1 second but 900 seconds (15 minutes) is 
 *     the recommended value and the default. 
 *
 *     Note that this timer only runs when privacy is enabled. If the timer fires 
 *     and the device is active scanning or advertising the timer will be restarted 
 *     otherwise it will not be restarted until active scanning or advertising is 
 *     started. Also note that once the timer is started it will not be canceled 
 *     even if privacy is disabled. In that way the address will be changed for 
 *     the next time privacy is enabled.
 *
 *     A CMEVENT_LE_NEW_PRIVATE_ADDR event will be sent to all registered privacy
 *     handlers when a new address is programmed into the controller.
 *
 * Parameters:
 *     timeValue - value for resolvable private address change timer.  This
 *         values is in seconds with a valid range of 0x0001 - 0xA1B8 (11.5 hrs).
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation was started successfully.  A 
 *         CMEVENTEX_LE_CHANGED_RPA_TIMEOUT event will be sent to all 
 *         registered privacy handlers when the timeout is setup in the 
 *         controller.
 *
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_IN_PROGRESS - Another RPA timeout operation is already
 *        ongoing.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because timeValue
 *         contains an invalid value.
 *
 */
BtStatus CMGR_SetPrivateAddrChangeTime(TimeT timeValue);

/*---------------------------------------------------------------------------
 * CMGR_GetPrivateAddrChangeTime()
 *
 *     Get the time in seconds used for changing the private address. 
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     The time in seconds
 *
 */
TimeT CMGR_GetPrivateAddrChangeTime(void);

#endif /* LE_PRIVACY == XA_ENABLED */

#if LL_PRIVACY == XA_ENABLED
/*---------------------------------------------------------------------------
 * CMGR_GetLeResolvingListInfo()
 *
 *     Get the current link layer resolving list information used by the 
 *     Connection Manager for LE scanning, creating LE connections, and 
 *     advertising.
 *
 * Parameters:
 *     resolvingListInfo - Pointer to a CmgrLeResolvingListInfo structure 
 *         to receive the LE resolving list information.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation was successfully.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because resolvingListInfo
 *         is an invalid pointer.
 *
 */
BtStatus CMGR_GetLeResolvingListInfo(CmgrLeResolvingListInfo *resolvingListInfo);

/*---------------------------------------------------------------------------
 * CMGR_GetLocalResolvablePrivateAddr()
 *
 *     Returns the current local resolvable private address (RPA) being used 
 *     for the corresponding peer Identity Address.
 *
 *     A CMEVENTEX_LE_READ_LOCAL_RPA event will be sent to all registered 
 *     privacy handlers when the address is retrieved from the controller.
 *
 * Parameters:
 *     devAddr - pointer to the peer identity address.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation was started successfully.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because timeValue
 *         contains an invalid value.
 *
 */
BtStatus CMGR_GetLocalResolvablePrivateAddr(BtDevAddr *devAddr);

/*---------------------------------------------------------------------------
 * CMGR_GetPeerResolvablePrivateAddr()
 *
 *     Returns the current peer resolvable private address (RPA) being used 
 *     for the corresponding peer Identity Address.
 *
 *     A CMEVENTEX_LE_READ_PEER_RPA event will be sent to all registered 
 *     privacy handlers when the address is retrieved from the controller.
 *
 * Parameters:
 *     devAddr - pointer to the peer identity address.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation was started successfully.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because timeValue
 *         contains an invalid value.
 *
 */
BtStatus CMGR_GetPeerResolvablePrivateAddr(BtDevAddr *devAddr);

/*---------------------------------------------------------------------------
 * CMGR_ClearResolvingList()
 *
 *     Clears all IRK device pairs from the LE controller resolving list.
 *     Please use care in making this call as it impacts all devices in the 
 *     resolving list.  Additionally, since the resolving list may have 
 *     pending add/remove operations, this call should only be made after
 *     all device bonding operations are complete.
 *
 *     A CMEVENTEX_LE_CLEAR_RESOLVE_LIST event will be sent to all registered 
 *     privacy handlers when the resolving list is cleared in the controller.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation was started successfully.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because timeValue
 *         contains an invalid value.
 *
 */
BtStatus CMGR_ClearResolvingList(void);
#endif /* LL_PRIVACY == XA_ENABLED */

/*---------------------------------------------------------------------------
 * IsDevAddrPrivateResolvable()
 *
 *     Return TRUE if "devAddr" is a resolvable private address otherwise
 *     return FALSE.
 *
 * Parameters:
 *     devAddr - Pointer to BtDevAddr
 *
 * Returns:
 *     TRUE - "devAddr" is a resolvable private  address
 *
 *     FALSE - "devAddr is not a resolvable private address
 *
 */
BOOL IsDevAddrPrivateResolvable(const BtDevAddr *devAddr);

/*---------------------------------------------------------------------------
 * IsDevAddrNonPrivateResolvable()
 *
 *     Return TRUE if "devAddr is a non-resolvable private address otherwise
 *     return FALSE.
 *
 * Parameters:
 *     devAddr - Pointer to BtDevAddr
 *
 * Returns:
 *     TRUE - "devAddr" is a non-resolvable private address
 *
 *     FALSE - "devAddr is not a non-resolvable private address
 *
 */
BOOL IsDevAddrPrivateNonResolvable(const BtDevAddr *devAddr);

#endif /* __CONMGRLEP_H_ */
