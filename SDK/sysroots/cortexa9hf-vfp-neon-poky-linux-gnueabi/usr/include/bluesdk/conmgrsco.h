/****************************************************************************
*
* File:
*     conmgrsco.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
*     $Product: OpenSynergy Blue SDK v5.x $
*     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
*
* Description:
*     Public types, defines, and prototypes for accessing the
*     SCO section of Connection Manager.
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

#ifndef __CONMGRSCO_H
#define __CONMGRSCO_H

#include "conmgr.h"

/*---------------------------------------------------------------------------
 * Connection Manager SCO API layer
 *
 *     The Connection Manager SCO API provides a set of functions for managing
 *     SCO (and eSCO) links.  Profiles make calls to the Connection Manager 
 *     when they need a resource.  It is up to the Connection Manager to 
 *     decide how to  best fulfill those requests.
 */

#if NUM_SCO_CONNS > 0

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

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * CMGR_GetScoHciHandle()
 *
 *     Returns the HCI handle of the SCO connection which is associated with
 *     the Connection Manager handler.
 *
 * Parameters:
 *
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     HCI handle for the SCO connection associated with 'Handler', or 0xFFFF
 *     if no SCO connection exists.
 */
U16 CMGR_GetScoHciHandle(CmgrHandler *Handler);

/* Used below */
BtStatus CmgrRegisterAudioHandler(CmgrHandler *Handler, BOOL flag);
BtStatus CmgrDeregisterAudioHandler(CmgrHandler *Handler);

/* Deprecated */
BtStatus CMGR_RegisterScoHandler(CmgrHandler *Handler);
#define CMGR_RegisterScoHandler(h) CmgrRegisterAudioHandler(h, FALSE)
BtStatus CMGR_RegisterScoRequestHandler(CmgrHandler *Handler);
#define CMGR_RegisterScoRequestHandler(h) CmgrRegisterAudioHandler(h, TRUE)
BtStatus CMGR_DeregisterScoHandler(CmgrHandler *Handler);
#define CMGR_DeregisterScoHandler(h) CmgrDeregisterAudioHandler(h)

/*---------------------------------------------------------------------------
 * CMGR_RegisterAudioHandler()
 *
 *     Register a SCO/eSCO handler.  When the handler is successfully
 *     registered, requests to bring up the audio link by the remote device are
 *     automatically accepted.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     BT_STATUS_NOT_FOUND - Could not register the audio handler, because
 *         Handler is not registered.
 *
 *     BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *
 *     BT_STATUS_INVALID_TYPE - Link is LE so invalid
 *
 *     (see SCO_RegisterHandler)
 */
BtStatus CMGR_RegisterAudioHandler(CmgrHandler *Handler);
#define CMGR_RegisterAudioHandler(h) CmgrRegisterAudioHandler(h, FALSE)

/*---------------------------------------------------------------------------
 * CMGR_RegisterAudioRequestHandler()
 *
 *     Register a SCO/eSCO handler.  When a handler is successfully
 *     registered, requests to bring up the audio link by the remote device will
 *     be indicated to the Connection Manager Handler.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     BT_STATUS_NOT_FOUND - Could not register the audio handler, because
 *         Handler is not registered.
 *
 *     BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *
 *     BT_STATUS_INVALID_TYPE - Link is LE so invalid
 *
 *     (see SCO_RegisterHandler)
 */
BtStatus CMGR_RegisterAudioRequestHandler(CmgrHandler *Handler);
#define CMGR_RegisterAudioRequestHandler(h) CmgrRegisterAudioHandler(h, TRUE)

/*---------------------------------------------------------------------------
 * CMGR_DeregisterAudioHandler()
 *
 *     Deregister a SCO/eSCO handler.  Audio link requests will no longer be
 *     accepted.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     BT_STATUS_NOT_FOUND - Could not deregister the audio handler, because
 *         Handler is not registered.
 *
 *     (see SCO_UnregisterHandler)
 */
BtStatus CMGR_DeregisterAudioHandler(CmgrHandler *Handler);
#define CMGR_DeregisterAudioHandler(h) CmgrDeregisterAudioHandler(h)

/*---------------------------------------------------------------------------
 * CMGR_CreateAudioLink()
 *
 *     Create the SCO or eSCO connection.  The CMEVENT_AUDIO_LINK_CON event will
 *     indicate if the connection was successful.  The Connection manager will
 *     automatically try to connect using the best possible parameters,
 *     starting with the values set by CMGR_SetAudioDefaultParms(). If the
 *     default audio parameters are set to CMGR_AUDIO_PARMS_S4 or lower, then
 *     Connection Manager will retry connections until, at the very least, a
 *     SCO connection is established. If the default parms are set to
 *     CMGR_AUDIO_PARMS_T1 or higher, Connection Manager will fail after the
 *     CMGR_AUDIO_PARMS_T1 settings fail. See CMGR_SetAudioDefaultParms() and
 *     CMGR_SetAudioCustomParms().
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - an audio connection is already up.
 *
 *     BT_STATUS_NO_CONNECTION - No data connection exists.
 *
 *     BT_STATUS_NOT_FOUND - Could not create audio link, because Handler is
 *         not registered.
 *
 *     BT_STATUS_INVALID_TYPE - Link is LE so invalid
 *
 *     (Also see SCO_CreateLink)
 */
