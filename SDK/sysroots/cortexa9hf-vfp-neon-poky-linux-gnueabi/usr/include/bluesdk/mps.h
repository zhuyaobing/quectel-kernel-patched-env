/****************************************************************************
 *
 * File:
 *     mps.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the Bluetooth
 *     Multi-Profile program interface.
 *
 * Copyright 2009-2017 OpenSynergy GmbH.
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

#ifndef __MPS_H_
#define __MPS_H_


#include "utils.h"
#include "sdp.h"

/*---------------------------------------------------------------------------
 * Multi-Profile Specification (MPS) Layer
 *
 */

/****************************************************************************
 *
 * Section: Default general constants that can be overridden in overide.h
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * MPS_SDP_REGISTER_SERVICE_DESCRIPTION Constant
 *
 *  When enabled, the service description will be included in the SDP record,
 *  otherwise it will not be included. Note: You should edit the
 *  MultiProfileDescription and set the length in the SDP_TEXT_8BIT() macro.
 */ 
#define MPS_SDP_REGISTER_SERVICE_DESCRIPTION  XA_ENABLED

/*---------------------------------------------------------------------------
 * MPS_MAX_LOCAL_DEVICE_RECORDS Constant
 *
 *  Defines the maximum number of multi-profile records supported
 *
 *  The default value is 1. Set to 0 to disable server support.
 */
#ifndef MPS_MAX_LOCAL_DEVICE_RECORDS
#define MPS_MAX_LOCAL_DEVICE_RECORDS            1
#endif

/*---------------------------------------------------------------------------
 * MPS_MAX_REMOTE_DEVICE_RECORDS Constant
 *
 *  Defines the maximum number of remote device records that will be support.
 *
 *  The default value is 1. Set to 0 to disable server support.
 */
#ifndef MPS_MAX_REMOTE_DEVICE_RECORDS
#define MPS_MAX_REMOTE_DEVICE_RECORDS            1
#endif

/*---------------------------------------------------------------------------
 * MPS_MAX_REMOTE_DEVICE_RECORDS Constant
 *
 *  Defines the maximum number of remote device records that will be support.
 *
 *  The default value is 1. Set to 0 to disable server support.
 */
#ifndef MPS_MAX_REMOTE_DEVICE_RECORDS
#define MPS_MAX_REMOTE_DEVICE_RECORDS            1
#endif

/*---------------------------------------------------------------------------
 * MPS_MAX_SERV_DESC_LEN Constant
 *
 *  Defines the maximum number of bytes allocated for a service description.
 *
 *  The default value is 128 bytes.
 */
#ifndef MPS_MAX_SERV_DESC_LEN
#define MPS_MAX_SERV_DESC_LEN                   128
#endif

/*---------------------------------------------------------------------------
 * MPS_SUPPORTED_VERSION Constant
 *
 *  Defines the specification version supported by this implementation
 *
 *  The default value is 0x0100
 */
#ifndef MPS_SUPPORTED_VERSION
#define MPS_SUPPORTED_VERSION 0x0100
#endif

/*---------------------------------------------------------------------------
 * MPS_SUPPORTED_FLAGS_INIT_VALUE Constant
 *
 * Initial placeholder value for Multi-Device supported scenario flags. These
 * flags are 64 bit value when stored in SDP record. They are represented as
 * an 8 byte array here for portability. By default, all flags are initialized
 * to 0.
 */
#ifndef MPS_SUPPORTED_FLAGS_INIT_VALUE
#define MPS_SUPPORTED_FLAGS_INIT_VALUE \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif

/****************************************************************************
 *
 * Types
 *
 ***************************************************************************/

 /*---------------------------------------------------------------------------
 * MpsMpSdFeatureFlagsBm1 type
 *
 * This type represents the bits 7-0 of the Multi-profile Single Device
 * bitmask stored in the SDP record.
 */
typedef U8 MpsMpSdFeatureFlagsBm1;

/* 0: Answer incoming call during Audio Streaming */
#define MPSD_BM1_IN_CALL_HFV_AG_AUDIO_STREAM_A2DP_SRC       0x01

/* 1: Answer incoming call during Audio Streaming */
#define MPSD_BM1_IN_CALL_HFV_HF_AUDIO_STREAM_A2DP_SNK       0x02

/* 2: Outgoing call during Audio Streaming */
#define MPSD_BM1_OUT_CALL_HFV_AG_AUDIO_STREAM_A2DP_SRC      0x04

