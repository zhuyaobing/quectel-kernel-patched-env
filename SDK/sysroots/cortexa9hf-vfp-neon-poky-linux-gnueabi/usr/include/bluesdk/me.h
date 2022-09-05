/****************************************************************************
 *
 * File:
 *     me.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     upper layer of the Management Entity.
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

#ifndef __ME_H
#define __ME_H

#include "osapi.h"
#include "bttypes2.h"
#include "radiomgr.h"
#include "sys/hci.h"
#include "sys/ilink.h"

/*---------------------------------------------------------------------------
 * Management Entity (ME) layer
 *
 *     The Management Entity allows applications to inquire for
 *     remote devices, establish different types of connections, and
 *     control the access to this device by other devices.
 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BT_INQ_TIME_GAP100 constant
 *     Time value specified by Generic Access Profile for the length
 *     of an Inquiry process.
 */
#define BT_INQ_TIME_GAP100 8

/*---------------------------------------------------------------------------
 * BT_MAX_REM_DEV_NAME constant
 *
 *     Maximum length of a Remote Device name (friendly name) allowed by
 *     Bluetooth. This does not include the null termination at the end.
 */
#define BT_MAX_REM_DEV_NAME 248

/*---------------------------------------------------------------------------
 * BT_INVALID_RSSI constant
 *
 *     Indicates an invalid RSSI value (used when RSSI is not reported
 *     by the controller).
 */
#define BT_INVALID_RSSI   127

/*---------------------------------------------------------------------------
 * BT_BR_EDR_KEY_LENGTH constant
 *
 *     This define indicates the size of a BR/EDR Link Key.
 */
#define BT_BR_EDR_KEY_LENGTH 16

/*---------------------------------------------------------------------------
 * BT_MAX_AMP_KEY_LENGTH constant
 *
 *     This define indicates the maximum size of AMP Keys for
 *     both the generic AMP and for any dedicated AMP type.
 */
#define BT_MAX_AMP_KEY_LENGTH 32

/*---------------------------------------------------------------------------
 * BT_DB_REV_CHARS constant
 *
 *     This define indicates the maximum number of characters that can be in
 *     a database record revision string, including the terminating zero
 *     character.
 */
#define BT_DB_REV_CHARS 8

/*---------------------------------------------------------------------------
 * BT_NUMBER_AMP_TYPES constant
 *
 *     This define indicates the total number of AMP types.  This number
 *     excludes the BR/EDR controller, the Fake PAL controller, and any
 *     future controller yet to be specified such as UWB.  (See
 *     BtControllerType.)
 */
#define BT_NUMBER_AMP_TYPES 1

/*---------------------------------------------------------------------------
 * BT_MAX_EIR_LENGTH constant
 *
 *     This define indicates the maximum byte length of an Extended Inquiry
 *     Response created for an SDP.
 */
#define BT_MAX_EIR_LENGTH 240

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtEventType type
 *
 *     All indications and confirmations are sent through a callback
 *     function. The event types are defined below.
 */
typedef U8 BtEventType;

/* Group: Global events sent to all handlers. These can be masked out if
 * desired.
 */

/* A remote device is found. "p.inqResult" is valid.  */
#define BTEVENT_INQUIRY_RESULT                1

/* The Inquiry process is completed. "errCode" is valid. */
#define BTEVENT_INQUIRY_COMPLETE              2

/* The Inquiry process is canceled. */
#define BTEVENT_INQUIRY_CANCELED              3

/* An incoming ACL connection. "p.remDev" is valid. */
#define BTEVENT_LINK_CONNECT_IND              4

/* An incoming SCO connection. "p.scoConnect" is valid. */
#define BTEVENT_SCO_CONNECT_IND               5

/* An ACL connection was disconnected. "p.disconnect" is valid, and
 * "errCode" may be valid (see documentation for the "disconnect" union for
 * details. This event will only occur if the BTEVENT_LINK_CONNECT_CNF event 
 * has previously occurred. */
#define BTEVENT_LINK_DISCONNECT               6

/* An outgoing ACL connection attempt has completed. This event occurs 
 * after several additional HCI commands have been issued. "p.remDev" 
 * and "errCode" are valid. If "errCode" is BEC_NO_ERROR then the
 * the connection is now considered usable. If "errCode" is not
 * BEC_NO_ERROR then connection did not complete succeessful or was
 * disconnected or canceled before it came up completely.
 */
#define BTEVENT_LINK_CONNECT_CNF              7

/* Event number 8 is not used */

/* Indicates the mode of the link changed. "p.modeChange" is valid */
#define BTEVENT_MODE_CHANGE                   9

/* Indicates that an accessibility change is complete. "errCode" and
 *  "p.aMode" are valid. */
#define BTEVENT_ACCESSIBLE_CHANGE             10

/* Indicates that an authentication attempt has completed. "p.remDev" and
 * "errCode" are valid.
 */
#define BTEVENT_AUTHENTICATED                 11

/* Encryption settings have changed. "p.encrypt" is valid. */
#define BTEVENT_ENCRYPTION_CHANGE             12

/* Security mode has changed. "p.secMode" is valid. */
#define BTEVENT_SECURITY_CHANGE               13

/* Indicates that a role change has occurred */
#define BTEVENT_ROLE_CHANGE                   14

/* SCO link has been disconnected. "p.scoConnect" is valid. */
#define BTEVENT_SCO_DISCONNECT                15

/* An outgoing SCO link has been established. "p.scoConnect" is valid. */
#define BTEVENT_SCO_CONNECT_CNF               16

/* Simple Pairing Mode completed. "errCode" and "p.bdAddr"
 *  are valid */
#define BTEVENT_SIMPLE_PAIRING_COMPLETE       17

/* Returned after each ACL connection is established to a remote device
 *  "p.remoteFeatures" is valid.
 */
#define BTEVENT_REMOTE_FEATURES               18

/* Returned after each ACL connection is established to a remote device
 *  "p.remoteExtFeatures" is valid.  This event is returned for each
 *  supported page of extended features.
 */
#define BTEVENT_REMOTE_EXT_FEATURES           19

/* Remote Host Supported Features.  Returned by 2.1 (Lisbon) radios before
 * each Remote Name Request response.  Also, returned after each ACL
 * connection is established to a remote device "p.remHostFeatures" is valid.
 */
#define BTEVENT_REM_HOST_FEATURES             20

/* Link Supervision Timeout Changed.  "p.linkSupervision" is valid.
 */
#define BTEVENT_LINK_SUPERV_TIMEOUT_CHANGED   21

/* Sniff Subrating parameters have been set.
 *  "p.sniffSubrateParms" is valid.  This event occurs as the
 *  result of a call to ME_SetSniffSubratingParms().  When Sniff
 *  Subrating is enabled on the connection, the
 *  BTEVENT_SNIFF_SUBRATE_INFO event will be received.
 */
#define BTEVENT_SET_SNIFF_SUBRATING_PARMS_CNF 22

/* Sniff Subrating has been enabled (or re-negotiated) on the
 *  specified connection.  "p.sniffSubrateInfo" is valid.
 */
#define BTEVENT_SNIFF_SUBRATE_INFO            23

/* The inquiry mode has been set. "p.inqMode" is valid. */
#define BTEVENT_SET_INQUIRY_MODE_CNF          24

/* The inquiry TX power level has been set.  "p.inqTxPwr" is valid.
 */
#define BTEVENT_SET_INQ_TX_PWR_LVL_CNF        25

/* The extended inquiry response has been set. */
#define BTEVENT_SET_EXT_INQUIRY_RESP_CNF      26

/* The erroneous data reporting state has been set */
#define BTEVENT_SET_ERR_DATA_REPORTING_CNF    27

/* A Keypress event was received during simple pairing.  "p.keyPress" is
 *  valid.
 */
#define BTEVENT_KEY_PRESSED                   28

/* A QOS Setup Complete event was received "p.qos" is valid */
#define BTEVENT_QOS_SETUP_COMPLETE            29

/* A device's page scan repetition mode has been updated.
 * "p.inqResult.bdAddr" and "p.inqResult.psi.psRepMode" are valid. No other
 * "p.inqResult" fields are valid.
 */
#define BTEVENT_PSREP_MODE_CHANGE             30

/* Indicates that a connection packet type change event has occurred.
 *  "p.packetTypeChng" is valid.
 */
#define BTEVENT_CONN_PACKET_TYPE_CHNG         31

/* Information relevant to the Extended Inquiry Response (EIR) has changed,
 *  and the EIR data may need to be updated.  This includes changes to the
 *  SDP server database, and the local device name. The EIR may be updated by
 *  calling ME_SetExtInquiryRsp().
 */
#define BTEVENT_UPDATE_EIR_REQ                32

/* A vendor specific event was received.  Vendor specific events are
 *  defined by the company that designed the Bluetooth controller, and are
 *  not parsed or interpreted in any way by the stack.  During this event,
 *  "p.vendorSpecific" is valid.
 */
#define BTEVENT_VENDOR_SPECIFIC               33

/* An outgoing ACL connection that has come partially up but where the
 * BTEVENT_LINK_CONNECT_CNF has not yet been sent has been aborted. "p.remDev" 
 * and "errCode" are valid. 
 */
#define BTEVENT_LINK_CONNECT_ABORT            34

/* Group: The following events are global events but cannot be masked */

/* Indicates that an ACL connection has received an internal data transmit
 *  request while it is in hold, park or sniff mode. The data will still be
 *  passed to the radio in park and sniff modes. However, hold mode will
 *  block data transmit. It may be necessary to return the ACL to active
 *  mode to restore normal data transfer. "p.remDev" is valid.
 */
#define BTEVENT_ACL_DATA_NOT_ACTIVE           100

/* Indicates that the HCI failed to initialize. This implies that the
 *  Bluetooth radio is having problems or a radio may not exist.
 *  Events will be generated with an appropriate error code for any outstanding
 *  operations.  All pending operations that use pointers to memory supplied by
 *  applications will have a corresponding event generated indicating the
 *  failure of the operation. Applications should not modify memory passed as
 *  part of operations until an event signaling the end of the operation
 *  has been received. The HCI and the radio will be reset. If the reset is
 *  successful then the BTEVENT_HCI_INITIALIZED event will be sent to all
 *  registered handlers.
 */
#define BTEVENT_HCI_INIT_ERROR                101

/* Indicates that the HCI initialization is successful. The ME will now
 *  accept commands again if an initialization error or fatal error
 *  has occurred. This event is sent whenever HCI is successfully initialized,
 *  including when the stack is first started.
 */
#define BTEVENT_HCI_INITIALIZED               102

/* Indicates that a fatal error has occurred in the radio or the HCI transport.
 *  The HCI and the radio will be reset. If the reset is successful then the
 *  BTEVENT_HCI_INITIALIZED event will be sent to all registered handlers.
 */
#define BTEVENT_HCI_FATAL_ERROR               103

/* Indicates that the HCI has been deinitialized.  This can happen as the
 *  result of a call to ME_RadioShutdown, or if some fatal error was reported
 *  by the radio or HCI transport. For any pending operations, events will
 *  be received with proper error codes prior to receiving this event.
 *  All pending commands that used memory supplied by the application will
 *  result in an event, so the application should not modify that memory until
 *  the event has been received. If the HCI was deinitialized as the result of
 *  a radio or transport error, the HCI will be reset. If the reset is
 *  successful then the BTEVENT_HCI_INITIALIZED event will be sent to all
 *  registered handlers.
 */
#define BTEVENT_HCI_DEINITIALIZED             104

/* Indicates that the HCI cannot be initialized. This happens when
 *  the HCI has been reset BT_HCI_NUM_INIT_RETRIES without success or
 *  if some unrecoverable error occurs.
 */
#define BTEVENT_HCI_FAILED                    105

/* Indicates that an HCI command has been sent to the radio. This
 *  event is intended for global handlers that would like to be informed
 *  of when certain radio processes are started. "p.hciCmd" is valid.
 */
#define BTEVENT_HCI_COMMAND_SENT              106

/* An outgoing create link has been canceled. "p.remDev" is valid. */
#define BTEVENT_LINK_CREATE_CANCEL            107

/* Group: These events are sent only to the handler that initiated the
 * operation.
 */

/* Indicates the name of a remote device or cancellation of a name request.
 *  "p.meToken" is valid.
 */
#define BTEVENT_NAME_RESULT                   200

/* Incoming SCO data has been received. Fields in "p.scoDataInd" are valid
 *  and describe the data received. This data is valid only for the duration
 *  of the callback.
 */
#define BTEVENT_SCO_DATA_IND                  201

/* Outgoing SCO data has been sent and the packet is free for re-use by
 *  the application. "p.scoPacketHandled.scoPacket" and
 *  "p.scoPacketHandled.scoCon" are valid.
 */
#define BTEVENT_SCO_DATA_CNF                  202

/* Incoming connection request. "p.remDev" is valid. */
#define BTEVENT_LINK_CONNECT_REQ              203

/* Incoming link accept complete. "p.remDev" is valid. */
#define BTEVENT_LINK_ACCEPT_RSP               204

/* Incoming link reject complete. "p.remDev" is valid. */
#define BTEVENT_LINK_REJECT_RSP               205

/* General command (async or sync) has completed. "p.meToken" is valid */
#define BTEVENT_COMMAND_COMPLETE              206

/* Incoming connection request. "p.scoConnect" is valid. The field
 *  "p.scoConnect.scoLinkType" indicates the SCO link type (BLT_SCO or
 *  BLT_ESCO.) When the link is of type BLT_ESCO, the fields
 *  p.scoConnect.scoTxParms and p.scoConnect.scoRxParms point to the
 *  associated eSCO connection parameters.
 */
#define BTEVENT_SCO_CONNECT_REQ               207

/* Set Audio/Voice settings complete.  
 *  "p.meToken" is valid.
 */
#define BTEVENT_SCO_VSET_COMPLETE             208

/* SCO link connection process started. */
#define BTEVENT_SCO_STARTED                   209

/* Select Device operation complete, "p.select" is valid. */
#define BTEVENT_DEVICE_SELECTED               210

/* The eSCO connection has changed. "p.scoConnect" is valid.
 *  The structure "p.scoConnect.scoRxParms" contains the updated
 *  connection parameters.
 */
#define BTEVENT_SCO_CONN_CHNG                 211

/* Set Read Local Supported Codecs command is complete. The
 *  results are contained in "p.localSupportedCodecs."
 */
#define BTEVENT_READ_SUPPORTED_CODECS         212

/* Group: Security-related events. */

/* Indicates access request is successful. "p.secToken" is valid. */
#define BTEVENT_ACCESS_APPROVED               213

/* Indicates access request failed. "p.secToken" is valid. */
#define BTEVENT_ACCESS_DENIED                 214

/* Request authorization when "errCode" is BEC_NO_ERROR. Other
 * error codes indicate cancellation of an outstanding authorization
 * request.
 *
 *  "p.remDev" is valid.
 */
#define BTEVENT_AUTHORIZATION_REQ             215

/* Request a Pin for pairing when "errCode" is BEC_NO_ERROR.
 *  "p.pinReq" is valid. If p.pinReq.pinLen is > 0 then SEC_SetPin()
 *  must be called in response to this event with a pin length >=
 *  p.pinReq.pinLen.
 */
#define BTEVENT_PIN_REQ                       216

/* Pairing operation is complete.  When the pairing is complete, a link key
 *  is generated and received from the controller.  This event includes
 *  information about the type of link key that was created.  With Secure
 *  Simple Pairing, it is possible that an unauthenticated link key will be
 *  created when MITM protection was requested (depending on the IO
 *  capabilities of the remote device).  Also, if Secure Simple Pairing is in
 *  debug mode (see SEC_SetSimplePairingDebugMode()), fixed private/public
 *  keys pairs are used, which present a security risk.
 *
 *  The application can take the appropriate action when the link key is not
 *  as strong as required.  If the link key type is not satisfactory, the
 *  application should disconnect the link with a BEC_AUTHENTICATE_FAILURE
 *  error code.  The 'p.pairingInfo' parameter is valid during this event.
 */
#define BTEVENT_PAIRING_COMPLETE              217

/* Authentication operation complete. "p.remDev" is valid. */
#define BTEVENT_AUTHENTICATE_CNF              218

/* Encryption operation complete. "p.remDev" is valid. */
#define BTEVENT_ENCRYPT_COMPLETE              219

/* Security mode 3 operation complete. "p.secMode" is valid. */
#define BTEVENT_SECURITY3_COMPLETE            220

/* A link key is returned. "p.bdLinkKey" is valid.  */
#define BTEVENT_RETURN_LINK_KEYS              221

/* Out of Band data has been received from the host controller. "p.oobData"
 *  is valid.  This data should be transmitted to the remote device using an
 *  out of band mechanism. 
 */
#define BTEVENT_LOCAL_OOB_DATA                222

/* Request a Pass Key for simple pairing. The application must call
 * SEC_SetPassKey() to provide the passkey or reject the request, and to
 * indicate link key storage requirements. "p.userIoReq" is valid.
 *
 * "errCode" must be checked by the pairing handler. If it is not
 * BEC_NO_ERROR, then this event cancels the previous request.
 */
#define BTEVENT_PASS_KEY_REQ                  223

/* Request user confirmation for simple pairing. The application must
 * call SEC_UserConfirm() to confirm or reject this request, and to indicate
 * link key storage requirements. "p.userIoReq" is valid for this event,
 * and contains the number which the user must confirm.
 *
 * "errCode" must be checked by the pairing handler. If it is not
 * BEC_NO_ERROR, then this event cancels the previous request.
 */
#define BTEVENT_CONFIRM_NUMERIC_REQ           224

/* Indicate that a numeric value must be displayed so that the remote device
 * can enter it. "p.userIoReq" is valid.
 *
 * The application should display the numeric value provided and clear the
 * display once authentication has completed. The link key is automatically
 * saved when either device is in bonding mode.
 *
 * "errCode" must be checked by the pairing handler. If it is not
 * BEC_NO_ERROR, then this event cancels the previous request.
 */
#define BTEVENT_DISPLAY_NUMERIC_IND           225

/* Request approval for procedures that will result in a bonding change.
 * "remDev", "keyReplace", "bondingMode", "bondingModeComplete" and "remBondingMode"
 * members of "p.userIoReq" are valid. SEC_UserConfirm() must be called by the
 * application to approve or deny the request.
 *
 * This request always occurs if the local device initiates security and wants bonding. In this
 * case the stack does not have the bonding informaton from the remote device yet. The
 * "p.userIoReq.bondingModeComplete" field can be used to determine this situation. In other
 * situations this event is generated if no other opportunity to confirm/deny the bond
 * is expected later in the bonding process.
 *
 * This event is not sent when replacing or upgrading a key on an authenticated
 * link.
 *
 * "errCode" must be checked by the pairing handler. If it is not
 * BEC_NO_ERROR, then this event cancels the previous request.
 *
 * Note that BTEVENT_CONFIRM_NUMERIC_REQ for MITM pairing and BTEVENT_CONFIRM_JW_REQ
 * for "Just Works" pairing can be used to accept/reject pairing and control bonding.
 * When these two events are sent bonding information from both the local and remote
 * devices is known.
 */
#define BTEVENT_BOND_REQ                      226

/* Out of Band extended data has been received from the host controller. 
 * "p.oobExtData" is valid.  This data should be transmitted to the remote device 
 *  using an out of band mechanism. 
 */
#define BTEVENT_LOCAL_OOB_EXT_DATA            227

/* Read Authentication Payload Timeout command is complete. The
 *  result is contained in "p.authPayloadTimeout."
 */
#define BTEVENT_WRITE_AUTH_PAYLOAD_TIMEOUT    228

/* Read Authentication Payload Timeout command is complete. The
 *  result is contained in "p.authPayloadTimeout."
 */
#define BTEVENT_READ_AUTH_PAYLOAD_TIMEOUT     229

/* An authenticated payload timeout event has occurred.  This signifies
 * that the defined maximum amount of time between receiving a packet with 
 * a valid Message Integrity Check (MIC) has been exceeded. A Host may 
 * choose to disconnect the link when this occurs. "p.remDev" is valid.
 */
#define BTEVENT_AUTH_PAYLOAD_TIMEOUT          230

/* Group: SDP Query related events. In all cases, the "p.token" parameter
 * is valid.
 */

/* SDP query completed successfully. The token should be examined for further
 * information about query results.
 */
#define SDEVENT_QUERY_RSP                     231

/* SDP query completed with an error. The token's "errorCode" member
 * describes the error.
 */
#define SDEVENT_QUERY_ERR                     232

/* SDP query failed. "errCode" describes the reason for query failure. */
#define SDEVENT_QUERY_FAILED                  233

/* Group: Device Selection monitor events. These events are only sent to
 * the device selection handler.
 */

/* Request device selection, "p.select" is valid. The device selection
 *  monitor must call DS_SelectDeviceResult() to return the provided
 *  selection request when the selection process is complete.
 */
#define BTEVENT_SELECT_DEVICE_REQ             234

/* Device added to the device database, "p.device" is valid. Note that
 *  during a device inquiry, BTEVENT_DEVICE_ADDED event may not mirror
 *  BTEVENT_INQUIRY_RESULT events. This is because duplicate devices are
 *  not added to the device database and because the device database may
 *  reach capacity, preventing the addition of new devices.
 */
#define BTEVENT_DEVICE_ADDED                  235

/* Device removed from the device database, "p.device" is valid. */
#define BTEVENT_DEVICE_DELETED                236

/* Request user confirmation for "Just Works" simple pairing. The application
 * must call SEC_UserConfirm() to confirm or reject this request, and to indicate
 * link key storage requirements. "p.userIoReq" is valid for this event.
 *
 * "errCode" must be checked by the pairing handler. If it is not
 * BEC_NO_ERROR, then this event cancels the previous request.
 *
 * This event is only sent if SEC_EnableJustWorksEventGeneration() has
 * been called to enable generation of the event.
 */
#define BTEVENT_CONFIRM_JW_REQ                237

/* A BR/EDR link key was generated during an LE pairing process (Cross transport
 * Key generation). This event is sent to the registered pairing handler.
 * only. The 'p.crossKey' parameter is valid during this event.
 */
#define BTEVENT_CROSS_TRANS_KEY               238

/* End of BtEventType */

