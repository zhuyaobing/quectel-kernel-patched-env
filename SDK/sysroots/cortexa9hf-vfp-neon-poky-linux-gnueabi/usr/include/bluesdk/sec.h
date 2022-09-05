/****************************************************************************
 *
 * File:
 *     sec.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     security manager functionality of the Management Entity.
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

#ifndef __SEC_H
#define __SEC_H

#include "me.h"
#include "eventmgr.h"

/*---------------------------------------------------------------------------
 * Security Manager (SEC) layer
 *
 *     The Security Manager provides access to the security features
 *     offered by Bluetooth radio hardware. This includes encryption,
 *     authentication, and pairing services.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

#define BT_MAX_PIN_LEN 16

/*---------------------------------------------------------------------------
 * BtPairingType type
 *
 *     Bluetooth Pairing type. Indicates the action that should be taken during
 *     pairing. A link key is generated as a result of pairing. What happens to
 *     the link key is determined by the setting of BtPairingType.
 *
 *     NOTE: if an authorization request is already outstanding for a device,
 *     (BTEVENT_AUTHORIZATION_REQ) then the authorize setting specified here is
 *     ignored.
 */
typedef U8 BtPairingType;

#define BPT_NOT_SAVED           0x00 /* Do not save the link key */
#define BPT_SAVE_NOT_AUTHORIZED 0x01 /* Save the link key but make not authorized */
#define BPT_SAVE_AUTHORIZED     0x02 /* Save the link key and make authorized */

/* End of BtPairingType */


/*---------------------------------------------------------------------------
 * BtSecurityLevel type
 *
 * Security on incoming connections.  Only one security level should be
 * selected for an incoming and one for an outgoing connection (OR'd).  The
 * highest security selected will be used.  If authorization of that service
 * is required, the BSL_AUTHORIZATION_IN or BSL_AUTHORIZATION_OUT bit can be
 * OR'd together with the security level.
 */
typedef U16 BtSecurityLevel;

/* Security on incoming connections */

/* Requirements:  No "Man In The Middle" protection, No encryption
 * (legacy devices only), and minimal user Interaction.
 *
 * If both devices are capable of Secure Simple Pairing, this security level
 * will result in "just works" pairing (unauthenticated link key), and an
 * encrypted connection.  If one of the devices is not capable of Secure
 * Simple Pairing, this level of security will result in no link key and no
 * encryption.
 */
#define BSL_SECURITY_L1_IN      0x0001

/* Requirements: No "Man In The Middle" protection, Encryption.
 *
 * If both devices are capable of Secure Simple Pairing, this security level
 * will result in "just works" pairing (unauthenticated link key) and an
 * encrypted connection.  If one of the devices is not capable of Secure
 * Simple Pairing, this level of security will result in a pin code exchange,
 * which will create a combination link key and an encrypted connection.
 */
#define BSL_SECURITY_L2_IN      0x0002

/* Requirements: "Man In The Middle" protection, Encryption, User Interaction is
 * acceptable.
 *
 * If both devices are capable of Secure Simple Pairing, this security level
 * will result in an authenticated link key and an encrypted connection.  If
 * one of the devices is not capable of Secure Simple Pairing, this level of
 * security will result in a pin code exchange, which will create a
 * combination link key and an encrypted connection.  It is recommended that
 * a 16 digit PIN code be used.
 */
#define BSL_SECURITY_L3_IN      0x0004

/* Requirements: "Man In The Middle" protection, 128-bit strength for 
 * Encryption and Link Keys, User Interaction is acceptable.
 *
 * If both devices are capable of Secure Connections, this security level
 * will result in an authenticated link key and an 128-bit encrypted 
 * connection.  If one of the devices is not capable of Secure Connections, 
 * this level of security will result in a link disconnection with the
 *  reason code "Authentication Failure".  There is no backwards 
 * compatibility with legacy devices when in this mode. 
 */
#define BSL_SECURITY_L4_IN      0x0008

/* Authorization on incoming required. This value can be OR'd with the
 * security level to indicate that authorization on this service is
 * required.  The authorize handler will be called if it has been
 * registered.
 */
#define BSL_AUTHORIZATION_IN    0x0010

/* Security on outgoing connections */

/* Requirements:  No "Man In The Middle" protection, No encryption
 * (legacy devices only), and minimal user Interaction.
 *
 * If both devices are capable of Secure Simple Pairing, this security level
 * will result in "just works" pairing (unauthenticated link key), and an
 * encrypted connection.  If one of the devices is not capable of Secure
 * Simple Pairing, this level of security will result in no link key and no
 * encryption.
 */
#define BSL_SECURITY_L1_OUT     0x0100

/* Requirements: No "Man In The Middle" protection, Encryption.
 *
 * If both devices are capable of Secure Simple Pairing, this security level
 * will result in "just works" pairing (unauthenticated link key) and an
 * encrypted connection.  If one of the devices is not capable of Secure
 * Simple Pairing, this level of security will result in a pin code exchange,
 * which will create a combination link key and an encrypted connection.
 */
#define BSL_SECURITY_L2_OUT     0x0200

/* Requirements: "Man In The Middle" protection, Encryption, User Interaction is
 * acceptable.
 *
 * If both devices are capable of Secure Simple Pairing, this security level
 * will result in an authenticated link key and an encrypted connection.  If
 * one of the devices is not capable of Secure Simple Pairing, this level of
 * security will result in a pin code exchange, which will create a
 * combination link key and an encrypted connection.  It is recommended that
 * a 16 digit PIN code be used.
 */
#define BSL_SECURITY_L3_OUT    0x0400

/* Requirements: "Man In The Middle" protection, 128-bit strength for 
 * Encryption and Link Keys, User Interaction is acceptable.
 *
 * If both devices are capable of Secure Connections, this security level
 * will result in an authenticated link key and an 128-bit encrypted 
 * connection.  If one of the devices is not capable of Secure Connections, 
 * this level of security will result in a link disconnection with the
 *  reason code "Authentication Failure".  There is no backwards 
 * compatibility with legacy devices when in this mode. 
 */
#define BSL_SECURITY_L4_OUT    0x0800

/* Authorization on outgoing required. This value can be OR'd with the
 * security level to indicate that authorization on this service is
 * required.  The authorize handler will be called if it has been
 * registered.
 */
#define BSL_AUTHORIZATION_OUT  0x1000

/* End of BtSecurityLevel */

/*=================================
 * For Backward Compatibility only.
 * Do not use these definitions.
 */
#define BSL_NO_SECURITY          0
#define BSL_AUTHENTICATION_IN    BSL_SECURITY_L2_IN
#define BSL_AUTHENTICATION_OUT   BSL_SECURITY_L2_OUT
#define BSL_ENCRYPTION_IN        BSL_SECURITY_L3_IN
#define BSL_ENCRYPTION_OUT       BSL_SECURITY_L3_OUT
#define BSL_DEFAULT             (BSL_SECURITY_L2_IN | BSL_AUTHORIZATION_IN | \
                                 BSL_SECURITY_L2_OUT)

/*---------------------------------------------------------------------------
 * BtEncryptMode type
 */
typedef U8 BtEncryptMode;

#define BECM_ENCRYPT_DISABLE        0x00    /* Encryption is OFF */
#define BECM_ENCRYPT_ENABLE         0x01    /* Encryption is ON - E0 for BR/EDR, AES-CCM for LE */
#define BECM_ENCRYPT_SECURE_ENABLE  0x02    /* Encryption is ON - AES-CCM for BR/EDR */
#define BECM_ENCRYPT_REFRESH        0x03

/* End of BtEncryptMode */

/*---------------------------------------------------------------------------
 * BtSecurityMode type
 */
typedef U8 BtSecurityMode;

#define BSM_SEC_MODE_1       0x01
#define BSM_SEC_MODE_2       0x02
#define BSM_SEC_MODE_3       0x03
#define BSM_SEC_MODE_4       0x04

/* End of BtSecurityMode */

/*=================================
 * For Backward Compatibility only.
 * Do not use these definitions.
 */
#define BSM_SEC_DISABLED     0x00
#define BSM_SEC_LEVEL_1      0x01
#define BSM_SEC_LEVEL_2      0x02
#define BSM_SEC_LEVEL_3      0x03

/*---------------------------------------------------------------------------
 * BtSimplePairingMode type
 */
typedef U8 BtSimplePairingMode;

/* The local controller does not support Secure Simple Pairing */
#define BSP_SIMPLE_PAIRING_NOT_SET  0

/* The local controller supports Secure Simple Pairing */
#define BSP_SIMPLE_PAIRING_ENABLED  1

