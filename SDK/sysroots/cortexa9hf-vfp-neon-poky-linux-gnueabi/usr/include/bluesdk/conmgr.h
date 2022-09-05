/****************************************************************************
 *
 * File:
 *     conmgr.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file contains public definitions for the Connection Manager.
 *
 * Created:
 *     June 6, 2003
 *
 * Copyright 2003-2017 OpenSynergy GmbH.
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

#ifndef __CONMGR_H_
#define __CONMGR_H_

#include "medev.h"
#if NUM_SCO_CONNS > 0
#include "mesco.h"
#endif /* NUM_SCO_CONNS > 0 */

/* Check for a known-Lisbon define; if not present; fall back to 2.x */
#ifndef BSE_NO_ERROR
#define BT_STACK_VERSION 220
#else
#ifndef BT_STACK_VERSION
#define BT_STACK_VERSION 300
#endif /* BT_STACK_VERSION */
#endif /* BSE_NO_ERROR */


/*---------------------------------------------------------------------------
 * Connection Manager API layer
 *
 *     The Connection Manager provides a set of functions for managing
 *     BR/EDR/LE ACL links.  Profiles make calls to the
 *     Connection Manager when they need a resource.  It is up to the
 *     Connection Manager to decide how to  best fulfill those requests.
 *
 *     Adhering to the API described below, it is possible to replace the
 *     Connection Manager included in any profile.  This may be desirable if
 *     a central connection management layer is required to manage resources
 *     between multiple profiles running on the same device.  This is
 *     highly dependent on system architecture, but the API is provided here
 *     in the event that it proves useful.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * CMGR_DEFAULT_SNIFF_EXIT_POLICY constant
 *     Defines the default behavior for exiting sniff mode.  The default
 *     value causes the device to exit sniff mode whenever an audio link
 *     is initiated.
 */
#ifndef CMGR_DEFAULT_SNIFF_EXIT_POLICY
#define CMGR_DEFAULT_SNIFF_EXIT_POLICY CMGR_SNIFF_EXIT_ON_AUDIO
#endif

/*---------------------------------------------------------------------------
 * CMGR_DEFAULT_SNIFF_TIMER constant
 *     Defines the default sniff timer for all handlers. See
 *     CMGR_SetSniffTimer for information on how the timer value is used.
 */
#ifndef CMGR_DEFAULT_SNIFF_TIMER
#define CMGR_DEFAULT_SNIFF_TIMER CMGR_SNIFF_DONT_CARE
#endif

/*---------------------------------------------------------------------------
 * CMGR_AUDIO_DEFAULT_PARMS constant
 *     Defines the highest audio connection parameters for an audio link.  See
 *     (CmgrAudioParms) for valid values.  If the defined settings are not
 *     supported by the radio module, the highest possible settings will
 *     be used.  For example, Bluetooth 1.1 radios will only support
 *     CMGR_AUDIO_PARMS_SCO and Bluetooth 1.2 radios will support
 *     CMGR_AUDIO_PARMS_S1 and CMGR_AUDIO_PARMS_SCO.  All parameter types
 *     are supported by Bluetooth 2.0 radios.
 */
#ifndef CMGR_AUDIO_DEFAULT_PARMS
#define CMGR_AUDIO_DEFAULT_PARMS CMGR_AUDIO_PARMS_S4
#endif

/*---------------------------------------------------------------------------
 * CMGR_MEMORY_EXTERNAL constant
 *     Memory is allocated externally.  When this constant is defined as
 *     XA_ENABLED, global context memory is not allocated by the Connection
 *     Manager code, and therefore must be allocated external to this module.
 *     If memory is allocated external to this module, if must be allocated
 *     using the context variable type defined in conmgr.h, and the entire
 *     structure must be initialized to 0. In addition, the macro CMGR() must
 *     be defined so that the context memory can be accessed local to this module.
 */
#ifndef CMGR_MEMORY_EXTERNAL
#define CMGR_MEMORY_EXTERNAL XA_DISABLED
#endif


/*---------------------------------------------------------------------------
 * CMGR_SNIFF_MIN_INTERVAL constant
 *     Defines the default minimum interval for sniff mode. This interval
 *     can be overridden with CMGR_SetDefaultSniffInfo().
 *
 */
#ifndef CMGR_SNIFF_MIN_INTERVAL
#define CMGR_SNIFF_MIN_INTERVAL 800
#endif

/*---------------------------------------------------------------------------
 * CMGR_SNIFF_MAX_INTERVAL constant
 *     Maximum interval for sniff mode if enabled (see CMGR_SetSniffTimer).
 */
#ifndef CMGR_SNIFF_MAX_INTERVAL
#define CMGR_SNIFF_MAX_INTERVAL 8000
#endif

/*---------------------------------------------------------------------------
 * CMGR_SNIFF_ATTEMPT constant
 *     Sniff attempts for sniff mode if enabled (see CMGR_SetSniffTimer).
 */
#ifndef CMGR_SNIFF_ATTEMPT
#define CMGR_SNIFF_ATTEMPT 1600
#endif

/*---------------------------------------------------------------------------
 * CMGR_SNIFF_TIMEOUT constant
 *     Sniff timeout for sniff mode if enabled (see CMGR_SetSniffTimer).
 */
#ifndef CMGR_SNIFF_TIMEOUT
#define CMGR_SNIFF_TIMEOUT 1600
#endif

/*---------------------------------------------------------------------------
 * CMGR_TIME_INIFINTY constant
 *     Use this for LE timers to indicate the operation should run until
 *     complete or canceled.
 */
#define CMGR_TIME_INIFINTY 0xFFFFFFFF

/* Link State Flags */

/* === Internal use only === */
#define CMGR_FLAG_AUDIO_UP              0x0001
#define CMGR_FLAG_SCO_REGISTERED        0x0002
#define CMGR_FLAG_DELAYING_SCO          0x0004
#define CMGR_FLAG_STAY_MASTER           0x0008
#define CMGR_FLAG_HANDLE_SCO            0x0010
#define CMGR_FLAG_SELECTING             0x0020
#define CMGR_FLAG_USE_SNIFF_INFO        0x0040
#define CMGR_FLAG_SEND_CON_CNF_DISASSOC 0x0080
#define CMGR_FLAG_STOPPING_SNIFF_MODE   0x0100
#define CMGR_FLAG_CON_CNF_CALLED        0x0200

/* Sniff time values */

#define CMGR_SNIFF_DISABLED  (0xFFFFFFFF)
#define CMGR_SNIFF_DONT_CARE  (0)

/* Audio flags */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * CmgrEvent type
 *
 *     All indications and confirmations are sent through a callback function.
 *     The profile registers with the Connection Manager at initialization.  Calls
 *     are made to request ACL and SCO resources.  The Connection Manager calls
 *     the profile's callback function to indicate the results of those requests.
 */
typedef U8 CmgrEvent;

/* An outgoing ACL connection request has been completed.  The 'Status'
 *  parameter of the callback indicates whether it is successful or
 *  unsuccessful.
 *
 *  The 'CmgrHandler.bdc' field contains a pointer to the device context
 *  if the connection was successful.  Disassociated handlers 
 *  which have been enabled via CMGR_EnableConCnfEventForDisassoc() will 
 *  receive this event if the connection is successful or fails. 
 * 
 *  In this case the 'CmgrHandler.remDev' field contains a pointer to the remote 
 *  device.
 */
#define CMEVENT_DATA_LINK_CON_CNF       1

/* A remote device has established an ACL connection with this device.
 *
 *  The 'CmgrHandler.remDev' field contains a pointer to the remote device.
 *  All disassociated handlers receive this event.
 */
#define CMEVENT_DATA_LINK_CON_IND       2

/* The ACL link has been disconnected. If the current handler was associated
 * with the link, it is now disassociated.
 *
 * The 'Status' field indicates the success of the disconnect.
 * The 'CmgrHandler.remDev' field contains a pointer to the remote device.
 * The 'CmgrHandler.error' field contains the disconnect reason.
 *
 * All disassociated handlers receive this event.
 */
#define CMEVENT_DATA_LINK_DIS           3

/* The SCO connection has been requested from the remote device.
 *  The 'CmgrHandler.bdc' field contains a pointer to the device
 *  context.  The application must call CMGR_AcceptAudioLink() to
 *  accept the connection.  All SCO handlers that were registered via
 *  CMGR_RegisterAudioRequestHandler() receive this event.  SCO handlers
 *  that were registered via CMGR_RegisterAudioHandler() do not receive
 *  this event because Connection Manager will handle this event
 *  internally and accept the connection automatically in that case.
 */
#define CMEVENT_AUDIO_LINK_REQ          4

/* The SCO connection has come up.  The 'Status' parameter indicates whether
 *  the connection was successful or not.  The 'CmgrHandler.bdc' field contains
 *  a pointer to the device context.
 */
#define CMEVENT_AUDIO_LINK_CON          5

/* The SCO link has been disconnected.  The 'Status' field should indicate
 *  success.  The 'CmgrHandler.bdc' field contains a pointer to the device
 *  context.  The 'CmgrHandler.error' field contains the disconnect reason.
 */
#define CMEVENT_AUDIO_LINK_DIS          6

/* Audio data has been received on the specified SCO link
 *  The 'CmgrHandler.bdc' field contains a pointer to the device and the
 *  'CmgrHandler.audioData' field contains a pointer to the received data.
 *  All registered SCO handlers receive this event.
 */
#define CMEVENT_AUDIO_DATA              7

/* Audio data has been sent on the specified SCO link
 *  The 'CmgrHandler.bdc' field contains a pointer to the device and the
 *  'CmgrHandler.audioData' field contains a pointer to the packet.
 *  All registered SCO handlers receive this event.
 */
#define CMEVENT_AUDIO_DATA_SENT         8

/* Sniff mode entered.
 */
#define CMEVENT_ENTER_SNIFF_MODE        9

/* Sniff or Park mode exited.
 */
#define CMEVENT_EXIT_SNIFF_PARK_MODE    10

#if NUM_LE_DEVICES > 0
/* LE Connection parameters have been set.
 */
#define CMEVENT_LE_CON_PARMS_SET        11

/* LE Adverting report has been received. This event is sent as a 
 *  CmgrLeScanEvent (based on BtEventEx) when passed to the callback function
 *  of a CmgrLeScanHandler. The "bcd" and "addReport" fields of the 
 *  CmgrLeScanEvent are valid during the callback
 */
#define CMEVENT_LE_REPORT               12

/* LE Scanning has started. This event is sent as a CmgrLeScanEvent event 
 * (based on BtEventEx) when passed to the callback function of a CmgrLeScanHandler. 
 * The 'e.status' field will be set to BT_STATUS_SUCCESS. Note that scanning
 * may be temporarily stopped due to enabling/disabling privacy or initiating a
 * a connection and then restarted. This event will be sent when scanning is
 * restarted.  
 */
#define CMEVENT_LE_SCAN_STARTED         13