/* Provided for backward-compatibility; do not use. Use BTEVENT_BOND_REQ
 * instead.  NOTE: This value has the same value as BTEVENT_BOND_REQ to 
 * preserve backward-compatibility.
 */
#define BTEVENT_REPLACE_KEY_REQ               226

/*---------------------------------------------------------------------------
 * BtEventMask type
 *
 *     All registered handlers can receive global events. Each handler has
 *     the ability to mask all or some of the events. A handler receives only
 *     events with the mask set. They can be ORed together. The mask requires
 *     two 32-bit variables and this is the first one.
 */
typedef U32 BtEventMask;

#define BEM_NO_EVENTS                    0x00000000
#define BEM_INQUIRY_RESULT               0x00000001
#define BEM_INQUIRY_COMPLETE             0x00000002
#define BEM_INQUIRY_CANCELED             0x00000004
#define BEM_LINK_CONNECT_IND             0x00000008
#define BEM_SCO_CONNECT_IND              0x00000010
#define BEM_LINK_DISCONNECT              0x00000020
#define BEM_LINK_CONNECT_CNF             0x00000040
#define BEM_AUTH_PAYLOAD_TIMEOUT         0x00000080
#define BEM_MODE_CHANGE                  0x00000100
#define BEM_ACCESSIBLE_CHANGE            0x00000200
#define BEM_AUTHENTICATED                0x00000400
#define BEM_ENCRYPTION_CHANGE            0x00000800
#define BEM_SECURITY_CHANGE              0x00001000
#define BEM_ROLE_CHANGE                  0x00002000
#define BEM_SCO_DISCONNECT               0x00004000
#define BEM_SCO_CONNECT_CNF              0x00008000
#define BEM_SIMPLE_PAIRING_COMPLETE      0x00010000
#define BEM_REMOTE_FEATURES              0x00020000     /* Same event mask for extended features */
#define BEM_REM_HOST_FEATURES            0x00040000
#define BEM_LINK_SUPERV_TIMEOUT_CHANGED  0x00080000
#define BEM_SET_SNIFF_SUBR_PARMS         0x00100000
#define BEM_SNIFF_SUBRATE_INFO           0x00200000
#define BEM_SET_INQ_MODE                 0x00400000
#define BEM_SET_INQ_RSP_TX_PWR           0x00800000
#define BEM_SET_EXT_INQ_RESP             0x01000000
#define BEM_SET_ERR_DATA_REP             0x02000000
#define BEM_KEY_PRESSED                  0x04000000
#define BEM_CONN_PACKET_TYPE_CHNG        0x08000000
#define BEM_QOS_SETUP_COMPLETE           0x10000000
#define BEM_PSREP_MODE_CHANGE            0x20000000
#define BEM_UPDATE_EIR_REQ               0x40000000
#define BEM_VENDOR_SPECIFIC              0x80000000
#define BEM_ALL_EVENTS                   0xffffffff

/* End of BtEventMask */

/*---------------------------------------------------------------------------
 * BtEventMask2 type
 *
 *     All registered handlers can receive global events. Each handler has
 *     the ability to mask all or some of the events. A handler receives only
 *     events with the mask set. They can be ORed together. The mask requires
 *     two 32-bit variables and this is the second one.
 */
typedef U32 BtEventMask2;

#define BEM2_NO_EVENTS                   0x00000000
#define BEM2_LINK_CONNECT_ABORT          0x00000001
#define BEM2_ALL_EVENTS                  0xffffffff

/* End of BtEventMask2 */


/*---------------------------------------------------------------------------
 * BtIac type
 *     Bluetooth Inquiry Access Codes are used to specify which types of
 *     devices should respond to inquiries. Currently there are only two
 *     defined.
 */
typedef U32 BtIac;

#define BT_IAC_GIAC 0x9E8B33   /* General/Unlimited Inquiry Access Code */
#define BT_IAC_LIAC 0x9E8B00   /* Limited Dedicated Inquiry Access Code */

/* End of BtIac */


/*---------------------------------------------------------------------------
 * BtLinkType type
 *
 *     Bluetooth Link type indicates the link type in the
 *     BtScoConnect link structure for SCO commands and operations.
 */
typedef U8 BtLinkType;

#define BLT_SCO   0x00
#define BLT_ACL   0x01
#define BLT_ESCO  0x02

/* End of BtLinkType */

/*---------------------------------------------------------------------------
 * BtAclPacketType type
 *     Bluetooth ACL Packet types are the different packet types allowed on
 *     an ACL link. They can be ORed together.
 */
typedef U16 BtAclPacketType;

#define BAPT_NO_2_DH1  0x0002
#define BAPT_NO_3_DH1  0x0004
#define BAPT_DM1       0x0008
#define BAPT_DH1       0x0010
#define BAPT_NO_2_DH3  0x0100
#define BAPT_NO_3_DH3  0x0200
#define BAPT_DM3       0x0400
#define BAPT_DH3       0x0800
#define BAPT_NO_2_DH5  0x1000
#define BAPT_NO_3_DH5  0x2000
#define BAPT_DM5       0x4000
#define BAPT_DH5       0x8000

/* The set of packet types for a reliable link that follows the Serial port
 * profile.
 */
#define BAPT_STANDARD (BAPT_DM1 | BAPT_DH1 | BAPT_DM3 | BAPT_DH3 |\
                       BAPT_DM5 | BAPT_DH5)

/* End of BtAclPacketType */


/*---------------------------------------------------------------------------
 * BtQosServiceType type
 */
typedef U8 BtQosServiceType;

#define BQST_NO_TRAFFIC  0x00
#define BQST_BEST_EFFORT 0x01
#define BQST_GUARANTEED  0x02

/* End of BtQosServiceType */


/*---------------------------------------------------------------------------
 * BtLinkPolicy type
 *
 *     Bluetooth Link Policy specifies which Link modes the Link Manager
 *     can use for a given link. Multiple policies can be enabled by
 *     performing a bit wise OR operation.
 */
typedef U16 BtLinkPolicy;

#define BLP_DISABLE_ALL         0x0000
#define BLP_MASTER_SLAVE_SWITCH 0x0001
#define BLP_HOLD_MODE           0x0002
#define BLP_SNIFF_MODE          0x0004
#define BLP_PARK_MODE           0x0008
#define BLP_SCATTER_MODE        0x0010

/* Mask must be updated if new policy values are added */
#define BLP_MASK                0xfff0  /* Disables ScatterNet bit */
#define BLP_SCATTER_MASK        0xffe0  /* Enables ScatterNet bit */

/* End of BtLinkPolicy */


/*---------------------------------------------------------------------------
 * BtAccessibleMode type
 *
 *     Bluetooth Accessibility mode includes Discoverable and connectable
 *     modes.
 */
typedef U8 BtAccessibleMode;

#define BAM_NOT_ACCESSIBLE     0x00 /* Non-discoverable or connectable */
#define BAM_GENERAL_ACCESSIBLE 0x03 /* General discoverable and connectable */
#define BAM_LIMITED_ACCESSIBLE 0x13 /* Limited discoverable and connectable */
#define BAM_CONNECTABLE_ONLY   0x02 /* Connectable but not discoverable */
#define BAM_DISCOVERABLE_ONLY  0x01 /* Discoverable but not connectable */

/* End of BtAccessibleMode */


/*---------------------------------------------------------------------------
 * BtStackState type
 *
 *     State of the Bluetooth protocol stack and radio.
 */
typedef U8 BtStackState;

/* The stack has completed initialization of the radio hardware. */
#define BTSS_NOT_INITIALIZED 0

/* The stack is initialized. */
#define BTSS_INITIALIZED     1

/* The stack has encountered an error while initializing the radio hardware. */
#define BTSS_INITIALIZE_ERR  2

/* The stack is deinitializing. */
#define BTSS_DEINITIALIZE    3

/* End of BtStackState */

/*---------------------------------------------------------------------------
 * BtRadioFeature type
 *
 *     Codes for radio features as defined in LMP.
 */
typedef U8 BtRadioFeature;

/* Group: Feature Mask Page 0 (Local/Remote Supported Features) */

#define BRF_3_SLOT_PACKETS               0
#define BRF_5_SLOT_PACKETS               1
#define BRF_ENCRYPTION                   2
#define BRF_SLOT_OFFSET                  3
#define BRF_TIMING_ACCURACY              4
#define BRF_ROLE_SWITCH                  5
#define BRF_HOLD_MODE                    6
#define BRF_SNIFF_MODE                   7

#define BRF_PARK_STATE                   8
#define BRF_POWER_CONTROL_REQ            9
#define BRF_CQDDR                        10
#define BRF_SCO_LINK                     11
#define BRF_HV2_PACKETS                  12
#define BRF_HV3_PACKETS                  13
#define BRF_ULAW_SCO_DATA                14
#define BRF_ALAW_SCO_DATA                15

#define BRF_CVSD_SCO_DATA                16
#define BRF_PAGING_PARAM_NEG             17
#define BRF_POWER_CONTROL                18
#define BRF_TRANSPARENT_SCO_DATA         19
#define BRF_FLOW_CONTROL_LAG_1           20
#define BRF_FLOW_CONTROL_LAG_2           21
#define BRF_FLOW_CONTROL_LAG_3           22
#define BRF_BROADCAST_ENCRYPTION         23

#define BRF_EDR_2MBS                     25
#define BRF_EDR_3MBS                     26
#define BRF_ENHANCED_INQUIRY_SCAN        27
#define BRF_INTERLACED_INQUIRY_SCAN      28
#define BRF_INTERLACED_PAGE_SCAN         29
#define BRF_RSSI_WITH_INQUIRY_RESULTS    30
#define BRF_ESCO_EV3_PACKETS             31

#define BRF_ESCO_EV4_PACKETS             32
#define BRF_ESCO_EV5_PACKETS             33
#define BRF_AFH_CAPABLE_SLAVE            35
#define BRF_AFH_CLASSIFICATION_SLAVE     36
#define BRF_BR_EDR_NOT_SUPPORTED         37
#define BRF_LE_SUPPORTED                 38
#define BRF_EDR_3_SLOTS                  39

#define BRF_EDR_5_SLOTS                  40
#define BRF_SNIFF_SUBRATING              41
#define BRF_PAUSE_ENCRYPTION             42
#define BRF_AFH_CAPABLE_MASTER           43
#define BRF_AFH_CLASSIFICATION_MASTER    44
#define BRF_EDR_ESCO_2MBS                45
#define BRF_EDR_ESCO_3MBS                46
#define BRF_EDR_ESCO_3_SLOTS             47

#define BRF_EXTENDED_INQUIRY             48
#define BRF_SIMUL_LE_BREDR_SAME_DEV      49

#define BRF_SECURE_SIMPLE_PAIRING        51
#define BRF_ENCAPSULATED_PDU             52
#define BRF_ERRONEOUS_DATA_REPORTING     53
#define BRF_NON_FLUSHABLE_PBF            54

#define BRF_LINK_SUPERV_TIMEOUT_EVENT    56
#define BRF_INQ_RSP_TX_POWER_LVL         57
#define BRF_EXTENDED_FEATURES            63

/* Group: Extended Feature Mask Page 1 (Local/Remote Supported Features) */

#define BRF_HOST_SECURE_SIMPLE_PAIRING   64

#define BRF_HOST_LE_SUPPORTED            65
#define BRF_HOST_SIMUL_LE_BREDR_SAME_DEV 66
#define BRF_HOST_SECURE_CONN_SUPPORT     67

/* Group: Extended Feature Mask Page 2 (Local/Remote Supported Features) */

#define BRF_SECURE_CONNECTIONS           136
#define BRF_PING                         137

/* End of BtRadioFeature */

/* Forward declaration of the callback parameters */
typedef struct _BtEvent BtEvent;

/* Forward declaration of the SCO connect structures */
typedef struct _BtScoConnect BtScoConnect;
typedef struct _BtScoTxParms BtScoTxParms;
typedef struct _BtScoRxParms BtScoRxParms;

/*---------------------------------------------------------------------------
 * BtCallBack type
 *
 *     A function of this type is called to indicate general Bluetooth and
 *     ME events.
 */
typedef void (*BtCallBack)(const BtEvent*);

/* End of BtCallBack */


/*---------------------------------------------------------------------------
 * BtProtocolId type
 *
 *     The protocol ID is a pointer to the protocol's security callback
 *     function. The user of the protocol ID should not use the protocol ID
 *     as a callback function since in the future it may be something else.
 *     The callback is a unique way to identify a protocol layer.
 */
typedef BtCallBack BtProtocolId;

/* End of BtProtocolId */


/*---------------------------------------------------------------------------
 * BtClassOfDevice type
 *
 *     Bit pattern representing the class of device along with the
 *     supported services. There can be more than one supported service.
 *     Service classes can be ORed together. The Device Class is composed
 *     of a major device class plus a minor device class. ORing together
 *     each service class plus one major device class plus one minor device
 *     class creates the class of device value. The minor device class is
 *     interpreted in the context of the major device class.
 */

typedef U32 BtClassOfDevice;

/* Group: Major Service Classes. Can be ORed together */
#define COD_LIMITED_DISCOVERABLE_MODE 0x00002000
#define COD_POSITIONING               0x00010000
#define COD_NETWORKING                0x00020000
#define COD_RENDERING                 0x00040000
#define COD_CAPTURING                 0x00080000
#define COD_OBJECT_TRANSFER           0x00100000
#define COD_AUDIO                     0x00200000
#define COD_TELEPHONY                 0x00400000
#define COD_INFORMATION               0x00800000

/* Group: Major Device Classes (Select one) */
#define COD_MAJOR_MISCELLANEOUS       0x00000000
#define COD_MAJOR_COMPUTER            0x00000100
#define COD_MAJOR_PHONE               0x00000200
#define COD_MAJOR_LAN_ACCESS_POINT    0x00000300
#define COD_MAJOR_AUDIO               0x00000400
#define COD_MAJOR_PERIPHERAL          0x00000500
#define COD_MAJOR_IMAGING             0x00000600
#define COD_MAJOR_WEARABLE            0x00000700
#define COD_MAJOR_TOY                 0x00000800
#define COD_MAJOR_HEALTH              0x00000900
#define COD_MAJOR_UNCLASSIFIED        0x00001F00

/* Group: Minor Device Class - Computer Major class */
#define COD_MINOR_COMP_UNCLASSIFIED   0x00000000
#define COD_MINOR_COMP_DESKTOP        0x00000004
#define COD_MINOR_COMP_SERVER         0x00000008
#define COD_MINOR_COMP_LAPTOP         0x0000000C
#define COD_MINOR_COMP_HANDHELD       0x00000010
#define COD_MINOR_COMP_PALM           0x00000014
#define COD_MINOR_COMP_WEARABLE       0x00000018

/* Group: Minor Device Class - Phone Major class */
#define COD_MINOR_PHONE_UNCLASSIFIED  0x00000000
#define COD_MINOR_PHONE_CELLULAR      0x00000004
#define COD_MINOR_PHONE_CORDLESS      0x00000008
#define COD_MINOR_PHONE_SMART         0x0000000C
#define COD_MINOR_PHONE_MODEM         0x00000010
#define COD_MINOR_PHONE_ISDN          0x00000014

/* Group: Minor Device Class - LAN Access Point Major class */
#define COD_MINOR_LAN_0               0x00000000     /* fully available */
#define COD_MINOR_LAN_17              0x00000020     /* 1-17% utilized */
#define COD_MINOR_LAN_33              0x00000040     /* 17-33% utilized */
#define COD_MINOR_LAN_50              0x00000060     /* 33-50% utilized */
#define COD_MINOR_LAN_67              0x00000080     /* 50-67% utilized */
#define COD_MINOR_LAN_83              0x000000A0     /* 67-83% utilized */
#define COD_MINOR_LAN_99              0x000000C0     /* 83-99% utilized */
#define COD_MINOR_LAN_NO_SERVICE      0x000000E0     /* 100% utilized */

/* Group: Minor Device Class - Audio Major class */
#define COD_MINOR_AUDIO_UNCLASSIFIED  0x00000000
#define COD_MINOR_AUDIO_HEADSET       0x00000004
#define COD_MINOR_AUDIO_HANDSFREE     0x00000008
#define COD_MINOR_AUDIO_MICROPHONE    0x00000010
#define COD_MINOR_AUDIO_LOUDSPEAKER   0x00000014
#define COD_MINOR_AUDIO_HEADPHONES    0x00000018
#define COD_MINOR_AUDIO_PORTABLEAUDIO 0x0000001C
#define COD_MINOR_AUDIO_CARAUDIO      0x00000020
#define COD_MINOR_AUDIO_SETTOPBOX     0x00000024
#define COD_MINOR_AUDIO_HIFIAUDIO     0x00000028
#define COD_MINOR_AUDIO_VCR           0x0000002C
#define COD_MINOR_AUDIO_VIDEOCAMERA   0x00000030
#define COD_MINOR_AUDIO_CAMCORDER     0x00000034
#define COD_MINOR_AUDIO_VIDEOMONITOR  0x00000038
#define COD_MINOR_AUDIO_VIDEOSPEAKER  0x0000003C
#define COD_MINOR_AUDIO_CONFERENCING  0x00000040
#define COD_MINOR_AUDIO_GAMING        0x00000048

/* Group: Minor Device Class - Peripheral Major class */
#define COD_MINOR_PERIPH_KEYBOARD     0x00000040
#define COD_MINOR_PERIPH_POINTING     0x00000080
#define COD_MINOR_PERIPH_COMBOKEY     0x000000C0
/* Group: Minor Device Class - ORed with Peripheral Minor Device class */
#define COD_MINOR_PERIPH_UNCLASSIFIED 0x00000000
#define COD_MINOR_PERIPH_JOYSTICK     0x00000004
#define COD_MINOR_PERIPH_GAMEPAD      0x00000008
#define COD_MINOR_PERIPH_REMOTECTRL   0x0000000C
#define COD_MINOR_PERIPH_SENSING      0x00000010
#define COD_MINOR_PERIPH_DIGITIZER    0x00000014
#define COD_MINOR_PERIPH_CARD_RDR     0x00000018

/* Group: Minor Device Class - Imaging Major class */
#define COD_MINOR_IMAGE_UNCLASSIFIED  0x00000000
#define COD_MINOR_IMAGE_DISPLAY       0x00000010
#define COD_MINOR_IMAGE_CAMERA        0x00000020
#define COD_MINOR_IMAGE_SCANNER       0x00000040
#define COD_MINOR_IMAGE_PRINTER       0x00000080

/* Group: Minor Device Class - Health Major class */
#define COD_MINOR_HEALTH_UNDEFINED    0x00000000
#define COD_MINOR_HEALTH_PRESSURE     0x00000004
#define COD_MINOR_HEALTH_THERMO       0x00000008
#define COD_MINOR_HEALTH_SCALE        0x0000000C
#define COD_MINOR_HEALTH_GLUCOSE      0x00000010
#define COD_MINOR_HEALTH_PULSE        0x00000014
#define COD_MINOR_HEALTH_HEARTRATE    0x00000018
#define COD_MINOR_HEALTH_DISPLAY      0x0000001C
#define COD_MINOR_HEALTH_COUNTER      0x00000020

/* Group: Masks used to isolate the class of device components */
#define COD_SERVICE_MASK              0x00ffC000     /* Less LIAC bit */
#define COD_MAJOR_MASK                0x00001F00
#define COD_MINOR_MASK                0x000000FC
#define COD_LIMITED_DISC_MASK         0x00002000     /* LIAC bit */

/* End of BtClassOfDevice */

/*---------------------------------------------------------------------------
 * BtLinkKeyType type
 */

typedef U8 BtLinkKeyType;

#define COMBINATION_KEY                0x00    /* Combination Key */
#define LOCAL_UNIT_KEY                 0x01    /* Local Unit Key */
#define REMOTE_UNIT_KEY                0x02    /* Remote Unit Key */
#define DEBUG_COMBINATION_KEY          0x03    /* Debug Combination Key (generated with P-192 or P-256) */
#define UNAUTH_COMBINATION_KEY         0x04    /* Unauthenticated Combination Key generated from P-192 */
#define AUTH_COMBINATION_KEY           0x05    /* Authenticated Combination Key generated from P-192 */
#define CHANGED_COMBINATION_KEY        0x06    /* Changed Combination Key */
#define UNAUTH_COMBINATION_SECURE_KEY  0x07    /* Unathenticated Combination Key generated from P-256 */
#define AUTH_COMBINATION_SECURE_KEY    0x08    /* Authenticated Combination Key generated from P-256 */
#define INVALID_KEY                    0xFF

/* End of BtLinkKeyType */

/*---------------------------------------------------------------------------
 * BtInquiryMode type
 *
 * Describes the format of the inquiry responses that will be received
 * from the controller.
 */
typedef U8 BtInquiryMode;

#define INQ_MODE_NORMAL    0 /* Normal Inquiry Response format           */
#define INQ_MODE_RSSI      1 /* RSSI Inquiry Response format             */
#define INQ_MODE_EXTENDED  2 /* Extended or RSSI Inquiry Response format */

/* End of BtInquiryMode */

/*---------------------------------------------------------------------------
 * BtInqRspTxPower type
 *
 *     Used to hold the value of an Inquiry Repsonse TX Power Level.  The
 *     range of this value is from -70 to + 20 dBm.
 */
typedef S8 BtInqRspTxPower;

#define BT_INVALID_TX_PWR   127

/* End of BtInqRspTxPower */

/*---------------------------------------------------------------------------
 * BtInqTxPower type
 *
 *     Used to hold the value of an Inquiry TX Power Level.  The range of
 *     this value is from -70 to + 20 dBm.
 */
typedef S8 BtInqTxPower;

/* End of BtInqTxPower */

/*---------------------------------------------------------------------------
 * BtErrDataReporting type
 *
 * Defines whether erroneous data reporting is enabled in the host
 * controller.
 */
typedef U8 BtErrDataReporting;

#define EDR_DISABLED    0 /* Erroneous Data Reporting disabled */
#define EDR_ENABLED     1 /* Erroneous Data Reporting enabled */

/* End of BtErrDataReporting */