/* End of BtSimplePairingMode */

/*---------------------------------------------------------------------------
 * BtSecureConnectionMode type
 */
typedef U8 BtSecureConnectionMode;

/* The local controller does not support Secure Connections */
#define BSC_SECURE_CONNECTION_NOT_SET  0

/* The local controller supports Secure Connections */
#define BSC_SECURE_CONNECTION_ENABLED  1

/* End of BtSecureConnectionMode */

/*---------------------------------------------------------------------------
 * BtUserConfirm type
 */
typedef U8 BtUserConfirm;

/* The user rejects the numerical value displayed */
#define BUC_NO   0

/* The user accepts the numerical value displayed */
#define BUC_YES  1

/* End of BtUserConfirm */

/*---------------------------------------------------------------------------
 * BtSecureConnRequirements type
 *
 *     Describes whether Secure Connections Only protection is required during
 *     simple pairing. If the remote device does not support secure connections
 *     then SECURE_CONN_ONLY_REQUIRED specifies that service level connections
 *     will not be possible (to any service requiring security) until this mode 
 *     is disabled.
 */
typedef U8 BtSecureConnRequirements;

#define SECURE_CONN_ONLY_NOT_REQUIRED  0x00 /* No Secure Connections Only mode  */
#define SECURE_CONN_ONLY_REQUIRED      0x01 /* Secure Connections Only req */

/* End of BtSecureConnRequirements */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtOobData structure
 *
 *     Out of Band data used for Secure Simple Pairing.
 */
typedef struct _BtOobData
{
    /* Bluetooth address of the remote device from which Out of Band
     * data was received.
     */
    BD_ADDR bdAddr;

    /* Secure Simple Pairing Hash C received from the remote device */
    U8 hashC[16];

    /* Secure Simple Pairing Randomizer R received from the remote device */
    U8 randR[16];

    /* === Internal use only === */

    BtPairingType pairingType;

} BtOobData;

/*---------------------------------------------------------------------------
 * BtOobExtData structure
 *
 *     Out of Band extended data used for Secure Connections.
 */
typedef struct _BtOobExtData
{
    /* Bluetooth address of the remote device from which Out of Band
     * data was received.
     */
    BD_ADDR bdAddr;

    /* P-192 key information received from the remote device.  
     * Indicates hashC_192 and randR_192 will contain valid values.
     */
    BOOL rcvd_p192_key;

    /* Secure Simple Pairing Hash C derived from the P-192 key received 
     * from the remote device 
     */
    U8 hashC_192[16];

    /* Secure Simple Pairing Randomizer R derived from the P-192 key received 
     * from the remote device 
     */
    U8 randR_192[16];

    /* P-256 key information received from the remote device.  
     * Indicates hashC_256 and randR_256 will contain valid values.
     */
    BOOL rcvd_p256_key;

    /* Secure Simple Pairing Hash C derived from the P-256 key received 
     * from the remote device 
     */
    U8 hashC_256[16];

    /* Secure Simple Pairing Randomizer R derived from the P-256 key received 
     * from the remote device 
     */
    U8 randR_256[16];

    /* === Internal use only === */

    BtPairingType pairingType;

} BtOobExtData;

/*---------------------------------------------------------------------------
 * BtSecurityRecord structure
 *
 *     Security records are not stored in non-volatile memory.
 */
typedef struct _BtSecurityRecord
{
    ListEntry  node; /* Used internally by the stack */

    /* Group: The user registering a security record must fill in all fields
     * below
     */

    /* Uniquely identifies the multiplexing protocol. This protocol will
     * use SEC_AccessRequest before permitting incoming or outgoing connections
     * on any channel.
     */
    BtProtocolId    id;

    /* Specifies the channel to which these security requirements apply.
     * The meaning of the channel value is unique to the protocol.
     */
    U32_PTR         channel;

    /* Identifies the application-level Service which manages
     * this channel. Must be set to zero if not used or not known.
     */
    BtServiceId     serviceId;

    /* Security required on incoming and outgoing connections. */
    BtSecurityLevel level;

    /* PinLen specifies the size of pin required by the entity registering the
     * record. A value of 0 means the registering entity does not care while
     * a non-0 value indicates the minimum size of the pin. Keys made with
     * pin codes of shorter length are considered not strong enough. This field
     * is ignored for Secure Simple Pairing since Pins are not used and ignored
     * for BtSecurityLevel less than BSL_SECURITY_L3_IN or BSL_SECURITY_L3_OUT.
     * The maximum value for pinLen is BT_MAX_PIN_LEN.
     */
    U8              pinLen;

} BtSecurityRecord;


/*---------------------------------------------------------------------------
 * BtSecurityParms structure
 *
 *     Used to indicate security requirements independent of a security
 *     record.
 */
typedef struct _BtSecurityParms {

    /* Security on incoming connections. */
    BtSecurityLevel level;

    /* PinLen specifies the size of pin required by the entity registering the
     * record. A value of 0 means the registering entity does not care while
     * a non-0 value indicates the minimum size of the pin. Keys made with
     * pin codes of shorter length are considered not strong enough. This field
     * is ignored for Secure Simple Pairing since Pins are not used and ignored
     * for BtSecurityLevel less than BSL_SECURITY_L3_IN or BSL_SECURITY_L3_OUT.
     * The maximum value for pinLen is BT_MAX_PIN_LEN.
     */
    U8              pinLen;
} BtSecurityParms;


/*---------------------------------------------------------------------------
 * BtSecurityToken structure
 *
 *     Security tokens are used to request access by protocol
 *     layers and multiplexors.
 */
typedef struct _BtSecurityToken
{
    ListEntry  node;   /* Used internally by the stack */

    /* Group: The following parameters must be set before calling
     * SEC_AccessRequest.
     */
    BtProtocolId    id;      /* Protocol ID of caller */

    /* The multiplexing channel number. For example for L2CAP it is the PSM
     * and for RFCOMM it is the Server ID (incoming only) or a pointer to an
     * RfChannel structure (outgoing only).
     */
    U32_PTR         channel;

    /* Pointer to remote device for which the access is being requested */
    BtRemoteDevice *remDev;

    /* Incoming must be set to TRUE if access is desired for a connection
     * request from a remote (incoming). It must be set to FALSE if the
     * request is for a connection to remote device (outgoing).
     */
    BOOL            incoming;

    /* === Internal use only === */

    BtSecurityRecord   *record;
    EvmTimer            timer;
    BtSecurityLevel     level;
    BOOL                changeLinkKey;
    BtAuthRequirements  authRequirements;
    BtEventType         result;
    U8                  pinLen;
} BtSecurityToken;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * SEC_RegisterPairingHandler()
 *
 *     Register the handler that will handle all user interactions for
 *     pairing.  This handler is called when user interaction is required.
 *     When both devices support Secure Simple Pairing, this handler is called
 *     when "Man In The Middle" protection is required, and an
 *     authenticated link key will be generated.  If "just works" pairing
 *     occurs no user interaction is required.  When one of the devices does
 *     not support Secure Simple Pairing, this handler will be called
 *     whenever a PIN code exchanges is required.
 *
 *     If no handler is set (handler == 0) then the pairing
 *     procedure will fail. This function is used to set pairable
 *     mode. The old handler is returned when the function is
 *     called.
 *
 *     The pairing handler is called with the following events:
 *
 *     BTEVENT_PIN_REQ - If "errCode" is BEC_NO_ERROR, this event
 *     indicates a PIN code is needed. The handler provides a PIN
 *     code by calling SEC_SetPin. If "errCode" is NOT BEC_NO_ERROR,
 *     this event indicates a previous PIN code request has
 *     been cancelled.
 *
 *     BTEVENT_PASS_KEY_REQ - If "errCode" is BEC_NO_ERROR, this event
 *     indicates a Passkey  is needed. The handler provides a Passkey by
 *     calling SEC_SetPassKey. If "errCode" is NOT BEC_NO_ERROR,
 *     this event indicates a previous Passkey request has
 *     been cancelled.
 *
 *     BTEVENT_CONFIRM_NUMERIC_REQ - If "errCode" is BEC_NO_ERROR, this event
 *     indicates that a user confirmation is needed. The handler provides this
 *     confirmation by calling SEC_UserConfirm.  If "errCode" is NOT BEC_NO_ERROR,
 *     this event indicates a previous confirmation request has been cancelled.
 *     When calling SEC_UserConfirm the 'type' parameter should be set to the
 *     bonding agreed upon by the local and remote device. If one side does
 *     not want bonding then 'type' should be set to BPT_NOT_SAVED. The 
 *     "p.useIoReq.bondingMode" field contains the "combined" bonding mode taking 
 *     into account both the local and remote settings. The 
 *     "p.useIoReq.remBondingMode" field contains the remote device's bonding 
 *     setting. 
 *
 *     BTEVENT_DISPLAY_NUMERIC_IND - If "errCode" is BEC_NO_ERROR, this event
 *     indicates that a numeric value must be displayed.  No confirmation of
 *     this event is required by the handler.  If "errCode" is NOT BEC_NO_ERROR,
 *     this event indicates a previous request has been cancelled.
 *
 *     BTEVENT_BOND_REQ - If "errCode" is BEC_NO_ERROR, this event indicates
 *     a request to create or replace a stored link key. The handler confirms
 *     this request by calling SEC_UserConfirm. If "errCode" is NOT BEC_NO_ERROR,
 *     this event indicates a previous request has been cancelled.
 *
 *     BTEVENT_PAIRING_COMPLETE - This event indicates that pairing
 *     has completed successfully and the link is authenticated.
 *     The "errCode" field of the BtEvent indicates the status of
 *     storing the link key. If pairing fails no event will be
 *     sent (there is no event sent by the radio module indicating
 *     failure).
 *
 *     BTEVENT_CONFIRM_JW_REQ - If "errCode" is BEC_NO_ERROR, this event
 *     indicates that a user confirmation is needed. The handler provides this
 *     confirmation by calling SEC_UserConfirm.  If "errCode" is NOT BEC_NO_ERROR,
 *     this event indicates a previous confirmation request has been cancelled.
 *     When calling SEC_UserConfirm the 'type' parameter should be set to the
 *     bonding agreed upon by the local and remote device. If one side does
 *     not want bonding then 'type' should be set to BPT_NOT_SAVED. The 
 *     "p.useIoReq.bondingMode" field contains the "combined" bonding mode taking 
 *     into account both the local and remote settings. The 
 *     "p.useIoReq.remBondingMode" field contains the remote device's bonding 
 *     setting.  
 *
 * Parameters:
 *     handler - pointer to the handler for dealing with pairing.
 *         This handler can be a registered global handler but does
 *         not have to be.
 *
 * Returns:
 *     Pointer to the old handler.
 */