/* LE Scanning has stopped. This event is sent as a CmgrLeScanEvent event 
 *  (based on BtEventEx) when passed to the callback function of a CmgrLeScanHandler.
 *  If 'e'status' is set to BT_STATUS_TIMEOUT then scanning has stopped because of
 *  a timeout and the handler has been disassociated from the Connection Manager. 
 *  Calling CMGR_StartLeScan() is allowed during the callback. If scanning is stopped 
 *  temporarily because privacy is being enabled/disabled or a connection
 *  has been started or the "first" timer expired and the parameters are being
 *  changed then the 'e.status' field will be set to BT_STATUS_PENDING and
 *  the handler is not removed since scanning will be restarted automatically. 
 */
#define CMEVENT_LE_SCAN_STOPPED         14

/* An LE Controller error has occurred during operation. All scan handlers
 *  advertising handlers, and connection requests will be terminated. Existing
 *  connections will not be disconnected. 
 * 
 *  There are three possible cases where this event is sent. The first case is when 
 *  the LE controller sends an error event to the stack. In this case the "errCode" 
 *  field in the CmgrHandler, CmgrLeScanEvent, or CmgrLeAdvertEvent will contain the 
 *  error code sent from by LE controller. The second case is when, an MELE request
 *  results in an error. In this case the "status" parameter in the callback or
 *  the "e.status" field in the CmgrLeScanEvent or CmgrLeAdvertEvent will contain
 *  the status returned from the failing MELE function call. When this event is
 *  received the application should call ME_RadioShutdown() to reset the LE
 *  controller. All future calls to CMGR for LE operations will result in
 *  BT_STATUS_RESTRICTED being returned until the LE controller has been reset.
 *  In this case the handlers will have been removed. The third case occurs when
 *  ME_RadioShutdown() has been called. All handlers will be removed from the
 *  CMGR. The status parameter will contain BT_STATUS_HCI_INIT_ERR and all
 *  future calls to CMGR for LE operations will result in BT_STATUS_RESTRICTED;
 */
#define CMEVENT_LE_ERROR                15

/* LE Advertising has been started. This event is sent as a CmgrLeAdvertEvent
 *  event (based on BtEventEx) when passed to the callback function of a
 *  CmgrLeAdvertHandler.
 */
#define CMEVENT_LE_ADVERT_STARTED       16

/* LE Advertising has been stopped. This event is sent as a CmgrLeAdvertEvent
 *  event (based on BtEventEx) when passed to the callback function of a
 *  CmgrLeAdvertHandler. If "info.stopReason" is SADR_PRIVACY_PAUSED then
 *  then advertising is paused (temporarily stopped) as part of enabling/disabling
 *  privacy. It will be restarted again. The CmgrLeAdvertHandler is still associated
 *  with CMGR. If "info.stopReason" is set to any other value the CmgrLeAdvertHandler
 *  is now disassociated from CMGR and will not receive any more events.
 */
#define CMEVENT_LE_ADVERT_STOPPED       17

/* LE Advertising has been changed to reflect new parameter settings. This event 
 *  is sent as a CmgrLeAdvertEvent event (based on BtEventEx) when passed to the 
 *  callback function of a CmgrLeAdvertHandler.
 */
#define CMEVENT_LE_ADVERT_CHANGED       18

/* LE Advertising has been disabled. This event is sent as a CmgrLeAdvertEvent
 *  event (based on BtEventEx) when passed to the callback function of a
 *  CmgrLeAdvertHandler. This event only occurs if Advertising must be stopped 
 *  prior to being disabled.
 */
#define CMEVENT_LE_ADVERT_DISABLED      19

/* LE Advertising data has been updated. This event is sent as a CmgrLeAdvertEvent
 *  event (based on BtEventEx) when passed to the callback function of a
 *  CmgrLeAdvertHandler. This event only occurs if Advertising data has been 
 *  specifically requested to be updated.
 */
#define CMEVENT_LE_ADVERT_DATA_UPDATE   20

/* Enable/Disable privacy operaton has completed. This event is sent as a
 *  CmgrLePrivacyEvent event to all registered privacy handlers. The 'e.status' 
 *  field will contain BT_STATUS_SUCCESS. 
 *
 *  The 'p.isPrivate' field contains the current privacy setting of the local 
 *  device.
 */
#define CMEVENT_LE_PRIVACY_COMPLETE     21

/* The local private address has been changed in the controller. This event 
 *  is sent as a CmgrLePrivacyEvent to all registered privacy handlers. The 'e.status' 
 *  field of the CmgrLePrivacyEvent indicates whether or not the change was successful. 
 *
 *  The 'p.newPrivAddr' field contains the new private address. 
 *
 *  Note that this event may be sent even if privacy is disabled. The timer that changes
 *  the event will only be started if privacy is enabled but once started it is not
 *  canceled even if privacy is disabled or the timer value is set to 0. Therefore, once
 *  started it will change the private address and notify all registered privacy handlers
 *  when the private address has changed.
 */
#define CMEVENT_LE_NEW_PRIVATE_ADDR     22

/* LE Remote features have been received. A remote features request is made
 *  after an LE link has been created. Some operations require that the remote
 *  features are received before they can be executed. The 'Status' parameter
 *  indicates whether or not the remote features request was successful. If 
 *  status is BT_STATUS_FAILED then the 'CmgrHandler.errCode' field contains
 *  reason for the failure.
 */
#define CMEVENT_LE_REMOTE_FEATURES      23

/* An LE Update Link parameters operation has completed. The 'Status' parameter
 *  indicates whether or not the update Link parameters was successful. If 
 *  status is BT_STATUS_FAILED then the 'CmgrHandler.errCode' field contains
 *  reason for the failure.
 */
#define CMEVENT_LE_UPDATE_LINK          24

 /* An LE Set Data Length operation has completed. The 'Status' parameter
 *  indicates whether or not the update Link parameters was successful. If
 *  status is BT_STATUS_FAILED then the 'CmgrHandler.errCode' field contains
 *  reason for the failure.
 */
#define CMEVENT_LE_DATA_LENGTH_SET      25

 /* An LE Set default Data Length operation has completed. The 'Status' parameter
 *  indicates whether or not the update Link parameters was successful. If
 *  status is BT_STATUS_FAILED then the 'CmgrHandler.errCode' field contains
 *  reason for the failure. All disassociated handlers receive this event.
 */
#define CMEVENT_LE_DEFAULT_DATA_LENGTH  26

/* The maximum payload lengths and/or maximum transmission times of Data
 * channel PDUs in either direction have changed for the given connection.
 * All handlers associated with the connection receive this event. Call
 * CMGR_GetLeCurrentDataLength() to get the current values.
 */
#define CMEVENT_LE_DATA_LENGTH_CHANGE   27     


/* The local resolvable private address (RPA) timeout has been changed in the 
 *  controller. This event is sent as a CmgrLePrivacyEvent to all registered privacy 
 *  handlers. The 'e.status' field of the CmgrLePrivacyEvent indicates whether or 
 *  not the change was successful. 
 *
 *  The 'p.resolvableAddrTimeout' field contains the new timeout. 
 */
#define CMEVENT_LE_CHANGED_RPA_TIMEOUT  28

/* The current local resolvable private address (RPA) has been retrieved from the 
 *  controller. This event is sent as a CmgrLePrivacyEvent to all registered privacy 
 *  handlers. The 'e.status' field of the CmgrLePrivacyEvent indicates whether or 
 *  not the change was successful. 
 *
 *  The 'p.localRpa' field contains the new timeout. 
 */
#define CMEVENT_LE_READ_LOCAL_RPA       29

/* The current peer resolvable private address (RPA) has been retrieved from the 
 *  controller. This event is sent as a CmgrLePrivacyEvent to all registered privacy 
 *  handlers. The 'e.status' field of the CmgrLePrivacyEvent indicates whether or 
 *  not the change was successful. 
 *
 *  The 'p.peerRpa' field contains the new timeout. 
 */
#define CMEVENT_LE_READ_PEER_RPA        30

/* The resolving list in the controller has been cleared of all IRK pairs.
 *  The 'e.status' field of the CmgrLePrivacyEvent indicates whether or 
 *  not this clearing was successful. 
 */
#define CMEVENT_LE_CLEAR_RESOLVE_LIST   31

/* The resolving list in the controller is full and the attempt to add
 *  to this list will fail unless an item is first removed. Removing the
 *  device can be done by removing the LE device record with SEC_DeleteDeviceRecordEx().
 */
#define CMEVENT_LE_RESOLVE_LIST_FULL    32

#endif /* NUM_LE_DEVICES > 0 */

/* End of CmgrEvent */

/*---------------------------------------------------------------------------
 * CmgrEventEx type
 *
 *     Connection Manager events based on BtEventTypeEx. CmgrEventEx events
 *     are offsets from BTE_TYPE_BASE_CMGR (defined in bttypes2.h) based on
 *     CmgrEvent. See CmgrEvent for descriptions of each CmgrEventEx value.
 */
typedef BtEventTypeEx CmgrEventEx;