/*---------------------------------------------------------------------------
 * BtScoErrFlags type
 *
 *     When Erroneous Data Reporting is enabled, these flags are used in the
 *     "scoDataInd.errFlags" field of BtEvent, during a BTEVENT_SCO_DATA_IND
 *     event. They indicated any potential errors in the SCO data that was
 *     received.
 */
typedef U8 BtScoErrFlags;

#define BSE_NO_ERROR           0x00
#define BSE_PARTIAL_ERRORS     0x01
#define BSE_NO_DATA            0x10
#define BSE_PARTIAL_DATA       0x11

/* End of BtScoErrFlags */

/*---------------------------------------------------------------------------
 * BtIoCapabilities type
 *
 *     Defines the I/O capabilities of a device.  Simple Pairing and LE
 *     securitiy will generate different events based on the capabilities of 
 *     the device.
 */
typedef U8 BtIoCapabilities;


/* The device is only capable of display     */
#define IO_DISPLAY_ONLY      0 

/* The device can display and accept yes/no input.   */
#define IO_DISPLAY_YESNO     1 

/* The device is a keyboard with no display  */
#define IO_KEYBOARD_ONLY     2 

/* The device is not capable of I/O          */
#define IO_NO_IO             3 

#if NUM_LE_DEVICES > 0
/* The device can display and accepts numerical input (numbers 0 - 9), a
 * confirmation, and at least two buttons that can be mapped to yes and no. 
 * This entry is unique for LE but maps to IO_DISPLAY_YESNO for BR/EDR. It
 * is the best choice for devices such as phones and computers that support
 * both LE and BR/EDR security.
 */
#define IO_KEYBOARD_DISPLAY   4 
#endif

/* End of BtIoCapabilities */

/*---------------------------------------------------------------------------
 * BtOobDataPresent type
 *
 *     Describes whether Out of Band data has been received from a remote
 *     device.
 */
typedef U8 BtOobDataPresent;

#define OOB_DATA_NOT_PRESENT  0 /* No Out of Band Data is present */
#define OOB_DATA_PRESENT      1 /* Out of Band Data is present    */

/* End of BtOobDataPresent */

/*---------------------------------------------------------------------------
 * BtAuthRequirements type
 *
 *     Describes whether Man in the Middle protection is required during
 *     simple pairing. If the remote device does not support simple pairing
 *     then MITM_PROTECT_REQUIRED specifies that authentication and
 *     encryption (BSL_SECURITY_L3_IN | BSL_SECURITY_L3_OUT) will be used
 *     if there is no BtSecurityRecord.
 */
typedef U8 BtAuthRequirements;

#define MITM_PROTECT_NOT_REQUIRED  0x00 /* No Man in the Middle protection  */
#define MITM_PROTECT_REQUIRED      0x01 /* Man in the Middle protection req */

/* End of BtAuthRequirements */

/*---------------------------------------------------------------------------
 * BtAuthorizeRequirements type
 *
 *     Describes whether Authorization is required when a service is accessed
 *     and no security record exists. AUTHORIZATION_REQUIRED specifies that
 *     authorization (BSL_AUTHORIZATION_IN | BSL_AUTHORIZATION_OUT) will be
 *     used if there is no BtSecurityRecord.
 */
typedef U8 BtAuthorizeRequirements;

#define AUTHORIZATION_NOT_REQUIRED  0x00 /* Authorization not required  */
#define AUTHORIZATION_REQUIRED      0x01 /* Authorziation is required   */

/* End of BtAuthorizeRequirements */

/*---------------------------------------------------------------------------
 * BtBondingMode type
 *
 *     Describes the bonding mode of the device.  Dedicated bonding implies
 *     that a device will connect for the express purpose of bonding, and
 *     then will disconnect.  General bonding implies that a device will bond
 *     while connecting to a particular service.
 */
typedef U8 BtBondingMode;

/* Bonding is not required. If a remote device attempts to bond, authentication
 * will be allowed to proceed, but the resulting link key may not be stored.
 */
#define BONDING_NOT_REQUIRED 0x00

/* Dedicated bonding mode enabled. Allows bonding with specific remote devices.
 */
#define DEDICATED_BONDING    0x02

/* General bonding mode enabled. Allows bonding with remote devices. */
#define GENERAL_BONDING      0x04

/* Bonding is always denied. If a remote device attempts to bond, its attempt
 * will be rejected. Non-bonding authentication attempts are allowed to
 * proceed. This setting is specific to Security Simple Pairing. Legacy Pairing
 * does not send bonding information over the air so there is nothing to reject.
 * In the case of legacy pairing the pairing will proceed and it is up to the
 * upper layer to determine if they want to bond (save the link key) or not.
 */
#define BONDING_NOT_ALLOWED  0x10

/* End of BtBondingMode */

/*---------------------------------------------------------------------------
 * BtEirDataType type
 *
 *     Defines the data types in an extended inquiry response.
 */
typedef U8 BtEirDataType;

#define EIR_FLAGS                0x01
#define EIR_SRV_CLASS_16_PART    0x02
#define EIR_SRV_CLASS_16_WHOLE   0x03
#define EIR_SRV_CLASS_32_PART    0x04
#define EIR_SRV_CLASS_32_WHOLE   0x05
#define EIR_SRV_CLASS_128_PART   0x06
#define EIR_SRV_CLASS_128_WHOLE  0x07
#define EIR_REM_NAME_PART        0x08
#define EIR_REM_NAME_WHOLE       0x09
#define EIR_TX_POWER             0x0A
#define EIR_MAN_SPECIFIC         0xFF

/* End of BtEirDataType */

/*---------------------------------------------------------------------------
 * BtEirAdFlagsType type
 *
 *     Defines the EIR/AD flags in an extended inquiry response.
 */
typedef U8 BtEirAdFlagsType;

#define EIR_AD_LE_LIMITED_DISCOV    0x01
#define EIR_AD_LE_GENERAL_DISCOV    0x02
#define EIR_AD_BR_EDR_NOT_SUPPORTED 0x04
#define EIR_AD_BR_EDR_LE_CONTROLLER 0x08
#define EIR_AD_BR_EDR_LE_HOST       0x10

/* End of BtEirAdFlagsType */

/*---------------------------------------------------------------------------
 * BtKeyPress type
 *
 *     Defines the keypress events during Secure Simple Pairing
 */
typedef U8 BtKeyPressParm;

#define PASSKEY_STARTED         0
#define PASSKEY_DIGIT_ENTERED   1
#define PASSKEY_DIGIT_ERASED    2
#define PASSKEY_CLEARED         3
#define PASSKEY_COMPLETED       4

/* End of BtKeyPressParm */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * BtPageScanInfo structure
 */
typedef struct _BtPageScanInfo
{
    U8   psRepMode;     /* Page Scan Repetition Mode */
    U8   psMode;        /* Always set to 0 */
    U16  clockOffset;   /* Current clock offset */
} BtPageScanInfo;


/*---------------------------------------------------------------------------
 * BtSniffInfo structure
 *
 *     Identifies the sniff requirements during a ME_StartSniff request.
 */
typedef struct _BtSniffInfo
{
    /* Maximum acceptable interval between each consecutive sniff period.
     * May be any even number between 0x0002 and 0xFFFE, but the mandatory
     * sniff interval range for controllers is between 0x0006 and 0x0540.
     * The value is expressed in 0.625 ms increments (0x0006 = 3.75 ms).
     *
     * The actual interval selected by the radio will be returned in
     * a BTEVENT_MODE_CHANGE event.
     */
    U16 maxInterval;

    /* Minimum acceptable interval between each consecutive sniff period.
     * Must be an even number between 0x0002 and 0xFFFE, and be less than
     * "maxInterval". Like maxInterval this value is expressed in
     * 0.625 ms increments.
     */
    U16 minInterval;

    /* The number of master-to-slave transmission slots during which
     * a device should listen for traffic (sniff attempt).
     * Expressed in 0.625 ms increments. May be between 0x0001 and 0x7FFF.
     */
    U16 attempt;

    /* The amount of time before a sniff timeout occurs. Expressed in
     * 1.25 ms increments. May be between 0x0000 and 0x7FFF, but the mandatory
     * range for controllers is 0x0000 to 0x0028.
     */
    U16 timeout;

} BtSniffInfo;


/*---------------------------------------------------------------------------
 * BtSniffSubrateParms structure
 *
 *     Identifies the sniff subrating requirements when enabling Sniff
 *     Subrating (ME_EnableSniffSubrating).
 */
typedef struct _BtSniffSubrateParms
{

    /* The Maximum Latency parameter used to calculate the
     * maximum sniff subrate that the remote device may use.  May be any
     * number between 0x0000 and 0xFFFE.  The value is expressed in 0.625 ms
     * increments.
     */
    U16 maxLatency;

    /* Minimum base sniff subrate timeout that the remote device may use.
     * May be any number between 0x0000 and 0xFFFE.  The value is expressed in
     * 0.625 ms increments.
     */
    U16 minRemoteTimeout;

    /* Minimum base sniff subrate timeout that the local device may use.
     * May be any number between 0x0000 and 0xFFFE.  The value is expressed in
     * 0.625 ms increments.
     */
    U16 minLocalTimeout;
} BtSniffSubrateParms;


/*---------------------------------------------------------------------------
 * BtAccessModeInfo structure
 */
typedef struct _BtAccessModeInfo
{
    U16 inqInterval;    /* Inquiry scan interval */
    U16 inqWindow;      /* Inquiry scan Window */
    U16 pageInterval;   /* Page scan interval */
    U16 pageWindow;     /* Page scan window */
} BtAccessModeInfo;


/*---------------------------------------------------------------------------
 * BtInquiryResult structure
 *
 *     Represents information about a single device found during an
 *     Inquiry process.
 */
typedef struct _BtInquiryResult
{
    BtDevAddr       devAddr;
    BtPageScanInfo  psi;              /* Page scan info used for connecting */
    U8              psPeriodMode;
    BtClassOfDevice classOfDevice;

    /* RSSI in dBm (-127 to +20). Only valid when controller reports RSSI with
     * in inquiry results (also see ME_SetInquiryMode). Otherwise it will be
     * set to BT_INVALID_RSSI.
     */
    S8              rssi;

    /* Extended Inquiry response.  Only valid when controller reports an
     * extended inquiry (also see ME_SetInquiryMode).  Otherwise it will be
     * set to all 0's.
     */
    U8              extInqResp[240];

    /* Describes the format of the current inquiry result */
    BtInquiryMode   inqMode;

} BtInquiryResult;


/*---------------------------------------------------------------------------
 * BtChannelClass structure
 *
 *     Indicates channel classification for AFH with 79 1-bit fields in
 *     a 10-byte array. Each channel may be marked bad or unknown. The nth
 *     field (0 - 78) indicates the value for channel n. Bit 79 is unused.
 */
typedef struct _BtChannelClass
{
    U8  map[10];
} BtChannelClass;

/*---------------------------------------------------------------------------
 * BtHandler structure
 */
typedef struct _BtHandler
{
    ListEntry    node;       /* For internal stack use only. */
    BtCallBack   callback;   /* Pointer to callback function */
    BtEventMask  eMask1;     /* For internal stack use only. */
    BtEventMask2 eMask2;     /* For internal stack use only. */

} BtHandler;


/* Internal types required for BtRemoteDevice structure */
typedef U8 BtAuthState;
typedef U8 BtEncryptState;
typedef U8 BtAuthorizeState;
typedef U8 BtSecAccessState;

#if NUM_LE_DEVICES > 0
#include "mele.h"
#endif /* NUM_LE_DEVICES > 0 */

/*---------------------------------------------------------------------------
 * BtAmpKeyInfo structure
 *
 *     The BtAmpKeyInfo structure type is used to store AMP Link Keys.
 */
typedef struct _BtAmpKeyInfo {
    BOOL             usable;  /* TRUE if the length and value are
                               * meaningful or valid, otherwise FALSE
                               */
    U8               length;  /* The number of bytes that are
                               * meaningful in the value array.
                               */
    U8               value[BT_MAX_AMP_KEY_LENGTH];
} BtAmpKeyInfo;

/*---------------------------------------------------------------------------
 * BtGAmpType structure
 *
 *     The BtGAmpType structure type is used to store Generic AMP Link Key
 *     information.
 */
typedef struct _BtGAmpType
{
    BtAmpKeyInfo     pending; /* temporary storage until it is known whether
                               * the connection is successful or not.
                               */
    BOOL             usable;  /* TRUE if the length and value are
                               * meaningful or valid, otherwise FALSE
                               */
    U8               length;  /* The number of bytes that are
                               * meaningful in the value array.
                               */
    U8               value[BT_MAX_AMP_KEY_LENGTH];
} BtGAmpType;

/*---------------------------------------------------------------------------
 * BtDAmpLinkKey structure
 *
 *     The BtDAmpLinkKey structure type is used to store Dedicated AMP Link
 *     Key information.
 */
typedef struct _BtDAmpLinkKey
{
    BtControllerType crlType;
    BOOL             usable;  /* TRUE if the length and value are
                               * meaningful or valid, otherwise FALSE
                               */
    U8               length;  /* The number of bytes that are
                               * meaningful in the value array.
                               */
    U8               value[BT_MAX_AMP_KEY_LENGTH];
} BtDAmpLinkKey;

/*---------------------------------------------------------------------------
 * BtVersionInfo structure
 *
 *     The BtVersionInfo structure type is used to store the return values from
 *     the HCC_READ_LOCAL_VERSION (HCI Read_Local_Version) command.
 */
typedef struct _BtVersionInfo
{
    U8 version;                 /* Version of the controller. Derived
                                 * from hciVersion and lmpVersion */
    U8 hciVersion;              /* See Bluetooth Assigned Numbers */
    U16 hciRevision;            /* Revision of Current HCI in BR/EDR
                                 * controller */
    U8 lmpVersion;            /* Version of the Current LMP in the
                                 * Controller. See Bluetooth Assigned
                                 * Numbers. */
    U16 mfgName;                /* Manufacturer Name of the BR/EDR Controller.
                                 * See Bluetooth Assigned Numbers. */
    U16 lmpSubversion;          /* Subversion of the Current LMP in the
                                 * Controller. This value is implementation
                                 * dependant. */
} BtVersionInfo;

/*---------------------------------------------------------------------------
 * BrEdrRemoteDevice structure
 *
 *     Represents a remote device. This structure is used to manage the ACL
 *     and SCO links to a remote device. Declaration of BtRemoteDevice is
 *     in bttypes2.h
 */
typedef struct _BrEdrRemoteDevice
{
    /* Inherit from BtRemoteDevice */
    BtRemoteDevice   rd;

    BtOperation      policyOp;      /* Used internally by the stack */
    BtOperation      cancelOp;      /* Used internally by the stack */
    BtOperation      roleOp;        /* Used internally by the stack */
    BtOperation      packetOp;      /* Used internally by the stack */
    BtOperation      remFeaturesOp; /* Used internally by the stack */
    BtOperation      sniffSubrOp;   /* Used internally by the stack */

    /* Group: Access functions are provided for accessing these values. They
     * should not be accessed directly by an application.
     */
    BtClassOfDevice  cod;    /* COD of the device */
    BOOL             initiator;
    void            *secRec; /* Current Security Record being processed */

    /* Simple Pairing */
    BtIoCapabilities   ioCap;               /* IO Capabilities of the remote device */
    BtAuthRequirements remAuthRequirements; /* Authentication and bonding requirements of remote device */
    BOOL               ioCapFlags;

    /* Group: ME uses the following fields internally. */
    ListEntry        scoList;           /* Head of list of SCO connections */
    BtHandler       *scoHandler;        /* Handler for SCO event (1 per remDev) */
    ListEntry        secTokens;         /* List of security tokens */
    BtHandler       *authHandler;       /* Handler for authentication */
    BtHandler       *encryptHandler;    /* Handler for encryption */
    BtAclPacketType  aclPacketTypes;    /* Current ACL packet types permitted */
    BtAclPacketType  tmpAclPacketTypes; /* ACL packet types attempting to be set */
    U8               remoteFeatures[8]; /* Remote Features */
    U8               remoteExtFeatures[2][8];   /* Extended Remote Features (page 1-2) */
    U8               remoteFeaturesRetries; /* Number of retries for Remote (and extended) Features */
    U8               extFeaturePage;    
    BOOL             linkUp;
    BOOL             haveFeatures;
    BOOL             haveExtFeatures;

    /* Group: The following are used internally for security */
    BOOL               newLinkKey;          /* TRUE means attempting to get a new link key */
    BOOL               pendingAuthOp;       /* TRUE means attempting an authentication operation */
    BOOL               remInitAuth;         /* TRUE means the remote initiated authentication */
    BtSecAccessState   secAccessState;
    BtAuthState        authState;
    BtAuthorizeState   authorizeState;
    BtEncryptState     encryptState;
    U8                 pairingType;         /* Uses the BtPairingType type */
    U8                 ioType;              /* Track the kind of I/O transaction */
    U32                numeric;             /* Numeric value to display */
    BtOobDataPresent   oobDataPresent;      /* Out of band data present on the remote device */
    BtAuthRequirements curAuthRequirements; /* Current auth requirements of local device */

    /* BR/EDR link key and type */
    BtLinkKeyType      keyType;             /* Type of link key in use */
    U8                 pinLen;              /* length of pin used in generating a key */
    U8                 linkKey[BT_BR_EDR_KEY_LENGTH];

    BtLinkKeyType      pendingKeyType;      /* Type of link key to be used  */
    U8                 pendingPinLen;       /* length of pin used in generating the key to be used */
    U8                 pendingLinkKey[BT_BR_EDR_KEY_LENGTH];

#if NUM_AMPS > 0
    BtGAmpType         gAmpLK;
    BtDAmpLinkKey      dAmpLK[BT_NUMBER_AMP_TYPES];
#endif

    /* Group: The following are parameters to me operations */
    union {
        /* Create connection operation */
        struct {
            BtPageScanInfo   psi;
            U8               allowRoleChange;
        } con;

        /* Park & Hold mode commands */
        struct {
            U16          max;
            U16          min;
        } mode;

        BtSniffInfo         sniff;             /* Sniff command */
        BtSniffSubrateParms sniffSubrateParms; /* Sniff subrating command */
        BtConnectionRole    acceptRole;        /* Connection accept */

        U8               rejectReason;         /* Connection reject */
        BtLinkPolicy     policy;               /* Write link policy */
    } parms;

    /* Group: Physical Link uses the following fields internally. */
    U32             gRateOut;     /* Total guaranteed rate out */
    U32             gRateIn;      /* Total guaranteed rate in */

    /* Table to hold pointer to the fixed channel clients. Call FIXMGR_FindClientIndex() to
     * get the index of a specific fixed channel client.
     */
    void           *fixedBrClients[L2CAP_NUM_BR_FIXED_CHANNELS];

    /* Group: The SDP client uses the following fields internally. */
    U16             sdpClientChannel;       /* L2CAP channel for SDP client */
    ListEntry       queryList;              /* List of queries */
    BtPacket        sdpClientPacket;        /* Packet for sending queries */
    BOOL            sdpClientPacketInUse;   /* Packet for sending queries in use flag */
    U16             transId;                /* Current transaction ID */
    U8              sdpClientState;         /* State of the client */
    U8              sdpKeepAlive;           /* Keep-alive count */
    struct _SdpQueryToken *curToken;        /* The token currently being processed. */

    /* Group: The SDP server uses the following field internally. */
    struct _SdpServerInfo *sdpServInfo;

    /* TRUE means that bonding occurred (i.e. the link key was saved) */
    BOOL            bonded;

#if NUM_LE_DEVICES > 0
    /* List of SDP tokens used for GATT service discovery over BR/EDR links */
    ListEntry       gattSdpTokens;

    /* Context for an ATT. */
    void           *attCtx;

    /* TRUE means that after authentication is complete encryption should be
     * enabled and the LE key generated and LE keys distributed
     */
    BOOL            genLEKey;

    /* TRUE means that a pairing operation has started so generating an LE key should
     * be attempted if allowed
     */
    BOOL            pairingStarted;

    /* Security token (BtSecurityToken) that is waiting for LE security to complete */
    void           *brSecToken;

#endif /* NUM_LE_DEVICES */

} BrEdrRemoteDevice;

#if NUM_LE_DEVICES > 0
/*---------------------------------------------------------------------------
 * BtDevLERecord structure
 *
 *     Structures of type BtDevLERecord represent Bluetooth Device
 *     Database record Low Energy fields as seen by the Security manager.
 */
typedef struct _BtDevLERecord {
    /* Group: Fields describing the LE Long-Term Key */
    struct {
        U8               keySize;    /* LTK key size (7-16 bytes) */
        U8               key[16];    /* LTK value (up to 128-bit) in Little Endian format */
        U16              ediv;       /* Encryption Diversifier identifying LTK (16-bit) */
        U8               rand[8];    /* Random number identifying LTK (64-bit) in Little Endian format */
    } ltk;

    /* Group: Field describing the LE Connection Signature Resolving Key */
    struct {
        U8               csrk[16];       /* CSRK value (128-bit) in Little Endian format */
        U32              signedCounter;  /* Last received signed counter value */
        BOOL             signedCounterValid; /* Value in signedCounter is valid */
    } signing;

    /* Group: Field describing the LE Identity Resolving Key */
    struct {
        U8               irk[16];    /* IRK value (128-bit) in Little Endian format */
        BtDevAddr        devAddr;
    } privacy;

    /* Describes the LE security keys (LTK, CSRK, and IRK) stored in this record */
    U8                   storedKeys; /* LeSmpKeyDistributionFormat type */
} BtDevLERecord;
#endif

/*---------------------------------------------------------------------------
 * BtDevHeadRecord structure
 *
 *     Structures of type BtDevHeadRecord represent head Bluetooth Device
 *     Database records as seen by the Security manager.
 */