BtStatus CMGR_CreateAudioLink(CmgrHandler *Handler);

/*---------------------------------------------------------------------------
 * CMGR_AcceptAudioLink()
 *
 *     Accepts or rejects an audio link.  If 'Error' is set to a value other
 *     than BEC_NO_ERROR, the connection is rejected, otherwise it is
 *     accepted.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *     Error - Error code if rejecting the connection.
 *
 * Returns:
 *
 *     BT_STATUS_NOT_FOUND - Could not create audio link, because Handler is
 *         not registered.
 *
 *     BT_STATUS_INVALID_TYPE - Link is LE so invalid
 *
 *    (Also see SCO_AcceptIncomingConnection() and
 *     SCO_RejectIncomingConnection())
 */
BtStatus CMGR_AcceptAudioLink(CmgrHandler *Handler, BtErrorCode Error);

/*---------------------------------------------------------------------------
 * CMGR_RemoveAudioLink()
 *
 *     Bring down the SCO connection.  The CMEVENT_AUDIO_LINK_DIS event indicates
 *     when the audio connection is down.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The audio connection is down.
 *
 *     BT_STATUS_NOT_FOUND - Could not remove audio link, because Handler is
 *         not registered.
 *
 *     (see also SCO_DisconnectLink)
 */
BtStatus CMGR_RemoveAudioLink(CmgrHandler *Handler);

#if BT_SCO_HCI_DATA == XA_ENABLED
/*---------------------------------------------------------------------------
 * CMGR_SendAudioData()
 *
 *     Sends the specified audio data on the SCO link.
 *
 * Requires:
 *     BT_SCO_HCI_DATA enabled
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 *     Packet - The packet of data to send. After this call, the SCO
 *         manager owns the packet.  When the packet has been transmitted
 *         to the host controller, CMEVENT_AUDIO_DATA_SENT is sent to the
 *         handler(s).
 *
 * Returns:
 *     BT_STATUS_PENDING - The packet was queued successfully.
 *
 *     BT_STATUS_NO_CONNECTION - No SCO connection exists.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus CMGR_SendAudioData(CmgrHandler *Handler, BtPacket *Packet);
#define CMGR_SendAudioData(h, p) (SCO_SendData((h)->scoConnect, p))
#endif /* BT_SCO_HCI_DATA == XA_ENABLED */


/*---------------------------------------------------------------------------
 * CMGR_IsAudioUp()
 *
 *     Returns the state of the audio link.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     TRUE - Audio link is up.
 *
 *     FALSE - No audio link exists.
 */
BOOL CMGR_IsAudioUp(CmgrHandler *Handler);

/*---------------------------------------------------------------------------
 * CMGR_SetAudioDefaultParms()
 *
 *     Sets the default parameters for SCO or eSCO connections.  These settings
 *     are based on the settings described in the Hands-free 1.5 specification.
 *     This is a global setting that directly affects all handlers.  If the
 *     parameters are set to CMGR_AUDIO_PARMS_CUSTOM, then the custom
 *     parameters set by CMGR_SetAudioCustomParms() will be used.
 *
 * Parameters:
 *     Parms - The default audio settings.  If these settings are not supported
 *         by the radio module, the highest possible settings will be used.
 *         For example, Bluetooth 1.1 radios will only support
 *         CMGR_AUDIO_PARMS_SCO and Bluetooth 1.2 radios will support
 *         CMGR_AUDIO_PARMS_S1 and CMGR_AUDIO_PARMS_SCO.  If set to
 *         CMGR_AUDIO_PARMS_CUSTOM, then the custom parameters will be used.
 *         If a connection attempt fails, then lower settings will be tried.
 *         At the very least, a SCO connection will be attempted.
 *
 * Returns:
 *     void
 */
void CMGR_SetAudioDefaultParms(CmgrAudioParms Parms);

/*---------------------------------------------------------------------------
 * CMGR_GetAudioDefaultParms()
 *
 *     Gets the default parameters for SCO or eSCO connections.  These settings
 *     are based on the settings described in the Hands-free 1.5 specification.
 *     This is a global setting that directly affects all handlers.
 *
 * Returns:
 *     (See CmgrAudioParms)
 */
CmgrAudioParms CMGR_GetAudioDefaultParms(void);