BtHandler *SEC_RegisterPairingHandler(BtHandler *handler);

/*---------------------------------------------------------------------------
 * SEC_RegisterAuthorizeHandler()
 *
 *     Register the handler that will handle all authorization.
 *     This handler is called when authorization is needed.
 *
 *     If no handler is set (handler == 0) then the authorization
 *     procedure will fail.
 *
 *     The authorization handler is called with the following events:
 *
 *     BTEVENT_AUTHORIZATION_REQ - If "errCode" is BEC_NO_ERROR, this event
 *     indicates a request for authorization. The link needing authorization
 *     is passed in the "remDev" field of BtEvent. During the callback, the
 *     handler can call SEC_GetSecurityRecord to obtain more information on
 *     what service or channel is being authorized. If "errCode" is NOT
 *     BEC_NO_ERROR, this event indicates a previous authorization request
 *     has been cancelled.
 *
 * Parameters:
 *     handler - pointer to the handler for dealing with
 *         authorization.  This handler can be a registered global
 *         handler or even the same handler that does pairing but it
 *         does not have to be.
 *
 * Returns:
 *     Pointer to the old handler.
 */
BtHandler *SEC_RegisterAuthorizeHandler(BtHandler *handler);

/*---------------------------------------------------------------------------
 * SEC_AccessRequest()
 *
 *     Determine if access is allowed for the device specified
 *     by rm at the protocol specified by id for the given
 *     channel. The incoming parameter indicates if the request
 *     is incoming or outgoing.
 *
 * Parameters:
 *     token - pointer to a Security token. The fields of the
 *         token must be filled in appropriately. If BT_STATUS_PENDING
 *         is returned the token cannot be modified until the protocol's
 *         callback is called.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Access is approved
 * 
 *     BT_STATUS_NO_RESOURCES - No resources are available to cancel the
 *         authentication operation.
 *
 *     BT_STATUS_PENDING - The operation has started. The results
 *         will be returned via the protocol's callback identified by
 *         id. The callback is called with "eventType"
 *         BTEVENT_ACCESS_APPROVED or BTEVENT_ACCESS_DENIED.
 *
 *     BT_STATUS_FAILED - Access is denied.
 *
 *     BT_STATUS_INVALID_PARM - Access is denied. Token is not
 *         properly initialized, token is 0 (XA_ERROR_CHECK only)
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_AccessRequest(BtSecurityToken *token);

/*---------------------------------------------------------------------------
 * SEC_CancelAccessRequest()
 *
 *     SEC_CancelAccessRequest cancels the security procedure associated with
 *     the "token" BtSecurityToken.
 *
 * Parameters:
 *     token - pointer to a Security token with active access request.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - cancel operation successful
 *
 *     BT_STATUS_FAILED - operation failed. The remote device is
 *         not in a state to cancel access request.
 */
BtStatus SEC_CancelAccessRequest(BtSecurityToken *token);

/*---------------------------------------------------------------------------
 * SEC_Register()
 *
 *     Register the security record with the security manager. Connections
 *     occurring later with the same protocol id and channel will have
 *     security parameters enforced as designated within the security record.
 *
 * Parameters:
 *     sr - A pointer to the security record.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - security record is registered.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the
 *         security record pointer is 0, or the security level is
 *         not properly formed. (error check only).
 *
 *     BT_STATUS_IN_USE - operation failed because the protocol ID and
 *         channel in security record was already registered.
 *
 *     BT_STATUS_FAILED - operation failed because record is
 *         already on the list.
 */
BtStatus SEC_Register(BtSecurityRecord *sr);

/*---------------------------------------------------------------------------
 * SEC_SetDefaultSecurityLevel()
 *
 *     Set the default security level used by SEC_AccessRequest() for
 *     services that do not have a registered security record.
 *
 * Parameters:
 *     level- the desired default level.
 *
 * Returns:
 *     The old level
 *
 */
BtSecurityLevel SEC_SetDefaultSecurityLevel(BtSecurityLevel level);

/*---------------------------------------------------------------------------
 * SEC_GetDefaultSecurityLevel()
 *
 *     Get the default security level used by SEC_AccessRequest() for
 *     services that do not have a registered security record.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     The current level
 *
 */
BtSecurityLevel SEC_GetDefaultSecurityLevel(void);

/*---------------------------------------------------------------------------
 * SEC_Unregister()
 *
 *     Unregister the security record with the security manager.
 *
 * Parameters:
 *     sr - A pointer to the security record.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - security record is unregistered.
 *
 *     BT_STATUS_FAILED - operation failed because record is not
 *         on the list.
 */
BtStatus SEC_Unregister(BtSecurityRecord *sr);

