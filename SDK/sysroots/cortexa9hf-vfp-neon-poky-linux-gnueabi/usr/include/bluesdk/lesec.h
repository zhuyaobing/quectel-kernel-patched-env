/****************************************************************************
 *
 * File:
 *     lesec.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     LE security manager functionality of the Management Entity.
 *
 * Copyright 2014-2017 OpenSynergy GmbH.
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

#ifndef __LESEC_H
#define __LESEC_H

#include "mele.h"
#include "l2cap.h"
#include "eventmgr.h"

/*---------------------------------------------------------------------------
 * LE Security Manager (LESEC) layer
 *
 *     The LE Security Manager provides access to the security features
 *     offered by Bluetooth LE radio hardware. This includes encryption,
 *     authentication, and pairing services.
 *
 *     Note that LE security shares some functions with BR/EDR security. The
 *     BR/EDR security functions SEC_SetIoCapabilities(), SEC_SetAuthRequirements(),
 *     SEC_SetBondingMode(), SEC_SetAuthorizeRequirements() are device settings 
 *     that apply to both LE and BR/EDR security.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

#define LE_MAX_ENCRYPT_KEY_LEN 16

/*---------------------------------------------------------------------------
 * LeProtocolId type
 *
 *     The protocol ID is a pointer to the protocol's security callback
 *     function. The user of the protocol ID should not use the protocol ID
 *     as a callback function since in the future it may be something else.
 *     The callback is a unique way to identify a protocol layer.
 */
typedef BtCallbackEx LeProtocolId;

/* End of LeProtocolId */

/*---------------------------------------------------------------------------
 * LeSecurityResult type
 *
 *     The LE Security result identifies whether the existing security
 *     is sufficient for a particular LE service.  If not, this result will 
 *     describe what is insufficient.
 */
typedef U16 LeSecurityResult;

#define LESEC_RESULT_OK                 0     /* Security is good enough, nothing needed */
#define LESEC_RESULT_OK_NO_SECURITY     1     /* Security is good enough, since none is required */
#define LESEC_INSUFF_AUTHENTICATION     2     /* Authentication has not been performed */
#define LESEC_INSUFF_AUTHORIZATION      3     /* Authorization has not been performed */
#define LESEC_INSUFF_ENCRYPT_KEY_SIZE   4     /* Encryption key strength is insufficient */
#define LESEC_INSUFF_ENCRYPTION         5     /* Encryption is not enabled */

/* LE secure Connections authentication is required but the remote does not support it */
#define LESEC_INSUFF_AUTHENTICATION_SC  6    

/* End of LeSecurityResult */


/*---------------------------------------------------------------------------
 * LeSecEventType type
 *
 *     The LE Security event types are passed to the registered pairing
 *     handler. A pairing handler is registered by calling 
 *     LESEC_RegisterPairingHandler(). The event types are defined below. 
 *     They are offsets from BTE_TYPE_BASE_LESEC defined in bttypes2.h
 */
typedef BtEventTypeEx LeSecEventType;

/* An LE security procedure is being requested by the remote device or an
 * encryption operation (LE authentication) failed because the remote
 * device is missing its link key and security may need to be retried. 
 * Approval of the security request is required.  LESEC_UserConfirmSecurity() 
 * must be called by the application to approve or deny the request or the link
 * must be disconnected. The "p.keyReplace" field is set to TRUE in the situation
 * where the remote key is missing. The "e.status" indicates whether this is a 
 * first security attempt (success) or a retry (failed). Note that this event
 * will be sent in the missing key case even if the LE security layer did not 
 * initiate the encryption (e.g. CMGR initiates encryption as part of an LE
 * link connect). The LeSecEvent fields "remDev", "p.keyReplace, "errCode", 
 * and "e.status" are valid.
 */

#define LESECE_SECURITY_REQUEST              (BTE_TYPE_BASE_LESEC + 0)

/* Indicates that a security operation has completed.  The LeSecEvent 
 * field "remDev", "p.reason", "errCode" and "e.status" are valid.
 */
#define LESECE_SECURITY_COMPLETE             (BTE_TYPE_BASE_LESEC + 1)

/* The LE Security pairing confirm and short-term key generation algorithms
 * require a passkey value to be provided.  LESEC_SetPasskey() must be
 * called to provide this value and ensure that the security procedure
 * can continue processing. LeSecEvent fields "remDev" and "e.status" 
 * are valid. "e.status" indicates the success or failure of the request.
 */
#define LESECE_PASSKEY_REQ                   (BTE_TYPE_BASE_LESEC + 2)

/* The LE Security pairing confirm and short-term key generation algorithms
 * created a 6-digit random passkey value to be displayed by the upper
 * layer so that the remote device can enter it. The application should
 * display the numeric value provided and clear the display once
 * authentication has completed.   
 *
 * For LE Secure Connections the Numeric Comparison algorithm will
 * generate a 6-digit confirm value when MITM has been negotiated. This
 * value is to be displayed by both devices and checked by the user. The
 * user must indicate if the displayed values match.
 *
 * LeSecEvent fields "remDev", "e.status", and "p.pkDisp are valid.
 * "remdev" points to the remote device, "e.status" indicates the success  
 * or failure of the request and "p.pkDisp.requireConfirm" specifies if user 
 * confirmation is required or not. If "p.pkDisp.requiredConfirm" is set to TRUE 
 * then user confirmation is required and LESEC_UserConfirm() must be called 
 * for pairing to continue. If "p.pkDisp.requireConfirm" is FALSE then no user
 * confirmation is needed. The numeric display should be cleared when 
 * user confirmation is provided or pairing completes. 
 */
#define LESECE_PASSKEY_DISPLAY               (BTE_TYPE_BASE_LESEC + 3)