/* 3: Outgoing call during Audio Streaming */
#define MPSD_BM1_OUT_CALL_HFP_HF_AUDIO_STREAM_A2DP_SNK      0x08

/* 4: Reject/Ignore incoming call during Audio Streaming */
#define MPSD_BM1_REJ_IN_CALL_HFP_AG_AUDIO_STREAM_A2DP_SRC   0x10

/* 5: Reject/Ignore incoming call during Audio Streaming */
#define MPSD_BM1_REJ_IN_CALL_HFP_HF_AUDIO_STREAM_A2DP_SNK   0x20

/* 6: HFP call termination during AVP connection */
#define MPSD_BM1_TERM_CALL_HFP_AG_AVP_CONN_A2DP_SRC         0x40

/* 7: HFP call termination during AVP connection */
#define MPSD_BM1_TERM_CALL_HFP_HF_AVP_CONN_A2DP_SNK         0x80

/* End of MpsMpSdFeatureFlagsBm1 */

/*---------------------------------------------------------------------------
 * MpsMpSdFeatureFlagsBm2 type
 *
 * This type represents the bits 15-8 of the Multi-profile Single Device
 * bitmask stored in the SDP record.
 */
typedef U8 MpsMpSdFeatureFlagsBm2;

/* 8: Press Play on Audio Player during active call */
#define MPSD_BM2_ACTIVE_CALL_HFP_AG_PRESS_PLAY_A2DP_SRC     0x01

/* 9: Press Play on Audio Player during active call */
#define MPSD_BM2_ACTIVE_CALL_HFP_HF_PRESS_PLAY_A2DP_SNK     0x02

/* 10: Start audio streaming after AVRCP Play Command */
#define MPSD_BM2_START_STREAM_HFP_AG_AVCRP_PLAY_A2DP_SRC    0x04

/* 11: Start audio streaming after AVRCP Play Command */
#define MPSD_BM2_START_STREAM_HFP_HF_AVRCP_PLAY_A2DP_SNK    0x08

/* 12: Suspend audio streaming after AVRCP Pause/Stop */
#define MPSD_BM2_SUSP_STREAM_HFP_AG_PAUSE_STOP_A2DP_SRC     0x10

/* 13: Suspend audio streaming after AVRCP Pause/Stop */
#define MPSD_BM2_SUSP_STREAM_HFP_HF_PAUSE_STOP_A2DP_SNK     0x20

/* 14: Data communication under PSDM (DUN) during active voice call */
#define MPSD_BM2_VOICE_CALL_HFP_AG_DATA_COMM_DUN_GW         0x40

/* 15: Data communication under PSDM (DUN) during active voice call */
#define MPSD_BM2_VOICE_CALL_HFP_HF_DATA_COMM_DUN_DT         0x80

/* End of MpsMpSdFeatureFlagsBm2 */

/*---------------------------------------------------------------------------
 * MpsMpSdFeatureFlagsBm3 type
 *
 * This type represents the bits 23-16 of the Multi-profile Single Device
 * bitmask stored in the SDP record.
 */
typedef U8 MpsMpSdFeatureFlagsBm3;

/* 16: Outgoing voice call during Data communication under PSDM (DUN) */
#define MPSD_BM3_OUT_CALL_HFP_AG_DATA_COMM_DUN_GW        0x01

/* 17: Outgoing voice call during Data communication under PSDM (DUN) */
#define MPSD_BM3_OUT_CALL_HFP_HF_DATA_COMM_DUN_DT        0x02

/* 18: Incoming voice call during Data communication under PSDM (DUN) */
#define MPSD_BM3_IN_CALL_HFP_AG_DATA_COMM_DUN_GW         0x04

/* 19: Incoming voice call during Data communication under PSDM (DUN) */
#define MPSD_BM3_IN_CALL_HFP_HF_DATA_COMM_DUN_DT         0x08

/* 20: Start audio streaming during Data communication under PSDM (DUN) */
#define MPSD_BM3_AUDIO_STREAM_HFP_AG_DATA_COMM_DUN_GW    0x10

/* 21: Start audio streaming during Data communication under PSDM (DUN) */
#define MPSD_BM3_AUDIO_STREAM_HFP_HF_DATA_COMM_DUN_DT    0x20

/* 22: Data communication establishment under PSDM (DUN) */
#define MPSD_BM3_AUDIO_STREAM_A2DP_SRC_DATA_COMM_DUN_GW  0x40