typedef struct _BtDevHeadRecord
{
    BOOL            authorized;

    /* BR/EDR link key information */
    U8              linkKey[BT_BR_EDR_KEY_LENGTH];
    BtLinkKeyType   keyType;

    /* Length of pin used to generate key for pin generated keys */
    U8              pinLen;

    /* Page-scan repetition mode to be used for this device */
    U8              psRepMode;

#if NUM_AMPS > 0
    /* TRUE if the gAmpLK field is applicable to this device,
     * or FALSE otherwise.  If TRUE, the database shall also
     * contain one or more associated AMP records.
     */
    BOOL            hasAmp;

    /* This is the generic AMP Link Key */
    BtAmpKeyInfo    gAmpLK;
#endif /* NUM_AMPS > 0 */

#if NUM_LE_DEVICES > 0
    /* LE specific fields */
    struct {
        /* LE link key type used to distribute the stored keys */
        MeLeKeyType     keyType;  

        /* TRUE indicates secure connections was used to generate the LTK */
        BOOL            secureCon;

        /* TRUE indicates support for central address resolution */
        BOOL            supportAddrRes; 

#if LL_PRIVACY == XA_ENABLED
       /* Static address put in the controller's address resolution table to 
         * uniquely identify the remote device when the identity address sent
         * during pairing is 0.
         */
        BtDevAddr       sAddr;
#endif

        BtDevLERecord   local;      /* Security keys for the local device */
        BtDevLERecord   remote;     /* Security keys for the remote device */
    } le;
#endif /* NUM_LE_DEVICES > 0 */
} BtDevHeadRecord;

/*---------------------------------------------------------------------------
 * BtDeviceRecord structure
 *
 *     Structures of type BtDeviceRecord represent a Bluetooth Device
 *     Database record as seen by the Security manager.
 */
typedef struct _BtDeviceRecord
{
    /* BR/EDR/LE address for this record. Multiple records may
     * exist for the same address, but only one will have the
     * BCT_BR_EDR controller type
     */
    BtDevAddr        bdAddr;

    /* Type of controller described by this device record */
    BtControllerType crlType;

    /* Information specific to the controller type */
    union {
        /* Valid if crlType is BCT_BR_EDR */
        BtDevHeadRecord head;

#if NUM_AMPS > 0
        /* Valid if crlType corresponds to an AMP type */
        BtAmpKeyInfo    dAmpLK;
#endif
    } u;
} BtDeviceRecord;


/*---------------------------------------------------------------------------
 * BtDeviceLinkKey structure
 *
 *     Represents a Bluetooth Device link key returned from an HCI
 *     ReadStoredLinkKey command.
 */
typedef struct _BtDeviceLinkKey
{
    BD_ADDR     bdAddr;           /* Device Address */
    U8          linkKey[16];
} BtDeviceLinkKey;


/*---------------------------------------------------------------------------
 * MeRemoteNameRequest structure
 *
 *     Used in an MeCommandToken when handling a "Remote Name Request"
 *     operation.
 */
typedef struct _MeRemoteNameRequest
{
    /* Common for both input and output */
    BD_ADDR   bdAddr;

    /* Input/Output parameters for the operation. Either the "in"
     * or "out" parameters are valid, depending on when this structure
     * is accessed.
     */
    union {

        /* Fields to fill before starting the operation. */
        struct {
            BtPageScanInfo   psi;  /* Page scan information */
        } in;

        /* Fields to read when the operation is complete. */
        struct {

            /* Length of string stored in "name". */
            U8  len;

            /* Remote device name in UTF-8 format */
            U8* name;
        } out;

    } io;

} MeRemoteNameRequest;


/*---------------------------------------------------------------------------
 * MeGeneralHciCommand structure
 *
 *     Used in an MeCommandToken when handling general and user defined
 *     HCI commands.
 */
typedef struct _MeGeneralHciCommand
{
    /* Fields to be filled before starting the operation */
    struct {

        /* The hciCommand is composed of the OGF and the OCF. The OGF
         * is the upper 6 bits while the OCF is the remaining 12 bits.
         */
        U16      hciCommand;   /* HCI command (see hci.h for commands) */
        U8       parmLen;      /* Length of the command parameters */
        U8      *parms;        /* Pointer to command parameters */
        U8       event;        /* Main event which signals end of operation .
                                * If the event type is HCE_VENDOR_SPECIFIC,
                                * then a pattern to match in the vendor
                                * specific event 'parms' should be provided 
                                * in the 'vendorSpecific' struct.
                                */

        struct {
            U8   offset;       /* Offset in 'parms' of the pattern to match. */
            U8   patternLen;   /* Length of the pattern to match.  If the 
                                * length is 0, then the pattern match will succeed,
                                * with the assumption that matching only the vendor
                                * specific event is sufficient.
                                */
            U8  *pattern;      /* Pattern to match in 'parms'. */
        } vendorSpecific;
    } in;

    /* Fields to read when the operation is complete */
    struct {
        /* The status must be checked to see if the remaining out parameters
         * valid. If the status is BT_STATUS_SUCCESS then the rest of the
         * out parameters are valid.
         */
        BtStatus status;

        U8       event;        /* Event ending operation (see hci.h for event) */
        U8       parmLen;      /* Length of event parameters */
        U8       *parms;       /* Pointer to event parameters */
    } out;
} MeGeneralHciCommand;


typedef U16 BtScoAudioSettings; /* This must match definition in mesco.h */
/*---------------------------------------------------------------------------
 * MeCommandToken structure
 *
 *     A token that represents a special HCI command, including
 *     user-defined HCI commands.
 */
typedef struct _MeCommandToken
{
    BtOperation     op;      /* For internal use by the stack. */
    BtOperation     cancelOp;/* For internal use by the stack. */
    BtRemoteDevice *remDev;  /* For internal use by the stack. */

    /* The caller must set the callback function. It is called
     * when the command is complete.
     */
    BtCallBack      callback;

    /* One of the fields in this union is used to store information
     * about the operation.
     */
    union {

        /* Information about a "Remote Name Request" operation */
        MeRemoteNameRequest name;

        /* Information for the SCO Audio settings command */
        BtScoAudioSettings vsettings;

        /* Information used in general and user defined operations */
        MeGeneralHciCommand general;

    } p;

    /* Group: The following field is for internal stack use only */
    BtEventType     eType;
    U8              flags;

} MeCommandToken;


/*---------------------------------------------------------------------------
 * BtAmpDedKey structure
 *
 *     Structures of type BtAmp,DedKey represent Dedicated AMP Link Key
 *     information.
 *
 */
typedef struct _BtAmpDedKey
{
    U8            len;
    BtLinkKeyType type;

    /* Holds the max size but use len to determine the actual key. The size
     * depends on the type of AMP
     */
    U8            key[32];
} BtAmpDedKey;

/*---------------------------------------------------------------------------
 * BtEirRspSpec structure
 *
 *     Used to specify data for the construction of an Extended Inquiry
 *     Response buffer. To use, this structure should be filled with zeros,
 *     then filled in as appropriate.
 */
typedef struct _BtEirRspSpec {
    /* An optional buffer of U8s containing SDP UUIDs in priority order.
     * UUIDs are specified using SDP_UUID_16BIT, SDP_UUID_32BIT, and/or
     * SDP_UUID_128BIT macros. Prioritized UUIDs that are found in
     * registered SDP records will appear before any unprioritized UUIDs.
     */
    const U8 *priority;

    /* Length of the priority buffer supplied in bytes, or 0 if not
     * supplied.
     */
    U16 priorityLen;

    /* An optional buffer of U8s containing SDP UUIDs to be excluded
     * from output.
     */
    const U8 *ignore;

    /* Length of the ignore buffer supplied in bytes, or 0 if not
     * supplied.
     */
    U16 ignoreLen;

} BtEirRspSpec;

/*---------------------------------------------------------------------------
 * BtEvent structure
 *
 *     Used to pass information from the stack to the application. Not all
 *     fields are valid at any given time. The type of event determines which
 *     fields are valid.
 */
struct _BtEvent
{
    /* Event causing callback. Always valid.*/
    BtEventType   eType;

    /* Error code. See BtEventType for guidance on whether errCode is valid. */
    BtErrorCode   errCode;

    /* Pointer to handler. Only valid for events directed to BtHandlers. */
    BtHandler    *handler;

    /* Parameters */
    union {
        BtInquiryResult    inqResult;  /* Inquiry result */
        BtDeviceLinkKey    bdLinkKey;  /* Returned Link Key */

        BtAccessibleMode   aMode;      /* New access mode */
        BtRemoteDevice    *remDev;     /* Pointer to remote device */
        BD_ADDR            bdAddr;     /* Device Address */
        MeCommandToken    *meToken;    /* Me command token */
        U8                 pMode;      /* Simple Pairing Mode */

        struct _BtSecurityToken  *secToken;   /* Security Token */
        struct _SdpQueryToken    *token;

        /* Information for BTEVENT_PAIRING_COMPLETE */
        struct {
            BtRemoteDevice *remDev;
            BtLinkKeyType   keyType;
        } pairingInfo;

        /* Information for BTEVENT_PIN_REQ */
        struct {
            BtRemoteDevice *remDev;

            /* If > 0 then the pin length returned in SEC_SetPin must be >= pinLen */
            U8              pinLen;
        } pinReq;

        /* Information for BTEVENT_SET_INQUIRY_MODE_CNF */
        BtInquiryMode      inqMode;

        /* Information for BTEVENT_SET_INQ_TX_PWR_LVL_CNF */
        BtInqTxPower       inqTxPwr;

        /* Information for BTEVENT_REMOTE_FEATURES */
        struct {
            BD_ADDR bdAddr;
            U8      features[8];
        } remoteFeatures;

        /* Information for BTEVENT_REMOTE_EXT_FEATURES */
        struct {
            BD_ADDR bdAddr;
            U8      page;
            U8      features[8];
        } remoteExtFeatures;

        /* Information for BTEVENT_REM_HOST_FEATURES */
        struct {
            BD_ADDR bdAddr;
            U8      features[8];
        } remHostFeatures;

        /* Information for BTEVENT_LINK_SUPERV_TIMEOUT_CHANGED */
        struct {
            BtRemoteDevice *remDev;
            U16             timeout;
        } linkSupervision;

        /* Information for BTEVENT_QOS_SETUP_COMPLETE */
        struct {
            /* Pointer to the remote device */
            BtRemoteDevice *remDev;

            /* Reserved */
            U8 flags;

            /* Flow Direction: 0 = outgoing, 1 = incoming, 2 = both */
            U8 flowDirection;

            /* Service Type: 0 = No Traffic, 1 = Best Effort, 2 Guaranteed */
            BtQosServiceType serviceType;

            /* Token Rate in octets per second */
            U32 tokenRate;

            /* Token Bucket Size in octets */
            U32 tokenBucketSize;

            /* Peak Bandwidth in octets per second */
            U32 peakBandwidth;

            /* Latency in microseconds */
            U32 latency;

            /* Delay Variation in microseconds */
            U32 delayVariation;

        } qos;

        /* Result for BTEVENT_SET_SNIFF_SUBRATING_PARMS_CNF */
        BtSniffSubrateParms *sniffSubrateParms;

        /* Information for BTEVENT_SNIFF_SUBRATE_INFO */
        struct {
            /* Pointer to the remote device */
            BtRemoteDevice *remDev;

            /* Maximum latency for data being transmitted from the local
             * device to the remote device.
             */
            U16 maxTxLatency;

            /* Maximum latency for data being received by the local
             * device from the remote device.
             */
            U16 maxRxLatency;

            /* The base sniff subrate timeout in baseband slots that the
             * remote device shall use.
             */
            U16 minRemoteTimeout;

            /* The base sniff subrate timeout in baseband slots that the
             * local device will use.
             */
            U16 minLocalTimeout;
        } sniffSubrateInfo;

        /* Result for BTEVENT_LOCAL_OOB_DATA */
        struct {
            /* Simple Pairing Hash C from the local device */
            U8 hashC[16];

            /* Simple Pairing Randomizer R the local device */
            U8 randR[16];
        } oobData;

        /* Result for BTEVENT_LOCAL_OOB_EXT_DATA */
        struct {
            /* Simple Pairing Hash C derived from the P-192 key from the local device */
            U8 hashC_192[16];

            /* Simple Pairing Randomizer R derived from the P-192 key from the local device */
            U8 randR_192[16];

            /* Simple Pairing Hash C derived from the P-256 key from the local device */
            U8 hashC_256[16];

            /* Simple Pairing Randomizer R derived from the P-256 key from the local device */
            U8 randR_256[16];
        } oobExtData;

        /* Information for BTEVENT_CONFIRM_NUMERIC_REQ, BTEVENT_PASS_KEY_REQ,
         * BTEVENT_DISPLAY_NUMERIC_IND, and BTEVENT_BOND_REQ.
         */
        struct {
            /* Pointer to the remote device */
            BtRemoteDevice *remDev;

            /* Numeric value received from Secure Simple Pairing (not valid
             * for BTEVENT_PASS_KEY_REQ or BTEVENT_BOND_REQ)
             */
            U32             numeric;

            /* Bonding has been requested. This contains the combined bonding information
             * of both the local and remote device. Both have to agree in order for bonding
             * to occur.
             */
            BtBondingMode   bondingMode;

            /* TRUE means the bonding information contains info from both sides */
            BOOL            bondingModeComplete;

            /* Bonding request from the remote device. This is only valid if bondingModeComplete
             * is TRUE
             */
            BtBondingMode   remBondingMode;

            /* If true, indicates a request to replace a local link key.
             * If false, indicates a request for creating a new local link key.
             */
            BOOL            keyReplace;

        } userIoReq;

        /* Result for BTEVENT_ENCRYPTION_CHANGE event. */
        struct {
            BtRemoteDevice *remDev;
            U8              mode; /* New encryption mode (uses the
                                   * BtEncryptMode type) */
        } encrypt;

        /* Result for BTEVENT_KEY_PRESSED */
        struct {
            BD_ADDR         bdAddr; /* Address of the device sending the keypress */
            BtKeyPressParm  parm;   /* The value of the keypress parameter */
        } keyPress;

        /* Result for BTEVENT_READ_AUTH_PAYLOAD_TIMEOUT */
        U16                 authPayloadTimeout;

        /* Result for BTEVENT_LINK_DISCONNECT */
        struct {
            BtRemoteDevice *remDev;

             /* If disconnection was successful, contains BEC_NO_ERROR.
              * errCode will contain the disconnect reason.
              *
              * Unsuccessful disconnections will contain an error code
              * as generated by the radio. In this case, errCode can be
              * ignored.
              */
            BtErrorCode status;
        } disconnect;

        /* Result for the BTEVENT_SCO_DATA_CNF event */
        struct {
            BtRemoteDevice *remDev;     /* Pointer to remote device */
            BtScoConnect   *scoCon;     /* SCO connection */
            BtPacket       *scoPacket;  /* SCO Packet Handled */
        } scoPacketHandled;

        /* Result for the BTEVENT_SCO_CONNECT_CNF && BTEVENT_SCO_CONNECT_IND
           events.
         */
        struct {
            BtRemoteDevice *remDev;     /* Pointer to remote device */
            U16             scoHandle;  /* SCO Connection handle for HCI */
            BtScoConnect   *scoCon;     /* SCO connection */
            BtLinkType      scoLinkType;/* SCO link type */
            BtScoTxParms   *scoTxParms; /* Pointer to eSCO TX parameters */
            BtScoRxParms   *scoRxParms; /* Pointer to eSCO RX parameters */
        } scoConnect;

        /* Result for the BTEVENT_SCO_DATA_IND event */
        struct {
            BtRemoteDevice *remDev;     /* Pointer to remote device */
            U16             scoHandle;  /* SCO Connection handle for HCI */
            BtScoConnect   *scoCon;     /* SCO connection. */
            U8              len;        /* SCO data len */
            U8             *ptr;        /* SCO data ptr */
            BtScoErrFlags   errFlags;   /* Erroneous Data Reporting */
        } scoDataInd;

        /* Result for the BTEVENT_SECURITY_CHANGE and
         * BTEVENT_SECURITY3_COMPLETE events
         */
        struct {
            U8    mode;    /* New security mode (uses the BtSecurityMode
                            * type). */
            BOOL  encrypt; /* Indicate if encryption set or not */
        } secMode;

        /* Results for the BTEVENT_MODE_CHANGE event */
        struct {
            BtRemoteDevice *remDev;
            BtLinkMode      curMode;
            U16             interval;
        } modeChange;

        /* Results for BTEVENT_ROLE_CHANGE */
        struct {
            BtRemoteDevice    *remDev;
            BtConnectionRole   newRole;    /* New role */
        } roleChange;

        /* Results for BTEVENT_CONN_PACKET_TYPE_CHNG */
        struct {
            BtRemoteDevice    *remDev;
            BtAclPacketType    packetTypes;    /* New packet types */
        } packetTypeChng;

        /* Informational BTEVENT_HCI_COMMAND_SENT event. */
        struct {
            /* "HciCommandType" values found in sys/hci.h.*/
            U16                    type;

            /* Length of optional parameters */
            U8                     length;

            /* Optional parameters to HCI command. Refer to Bluetooth HCI
             * specification for command parameter descriptions.
             */
            const U8*              parms;
        } hciCmd;

        /* Results for BTEVENT_DEVICE_SELECTED event. When received by
         * the Device Selection monitor, it represents the selection query.
         */
        struct _BtSelectDeviceToken*     select;

        /* BTEVENT_DEVICE_ADDED and BTEVENT_DEVICE_DELETED events. */
        struct _BtDeviceContext*   device;

        /* Valid during BTEVENT_HCI_FATAL_ERROR */
        HciTransError transError;

        /* Valid during BTEVENT_READ_SUPPORTED_CODECS */
        struct {
            U8      numCodecs;
            U8     *supportedCodecs;
            U8      numVendorCodecs;
            U8     *supportedVendorCodecs;
        } localSupportedCodecs;

        /* Valid during BTEVENT_VENDOR_SPECIFIC */
        struct {
            U8      length;   /* Length of vendor specific parameters */
            U8     *parms;    /* Vendor specfic parameters */
        } vendorSpecific;

        /* Valid during BTEVENT_CROSS_TRANS_KEY */
        struct {
            /* remDev may be 0  if no connection to BR/EDR exists */
            BtRemoteDevice *remDev;
            BtLinkKeyType   keyType;
            /*  device address used to store pairing information in the device
             *  database if bonding occurred (i.e. "bonded" is TRUE).
             */
            BtDevAddr       devAddr;
            /* TRUE if bonding occured and link was stored otherwise FALSE */
            BOOL            bonded;
        } crossKey;

    } p;
};


/*---------------------------------------------------------------------------
 * MeHciFlowParms structure
 *
 *     A structure used to hold the current HCI flow parameters.  This
 *     structure contains the maximum size of ACL and SCO transmit packets,
 *     as well as the current number of available buffers on the Host
 *     Controller.
 */
typedef struct _MeHciFlowParms {
    U16  aclTxPacketLen;
    U16  aclTxPacketsLeft;
    U8   scoTxPacketLen;
    U16  scoTxPacketsLeft;
    U16  aclMaxTxPackets;
    U16  leTxPacketLen;
    U16  leTxPacketsLeft;
    U16  leMaxTxPackets;
} MeHciFlowParms;

/*---------------------------------------------------------------------------
 * BtHandlerCtx structure
 *
 *     BtHandler structure with a context.
 */
typedef struct _BtHandlerCtx
{
    BtHandler handler;
    void      *context;
} BtHandlerCtx;

/*---------------------------------------------------------------------------
 * MeCmdTokenCtx structure
 *
 *     MeCmdToken structure with a context.
 */
typedef struct _MeCmdTokenCtx
{
    MeCommandToken  token;
    void           *context;
} MeCmdTokenCtx;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * ME_RegisterGlobalHandler()
 *
 *     Register a callback handler with the Management Entity to
 *     receive global events. The protocol stack sends a number
 *     of global events to all registered handlers (who have the
 *     proper event mask set). Clients must register a handler
 *     in order to receive events. The list of global events for
 *     which the callback will be called are listed in the
 *     Callback section below. The handler's event mask is two 32-bit 
 *     values as is initialized to BEM_NO_EVENTS and BEM2_NO_EVENTS.
 *
 * Parameters:
 *     handler - pointer to a BtHandler structure. The callback
 *         field of the handler must point to a valid callback function.
 *         The handler must be initialized using ME_InitHandler.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation completed successfully.
 *
 *     BT_STATUS_FAILED - the operation failed because the handle is
 *         already registered, the handle is not initialized properly.
 */
BtStatus ME_RegisterGlobalHandler(BtHandler *handler);

/*---------------------------------------------------------------------------
 * ME_InitHandler()
 *
 *     Initialize a handler. All global handlers must be initialized
 *     before they can be passed to ME functions. This function
 *     can be used or filling the contents of the handler with 0
 *     will also suffice. A handler should only need to be initialized
 *     once.
 *
 * Parameters:
 *     handler - pointer to a BtHandler structure.
 */
void ME_InitHandler(BtHandler *handler);
#define ME_InitHandler(h) InitializeListEntry(&((h)->node))

/*---------------------------------------------------------------------------
 * ME_UnregisterGlobalHandler()
 *
 *     Unregister a global handler. Upon successful completion of this
 *     function the handler is initialized and can be registered again
 *     without the need for calling ME_InitHandler.
 *
 * Parameters:
 *     handler - pointer to handler to unregister
 *
 * Returns
 *     BT_STATUS_SUCCESS - operation successful
 *
 *     BT_STATUS_FAILED - operations failed because handler
 *         was not registered or 0 (error check only).
 */
BtStatus ME_UnregisterGlobalHandler(BtHandler *handler);

/*---------------------------------------------------------------------------
 * ME_SetEventMask()
 *
 *     Set the event mask for the global handler. The event mask is for 
 *     global events. The event mask is two 32-bit values and this function 
 *     sets the first value to "mask" and sets the the second value to 0. 
 *     This is for backwards compatibility with code that is not aware of 
 *     the second event mask and the new events.
 *
 * Parameters:
 *     handler - pointer to handler.
 *
 *     mask - event mask.
 *
 * Returns
 *     BT_STATUS_SUCCESS - operation successful
 *
 *     BT_STATUS_FAILED - operations failed because handler
 *         was not registered or 0 (error check only).
 */
BtStatus ME_SetEventMask(BtHandler *handler, BtEventMask mask);

/*---------------------------------------------------------------------------
 * ME_SetEventMaskEx()
 *
 *     Set the event mask for the global handler. The event mask
 *     is for global events. The event mask is two 32-bit values.
 *
 * Parameters:
 *     handler - pointer to handler.
 *
 *     mask1 - event mask1.
 *
 *     mask2 - event mask2
 *
 * Returns
 *     BT_STATUS_SUCCESS - operation successful
 *
 *     BT_STATUS_FAILED - operations failed because handler
 *         was not registered or 0 (error check only).
 */