/* The LE connection had an encrypted operation complete in response to
 * a call to LESEC_InitiateSecurity(), the GATT client or server initiating
 *  encryption to authenticate the link or due to an operation started by the
 * remote device. "e.status" indicates the success or failure of the
 * encryption operation.
 *
 * Note that this event signifies that the link has been authenticated. If
 * a pairing process was not underway then a stored LTK was used. If 
 * LESEC_InitiateSecurity() or LESEC_FixSecurityError() was not called then
 * this will be the only LESEC event indicating authentication is complete.
 */
#define LESECE_ENCRYPTED                     (BTE_TYPE_BASE_LESEC + 4)

/* The LE connection transitioned from encrypted to the not encrypted
 * state as a result of an encryption change event. LeSecEvent fields 
 * "remDev" and "e.status" are valid. "e.status" always indicates the 
 * success in this case. 
 * 
 */
#define LESECE_NOT_ENCRYPTED                 (BTE_TYPE_BASE_LESEC + 5)

/* The LE Secure Connections Get OOB data operation is complete. LeSecEvent 
* fields "remDev", "smp.oob", and "e.status" are valid. "e.status"
 * indicates if the operation was successful or not. If successful then
 * "smp.oob" contains pointers to the generated OOB data in Little 
 * Endian format. 
 */
#define LESECE_LOCAL_OOB_DATA                (BTE_TYPE_BASE_LESEC + 6)

/* If "e.status" is BT_STATUS_SUCCESS then LE Security authorization is 
 * being requested otherwise an outstanding LE authorization request is 
 * being cancelled. In the case of an authorization request, 
 * LESEC_Authorize() can be called during the context of this event or 
 * after the callback returns. 
 *
 * An authorization request is generated either by the GATT server when a 
 * remote device attempts to access an attribute that requires authorization 
 * or by the LE L2CAP Connection Oriented Channel layer when a remote device 
 * attempts to connect to a PSM that requires authorization. If 
 * LESEC_Authorize() is not called during the context of the callback then 
 * the GATT server or LE L2CAP will wait until it is called before proceeding 
 * with the operation. For a GATT operation this will stop the remote GATT client 
 * from sending other commands. For an L2CAP Connect procedure this will block 
 * the connection from completing. If the event was sent for a GATT "write with 
 * no response" or a "signed write" command and LESEC_Authorize() is not called 
 * during the context of the callback, then the GATT server will assume the "write 
 * with no response" or signed write command is not authorized and will ignore it. 
 * LeSecEvent fields "e.status, "errCode", and "remDev" are valid.
 */
#define LESECE_AUTHORIZATION_REQ             (BTE_TYPE_BASE_LESEC + 7)

/* An LE LTK was generated during a BR/EDR pairing process. Possibly other
 * keys were distributed as well. "remDev" points to the MeLeRemoteDevice structure 
 * representing the remote device. It may be 0 if there is no LE connection to the
 * remote device. "p.crossKey.devAddr" is the device address used to store pairing 
 * information in the device database if bonding occurred. "p.crossKey.bonded" is
 * TRUE if bonding occurred otherwise it is false.
 */
#define LESECE_CROSS_TRANS_KEY               (BTE_TYPE_BASE_LESEC + 8) 

/* Group: The following are sent only to protocol ID callback functions
 * passed in a an LeSecurityToken structure to LESEC_AccessRequest().     
 */

/* Indicates access request is successful. */
#define LESECE_ACCESS_APPROVED               (BTE_TYPE_BASE_LESEC + 20)

/* Indicates access request failed. */
#define LESECE_ACCESS_DENIED                 (BTE_TYPE_BASE_LESEC + 21)

/* Request user confirmation for "Just Works" pairing. The application
 * must call LESEC_UserConfirm() to confirm or reject this request. 
 *
 * LeSecEvent fields "remDev" and "e.status" are valid. "remdev" points 
 * to the remote device and "e.status" indicates the success or failure 
 * of the request. 
 *
 * This event is only sent if LESEC_EnableJustWorksEventGeneration() has
 * been called to enable generation of the event.
 */
#define LESECE_CONFIRM_JW_REQ                (BTE_TYPE_BASE_LESEC + 22)

/* End of LeSecEventType */

/*---------------------------------------------------------------------------
 * LeSecSecurityLevel type
 *
 *     Security Level to be applied to the LE device.  These values are
 *     based on the GAP specification and the defined LE Security Mode
 *     1 and Levels (1,2, 3 and 4).
 */
typedef U8 LeSecSecurityLevel;

/* Security Mode 1, Level 1 - No security */
#define MLSL_NO_SECURITY         0x00

/* Security Mode 1, Level 2 - Unauthenticated Link Key and Encryption */
#define MLSL_UNAUTH_KEY_ENCRYPT  0x01

/* Security Mode 1, Level 3 - Authenticated Link Key and Encryption */
#define MLSL_AUTH_KEY_ENCRYPT    0x02

/* Security Mode 1, Level 4 - Authenticated LE Secure Connections pairing 
 * with encryption.
 */
#define MLSL_SC_AUTH_KEY_ENCRYPT 0x03

/* Security level is invalid */
#define MLSL_INVALID_SECURITY    0xFF

/* End of LeSecSecurityLevel */

/*---------------------------------------------------------------------------
 * LeSmpPairingFailureReason type
 *
 *     Describes the reason codes for a pairing failure.
 */
typedef U8 LeSmpPairingFailureReason;