/*---------------------------------------------------------------------------
 * SEC_Authorize()
 *
 *     If "isAuthorized" is set to TRUE then temporary authorization
 *     for the remote device is granted otherwise it is denied. If
 *     the "alwaysAuthorize" parameter is set to TRUE then the device
 *     is marked as an authorized device in the device database. The
 *     authorize mark holds beyond this connection. If "alwaysAuthorize" is
 *     FALSE then the device database record is not modified.
 *     
 *     This function can be invoked whenever authorization is required, but
 *     this will usually occur when the authorization handler receives a
 *     BTEVENT_AUTHORIZATION_REQ event and "errCode" is BEC_NO_ERROR.
 *     
 * Parameters:
 *     rm - Pointer to remote device structure.
 *
 *     isAuthorized - TRUE indications authorization is granted.
 *         FALSE indicates authorization is denied.
 *
 *     alwaysAuthorize - If set to TRUE the device is marked as authorized
 *         in the device database. If set to FALSE the device database
 *         record is not modified.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation successful
 *
 *     BT_STATUS_FAILED - operation failed. The remote device is
 *         not in a state to accept authorization.
 *
 *     BT_STATUS_DEVICE_NOT_FOUND - the authorization was successful
 *         but the device data base write failed because the device was
 *         not found in the database or the write operation to the
 *         database failed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remote
 *         device pointer is 0 (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_Authorize(BtRemoteDevice *rm, BOOL isAuthorized,
                       BOOL alwaysAuthorize);
#define SEC_Authorize(remDev, isAuthorized, alwaysAuthorize) \
    (SEC_AuthorizeServiceB(remDev, isAuthorized, alwaysAuthorize, FALSE))

/*---------------------------------------------------------------------------
 * SEC_AuthorizeService()
 *
 *     SEC_AuthorizeService is similar to SEC_Authorize, but temporarily
 *     authorizes a device for a specific service or channel. If
 *     "isAuthorized" is set to TRUE then temporary authorization
 *     for a service is granted to the remote device, otherwise it
 *     is denied.  There is no "alwaysAuthorize" parameter as this authorization
 *     is always temporary for a one time only access of the service.
 *     
 *     This function can be invoked whenever authorization is required, but this
 *     will usually occur when the authorization handler receives a
 *     BTEVENT_AUTHORIZATION_REQ event and "errCode" is BEC_NO_ERROR.
 *     The event handler can access the security record registered for the
 *     service with the SEC_GetSecurityRecord API.
 *
 * Parameters:
 *     rm - Pointer to remote device structure.
 *
 *     isAuthorized - TRUE indications authorization is granted.
 *         FALSE indicates authorization is denied.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation successful
 *
 *     BT_STATUS_FAILED - operation failed. The remote device is
 *         not in a state to accept authorization.
 *
 *     BT_STATUS_DEVICE_NOT_FOUND - the authorization was successful
 *         but the device data base write failed because the device was
 *         not found in the database or the write operation to the
 *         database failed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remote
 *         device pointer is 0 (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_AuthorizeService(BtRemoteDevice *rm, BOOL isAuthorized);
#define SEC_AuthorizeService(remDev, isAuthorized) \
        (SEC_AuthorizeServiceB(remDev, isAuthorized, FALSE, TRUE))

/* Prototype for actual SEC_Authorize and SEC_AuthorizeService implementation */
BtStatus SEC_AuthorizeServiceB(BtRemoteDevice *rm, BOOL isAuthorized,
                               BOOL alwaysAuthorize, BOOL authService);

/*---------------------------------------------------------------------------
 * SEC_GetSecurityRecord()
 *
 *     SEC_GetSecurityRecord retrieves the security record associated with
 *     a BTEVENT_AUTHORIZATION_REQ event. The API must be used only during
 *     the BTEVENT_AUTHORIZATION_REQ event. An authorization handler might
 *     use this call to determine what service or channel is being authorized.
 *
 * Parameters:
 *     rm - Pointer to remote device structure.
 *
 * Returns:
 *     pointer to a BtSecurityRecord structure
 */
const BtSecurityRecord *SEC_GetSecurityRecord(BtRemoteDevice *rm);

/*---------------------------------------------------------------------------
 * SEC_RegisterService()
 *
 *     Sets security parameters for ALL connections matching the designated
 *     service ID. Any connection made with the same service ID must
 *     meet the requirements specified in the security record in addition to
 *     any connection-specific requirements from SEC_Register.
 *
 *     To unregister the record registered with this function, use
 *     SEC_Unregister.
 *
 * Parameters:
 *     record - BtSecurityRecord containing the service identifier
 *         (serviceId) and associated service requirements. The protocol id
 *         and channel are ignored. The record remains in use until
 *         SEC_Unregister() is called on the record.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The supplied security record is now registered.
 *
 *     BT_STATUS_FAILED - A record with this service ID was already
 *         registered.
 *
 *     BT_STATUS_INVALID_PARM - a parameter was invalid.
 */
BtStatus SEC_RegisterService(BtSecurityRecord *record);

/*---------------------------------------------------------------------------
 * SEC_UpdateServiceParms()
 *
 *     Updates security parameters for future connections matching a
 *     previously registered service ID.
 *
 *     Changing the security level for a service will not affect existing
 *     service connections, but will only take effect when new connections
 *     are made.
 *
 * Parameters:
 *     serviceId - The service ID to update with new security parameters.
 *         A service record must have been previously registered for this
 *         serviceId with SEC_RegisterService().
 *
 *     srcParms - The new security requirements to apply.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The security requirements were updated.
 *
 *     BT_STATUS_FAILED - No matching record with the specified serviceId was
 *         found.
 *
 *     BT_STATUS_INVALID_PARM - a parameter was invalid.
 */
BtStatus SEC_UpdateServiceParms(BtServiceId serviceId, BtSecurityParms *srcParms);

/*---------------------------------------------------------------------------
 * SEC_UserConfirm()
 *
 *     Provide a user confirmation for the pairing process. User
 *     confirmation is required when "Man In The Middle" protection is
 *     required, and may also be required when the remote device has lost
 *     its key. The IO capabilities of the local device must be
 *     IO_DISPLAY_YESNO to receive a request for user confirmation.
 *
 *     If Generation of the BTEVENT_CONFIRM_JW_REQ event is enabled then user
 *     confirmation is required for this event. Generation of the
 *     BTEVENT_CONFIRM_JW_REQ is set by calling 
 *     SEC_EnableJustWorksEventGeneration().
 *
 * Parameters:
 *
 *     remDev - Pointer to remote device structure
 *
 *     confirm - The confirmation action.
 *
 *     type - indicates how the link key should be handled. Note that if
 *         confirming MITM (BTEVENT_CONFIRM_NUMERIC_REQ event) or 
 *         "just works" (BTEVENT_CONFIRM_JW_REQ) this value should reflect
 *         the bonding settings of both the local and remote device. The
 *         bonding information is found in the 'p.userIoReq.bondingMode' of
 *         the BtEvent structure.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The User Confirmation was set. The pairing handler
 *         will be called with the BTEVENT_PAIRING_COMPLETE event only if
 *         authentication is successful and a link key is generated.
 *
 *     BT_STATUS_FAILED - operation failed because the remote
 *         device structure is not in a state where a user confirmation
 *         is needed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remote
 *         device structure is invalid or the pin is not set to 0 but
 *         the len is 0 or greater than 16.
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_UserConfirm(BtRemoteDevice *remDev, BtUserConfirm confirm,
                         BtPairingType type);

/*---------------------------------------------------------------------------
 * SEC_SetPassKey()
 *
 *     Provide a passkey for the pairing process.  A passkey is required when
 *     "Man In The Middle" protection is required.  The IO capabilities of the
 *     device must be IO_KEYBOARD_ONLY to receive a request for a passkey.
 *
 * Parameters:
 *
 *     remDev - Pointer to remote device structure
 *
 *     passKey - The Passkey.
 *
 *     len - number of bytes in the Passkey (should not exceed 6 digits).
 *
 *     type - indicates how the link key should be handled.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The Pass Key was set. The pairing handler will be called
 *         with the BTEVENT_PAIRING_COMPLETE event only if authentication is
 *         successful and a link key is generated.
 *
 *     BT_STATUS_FAILED - operation failed because the remote
 *         device structure is not in a state where a pin is needed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remote
 *         device structure is invalid or the pin is not set to 0 but
 *         the len is 0 or greater than 16.
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_SetPassKey(BtRemoteDevice *remDev, const U8 *passKey, U8 len,
                        BtPairingType type);

/*---------------------------------------------------------------------------
 * SEC_SetPin()
 *
 *     Set the pin value for the device specified by rm. This
 *     function is called as a response to the BTEVENT_PIN_REQ
 *     event when "errCode" is BEC_NO_ERROR. Setting pin to 0
 *     rejects the pairing operation. If p.pinReq.pinLen for the
 *     BTEVENT_PIN_REQ event is greater than 0 then len must be equal to or
 *     greater than p.pinReq.pinLen.
 *
 * Parameters:
 *     rm - Pointer to remote device structure
 *
 *     pin - pointer to pin. Pin set to 0 indicates that
 *         the pairing operation should be rejected.
 *
 *     len - number of bytes in the pin.
 *
 *     type - indicates how the link key should be handled.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The Pin was set. The pairing handler will be called
 *         with the BTEVENT_PAIRING_COMPLETE event only if authentication is
 *         successful and a link key is generated.
 *
 *     BT_STATUS_FAILED - operation failed because the remote
 *         device structure is not in a state where a pin is needed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remote
 *         device structure is invalid or the pin is not set to 0 but
 *         the len is 0 or greater than 16 or the BTEVENT_PIN_REQ indicated
 *         a specific pin length was required and the len is smaller
 *         that the required length.
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_SetPin(BtRemoteDevice *rm, const U8* pin, U8 len, BtPairingType type);

/*---------------------------------------------------------------------------
 * SEC_OobDataReceived()
 *
 *     Provide Out of Band data received from a remote device.  When Out of
 *     Band data has been received from the remote device, it should be provided
 *     to the stack using this function call.  During the Secure Simple
 *     Pairing process, this data will be provided to the host controller for
 *     the purpose of Secure Simple Pairing.  To clear the oobData, this
 *     function can be called again with a BD_ADDR set to all 0's.
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller.
 *
 * Parameters:
 *
 *     oobData - Pointer to a structure containing Out of Band Data received
 *         from the remote device.
 *
 *     type - indicates how the link key should be handled.
 *
 * Returns:
 *
 *     BT_STATUS_SUCCESS - Out of Band data has been registered with the stack.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remote
 *         device structure is invalid or the oobData pointer is 0.
 */