BtStatus ME_SetEventMaskEx(BtHandler *handler, BtEventMask mask1, BtEventMask2 mask2);

/*---------------------------------------------------------------------------
 * ME_GetEventMask()
 *
 *     Get the event mask for the global handler. The event mask is for global 
 *     events. The event mask is composed of two 32-bit values and this 
 *     function returns the first mask value. This function does not check to
 *     see if the handler is valid.
 *
 * Parameters:
 *     handler - pointer to handler.
 *
 * Returns:
 *     The fist event mask
 */
BtEventMask ME_GetEventMask(BtHandler *handler);
#define ME_GetEventMask(h) ((h)->eMask1)

/*---------------------------------------------------------------------------
 * ME_GetEventMask2()
 *
 *     Get the event mask for the global handler. The event mask is for global 
 *     events. The event mask is composed of two 32-bit values and this 
 *     function returns the second mask value. This function does not check to
 *     see if the handler is valid.
 *
 * Parameters:
 *     handler - pointer to handler.
 *
 * Returns:
 *     The second event mask
 */
BtEventMask2 ME_GetEventMask2(BtHandler *handler);
#define ME_GetEventMask2(h) ((h)->eMask2)

/*---------------------------------------------------------------------------
 * ME_SetInquiryMode()
 *
 *     Set the type of inquiry response that will be used during the inquiry
 *     process.  The inquiry result is received during the
 *     BTEVENT_INQUIRY_RESULT event.  It can contain the default response, a
 *     response with RSSI information, or an extended inquiry response.
 *
 * Parameters:
 *     mode - Contains one of the desired inquiry modes.
 *
 * Returns:
 *     BT_STATUS_PENDING - The Inquiry Mode is being sent to the host
 *         controller.  When complete, a BTEVENT_SET_INQUIRY_MODE_CNF event
 *         will be sent to all registered handlers.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress.  Only one operation can be in progress at a time. Keep
 *         track of the general events to see when the operation is complete.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameter
 *         was not properly initialized (XA_ERROR_CHECK only).
 */
BtStatus ME_SetInquiryMode(BtInquiryMode mode);

/*---------------------------------------------------------------------------
 * ME_GetInquiryMode()
 *
 *     Get the current inquiry mode.
 *
 * Returns:
 *     The current inquiry mode.
 */
BtInquiryMode ME_GetInquiryMode(void);

/*---------------------------------------------------------------------------
 * ME_SetInqTxPowerLevel()
 *
 *     Set the inquiry TX power level.  The power level must be between
 *     -70 and +20 dBm.
 *
 * Returns:
 *     BT_STATUS_PENDING - The Inquiry TX Power Level is being
 *         sent to the host controller.  When complete, a
 *         BTEVENT_SET_INQ_TX_PWR_LVL_CNF5 event will be sent to all
 *         registered handlers.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the
 *         parameter was not properly initialized (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress.  Only one operation can be in progress at a time. Keep
 *         track of the general events to see when the operation is complete.
 *
 *     BT_STATUS_NOT_SUPPORTED - The feature is not supported by the host
 *         controller.
 */
BtStatus ME_SetInqTxPowerLevel(BtInqTxPower power);

/*---------------------------------------------------------------------------
 * ME_GetInqRspTxPowerLevel()
 *
 *     Get the inquiry response TX power level.
 *
 * Returns:
 *     The current power level.
 */
BtInqRspTxPower ME_GetInqRspTxPowerLevel(void);

#if SDP_SERVER_SUPPORT == XA_ENABLED

/*---------------------------------------------------------------------------
 * ME_AutoCreateExtInquiryRsp()
 *
 *     Automatically creates an Extended Inquiry Response (EIR) from the
 *     currently registered SDP services, local device name, and inquiry
 *     response TX power level. This function will fill the buffer with as
 *     many Service Class UUIDs as possible, add the inquiry response TX
 *     power level (if valid), and then put in as much of the local device
 *     name as possible. Any remaining buffer space will be set to 0.
 *
 *     It is possible to add other information at the end of the created EIR.
 *     Please see the Bluetooth specification for a description of vendor
 *     specific items and flags that can be added to the EIR.
 *
 *     Once the inquiry response is assembled into the provided buffer, it
 *     can be registered with the controller by calling
 *     ME_SetExtInquiryRsp().
 *
 * Parameters:
 *     outBuffer - A memory location where the EIR will be assembled.
 *
 *     length - Length of the buffer.  An EIR can be no more than 240 bytes
 *         long, and no less than 5 bytes long.
 *
 * Returns:
 *     Length of the data in the buffer.
 */
U8 ME_AutoCreateExtInquiryRsp(U8 *outBuffer, U8 length);

/*---------------------------------------------------------------------------
 * ME_AutoCreateExtInquiryRspEx()
 *
 *     Similar to ME_AutoCreateExtInquiryRsp(), but allows extended
 *     parameters to govern the creation of the inquiry response.
 *     See BtEirRspSpec for details.
 *
 * Parameters:
 *     outBuffer - A memory location where the EIR will be assembled.
 *
 *     length - Maximum length of the buffer. An EIR can be no more than
 *         240 bytes long, and no less than 5 bytes long.
 *
 *     outLength - Upon successful return, filled with the length of data
 *         written into outBuffer.
 *
 *     info - Additional, optional information describing the request.
 *         See BtEirRspSpec for details.
 *
 * Returns:
 *     Success of the operation. If BT_STATUS_SUCCESS, outBuffer and
 *     outLength are filled with results.
 */
BtStatus ME_AutoCreateExtInquiryRspEx(U8 *outBuffer, U8 length,
                                      U8 *outLength, BtEirRspSpec *info);
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 * ME_SetExtInquiryRsp()
 *
 * Set the extended inquiry response that will be used when extended inquiry
 * mode is set.
 *
 * Parameters:
 *     Fec - Determines if FEC will be used with the extended inquiry
 *     response.
 *
 *     extInqResp - Contains the extended inquiry response that will be used
 *     by the local device when a remote device performs an inquiry.
 *
 *     Length - Contains the length of the extended inquiry response.
 *
 * Returns:
 *     BT_STATUS_PENDING - The Extended Inquiry Response is being sent
 *         to the host controller.  When complete, a
 *         BTEVENT_SET_EXT_INQUIRY_RESP_CNF event will be sent to all
 *         registered handlers.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress.  Only one operation can be in progress at a time. Keep
 *         track of the general events to see when the operation is complete.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the token
 *         was not properly initialized (XA_ERROR_CHECK only).
 */
BtStatus ME_SetExtInquiryRsp(BOOL Fec, U8* extInqResp, U8 Length);

/*---------------------------------------------------------------------------
 * ME_GetExtInqData()
 *
 *     Search the extended inquiry result (EIR) for the specified EIR data
 *     type, and copy the EIR data into the provided buffer. Any extra buffer
 *     space will be initialized to 0.  The length of the EIR data is assumed
 *     to be 240 bytes as defined in by the HCI specification.
 *
 * Parameters:
 *     Eir - Extended inquiry result received from the remote device.
 *
 *     Type - The type of EIR data to search for.
 *
 *     outBuffer - A buffer to receive the EIR data.
 *
 *     Length - The length of the buffer that will receive the EIR data.
 *
 * Returns:
 *     The length of the EIR data.
 */
U8 ME_GetExtInqData(U8 *Eir, BtEirDataType Type, U8 *outBuffer, U8 Length);
#define ME_GetExtInqData(e,t,o,l) ME_GetEirAdData(e, 240, t, o, l)

/*---------------------------------------------------------------------------
 * ME_GetEirAdData()
 *
 *     Search the extended inquiry result (EIR) or LE advertising data
 *     for the specified EIR data type, and copy the data into the provided
 *     buffer. Any extra buffer space will be initialized to 0.  The length
 *     of the EIR/LE advertising data is specified by the "eirLen" parameter.
 *     For EIR data it should be 240 bytes as defined in by the HCI
 *     specification.
 *
 * Parameters:
 *     eir - Extended inquiry result or LE advertising data received from
 *         the remote device.
 *
 *     eirLen - length of data pointed by "eir".
 *
 *     type - The type of EIR/LE advertising data to search for.
 *
 *     outBuffer - A buffer to receive the EIR/LE advertising data.
 *
 *     Length - The length of the buffer that will receive the
 *         EIR/LE advertising data.
 *
 * Returns:
 *     The length of the EIR/LE advertising data.
 */
U8 ME_GetEirAdData(U8 *eir, U8 eirLen, BtEirDataType type, U8 *out, U8 outLen);

/*---------------------------------------------------------------------------
 * ME_GetExtInqRemoteDeviceName()
 *
 *     Get the remote device name from an extended inquiry response (EIR).
 *     The name will be null terminated.  The length of the EIR data is
 *     assumed to be 240 bytes as defined in by the HCI specification.
 *
 * Parameters:
 *     Eir - Extended inquiry result data received from the remote device.
 *
 *     outBuffer - A buffer to receive a null-terminated remote device name.
 *
 *     Length - The length of the buffer that will receive the name
 *         (including terminating NULL). May be 0. If Length is non-zero,
 *         but shorter than the name data available, a truncated but
 *         NULL-terminated string is stored in outBuffer.
 *
 *     Partial - Pointer to a boolean. On return, will be set to TRUE if a
 *         partial name was found or FALSE if a complete name or no name
 *         was found.
 *
 * Returns:
 *     The full length of the remote device name available in EIR data, not
 *     including NULL termination.
 */
U8 ME_GetExtInqRemoteDeviceName(U8 *Eir, U8 *outBuffer, U8 Length, BOOL *partial);

/*---------------------------------------------------------------------------
 * ME_GetExtInqServiceClassList()
 *
 *     Get the list of service class UUIDs from an extended inquiry response
 *     (EIR). Upon return, the outBuffer will be filled with UUIDs of the
 *     size specified by uuidSize.  In the case of 2 byte and 4 byte UUIDs,
 *     they are returned in the host processor's format and can be accessed
 *     as U16 and U32 values.  In the case of 16 byte UUIDs, they are
 *     returned in big-endian format as defined by the SDP protocol.  The
 *     length of the EIR data is assumed to be 240 bytes as defined in by the
 *     HCI specification.
 *
 * Parameters:
 *     Eir - Extended inquiry result data received from the remote device.
 *
 *     uuidSize - Size of the UUID in bytes.  Must be 2, 4, or 16.
 *
 *     outBuffer - A buffer to receive the remote device name.
 *
 *     Length - The length of the buffer that will receive the name
 *         (including terminating NULL).
 *
 *     Partial - A boolean value, which is set to TRUE if a partial list was
 *         found.
 *
 * Returns:
 *     The number of UUIDs in the list.
 */
U8 ME_GetExtInqServiceClassList(U8 *Eir, U8 uuidSize, U8 *outBuffer, U8 Length, BOOL *partial);

/*---------------------------------------------------------------------------
 * ME_Inquiry()
 *
 *     Start a Bluetooth Inquiry procedure. If BT_STATUS_PENDING is
 *     returned then the results of the operation will be returned
 *     to all clients with registered global handlers. The following
 *     events will be sent to all registered handlers:
 *
 *     BTEVENT_INQUIRY_RESULT - indicates that a device was found. The
 *     "p.inqResult" field of the BtEvent contains the result.
 *
 *     BTEVENT_INQUIRY_COMPLETE - indicates that the inquiry process is
 *     complete. The "errCode" field of the BtEvent indicates whether
 *     the operation completed without error.
 *
 *     BTEVENT_INQUIRY_CANCELED - this will be returned if the inquiry
 *     operation is canceled. BTEVENT_INQUIRY_COMPLETE will not be
 *     returned.
 *
 *     BTEVENT_HCI_FATAL_ERROR - indicates fatal radio or HCI
 *     transport error. Assume all pending operations have failed.
 *
 * Parameters:
 *     lap - LAP used for the Inquiry (General or Limited)
 *
 *     length - Maximum amount of time before the Inquiry is
 *         halted. Range is 0x01 to 0x30. Time is length * 1.28 sec.
 *         The Generic Access profile specifies using the value
 *         BT_INQ_TIME_GAP100.
 *
 *     maxResp - The maximum number of responses. Specify zero to receive
 *         an unlimited number of responses.
 *
 * Returns:
 *     BT_STATUS_PENDING - The Inquiry process is started results
 *         will be sent via the handler. A BTEVENT_HCI_FATAL_ERROR event
 *         indicates a fatal radio or HCI transport error and that all
 *         pending operations have failed.
 *
 *     BT_STATUS_IN_PROGRESS - An inquiry process is already in
 *         progress. Only one Inquiry can be in progress at a time.
 *         Keep track of the general events to get the results from
 *         the current Inquiry or to see when it ends. If it has just
 *         ended then a cancel is also in progress so wait for
 *         the cancel to complete to start another inquiry.
 *
 *     BT_STATUS_FAILED - The operation failed.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_Inquiry(BtIac lap, U8 Length, U8 maxResp);

/*---------------------------------------------------------------------------
 * ME_CancelInquiry()
 *
 *     Cancel the current Inquiry process. When the inquiry
 *     operation is canceled all registered handlers will
 *     receive the BTEVENT_INQUIRY_CANCELED event. The "errCode"
 *     field of the BtEvent indicates the status of the
 *     operation.
 *
 * Returns:
 *     BT_STATUS_PENDING - The cancel operation was started
 *         successfully. The results will be sent to all clients
 *         with registered handlers. A BTEVENT_HCI_FATAL_ERROR event
 *         indicates a fatal radio or HCI transport error and that all
 *         pending operations have failed.
 *
 *     BT_STATUS_SUCCESS - The inquiry process was canceled
 *         immediately. It actually never was started.
 *         BTEVENT_INQUIRY_CANCELED event will be sent to all handlers
 *         before this function returns.
 *
 *     BT_STATUS_IN_PROGRESS - A cancel Inquiry is already in
 *         progress. Only one cancel can be in progress at a time.
 *         Keep track of the general events to see when the cancel
 *         is complete.
 *
 *     BT_STATUS_FAILED - The operation failed because an inquiry
 *         operation was not in progress.
 */
BtStatus ME_CancelInquiry(void);

/*---------------------------------------------------------------------------
 * ME_GetRemoteDeviceName()
 *
 *     Get the name of the remote device. If an ACL connection
 *     does not exist then a temporary one will be created to
 *     get the name. If this function returns BT_STATUS_PENDING
 *     then the result will be returned via the callback in the
 *     token with the BTEVENT_NAME_RESULT event. The "errCode" field
 *     indicates the status of the operation. "p.meToken" points
 *     to the token passed into the function. The output field of
 *     token contains the results if the status indicates success.
 *
 *     If a request to the same remote device is already in progress
 *     only one request will be made. This token will receive the
 *     results of the request in progress.
 *
 * Parameters:
 *     token - pointer to token containing input parameters and
 *         storage for output parameters. The token must be initialized
 *         before calling this function as follows:
 *
 *         "token.callback" must be set to the callback function.
 *
 *         "token.p.name.bdAddr" must contain the 48-bit address of
 *         the remote device.
 *
 *         "token.p.name.io.in.psi" must contain the page scan information.
 *         If the page scan information is not known then all fields should
 *         be 0. If "psi.clockOffset" is known (e.g. received in an
 *         Inquiry Response) then bit 15 should be set to 1 to inform the
 *         BT controller that the clock offset is valid.
 * Returns:
 *     BT_STATUS_PENDING - Operation is started and results will
 *         be sent to the callback.
 *
 *     BT_STATUS_FAILED - operation failed because the token was
 *         not properly initialized or already active (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_GetRemoteDeviceName(MeCommandToken *token);

/*---------------------------------------------------------------------------
 * ME_CancelGetRemoteDeviceName()
 *
 *     Request cancellation of a pending ME_GetRemoteDeviceName request.
 *     If this function returns BT_STATUS_SUCCESS, then "token" is
 *     free and no further events will be returned. If this function
 *     returns BT_STATUS_PENDING, then the result will be returned
 *     via the callback in "p.meToken" with the BTEVENT_NAME_RESULT event.
 *     The "errCode" field will indicate a radio-specific error code,
 *     or BEC_NO_ERROR if the remote name was received before the cancel
 *     completed.
 *
 * Parameters:
 *     token - pointer to token passed in to ME_GetRemoteDeviceName.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - the operation has completed successfully.
 *         The "token" is free and no further events will be returned.
 *
 *     BT_STATUS_PENDING - Operation is started and results will
 *         be sent to the callback.
 *
 *     BT_STATUS_FAILED - operation failed because the get remote
 *         device name request is no longer in progress.
 */
BtStatus ME_CancelGetRemoteDeviceName(MeCommandToken *token);

/*---------------------------------------------------------------------------
 * ME_SetLocalDeviceName()
 *
 *     Set the local device name. This is the name that other devices will
 *     get when performing a GetRemoteDeviceName to this device. The name
 *     is a UTF-8 encoded string. The name can be up to 248 bytes in length.
 *     If the name is less than 248 bytes it must be null-terminated.
 *
 * Parameters:
 *     name - pointer to a UTF-8 encoded string. The name will remain in use
 *         until the stack is deinitialized, or until this function is called
 *         with a different name. The name string should not be modified and
 *         the pointer must remain valid while it is in use.
 *
 *     len - indicates the number of bytes in the name including the null
 *         termination byte if the name is less than 248 bytes.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation is successful.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the name or
 *         length were not properly initialized (XA_ERROR_CHECK only).
 */
BtStatus ME_SetLocalDeviceName(const U8 *name, U8 len);

/*---------------------------------------------------------------------------
 * ME_ReadLocalBdAddr()
 *
 *     Get the 48-bit Bluetooth device address of the local device. This
 *     request cannot be issued until the radio has been initialized. Check
 *     ME_GetStackInitState for BTSS_INITIALIZED. If it's not initialized
 *     wait for the BTEVENT_HCI_INITIALIZED event on a global handler.
 *
 * Parameters:
 *     Addr - Pointer to a BD_ADDR structure to receive the address.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was successful.
 *
 *     BT_STATUS_FAILED - Operation failed because the radio is not
 *         initialized yet. Monitor the global events to be notified
 *         when the radio is initialized (BTEVENT_HCI_INITIALIZED).
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_ReadLocalBdAddr(BD_ADDR *Addr);

/*---------------------------------------------------------------------------
 * ME_SetClassOfDevice()
 *
 *     Set the class of device. The class of device is sent out as part of
 *     the response to inquiry. The actual bit pattern that is sent out is
 *     a composite of the value set using this function and the service
 *     class information found in registered SDP records.
 *
 * Parameters:
 *     classOfDevice - A 32-bit integer where the lower 3 bytes represents
 *         the class of device. The most significant 8 bits are ignored.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation is successful.
 */
BtStatus ME_SetClassOfDevice(BtClassOfDevice classOfDevice);

/*---------------------------------------------------------------------------
 * ME_FindRemoteDevice()
 *
 *     Return a pointer to the BtRemoteDevice structure corresponding to
 *     the device with given BD_ADDR. 0 is returned if a match is not found.
 *
 *     When this function returns successfully, the BtRemoteDevice structure
 *     returned always refers to a connected or connecting device. Use
 *     caution, as it will also return successfully if the device is in the
 *     process of disconnecting, but has not yet completed. This typically
 *     occurs if called during the notification of a link failure.
 *
 * Parameters:
 *     bdAddr - pointer to 48-bit address of the device
 *
 * Returns:
 *     Pointer to BtRemoteDevice structure if found. 0 is returned if the
 *     device is not found.
 */
BtRemoteDevice *ME_FindRemoteDevice(BD_ADDR *bdAddr);

/* Prototype for actual ME_FindRemoteDevice implementation */
BrEdrRemoteDevice *ME_FindRemoteDeviceP(U8* bdAddr);
#define ME_FindRemoteDevice(bd) (BtRemoteDevice *)(ME_FindRemoteDeviceP((bd)->addr))