#define SMP_PFRC_NO_ERROR                   0x00
#define SMP_PFRC_PASSKEY_ENTRY_FAILURE      0x01
#define SMP_PFRC_OOB_UNAVAILABLE            0x02
#define SMP_PFRC_AUTH_REQ_FAILURE           0x03
#define SMP_PFRC_CONFIRM_VALUE_FAILURE      0x04
#define SMP_PFRC_PAIRING_NOT_SUPPORTED      0x05
#define SMP_PFRC_ENCRYPTION_KEY_SIZE        0x06
#define SMP_PFRC_COMMAND_NOT_SUPPORTED      0x07
#define SMP_PFRC_UNSPECIFIED_REASON         0x08
#define SMP_PFRC_REPEATED_ATTEMPTS          0x09
#define SMP_PFRC_INVALID_PARAMETERS         0x0A
#define SMP_PFRC_DHKEY_CHECK_FAILED         0x0B
#define SMP_PFRC_NUM_COMPARE_FAILED         0x0C
#define SMP_PFRC_BREDR_PAIRING_PROGRESS     0x0D
#define SMP_PFRC_CROSSTX_KEY_NOT_ALLOWED    0x0E

/* End of LeSmpPairingFailureReason */

/*---------------------------------------------------------------------------
 * LeSmpKeyDistributionFormat type
 *
 *     Describes the key(s) the initiator shall distribute during the
 *     Transport Specific Key Distribution phase.
 */
typedef U8 LeSmpKeyDistributionFormat;

/* Device shall distribute LTK using Encryption Information Command followed by
 * EDIV and Rand using the Master Identification command.
 */
#define SMP_DISTRIBUTE_LTK            0x01

/* Device shall distribute IRK using Identity Information Command followed by
 * its public device or static random address using the Identity Address
 * Information command.
 */
#define SMP_DISTRIBUTE_IRK            0x02

/* Device shall distribute CSRK using Signing Information Command */
#define SMP_DISTRIBUTE_CSRK           0x04

/* Derive the BR/EDR link the link key from the LE LTK. */
#define SMP_DISTRIBUTE_LINK_KEY       0x08

/* Bitmask of used key distribution flags when legacy SM is used */
#define SMP_DISTRIBUTE_BITMASK_LEGACY 0x07

/* Bitmask of used key distribution flags when Secure Connections is used */
#define SMP_DISTRIBUTE_BITMASK_SC     0x0F

/* End of LeSmpKeyDistributionFormat */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * LeSecLegOobData structure
 *
 *     Out of Band data used for LE Legacy Pairing.
 */
typedef struct _LeSecLegOobData
{
    /* Identity address of the remote device from which Out of Band
     * data was received.
     */
    BtDevAddr devAddr;

    /* Number of bytes valid in tk */
    U8   len;

    /* TK value is a 128-bit random number */
    U8   tk[16];

} LeSecLegOobData;

/*---------------------------------------------------------------------------
 * LeSecScOobData structure
 *
 *     Out of Band data used for LE Secure Connections Pairing.
 */
typedef struct _LeSecScOobData
{
    /* Identity address of the remote device from which Out of Band
     * data was received.
     */
    BtDevAddr devAddr;

    /* LE Secure Connections Hash C received from the remote device */
    U8 hashC[16];

    /* LE Secure Connections Randomizer R received from the remote device */
    U8 randR[16];

} LeSecScOobData;

#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
/*---------------------------------------------------------------------------
 * LeSecurityRecord structure
 *
 *     Security records are not stored in non-volatile memory.
 */
typedef struct _LeSecurityRecord
{
    ListEntry               node; /* Used internally by the stack */

    /* Group: The user registering a security record must fill in all fields
     * below
     */

    /* Uniquely identifies the multiplexing protocol. This protocol will
     * use LESEC_AccessRequest before permitting incoming or outgoing connections
     * on any channel.
     */
    LeProtocolId            id;

    /* Specifies the channel to which these security requirements apply.
     * The meaning of the channel value is unique to the protocol.
     */
    U32                     channel;

    /* Security required on incoming connections */
    LeSecSecurityLevel       inLevel;

    /* Security required on outgoing connections */
    LeSecSecurityLevel       outLevel;

    /* Authorization required on incoming connections */
    BtAuthorizeRequirements  inAuthorize;

    /* Authorization required on outgoing connections */
    BtAuthorizeRequirements  outAuthorize;

    /* keyLen specifies the length of the link key required by the entity 
     * registering the record. A value of 0 means the registering entity 
     * does not care while a non-0 value indicates the minimum size of the key. 
     * Keys that are a shorter length are considered not strong enough. 
     * The maximum value for keyLen is LE_MAX_ENCRYPT_KEY_LEN.
     */
    U8                      keyLen;

} LeSecurityRecord;

/*---------------------------------------------------------------------------
 * LeSecurityToken structure
 *
 *     Security tokens are used to request access by protocol
 *     layers and multiplexors.
 */
typedef struct _LeSecurityToken
{
    ListEntry           node;   /* Used internally by the stack */

    /* Group: The following parameters must be set before calling
     * LESEC_AccessRequest.
     */

    /* The multiplexing channel */
    U32                 channel;

    /* Pointer to remote device for which the access is being requested */
    BtRemoteDevice     *remDev;

    /* Function pointer for the protocol access request callback */
    LeProtocolId        protocolId;

    /* TRUE if for incoming connection and FALSE for outgoing connection*/
    BOOL                incoming;

    /* === Internal use only === */
    U32_PTR             l2capChannel;
    EvmTimer            timer;

} LeSecurityToken;

#endif /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */

/*---------------------------------------------------------------------------
 * LeSecEvent structure
 *
 *     Provides information to the LE Security user about an LeSec callback event.
 */