/* 23: Data communication establishment under PSDM (DUN) */
#define MPSD_BM3_AUDIO_STREAM_A2DP_SRC_DATA_COMM_DUN_DT  0x80

/* End of MpsMpSdFeatureFlagsBm3 */

/*---------------------------------------------------------------------------
 * MpsMpSdFeatureFlagsBm4 type
 *
 * This type represents the bits 31-24 of the Multi-profile Single Device
 * bitmask stored in the SDP record.
 */
typedef U8 MpsMpSdFeatureFlagsBm4;

/* 24: Terminate voice call/data call during data communication and voice call */
#define MPSD_BM4_TERM_CALL_HFP_AG_DATA_COMM_DUN_GW       0x01

/* 25: Terminate voice call/data call during data communication and voice call */
#define MPSD_BM4_TERM_CALL_HFP_HF_DATA_COMM_DUN_DT       0x02

/* 26: Data communication in Personal Area Network during active voice call */
#define MPSD_BM4_ACTIVE_CALL_HFP_AG_DATA_COMM_PAN_NAP    0x04

/* 27: Data communication in Personal Area Network during active voice call */
#define MPSD_BM4_ACTIVE_CALL__HFP_HF_DATA_COMM_PAN_PANU  0x08

/* 28: Outgoing voice call during Data communication in Personal Area Network */
#define MPSD_BM4_OUT_CALL_HFG_AG_DATA_COMM_PAN_NAP       0x10

/* 29: Outgoing voice call during Data communication in Personal Area Network */
#define MPSD_BM4_OUT_CALL_HFG_HF_DATA_COMM_PAN_PANU      0x20

/* 30: Incoming voice call during Data communication in Personal Area Network */
#define MPSD_BM4_IN_CALL_HFG_AG_DATA_COMM_PAN_NAP        0x40

/* 31: Incoming voice call during Data communication in Personal Area Network */
#define MPSD_BM4_IN_CALL_HFG_HF_DATA_COMM_PAN_PANU       0x80

/* End of MpsMpSdFeatureFlagsBm4 */

/*---------------------------------------------------------------------------
 * MpsMpSdFeatureFlagsBm5 type
 *
 * This type represents the bits 39-32 of the Multi-profile Single Device
 * bitmask stored in the SDP record.
 */
typedef U8 MpsMpSdFeatureFlagsBm5;

/* 32: Start audio streaming during Data communication in Personal Area Network */
#define MPSD_BM5_START_STREAM_A2DP_SRC_DATA_COMM_PAN_NAP       0x01

/* 33: Start audio streaming during Data communication in Personal Area Network */
#define MPSD_BM5_START_STREAM_A2DP_SNK_DATA_COMM_PAN_PANU      0x02

/* 34: Data communication establishment in Personal Area Network during Audio Streaming */
#define MPSD_BM5_AUDIO_STREAM_A2DP_SRC_EST_DATA_COMM_PAN_NAP   0x04

/* 35: Data communication establishment in Personal Area Network during Audio Streaming */
#define MPSD_BM5_AUDIO_STREAM_A2DP_SNK_EST_DATA_COMM_PAN_PANU  0x08

/* 36: Phonebook download during Audio Streaming */
#define MPSD_BM5_AUDIO_STREAM_A2DP_SRC_PHONEBOOK_DL_PBAP_SRV   0x10

/* 37: Phonebook download during Audio Streaming */
#define MPSD_BM5_AUDIO_STREAM_A2DP_SNK_PHONEBOOK_DL_PBAP_CLI   0x20

/* 38 - 39: Reserver for future use */ 
#define MPSD_BM5_RFU                                           0xC0

/* End of MpsMpSdFeatureFlagsBm5 */

/*---------------------------------------------------------------------------
 * MpsMpMdFeatureFlagsBm1 type
 *
 * This type represents the bits 7-0 of the Multi-profile Multiple Device
 * bitmask stored in the SDP record.
 */
typedef U8 MpsMpMdFeatureFlagsBm1;

/* 0: Answer incoming call during Audio Streaming */
#define MPMD_BM1_INC_CALL_HFP_HF_AUDIO_STREAM_A2DP_SNK_AVRCP_CT 0x01

/* 1: Answer incoming call during Audio Streaming */
#define MPMD_BM1_INC_CALL_A2DP_SRC_AUDIO_STREAM_AVRCP_TG        0x02