BtStatus SEC_OobDataReceived(BtOobData *oobData, BtPairingType type);

/*---------------------------------------------------------------------------
 * SEC_GetLocalOobData()
 *
 *     Retrieves Out of Band data from the local host controller.  This data
 *     should be transmitted to the remote device using an Out of Band mechanism.
 *     The data is used during Simple Pairing.
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller.
 *
 * Parameters:
 *
 *     none
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - operation is started successful. The
 *         pairing handler will be called with the BTEVENT_LOCAL_OOB_DATA
 *         event when the out of band data has been read from the
 *         host controller.
 *
 *     BT_STATUS_BUSY - operation failed because another operation is already
 *         active.  Try again in a little while.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_GetLocalOobData(void);

/*---------------------------------------------------------------------------
 * SEC_OobExtDataReceived()
 *
 *     Provide Out of Band extended data received from a remote device.  When 
 *     Out of Band data has been received from the remote device, it should 
 *     be provided to the stack using this function call.  During the Secure 
 *     Simple Pairing process, this data will be provided to the host 
 *     controller for the purpose of Secure Connections.  To clear the 
 *     oobExtData, this function can be called again with a BD_ADDR set to 
 *     all 0's.
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller.
 *     Secure Connections support in the host controller.
 *
 * Parameters:
 *
 *     oobExtData - Pointer to a structure containing Out of Band Extended 
 *         Data received from the remote device.
 *
 *     type - indicates how the link key should be handled.
 *
 * Returns:
 *
 *     BT_STATUS_SUCCESS - Out of Band extended data has been registered with 
 *         the stack.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remote
 *         device structure is invalid or the oobExtData pointer is 0.
 */
BtStatus SEC_OobExtDataReceived(BtOobExtData *oobExtData, BtPairingType type);

/*---------------------------------------------------------------------------
 * SEC_GetLocalOobExtData()
 *
 *     Retrieves Out of Band extended data from the local host controller.  
 *     This data should be transmitted to the remote device using an 
 *     Out of Band mechanism. The data is used during Secure Connections.
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller.
 *     Secure Connection support in the host controller.
 *
 * Parameters:
 *
 *     none
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - operation is started successful. The
 *         pairing handler will be called with the BTEVENT_LOCAL_OOB_EXT_DATA
 *         event when the out of band extended data has been read from the
 *         host controller.
 *
 *     BT_STATUS_BUSY - operation failed because another operation is already
 *         active. Try again in a little while.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_GetLocalOobExtData(void);

/*---------------------------------------------------------------------------
 * SEC_SetAuthRequirements
 *
 *     Set the default authentication requirements to be used during Secure
 *     Simple pairing and LE security pairing. This setting is not used
 *     to verify a service during SEC_AccessRequest() or LESEC_AccessRequest().
 *
 *     This setting is ignored if Secure Simple Pairing is not supported by 
 *     the Local Controller and LE Security is not supported by the host.
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller or LE security 
 *     support in the host.
 *
 * Parameters:
 *
 *     authReq - The new default authentication requirements.
 *
 * Returns:
 *
 *     The previous authentication requirements.
 */
BtAuthRequirements SEC_SetAuthRequirements(BtAuthRequirements authReq);

/*---------------------------------------------------------------------------
 * SEC_GetAuthRequirements
 *
 *     Get the current default authentication requirements.
 *
 * Returns:
 *
 *     The current authentication requirements.
 */
BtAuthRequirements SEC_GetAuthRequirements(void);

/*---------------------------------------------------------------------------
 * SEC_SetAuthorizeRequirements
 *
 *     Set the default authorization requirements.  If no security record
 *     exists for a particular connection, then the provided authorization
 *     requirements will be used.
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller or LE security 
 *     support in the host.
 *
 * Parameters:
 *
 *     authorizeReq - The new default authorization requirements.
 *
 * Returns:
 *
 *     The previous authorization requirements.
 */
BtAuthorizeRequirements SEC_SetAuthorizeRequirements(BtAuthorizeRequirements authorizeReq);

/*---------------------------------------------------------------------------
 * SEC_GetAuthorizeRequirements
 *
 *     Get the current default authorization requirements.
 *
 * Returns:
 *
 *     The current authentication requirements.
 */
BtAuthorizeRequirements SEC_GetAuthorizeRequirements(void);

/*---------------------------------------------------------------------------
 * SEC_SetSecureConnRequirements
 *
 *     Enable/Disable Secure Connections Only mode. If SECURE_CONN_ONLY_REQUIRED 
 *     is set then SEC_AccessRequest(), LESEC_AccessRequest() and the GATT server
 *     will require that the link by encrypted with an Authenticated Secure 
 *     Connections key in order to access a service that requires security. The
 *     exception is BR/EDR services such as SDP where security is optional and 
 *     LE services and ATT/GATT attributes that only require security mode 1 
 *     level 1.  This setting is ignored if Secure Simple Pairing, BR/EDR Secure 
 *     Connections and LE Secure Connections are not supported by the Local 
 *     Controller and/or host. If the remote device does not support Secure 
 *     Simple Pairing, BR/EDR Secure Connections or LE Secure Connections then 
 *     SECURE_CONN_ONLY_REQUIRED specifies that service level connections will not 
 *     be possible (to any service requiring security) until this requirement is 
 *     disabled.
 *
 * Requires:
 *     Secure Simple Pairing and BR/EDR Secure Connections support in the local 
 *     controller or LE Secure Connections in the host/controller.
 *
 * Parameters:
 *
 *     secureConnReq - The new default Secure Connections requirement.
 *
 * Returns:
 *
 *     The previous Secure Connections requirement.
 */
BtSecureConnRequirements SEC_SetSecureConnRequirements(BtSecureConnRequirements secureConnReq);

/*---------------------------------------------------------------------------
 * SEC_GetSecureConnRequirements
 *
 *     Get the current Secure Connections only mode setting.
 *
 * Returns:
 *
 *     The current Secure Connections requirement.
 */
BtSecureConnRequirements SEC_GetSecureConnRequirements(void);

/*---------------------------------------------------------------------------
 * SEC_AuthenticateLink()
 *
 *     Authenticate an existing link with a remote device. If
 *     a link key does not exist then pairing will be attempted.
 *     Pairing events will be sent to the Pairing handler. If
 *     a pairing handler does not exist then pairing will fail
 *     which in turn will cause authentication to fail.
 *     BTEVENT_AUTHENTICATE_CNF will be sent to the handler provided.
 *     The "errCode" field of the BtEvent specifies if the operation
 *     was successful or not.
 *
 *     When the operation completes, BTEVENT_AUTHENTICATED will
 *     be sent to all registered global handlers and all handlers bound
 *     to the remote device structure. "errCode" will indicate success.
 *     Note that this event does not occur if the remote device initiated
 *     an authentication operation because authentication is a one-way
 *     operation.
 *
 *     The link must be active (not in hold, sniff, park or in the
 *     process of being disconnected). Also it must not already
 *     be authenticated.
 *
 * Parameters:
 *     handler - handler to receive authenticate results. This can
 *         be the registered global handler or a handler bound to
 *         a remote device or 0. Failures are sent to this handler
 *         only. The handler can be 0 if desired, though no handler
 *         will receive failure notifications in this case.
 *
 *     remDev - pointer to remote device structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *
 *     BT_STATUS_FAILED - the operation failed because the link
 *         is not in a state to perform authentication or is already
 *         authenticated.
 *
 *     BT_STATUS_INVALID_PARM - the operation failed because
 *         the "remDev" is invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_AuthenticateLink(BtHandler *handler, BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * SEC_AuthenticateLinkEx()
 *
 *     Authenticate an existing link with a remote device. If
 *     a link key does not exist then pairing will be attempted and if
 *     specied, an LE link key will be generated. Generating the
 *     LE link key requires that the link be encrypted so this will be
 *     performed as well (if needed).
 *
 *     Pairing events will be sent to the Pairing handler. If a pairing 
 *     handler does not exist then pairing will fail which in turn will 
 *     cause authentication to fail. BTEVENT_AUTHENTICATE_CNF will be sent 
 *     to the handler provided. The "errCode" field of the BtEvent specifies 
 *     if the operation was successful or not.
 *
 *     If encryption is performed, the BTEVENT_ENCRYPTION_CHANGE event will
 *     be sent to all registerd global handlers and all handlers bound to the
 *     remote device structure. "errorCode" will indicate success.
 *
 *     When the operation completes, BTEVENT_AUTHENTICATED will
 *     be sent to all registered global handlers and all handlers bound
 *     to the remote device structure. "errCode" will indicate success.
 *     Note that this event does not occur if the remote device initiated
 *     an authentication operation because authentication is a one-way
 *     operation.
 *
 *     The link must be active (not in hold, sniff, park or in the
 *     process of being disconnected). Also it must not already
 *     be authenticated.
 *
 * Parameters:
 *     handler - handler to receive authenticate results. This can
 *         be the registered global handler or a handler bound to
 *         a remote device or 0. Failures are sent to this handler
 *         only. The handler can be 0 if desired, though no handler
 *         will receive failure notifications in this case.
 *
 *     remDev - pointer to remote device structure.
 *
 *     genLEKey - TRUE specifies that the stacks should enable encryption 
 *         if both the local and remote devices supports the BR/EDR security 
 *         channel and then generate the LE link key and distribute the other keys. 
 *         FALSE indicates enabling encryption and generating the LE key is not
 *         performed causing this function perform the same as SEC_AuthenticateLink().
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *
 *     BT_STATUS_FAILED - the operation failed because the link
 *         is not in a state to perform authentication or is already
 *         authenticated.
 *
 *     BT_STATUS_INVALID_PARM - the operation failed because
 *         the "remDev" is invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_RESTRICTED - An Pairing procedure on the LE transport to the
 *         same device is in progress which could result in pairing for 
 *         BR/EDR (Secure Connections on both). Wait until the LE pairing 
 *         process is complete.
 *         
 */