typedef struct _LeSecEvent {

    /* Inherit from BtEventEx */
    BtEventEx       e;

    /* Error code associated with event. */
    BtErrorCode     errCode;

    /* Remote Device associated with event. This could be a BR/EDR remote device
     * for LESECE_AUTHORIZATION_REQ event.
     */
    BtRemoteDevice *remDev;

    union {
        /* Valid for LESECE_PASSKEY_DISPLAY event. */
        struct {
            /* Holds the 6-digit randomly generated numeric passkey */ 
            U32    passkey;    

            /* TRUE indicates that user confirmation is required and 
             * LESEC_UserConfirm() must be called. FALSE indicates no 
             * user confirmation is required. */
            BOOL   requireConfirm;
        } pkDisp;

        /* Valid for LESECE_EVENT_LOCAL_OOB_DATA */
        struct {
            /* 128-bit Random number. Is useful for both Legacy and LE Secure Connection
             * OOB data.
             */
            U8        *randR;

            /* 128-bit hashC value used for LE Secure Connections OOB data */
            U8        *hashC;
        } oob;

        /* Valid for LESECE_SECURITY_REQUEST event. If true, indicates a security 
         * request to replace a local link key otherwise it indicates a security 
         * request for creating a new local link key.
         */
        BOOL            keyReplace;

        /* Valid for LESEC_SECURITY_COMPLETE event */
        /* Valid if "e.status" is unsuccessful */
        LeSmpPairingFailureReason   reason;

        /* Valid for LESECE_CROSS_TRANS_KEY */
        struct {
            BOOL           bonded;
            BtDevAddr      devAddr;
        } crossKey;
    } p;

} LeSecEvent;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * LESEC_RegisterPairingHandler()
 *
 *     Register the handler that will handle all user interactions for
 *     pairing.  This handler is called when user interaction is required and
 *     to report results of LE security. It is passed a pointer to an
 *     LeSecEvent structure.
 *
 *     If no handler is set (handler == 0) then the pairing
 *     procedure will fail. This function is used to set pairable
 *     mode. The old handler is returned when the function is
 *     called.
 *
 *     The pairing handler is called with the following events: 
 *
 *     LESECE_SECURITY_REQUEST - An LE security procedure is being requested 
 *     by the remote device or an encyrption operation (LE authentication) 
 *     failed because the remote device is missing its link key and security 
 *     may need to be retried. Approval of the security request is required.
 *
 *     LESECE_SECURITY_COMPLETE - This event indicates that pairing
 *     has completed.  
 *
 *     LESECE_PASSKEY_REQ - The LE Security pairing confirm and short-term 
 *     key generation algorithms require a passkey value to be provided.  
 *     LESEC_SetPasskey() must be called to provide this value and ensure that 
 *     the security procedure can continue processing. 
 *
 *     LESECE_PASSKEY_DISPLAY -  The LE Security pairing confirm, short-term 
 *     key generation algorithms or LE Secure connections Numeric Comparison
 *     algorithm created a 6-digit random passkey value to be displayed by the 
 *     upper layer. The application should display the numeric value provided.  
 *     If "p.pkDisp.requiredConfirm" is set to TRUE then user confirmaton is 
 *     required and LESEC_UserConfirm() must be called for pairing to continue.   
 *
 *     LESECE_ENCRYPTED - The LE connection had an encrypted operation complete 
 *     in response to a call to LESEC_InitiateSecurity or due an operation started 
 *     by the remote device. 
 *
 *     LESECE_NOT_ENCRYPTED - The LE connection transitioned from encrypted to the 
 *     not encrypted state as a result of an encryption change event. 
 * 
 *     LESECE_CROSS_TRANS_KEY - An LE LTK was generated during a BR/EDR pairing 
 *     process. Possibly other keys were distributed as well. 
 *
 * Parameters:
 *     handler - pointer to the handler for dealing with pairing.
 *         This handler can be a registered global handler but does
 *         not have to be. 0 can be passed in to disable pairing.
 *
 * Returns:
 *     Pointer to the old handler or 0 if the provided handler is
 *     not initialized properly.
 */
