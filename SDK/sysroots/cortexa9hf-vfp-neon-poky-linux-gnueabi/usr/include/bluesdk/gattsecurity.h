/****************************************************************************
 *
 * File:
 *     gattsecurity.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     upper layer of Generic Attribute Profile (GATT) security for client
 *     and server applications.
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

#ifndef __GATTSECURITY_H
#define __GATTSECURITY_H

#include "mele.h"
#include "gatt.h"
#include "gattuuid.h"
#include "sdp.h"
#include "gattserver.h"
#include "lesec.h"

/*---------------------------------------------------------------------------
 * Generic Attribute Profile (GATT) Security layer
 *
 *     The Generic Attribute Profile Security layer provides functions that
 *     allow GATT client and server applications to make security requests to 
 *     remote devices in order to satisfy security requirements discovered 
 *     during the execution of GATT operations. 
 *
 *     There are general purpose functions intended to be used by both
 *     GATT client and GATT server applications as well as specific functions
 *     for GATT client applications and for GATT server applications. There is
 *     an API for setting a GATT security handler that is intended for an 
 *     application that implements GAP functionality. 
 *
 */


/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GattSecurityEventType type
 *
 *     The GATT Security Event types are passed to the token handler to
 *     indicate completion of security operations. If a Security handler
 *     is registered it will receive all events. Also some operations
 *     use a security token which will received events related to the 
 *     specific security operation in which the token is involved. 
 */
typedef BtEventTypeEx GattSecurityEventType;

/* The LE or BR/EDR security procedure has completed. "e.status"
 * indicates the success or failure of the operation. If "e.status"
 * is BT_STATUS_FAILED then an error occurred during the security
 * procedure which is reported in "secFailureReason". It is possible that
 * a security operation can complete successfully (i.e. "e.status" set to
 * BT_STATUS_SUCCESS) but the desired security level is not achieved.
 * This can occur if the I/O Capabilities of the two devices are not 
 * sufficient to achieve the desired security level. If "e.status" is
 * BT_STATUS_SUCCESS then "secLevelAchieved" indicates if the requested 
 * security level has been achieved. TRUE mean the requested security level 
 * has been achieved while FALSE means it has not. 
 */
#define GATTSEC_SECURITY_COMPLETE                (BTE_TYPE_BASE_GATTSEC + 0)

/* End of GattSecurityEventType */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GattSecurityToken structure
 *
 *     GattSecurityToken is used to make security requests.
 */
typedef struct _GattSecurityToken {
    /* Inherit from BtHandlerEx */
    BtHandlerEx            h;

    /* === Internal use only === */

    /* Pointer to remote device (either BR/EDR or LE )*/
    BtRemoteDevice        *remDev;

    /* Pointer to ATT context */
    void                  *attCtx;

    BOOL                   encryptOnly;

    LeSecSecurityLevel     secLevel;   /* Security level for the requested operation */

    /* Event that should be sent to handler when operation completes */
    GattSecurityEventType  eType;

    LeSmpPairingFailureReason   errCode;    /* SMP Pairing Failure Reason Code */

} GattSecurityToken;

/*---------------------------------------------------------------------------
 * GattSecurityEvent structure
 *
 *     Used to pass information from the GATT layer to GATT applications.
 */
typedef struct _GattSecurityEvent {
    /* Inherit from BtEventEx */
    BtEventEx                   e;

    BtRemoteDevice             *remDev;

    /* Used for GATTCE_SECURITY_REQUEST event */
    BOOL                        keyReplace;

    /* Group: Information used for GATTSE_SECURITY_COMPLETE event */

    /* Error code if e.status is not BT_STATUS_SUCCESS */
    LeSmpPairingFailureReason   secFailureReason;

    /* Valid only when e.status is BT_STATUS_SUCCESS. TRUE indicates the
     * desired security level was achieved while FALSE means it was
     * not achieved. For example if the desired security level was
     * MITM but only "just works" was achieved this flag would be FALSE.
     */
    BOOL                        secLevelAchieved;

} GattSecurityEvent;


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GATTSEC_InitToken()
 *
 *     Initialize the security token for operation over a link.
 *
 * Parameters:
 *     token - Pointer to token with handler fields initialized (especially
 *         h.callback).
 *
 *     remDev - Pointer to remote device structure representing the link to
 *         the remote GATT entity.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed because remDev is not valid.
 *
 *     BT_STATUS_INVALID_PARM - token, link or h.callback in token is 0
 *         (XA_ERROR_CHECK only).
 *
 */