BtStatus SEC_AuthenticateLinkEx(BtHandler *handler, BtRemoteDevice *remDev, BOOL genLEKey);

/*---------------------------------------------------------------------------
 * SEC_AuthenticateLinkSpec()
 *
 *     Authenticate an existing link with a remote device using the specific
 *     authentication requirements parameter. If a link key does not exist 
 *     then pairing will be attempted. Pairing events will be sent to the 
 *     Pairing handler. If  a pairing handler does not exist then pairing will 
 *     fail  which in turn will cause authentication to fail.
 *     BTEVENT_AUTHENTICATE_CNF will be sent to the handler provided.
 *     The "errCode" field of the BtEvent specifies if the operation
 *     was successful or not.
 *
 *     When the operation completes, BTEVENT_AUTHENTICATED will
 *     be sent to all registered global handlers and all handlers bound
 *     to the remote device structure. "errCode" will indicate success.
 *     Note that this event does not occur if the remote device initiated
 *     an authentication operation because authentication is a one-way
 *     operation.
 *
 *     The link must be active (not in hold, sniff, park or in the
 *     process of being disconnected). 
 *
 * Parameters:
 *     handler - handler to receive authenticate results. This can
 *         be the registered global handler or a handler bound to
 *         a remote device or 0. Failures are sent to this handler
 *         only. The handler can be 0 if desired, though no handler
 *         will receive failure notifications in this case.
 *
 *     remDev - pointer to remote device structure.
 *
 *     authReq - contains the desired authentication requirements
 *         including both the desired authentication "level"
 *         (BtAuthRequirements) and bonding mode (BtBondingMode).
 *         BtBondingMode is ORed with the BtAuthRequirements.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *
 *     BT_STATUS_FAILED - the operation failed because the link
 *         is not in a state to perform authentication or is already
 *         authenticated.
 *
 *     BT_STATUS_INVALID_PARM - the operation failed because
 *         the "remDev" is invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_AuthenticateLinkSpec(BtHandler *handler, BtRemoteDevice *remDev, 
                                  BtAuthRequirements authReq);

/*---------------------------------------------------------------------------
 * SEC_SetSimplePairingDebugMode()
 *
 *     Enables or disables simple pairing debug mode. When enabled, the
 *     controller uses a fixed private/public key pair which permits
 *     over-the-air sniffers to monitor encrypted traffic.
 *
 *     In addition, this function also enables or disables the acceptance
 *     of debug keys. When debug mode is enabled, debug keys are considered
 *     acceptable regardless of MITM requirements. Debug keys are inherently
 *     weaker than ordinary keys so this function should be used with care.
 *
 * Parameters:
 *     enable - TRUE to enable debug-mode pairing. FALSE to return to
 *         generating and using ordinary security link keys.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - the operation started successfully. No event
 *         will be generated.
 *
 *     BT_STATUS_IN_PROGRESS - an attempt to set the simple pairing
 *         debug mode is already in progress.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_SetSimplePairingDebugMode(BOOL enable);

/*---------------------------------------------------------------------------
 * SEC_SetLinkEncryption()
 *
 *     Enable, disable, or refresh (v2.1 controllers only) link level
 *     encryption for the given link. If the function returns
 *     BT_STATUS_PENDING then BTEVENT_ENCRYPT_COMPLETE will be sent to the
 *     handler. The "errCode" field will indicate the result and the
 *     "p.remDev" field will indicate the remote device.
 *
 *     The operation is successful then all handlers bound to the
 *     remote device and all registered global handlers will receive
 *     BTEVENT_ENCRYPTION_CHANGE event. The "p.encrypt.remDev" field
 *     indications the link and the "p.encrypt.mode" field indicates the new
 *     encryption mode.
 *
 *     On a v2.1 controller, setting the link encryption on a link that is
 *     already encrypted will refresh the link and result in a pause and
 *     resume operation, and this function will return BT_STATUS_PENDING.
 *     When the operation is complete BTEVENT_ENCRYPT_COMPLETE will be
 *     indicated to the application.  The BTEVENT_ENCRYPTION_CHANGE event
 *     will also be indicated with the mode set to BECM_ENCRYPT_REFRESH.  On
 *     a 2.0 or previous controller, setting the link encryption on a link
 *     that is already encrypted will result in a BT_STATUS_SUCCES return
 *     code.
 *
 *     The link must be active (not in hold, sniff, park or in the
 *     process of being disconnected) and must have already been
 *     authenticated. Also it must not already be in the process of 
 *     performing an encryption operation.
 *
 * Parameters:
 *     handler - handler to receive encryption results. This can
 *         be the registered global handler or a handler bound to
 *         a remote device or 0. Failures are sent to this handler
 *         only. The handler can be 0 if desired, though no handler
 *         will receive failure notifications in this case.
 *
 *     remDev - pointer to remote device structure.
 *
 *     mode - desired encryption mode (enabled or disabled)
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         When the operation is complete the handler will be
 *         called with the BTEVENT_ENCRYPT_COMPLETE event.
 *         A BTEVENT_HCI_FATAL_ERROR event indicates a fatal radio
 *         or HCI transport error and all pending operations have failed.
 *
 *     BT_STATUS_SUCCESS - the link is already in the desired mode.
 *         No events will be generated.
 *
 *     BT_STATUS_FAILED - the operation failed because the link
 *         is not in a state to perform encryption. The link must
 *         be authenticated before encryption is allowed.
 *
 *     BT_STATUS_INVALID_PARM - the operation failed because
 *         the handler, "remDev" or mode are invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_SetLinkEncryption(BtHandler *handler, BtRemoteDevice *remDev,
                               BtEncryptMode mode);

/*---------------------------------------------------------------------------
 * SEC_EnableSecurityMode3()
 *
 *     If Secure Simple Pairing is not enabled (i.e. - the local or remote
 *     device does not support Secure Simple Pairing), then enable security
 *     mode 3. That means all connections (incoming and outgoing) must be
 *     authenticated. If any client sets security mode 3 then security mode 3
 *     is in force for all. The security Mode 2 security records are still
 *     checked and authorization may still be required. Existing connections
 *     are not affected.
 *
 *     If the function returns BT_STATUS_PENDING then the handler
 *     is called with BTEVENT_SECURITY3_COMPLETE when the operation
 *     is finished. The "errCode" field indicates the status of the
 *     operation and the "p.secMode.mode" field indicates the new
 *     security mode. Success of the operation is based on whether
 *     authentication is set or not.
 *
 *     If the operation is successful (as defined above) then all
 *     registered global handlers will receive BTEVENT_SECURITY_CHANGE
 *     event. The same fields as used above provide the results of
 *     the change.
 *
 * Parameters:
 *     handler - A pointer to a handler. It can be a registered
 *         handler, the pairing handler, or the authorization handler
 *         but doesn't have to be. Failures are sent only to this
 *         handler.
 *
 *     encryption - This parameter is ignored and only exists so the
 *         function prototype remains unchanged. The encryption
 *         feature has been removed from the specification.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation has started successfully.
 *         When the operation is complete the handler will be
 *         called with the BTEVENT_SECURITY3_COMPLETE event.
 *         A BTEVENT_HCI_FATAL_ERROR event indicates a fatal radio
 *         or HCI transport error and all pending operations have failed.
 *
 *     BT_STATUS_SUCCESS - the operation is successful. The
 *         security mode was already set to 3. No events will be
 *         generated.
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_INVALID_PARM - the operation failed because
 *         the handler is 0 (XA_ERROR_CHECK only)
 *
 *     BT_STATUS_FAILED - operations failed because change of
 *         security mode is already in progress.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_EnableSecurityMode3(BtHandler *handler, BOOL encryption);


/*---------------------------------------------------------------------------
 * SEC_DisableSecurityMode3()
 *
 *     Disable security mode 3. The Security Manager now operates
 *     in security mode 2. Security mode 2 and 3 are only used for
 *     connections to devices that do not support Secure Simple Pairing.
 *     If Secure Simple Pairing is supported, the device is always in
 *     Security Mode 4.  Disabling security mode 3 simply disables this
 *     feature for legacy devices (v2.0 and early)
 *
 *     If the function returns BT_STATUS_PENDING then the handler
 *     is called with BTEVENT_SECURITY3_COMPLETE when the operation
 *     is finished. The "errCode" field indicates the status of the
 *     operation and the "p.secMode.mode" field indicates the new
 *     security mode.  If the operation is successful then all
 *     registered global handlers will receive BTEVENT_SECURITY_CHANGE event.
 *
 * Parameters:
 *     handler - A pointer to a handler.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation has started successfully.
 *         When the operation is complete the handler will be
 *         called with the BTEVENT_SECURITY3_COMPLETE event.
 *         A BTEVENT_HCI_FATAL_ERROR event indicates a fatal radio
 *         or HCI transport error and all pending operations have failed.
 *
 *     BT_STATUS_SUCCESS - the operation is successful. The
 *         security mode was already set to 3. No events will be
 *         generated.
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_INVALID_PARM - the operation failed because
 *         the handler is 0 (XA_ERROR_CHECK only)
 *
 *     BT_STATUS_FAILED - operations failed because change of
 *         security mode is already in progress.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_DisableSecurityMode3(BtHandler *handler);

/*---------------------------------------------------------------------------
 * SEC_GetSimplePairingMode()
 *
 *    Get the Secure Simple Pairing mode.  If the local host controller is
 *    capable of Secure Simple Pairing, this value will be set to
 *    BSP_SIMPLE_PAIRING_ENABLED.
 *
 * Returns:
 *    BSP_SIMPLE_PAIRING_NOT_SET - The device does not support Secure Simple
 *        Pairing.
 *
 *    BSP_SIMPLE_PAIRING_ENABLED - The device supports Secure Simple Pairing.
 */