/*---------------------------------------------------------------------------
 * ME_CreateLink()
 *
 *     Create an ACL link to the device specified by "bdAddr". If
 *     this function returns BT_STATUS_PENDING the results will be sent to
 *     the handler. When the operation is complete all registered global
 *     handlers will receive BTEVENT_LINK_CONNECT_CNF as will the handler
 *     passed to this function. The "errCode" field of the BtEvent will
 *     indicate if the operation was successful or not. If an incoming
 *     connection is in progress to this device then the handler will be
 *     bound and when the incoming connection is complete the handler will
 *     receive the BTEVENT_LINK_CONNECT_CNF event. In this case all global
 *     registered handlers will receive BTEVENT_LINK_CONNECT_IND instead of
 *     BTEVENT_LINK_CONNECT_CNF.
 *
 *     If connections already exist to other devices, then ME will attempt
 *     to put all other connections on hold, then begin a connection
 *     to the specified device.
 *
 *     If a connection already exists to the device indicated by bdAddr,
 *     this function will register the handler with the BtRemoteDevice
 *     and return BT_STATUS_SUCCESS immediately. In this way, many local
 *     handlers can be registered to the same connection. Be aware that
 *     ME_DisconnectLink will not take effect until it is called for
 *     ALL registered handlers of the BtRemoteDevice. To determine whether
 *     a connection already exists without registering a handler,
 *     use ME_FindRemoteDevice instead.
 *
 *     A slave device does not need to use this API unless it needs
 *     to register a local handler or initiate a disconnection of the link
 *     (see ME_DisconnectLink).
 *
 *     The type of connection created to the remote device uses
 *     the quality of service parameters necessary for the RFCOMM
 *     based profiles or profiles requiring reliable, non
 *     real-time critical links.
 *
 * Parameters:
 *     handler - pointer to an unregistered handler to receive
 *         events. This handler cannot be the global handler though
 *         they both can point to the same callback. The handler
 *         must be initialized using ME_InitHandler. The handler
 *         is in use until the link is disconnected and should not
 *         be modified while in use.
 *
 *     bdAddr - pointer to address of remote device.
 *
 *     psi - pointer to page scan information obtained during
 *         Inquiry. Initial links to a device should set this parameter
 *         to ensure a reliable connection. If the psi is not known, a
 *         default "psi" of 0 on an initial link will use clockOffset = 0,
 *         psMode = 0, and psRepMode = 2. If a client is binding to an
 *         existing remote device, then "psi" can be set to 0. If
 *         "psi.clockOffset" is known (e.g. received in an Inquiry Response)
 *         then bit 15 should be set to 1 to inform the BT controller that the
 *         clock offset is valid.
 *
 *     remDev - pointer to memory to receive remote device pointer.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation has started successfully
 *         result will be sent to handler.
 *
 *     BT_STATUS_SUCCESS - client is successfully bound to the remote device.
 *
 *     BT_STATUS_RESTRICTED - the operation failed because this remote
 *         device already has a restricted link. The handler is free.
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because this remote
 *         device is in the process of being disconnected. The handler
 *         is free.
 *
 *     BT_STATUS_NO_RESOURCES - the operation failed because
 *         the maximum number of connections already exist. The
 *         handler is free.
 *
 *     BT_STATUS_FAILED - operation failed because the handler is 0, the
 *         callback is 0 or the handler is not initialized (XA_ERROR_CHECK
 *         only). The handler is free.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_CreateLink(BtHandler *handler, BD_ADDR *bdAddr,
                       BtPageScanInfo *psi, BtRemoteDevice **remDev);


/*---------------------------------------------------------------------------
 * ME_ForceDisconnectLinkWithReason()
 *
 *     Force disconnection of the link regardless of registered handlers.
 *     See ME_DisconnectLink for additional information.
 *
 * Parameters:
 *     handler - pointer to registered handler (can be 0).
 *
 *     remDev - pointer to the remote device to be disconnected.
 *
 *     reason - disconnect reason. Must be one of: BEC_AUTHENTICATE_FAILURE,
 *          BEC_USER_TERMINATED, BEC_LOW_RESOURCES, BEC_POWER_OFF,
 *          BEC_UNSUPPORTED_REMOTE, or BEC_PAIR_UNITKEY_NO_SUPP.
 *
 *     forceDisconnect - If TRUE, will attempt to disconnect regardless of
 *         whether other handlers are still registered. If FALSE, behaves as
 *         ME_DisconnectLinkWithReason.
 *
 * Returns:
 *     BT_STATUS_INVALID_PARM - the operation failed because the "reason"
 *         parameter is invalid for a disconnect link operation.
 *
 *     See ME_DisconnectLink for additional returns.
 */
BtStatus ME_ForceDisconnectLinkWithReason(BtHandler *handler,
                                          BtRemoteDevice *remDev,
                                          BtErrorCode reason,
                                          BOOL forceDisconnect);

/*---------------------------------------------------------------------------
 * ME_ForceDisconnectLinkWithReasonEx()
 *
 *     Force disconnection of the link regardless of registered handlers.
 *     The link will not be closed if the remote device initiated the 
 *     connection unless the forceDisconnect flag is used.
 *
 *     See ME_DisconnectLink for additional information.
 *
 * Parameters:
 *     handler - pointer to registered handler (can be 0).
 *
 *     remDev - pointer to the remote device to be disconnected.
 *
 *     reason - disconnect reason. Must be one of: BEC_AUTHENTICATE_FAILURE,
 *          BEC_USER_TERMINATED, BEC_LOW_RESOURCES, BEC_POWER_OFF,
 *          BEC_UNSUPPORTED_REMOTE, or BEC_PAIR_UNITKEY_NO_SUPP.
 *
 *     forceDisconnect - If TRUE, will attempt to disconnect regardless of
 *         whether other handlers are still registered. If FALSE, behaves as
 *         ME_DisconnectLinkWithReason.
 *
 * Returns:
 *     BT_STATUS_INVALID_PARM - the operation failed because the "reason"
 *         parameter is invalid for a disconnect link operation.
 *
 *     See ME_DisconnectLink for additional returns.
 */
BtStatus ME_ForceDisconnectLinkWithReasonEx(BtHandler *handler,
                                            BtRemoteDevice *remDev,
                                            BtErrorCode reason,
                                            BOOL forceDisconnect);

/*---------------------------------------------------------------------------
 * ME_DisconnectLinkWithReason()
 *
 *     Request disconnection of the link with a non-default reason code.
 *     See ME_DisconnectLink for additional information.
 *
 * Parameters:
 *     handler - pointer to registered handler.
 *
 *     remDev - pointer to the remote device to be disconnected.
 *
 *     reason - disconnect reason. If multiple handlers call this
 *         function or ME_DisconnectLink, the last handler's reason code
 *         will be used. If reason is BEC_POWER_OFF, disconnect is
 *         more forceful in that all the handlers need not have already
 *         been freed.
 *
 * Returns:
 *     BT_STATUS_INVALID_PARM - the operation failed because the "reason"
 *         parameter is invalid for a disconnect link operation.
 *
 *     See ME_DisconnectLink for additional returns.
 */
BtStatus ME_DisconnectLinkWithReason(BtHandler* handler,
                                     BtRemoteDevice* remDev,
                                     BtErrorCode reason);
#define ME_DisconnectLinkWithReason(ha, rd, rs) \
    (ME_ForceDisconnectLinkWithReason(ha, rd, rs, FALSE))

/*---------------------------------------------------------------------------
 * ME_DisconnectLinkWithReasonEx()
 *
 *     Request disconnection of the link with a non-default reason code.
 *     The link will not be closed if the remote device initiated the 
 *     connection.  However, the connection can always be forced down by 
 *     using the other disconnect API's. 
 * 
 *     See ME_DisconnectLink for additional information.
 *
 * Parameters:
 *     handler - pointer to registered handler.
 *
 *     remDev - pointer to the remote device to be disconnected.
 *
 *     reason - disconnect reason. If multiple handlers call this
 *         function or ME_DisconnectLink, the last handler's reason code
 *         will be used. If reason is BEC_POWER_OFF, disconnect is
 *         more forceful in that all the handlers need not have already
 *         been freed.
 *
 * Returns:
 *     BT_STATUS_INVALID_PARM - the operation failed because the "reason"
 *         parameter is invalid for a disconnect link operation.
 *
 *     See ME_DisconnectLink for additional returns.
 */
BtStatus ME_DisconnectLinkWithReasonEx(BtHandler* handler,
                                       BtRemoteDevice* remDev,
                                       BtErrorCode reason);
#define ME_DisconnectLinkWithReasonEx(ha, rd, rs) \
    (ME_ForceDisconnectLinkWithReasonEx(ha, rd, rs, FALSE))

/*---------------------------------------------------------------------------
 * ME_DisconnectLink()
 *
 *     Request disconnection of the link, using the default reason code
 *     (BEC_USER_TERMINATED). This function also deregisters the
 *     specified handler, which was previously registered with ME_CreateLink.
 *
 *     The link is actually closed only when all registered handlers have
 *     requested disconnection and all L2CAP connections are closed. If
 *     a device needs to know exactly when the disconnection occurs,
 *     it should use a global handler to monitor connection state. The
 *     handler specified in this API is freed by this function, so it
 *     will not be notified of any disconnection events.
 *
 *     If the remote device initiated the connection, the local
 *     device may have no registered handler for the connection. In this
 *     case, the local device must call ME_CreateLink to register a handler, 
 *     then call this function to request disconnection.
 *
 * Parameters:
 *     handler - pointer to registered handler.
 *
 *     remDev - pointer to the remote device to be disconnected.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - the operation has completed successfully.
 *         The caller's handler is unbound from the link. When the link
 *         is actually disconnected the BTEVENT_LINK_DISCONNECT event will
 *         be sent to all registered global handlers, unless the link had
 *         not actually come up yet, in which case no event will be
 *         generated. The handler is free upon return from this function.
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because the
 *         link is the process of coming up. The connect request has
 *         been made by a remote device. Wait until the
 *         BTEVENT_LINK_CONNECT_CNF event has been received. The handler
 *         is not free.
 *
 *     BT_STATUS_FAILED - the operation failed. The link is
 *         already disconnected, disconnect has already been
 *         issued for this handler, or the handler is not
 *         bound to the link (XA_ERROR_CHECK only).
 */
BtStatus ME_DisconnectLink(BtHandler* handler, BtRemoteDevice* remDev);
#define ME_DisconnectLink(ha, rd) \
    (ME_ForceDisconnectLinkWithReason(ha, rd, BEC_USER_TERMINATED, FALSE))

/*---------------------------------------------------------------------------
 * ME_DisconnectLinkEx()
 *
 *     Request disconnection of the link, using the default reason code
 *     (BEC_USER_TERMINATED). This function behaves the same as 
 *     ME_DisconnectLink, except that the link will not be closed if the 
 *     remote device initiated the connection.  However, the connection can 
 *     always be forced down by using the other disconnect API's. 
 *
 *     See ME_DisconnectLink for additional information.
 *
 * Parameters:
 *     handler - pointer to registered handler.
 *
 *     remDev - pointer to the remote device to be disconnected.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - the operation has completed successfully.
 *         The caller's handler is unbound from the link. When the link
 *         is actually disconnected the BTEVENT_LINK_DISCONNECT event will
 *         be sent to all registered global handlers, unless the link had
 *         not actually come up yet, in which case no event will be
 *         generated. The handler is free upon return from this function.
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because the
 *         link is the process of coming up. The connect request has
 *         been made by a remote device. Wait until the
 *         BTEVENT_LINK_CONNECT_CNF event has been received. The handler
 *         is not free.
 *
 *     BT_STATUS_FAILED - the operation failed. The link is
 *         already disconnected, disconnect has already been
 *         issued for this handler, or the handler is not
 *         bound to the link (XA_ERROR_CHECK only).
 */
BtStatus ME_DisconnectLinkEx(BtHandler* handler, BtRemoteDevice* remDev);
#define ME_DisconnectLinkEx(ha, rd) \
    (ME_ForceDisconnectLinkWithReasonEx(ha, rd, BEC_USER_TERMINATED, FALSE))

/*---------------------------------------------------------------------------
 * ME_CancelCreateLink()
 *
 *     Request cancellation of the pending ME_CreateLink. This function
 *     also deregisters the specified handler, which was previously
 *     registered when the ME_CreateLink was initiated.
 *
 *     Note: This API shall not be used directly if the BlueSDK Connection 
 *     Manager is enabled. The Connection manager uses this API, acts on 
 *     events generated by this API and assumes that it is the sole user 
 *     of it. (see CMGR_RemoveDataLink).
 *
 * Parameters:
 *     handler - pointer to registered handler.
 *
 *     remDev - pointer to the remote device associated with the
 *          ME_CreateLink to be canceled.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - the operation has completed successfully.
 *         The caller's handler is unbound from the link. If more than one
 *         handler is bound to the link, no further events will occur.
 *         Otherwise, the BTEVENT_LINK_CREATE_CANCEL event will be sent to
 *         all registered global handlers when the cancel completes. The
 *         global handler must still check the BtEvent "errCode" field
 *         in the event structure, as the cancel may fail and the link may
 *         require further maintenance. The handler is free upon return
 *         from this function.
 *
 *     BT_STATUS_IN_PROGRESS - the operation is already in progress
 *
 *     BT_STATUS_FAILED - the operation failed. The link is already
 *         connected or disconnected, or the handler is not bound to
 *         the link (XA_ERROR_CHECK only).
 */
BtStatus ME_CancelCreateLink(BtHandler* handler, BtRemoteDevice* remDev);

/*---------------------------------------------------------------------------
 * ME_RegisterAcceptHandler()
 *
 *     Register the handler that will accept incoming connections.
 *     If no handler is registered then the Management Entity will
 *     accept all incoming connections automatically. If the
 *     handler is set it will be responsible for accepting
 *     connections by calling ME_AcceptIncomingLink. The handler
 *     will be notified with the BTEVENT_LINK_CONNECT_REQ event. The
 *     p.remDev field of the btEvent will contain a pointer to the
 *     remote device.
 *
 * Parameters:
 *     handler - pointer to handler. This can be a registered global
 *         handler but does not have to be.
 *
 * Returns:
 *     Pointer to old handler.
 */
BtHandler *ME_RegisterAcceptHandler(BtHandler *handler);

/*---------------------------------------------------------------------------
 * ME_AcceptIncomingLink()
 *
 *     Accept an incoming link. The incoming link is signaled by
 *     a call to the accept handler with the event
 *     BTEVENT_LINK_CONNECT_REQ.
 *
 * Parameters:
 *     remDev - pointer to the remote device structure representing
 *         the incoming connection. This was passed in the
 *         "btEvent.p.remDev" field of the BTEVENT_LINK_CONNECT_REQ event.
 *
 *     role - desired role. BCR_MASTER cause a master/slave switch
 *         with the local device becoming the master. BCR_SLAVE will
 *         cause the local device to remain the slave.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully
 *         and the result will be sent to the accept handler via
 *         the BTEVENT_LINK_ACCEPT_RSP event. When complete all registered
 *         global handlers will receive the BTEVENT_LINK_CONNECT_IND event.
 *         The "errCode" field of the BtEvent will indicate the success.
 *
 *     BT_STATUS_FAILED - the operation failed because the remote
 *         device is not in a state to accept an incoming connection.
 *
 *     BT_STATUS_INVALID_PARM - the operation failed because the
 *         "remDev" parameter is invalid or 0, or the role parameter
 *         is invalid (XA_ERROR_CHECK only).
 *
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_AcceptIncomingLink(BtRemoteDevice *remDev, BtConnectionRole role);

/*---------------------------------------------------------------------------
 * ME_RejectIncomingLink()
 *
 *     Reject an incoming link. The incoming link is signaled by
 *     a call to the accept handler with the event
 *     BTEVENT_LINK_CONNECT_REQ.
 *
 * Parameters:
 *     remDev - pointer to the remote device structure representing
 *         the incoming connection. This was passed in the
 *         "btEvent.p.remDev" field of the BTEVENT_LINK_CONNECT_REQ event.
 *
 *     reason - reason for the rejection. The only reasons allowed
 *         are as follows:
 *
 *     BEC_LIMITED_RESOURCE - Host rejected due to limited resources
 *
 *     BEC_SECURITY_ERROR   - Host rejected due to security reasons
 *
 *     BEC_PERSONAL_DEVICE  - Host rejected (remote is personal device)
 *
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully
 *         and the result will be sent to the accept handler via
 *         the BTEVENT_LINK_REJECT_RSP event.
 *
 *     BT_STATUS_FAILED - the operation failed because the remote
 *         device is not in a state to reject an incoming connection.
 *
 *     BT_STATUS_INVALID_PARM - the operation failed because the
 *         "remDev" parameter is invalid or 0, or the reason is not
 *         valid (XA_ERROR_CHECK only).
 *
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_RejectIncomingLink(BtRemoteDevice *remDev, BtErrorCode reason);

/*---------------------------------------------------------------------------
 * ME_SetConnectionRole()
 *
 *     Set the role that the local device prefers in both incoming
 *     and outgoing connections. This role applies only if there is no
 *     registered accept handler when the Management Entity automatically
 *     accepts incoming connections. The connection role applies to
 *     all future connections; it does not affect existing connections.
 *
 * Parameters:
 *     role - The desired role must be BCR_MASTER or BCR_ANY. When set to
 *         BCR_ANY, incoming connections default to slave role and outgoing
 *         connections default to master role, the remote device may
 *         change these roles. When set to BCR_MASTER, incoming connections
 *         force a role switch and outgoing connections do not permit a
 *         role switch. If the remote device refuses a master/slave switch,
 *         the connection will still succeed with role unchanged.
 *
 * Returns:
 *     The old role.
 */
BtConnectionRole ME_SetConnectionRole(BtConnectionRole role);

/*---------------------------------------------------------------------------
 * ME_GetCurrentMode()
 *
 *     Get the current mode of the link to the remote device. The
 *     value is meaningless if a connection does not exist to
 *     the remote device. This function does not check for a valid
 *     remDev pointer.
 *
 * Parameters:
 *     rm - pointer to remote device.
 *
 * Returns:
 *     The current mode if a link exists otherwise the value
 *     is meaningless.
 */
BtLinkMode ME_GetCurrentMode(BtRemoteDevice *rm);
#define ME_GetCurrentMode(r) ((r)->mode)

/*---------------------------------------------------------------------------
 * ME_GetCurrentRole()
 *
 *     Get the current role played by the local device. The
 *     value is meaningless if a connection does not exist to
 *     the remote device. This function does not check for a valid
 *     remDev pointer. When the role is currently being discovered,
 *     the role BCR_UNKNOWN will be returned. When the role discovery
 *     completes, the BTEVENT_ROLE_CHANGE event will be indicated.
 *
 * Parameters:
 *     rm - pointer to remote device.
 *
 * Returns:
 *     The current role if a link exists otherwise the value
 *     is meaningless.
 */
BtConnectionRole ME_GetCurrentRole(BtRemoteDevice *rm);
#define ME_GetCurrentRole(r) ((r)->role > BCR_UNKNOWN ? BCR_UNKNOWN : (r)->role)