/*---------------------------------------------------------------------------
 * CMGR_GetAudioAltDefaultParms()
 *
 *     Gets the alternate default parameters for SCO or eSCO connections.
 *     This alternate setting is to save the settings of the current codec
 *     when a new codec is selected.
 *     This is a global setting that directly affects all handlers.
 *
 * Returns:
 *     (See CmgrAudioParms)
 */
CmgrAudioParms CMGR_GetAudioAltDefaultParms(void);

/*---------------------------------------------------------------------------
 * CMGR_GetAudioParms()
 *
 *     Gets the parameters for SCO or eSCO connections.  These settings
 *     are based on the settings described in the Hands-free 1.5
 *     specification.  This is a global setting that directly affects all
 *     handlers.
 *
 * Returns:
 *     (See CmgrAudioParms)
 */
CmgrAudioParms CMGR_GetAudioParms(void);

/*---------------------------------------------------------------------------
 * CMGR_SetAudioVoiceSettings()
 *
 *     Sets the voice settings for audio connections.
 *
 * Returns:
 *     void
 */
void CMGR_SetAudioVoiceSettings(BtScoAudioSettings settings);

/*---------------------------------------------------------------------------
 * CMGR_GetAudioVoiceSettings()
 *
 *     Gets the current voice settings for audio connections.
 *
 * Returns:
 *     Current voice settings (See BtScoAudioSettings)
 */
BtScoAudioSettings CMGR_GetAudioVoiceSettings(void);

/*---------------------------------------------------------------------------
 * CMGR_SetAudioChannelType()
 *
 *     Sets the channel type for audio connections.
 *
 * Returns:
 *     void
 */
void CMGR_SetAudioChannelType(BtScoAudioChannelType channelType);

/*---------------------------------------------------------------------------
 * CMGR_GetAudioChannelType()
 *
 *     Gets the current channel type for audio connections.
 *
 * Returns:
 *     Current channel type (See BtScoAudioChannelType)
 */
BtScoAudioChannelType CMGR_GetAudioChannelType(void);

/*---------------------------------------------------------------------------
 * CMGR_SetAudioCustomParms()
 *
 *     Sets the custom audio parameters.  This is a global setting that directly
 *     affects all handlers.  These parameters will be used to create an eSCO
 *     connection if possible.  If the connection fails, then the
 *     CMGR_AUDIO_PARMS_S1 settings will be used.  If that fails, then a SCO
 *     connection will be attempted.
 *
 * Parameters:
 *     customParms - a pointer to the SCO transmit parameters.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - parameters were set successfully.
 *     BT_STATUS_INVALID_PARMS - invalid parameter (Error check only)
 */
BtStatus CMGR_SetAudioCustomParms(BtScoTxParms *customParms);

/*---------------------------------------------------------------------------
 * CMGR_GetAudioCustomParms()
 *
 *     Copies the current custom parameters into the specified memory.  These
 *     parameters are used to create audio connections when the default
 *     audio parameters are set to CMGR_AUDIO_PARMS_CUSTOM
 *     (see CMGR_SetAudioDefaultParms()).
 *
 * Parameters:
 *     customParms - a pointer to memory that will receive the SCO
 *         transmit parameters.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - parameters were copied successfully.
 *     BT_STATUS_INVALID_PARMS - invalid parameter (Error check only)
 */
BtStatus CMGR_GetAudioCustomParms(BtScoTxParms *customParms);

/*---------------------------------------------------------------------------
 * CMGR_GetNumAudioLinks()
 *
 *     Returns the number of active SCO links.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     Number of active SCO links.
 */
U8 CMGR_GetNumAudioLinks(void);

#if BT_SCO_HCI_DATA == XA_ENABLED

/*---------------------------------------------------------------------------
 * CMGR_GetAudioPacket()
 *
 *     Gets the audio packet associated with the given handler structure.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     A pointer to an audio packet of type BtPacket.
 */
BtPacket *CMGR_GetAudioPacket(CmgrHandler *Handler);
#define CMGR_GetAudioPacket(h) ((h)->audioPacket)

/*---------------------------------------------------------------------------
 * CMGR_GetAudioData()
 *
 *     Gets the audio data associated with the given handler structure.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     A pointer to an audio data of type CmgrAudioData.
 */
CmgrAudioData *CMGR_GetAudioData(CmgrHandler *Handler);
#define CMGR_GetAudioData(h) (&(h)->audioData)

#endif /* BT_SCO_HCI_DATA == XA_ENABLED */

/*---------------------------------------------------------------------------
 * CMGR_GetScoConnect()
 *
 *     Gets the SCO connection associated with the given handler structure.
 *
 * Parameters:
 *     Handler - Handler structure to use.
 *
 * Returns:
 *     A pointer to a SCO connection of type BtScoConnect.
 */
BtScoConnect *CMGR_GetScoConnect(CmgrHandler *Handler);
#define CMGR_GetScoConnect(h) ((h)->scoConnect)

#endif /* NUM_SCO_CONNS > 0 */

#endif /* __CONMGRSCO_H */