BtStatus GATTSEC_InitToken(GattSecurityToken *token, BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * GATTSEC_FixSecurityError()
 *
 *     Initiates the correct LE or BR/EDR security procedure based on the
 *     the security error received from a remote GATT server when performing
 *     a GATT client operation and the current security state with the remote GATT 
 *     server.
 *
 *     Note that the security related errors are 
 *     ATTEC_INSUFFICIENT_AUTHENTICATION, ATTEC_INSUFFICIENT_ENCRYPTION 
 *     and ATTEC_INSUFFICIENT_ENCRYPT_KEY_SIZE.
 *
 *     It is possible that after calling GATTSEC_FixSecurityError() another attempt at the
 *     GATT client operation will result in an error. GATTSEC_FixSecurityError() should
 *     be called again. If errors still occur (in particular 
 *     ATTEC_INSUFFICIENT_AUTHENTICATION) then it is possible that the remote devce
 *     requires a Secure Connection key and the local device does not support Secure
 *     Connections. In this case continuing to call GATTSEC_FixSecurityError() will not
 *     fix the error. Therefore, it is suggested that a if GATT operation continues to
 *     produce errors even after calling GATTSEC_FixSecurity() twice then the GATT operation
 *     should not be attempted any more. 
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device and
 *         security information. 
 *
 *     securityError - the security error received from a remote GATT server.
 *
 * Returns:
 *     BT_STATUS_PENDING - request was started and when complete a
 *         GATTCE_SECURITY_COMPLETE event will be sent to the token handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT server including a connection that is in the process
 *         of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_TYPE - security error is not a valid security related
 *         error.
 *
 *     BT_STATUS_INVALID_PARM - token is 0 or token information is not
 *         setup properly (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform operation.
 *
 *     BT_STATUS_RESTRICTED - The pairing request is for the LE transport and
 *         a pairing operation is occuring on the BR/EDR transport or the pairing 
 *         request is for the BR/EDR transport and a pairing operation is occuring 
 *         on the LE transport. Wait until pairing is complete on the other transport
 *         before performing this operation. With BR/EDR and LE secure connections 
 *         pairing one transport may result in pairing also occuring on the other
 *         transport.

 */
BtStatus GATTSEC_FixSecurityError(GattSecurityToken *token, AttErrorCode securityError);

/*---------------------------------------------------------------------------
 * GATTSEC_SetupSecurityNI()
 *
 *     Setup the correct security required for a GATT Server notification or 
 *     indication request. The 'service' and 'index' parameters identify
 *     the attribute that will be notified/indicated and are used to 
 *     determine the required security. The read permissions of the attribute
 *     are checked against the current security state and if the current
 *     state is found to be insufficient the correct security procedure 
 *     is invoked to achieve the required security state. 
 *
 * Parameters:
 *     token - Pointer to a token initialized by calling GATTSEC_InitToken() and
 *         containing the information for a notification/indication for which
 *         security is checked. 
 *
 *     service - Pointer to service used to identify the attribute being notified
 *         or indicated in order to check the read permissions against the
 *         current security state. 
 *
 *     index - Index of attribute in service used to identify the attribute being 
 *         notified or indicated in order to check the read permissions against 
 *         the current security state. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - security is sufficient based on the current
 *         security level.
 *
 *     BT_STATUS_PENDING - a security request was started and when complete a
 *         GATTSEC_SECURITY_COMPLETE event will be sent to the token handler.
 *
 *     BT_STATUS_RESTRICTED - request failed because authorization is required but has 
 *         been rejected by the user.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT client including a connection that is in the process
 *         of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - token is 0 or "token->service" is 0
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform operation or
 *         attribute is invalid (service, index).
 */
BtStatus GATTSEC_SetupSecurityNI(GattSecurityToken *token, GattService *service,
                                 U16 index);

#endif /* __GATTSECURITY_H */