/*---------------------------------------------------------------------------
 * ME_Hold()
 *
 *     Put the ACL link associated with remDev in hold mode. When the
 *     link is actually placed in hold all registered and bound handlers
 *     will receive a BTEVENT_MODE_CHANGE event signaling the change.
 *
 * Parameters:
 *     remDev - pointer to remote device
 *
 *     max - maximum length of the hold interval. The hold time
 *         is calculated as max * 0.625 ms. Range is 0.625ms to 40.9 sec.
 *
 *     min - minimum length of the hold interval. The hold time
 *         is calculated as min * 0.625 ms. Range is 0.625ms to 40.9 sec.
 *         The parameter min should not be greater than max.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         All registered handlers and handlers bound to the remote device
 *         will receive the BTEVENT_MODE_CHANGE event. The "errCode" field
 *         of the BtEvent will indicate the success or failure of the mode
 *         change event. The "p.modeChange" field indicates for which remote
 *         Device the change has occurred along with the new mode and
 *         interval. It is possible that link is disconnected before the
 *         mode change has occurred. In that case the handlers will not
 *         receive BTEVENT_MODE_CHANGE but instead will receive
 *         BTEVENT_LINK_DISCONNECT.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because a mode
 *         change or disconnect operation is already in progress.
 *
 *     BT_STATUS_FAILED - the operation failed because ACL link is
 *         in wrong state.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_Hold(BtRemoteDevice *remDev, U16 max, U16 min);

/*---------------------------------------------------------------------------
 * ME_StartSniff()
 *
 *     Start sniff mode for the ACL link specified by "remDev".
 *
 * Parameters:
 *     remDev - pointer to remote device
 *
 *     info - pointer to the sniff mode parameters. This structure may
 *            be freed after ME_StartSniff returns.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         All registered handlers and handlers bound to the remote device
 *         will receive the BTEVENT_MODE_CHANGE event. The "errCode" field
 *         of the BtEvent will indicate the success or failure of the mode
 *         change event. The "p.modeChange" field indicates for which remote
 *         Device the change has occurred along with the new mode and
 *         interval. It is possible that link is disconnected before the
 *         mode change has occurred. In that case the handlers will not
 *         receive BTEVENT_MODE_CHANGE but instead will receive
 *         BTEVENT_LINK_DISCONNECT.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because a mode
 *         change or disconnect operation is already in progress.
 *
 *     BT_STATUS_FAILED - the operation failed.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_StartSniff(BtRemoteDevice *remDev, const BtSniffInfo *info);


/*---------------------------------------------------------------------------
 * ME_StopSniff()
 *
 *     Stop sniff mode and enter active mode for the ACL link
 *     specified by remDev.
 *
 * Parameters:
 *     remDev - pointer to remote device.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         All registered handlers and handlers bound to the remote device
 *         will receive the BTEVENT_MODE_CHANGE event. The "errCode" field
 *         of the BtEvent will indicate the success or failure of the mode
 *         change event. The "p.modeChange" field indicates for which remote
 *         Device the change has occurred along with the new mode and
 *         interval. It is possible that link is disconnected before the
 *         mode change has occurred. In that case the handlers will not
 *         receive BTEVENT_MODE_CHANGE but instead will receive
 *         BTEVENT_LINK_DISCONNECT.
 *
 *     BT_STATUS_INVALID_PARM - the parameters are not valid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because a mode
 *         change or disconnect operation is already in progress.
 *
 *     BT_STATUS_FAILED - the operation failed. Device is not in
 *         sniff mode.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_StopSniff(BtRemoteDevice *remDev);


/*---------------------------------------------------------------------------
 * ME_SetSniffSubratingParms()
 *
 *     Set the Sniff Subrating Parameters for ACL link specified by "remDev".
 *
 * Parameters:
 *     remDev - pointer to remote device
 *
 *     info - pointer to the Sniff Subrating parameters. This structure may
 *            be freed after ME_SetSniffSubratingParms() returns.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         All registered handlers and handlers bound to the remote device
 *         will receive the BTEVENT_SET_SNIFF_SUBRATING_PARMS_CNF event.
 *         The "errCode" field of the BtEvent will indicate the success or
 *         failure of the command.  The "p.sniffSubrateParms" field contains
 *         the parameters during the call.  When Sniff Subrating is enabled
 *         or re-negotiated on the link, a BTEVENT_SNIFF_SUBRATE_INFO event
 *         will be received.  These parameters are valid for the duration
 *         of the ACL link.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because a mode
 *         change or disconnect operation is already in progress.
 *
 *     BT_STATUS_FAILED - the operation failed.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_SetSniffSubratingParms(BtRemoteDevice *remDev, const BtSniffSubrateParms *parms);

/*---------------------------------------------------------------------------
 * ME_StartPark()
 *
 *     Start park mode for the ACL link specified by "remDev".
 *
 * Parameters:
 *     rm - pointer to remote device
 *
 *     max - acceptable longest period between beacons. The time
 *         is calculated as max * 0.625 ms. Range is 0.625ms to 40.9 sec.
 *
 *     min - acceptable shortest period between beacons. The time
 *         is calculated as min * 0.625 ms. Range is 0.625ms to 40.9 sec.
 *         The parameter min should not be greater than max.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         All registered handlers and handlers bound to the remote device
 *         will receive the BTEVENT_MODE_CHANGE event. The "errCode" field
 *         of the BtEvent will indicate the success or failure of the mode
 *         change event. The "p.modeChange" field indicates for which remote
 *         Device the change has occurred along with the new mode and
 *         interval. It is possible that link is disconnected before the
 *         mode change has occurred. In that case the handlers will not
 *         receive BTEVENT_MODE_CHANGE but instead will receive
 *         BTEVENT_LINK_DISCONNECT.
 *
 *     BT_STATUS_INVALID_PARM - the parameters are not valid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because a mode
 *         change or disconnect operation is already in progress.
 *
 *     BT_STATUS_FAILED - the operation failed.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_StartPark(BtRemoteDevice *remDev, U16 max, U16 min);


/*---------------------------------------------------------------------------
 * ME_StopPark()
 *
 *     Stop park mode and enter active mode for the ACL link
 *     specified by "remDev".
 *
 * Parameters:
 *     remDev - pointer to remote device.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         All registered handlers and handlers bound to the remote device
 *         will receive the BTEVENT_MODE_CHANGE event. The "errCode" field
 *         of the BtEvent will indicate the success or failure of the mode
 *         change event. The "p.modeChange" field indicates for which remote
 *         Device the change has occurred along with the new mode and
 *         interval. It is possible that link is disconnected before the
 *         mode change has occurred. In that case the handlers will not
 *         receive BTEVENT_MODE_CHANGE but instead will receive
 *         BTEVENT_LINK_DISCONNECT.
 *
 *     BT_STATUS_INVALID_PARM - the parameters are not valid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because a mode
 *         change or disconnect operation is already in progress.
 *
 *     BT_STATUS_FAILED - the operation failed because the link is
 *         not in park.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_StopPark(BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * ME_StartParkAll()
 *
 *     Start park mode for all ACL links.
 *
 * Parameters:
 *     max - acceptable longest period between beacons. The time
 *         is calculated as max * 0.625 ms. Range is 0.625ms to 40.9 sec.
 *
 *     min - acceptable shortest period between beacons. The time
 *         is calculated as min * 0.625 ms. Range is 0.625ms to 40.9 sec.
 *         The parameter min should not be greater than max.
 *
 *     num - pointer to an integer that receives the number of devices
 *         that were requested to enter park mode. This count can be
 *         used in conjunction with BTEVENT_MODE_CHANGE events to determine
 *         when all devices are parked.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         All registered handlers and handlers bound to the remote devices
 *         will receive the BTEVENT_MODE_CHANGE event. The "errCode" field
 *         of the BtEvent will indicate the success or failure of the mode
 *         change event. The "p.modeChange" field indicates for which remote
 *         device the change has occurred along with the new mode and
 *         interval. It is possible that link is disconnected before the
 *         mode change has occurred. In that case the handlers will not
 *         receive BTEVENT_MODE_CHANGE but instead will receive
 *         BTEVENT_LINK_DISCONNECT.
 *
 *     BT_STATUS_INVALID_PARM - the parameters are not valid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because a mode
 *         change or disconnect operation is already in progress.
 *
 *     BT_STATUS_FAILED - the operation failed.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_StartParkAll(U16 max, U16 min, U8 *num);

/*---------------------------------------------------------------------------
 * ME_StopParkAll()
 *
 *     Return all parked ACL links to active mode.
 *
 * Parameters:
 *     num - pointer to an integer that receives the number of devices
 *         that were requested to exit park mode. This count can be
 *         used in conjunction with BTEVENT_MODE_CHANGE events to determine
 *         when all devices are unparked.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         All registered handlers and handlers bound to the remote device
 *         will receive the BTEVENT_MODE_CHANGE event. The "errCode" field
 *         of the BtEvent will indicate the success or failure of the mode
 *         change event. The "p.modeChange" field indicates for which remote
 *         Device the change has occurred along with the new mode and
 *         interval. It is possible that link is disconnected before the
 *         mode change has occurred. In that case the handlers will not
 *         receive BTEVENT_MODE_CHANGE but instead will receive
 *         BTEVENT_LINK_DISCONNECT.
 *
 *     BT_STATUS_INVALID_PARM - the parameters are not valid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because a mode
 *         change or disconnect operation is already in progress.
 *
 *     BT_STATUS_FAILED - the operation failed because the link is
 *         not in park.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_StopParkAll(U8 *num);

/*---------------------------------------------------------------------------
 * ME_SwitchRole()
 *
 *     Switch the current role the device is performing for the ACL link
 *     specified by remDev. If the current role is slave then switch to
 *     master. If the current role is master then switch to slave. The
 *     current role can be found via remDev.role. The result of the
 *     operation will be returned via the BTEVENT_ROLE_CHANGE event.
 *
 * Parameters:
 *     remDev - pointer to remote device
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         All registered handlers and handlers bound to the remote device
 *         will receive the BTEVENT_ROLE_CHANGE event. The "errCode" field
 *         of the BtEvent will indicate the success or failure of the role
 *         change event. The "p.roleChange" field indicates for which remote
 *         Device the change has occurred along with the new role.  It is
 *         possible that link is disconnected before the role change has
 *         occurred. In that case the handlers will not receive
 *         BTEVENT_ROLE_CHANGE but instead will receive
 *         BTEVENT_LINK_DISCONNECT.
 *
 *     BT_STATUS_INVALID_PARM - the parameters are not valid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because a mode
 *         change or disconnect operation is already in progress.
 *
 *     BT_STATUS_FAILED - the operation failed.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_SwitchRole(BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * ME_SetAccessibleModeNC()
 *
 *     Set the accessibility mode when the device is not connected. If the
 *     mode is set to a value other than BAM_NOT_ACCESSIBLE and there are no
 *     connections then the Bluetooth radio will enter inquiry and/or page
 *     scan mode using the information passed in info. If i
 *     BT_ALLOW_SETTING_SCAN_PARMS is disabled or if the info fields are set
 *     to zero (0) then the scan parameters are not set by the host and the
 *     radio module default values are used. It is assumed that the macro
 *     defaults match the radio defaults (see BT_DEFAULT_PAGE_SCAN_WINDOW
 *     documentation.) Any call to ME_SetAccessibleModeNC with info
 *     set to 0 will not change the settings as the radio has already been
 *     initialized to its default settings. If the default values of the scan
 *     parameters are all set to 0, then the host will not set these values until
 *     a call to ME_SetAccessibleModeNC provides non-zero valid values. If there
 *     is a connection or a connection is in the process  of being created then
 *     mode and info are saved and applied when all connections are disconnected.
 *
 *     To keep other devices from finding and connecting to this  device set
 *     the mode to BAM_NOT_ACCESSIBLE. The default mode when the stack is
 *     first loaded and initialized is controlled by BT_DEFAULT_ACCESS_MODE_NC.
 *
 *     In setting the values for info. Both "inqWindow" and "inqInterval" must
 *     be set to defaults or to legitimate values. The range for values is
 *     0x0012 to 0x1000. The inqWindow must be less than or equal to the
 *     inqInterval. Likewise, the pageWindow must be less than or equal to the
 *     pageInterval. The time calculated by taking the value * 0.625ms. It is
 *     an error if one is a zero and the other is not. This is also true for
 *     "pageInterval" and "pageWindow".
 *
 *     Any time the scan interval or window is different from the current
 *     settings in the radio, the radio will be instructed to change to the
 *     new settings. This means that if there are different settings for the
 *     connected state versus the non-connected state, the radio module will be
 *     instructed to change the settings when the first connection comes up and
 *     when the last connection goes down (automatically). This also means that
 *     if different values for window and interval are set when going from any
 *     setting of accessible to non-accessible then the radio will be instructed
 *     to change. In most cases it is best to use the radio defaults. In this
 *     way the radio is never told to change the scan interval or window.
 *
 * Parameters:
 *     mode - desired accessibility mode
 *
 *     info - pointer to structure containing the inquiry and page
 *         scan interval and window to use. If info is 0 then the
 *         defaults set by the radio module are used.
 *
 * Returns:
 *     BT_STATUS_PENDING - the mode is being set. All registered
 *         global handlers with the BEM_ACCESSIBLE_CHANGE mask set will
 *         receive BTEVENT_ACCESSIBLE_CHANGE event when the mode change
 *         actually takes affect or an error has occurred. The "errCode"
 *         field of the BtEvent indicates the status of the operation.
 *         If the operation is successful the "aMode" field of BtEvent
 *         indicates the new mode.  A BTEVENT_HCI_FATAL_ERROR event
 *         indicates a fatal radio or HCI transport error and that all
 *         pending operations have failed.
 *
 *     BT_STATUS_SUCCESS - Accessible mode is set. No event
 *         is sent out. This is returned if a connection exists and
 *         the values are only saved or info already matches the current
 *         setting.
 *
 *     BT_STATUS_IN_PROGRESS - operation failed because a change
 *         is already in progress. Monitor the global events to
 *         determine when the change has taken place.
 *
 *     BT_STATUS_INVALID_PARM - operation failed. The mode or info
 *         parameter contains bad values.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_SetAccessibleModeNC(BtAccessibleMode mode, const BtAccessModeInfo *info);

/*---------------------------------------------------------------------------
 * ME_GetAccessibleModeNC()
 *
 *     Return the current accessibility mode that is used when no
 *     connections exist. This is not necessarily the current mode.
 *
 * Parameters:
 *     mode - pointer to memory to receive accessibility mode. If
 *         mode is 0 then mode is not returned. If the accessible mode
 *         has not yet been set, and therefore unknown, 0xff will be returned.
 *
 *     info - pointer to structure to receive accessibility info.
 *         If info is 0 then info is not returned.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation was successful
 *
 *     BT_STATUS_IN_PROGRESS - operation failed because a change
 *         is in progress. Monitor the global events to determine when
 *         the change is finished.
 *
 *     BT_STATUS_FAILED - operation failed.
 */
BtStatus ME_GetAccessibleModeNC(BtAccessibleMode *mode,
                                BtAccessModeInfo *info);

/*---------------------------------------------------------------------------
 * ME_GetCurAccessibleMode()
 *
 *     Return the current accessibility mode. This is the mode
 *     at the very instant that call is made. It may be about
 *     to change so it is important to check the global events.
 *
 * Parameters:
 *     mode - pointer to memory to receive accessibility mode. If
 *         mode is 0 then mode is not returned. If the accessible mode
 *         has not yet been set, and therefore unknown, 0xff will be returned.
 *
 *     info - pointer to structure to receive accessibility info.
 *         If info is 0 then info is not returned.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation was successful
 *
 *     BT_STATUS_FAILED - operation failed.
 */
BtStatus ME_GetCurAccessibleMode(BtAccessibleMode *mode,
                                 BtAccessModeInfo *info);


#if BT_ALLOW_SCAN_WHILE_CON == XA_ENABLED
/*---------------------------------------------------------------------------
 * ME_SetAccessibleModeC()
 *
 *     Set the accessibility mode to be used when the device is connected to
 *     one or more devices. If the mode is set to a value other than
 *     BAM_NOT_ACCESSIBLE and there is a connection to one or more devices
 *     then the Bluetooth radio will enter inquiry and/or page scan mode using
 *     the information passed in info. If the info fields are set to 0 the host
 *     will not the radio module default values are used. It is assumed that
 *     the macro defaults match the radio defaults (see BT_DEFAULT_PAGE_SCAN_WINDOW
 *     documentation.) Any call to ME_SetAccessibleModeC with info set to 0
 *     will not change the settings as the radio has already been initialized
 *     to its default settings. If the default values of the scan parameters
 *     are all set to 0, then the host will not set these values until a call
 *     to ME_SetAccessibleModeNC provides non-zero valid values.  If there are
 *     no active connections then mode and info are saved and applied when the
 *     first connection comes up.

 *     In setting the values for info. Both "inqWindow" and "inqInterval" must
 *     be set to defaults or to legitimate values. The range for values is
 *     0x0012 to 0x1000. The inqWindow must be less than or equal to the
 *     inqInterval. Likewise, the pageWindow must be less than or equal to the
 *     pageInterval. The time calculated by taking the value * 0.625ms. It is
 *     an error if one is a zero and the other is not. This is also true for
 *     "pageInterval" and "pageWindow".
 *
 *     Any time the scan interval or window is different from the current
 *     settings in the radio, the radio will be instructed to change to the
 *     new settings. This means that if there are different settings for the
 *     connected state versus the non-connected state, the radio module will be
 *     instructed to change the settings when the first connection comes up and
 *     when the last connection goes down (automatically). This also means that
 *     if different values for window and interval are set when going from any
 *     setting of accessible to non-accessible then the radio will be instructed
 *     to change. In most cases it is best to use the radio defaults. In this
 *     way the radio is never told to change the scan interval or window.
 *     To keep other devices from finding and connecting to this device when
 *     connected set the mode to BAM_NOT_ACCESSIBLE. The default mode when the
 *     stack is first loaded and initialized is controlled by
 *     BT_DEFAULT_ACCESS_MODE_C.
 *
 * Requires:
 *     BT_ALLOW_SCAN_WHILE_CON enabled.
 *
 * Parameters:
 *     mode - desired accessibility mode
 *
 *     info - pointer to structure containing the inquiry and page
 *         scan interval and window to use. If info is 0 then the
 *         defaults set by the radio module are used.
 *
 * Returns:
 *     BT_STATUS_PENDING - the mode is being set. All registered
 *         global handlers with the BEM_ACCESSIBLE_CHANGE mask set will
 *         receive BTEVENT_ACCESSIBLE_CHANGE event when the mode change
 *         actually takes affect or an error has occurred. The "errCode"
 *         field of the BtEvent indicates the status of the operation.
 *         If the operation is successful the "aMode" field of BtEvent
 *         indicates the new mode. A BTEVENT_HCI_FATAL_ERROR event
 *         indicates a fatal radio or HCI transport error and that all
 *         pending operations have failed.
 *
 *     BT_STATUS_SUCCESS - Accessible mode is set. No event
 *         is sent out. This is returned if no connections exist and
 *         the values are only saved or info already matches the current
 *         setting.
 *
 *     BT_STATUS_IN_PROGRESS - operation failed because a change
 *         is already in progress. Monitor the global events to
 *         determine when the change has taken place.
 *
 *     BT_STATUS_INVALID_PARM - operation failed. The mode or info
 *         parameter contains bad values
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_SetAccessibleModeC(BtAccessibleMode mode, const BtAccessModeInfo *info);


/*---------------------------------------------------------------------------
 * ME_GetAccessibleModeC()
 *
 *     Return the current accessibility mode that is used when
 *     connections exist. This is not necessarily the current
 *     mode.
 *
 * Requires:
 *     BT_ALLOW_SCAN_WHILE_CON enabled.
 *
 * Parameters:
 *     mode - pointer to memory to receive accessibility mode. If
 *         mode is 0 then mode is not returned. If the accessible mode
 *         has not yet been set, and therefore unknown, 0xff will be returned.
 *
 *     info - pointer to structure to receive accessibility info.
 *         If info is 0 then info is not returned.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation was successful
 *
 *     BT_STATUS_IN_PROGRESS - operation failed because a change
 *         is in progress. Monitor the global events to determine when
 *         the change is finished.
 *
 *     BT_STATUS_FAILED - operation failed.
 */
BtStatus ME_GetAccessibleModeC(BtAccessibleMode *mode, BtAccessModeInfo *info);

#endif /* BT_ALLOW_SCAN_WHILE_CON */


/*---------------------------------------------------------------------------
 * ME_SendHciCommandSync()
 *
 *     Send an HCI command synchronously. This function is used to send HCI
 *     commands not handled directly by the ME and radio specific
 *     user defined commands. Synchronously means that the command will
 *     block other HCI commands until it is complete. Use ME_SendHciCommandSync
 *     for HCI operations that return with the "Command Complete" event
 *     or local operations that do not require communication with a remote device.
 *     The "Command Status" event with an error will also cause the operation
 *     to complete. If the function returns BT_STATUS_PENDING then the result
 *     will be returned via the callback in the token with the
 *     BTEVENT_COMMAND_COMPLETE event.
 *
 *     The caller must fill in the "in" fields of the MeCommandToken.
 *     The "out" fields of the MeCommandToken will contain the result
 *     of the operation. The "errCode" field of the BtEvent is not valid
 *     since the "errCode" is part of the event and its location in "parms"
 *     varies.
 *
 * Parameters:
 *     token - pointer to token containing input parameters and
 *         storage for output parameters. The token must be initialized
 *         before calling this function as follows:
 *
 *         "token.callback" must be set to the callback function.
 *
 *         "token.p.general.in.hciCommand" must contain the desired command.
 *         The command can be one of the HCI commands defined in hci.h that
 *         is not supported by the ME or user defined commands.
 *
 *         "token.p.general.in.parmLen" must contain the length of the
 *         parameter.
 *
 *         "token.p.general.in.parms" must point to the command parameters.
 *         The memory pointed to by "parms" must be valid until the operation
 *         is complete.
 *
 *         "token.p.general.in.event" must contain the event that is expected
 *         to signal completion of the operation. The most common event is
 *         "Command Complete". See hci.h for a list of events. The "Command
 *         Status" event is always checked. If a "Command Status" event is
 *         received with an error then the operation is considered complete.
 *         If a "Command Status" is received without an error then the command
 *         will finish when the event matches "token.p.general.in.event".
 *
 * Returns:
 *     BT_STATUS_PENDING - Operation is started and results will
 *         be sent to the callback. A BTEVENT_HCI_FATAL_ERROR event
 *         indicates a fatal radio or HCI transport error and all
 *         pending operations have failed.
 *
 *     BT_STATUS_INVALID_TYPE - Operation failed because
 *         "token.p.general.in.hciCommand" is a command already handled by
 *         the ME or "token.p.general.in.event" is an asynchronous event
 *         already handled by the ME.
 *
 *     BT_STATUS_INVALID_PARM - Operation failed because the parameter
 *         length exceeds HCI_CMD_PARM_LEN, the maximum allowed
 *         parameter size for an HCI command.
 *
 *     BT_STATUS_FAILED - operation failed because the token was
 *         not properly initialized or already active (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_SendHciCommandSync(MeCommandToken *token);

/*---------------------------------------------------------------------------
 * ME_SendHciCommandAsync()
 *
 *     Send an HCI command asynchronously. This function is used to send HCI
 *     commands not implemented explicitly by the ME and radio specific
 *     user defined commands. Asynchronously means that the command will not
 *     block other HCI commands. It will wait for a "Command Status" event
 *     but then will allow other HCI commands to execute as it waits for
 *     the asynchronous event signaling the completion of the operation.
 *     The result will be returned when the appropriate event as set in
 *     "token.p.general.in.event" is received. If the function returns
 *     BT_STATUS_PENDING then the result will be  returned via the callback
 *     in the token with the BTEVENT_COMMAND_COMPLETE event.
 *
 *     The caller must fill in the "in" fields of the MeCommandToken.
 *     The "out" fields of the MeCommandToken will contain the result
 *     of the operation. The "errCode" field of the BtEvent is not valid
 *     since the "errCode" is part of the event and its location in "parms"
 *     varies.
 *
 *     Only one Async operation with the same event is allowed to
 *     execute at a time. Any additional requests are queued until the
 *     first command is complete.
 *
 * Parameters:
 *     token - pointer to token containing input parameters and
 *         storage for output parameters. The token must be initialized
 *         before calling this function as follows:
 *
 *         "token.callback" must be set to the callback function.
 *
 *         "token.p.general.in.hciCommand" must contain the desired command.
 *         The command can be one of the HCI commands defined in hci.h that
 *         is not supported by the ME or user defined commands.
 *
 *         "token.p.general.in.parmLen" must contain the length of the
 *         parameter.
 *
 *         "token.p.general.in.parms" must point to the command parameters.
 *         The memory pointed to by "parms" must be valid until the operation
 *         is complete.
 *
 *         "token.p.general.in.event" must contain the event that is expected
 *         to signal completion of the operation. The "Command Status" event
 *         is always checked. If a "Command Status" event is returned
 *         with an error then the operation is considered complete. If a
 *         "Command Status" is returned without an error then the command
 *         will finish when the event matches "token.p.general.in.event".
 *
 * Returns:
 *     BT_STATUS_PENDING - Operation is started and results will
 *         be sent to the callback. A BTEVENT_HCI_FATAL_ERROR event
 *         indicates a fatal radio or HCI transport error and all
 *         pending operations have failed.
 *
 *     BT_STATUS_INVALID_TYPE - Operation failed because
 *         "token.p.general.in.hciCommand" is a command already handled by
 *         the ME or "token.p.general.in.event" is an asynchronous event
 *         already handled by the ME.
 *
 *     BT_STATUS_INVALID_PARM - Operation failed because the parameter
 *         length exceeds HCI_CMD_PARM_LEN, the maximum allowed
 *         parameter size for an HCI command.
 *
 *     BT_STATUS_FAILED - operation failed because the token was
 *         not properly initialized or already active (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_SendHciCommandAsync(MeCommandToken *token);

/*---------------------------------------------------------------------------
 * ME_SendHciCommandVendorSpecific()
 *
 *     Send a vendor specific HCI command. Many Bluetooth controller
 *     vendors use the HCI to send commands that are not specified by
 *     the Bluetooth SIG.  Often, the controller manufacturers use the
 *     HCI in unconventional ways that violate the command/response flow
 *     control mechanisms in the Host Controller specification.  This
 *     function allows an application to send an HCI command to the
 *     controller, without the expectation of a response from the controller.
 *     Commands are also sent outside the normal flow control mechanism.
 *     Commands will always be transmitted to the controller immediately.
 * 
 *     This function can only be used for vendor specific data, and
 *     cannot not be used for any HCI commands specified by the
 *     Bluetooth SIG.
 * 
 *     Even though, when using this function, there is no expectation of
 *     a response, it is possible to receive responses to commands, because
 *     all vendor specific events are sent to the global handler. When
 *     using this function the flow of commands and events must be managed
 *     by the application.
 * 
 *     For many applications, where the vendor specific commands require
 *     responses, it is easier to let ME manage the flow of commands and
 *     responses, by using ME_SendHciCommandSync() and
 *     ME_SendHciCommandAsync(), especially in cases where the vendor
 *     specific commands/events follow HCI conventions.
 * 
 *     It is very important to understand the mechanisms of the vendor
 *     specific commands/events before using this function or the other
 *     functions that are capable of sending vendor specific commands.
 *     Please review and understand the documentation provided with the
 *     controller before attempting to use these commands.
 *
 * Parameters:
 * 
 *     commandType - The vendor specific command type to send.
 * 
 *     parmLen - The length of the parameters to send.
 * 
 *     hciCmd - The HciCommand structure that will be used to send
 *         the command.  The memory must be available until
 *         BTEVENT_HCI_COMMAND_SENT is received.
 *
 * Returns:
 *     BT_STATUS_PENDING - Operation is started and an event
 *         will be sent to the global handler when the command has been
 *         sent (see BTEVENT_HCI_COMMAND_SENT).
 *
 *     BT_STATUS_INVALID_TYPE - Operation failed because
 *         "commandType" is not a vendor specific command.
 *
 *     BT_STATUS_INVALID_PARM - Operation failed because the parameter
 *         length exceeds HCI_CMD_PARM_LEN, the maximum allowed
 *         parameter size for an HCI command, or the 'hciCmd' parameter
 *         is not initialized (XA_ERROR_CHECK only).
 * 
 *     BT_STATUS_IN_USE - The command is already queued to be sent.
 *
 *     BT_STATUS_FAILED - Operation failed because the HCI is not
 *         in the proper state to send a command.
 */