BtSimplePairingMode SEC_GetSimplePairingMode(void);

/*---------------------------------------------------------------------------
 * SEC_GetSecureConnectionMode()
 *
 *    Get the Secure Connection mode.  If the local host controller is
 *    capable of Secure Connections, this value will be set to
 *    BSC_SECURE_CONNECTION_ENABLED.
 *
 * Returns:
 *    BSC_SECURE_CONNECTION_NOT_SET - The device does not support Secure 
 *        Connections.
 *
 *    BSC_SECURE_CONNECTION_ENABLED - The device supports Secure Connections.
 */
BtSecureConnectionMode SEC_GetSecureConnectionMode(void);

/*---------------------------------------------------------------------------
 * SEC_SetBondingMode()
 *
 *     Enable the current bonding mode.  DEDICATED_BONDING should be used
 *     when the intent is to connect to a device with the explicit purpose of
 *     bonding, and then disconnect.  Afterward, it should be disabled.
 *     GENERAL_BONDING should be enabled if the intent is to allow bonding
 *     while connecting to a service. Bonding can be disabled by setting the
 *     mode to BONDING_NOT_ALLOWED.  If Secure Simple Pairing and LE 
 *     security are not supported, this value is ignored.
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller or LE security 
 *     support in the host.
 *
 * Parameters:
 *     mode - Should be set to the desired bonding mode.
 *
 * Returns:
 *     The previous bonding mode.
 */
BtBondingMode SEC_SetBondingMode(BtBondingMode mode);

/*---------------------------------------------------------------------------
 * SEC_GetBondingMode()
 *
 *     Get the current Bonding mode.
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     The current bonding mode.
  */
BtBondingMode SEC_GetBondingMode(void);

/*---------------------------------------------------------------------------
 * SEC_SetIoCapabilities()
 *
 *     Set the IO capabilities of this device.  The IO capabilities determine
 *     how Secure Simple Pairing and LE security is performed.  Use the IO 
 *     capabilities that best describe the capabilities of the device.  If 
 *     Secure Simple Pairing and LE Security are not supported, this value is 
 *     ignored.
 *
 * Requires:
 *     Secure Simple Pairing or LE security supported in the host controller.
 *
 * Parameters:
 *     ioCap - The IO capabilities of the device.
 *
 * Returns:
 *     The previous IO capabilities.
 */
BtIoCapabilities SEC_SetIoCapabilities(BtIoCapabilities ioCap);

/*---------------------------------------------------------------------------
 * SEC_GetIoCapabilities()
 *
 *     Get the current IO capabilities of this device.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     The current IO capabilities.
 */
BtIoCapabilities SEC_GetIoCapabilities(void);

/*---------------------------------------------------------------------------
 * SEC_GetRemoteIoCapabilities()
 *
 *     Get the current IO capabilities of the remote device.  Not valid if
 *     Secure Simple Pairing is not supported.
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller.
 *
 * Parameters:
 *     remDev - pointer to remote device structure.
 *
 * Returns:
 *     The remote IO capabilities.
 */