#define CMEVENTEX_DATA_LINK_CON_CNF      (BTE_TYPE_BASE_CMGR + CMEVENT_DATA_LINK_CON_CNF)
#define CMEVENTEX_DATA_LINK_CON_IND      (BTE_TYPE_BASE_CMGR + CMEVENT_DATA_LINK_CON_IND)
#define CMEVENTEX_DATA_LINK_DIS          (BTE_TYPE_BASE_CMGR + CMEVENT_DATA_LINK_DIS)
#define CMEVENTEX_AUDIO_LINK_REQ         (BTE_TYPE_BASE_CMGR + CMEVENT_AUDIO_LINK_REQ)
#define CMEVENTEX_AUDIO_LINK_CON         (BTE_TYPE_BASE_CMGR + CMEVENT_AUDIO_LINK_CON)
#define CMEVENTEX_AUDIO_LINK_DIS         (BTE_TYPE_BASE_CMGR + CMEVENT_AUDIO_LINK_DIS)
#define CMEVENTEX_AUDIO_DATA             (BTE_TYPE_BASE_CMGR + CMEVENT_AUDIO_DATA)
#define CMEVENTEX_AUDIO_DATA_SENT        (BTE_TYPE_BASE_CMGR + CMEVENT_AUDIO_DATA_SENT)
#define CMEVENTEX_ENTER_SNIFF_MODE       (BTE_TYPE_BASE_CMGR + CMEVENT_ENTER_SNIFF_MODE)
#define CMEVENTEX_EXIT_SNIFF_PARK_MODE   (BTE_TYPE_BASE_CMGR + CMEVENT_EXIT_SNIFF_PARK_MODE)
#if NUM_LE_DEVICES > 0
#define CMEVENTEX_LE_CON_PARMS_SET       (BTE_TYPE_BASE_CMGR + CMEVENT_LE_CON_PARMS_SET)
#define CMEVENTEX_LE_REPORT              (BTE_TYPE_BASE_CMGR + CMEVENT_LE_REPORT)
#define CMEVENTEX_LE_SCAN_STARTED        (BTE_TYPE_BASE_CMGR + CMEVENT_LE_SCAN_STARTED)
#define CMEVENTEX_LE_SCAN_STOPPED        (BTE_TYPE_BASE_CMGR + CMEVENT_LE_SCAN_STOPPED)
#define CMEVENTEX_LE_ERROR               (BTE_TYPE_BASE_CMGR + CMEVENT_LE_ERROR)
#define CMEVENTEX_LE_ADVERT_STARTED      (BTE_TYPE_BASE_CMGR + CMEVENT_LE_ADVERT_STARTED)
#define CMEVENTEX_LE_ADVERT_STOPPED      (BTE_TYPE_BASE_CMGR + CMEVENT_LE_ADVERT_STOPPED)
#define CMEVENTEX_LE_ADVERT_CHANGED      (BTE_TYPE_BASE_CMGR + CMEVENT_LE_ADVERT_CHANGED)
#define CMEVENTEX_LE_ADVERT_DISABLED     (BTE_TYPE_BASE_CMGR + CMEVENT_LE_ADVERT_DISABLED)
#define CMEVENTEX_LE_ADVERT_DATA_UPDATE  (BTE_TYPE_BASE_CMGR + CMEVENT_LE_ADVERT_DATA_UPDATE)
#define CMEVENTEX_LE_PRIVACY_COMPLETE    (BTE_TYPE_BASE_CMGR + CMEVENT_LE_PRIVACY_COMPLETE)
#define CMEVENTEX_LE_NEW_PRIVATE_ADDR    (BTE_TYPE_BASE_CMGR + CMEVENT_LE_NEW_PRIVATE_ADDR)
#define CMEVENTEX_LE_DATA_LENGTH_SET     (BTE_TYPE_BASE_CMGR + CMEVENT_LE_DATA_LENGTH_SET)
#define CMEVENTEX_LE_DEFAULT_DATA_LENGTH (BTE_TYPE_BASE_CMGR + CMEVENT_LE_DEFAULT_DATA_LENGTH)
#define CMEVENTEX_LE_DATA_LENGTH_CHANGE  (BTE_TYPE_BASE_CMGR + CMEVENT_LE_DATA_LENGTH_CHANGE)
#define CMEVENTEX_LE_CHANGED_RPA_TIMEOUT (BTE_TYPE_BASE_CMGR + CMEVENT_LE_CHANGED_RPA_TIMEOUT)
#define CMEVENTEX_LE_READ_LOCAL_RPA      (BTE_TYPE_BASE_CMGR + CMEVENT_LE_READ_LOCAL_RPA)
#define CMEVENTEX_LE_READ_PEER_RPA       (BTE_TYPE_BASE_CMGR + CMEVENT_LE_READ_PEER_RPA)
#define CMEVENTEX_LE_CLEAR_RESOLVE_LIST  (BTE_TYPE_BASE_CMGR + CMEVENT_LE_CLEAR_RESOLVE_LIST)
#define CMEVENTEX_LE_RESOLVE_LIST_FULL   (BTE_TYPE_BASE_CMGR + CMEVENT_LE_RESOLVE_LIST_FULL)

#endif /* NUM_LE_DEVICES > 0 */

/* End of CmgrEventEx */

/*---------------------------------------------------------------------------
 * CmgrSniffExitPolicy type
 *
 * Defines the policy for exiting sniff mode.  While it is possible to both
 * send data and create SCO connections in sniff mode, it may be desired to
 * exit sniff mode for performances or compatibility reasons. Policy bits may
 * be OR'ed together.
 */
typedef U8 CmgrSniffExitPolicy;

/** Exit sniff mode whenever there is data to send.
 */
#define CMGR_SNIFF_EXIT_ON_SEND     0x01

/** Exit sniff mode whenever an audio link is being created.
 */
#define CMGR_SNIFF_EXIT_ON_AUDIO    0x02

/* End of CmgrSniffExitPolicy */

/*---------------------------------------------------------------------------
 * CmgrAudioParms type
 *
 * Defines the parameters for an audio connection.  These values are based
 * on the audio parameters defined by the Hands-free v1.5 specification for
 * eSCO connections.  If defined as CMGR_AUDIO_PARMS_CUSTOM, the default
 * eSCO settings will be used first.  See CMGR_SetAudioDefaultParms(),
 * CMGR_SetAudioVoiceSettings(), and CMGR_SetAudioCustomParms().
 */
typedef U8 CmgrAudioParms;

#define CMGR_AUDIO_PARMS_SCO      1
#define CMGR_AUDIO_PARMS_S1       2
#define CMGR_AUDIO_PARMS_S2       3
#define CMGR_AUDIO_PARMS_S3       4
#define CMGR_AUDIO_PARMS_S4       5
#define CMGR_AUDIO_PARMS_T1       6
#define CMGR_AUDIO_PARMS_T2       7
#define CMGR_AUDIO_PARMS_CUSTOM   8

/* End of CmgrAudioParms */

/* Forward reference */
typedef struct _CmgrHandler CmgrHandler;

/*---------------------------------------------------------------------------
 * CmgrCallback type
 *
 * A function of this type is called to indicate events to the profile.
 */
typedef void (*CmgrCallback)(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status);

/* End of CmgrCallback */

#if NUM_LE_DEVICES > 0
/*---------------------------------------------------------------------------
 * CmgrLeConType type
 *
 * Connection type for an LE connection which is based on the GAP Connection
 * procedures defined in the Bluetooth Core Specification. This setting 
 * controls the type of connection procedure used by CMGR_CreateDataLinkEx()
 * and CMGR_CreateDataLinkList() when the BtDevAddr is for an LE remote device.
 */
typedef U8 CmgrLeConType;

/* Tell CMGR_CreateDataLinkEx() or CMGR_CreateDataLinkList() to use the Auto 
 * Connection Establishment procedure. This procedure uses the LE controller's 
 * white list. If the number of addresses passed to CMGR_CreateDataLinkEx() or 
 * CMGR_CreateDataLinkList() of CMGR_AddLeWhiteList() causes the total number 
 * of addresses, including addresses from previous calls to CMGR_CreateDataLinkEx(),
 * CMGR_CreateDataLinkList() and CMGR_AddLeWhiteList to exceed the available 
 * entries in the white list an error is returned. Use CMGR_GetLeWhiteListInfo() 
 * to get the number of available entries in the white list. 
 *
 * Note that since the LE controller's white list is used a connect request
 * will complete if any address in the white list responds including addresses
 * added using CMGR_AddLeWhiteList(). Normally, only handlers used in 
 * CMGR_CreateDataLinkEx() or CMGR_CreateDataLinkList() will received the 
 * CMEVENT_DATA_LINK_CON_CNF event. But in the case Auto if a connection
 * complete is for an address added via CMGR_AddLeWhiteList() then it will
 * cause all handlers waiting for a connection complete to receive the
 * the CMEVENT_DATA_LINI_CON_CNF event.
 */
#define CMGR_LECON_AUTO    0x00

/* Tell CMGR_CreateDataLinkEx() or CMGR_CreateDataLinkList() to use the General 
 * Connection Establishment procedure. This is the default mode when privacy
 * is supported. This procedure uses scanning to look for LE devices in 
 * "connectable" mode. When a remote device is found that matches an address 
 * being requested, scanning is stopped and a direct connection is made to that 
 * device. Connection manager will connect to the remote device using the address 
 * contained in the remote devices advertising packet. If privacy is supported
 * the connection manager will attempt to locate a device record for each address 
 * passed into CMGR_CreateDataLinkEx() or CMGR_CreateDataLinkList(). The device 
 * record contains the identity address of remote device. All received advertising 
 * packets are similarly processed. If a received address matches a requested
 * address directly or by resolving to the same identity as a requested address
 * then a connection is attempted to that device. If the remote device advertises 
 * using a reconnection address the connection manager will set the local device 
 * address to the reconnection address in order for the remote device to be able 
 * to identify the local device. This is done independently of the current 
 * privacy setting. If privacy is supported this is the preferred connection mode
 * to use since it will connect to a remote device no matter the state of
 * privacy and the type of advertising being used by the remote device.
 */
#define CMGR_LECON_GENERAL 0x01

/* Tell CMGR_CreateDataLinkEx()or CMGR_CreateDataLinkList() to use the Selective 
 * Connection Establishment procedure. This procedure uses the LE controller's white
 * list. This procedure is similar to the General Connection Establishment procedure
 * in that it uses scanning to search for remote devices in "connectable" mode. Only 
 * addresses in the white list will be seen. When a remote device is found that
 * matches an address being requested, scanning is stopped and a direct
 * connection is made to that device. If the number of addresses passed to 
 * CMGR_CreateDataLinkEx() or CMGR_CreateDataLinkList() causes the total number 
 * of addresses, including addresses from previous calls to 
 * CMGR_CreateDataLinkEx() and CMGR_CreateDataLinkList(), to exceed the available
 * in the white list an error is returned. Use CMGR_GetLeWhiteListInfo() to get the 
 * number of available entries in the white list. If privacy is supported and a 
 * reconnection address has been set on the remote device and connection manager 
 * believes the remote device has privacy enabled then the reconnection address 
 * will be used otherwise the address passed into CMGR_CreateDataLinkEx() or 
 * CMGR_CreateDataLinkList() will be used. Also the identity address 
 * (static/public) address of the remote device will be added to the white list 
 * if it is known.
 */
#define CMGR_LECON_SELECT  0x02

/* Tell CMGR_CreateDataLinkEx() or CMGR_CreateDataLinkList() to use the Direct 
 * Connection Establishment procedure. This procedure does not use the white list 
 * nor does it use scanning. Each call to CMGR_CreateDataLinkEx() or 
 * CMGR_CreateDataLinkList() is handled independently in the order the requests 
 * come. Only the first item on the "List" passed to CMGR_CreateDataLinkList() is 
 * used. If a connection is made to remote device all handlers with a matching address 
 * will be associated with the remote device. This procedure will use the address 
 * passed into CMGR_CreateDataLinkEx() and CMGR_CreateDataLinkList() even if privacy 
 * is supported and is enabled. This procedure assumes the caller knows the correct 
 * address in which to connect to the remote device.  
 */
#define CMGR_LECON_DIRECT  0x03

/* End of CmgrLeConType */

/*---------------------------------------------------------------------------
 * CmgrLeWinInt type
 *
 * LE Window and Interval values used for scanning and connecting.
 * The time = value * 0.625 ms. Below are values expressed in milliseconds
 * commonly used in profiles
 */
typedef U16 CmgrLeWinInt;

/* 11.25ms used for scan window in reduced power mode when creating
 * connections
 */
#define CMGR_LE_WININT_11PT25MS   18