/* 2: Outgoig Call during Audio Streaming */
#define MPMD_BM1_OUT_CALL_HFP_HF_AUDIO_STREAM_A2DP_SNK_AVRCP_CT 0x04

/* 3: Outgoing call during Audio Streaming */
#define MPMD_BM1_OUT_CALL_A2DP_SRC_AUDIO_STREAM_AVRCP_TG        0x08

/* 4: Reject/Ignore Incoming Call during Audio Streaming */
#define MPMD_BM1_REJ_CALL_HFP_HF_AUDIO_STREAM_A2DP_SNK_AVRCP_CT 0x10

/* 5: Reject/Ignore Incoming Call during Audio Streaming */
#define MPMD_BM1_REJ_CALL_A2DP_SRC_AUDIO_STREAM_AVRCP_TG        0x20

/* 6: HFP Call termination during AVP connection */
#define MPMD_BM1_CALL_TERM_AVP_CONN_HFP_AG                      0x40

/* 7: HFP Call termination during AVP connection */
#define MPMD_BM1_CALL_TERM_HFP_HF_AVP_CONN_A2DP_SNK_AVRCP_CT    0x80

/* End of MpsMpMdFeatureFlagsBm1 */

/*---------------------------------------------------------------------------
 * MpsMpMdFeatureFlagsBm2 type
 *
 * This type represents the bits 15-8 of the Multi-profile Multiple Device
 * bitmask stored in the SDP record.
 */
typedef U8 MpsMpMdFeatureFlagsBm2;

/* 8: HFP Call termination during AVP Connection */
#define MPMD_BM2_CALL_TERM_A2DP_SRC_AVP_CONN_AVRCP_TG            0x01

/* 9: Press Play on Audio Player during Active Call */
#define MPMD_BM2_ACTIVE_CALL_HFP_HF_PRESS_PLAY_A2DP_SNK_AVRCP_CT 0x02

/* 10: Press Play on Audio Player during Active Call */
#define MPMD_BM2_ACTIVE_CALL_A2DP_SRC_PRESS_PLAY_AVRCP_TG        0x04

/* 11: Start Audio Streaming after AVRCP Play Command (AVRCP_CT where
 *     the same device does not carry out the role of an A2DP_SNK)
 */
#define MPMD_BM2_START_STREAM_PLAY_COMMAND_AVRCP_CT_NOT_A2DP_SNK 0x08

/* 12: Start Audio Streaming after AVRCP Play Command */
#define MPMD_BM2_START_STREAM_A2DP_SRC_AVRCP_PLAY_AVRCP_TG       0x10

/* 13: Suspend Audio Streaming after AVRCP Pause/Stop (AVRCP_CT where 
 *     the same device does not carry out the role of an A2DP_SNK)
 */
#define MPMD_BM2_SUSP_AUDIO_STREAM_AVRCP_CT_NOT_A2DP_SNK         0x20

/* 14: Suspend Audio Streaming after AVRCP Pause/Stop */
#define MPMD_BM2_SUSP_AUDIO_STREAM_A2DP_SRC_AVRCP_TG             0x40

/* 15: Start Audio streaming during Data communication under PSDM */
#define MPMD_BM2_START_STREAM_A2DP_SRC_DATA_COMM_AVRCP_TG        0x80

/* End of MpsMpMdFeatureFlagsBm2 */

/*---------------------------------------------------------------------------
 * MpsMpMdFeatureFlagsBm3 type
 *
 * This type represents the bits 23-16 of the Multi-profile Multiple Device
 * bitmask stored in the SDP record.
 */
typedef U8 MpsMpMdFeatureFlagsBm3;

/* 16: Start Audio streaming during Data communication under PSDM */
#define MPMD_BM3_START_STREAM_A2DP_SNK_AVRCP_CT_DATA_COMM_DUN_DT 0x01

/* 17: Start Packet data communication during Audio streaming */
#define MPMD_BM3_START_DATA_COMM_A2DP_SRC_AUDIO_STREAM_AVRCP_TG  0x02

/* 18: Start Packet data communication during Audio streaming */
#define MPMD_BM3_START_DATA_COMM_DUN_DT_STREAM_A2DP_SNK_AVRC_CT  0x04

/* 19-23: Reserved for future use */
#define MPMD_BM3_RFU                                             0xF8

/* End of MpsMpMdFeatureFlagsBm3 */

/*---------------------------------------------------------------------------
 * MpsProtocolDependencyBm1 type
 *
 * This type represents Protocol and Profile dependencies bitmask.
 */