BtHandlerEx* LESEC_RegisterPairingHandler(BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * LESEC_RegisterAuthorizeHandler()
 *
 *     Register the handler that will handle all authorization.
 *     This handler is called when authorization is needed.
 *
 *     If no handler is set (handler == 0) then the authorization
 *     procedure will fail.
 *
 *     The authorization handler is called with the following events:
 *
 *     LESECE_AUTHORIZATION_REQ - If "errCode" is BEC_NO_ERROR, this event
 *     indicates a request for authorization. The link needing authorization
 *     is passed in the "remDev" field of LeSecEvent. During the callback, the
 *     handler can call LESEC_GetSecurityRecord to obtain more information on
 *     what service or channel is being authorized. If "errCode" is NOT
 *     BEC_NO_ERROR, this event indicates a previous authorization request
 *     has been cancelled.
 *
 * Parameters:
 *     handler - pointer to the handler for dealing with
 *         authorization.  This handler can be a registered global
 *         handler or even the same handler that does pairing but it
 *         does not have to be. 0 can be passed in to disable authorization.
 *
 * Returns:
 *     Pointer to the old handler.
 */
BtHandlerEx *LESEC_RegisterAuthorizeHandler(BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * LESEC_SetKeyDistribution()
 *
 *     Set the key distribution to be issued by this device and expected from
 *     a remote device.  This should be setup prior to security being
 *     performed, otherwise no key distribution will be used as the default
 *     value.
 *
 * Parameters:
 *     localKeys - keys to distribute from the local device.
 *
 *     remoteKeys - keys to be distributed from a remote device.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - successfully set the authentication requirements
 *         for the device.
 *
 *     BT_STATUS_FAILED - an invalid authentication requirement was provided.
 */
BtStatus LESEC_SetKeyDistribution(LeSmpKeyDistributionFormat localKeys,
                                  LeSmpKeyDistributionFormat remoteKeys);

/*---------------------------------------------------------------------------
 * LESEC_SetMaxKeySize()
 *
 *     Set the maximum key size (7-16 bytes) supported on this device.  This
 *     should be setup prior to security being performed, otherwise 16 bytes
 *     (BT_LE_KEY_LENGTH) will be used as the default value.
 *
 * Parameters:
 *     maxKeySize - maximum key size value to setup this device to use.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - successfully set the maximum key size
 *         for the device.
 *
 *     BT_STATUS_FAILED - an invalid maximum key size was provided.
 */
BtStatus LESEC_SetMaxKeySize(U8 maxKeySize);

/*---------------------------------------------------------------------------
 * LESEC_GetMaxKeySize()
 *
 *     Return the maximum key size (7-16 bytes) supported on this device. 
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     The maximum key of the device. 
 */
U8 LESEC_GetMaxKeySize(void);

/*---------------------------------------------------------------------------
 * LESEC_OobScDataReceived()
 *
 *     Provide LE Secure Connections Out of Band data received from a remote device.  
 *     When Secure Connections Out of Band data has been received from the remote 
 *     device, it should be provided to LE Security using this function call. 
 *     During the LE Secure Connections Pairing process with a remote device 
 *     whose address matches the one in "oobData", the OOB data will be used as 
 *     part of the pairing process. The LE Security layer stores LE Secure
 *     connections OOB data for one device. To clear the stored data, this 
 *     function can be called with "devAddr.type" set to BTADDR_INVALID or with 
 *     oobData set to 0.
 *
 *     Note that this function must be called before the pairing process is
 *     started (by either side) for the data to be used. It is the presence
 *     of this data that causes the "OOB data available" flag to be set in the
 *     Pairing Request or Pairing Response.
 *
 *     Note that this is used only for LE secure connections OOB data and not
 *     for LE legacy OOB data which is a different format. Use 
 *     LESEC_OobLegDataReceived() to provide LE legacy OOB data.
 *
 * Parameters:
 *     oobData - Pointer to a structure containing LE Secure Connections Out of 
 *         Band Data received from the remote device or 0 to clear the oob data.
 *
 * Returns:
 *
 *     BT_STATUS_SUCCESS - Out of Band data has been registered with LE Security.
 */
BtStatus LESEC_OobScDataReceived(LeSecScOobData *oobData);

/*---------------------------------------------------------------------------
 * LESEC_OobLegDataReceived()
 *
 *     Provide LE legacy Out of Band data received from a remote device.  When 
 *     LE Legacy Out of Band data has been received from the remote device, it 
 *     should be provided to LE Security using this function call. During the 
 *     LE legacy Pairing process with a remote device whose address matches the 
 *     one in "oobData", the LE Legacy OOB data will be used as part of the 
 *     pairing process. The LE Security layer stores LE Legacy OOB data for one 
 *     device. To clear the stored data, this function can be called with 
 *     "devAddr.type" set to BTADDR_INVALID or with oobData set to 0.
 *
 *     Note that this function must be called before the pairing process is
 *     started (by either side) for the data to be used. It is the presence
 *     of this data that causes the "OOB data available" flag to be set in the
 *     Pairing Request or Pairing Response.
 *
 *     Note that this is used for legacy OOB data only. Call LESEC_OobScDataReceived()
 *     to provide LE Secure Connections OOB data. In the case of LE legacy pairing 
 *     both the local device and remote device must have OOB data and the data itself,
 *     must have the same value on both devices for pairing to work correctly. This 
 *     is different than BR/EDR Secure Simple Pairing and LE Secure Connections pairing. 
 *     The mechanism to make this happen is up to the upper layer.
 *
 * Parameters:
 *     oobData - Pointer to a structure containing LE legacy Out of Band Data received
 *         from the remote device or 0 to clear the oob data.
 *
 * Returns:
 *
 *     BT_STATUS_SUCCESS - Out of Band data has been registered with LE Security.
 */
BtStatus LESEC_OobLegDataReceived(LeSecLegOobData *oobData);

/*---------------------------------------------------------------------------
 * LESEC_GetLocalOobData()
 *
 *     Retrieves Out of Band data from the local host controller.  This data
 *     should be transmitted to the remote device using an Out of Band mechanism.
 *     The data is used during LE Pairing.
 *
 *     Note that this function returns LE Secure Connections OOB data which can 
 *     also be used for LE legacy OOB pairing. In the case of legacy pairing only
 *     the "p.oob.randR" value in the LeSecEvent structure is used. Since the OOB 
 *     data for the local and remote devices must be the same in LE legacy pairing
 *     LESEC_OobLegDataReceived() must be called to provide this data to the LE 
 *     Security layer in the case where this data is used as the OOB data on 
 *     both devices.  
 *
 * Parameters:
 *
 *     none
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - operation is started successfully. The pairing
 *         handler will be called with the LESECE_LOCAL_OOB_DATA event
 *         when the out of band data has been generated. 
 *
 *     BT_STATUS_BUSY - operation failed because another operation is already
 *         active.  .
 *
 */
BtStatus LESEC_GetLocalOobData(void);

/*---------------------------------------------------------------------------
 * LESEC_SetMITMCapabilityForOOB()
 *
 *     Set the MITM capability for the OOB mechanism used on this device. 
 *     TRUE indicates that the OOB mechanism is secure from eavesdropping
 *     so the resulting key will be Authenticated. FALSE indicates that the
 *     OOB mechanism is not secure from eavesdropping or the level of 
 *     eavesdropping protection is unknown so the resulting key will be
 *     Unauthenticated. This function returns the old setting.
 *
 *     Note if the OOB authentication method is used then this setting controls
 *     the resulting key type independent of the MITM setting set via
 *     SEC_SetAuthRequirements(). Also if OOB data is present from the remote
 *     device this setting is used for the MITM flag in pairing request and
 *     pairing response packets. 
 *
 * Parameters:
 *
 *     mitmSecure - TRUE means mechanism supports MITM. FALSE means it does not.
 *
 * Returns:
 *
 *     The old setting. 
 *
 */
BOOL LESEC_SetMITMCapabilityForOOB(BOOL mitmSecure);

/*---------------------------------------------------------------------------
 * LESEC_InitiateSecurity()
 *
 *     Initiate LE security on an existing link with a remote device. If a
 *     link key does not exist then pairing will be attempted using the
 *     security level set by SEC_SetAuthRequirements() and bonding mode set
 *     by SEC_SetBondingMode() . Pairing events will be sent to the Pairing
 *     handler. If a pairing handler does not exist then pairing will fail
 *     which in turn will cause security to fail. LESECE_SECURITY_COMPLETE
 *     will be sent to the handler provided. The "errCode" field of the
 *     LeSecEvent specifies if the operation is successful or not.
 *
 *     The link must be active (not in the process of being
 *     disconnected). Also it must not already be authenticated.
 *
 * Parameters:
 *     remDev - pointer to remote device structure.
 *
 *     secLevel - requested security level for this security operation.
 *
 *     forcePair - determines if pairing will be triggered regardless if
 *         security is sufficient or all that is required is a start/refresh
 *         encryption operation.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - current security is sufficient for the requested
 *         security level.
 *
 *     BT_STATUS_PENDING - the operation was started successfully.
 *
 *     BT_STATUS_FAILED - the operation failed because the link
 *         is not in a state to perform security.
 *
 *     BT_STATUS_NOT_READY - incorrect internal state to issue this command.
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
 *     BT_STATUS_RESTRICTED - a pairing operation is occurring on the
 *         BR/EDR transport which may conflict with LE Secure Connections
 *         so wait until it is completes. Pairing on BR/EDR may result in
 *         pairing also being performed on LE.
 */
BtStatus LESEC_InitiateSecurity(BtRemoteDevice *remDev, BOOL forcePair);

/*---------------------------------------------------------------------------
 * LESEC_UserConfirmSecurity()
 *
 *     Responds to an incoming LE security procedure.  This function shall be
 *     invoked after receiving the LESECE_SECURITY_REQUEST event indicating
 *     that the remote device has initiated security.
 *
 * Parameters:
 *     remDev - Pointer to connected remote device.
 *
 *     accept - Determines whether the security procedure will be accepted
 *         or rejected.
 *
 * Returns:
 *     BT_STATUS_PENDING - response was issued and when complete a
 *         LESECE_SECURITY_COMPLETE event is sent to the registered handlers.
 *
 *     BT_STATUS_INVALID_PARM - remDev is 0 (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - remDev is invalid or in wrong state to perform operation.
 *
 *     BT_STATUS_NOT_READY - Unable to continue with the operation. The 
 *         LESECE_SECURITY_COMPLETE event will not be sent.
 *
 *     BT_STATUS_RESTRICTED - a pairing operation is occurring on the
 *         BR/EDR transport which may conflict with LE Secure Connections
 *         so wait until it is completes or reject the request. Pairing on 
 *         BR/EDR may result in pairing also being performed on LE.
 */
BtStatus LESEC_UserConfirmSecurity(BtRemoteDevice *remDev, BOOL accept);

/*---------------------------------------------------------------------------
 * LESEC_SetPasskey()
 *
 *     Assigns the temporary key (TK) value used when forming the SMP pairing
 *     confirm value and also when generating the Short-Term Key (STK) during
 *     the SMP pairing process to the provided 6-digit numeric passkey value.
 *     If less than 6-digits are provided, zeroes will be automatically added
 *     to the beginning of the passkey value.
 *
 *     This function should be called when LESECE_PASSKEY_REQ is received,
 *     but may occur later if the user needs to be prompted for passkey entry.
 *     It should be noted that no further SMP processing will occur until the
 *     passkey is provided, but the stack thread will not block.
 *
 *     If the passkey is 0 then pairing is aborted. Pairing failed is sent with
 *     reason Passkey Entry Failed.
 *
 * Parameters:
 *     remDev - pointer to remote device structure.
 *
 *     passkey - 6-digit numeric passkey or 0 to abort pairing process
 *
 *     len - length of the passkey (cannot exceed 6)
 *
 * Returns:
 *     BT_STATUS_SUCCESS - passkey was successfully provided.
 *
 *     BT_STATUS_FAILED - request failed because link is not valid.
 *
 *     BT_STATUS_BUSY - the Passkey was already setup and is being
 *         used to process the Pairing Confirm value.  It cannot be set
 *         again.
 *
 *     BT_STATUS_NOT_READY - incorrect internal state to issue this command.
 *
 *     BT_STATUS_INVALID_PARM - a parameter was invalid.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with the
 *         SMP channel is no longer active.
 */
BtStatus LESEC_SetPasskey(BtRemoteDevice *remDev, const U8 *passkey, U8 len);

/*---------------------------------------------------------------------------
 * LESEC_UserConfirm()
 *
 *     Provide a user confirmation for the pairing process. User
 *     confirmation is required when "Man In The Middle" protection is
 *     required. It may also be required when the remote device has lost
 *     its key or when LESEC_EnableJustWorksEventGeneration() has been called
 *     to enable generation of the LESECE_CONFIRM_JW_REQ event. In the case
 *     of MITM  IO capabilities of the local device must be SMP_IO_DISPLAY_YESNO 
 *     or SMP_IO_KEYBOARD_DISPLAY to receive a request for user confirmation.
 *     In the case of the LESECE_CONFIRM_JW_REQ the IO capabilities of the 
 *     local device can be any value.
 *
 * Parameters:
 *     remDev - pointer to remote device structure.
 *
 *     accept - TRUE means the pairing is valid. In the case of MITM pairing
 *         the numeric values match. FALSE means the pairing should be aborted. 
 *         In the case MITM the numeric values do not match.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation is started successful. The
 *         pairing handler will be called with the BTEVENT_PAIRING_COMPLETE
 *         event when pairing is complete.
 *
 *     BT_STATUS_FAILED - operation failed because the link is not valid.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remote
 *         device structure is invalid.
 *
 *     BT_STATUS_NOT_READY - incorrect internal state to issue this command.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with the
 *         SMP channel is no longer active.
 *
 */
BtStatus LESEC_UserConfirm(BtRemoteDevice *remDev, BOOL accept);

/*---------------------------------------------------------------------------
 * LESEC_Authorize()
 *
 *     Initiates the LE authorization procedure.  This function can be 
 *     invoked whenever authorization is required, but this will usually
 *     occur when the Server indicates that authorization is needed
 *     (LESECE_AUTHORIZATION_REQ event).
 *
 *     If "isAuthorized" is set to TRUE then temporary authorization
 *     for the remote device is granted otherwise it is denied. If
 *     the "alwaysAuthorize" parameter is set to TRUE then the device
 *     is marked as a authorized device in the device database. The
 *     authorize mark holds beyond this connection. If "alwaysAuthorize" is
 *     FALSE then the device database record is not modified.
 *
 * Parameters:
 *     remDev - Pointer to connected remote device. This can be either
 *         an LE or BR/EDR device.
 *
 *     isAuthorized - TRUE indicates authorization is granted.
 *         FALSE indicates authorization is denied.
 *
 *     alwaysAuthorize - If set to TRUE the device is marked as authorized
 *         in the device database. If set to FALSE the device database
 *         record is not modified. If there is not an existing record
 *         in the device database this parameter is ignored.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - authorization was successful.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote server including a connection that is in the process
 *         of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - token is 0 or is not setup properly (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform operation.
 */
BtStatus LESEC_Authorize(BtRemoteDevice *remDev, BOOL isAuthorized, BOOL alwaysAuthorize);

#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
/*---------------------------------------------------------------------------
 * LESEC_AccessRequest()
 *
 *     Determine if access is allowed for the device specified
 *     by rm at the protocol specified by id for the given
 *     channel. The incoming parameter indicates if the request
 *     is incoming or outgoing.
 *
 * Parameters:
 *     token - pointer to the LE security token.
 *
 *     result - identifies the specific LE security result.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Access is approved
 *
 *     BT_STATUS_PENDING - The operation has started. The results
 *         will be returned via the protocol's callback, which
 *         is defined to be LE L2CAP since no other protocols 
 *         currently exist. The callback is called with "eventType"
 *         LESECE_ACCESS_APPROVED or LESECE_ACCESS_DENIED.
 *
 *     BT_STATUS_FAILED - Access is denied.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus LESEC_AccessRequest(LeSecurityToken *token, LeSecurityResult *result);

/*---------------------------------------------------------------------------
 * LESEC_CancelAccessRequest()
 *
 *     LESEC_CancelAccessRequest cancels the security procedure associated 
 *     with the "token" LeSecurityToken.
 *
 * Parameters:
 *     token - pointer to a Security token with an active access request.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - cancel operation successful
 *
 *     BT_STATUS_FAILED - operation failed. The remote device is
 *         not in a state to cancel access request.
 */
BtStatus LESEC_CancelAccessRequest(LeSecurityToken *token);

/*---------------------------------------------------------------------------
 * LESEC_Register()
 *
 *     Register the LE security record with the security manager. Connections
 *     occurring later with the same channel will have security parameters 
 *     enforced as designated within the security record.
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
 *    BT_STATUS_NOT_SUPPORTED - the level in the security record is set
 *        MLSL_SC_AUTH_KEY_ENCRYPT but the Bluetooth Controller does not 
 *        support LE Secure Connections.
 *    
 *
 *     BT_STATUS_FAILED - operation failed because record is
 *         already on the list.
 */
BtStatus LESEC_Register(LeSecurityRecord *sr);

/*---------------------------------------------------------------------------
 * LESEC_Unregister()
 *
 *     Unregister the LE security record with the security manager.
 *
 * Parameters:
 *     sr - A pointer to the security record.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - security record is unregistered.
 *
 *     BT_STATUS_FAILED - operation failed because record is not
 *         on the list.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the
 *         security record pointer is 0 (error check only).
 */
BtStatus LESEC_Unregister(LeSecurityRecord *sr);

/*---------------------------------------------------------------------------
 * LESEC_FixSecurityError()
 *
 *     Initiates the correct LE security procedure based on the security 
 *     error received from a remote LE L2CAP server when performing a LE 
 *     L2CAP connection operation.
 *
 *     Note that the security related errors are L2DISC_LE_CONN_INSUFF_AUTHEN, 
 *     L2DISC_LE_CONN_INSUFF_KEY_SIZE and  L2DISC_LE_CONN_INSUFF_ENCRYPT. Error
 *     L2DISC_REASON_UNKNOWN can be used to initiate pairing in a way similar
 *     to LESEC_InitiateSecurity() if no security is currently in place.
 *
 *     It is possible that after LESEC_FixSecurityError() another attempt at the
 *     LE L2CAP connection operation will result in an error. LESEC_FixSecurityError()
 *     should be called again. If errors still occur (in particular 
 *     L2DISC_LE_CONN_INSUFF_AUTHEN) then it is possible that the remote device
 *     requires a Secure Connection key and the local device does not support Secure
 *     Connections. In this case continuing to call LESEC_FixSecurityError() will not
 *     fix the error. Therefore, it is suggested that if calling LE L2CAP connection
 *     request continues to produce errors even after calling LESEC_FixSecurity() twice 
 *     then the LE L2CAP connect operation should not be attempted any more. 
 *
 * Parameters:
 *     handler - pointer to handler to receive the LESECE_SECURITY_COMPLETE
 *         event. The "callback" field must be set.
 *
 *     remDev - pointer to remote device structure.
 *
 *     securityLevel - LE L2CAP disconnect reason for the connection failure.
 *         The acceptable values range from L2DISC_LE_CONN_INSUFF_AUTHEN to
 *         L2DISC_LE_CONN_INSUFF_ENCRYPT and L2DISC_REASON_UNKNOWN.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully. The handler
 *         will be called with the LESECE_SECURITY_COMPLETE event when the
 *         operation completes.
 *
 *     BT_STATUS_BUSY - the hander is already involved in a request.
 *
 *     BT_STATUS_FAILED - the operation failed because the link
 *         is not in a state to perform security.
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
 *     BT_STATUS_RESTRICTED - a pairing operation is occurring on the
 *         BR/EDR transport which may conflict with LE Secure Connections
 *         so wait until it is completes. Pairing on BR/EDR may result in
 *         pairing also being performed on LE.
 */
BtStatus LESEC_FixSecurityError(BtHandlerEx *handler, BtRemoteDevice *remDev, 
                                L2capDiscReason securityError);

/*---------------------------------------------------------------------------
 * LESEC_SetDefaultSecurityLevel()
 *
 *     Set the default security levels used by LESEC_AccessRequest() for 
 *     services that do not have a registered security record.
 *
 * Parameters:
 *     inLevel- the desired default level for incoming connections.
 *
 *     outLevel- the desired default level for outgoing connections.
 * Returns:
 *     void
 *
 */
void LESEC_SetDefaultSecurityLevel(LeSecSecurityLevel inLevel, LeSecSecurityLevel outLevel);

/*---------------------------------------------------------------------------
 * LESEC_GetDefaultSecurityLevel()
 *
 *     Get the default security levels used by LESEC_AccessRequest() for
 *     services that do not have a registered security record.
 *
 * Parameters:
 *     inLevel - pointer to memory to receive the default level for incoming
 *               connections. Can be set to 0 if value is not desired.
 *
 *     inLevel - pointer to memory to receive the default level for outgoing
 *               connections. Can be set to 0 if value is not desired.
 *
 * Returns:
 *     void
 *
 */
void LESEC_GetDefaultSecurityLevel(LeSecSecurityLevel *inLevel, LeSecSecurityLevel *outLevel);

#endif /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */

/*---------------------------------------------------------------------------
 * LESEC_SetZeroIdentityAdd()
 *
 *     Set which identity address that will be distributed during pairing.
 *     If TRUE a zero identity address will be sent otherwise the public
 *     address of the device will be sent.
 *
 * Parameters:
 *     useZero - TRUE indicates that a zero identity address will be
 *               distributed during pairing. FALSE indicates that the
 *               public address of the device will be distributed.
 *
 * Returns:
 *     The old value
 *
 */
BOOL LESEC_SetZeroIdentityAddr(BOOL useZero);

/*---------------------------------------------------------------------------
 * LESEC_GetZeroIdentityAdd()
 *
 *     Get which identity address that will be distributed during pairing.
 *     If TRUE a zero identity address will be sent otherwise the public
 *     address of the device will be sent.
 *
 * Returns:
 *     TRUE - zero identity address will be distributed during pairing.
 *
 *     FALSE - the public address of the device will be distributed. 
 *
 */
BOOL LESEC_GetZeroIdentityAddr(void);

/*---------------------------------------------------------------------------
 * LESEC_EnableJustWorksEventGeneration()
 *
 *     Enable/Disable the generation of the LESECE_CONFIRM_JW_REQ 
 *     event. If enabled the LESECE_CONFIRM_JW_REQ event is sent for
 *     "just works" pairing when the stack determines that the pairing
 *     operaton will result in a Just works key. This event is sent at
 *     similar point in the pairing process where the LESECE_PASSKEY_REQ
 *     or LESECE_PASSKEY_DISPLAY event is sent when a MITM key is being
 *     generated. LESEC_UserConfirm() must be called for pairing to continue.
 *
 *     If disabled the LESECE_CONFIRM_JW_REQ is not sent and the flow of 
 *     LE pairing events is unchanged.
 *
 * Parameters:
 *     enable - TRUE enables generation of the event while FALSE disables 
 *         generation of the event.
 *
 * Returns:
 *     The old setting.
 */
BOOL LESEC_EnableJustWorksEventGeneration(BOOL enable);

/*---------------------------------------------------------------------------
 * LESEC_GetJustWorksEventGeneration()
 *
 *     Get the current setting for the generation of the 
 *     LESECE_CONFIRM_JW_REQ event. 
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     The current setting.
 */
BOOL LESEC_GetJustWorksEventGeneration(void);

#if JETTEST == XA_ENABLED
/* Special test functions for causing SMP protocol errors */
void JestTestSmp_SetAuthReq(U8 val);
void JestTestEmp_SetScFailure(U8 failType);

#endif /* JETTEST == XA_ENABLED */

#endif /* __LESEC_H */