BtStatus ME_SendHciCommandVendorSpecific(U16 commandType, 
                                         U8 parmLen, 
                                         HciCommand *hciCmd);

/*---------------------------------------------------------------------------
 * ME_SetDefaultLinkPolicy()
 *
 *     Set the default link policy used on outgoing and incoming ACL
 *     connections. The link policy determines the behavior of the local
 *     link manager when it receives a request from a remote device or it
 *     determines itself to change the master-slave role or to enter hold,
 *     sniff, or park mode. The default if this function is not called
 *     is to disable all modes.
 *
 *     Policy for incoming ACL connections is set independent of policy
 *     for outgoing connections. The default policy is applied immediately
 *     after the link comes up. An higher level Management Entity should
 *     use this function, not applications. It should be called before any
 *     ACL connections exist and does not effect existing
 *     ACL connections.
 *
 *     If this function has not been called, the default link policy applied
 *     to both incoming and outgoing connections is BLP_DISABLE_ALL.
 *
 * Parameters:
 *     inACL - default link policy applied to all incoming ACL connections.
 *         Incoming ACL connections are those initiated by a remote device.
 *
 *     outACL - default link policy applied to all outgoing ACL connections.
 *         Outgoing ACL connections are those initiated by the local device.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation was successful.
 *
 *     BT_STATUS_FAILED - operation failed because the policy settings are
 *         not valid (error checking only).
 */
BtStatus ME_SetDefaultLinkPolicy(BtLinkPolicy inACL, BtLinkPolicy outACL);

/*---------------------------------------------------------------------------
 * ME_SetConnectionPacketType()
 *
 *     Changes the allowed ACL packet types for the specified remote device.
 *     The result of the operation will be returned via the
 *     BTEVENT_CONN_PACKET_TYPE_CHNG event.
 *
 * Parameters:
 *     remDev - pointer to remote device
 *
 *     packetTypes - allowed connection packet types.
 *
 * Returns:
  *     BT_STATUS_PENDING - the operation was started successfully.
 *         All registered handlers and handlers bound to the remote device
 *         will receive the BTEVENT_CONN_PACKET_TYPE_CHNG event. The
 *         "errCode" field of the BtEvent will indicate the success or
 *         failure of the connection packet type change event. The
 *         "p.packetTypeChng" field indicates for which remote
 *         Device the change has occurred along with the packet types.
 *         It is possible that link is disconnected before the connection
 *         packet type change has occurred so the handlers may not receive
 *         BTEVENT_CONN_PACKET_TYPE_CHNG  but instead will receive
 *         BTEVENT_LINK_DISCONNECT.
 *
 *     BT_STATUS_INVALID_PARM - The parameters are not valid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because another
 *         operation is already in progress.
 *
 */
BtStatus ME_SetConnectionPacketType(BtRemoteDevice *remDev,
                                    BtAclPacketType packetTypes);

/*---------------------------------------------------------------------------
 * ME_GetHciConnectionHandle()
 *
 *     Get the HCI connection handle for the link to the remote device. The
 *     value is meaningless if a connection does not exist to the remote
 *     device.
 *
 * Parameters:
 *     rm - pointer to remote device.
 *
 * Returns:
 *     The HCI connection handle a link exists otherwise the value
 *     is meaningless.
 */
U16 ME_GetHciConnectionHandle(BtRemoteDevice *rm);

/*---------------------------------------------------------------------------
 * ME_GetBdAddr()
 *
 *     Get the 48-bit address of the remote device. The value is meaningless
 *     if a connection does not exist to the remote device.
 *
 * Parameters:
 *     rm - pointer to remote device.
 *
 *     bdAddr - pointer to a BD_ADDR structure to receive the result.
 */
void ME_GetBdAddr(BtRemoteDevice *rm, BD_ADDR *bdAddr);

/*---------------------------------------------------------------------------
 * ME_GetDevAddr()
 *
 *     Get the 48-bit address plus type of the remote device. The value is
 *     meaningless if a connection does not exist to the remote device.
 *
 * Parameters:
 *     rm - pointer to remote device.
 *
 *     devAddr - pointer to a BtDevAddr structure to receive the result.
 */
void ME_GetDevAddr(BtRemoteDevice *rm, BtDevAddr *devAddr);

/*---------------------------------------------------------------------------
 * ME_GetRemDevState()
 *
 *     Get the connection state of the remote device. This function
 *     does not check for a valid remDev pointer.
 *
 * Parameters:
 *     remDev - pointer to remote device.
 *
 * Returns:
 *     BtRemDevState
 */
BtRemDevState ME_GetRemDevState(BtRemoteDevice *remDev);
#define ME_GetRemDevState(r) ((r)->state)

/*---------------------------------------------------------------------------
 * ME_GetRemDevCOD()
 *
 *     Get the Class of Device for the remote device. This function
 *     does not check for a valid remDev pointer.
 *
 * Parameters:
 *     remDev - pointer to remDev.
 *
 * Returns:
 *     BtClassOfDevice
 */
BtClassOfDevice ME_GetRemDevCOD(BtRemoteDevice *remDev);
#define ME_GetRemDevCOD(r) (((BrEdrRemoteDevice *)r)->cod)

/*---------------------------------------------------------------------------
 * ME_GetRemDevEncryptState()
 *
 *     Get the encryption state of the remote device. This function
 *     does not check for a valid remDev pointer.
 *
 * Parameters:
 *     remDev - pointer to remDev.
 *
 * Returns:
 *     Zero for no encryption, non-zero for encryption.
 */
BtEncryptState ME_GetRemDevEncryptState(BtRemoteDevice *remDev);
#define ME_GetRemDevEncryptState(r) (((BrEdrRemoteDevice *)r)->encryptState)

/*---------------------------------------------------------------------------
 * ME_GetStackInitState()
 *
 *     Reads the initialization state of the stack.
 *
 * Returns:
 *     BtStackState - If the value is BTSS_INITIALIZED the stack is
 *         initialized and ready.
 */
BtStackState ME_GetStackInitState(void);

/*---------------------------------------------------------------------------
 * ME_GetBtVersion()
 *
 *     Gets the Bluetooth version of the local radio.
 *
 * Returns:
 *     0 for BT v1.0.
 *     1 for BT v1.1.
 *     2 for BT v1.2.
 *     3 for BT v2.0.
 *     4 for BT v2.1.
 *     5 for BT v3.0.
 *     6 for BT v4.0.
 *     7 for BT v4.1.
 *     8 for BT v4.2.
 */
U8 ME_GetBtVersion(void);

/*---------------------------------------------------------------------------
 * ME_GetBtVersionInfo()
 *
 *     Gets the value of the specified byte of the Bluetooth verion info
 *     from the local radio. See the Bluetooth specification for a description
 *     of the version (Read_Local_Version).
 *
 * Parameters:
 *     versionInfo - pointer to the version information that will be assigned
 *         by this function.
 *
 * Returns:
 *     Pointer to BT version Info.
 */
void ME_GetBtVersionInfo(BtVersionInfo* versionInfo);

/*---------------------------------------------------------------------------
 * ME_GetBtFeatures()
 *
 *     Gets the value of the specified byte of the Bluetooth features
 *     bitmask from the local radio. See the Bluetooth specification for
 *     a description of the features bitmask.
 *
 * Parameters:
 *     byte - byte to retrieve. Must be between 0 and 7.
 *
 * Returns:
 *     Value of the specified byte of the BT features bitmask.
 */
U8 ME_GetBtFeatures(U8 byte);

/*---------------------------------------------------------------------------
 * ME_GetRemoteFeatures()
 *
 *     Gets the value of the specified byte of the Bluetooth features
 *     bitmask from the remote device. See the Bluetooth specification for
 *     a description of the features bitmask.
 *
 * Parameters:
 *     remDev - pointer to remDev.
 *
 *     byte - byte to retrieve. Must be between 0 and 7.
 *
 * Returns:
 *     Value of the specified byte of the remote features bitmask.
 */
U8 ME_GetRemoteFeatures(BtRemoteDevice *remDev, U8 byte);

/*---------------------------------------------------------------------------
 * ME_GetRemoteHostFeatures()
 *
 *     Gets the value of the specified byte of page 1 or page 2 of the Bluetooth
 *     extended features bitmask from the remote device. See the Bluetooth
 *     specification for a description of the extended features bitmask.
 *
 * Parameters:
 *     remDev - pointer to remDev.
 *
 *     page - page of extended features to retrieve. Must be 1 or 2.
 *
 *     byte - byte to retrieve. Must be between 0 and 7.
 *
 * Returns:
 *     Value of the specified byte of the remote extended features bitmask.
 */
U8 ME_GetRemoteHostFeatures(BtRemoteDevice *remDev, U8 page, U8 byte);

/*---------------------------------------------------------------------------
 * ME_GetHciSupportedCommands()
 *
 *     Gets the array of local supported HCI Commands. See Table 6.27 of the
 *     Bluetooth specification, vol 2, Part E for the definition of the bitmask.
 *
 * Parameters:
 *     hciSupportedCommands - pointer to the 64 bytes detailing each supported
 *         HCI command
 *
 * Returns:
 *     Pointer to the array of local supported HCI Commands.
 */
void ME_GetHciSupportedCommands(U8 *hciSupportedCommands);

/*---------------------------------------------------------------------------
 * ME_GetHciSupportedCommandByte()
 * 
 *     Return the byte from the local supported HCI Commands array. See Table
 *     6.27 of the Bluetooth specification, vol 2, Part E for the definition
 *     of the bitmask. The array is 64 octets in size.
 *
 * Parameters:
 *     byte - byte to retreive (0 - 63)
 *
 * Returns:
 *     Value of the specified byte of the local suported HCI command array.
 */
U8 ME_GetHciSupportedCommandByte(U8 byte);

/*---------------------------------------------------------------------------
 * ME_GetLocalSupportedCodecs()
 *
 *     Gets the array of local supported Codecs.
 *
 * Parameters:
 *     hciSupportedCommands - pointer to the list of supported codecs
 *
 * Returns:
 *     Number of local supported Codecs.
 */
U8 ME_GetLocalSupportedCodecs(U8 *localSupportedCodecs);

/*---------------------------------------------------------------------------
 * ME_CheckRadioFeature()
 *
 *     Indicates whether the local radio supports the specified feature.
 *     This function also provides support for extended features.
 *
 * Parameters:
 *     feature - Feature code
 *
 * Returns:
 *     TRUE if radio indicates its support for the feature. FALSE if not.
 *
 */
BOOL ME_CheckRadioFeature(BtRadioFeature feature);

/*---------------------------------------------------------------------------
 * ME_CheckRemoteFeature()
 *
 *     Indicates whether the remote device supports the specified feature. This
 *     function works for all features including the extended features.
 *
 * Parameters:
 *     remDev - pointer to remDev.
 *
 *     feature - Feature code
 *
 * Returns:
 *     TRUE if remote device indicates its support for the feature. FALSE if not.
 *
 */
BOOL ME_CheckRemoteFeature(BtRemoteDevice *remDev, BtRadioFeature feature);

/*---------------------------------------------------------------------------
 * ME_CheckRemoteHostFeature()
 *
 *     Indicates whether the remote host supports the specified feature. This
 *     function should be used. Instead use ME_CheckRemoteFeature() which
 *     covers all remote features bits.
 *
 * Parameters:
 *     remDev - pointer to remDev.
 *
 *     feature - Feature code
 *
 * Returns:
 *     TRUE if remote host indicates its support for the feature. FALSE if not.
 *
 */
BOOL ME_CheckRemoteHostFeature(BtRemoteDevice *remDev, BtRadioFeature feature);

/*---------------------------------------------------------------------------
 * ME_HasPendingTask()
 *
 *     Checks for pending ME tasks. The ME task is set when an HCI command
 *     is sent to the controller. ME_HasPendingTask will return the event
 *     expected to complete the pending HCI command.
 *
 * Returns:
 *     Pending HCI event associated with the completion of the current
 *     task or 0 for no pending tasks. See HciEventType in hci.h for possible
 *     return values. If not enough information is available to determine the
 *     completion event, ME_HasPendingTask returns 0xFF00.
 */
U16 ME_HasPendingTask(void);

/*---------------------------------------------------------------------------
 * ME_GetChannelClassification()
 *
 *     Gets the channel classification currently stored in ME for Adaptive
 *     Frequency Hopping (AFH.) See ME_SetChannelClassification.
 *
 * Parameters:
 *     channelClass - BtChannelClass structure containing buffer to receive
 *         the stored channel classification.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The current channel classification has been
 *         copied to the BtChannelClass structure.
 */
BtStatus ME_GetChannelClassification(BtChannelClass *channelClass);

/*---------------------------------------------------------------------------
 * ME_SetChannelClassification()
 *
 *     Sets the channel classification for Adaptive Frequency Hopping (AFH.)
 *     Each of the 79 1-MHz channels in the Bluetooth 2.4-GHz band may be
 *     marked bad or unknown. The AFH controller will not use channels
 *     marked bad. The AFH controller will evaluate for use any channels
 *     marked unknown (if automatic classification mode is enabled.) Bad
 *     channels are indicated with a 0 value. Unknown channels are
 *     indicated with a 1 value in the bit-mask.
 *
 * Parameters:
 *     channelClass - 79 1-bit fields in a 10-byte array, indicating channels
 *         marked bad or unknown. The nth field (0 - 78) indicates the value
 *         for channel n. Bit 79 is unused and must be 0.
 *
 *     autoMode - Boolean value indicating whether the controller should
 *         automatically determine channel classification for the channels
 *         marked unknown.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - the channel classification has been sent to the
 *         controller.
 *
 *     BT_STATUS_IN_PROGRESS - another ME_SetChannelClassification operation
 *         is currently in progress. Try again later.
 *
 *     BT_STATUS_INVALID_PARM - The parameters are not valid
 *         (XA_ERROR_CHECK only).
 */
BtStatus ME_SetChannelClassification(BtChannelClass *channelClass, BOOL autoMode);


/*---------------------------------------------------------------------------
 * ME_MarkAfhChannels()
 *
 *     Sets or clears the bits associated with the indicated range of
 *     channels in the BtChannelClass map. If the "initialize" parameter is
 *     TRUE, also initializes the BtChannelClass map to the "Unknown" state.
 *
 * Parameters:
 *     channelClass - BtChannelClass structure containing buffer to be
 *         modified.
 *
 *     begin - the first channel (0 - 78) in the range to set.
 *
 *     count - the number of channels to set starting at "begin".
 *
 *     state - If TRUE, the Bluetooth channel will be set to the "Unknown"
 *         state. If FALSE, the Bluetooth channel will be set to the "Bad"
 *         state.
 *
 *     initialize - If TRUE, the BtChannelClass structure will be initialized
 *         to the "Unknown" state. If FALSE, the structure will be left
 *         as is.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The channels have been marked successfully.
 */
BtStatus ME_MarkAfhChannels(BtChannelClass *channelClass, U8 begin, U8 count,
                            BOOL state, BOOL initialize);

/*---------------------------------------------------------------------------
 * ME_MarkWiFiChannel()
 *
 *     Sets or clears the bits associated with the indicated WiFi channel in
 *     the BtChannelClass map. If the "initialize" parameter is TRUE, also
 *     initializes the BtChannelClass map to the "Unknown" state.
 *
 * Parameters:
 *     channelClass - BtChannelClass structure containing buffer to be
 *         modified.
 *
 *     wifiChannel - WiFi channel (1-14) to set.
 *
 *     state - If TRUE, the WiFi channel will be set to the "Unknown"
 *         state. If FALSE, the WiFi channel will be set to the "Bad"
 *         state.
 *
 *     initialize - If TRUE, the BtChannelClass structure will be initialized
 *         to the "Unknown" state and then set as indicated. If FALSE, the
 *         structure will be modified as is.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The channels have been marked successfully.
 */
BtStatus ME_MarkWiFiChannel(BtChannelClass *channelClass, U8 wifiChannel,
                            BOOL state, BOOL initialize);

/*---------------------------------------------------------------------------
 * ME_RadioInit()
 *
 *     Called to initialize the radio module, including the HCI
 *     transport drivers, the HCI, and the radio module.  This function is
 *     called by the Event Manager in EVM_Init() when the stack initialization
 *     sequence begins, but can be called by applications afterwards if
 *     necessary (ME_RadioShutdown() has been called).  When initialization
 *     is successful, the BTEVENT_HCI_INITIALIZED event will be received.
 *
 * Parameters:  void
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Initialization was successful.  The application will
 *         be notified during the call that initialization is complete with
 *         BTEVENT_HCI_INITIALIZED.

 *     BT_STATUS_PENDING - Initialization was started successfully.  The
 *         application will be notified when initialization is complete.  If
 *         initialization is successful, BTEVENT_HCI_INITIALIZED will be
 *         received.  If initialization fails, BTEVENT_HCI_INIT_ERROR will be
 *         received.  Initialization will be retried BT_HCI_NUM_INIT_RETRIES
 *         times when an error occurs.  After initialization has been retried
 *         and has failed the maximum number of times, BTEVENT_HCI_FAILED will
 *         be received.

 *     BT_STATUS_FAILED - Initialization failed to start.
 */
BtStatus ME_RadioInit(void);
#define ME_RadioInit() RMGR_RadioInit()

/*---------------------------------------------------------------------------
 * ME_RadioShutdown()
 *
 *     Gracefully shuts down the radio module, including the transports and
 *     the HCI.  ME_RadioShutdown will wait 1 second for pending operations
 *     before shutting down.  (Note: Use ME_RadioShutdownTime if you need to
 *     specify the wait time.)  When the shutdown is complete, the global
 *     handler will receive a BTEVENT_HCI_DEINITIALIZED event.
 *
 *     This function must NOT be called during a stack callback.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The radio is down. The application will be notified
 *         during the call that the shutdown was complete with
 *         BTEVENT_HCI_DEINITIALIZED.
 *     BT_STATUS_FAILED - The radio is already down.
 *     BT_STATUS_PENDING - The radio will be shut down. The application will
 *         be notified when shutdown is complete with BTEVENT_HCI_DEINITIALIZED.
 */
BtStatus ME_RadioShutdown(void);
BtStatus ME_RadioShutdownTime(TimeT time);
#define ME_RadioShutdown() ME_RadioShutdownTime(1000)

/*---------------------------------------------------------------------------
 * ME_GetHciFlowParms()
 *
 *     Called to retrieve the negotiated flow control parameters of the
 *     HCI transport.  This information may be useful for upper layers that
 *     wish to optimize the transmission of data in BtPackets.  It is especially
 *     useful for the transmission of SCO/eSCO data, which should be sent in
 *     packets equal to the size advertised by the radio.
 *
 * Parameters:
 *     Parms - A pointer to a memory structure that will receive the flow
 *         control information.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The request was successful
 *
 *     BT_STATUS_FAILED - The request failed because the parameters have
 *         not yet been negotiated.
 */
BtStatus ME_GetHciFlowParms(MeHciFlowParms *Parms);

/*---------------------------------------------------------------------------
 * ME_SetHciScoPacketSize()
 *
 *     Called to change the negotiated SCO packet size.  The ability
 *     to change the SCO packet size is important when BT_SCO_HCI_DATA
 *     is enabled and the transport is USB.  USB has very strict
 *     requirements for the packet size, and often radios do not
 *     advertise the appropriate size.  In addition, the size should
 *     be changed if multiple SCO channels are used.  This function should
 *     only be called if the HCI has completed initialization.
 *
 *     Example USB packet sizes:
 *
 *     16 bit audio:  Each SCO channel requires 48 byte packets.
 *     8  bit audio:  Each SCO channel requires 24 byte packets.
 *
 * Parameters:
 *     scoPacketLen - The new SCO packet size.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The request was successful
 *
 *     BT_STATUS_FAILED - The request failed because the parameters have
 *                        not yet been negotiated.
 */
BtStatus ME_SetHciScoPacketSize(U8 scoPacketLen);

/*---------------------------------------------------------------------------
 * ME_IsOurBdAddrLarger()
 *
 *     Return TRUE if the local BD_ADDR is larger than the remote device's
 *     BD_ADDR based on the collision resolution algorithm in the L2CAP
 *     and A2MP specifications otherwise return FALSE.
 *
 * Parameters:
 *     remDev - pointer to remote device structure
 *
 * Returns:
 *     TRUE - local BD_ADDR is larger
 *
 *     FALSE - remote BD_ADDR is larger
 */
BOOL ME_IsLocalBdAddrLarger(BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * ME_RequestEIRUpdate()
 *
 *  Schedule a task to indicate the necessity of an EIR update to the global
 *  handler.  This is typically invoked internally in the stack components
 *  when EIR associated values have been changed.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     void
 */
void ME_RequestEIRUpdate(void);


#if JETTEST == XA_ENABLED
void ME_SetClearRemoteFeature(BtRemoteDevice *remDev, BtRadioFeature feature, BOOL clear);
#endif
#endif /* __ME_H */