BtIoCapabilities SEC_GetRemoteIoCapabilities(BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * SEC_GetRemoteAuthRequirements()
 *
 *     Get the Authentication Requirements and bonding mode of the remote 
 *     device.  Not valid if Secure Simple Pairing is not supported.
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller.
 *
 * Parameters:
 *     remDev - pointer to remote device structure.
 *
 * Returns:
 *     The remote authentication requirements and bonding mode which 
 *     includes BtAuthRequirements ORed with BtBondingMode.
 */
BtAuthRequirements SEC_GetRemoteAuthRequirements(BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * SEC_EnableJustWorksEventGeneration()
 *
 *     Enable/Disable the generation of the BTEVENT_CONFIRM_JW_REQ 
 *     event. If enabled the BTEVENT_CONFIRM_JW_REQ event is sent
 *     for "just works" pairing when the stack receives the User Confirmation
 *     Request event from the controller. This is the same time that the 
 *     BTEVENT_CONFIRM_NUMERIC_REQ is sent for MITM pairing. BTEVENT_BOND_REQ 
 *     events will be generated as before but the BTEVENT_CONFIRM_JW_REQ 
 *     allows the upper layer to basically ignore BTEVENT_BOND_REQ events 
 *     since the BTEVENT_CONFIRM_JW_REQ contains all the information necessary 
 *     to make decisions concerning bonding and missing keys. The upper 
 *     layer must still confirm BTEVENT_BOND_REQ events by calling 
 *     SEC_UserConfirm(). Note that when confirming BTEVENT_BOND_REQ events
 *     it is important to respond with the correct bonding information. If the 
 *     local device wants bonding it should indicate so in the call to 
 *     SEC_UserConfirm().
 *
 *     If disabled the BTEVENT_CONFIRM_JW_REQ is not sent and the flow of 
 *     Secure Simple pairing events is unchanged.
 *
 *     This setting is not valid if Secure Simple Pairing is not supported.
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller.
 *
 * Parameters:
 *     enable - TRUE enables generation of the event while FALSE disables 
 *         generation of the event.
 *
 * Returns:
 *     The old setting.
 */
BOOL SEC_EnableJustWorksEventGeneration(BOOL enable);

/*---------------------------------------------------------------------------
 * SEC_GetJustWorksEventGeneration()
 *
 *     Get the current setting for the generation of the 
 *     BTEVENT_CONFIRM_JW_REQ event. 
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     The current setting.
 */
BOOL SEC_GetJustWorksEventGeneration(void);

/*---------------------------------------------------------------------------
 * SEC_EnableMitmWithNoIo()
 *
 *     Enable/Disable a setting in the stack that allows the local device to
 *     complete pairing/bonding with a remote device which has IO Capbilities 
 *     of NO_IO while the local security setting is for MITM. An 
 *     unauathenticated key will be generated but that is the only type of
 *     pairing possible with a NO_IO device.
 * 
 *     Note that only pairing will complete, authentication will still fail
 *     because the local security setting is for MITM.
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller.
 *
 * Parameters:
 *     enable - TRUE enables pairing with NO_IO device while FALSE disables 
 *         pairing with NO_IO device and MITM set.
 *
 * Returns:
 *     The old setting.
 */
BOOL SEC_EnableMitmWithNoIo(BOOL enable);

/*---------------------------------------------------------------------------
 * SEC_GetMitmWithNoIo()
 *
 *     Get the current value for the NO_IO with MITM setting. 
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     The current setting.
 */
BOOL SEC_GetMitmWithNoIo(void);

/*---------------------------------------------------------------------------
 * SEC_IsAuthenticationActive()
 *
 *     Return TRUE if the "remDev" is currently involved in an authentication
 *     process otherwise return FALSE. 
 *
 * Parameters:
 *     remDev - pointer to remote device structure.
 *
 * Returns:
 *     TRUE or FALSE.
 */
BOOL SEC_IsAuthenticationActive(BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * SEC_WriteAuthPayloadTimeout()
 *
 *     Issues the HCI command to write the authenticated payload timeout
 *     for either the BR/EDR or LE connection that is active.  This is used
 *     in the LL Ping behavior.
 *
 * Parameters:
 *     remDev - pointer to remote device structure.
 *
 *     timeout - maximum amount of time between packets authenticated by
 *         a valid MIC. Each value is an increment of 10 ms.  Valid values 
 *         are 0x0001 - 0xFFFF.
 *
 * Returns:
 *     TRUE or FALSE.
 */
BtStatus SEC_WriteAuthPayloadTimeout(BtRemoteDevice *remDev, U16 timeout);

/*---------------------------------------------------------------------------
 * SEC_ReadAuthPayloadTimeout()
 *
 *     Issues the HCI command to read the authenticated payload timeout
 *     for either the BR/EDR or LE connection that is active. This is used
 *     in the LL Ping behavior.
 *
 * Parameters:
 *     remDev - pointer to remote device structure.
 *
 * Returns:
 *     TRUE or FALSE.
 */
BtStatus SEC_ReadAuthPayloadTimeout(BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * SEC_DeleteDeviceRecord()
 *
 *     Delete a record with the given "bdAddr" from the device
 *     database, together with all its associated AMP records.
 *
 * Parameters:
 *     bdAddr - pointer to 48-bit address of device record to delete.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation is successful.
 *
 *     BT_STATUS_FAILED - operation failed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because bdAddr
 *         is invalid (XA_ERROR_CHECK only)
 */
BtStatus SEC_DeleteDeviceRecord(const BD_ADDR *bdAddr);

/*---------------------------------------------------------------------------
 * SEC_DeleteDeviceRecordEx()
 *
 *     Delete the record with the given devAddr from the device database.
 *     If the type is BR/EDR then also delete all its associated AMP records.
 *     Devices are typically stored in the device database using their
 *     identity (public/static) address. For a BR/EDR device the identity 
 *     address is the public and is remdev->devAddr. For an LE device the 
 *     best address to use is (MeLeRemoteDevice*)remdev->identityAddr.
 *
 * Parameters:
 *     devAddr - pointer to BtDevAddr of device record to delete.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation is successful.
 *
 *     BT_STATUS_FAILED - operation failed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because bdAddr
 *         is invalid (XA_ERROR_CHECK only)
 */
BtStatus SEC_DeleteDeviceRecordEx(const BtDevAddr *devAddr);

/*---------------------------------------------------------------------------
 * SEC_AddDeviceRecord()
 *
 *     Add a record to the device database. A pointer to a record is passed
 *     to the function. The contents of the record are copied into the
 *     device database. If a record with the same BD_ADDR already exists then
 *     the existing record is changed. This function can be used to preload
 *     the device database. Only head records (Controller type of BR/EDR) can
 *     be added using this function.
 *
 *     If the link key or link key type of an existing record is changed then
 *     all the associated AMP records will be deleted since their dedicated
 *     AMP keys are now invalid. Also the gAmpLK of the existing record must
 *     be set to "unusable" so it can be regenerated. This function assumes
 *     that the caller has set gAmpLK unusable in the record passed in.
 *
 * Parameters:
 *     record - pointer to record which is to be copied into the
 *     database.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - record written successfully.
 *
 *     BT_STATUS_FAILED - record was not written.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because record
 *         is invalid (XA_ERROR_CHECK only)
 *
 *     BT_STATUS_INVALID_TYPE - operation failed because the record is not
 *         a head record (crlType == BCT_BR_EDR) (XA_ERROR_CHECK only)
 */
BtStatus SEC_AddDeviceRecord(BtDeviceRecord *record);

/*---------------------------------------------------------------------------
 * SEC_FindDeviceRecord()
 *
 *     Find the device record with the given bdAddr. The record
 *     is copied to the location pointed to by the record parameter.
 *
 * Parameters:
 *     bdAddr - pointer to 48-bit address of device record to find.
 *
 *     record - pointer to location to receive record.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation is successful.
 *
 *     BT_STATUS_FAILED - operation failed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because bdAddr
 *         or record is invalid (XA_ERROR_CHECK only)
 */
BtStatus SEC_FindDeviceRecord(const BD_ADDR *bdAddr, BtDeviceRecord *record);

/*---------------------------------------------------------------------------
 * SEC_FindDeviceRecordEx()
 *
 *     Find the device record with the given devAddr. The record
 *     is copied to the location pointed to by the record parameter.
 *
 * Parameters:
 *     devAddr - pointer to BtDevAddr structure of device record to find. It
 *         recommended that the identity (public/static) address be used for 
 *         the search. For a BR/EDR device the identity address is the public 
 *         and is remdev->devAddr. For an LE device the best address to use is 
 *         (MeLeRemoteDevice*)remdev->identityAddr.
 *
 *     record - pointer to location to receive record.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation is successful.
 *
 *     BT_STATUS_FAILED - operation failed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because devAddr
 *         or record is invalid (XA_ERROR_CHECK only)
 */
BtStatus SEC_FindDeviceRecordEx(const BtDevAddr *devAddr, BtDeviceRecord *record);

/*---------------------------------------------------------------------------
 * SEC_EnumDeviceRecords()
 *
 *     Enumerate the device database. The first record is at index 0. The
 *     intent of this function is to enumerate all records in the device
 *     database in a loop. If records are added or deleted in between calls
 *     to this function then all records may not be returned or duplicate
 *     records could be returned. Note that LE records and AMP records along
 *     with BR/EDR records will be returned.
 *
 * Parameters:
 *     i - index of desired record. The first record is at index 0.
 *
 *     record - pointer to location to receive record.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation is successful.
 *         Record is returned
 *
 *     BT_STATUS_FAILED - operation failed. No record is
 *         returned. This occurs if the index is greater than
 *         the number of records or an error occurred in reading
 *         the database.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because record
 *         is invalid (XA_ERROR_CHECK only).
 */
BtStatus SEC_EnumDeviceRecords(I16 i, BtDeviceRecord *record);

/*---------------------------------------------------------------------------
 * SEC_FindDualDeviceRecord()
 *
 *     Find the dual mode device record for the given devAddr. If the devAddr
 *     is for a BR/EDR transport the record for the LE transport on the same 
 *     device is returned. If the devAddr is for an LE transport the record 
 *     for the BR/EDR transport on the same device is returned. The record is 
 *     copied to the location pointed to by the record parameter.
 *
 * Parameters:
 *     devAddr - pointer to BtDevAddr structure of the dual transport of 
 *         device record to find. It recommended that the identity 
 *         (public/static) address be used for the search. For a 
 *         BR/EDR device the identity address is the public and is 
 *         remdev->devAddr. For an LE device the best address to use is 
 *         (MeLeRemoteDevice*)remdev->identityAddr.
 *
 *     record - pointer to location to receive record.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation is successful.
 *
 *     BT_STATUS_FAILED - operation failed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because devAddr
 *         or record is invalid (XA_ERROR_CHECK only)
 */
BtStatus SEC_FindDualDeviceRecord(const BtDevAddr *devAddr, BtDeviceRecord *record);

#endif /* __SEC_H */