typedef U8 MpsProtocolDependencyBm1;

/* Sniff mode during streaming */
#define MPD_BM_SNIFF_MODE_DURING_STREAMING 0x01

/* GAVDP Requirements */
#define MPD_BM_GAVDP_REQUIREMENTS          0x02

/* (Dis-)Connection Order / Behavior */
#define MPD_BM_CONNECTION_ORDER_BEHAVIOR   0x04

/* Reserved for future use */
#define MPD_BM_RFU                         0xF8

/* End of MpsProtocolDependencyBm1 */

/*---------------------------------------------------------------------------
 * MultiProfileEventType type
 *
 *      Indicates the type of a Multi-Profile callback's event during an MPS
 *      callback function call. These events are delivered in the callback
 *      structure's "event" field.
 */
typedef U8 MultiProfileEventType;

/* The results of the MPS_StartQuery() request */
#define MPSEVENT_QUERY_RESULTS  1

/* End of MultiProfileEventType */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * MpsMpSdFeatureFlags structure
 *
 * Used to register and respond to query requests for the Multi-Profile Single 
 * Device 64 bit bitmask stored in the SDP record. Each byte is stored as an
 * 8 bit value for portability.
 *
 */
typedef struct _MpsMpSdFeatureFlags {

    /* Reserved for future use */
    U8  rfu8;

    /* Reserved for future use */
    U8  rfu7;

    /* Reserved for future use */
    U8  rfu6;

    /* Flag bits 39-32 */
    MpsMpSdFeatureFlagsBm5 bm5;

    /* Flag bits 31-24 */
    MpsMpSdFeatureFlagsBm4 bm4;

    /* Flag bits 23-16 */
    MpsMpSdFeatureFlagsBm3 bm3;

    /* Flag bits 15-8 */
    MpsMpSdFeatureFlagsBm2 bm2;

    /* Flag bits 7-0 */
    MpsMpSdFeatureFlagsBm1 bm1;
} MpsMpSdFeatureFlags;


/*---------------------------------------------------------------------------
 * MpsMpMdFeatureFlags structure
 *
 * Used to register and respond to query requests for the Multi-Profile 
 * Multiple Device 64 bit bitmask stored in the SDP record. Each byte is 
 * stored as an 8 bit value for portability.
 *
 */
typedef struct _MpsMpMdFeatureFlags {
    /* Reserved for future use */
    U8  rfu8;

    /* Reserved for future use */
    U8  rfu7;
    
    /* Reserved for future use */
    U8  rfu6;

    /* Reserved for future use */
    U8  rfu5;

    /* Reserved for future use */
    U8  rfu4;

    /* Flag bits 23-16 */
    MpsMpMdFeatureFlagsBm3 bm3;
    
    /* Flag bits 15-8 */
    MpsMpMdFeatureFlagsBm2 bm2;

    /* Flag bits 7-0 */
    MpsMpMdFeatureFlagsBm1 bm1;

} MpsMpMdFeatureFlags;


/*---------------------------------------------------------------------------
 * MpsProtocolDependencyFlags structure
 *
 * Represents the bitmask for the Supported Profile and Protocol Dependency
 * Bitmask.
 *
 */
typedef struct _MpsProtocolDependencyFlags {
    /* Reserved for future use */
    U8  rfu;

    /* Flag bits 7-0 */
    MpsProtocolDependencyBm1 bm1;
} MpsProtocolDependencyFlags;


/*---------------------------------------------------------------------------
 * MultiProfileInfo structure
 *
 * Used to pass information when registering a Multi-Profile record or to
 * pass information about a remote Multi-Profile record resulting from an SDP 
 * query.
 *
 */
typedef struct _MultiProfileInfo {

    ListEntry                  node;   /* For internal use only. */

    /* Multi-Profile Specification Version */
    U16                        specVer;

    /* Multi-Profile Single Device supported scenarios bitmask */
    MpsMpSdFeatureFlags        mpsdFlags;
    
    /* Multi-Profile Multiple Device supported scenarios bitmask */
    MpsMpMdFeatureFlags        mpmdFlags;

    /* Supported Profile and Protocol Dependencies bitmask */
    MpsProtocolDependencyFlags depFlags;

    /* The service description is a string containing a brief description of the
     * service. This value is not null terminated.
     * Note: This value is currently used only for retrieving the service description
     *       from the remote device. The current implementation of the Multi-Profile in
     *       Blue SDK does not support setting a service Description string.
     */
    U8                         serviceDesc[MPS_MAX_SERV_DESC_LEN];

    /* The length of the string in the serviceDesc field */
    U16                        serviceDescLen;

} MultiProfileInfo;