#define CMGR_LE_WININT_20MS       32
#define CMGR_LE_WININT_25MS       40
#define CMGR_LE_WININT_30MS       48
#define CMGR_LE_WININT_35MS       56
#define CMGR_LE_WININT_40MS       64
#define CMGR_LE_WININT_45MS       72
#define CMGR_LE_WININT_50MS       80
#define CMGR_LE_WININT_55MS       88
#define CMGR_LE_WININT_60MS       96
#define CMGR_LE_WININT_1000MS   1600
#define CMGR_LE_WININT_1280MS   2048
#define CMGR_LE_WININT_1500MS   2400
#define CMGR_LE_WININT_2000MS   3200
#define CMGR_LE_WININT_2500MS   4000
#define CMGR_LE_WININT_2560MS   4096

/* End of CmgrLeWinInt */

/*---------------------------------------------------------------------------
 * CmgrLeConInt type
 *
 * LE Interval values used for the Minimum and Maximum connection interval.
 * The time = value * 1.25ms. Below are values expressed in milliseconds
 * commonly used in profiles
 */
typedef U16 CmgrLeConInt;

#define CMGR_LE_CONINT_30MS  24
#define CMGR_LE_CONINT_50MS  40

/* End of CmgrLeConInt */

/*---------------------------------------------------------------------------
 * CmgrLeLinkSuper type
 *
 * LE Link Supervision timeout values. Time = value * 10ms. Below are some
 * values expressed in milliseconds.
 */
typedef U16 CmgrLeLinkSuper;

#define CMGR_LE_LS_100MS 10
#define CMGR_LE_LS_200MS 20
#define CMGR_LE_LS_300MS 30
#define CMGR_LE_LS_1SEC  100
#define CMGR_LE_LS_5SEC  500

/* End of CmgrLeLinkSuper */

/*---------------------------------------------------------------------------
 * CmgrLeCeLen type
 *
 * LE CE length values which are the expected minimum and maximum length of
 * the connection events. They are informative. Time = value * 0.625ms.
 */
typedef U16 CmgrLeCeLen;

#define CMGR_LE_CELEN_0MS         0
#define CMGR_LE_CELEN_1000MS   1600

/* End of CmgrLeCeLen */

#endif /* NUM_LE_DEVICES > 0 */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * CmgrAudioData structure
 *
 *     Contains audio data.
 */
typedef struct _CmgrAudioData {
    U16            len;
    U8            *data;

#if BT_STACK_VERSION >= 300
    BtScoErrFlags  errFlags;
#endif

} CmgrAudioData;

#if NUM_LE_DEVICES > 0
/*---------------------------------------------------------------------------
 * CmgrLeWhiteListInfo structure
 *
 *     Used to hold LE White List information used for creating LE connections.
 */
typedef struct _CmgrLeWhiteListInfo {
    /* Holds the total number of white list entries that may be stored in 
     * the LE conroller.
     */
    U8      max;

    /* Holds the number of white list entries available (those are not yet
     * filled).
     */
    U8      avail;

} CmgrLeWhiteListInfo;

/*---------------------------------------------------------------------------
 * CmgrLeGlobalConParms structure
 *
 *     Used to hold LE connection parameters.
 */
typedef struct _CmgrLeGlobalConParms {

    /* Type of connection request */
    CmgrLeConType  type; /* Default is CMGR_LECON_DIRECT */

    /* First set of scan window/interval and time values. "interval12" is used 
     * instead of interval1 when the controller is supporting other operations 
     * such as when a connection already exists or advertising is also going on.
     * The scan window/interval values correspond to time in milliseconds
     * where time = value * 0.625ms. These values are passed to the LE controller
     * and are in operation for time1 milliseconds. If a connection is not
     * established after time1 milliseconds then set 2 is started.
     *
     * If time1 is set to CMGR_TIME_INIFINTY then the window/interval runs until a connection
     * is created or CMGR_RemoveDataLink() is called and set 2 will not be used. Time1 must
     * not be set to 0.
     */
    CmgrLeWinInt interval1;  /* Default setting is CMGR_LE_WININT_30MS */
    CmgrLeWinInt interval12; /* Default setting is CMGR_LE_WININT_60MS */
    CmgrLeWinInt window1;    /* Default setting is CMGR_LE_WININT_30MS */
    TimeT        time1;      /* Default setting is 30000 (30 seconds) */

    /* Second set of scan window/interval and time values. The scan window/interval 
     * values correspond to time in milliseconds where time = value * 0.625ms. These
     * values are passed to the LE controller after the first set and are in
     * operation for time2 milliseconds. If a connection is not established after
     * time2 the connection attempt is considered complete and the handler is
     * called with CMEVENT_DATA_LINK_CON_CNF and the status set to
     * BT_STATUS_TIMEOUT.
     *
     * If time2 is set to 0 then this set is skipped. If time2 is set to
     * CMGR_TIME_INIFINTY then the window/interval runs until a connection
     * is created or CMGR_RemoveDataLink() is called.
     */
    CmgrLeWinInt interval2;  /* Default setting is CMGR_LE_WININT_1280MS */
    CmgrLeWinInt window2;    /* Default setting is CMGR_LE_WININT_11PT25MS */
    TimeT        time2;      /* Default setting is   */

    /* Minimum and maximum connection intervals. The values correspond to
     * time in milliseconds where time = value * 1.25ms.
     */
    CmgrLeWinInt minConInt; /* Default setting is CMGR_LE_CONINT_30MS */
    CmgrLeWinInt maxConInt; /* Default setting is CMGR_LE_CONINT_50MS */

    /* Link Supervision timeout. The values correspond to time in
     * milliseconds where time = value * 10ms. This value must be
     * greater than maxConInt.
     */
    CmgrLeLinkSuper linkSuper; /* Default setting is CMGR_LE_LS_100MS */

    /* Defines the maximum connection latency in connection events */
    U16  conLatency; /* Default setting is 0 */

    /* Informative parameters minimum CE length and maximum CE length. These
     * are provide the controller with the expected minimum and maximum
     * length of the connection events. The minimum must be less than or equal
     * to the maximum. The values correcponse to time in milliseconds where
     * time = value * 0.625ms
     */
    CmgrLeCeLen minCeLen; /* Default is CMGR_LE_CELEN_0MS */
    CmgrLeCeLen maxCeLen; /* Default is CMGR_LE_CELEN_0MS */

} CmgrLeGlobalConParms;

/*---------------------------------------------------------------------------
 * CmgrLePeriphPrefConParms structure
 *
 *     Used to hold LE Perpherial Preferred Connection parameters. 
 */
typedef struct _CmgrLePeriphPrefConParms {

    /* Minimum connection interval. The value corresponds to time in 
     * milliseconds where time = minConInt * 1.25ms. The valid range is 
     * 0x0006 to 0x0C80. A value of 0xFFFF indicates no specific minimum.
     * All other values are reserved.
     */
    U16 minConInt; 

    /* Maximum connection interval. The value corresponds to time in 
     * milliseconds where time = maxConInt * 1.25ms. The valid range is 
     * 0x0006 to 0x0C80. A value for maxConInt must be equal to or greater
     * than minConInt. A value of 0xFFFF indicates no specific minimum. All
     * other values are reserved.
     */
    U16 maxConInt; 

    /* Slave latency for the connection in number of connection events.
     * The valid range is 0x0000 to 0x01F3. All other values are reserved.
     */
    U16 slaveLatency;

    /* Connection Supervision timeout multiplier. The values correspond to 
     * time in milliseconds where time = linkSuper * 10ms. The valid range
     * is 0x000A - 0x0c80. This is a time range of 100ms - 32 seconds.
     * Value of 0xFFFF indicates no specific value is requested. All other 
     * values are reserved.
     */
    CmgrLeLinkSuper conSuperTimeout; 

} CmgrLePeriphPrefConParms;

/*---------------------------------------------------------------------------
 * CmgrLeWhiteListAddrs structure
 *
 *     Used to hold LE Device Addresses to be added to the BT Controller's
 *     white list.
 */
typedef struct _CmgrLeWhiteListAddrs {
    /* Must be the first field. Linkage for CMGR(leWhiteList) */
    ListEntry    node;    /* === Internal Use Only ===  */

    /* Linkage for CMGR(leAddWhiteList) */
    ListEntry    addWl;   /* === Internal Use Only ===  */

    /* Group: The following parameters must be set before calling CMGR_AddLeWhitelist() */

    /* Number of addresses in "addrs" and "ids". */
    U8           len; 

    /* An array of addresses to be added to the whitelist. If an address is an RPA
     * then the identity address will be put in the whitelist. If the address type is
     * BTADDR_LE_ID and Link Layer Privacy is supported in the controller then the ID
     * address will be put in the whitelist. If the address type is BTADDR_LE_ID and
     * Link Layer Privacy is not supported or there is no Security record for the address
     * (i.e. no ID address) then call to CMGR_AddLeWhitelist() will fail. The size of the
     * array is "len".
     */
    BtDevAddr   *addrs;

    /* Array for the identity or "ID" addresses that will put in the whitelist. This field 
     * must have "len" entries.
     */
    BtDevAddr   *ids;

    /* === Internal use only === */

    U8           totalWl;  /* Total number of addrs to be added to white list */
    U8           numWl;    /* Current number of addrs added so far to white list */
    U8           wlI;      /* Index into addrList where next to start adding */
    BOOL         isConReq; /* True if a connection request otherwise false */

} CmgrLeWhiteListAddrs;

#endif /* NUM_LE_DEVICES > 0 */

/*---------------------------------------------------------------------------
 * CmgrHandler structure
 *
 *     Used to identify the handler for Connection Manager events.
 */
struct _CmgrHandler {
    ListEntry            node;        /* Internal use only  */

    /* Group: Read-only parameters */

    CmgrCallback         callback;  /* Callback function to receive events */
    BtDeviceContext     *bdc;       /* Pointer to the device context       */
    BtErrorCode          errCode;   /* Error code - valid during callback  */
    BtRemoteDevice      *remDev;    /* Remote device on connect indication */

#if NUM_SCO_CONNS > 0

#if BT_SCO_HCI_DATA == XA_ENABLED

    CmgrAudioData        audioData; /* Audio data when enabled of HCI      */
    BtPacket            *audioPacket; /* An audio packet that has been transmitted */

#endif /* BT_SCO_HCI_DATA == XA_ENABLED */

    BtScoConnect        *scoConnect; /* The connection structure of a SCO connection */

#endif /* NUM_SCO_CONNS > 0 */

    /* === Internal use only === */
    BtHandler            btHandler;

#if NUM_LE_DEVICES > 0
    ListEntry            leNode;
    ListEntry            leWlNode;
    MeLeLinkHandler      leHandler;
    CmgrLeWhiteListAddrs leWl;
    BtDevAddr            idAddr;
    U8                   leWlAddr;   /* 0 - indicates address from leConParms.list 1 - is leConParms.idList */

    /* In the case of privacy the address passed to CMGR may not be the actual address
     * that will be used to make the connection. For example the address passed to CMGR may  
     * be an old resolvable private address but the connection procedure will need to use a
     * reconnection address. This field contains the actual address that will be used the connection 
     */
    BtDevAddr            leDevAddr;
#endif /* NUM_LE_DEVICES > 0 */