/*---------------------------------------------------------------------------
 * MultiProfileRegisterParms structure
 *
 * Contains parameters used by Multi-Profile registration functions. The 
 * entire structure should be initialized to zero, and parameters filled 
 * in as required.
 */
typedef struct _MultiProfileRegisterParms {

    /* Information about the device being registered */
    MultiProfileInfo multiProfileInfo[MPS_MAX_LOCAL_DEVICE_RECORDS];

    /* The number of device records to add */
    U8               numRecords;

} MultiProfileRegisterParms;


/*---------------------------------------------------------------------------
 * MultiProfileCallbackParms structure
 *
 * Contains parameters by the MultiProfile query function.
 *
 */
typedef struct _MultiProfileCallbackParms {

    /* MPS upper layer event. Always valid. */
    MultiProfileEventType event;

    /* Status - In the case of a parsing failure, partially parsed service
     * information will be made available in last record instance.
     */
    BtStatus        status;

    /* The number of device records found (last record may be partially 
     * parsed if the status indicates a failure).
     */
    U8              numRecords;

    /* List of MultiProfileIdInfo's */
    ListEntry       recordList;

} MultiProfileCallbackParms;


/*---------------------------------------------------------------------------
 * MultiProfileCallback type
 *
 *     A function of this type is called to indicate MultiProfile Callback Events.
 */
typedef void (*MultiProfileCallback)(BtRemoteDevice *remDev, MultiProfileCallbackParms *cbParms);

/* End of MultiProfileCallback */

/****************************************************************************
 *
 * Section: Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * MPS_Init()
 *
 *      Initialize the MPS component.  This must be the first MPS
 *      function called by the application layer.
 *
 * Returns:
 *     TRUE - Initialization was successful.
 *
 *     FALSE - Initialization failed.
 */
BOOL MPS_Init(void);

/*---------------------------------------------------------------------------
 * MPS_Deinit()
 *
 *     Deinitializes the MPS module. MPS_Init() must later be called
 *     again to use any MPS function call.  This function does no
 *     error checking so do NOT call when MPS is in use.
 *
 * Requires:
 *     L2CAP_DEREGISTER_FUNC set to XA_ENABLED
 */
void MPS_Deinit(void);

#if SDP_SERVER_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * MPS_RegisterRecords()
 *
 *     Registers the Multi-Profile Specification SDP records.
 *
 * Parameters:
 *     Parms - Multi-Profile records to be registerd in SDP.
 *
 * Returns:
 *     BT_STATUS_INVALID_PARM - Invalid or in use profile type.
 *
 *     BT_STATUS_SUCCESS - The MPS is initialized.
 *
 *     BT_STATUS_FAILED - Problem initializing the MPS profile.
 *
 *     BT_STATUS_BUSY - MPS is already registered.
 */
BtStatus MPS_RegisterRecords(const MultiProfileRegisterParms *Parms);

/*---------------------------------------------------------------------------
 * MPS_DeregisterRecords()
 *
 *     Deregisters the Multi-Profile Specification including removing the 
 *     SDP records.
 *
 * Parameters:
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The MPS profile was deregistered.
 *
 *     BT_STATUS_INVALID_PARM - The MPS profile is not registered.
 *
 *     BT_STATUS_BUSY - The MPS profile could not be deregistered because
 *          it is currently processing an operation.
 *
 *     BT_STATUS_FAILED - The MPS profile failed to deinitialize.
 */
BtStatus MPS_DeregisterRecords(void);
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */


/*---------------------------------------------------------------------------
 * MPS_StartQuery()
 *
 *     Initiates an SDP query to get the Device Id information from the remote
 *     device.
 *
 * Parameters:
 *      remDev - the pointer to a BtRemoteDevice.
 *
 *      callback - the callback routine for indicating the MultiProfile results.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The MPS profile was deregistered.
 *
 *     BT_STATUS_INVALID_PARM - The remDev is invalid.
 *
 *     BT_STATUS_FAILED - The SDP query failed to be initiated.
 */
BtStatus MPS_StartQuery(BtRemoteDevice *remDev, MultiProfileCallback callback);

#endif /* __MPS_H_ */