    BtSelectDeviceToken  devQuery;
    U16                  flags;

    /* Sniff Mode Variables */
    CmgrSniffExitPolicy  sniffPolicy;
    EvmTimer             sniffTimer;
    TimeT                sniffTimeout;
    BtSniffInfo          sniffInfo;
    U16                  sniffInterval;

};

/*---------------------------------------------------------------------------
 * CmgrHandler with Context structure
 *
 *     CmgrHandler structure with a context.
 */
typedef struct _CmgrHandlerCtx {
    CmgrHandler  handler;
    void        *context;
} CmgrHandlerCtx;


#if NUM_SCO_CONNS > 0
#include "conmgrsco.h"
#endif

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * CMGR_Init()
 *
 *     Initialize the connection manager.  This function should only be called
 *     once, normally at initialization time.  The calling of this function
 *     can be specified in overide.h using the XA_LOAD_LIST macro
 *     (i.e. #define XA_LOAD_LIST XA_MODULE(CMGR)).
 *
 * Returns:
 *     TRUE - Initialization was successful
 *
 *     FALSE - Initialization failed.
 */
BOOL CMGR_Init(void);

/*---------------------------------------------------------------------------
 * CMGR_RegisterHandler()
 *
 *     Register and Initialize the connection manager handler.  A function
 *     callback must be specified.
 *
 * Parameters:
 *     Handler - Handler structure to use.  This memory is allocated by the
 *         application.
 *
 *     Callback - Function that is called to receive events from the connection
 *         manager.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Initialization was successful.
 *
 *     BT_STATUS_IN_USE - Could not register Handler, because it is
 *         already in use.
 *
 *     BT_STATUS_INVALID_PARM - Callback function or handler are not valid.
 */
BtStatus CMGR_RegisterHandler(CmgrHandler *Handler, CmgrCallback Callback);

/*---------------------------------------------------------------------------
 * CMGR_DeregisterHandler()
 *
 *     Deregister a connection manager handler.
 *
 * Parameters:
 *     Handler - Handler structure to deregister. This handler must have
 *         been previously registered and must not be associated with a link.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Deinitialization was successful.
 *
 *     BT_STATUS_NOT_FOUND - Could not deregister Handler, because it is
 *         not registered.
 *
 *     BT_STATUS_IN_USE - Handler is currently associated with a connection
 *         or connection attempt.
 *
 *     BT_STATUS_INVALID_PARM - Handler is not valid.
 */
BtStatus CMGR_DeregisterHandler(CmgrHandler *Handler);

/*---------------------------------------------------------------------------
 * CMGR_EnableConCnfEventForDisassoc()
 *
 *     Enable/disable the "Send Connect Confirm Event when Disassociated" flag
 *     in the handler. When enabled the handler will receive the 
 *     CMEVENT_DATA_LINK_CON_CNF event. 
 *
 * Parameters:
 *     Handler - Handler structure to use.  This handler must have
 *         been previously registered.
 *
 *     enable - TRUE enables the sending of the event while FALSE disables it. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was successful.
 *
 *     BT_STATUS_NOT_FOUND - Could not deregister Handler, because it is
 *         not registered.
 *
 *     BT_STATUS_INVALID_PARM - Callback handler is not valid.
 */
BtStatus CMGR_EnableConCnfEventForDisassoc(CmgrHandler *Handler, BOOL enable);

/*---------------------------------------------------------------------------
 * CMGR_CreateDataLink()
 *
 *     Associates the handler with an ACL link to the specified device.
 *     If no ACL link exists, an outgoing link is established.  If an ACL
 *     link exists, the Handler is associated with the link and only receives
 *     events for that link.
 *
 *     If a handler wishes to receive events specific to a new ACL link that
 *     it did not create, it must use this function to associate with the link.
 *
 * Parameters:
 *     Handler - A previously registered handler structure to use for
 *         the duration of the link.
 *
 *     Addr - Address of target device. If set to 0, the call will attempt
 *            to get a device through the Device Selection entity by calling
 *            DS_SelectDevice(). The parameters used to select the device
 *            must be provided by the caller by using the "devQuery"
 *            member of the CmgrHandler structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - Connection started.  The CMEVENT_DATA_LINK_CON_CNF
 *         event will indicate if the connection was successful. The
 *         CMEVENT_LE_ERROR will occur in the case where the LE controller
 *         issues an unexpected error while trying to create an LE connection.
 *
 *     BT_STATUS_NOT_FOUND - Could not create data link, because
 *         Handler is not registered.
 *
 *     BT_STATUS_SUCCESS - A connection already exists and the handler is
 *         now associated with that link.
 *
 *     BT_STATUS_IN_PROGRESS - The link is in the process of being disconnected.
 *
 *     BT_STATUS_BUSY - The handler is already involved in a connection attempt.
 *
 *     BT_STATUS_FAILED - Could not create connection
 *
 *     (Also see ME_CreateLink)
 */
BtStatus CMGR_CreateDataLink(CmgrHandler *Handler, BD_ADDR *Addr);

/*---------------------------------------------------------------------------
 * CMGR_CreateDataLinkEx()
 *
 *     Associates the handler with an ACL link to the specified device.
 *     If no ACL link exists, an outgoing link is established.  If an ACL
 *     link exists, the Handler is associated with the link and only receives
 *     events for that link.
 *
 *     If a handler wishes to receive events specific to a new ACL link that
 *     it did not create, it must use this function to associate with the link.
 *
 *     For LE this function is governed by the LE connection parameters 
 *     currently set in the Connection manager. The LE connection parameters 
 *     are set by calling CMGR_SetLeConParms()and can be retrieved by calling 
 *     CMGR_GetLeConParms().
 *
 * Parameters:
 *     Handler - A previously registered handler structure to use for
 *         the duration of the link.
 *
 *     Addr - Address of target device. If set to 0, the call will attempt
 *            to get a device through the Device Selection entity by calling
 *            DS_SelectDevice(). The parameters used to select the device
 *            must be provided by the caller by using the "devQuery"
 *            member of the CmgrHandler structure. For LE  
 *            "Handler->leConParms.devAddrListLen" will be set to 1 by the
 *            Connection Manager.
 *
 *            If Addr.type is set to BTADDR_INVALID a connection attempt will
 *            first be made using BR and then LE.  Setting Addr.type to a value
 *            other than BTADDR_INVALID will force the connection type.
 *
 * Returns:
 *     BT_STATUS_PENDING - Connection started.  The CMEVENT_DATA_LINK_CON_CNF
 *         event will indicate if the connection was successful.
 *
 *     BT_STATUS_NOT_FOUND - Could not create data link, because
 *         Handler is not registered.
 *
 *     BT_STATUS_SUCCESS - A connection already exists and the handler is
 *         now associated with that link.
 *
 *     BT_STATUS_NO_RESOURCES - the operation failed because the maximum number
 *         of connections already exist.
 *
 *     BT_STATUS_FULL - The request failed because the Connection Manager
 *         is setup to use the LE Controller's white list and there is not enough
 *         room in the white list to accomodate the LE connection request.
 *
 *     BT_STATUS_IN_PROGRESS - The link is in the process of being disconnected.
 *
 *     BT_STATUS_BUSY - Handler is already involved in a connection attempt.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 *
 *     BT_STATUS_FAILED - Could not create a connection.
 *
 *     BT_STATUS_INVALID_TYPE - There are to situation in which this error is returned.
 *        First, the Connection manager is setup to use the LE controller's white list 
 *        and one or more of the addresses are invalid for use with the white list. If 
 *        an address is an LE resolvable private address (RPA) and the identity address 
 *        is an RPA then address resolution cannot be performed. Second, direct connection
 *        is attempted to an address of type BTADDR_LE_ID.
 *
 *     BT_INVALID_PARMS - parameters are invalid (error checking only).
 *
 *     (Also see ME_CreateLink and MELE_CreateLink)
 */
BtStatus CMGR_CreateDataLinkEx(CmgrHandler *Handler, BtDevAddr *Addr);

/*---------------------------------------------------------------------------
 * CMGR_CreateDataLinkList()
 *
 *     Associates the handler with an ACL link to one of the devices
 *     in a list of devices. If no ACL link exists, an outgoing link attempt
 *     is started and a connection is made to the first remote device that 
 *     matches a device on "list". If an ACL link exists to one of the 
 *     addresses in the list, the Handler is associated with the link and 
 *     only receives events for that link.
 *
 *     If a handler wishes to receive events specific to a new ACL link that
 *     it did not create, it can use this function to associate with the link.
 *
 *     For LE this function is governed by the LE connection parameters 
 *     currently set in the Connection manager. The LE connection parameers 
 *     are set by calling CMGR_SetLeConParms()and can be retrieved by calling
 *     CMTR_GetLeConParms().
 *
 * Parameters:
 *     Handler - A previously registered handler structure to use for
 *         the duration of the link.
 *
 *     List - A list of 1 or more addresses. If set to 0, the call will attempt
 *         to get a device through the Device Selection entity by calling
 *         DS_SelectDevice(). The parameters used to select the device
 *         must be provided by the caller by using the "devQuery"
 *         member of the CmgrHandler structure. If this used for LE the
 *         "Handler->leConParms.devAddrListLen" will be set to 1 by the
 *         Connection Manager. If "Len" is greater than 1 geb "List" must 
 *         persist for the duration of the connection attempt (until a
 *         connection is made, a timeout or the connection attempt is canceled).
 *
 *         When privacy code is included (LE_PRIVACY == XA_ENABLED) addresses
 *         passed in "List" are used to find the BtDeviceRecord for that device
 *         if found the Connection Manager will use the appropriate addresses
 *         to make the connection. In doing this it may change the addresses
 *         passed in "List". For example the address passed in list may be 
 *         a private resolvable address and Connection manager may need to
 *         connect to this device using a reconnection address.
 *
 *         If list[0].type is set to BTADDR_INVALID a connection attempt will
 *         first be made using BR and then LE.  Setting list[0].type to a value
 *         other than BTADDR_INVALID will force the connection type.
 *
 *         Note that for BR/EDR only item[0] is considered. BR/EDR does not support
 *         lists. 
 *
 *     Len - Number of items in "List" and must be at least 1 even in the case
 *         where the "list" is 0. 
 *
 *     IdList - An array of BtDevAddr structures of size "Num" to hold the
 *         identity addresses of the addresses in "List". If "Num" is 1 then
 *         "IdList" may be set to 0 otherwise it must be a valid array. "IdList"
 *         must persist for the duration of the connection attempt (until
 *         a connection is made, a timeout or the connection attempt is canceled).
 *
 * Returns:
 *     BT_STATUS_PENDING - Connection started.  The CMEVENT_DATA_LINK_CON_CNF
 *         event will indicate if the connection was successful.
 *
 *     BT_STATUS_NOT_FOUND - Could not create data link, because
 *         Handler is not registered.
 *
 *     BT_STATUS_SUCCESS - A connection already exists and the handler is
 *         now associated with that link.
 *
 *     BT_STATUS_NO_RESOURCES - the operation failed because the maximum number
 *         of connections already exist.
 *
 *     BT_STATUS_FULL - The request failed because the Connection Manager
 *         is setup to use the LE Controller's white list and there is not enough
 *         room in the white list to accomodate the LE connection request.
 *
 *     BT_STATUS_IN_PROGRESS - The link is in the process of being disconnected.
 *
 *     BT_STATUS_BUSY - Handler is already involved in a connection attempt.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 *
 *     BT_STATUS_FAILED - Could not create connection
 *
 *     BT_STATUS_INVALID_TYPE - The Connection manager is setup to use the
 *        LE controller's white list and one or more of the addresses are invalid
 *        for use with the white list. If an address is an LE resolvable private
 *        address or the remote device has privacy enabled and does not support
 *        a reconnection address then the white list cannot be used.
 *
 *     BT_INVALID_PARMS - parameters are invalid (error checking only).
 *
 *     (Also see ME_CreateLink and MELE_CreateLink)
 */
BtStatus CMGR_CreateDataLinkList(CmgrHandler *Handler, BtDevAddr *List,
                                 U8 Len, BtDevAddr *IdList);

/*---------------------------------------------------------------------------
 * CMGR_RemoveDataLink()
 *
 *     Disassociates the handler from the link. If no other data links are
 *     present, the management entity will automatically drop the ACL link.
 *     After the handler is disassociated it will again receive events
 *     that target disassociated handlers.
 * 
 *     If a connection is in the process of being created, this function will
 *     attempt to cancel the link creation.  If the connection is established
 *     before the connection can be cancelled (race condition), the Connection
 *     Manager will handle the race condition, and disconnect the established
 *     connection.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The data link is successfully disassociated.
 *         If the ACL is dropped, notification of this and all other ACL
 *         link losses will be passed along to the handler.
 *
 *     BT_STATUS_NOT_FOUND - Could not remove data link, because
 *         Handler is not registered.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 *
 *     BT_INVALID_PARMS - parameters are invalid (error checking only).
 */
BtStatus CMGR_RemoveDataLink(CmgrHandler *Handler);

/*---------------------------------------------------------------------------
 * CMGR_RemoveDataLinkEx()
 *
 *     Disassociates the handler from the link. If no other data links are
 *     present, the management entity will automatically drop the ACL link
 *     but only if we aren't the slave device. 
 *
 *     See CMGR_RemoveDataLink() for additional information.

 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The data link is successfully disassociated.
 *         If the ACL is dropped, notification of this and all other ACL
 *         link losses will be passed along to the handler.
 *
 *     BT_STATUS_NOT_FOUND - Could not remove data link, because
 *         Handler is not registered.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 *
 *     BT_INVALID_PARMS - parameters are invalid (error checking only).
 */
BtStatus CMGR_RemoveDataLinkEx(CmgrHandler *Handler);

/*---------------------------------------------------------------------------
 * CMGR_ForceDisconnectLink()
 *
 *     Forces the disconnection of an existing link to a remote device or
 *     cancels the link if it is in the process of coming up. The link to
 *     disconnect is specified by a Handler. The "remove" parameter is used
 *     to diassociate the handler from the link.
 *
 * Parameters:
 *     Handler - Handler structure associated with the link to disconnect
 *
 *     remove  - If set to the TRUE the handler is diassociated. If set to
 *         FALSE the handler is left associated if possible. There are cases
 *         where the handler will not remain associated. If the link is started
 *         but has not yet come up and needs to be canceled the handler becomes
 *         disassociated with the link regards of the setting for remove. The
 *         same is true of an LE link where cancel need to be called or the
 *         link has not been started.
 *
 * Returns:
 *     BT_STATUS_PENDING - The handler is still associated with the link and
 *         when the link is disconnected the handler will receive the
 *         CMEVENT_DATA_LINK_DIS or CMEVENT_DATA_LINK_CON_CNF event.
 *
 *     BT_STATUS_SUCCESS - The operation has completed successfully.
 *         The caller's handler is unbound from the link. When the link
 *         is actually disconnected all disassociated handlers
 *         will receive the the CMEVENT_DATA_LINK_DIS event if they have 
 *         previously received the CMEVENT_DATA_LINK_CON_IND. If link did
 *         not come up fully or was canceled then all disassociated handler
 *         that have called CMGR_EnableConCnfEventForDisassoc() to enable
 *         receiving y they will receive
 *         the CMEVENT_DATA_LINK_CON_CNF event if the link has not actually
 *         come up yet and the handler has been configured by calling 
 *         CMGR_EnableConCnfEventForDisassoc(). 
 *
 *     BT_STATUS_INVALID_PARM -  The "handler" is 0.
 *
 *     BT_STATUS_NOT_FOUND - The Handler parameter is not found on list of 
 *         registered handlers.
 *
 *     BT_STATUS_NO_CONNECTION - handler is not associated with a connection.
 */
BtStatus CMGR_ForceDisconnectLink(CmgrHandler *Handler, BOOL remove);

/*---------------------------------------------------------------------------
 * CMGR_IsLinkUp()
 *
 *     Returns TRUE if the handler is associated with an open data link.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     TRUE - Link is up.
 *
 *     FALSE - No link exists.
 */
BOOL CMGR_IsLinkUp(CmgrHandler *Handler);

/*---------------------------------------------------------------------------
 * CMGR_GetNumLinks()
 *
 *     Returns the number of active ACL links.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     Number of active links.
 */
U8 CMGR_GetNumLinks(void);

/*---------------------------------------------------------------------------
 * CMGR_GetLinkState()
 *
 *     Returns the state of the data link. If the handler is currently not
 *     associated with any link, returns BDS_DISCONNECTED.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     BtRemDevState
 */
BtRemDevState CMGR_GetLinkState(CmgrHandler *Handler);
#define CMGR_GetLinkState(h) (((h)->bdc && (h)->bdc->link) ? ME_GetRemDevState((h)->bdc->link) : BDS_DISCONNECTED)

/*---------------------------------------------------------------------------
 * CMGR_GetLocalSupportedCodecs()
 *
 *     Gets the array of local supported Codecs.
 *
 * Parameters:
 *     hciSupportedCommands - pointer to the list of supported codecs
 *
 * Returns:
 *     Number of local supported Codecs.
 */
U8 CMGR_GetLocalSupportedCodecs(U8 *localSupportedCodecs);

/*---------------------------------------------------------------------------
 * CMGR_GetRemoteDevice()
 *
 *     Returns the BtRemoteDevice structure associated with the specified
 *     Connection Manager handler bluetooth device context.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     BtRemoteDevice if one exists, otherwise 0.
 */
BtRemoteDevice * CMGR_GetRemoteDevice(CmgrHandler *Handler);

/*---------------------------------------------------------------------------
 * CMGR_GetRemDev()
 *
 *     Returns the BtRemoteDevice structure associated with the specified
 *     Connection Manager handler.  If the connection is down, this will not
 *     be the same as the remote device associated with the specified
 *     Connection Manager handler bluetooth device context.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     BtRemoteDevice if one exists, otherwise 0.
 */
BtRemoteDevice *CMGR_GetRemDev(CmgrHandler *Handler);
#define CMGR_GetRemDev(h) ((h)->remDev)

/*---------------------------------------------------------------------------
 * CMGR_SetRemDev()
 *
 *     Sets the BtRemoteDevice structure associated with the specified
 *     Connection Manager handler.  If the connection is down, this will not
 *     be the same as the remote device associated with the specified
 *     Connection Manager handler bluetooth device context.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 *     remDev - This parameter must contain the address of a structure of
 *         type BtRemoteDevice.
 *
 * Returns:
 *     BtRemoteDevice if one exists, otherwise 0.
 */
void CMGR_SetRemDev(CmgrHandler *Handler, BtRemoteDevice *remDev);
#define CMGR_SetRemDev(h, r) ((h)->remDev = r)

/*---------------------------------------------------------------------------
 * CMGR_SetSniffTimer()
 *
 *     Sets the timeout value used to put the link into sniff mode.  This
 *     timer starts after a link connection is complete.  It is disabled
 *     when a SCO (or eSCO) connection is established and the sniff mode is
 *     exited.  When the SCO (or eSCO) connection is terminated the timer
 *     is started again.  If CMGR_IndicateLinkActivity is called, the timer
 *     is restarted.  If multiple handlers set sniff mode timers, then
 *     the longest timer is used.
 *
 *     The sniff timeout can be set to any value, however, it is
 *     important to note that a value of 0 means that the handler
 *     doesn't care if sniff mode is used and a value of -1 means
 *     that sniff mode is disabled for the specified handler (which
 *     also disables sniff mode for all handlers).
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 *     SniffInfo - Sniff mode parameters. May be 0 to use previously
 *         configured parameters (or default parameters if none were
 *         ever provided). If multiple sets of BtSniffInfo parameters are
 *         provided for a single Bluetooth link, Connection Manager may
 *         use any of the supplied parameters when putting the link into
 *         sniff mode.
 *
 *     Time - Amount of time to wait before starting sniff mode. Any value
 *         greater than 0 will set the timer.  CMGR_SNIFF_DISABLED disables
 *         sniff mode for the specified handler, and consequently, for all
 *         handlers with connections to the same device (see
 *         CMGR_DisableSniffTimer()).  CMGR_SNIFF_DONT_CARE means that this
 *         handler doesn't care if sniff mode is used (see
 *         CMGR_ClearSniffTimer()).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Sniff timer set.
 *
 *     BT_STATUS_NOT_FOUND - Could not set the sniff timer, because Handler
 *         is not registered.
 *
 *     BT_STATUS_INVALID_TYPE - Link is LE so invalid
 *
 */
BtStatus CMGR_SetSniffTimer(CmgrHandler *Handler, BtSniffInfo *SniffInfo,
                            TimeT Time);

/*---------------------------------------------------------------------------
 * CMGR_GetSniffTimer()
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     TimeT - Amount of time to wait before starting sniff mode.
 */
TimeT CMGR_GetSniffTimer(CmgrHandler *Handler);
#define CMGR_GetSniffTimer(h) ((h)->sniffTimeout)

/*---------------------------------------------------------------------------
 * CMGR_ClearSniffTimer()
 *
 *     Clears the sniff mode timeout value.  This is equivalent to saying that
 *     the handler no longer cares if the link is put into sniff mode or not.
 *     Clears the timeout value before the link is put into sniff mode.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Sniff timer cleared.
 *
 *     BT_STATUS_NOT_FOUND - Could not clear the sniff timer, because
 *         Handler is not registered.
 *
 *     BT_STATUS_INVALID_TYPE - Link is LE so invalid
 */
BtStatus CMGR_ClearSniffTimer(CmgrHandler *Handler);

/*---------------------------------------------------------------------------
 * CMGR_IndicateLinkActivity()
 *
 *     Indicates that activity has occurred on the link, which restarts
 *     the sniff timeout.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 */
void CMGR_IndicateLinkActivity(CmgrHandler *Handler);

/*---------------------------------------------------------------------------
 * CMGR_DisableSniffTimer()
 *
 *     Disables sniff mode for the specified handler.  This also disables
 *     sniff mode for all handlers.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Sniff mode disabled.
 *
 *     BT_STATUS_NOT_FOUND - Could not disable the sniff timer, because
 *         Handler is not registered.
 *
 *     BT_STATUS_INVALID_TYPE - Link is LE so invalid
 */
BtStatus CMGR_DisableSniffTimer(CmgrHandler *Handler);

/*---------------------------------------------------------------------------
 * CMGR_SetSniffExitPolicy()
 *
 *     Sets the policy for exiting sniff mode.  The policy can be set to
 *     CMGR_SNIFF_EXIT_ON_SEND or CMGR_SNIFF_EXIT_ON_AUDIO_LINK.  These
 *     values can also be OR'd together to enable both (See
 *     CmgrSniffExitPolicy).
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 *     Policy - Bitmask that defines the policy for exiting sniff mode.
 *
 * Returns:
 *
 *     BT_STATUS_SUCCESS - The policy was set successfully.
 *
 *     BT_STATUS_NOT_FOUND - Could not set the sniff policy, because
 *         Handler is not registered.
 *
 *     BT_STATUS_INVALID_TYPE - Link is LE so invalid
 */
BtStatus CMGR_SetSniffExitPolicy(CmgrHandler *Handler, CmgrSniffExitPolicy Policy);

/*---------------------------------------------------------------------------
 * CMGR_GetSniffExitPolicy()
 *
 *     Gets the policy for exiting sniff mode on the specified channel.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *
 *     CmgrSniffExitPolicy
 */
CmgrSniffExitPolicy CMGR_GetSniffExitPolicy(CmgrHandler *Handler);

/*---------------------------------------------------------------------------
 * CMGR_SetMasterRole()
 *
 *     When 'flag' is TRUE, this function attempts to keep the local device
 *     in the Master role.  If an incoming connection supports the
 *     master/slave role switch, and any handler has called this function to
 *     set the Master Role, then the Connection Manager will attempt to
 *     switch to the Master role.
 *
 *     If a connection exists on the specified handler, and a scatternet does
 *     not already exist, then this function will attempt to switch roles.
 *
 *     Note that the connection manager takes over use of
 *     ME_SetConnectionRole(). Other calls to ME_SetConnectionRole may be
 *     overwritten by the connection manager.
 *
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 *     Flag - TRUE indicates that the Connection Manager must try to maintain
 *         the master role.  FALSE means that no attempt will be made to
 *         maintain the master role.
 *
 * Returns:
 *
 *     BT_STATUS_SUCCESS - The role has been set.
 *
 *     BT_STATUS_NOT_FOUND - Could not set the master role, because
 *         Handler is not registered.
 *
 *     BT_STATUS_INVALID_TYPE - Link is LE so invalid
 */
BtStatus CMGR_SetMasterRole(CmgrHandler *Handler, BOOL flag);

/*---------------------------------------------------------------------------
 * CMGR_ClearErrCode()
 *
 *     Clears the error code associated with the given handler structure.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     void
 */
void CMGR_ClearErrCode(CmgrHandler *Handler);
#define CMGR_ClearErrCode(h) ((h)->errCode = BEC_NO_ERROR)

/*---------------------------------------------------------------------------
 * CMGR_SetErrCodeRemoteTerminated()
 *
 *     Sets the error code associated with the given handler structure to
 *     indicate that the link has been disconnected at the request of the
 *     remote device.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     void
 */
void CMGR_SetErrCodeRemoteTerminated(CmgrHandler *Handler);
#define CMGR_SetErrCodeRemoteTerminated(h) ((h)->errCode = BEC_USER_TERMINATED)

/*---------------------------------------------------------------------------
 * CMGR_SetErrCodeLocalTerminated()
 *
 *     Sets the error code associated with the given handler structure to
 *     indicate that the link has been disconnected at the request of the
 *     local device.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     void
 */
void CMGR_SetErrCodeLocalTerminated(CmgrHandler *Handler);
#define CMGR_SetErrCodeLocalTerminated(h) ((h)->errCode = BEC_LOCAL_TERMINATED)

/*---------------------------------------------------------------------------
 * CMGR_GetErrCode()
 *
 *     Gets the error code associated with the given handler structure.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     An error code of type BtErrorCode.
 */
BtErrorCode CMGR_GetErrCode(CmgrHandler *Handler);
#define CMGR_GetErrCode(h) ((h)->errCode)

/*---------------------------------------------------------------------------
 * CMGR_GetBdAddr()
 *
 *     Gets the 48-bit Bluetooth device address corresponding to the given
 *     remote device.  The value is meaningless if a connection does not
 *     exist to the remote device.
 *
 * Parameters:
 *     remDev - pointer to remote device structure of type BtRemoteDevice.
 *
 *     bdAddr - pointer to a BD_ADDR structure to receive the result.
 *
 * Returns:
 *     void
 */
void CMGR_GetBdAddr(BtRemoteDevice *remDev, BD_ADDR *bdAddr);
#define CMGR_GetBdAddr(r, b) (ME_GetBdAddr(r, b))

/*---------------------------------------------------------------------------
 * CMGR_ReadLocalBdAddr()
 *
 *     Get the 48-bit Bluetooth device address of the local device.
 *
 * Parameters:
 *     Addr - Pointer to a BD_ADDR structure to receive the address.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was successful.
 *
 *     BT_STATUS_FAILED - Operation failed because the system is not
 *         initialized yet.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error.
 */
BtStatus CMGR_ReadLocalBdAddr(BD_ADDR *Addr);

/*---------------------------------------------------------------------------
 * CMGR_GetScoTxPacketLen()
 *
 *     Gets the negotiated flow control SCO TX Packet length from the HCI
 *     transport layer.
 *
 * Parameters:
 *     scoTxPacketLen - This parameter must point to a location where the
 *     SCO transmission packet length can be written.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The request was successful
 *
 *     BT_STATUS_FAILED - The request failed because the parameters have
 *         not yet been negotiated.
 */
BtStatus CMGR_GetScoTxPacketLen(U8 *scoTxPacketLen);

/*---------------------------------------------------------------------------
 * CMGR_SetHciScoPacketSize()
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
 *         not yet been negotiated.
 */
BtStatus CMGR_SetHciScoPacketSize(U8 scoPacketLen);
#define CMGR_SetHciScoPacketSize(s) (ME_SetHciScoPacketSize(s))

/*---------------------------------------------------------------------------
 * CMGR_ScoSetEDR()
 *
 *     Enables or disables (e)SCO Erroneous Data Reporting.  When
 *     Erroneous Data Reporting is enabled, the SCO or eSCO data
 *     received during a BTEVENT_SCO_DATA_IND event will be flagged
 *     in the "scoDataInd.errFlags" field of BtEvent as follows:
 *
 *     BSE_NO_ERROR - Valid data.
 *
 *     BSE_PARTIAL_ERRORS - Possibly invalid data.  Some of the data may
 *         have errors.
 *
 *     BSE_NO_DATA - No data received.  The data payload will be set to 0.
 *
 *     BSE_PARTIAL_DATA - Partially lost data. Some of the data was lost and
 *          that part of the payload will be set to 0.
 *
 * Parameters:
 *     errDataRep - Defines the desired state of Erroneous Data Reporting.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation was started successfully.  The
 *         stack will attempt to change the Erroneous Data Reporting state.
 *         The results will be reported in the
 *         BTEVENT_SET_ERR_DATA_REPORTING_CNF event.
 *
 *     BT_STATUS_IN_PROGRESS - The operation failed because another
 *         attempt to change the Erroneous Data Reporting state is already
 *         in progress.
 *
 *     BT_STATUS_HCI_INIT_ERR - The operation cannot be processed, because
 *         an initialization error has occurred.
 */
BtStatus CMGR_ScoSetEDR(BtErrDataReporting errDataRep);
#define CMGR_ScoSetEDR(e) (SCO_SetErroneousDataReporting(e))

#if NUM_LE_DEVICES > 0
/*---------------------------------------------------------------------------
 * CMGR_SetLeConParms()
 *
 *     Set the connection parameters to be used for all LE connection
 *     requests made via CMGR_CreateDataLinkEx(). Note that the parameters 
 *     will not affect ongoing connections and queued requests. It will take 
 *     effect the next time the queue is empty. Only one change request can 
 *     be pending at a time.
 *
 * Parameters:
 *     handler - pointer to a registered handler to receive the
 *         CMEVENT_LE_CON_PARMS_SET in the case where the function
 *         returns BT_STATUS_PENDING
 *
 *     leConParms - Pointer to a CmgrLeGlobalConParms structure containing 
 *         the parameters to be used. The structure is available when the
 *         function returns.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The parameters were updated successfully.
 *
 *     BT_STATUS_PENDING - The parameters are copied into a temp location and 
 *        will be applied when the LE connection queue is empty. A
 *        CMEVENT_LE_CON_PARMS_SET will be sent to the "handler" when the 
 *        parameters are actually set.
 *
 *     BT_STATUS_BUSY - The operation failed because a request is already
 *         pending.
 *
 *     BT_STATUS_NOT_FOUND - The operation failed because "handler" is not
 *         registered.
 *
 *     BT_STATUS_FAILED - The operation failed because the parameters within 
 *         the structure are not consistent or are invalid.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because leConParms
 *         is an invalid pointer.
 */
BtStatus CMGR_SetLeConParms(CmgrHandler *handler, CmgrLeGlobalConParms *leConParms);

/*---------------------------------------------------------------------------
 * CMGR_GetLeConParms()
 *
 *     Get the current LE connection parameters that are set in the
 *     Connection manager.
 *
 * Parameters:
 *     leConParms - Pointer to a CmgrLeGlobalConParms structure to receive 
 *         the LE connection parameters.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation was successfully.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because leConParms
 *         is an invalid pointer.
 *
 */
BtStatus CMGR_GetLeConParms(CmgrLeGlobalConParms *leConParms);

/*---------------------------------------------------------------------------
 * CMGR_GetLeWhiteListInfo()
 *
 *     Get the current white list Information used by the Connection Manager
 *     for creating LE connections.
 *
 * Parameters:
 *     whiteListInfo - Pointer to a CmgrLeWhiteListInfo structure to receive the
 *         the LE white list information.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation was successfully.
 *
 *     BT_STATUS_FAILED - The operation failed because the white list is not
 *         currently being used by the Connection Manager for connections.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because whiteListInfo
 *         is an invalid pointer.
 *
 */
BtStatus CMGR_GetLeWhiteListInfo(CmgrLeWhiteListInfo *whiteListInfo);

/*---------------------------------------------------------------------------
 * CMGR_SetLePeriphPrefConParms()
 *
 *     Set the preferred connection parameters of the peripheral stored in the 
 *     Peripheral Preferred Connection Parameters Characteristic of the GAP
 *     service. 
 *
 * Parameters:
 *     parms - Pointer to a CmgrLePeriphPrefConParms structure containing 
 *         the parameters to be used. The structure is available when the
 *         function returns.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The parameters were updated successfully.
 *
 *     BT_STATUS_FAILED - The operation failed because the parameters within 
 *         the structure are not consistent or are invalid.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because parms
 *         is an invalid pointer.
 */
BtStatus CMGR_SetLePeriphPrefConParms(CmgrLePeriphPrefConParms *parms);

/*---------------------------------------------------------------------------
 * CMGR_GetLePeriphPrefConParms()
 *
 *     Get the preferred connection parameters of the peripheral stored in the 
 *     Peripheral Preferred Connection Parameters Characteristic of the GAP
 *     service. 
 *
 * Parameters:
 *     parms - Pointer to a CmgrLePeriphPrefConParms structure to receive 
 *         the parameters. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The parameters were retrieved successfully.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because parms
 *         is an invalid pointer.
 */
BtStatus CMGR_GetLePeriphPrefConParms(CmgrLePeriphPrefConParms *parms);

/*---------------------------------------------------------------------------
 * CMGR_SetLeDataLength()
 *
 *     Suggest the maximum transmission packet size and maximum packet
 *     transmission time to be used for the LE connection associated with the
 *     given handler structure. The Controller may use smaller or larger
 *     values based on local information.
 *
 * Parameters:
 *     handler - pointer to a registered handler representing the connection.
 *
 *     maxTxOctets - the maximum number of octets. The valid range is
 *         LE_MIN_DATA_OCTETS through LE_MAX_DATA_OCTETS.
 *
 *     maxTxTime - the maximum time in milliseconds. The valid range is
 *         LE_MIN_DATA_TIME through LE_MAX_DATA_TIME.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation was started successfully. When the
 *        operation completes the CMEVENT_LE_CON_PARMS_SET event will be sent 
 *        to all handlers associated with the connection. 
 *
 *     BT_STATUS_BUSY - The operation failed because a request is already
 *         pending.
 *
 *     BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress. Keep track of the ME LE events to see when the
 *         operation is complete.
 *
 *     BT_STATUS_INVALID_TYPE - The link is BR/EDR so invalid. 
 *
 *     BT_STATUS_NOT_FOUND - The operation failed because "handler" is not
 *         registered.
 *
 *     BT_STATUS_NOT_SUPPORTED - Setting the data length is not supported
 *         by the controller.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because handler, maxTxOctets
 *         and/or maxTxTime are invalid.
 */
BtStatus CMGR_SetLeDataLength(CmgrHandler *handler, U16 maxTxOctets, U16 maxTxTime);

/*---------------------------------------------------------------------------
 * CMGR_SetLeDefaultDataLength()
 *
 *     Set the host perferred values for the Controller's maximum transmission
 *     number of payload octets and maximum packet transmission time to be used
 *     for new connections. The controller may use smaller or larger values based
 *     on local information.
 *
 * Parameters:
 *     maxTxOctets - the maximum number of octets. The valid range is
 *         LE_MIN_DATA_OCTETS through LE_MAX_DATA_OCTETS.
 *
 *     maxTxTime - the maximum time in milliseconds. The valid range is
 *         LE_MIN_DATA_TIME through LE_MAX_DATA_TIME.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation was started successfully. When the
 *        operation completes the CMEVENT_LE_DEFAULT_DATA_LENGTH event will be 
 *        sent to all disassociated handlers associated with the connection.
 *
 *     BT_STATUS_SUCCESS - The values maxTxOctets and maxTxTime are different
 *         from what is already in the controller so nothing is done.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress. Keep track of the ME LE events to see when the
 *         operation is complete.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_NOT_SUPPORTED - Setting the default data length is not
 *         supported by the controller.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because handler, maxTxOctets
 *         and/or maxTxTime are invalid.
 */
BtStatus CMGR_SetLeDefaultDataLength(U16 maxTxOctets, U16 maxTxTime);

/*---------------------------------------------------------------------------
 * CMGR_GetLeCurrentDataLength()
 *
 *     Get the current maximum supported payload octets and packet duration
 *     times for transmission and reception for the LE connection associated 
 *     with the given handler structure.
 *
 * Parameters:
 *     handler - pointer to a registered handler representing the connection.
 *
 *     parms - pointer to memory to receive the values.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The parameters were retrieved successfully.
 *
 *     BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress. Keep track of the ME LE events to see when the
 *         operation is complete.
 *
 *     BT_STATUS_INVALID_TYPE - The link is BR/EDR so invalid.
 *
 *     BT_STATUS_NOT_FOUND - The operation failed because "handler" is not
 *         registered.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because parms
 *         is an invalid pointer.
 */
BtStatus CMGR_GetLeCurrentDataLength(CmgrHandler *handler, MeLeDataLengthParms *parms);

/*---------------------------------------------------------------------------
 * CMGR_GetLeDefaultDataLength()
 *
 *     Get the host perferred values for the Controller's maximum transmission
 *     number of payload octets and maximum packet transmission time to be used
 *     for new connections. The controller may use smaller or larger values based
 *     on local information.
 *
 * Parameters:
 *     maxTxOctets - pointer to memory to receive the current settings for the
 *         suggested maximum number of octets. Can be 0 if this value is not
 *         desired.
 *
 *     maxTxTime - pointer to memory to receive the current settings for the
 *         suggested maximum time in milliseconds. Can be 0 if this value is
 *         not desired
 *
 * Returns:
 *     void
 */
void CMGR_GetLeDefaultDataLength(U16 *maxTxOctets, U16 *maxTxTime);
#define CMGR_GetLeDefaultDataLength(o,t) MELE_GetDefaultDataLength(o,t)

/*---------------------------------------------------------------------------
 * CMGR_GetLeMaximumDataLength()
 *
 *     Get the controller's maximum supported payload octets and packet duration
 *     times for transmission and reception.
 * 
 * Parameters:
 *     parms - pointer to memory to receive the values.
 *
 * Returns:
 *     void
 */
void CMGR_GetLeMaximumDataLength(MeLeDataLengthParms *parms);
#define CMGR_GetLeMaximumDataLength(p) MELE_GetMaximumDataLength(p)


/*---------------------------------------------------------------------------
 * CMGR_AddLeWhitelist()
 *
 *     Add the following addresses to the Bluetooth Controller's white list.
 *     The structure pointed to by "addrs" is added to CMGR and the addresses
 *     will be added to the white list at the appropriate time. Only unique
 *     addresses from "addrs" will be added. Duplicates will not be added. 
 *
 *     The white list can be used during connection creation, advertising and
 *     scanning (discovery) depending on the filter policy or settings used 
 *     for these operations. If a connection operation is underway that is
 *     using the white list, there may be situations where the
 *     addresses may not make it into the white list until the connection
 *     completes. If an advertising and/or scanning operation is under way
 *     that is using the white list then this function will return with
 *     BT_STATUS_BUSY. 
 *
 *     If the "type" field of the connection parameters set using 
 *     CMGR_SetLeConParms() is CMGR_LECON_AUTO then the addresses added using 
 *     CMGR_AddLeWhiteList() are considered part of the list of addresses for 
 *     all the calls to CMGR_CreateDataLinkEx() and CMGR_CreateDataLinkList(). 
 *     This means that while in "Auto" connection mode if a connection request 
 *     occurs for a device with an adddress added using CMGR_AddLeWhiteList() 
 *     then it will match all handlers used in calls to CMGR_CreateDataLinkEx() 
 *     and CMGR_CreateDataLinkList().
 *
 *     The "len", "addrs", and "ids" fields of the CmgrLeWhiteListAddrs structure
 *     must be set before calling this function. If an address is an RPA then 
 *     the identity address will be put in the whitelist. If the address type is
 *     BTADDR_LE_ID and Link Layer Privacy is supported in the controller then the 
 *     ID address will be put in the whitelist. If the address type is BTADDR_LE_ID 
 *     and Link Layer Privacy is not supported or there is no Security record for 
 *     the address (i.e. no ID address) then a call to CMGR_AddLeWhitelist() will 
 *     fail. The size of the "addrs" and "ids" arrays are "len".  
 *
 * Parameters:
 *     addrs - pointer to structure holding the address to be added to the white 
 *         list. The memory for this structure must persist until removed using the
 *         CMGR_RemoveWhiteList() is called.
 *
 *     rejectRpa - Normally a resolvable private address (RPA) should not be
 *         put in the white list since the remote device can change its RPA. 
 *         If Link level privacy is enabled the controller will resolve an RPA into
 *         an identity address so it is put in the white list instead. For the 
 *         normal case this function should be set to TRUE so if an RPA is given
 *         that does not have a security record with its identity address the function
 *         will return BT_STATUS_INVALID_TYPE. To put the RPA in the white list
 *         anyway set rejectRpa to FALSE.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The addresses have been added to CMGR successfully.
 *
 *     BT_STATUS_INVALID_TYPE - The operation failed because one or more addresses 
 *         are not an LE address or are of type BTADDR_LE_ID and Link Layer Privacy 
 *         is not supported or there is no security record containing the ID for 
 *         the address.
 *
 *     BT_STATUS_BUSY - The operation failed because CMGR is advertising and/or
 *         performing scanning (discovery) or is busy setting them up. 
 *
 *     BT_STATUS_FULL - The operation failed because adding the addresses 
 *         will exceed the size of the Bluetooth Controller's white list.
 *
 *     BT_STATUS_IN_USE - addrs has already been added.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because addrs is an 
 *         invalid pointer or "len", "addrs" or "ids" fields are invalid.
 */
BtStatus CMGR_AddLeWhiteList(CmgrLeWhiteListAddrs *addrs, BOOL rejectRpa);

/*---------------------------------------------------------------------------
 * CMGR_RemoveLeWhitelist()
 *
 *     Remove the following addresses from the Bluetooth Controller's white list.
 *     The white list can be used during connection creation, advertising and
 *     scanning (discovery) depending on the filter policy or settings used 
 *     for these operations. If a connection, advertising and/or scanning
 *     operation is underway the addresses may not be removed from the white 
 *     list until all operations are stopped and a new operation that uses 
 *     the white list is started. This function removes the "addrs" structure 
 *     from CMGR so it can be freed by the caller. 
 *
 * Parameters:
 *     addrs - pointer to structure holding the address to be removed from the 
 *         white list. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The "addr" structure has been removed from CMGR.
 *
 *     BT_STATUS_NOT_FOUND - "addrs" has not been added to CMGR. 
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because addrs is an 
 *         invalid pointer.
 */
BtStatus CMGR_RemoveLeWhiteList(CmgrLeWhiteListAddrs *addrs);

#endif /* NUM_LE_DEVICES > 0 */

#endif /* __CONMGR_H_ */

