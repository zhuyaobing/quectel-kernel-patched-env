/****************************************************************************
 *
 * File:
 *     csrbccmd.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description: This file is the header file for CSR BCCMD API.
 *
 * Created:     April 11, 2014
 *
 * Copyright 2014-2017 OpenSynergy GmbH
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

#ifndef __CSRBCCMD_H
#define __CSRBCCMD_H

#include "me.h"
#include "oslib.h"

/*---------------------------------------------------------------------------
 * CSR BCCMD API layer
 * 
 *     The CSR BCCMD API defines procedures for communicating with CSR
 *     controllers using proprietary BCCMD and HQ protocols.  It can be 
 *     used to send vendor specific commands to CSR controllers.
 *  
 *     Only a small subset of available commands are implemented, but this
 *     code can serve as a model for adding support for more commands in
 *     the future.
 *  
 *     This API also provides code for the initialization of the CSR BlueCore
 *     8350 A7 controller for Wide-band speech over eSCO.  It makes use of
 *     the on-board DSP Manager.
 *  
 *     This code requires BlueSDK 4.3.1 or later software.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HCC_VENDOR_SPECIFIC 
 *  
 *     This constant identifies the HCI command type used for 
 *     tunnelling BCCMD and HQ messages over the HCI.
 */
#define HCC_VENDOR_SPECIFIC   0xFC00

/*---------------------------------------------------------------------------
 * CSRAUDIO_DSP_CONFIG_DATA_SIZE 
 *  
 *      The Max data size for DSP configuration message.
 */
#define CSR_DSP_CONFIG_DATA_SIZE         112

/*---------------------------------------------------------------------------
 * CSRAUDIO_DSP_MAX_ASCII_LINE_LEN 
 *  
 *      The Max Line length in a DSP patch file (*.pat).
 */
#define CSR_DSP_MAX_ASCII_LINE_LEN       (7*CSR_DSP_CONFIG_DATA_SIZE/2)

/*---------------------------------------------------------------------------
 * CSR_DSP_PATCH_FILE_LOCATION 
 *  
 *      The location of a CSR DSP patch file (dsp.pat).  The chip type will
 *      be appended (i.e. "csr/8311" or "csr/8350".  If the files are not
 *      found at this location, the code will call OS_GetDocumentsDir() to
 *      get the base directory and append 'csr/8350' or 'csr/8311'.
 */
#ifndef CSR_DSP_PATCH_FILE_LOCATION
#define CSR_DSP_PATCH_FILE_LOCATION  "../../../bluesdk/external/hcitrans/modinit/"
#endif

/* Maximum length of a file name and path */
#define MAX_FNAME_SIZE 260

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * CsrChipType type 
 *  
 *     Defines the supported chip types.
 */
typedef U8 CsrChipType;

#define CSR_CHIP_TYPE_8350  0x01
#define CSR_CHIP_TYPE_8311  0x02

/* End of CsrChipType */

/*---------------------------------------------------------------------------
 * CsrBccmdChannelType type 
 *  
 *     Defines the channel type for BCCMD and HQ channels.  This value is
 *     OR'd together with CsrBccmdFragmentType to define the first byte
 *     of a message.
 */
typedef U8 CsrBccmdChannelType;

#define CSR_BCCMD_CHANNEL_ID  0x02
#define CSR_HQ_CHANNEL_ID     0x03

/* End of CsrBccmdChannelType */ 

/*---------------------------------------------------------------------------
 * CsrBccmdFragmentType type 
 *  
 *     Defines the fragmentation for BCCMD and HQ messages.  This code
 *     only makes use of CSR_BCCMD_FRAGMENT_SINGLE. This value is
 *     OR'd together with CsrBccmdChannelType to define the first byte
 *     of a message.
 */
typedef U8 CsrBccmdFragmentType;

#define CSR_BCCMD_FRAGMENT_FIRST    0x80
#define CSR_BCCMD_FRAGMENT_LAST     0x40
#define CSR_BCCMD_FRAGMENT_SINGLE   0xC0

/* End of CsrBccmdFragmentType */

/*---------------------------------------------------------------------------
 * CsrBccmdMsgType type
 *  
 *     Defines the message types for BCCMD and HQ. 
 */
typedef U16 CsrBccmdMsgType;

#define CSR_BCCMD_TYPE_GETREQ  0x0000
#define CSR_BCCMD_TYPE_GETRSP  0x0001
#define CSR_BCCMD_TYPE_SETREQ  0x0002

/* End of CsrBccmdMsgType */

/*---------------------------------------------------------------------------
 * CsrBccmdVaridType type
 *  
 *     Defines the Variable IDs (varid) that are used in BCCMD/HQ messages.
 *     There are too many to define right here, but a few are included for
 *     convenience.
 */
typedef U16 CsrBccmdVaridType;

/* Group: BCCMD Requests */
#define CSR_BCCMD_STREAM_TRANSFORM_DISC  0x486D
#define CSR_BCCMD_ENABLE_SCO_STREAMS     0x4876     
#define CSR_BCCMD_STREAM_GET_SOURCE      0x505A
#define CSR_BCCMD_STREAM_GET_SINK        0x505B
#define CSR_BCCMD_STREAM_CONFIGURE       0x505C
#define CSR_BCCMD_STREAM_CONNECT         0x505E
#define CSR_BCCMD_START_OPERATORS        0x5070
#define CSR_BCCMD_STOP_OPERATORS         0x5071
#define CSR_BCCMD_DESTROY_OPERATORS      0x5073
#define CSR_BCCMD_OPERATOR_MESSAGE       0x5074
#define CSR_BCCMD_CREATE_OPERATOR_C      0x5075
#define CSR_BCCMD_PS                     0x7003

/* Group: HQ Reqeusts */
#define CSR_HQ_FAULT                     0x1000
#define CSR_HQ_DSP_CONFIG_REQ            0x101C

/* End of CsrBccmdVaridType */

/*---------------------------------------------------------------------------
 * CsrBccmdStatus type 
 *  
 *     Defines status codes for BCCMD and HQ messages. 
 */
typedef U16 CsrBccmdStatus;

#define CSR_BCCMD_STATUS_OK                 0x0000
#define CSR_BCCMD_STATUS_NO_SUCH_VARID      0x0001
#define CSR_BCCMD_STATUS_TOO_BIG            0x0002
#define CSR_BCCMD_STATUS_NO_VALUE           0x0003
#define CSR_BCCMD_STATUS_BAD_REQ            0x0004
#define CSR_BCCMD_STATUS_NO_ACCESS          0x0005
#define CSR_BCCMD_STATUS_READ_ONLY          0x0006
#define CSR_BCCMD_STATUS_WRITE_ONLY         0x0007
#define CSR_BCCMD_STATUS_ERROR              0x0008
#define CSR_BCCMD_STATUS_PERMISSION_DENIED  0x0009
#define CSR_BCCMD_STATUS_TIMEOUT            0x000a

/* End of CsrBccmdStatus */

/*---------------------------------------------------------------------------
 * CsrStreamType type 
 *  
 *     Defines the various stream types supported by the DSP Manager.  This
 *     code only makes us of CSR_STREAM_TYPE_CODEC and CSR_STREAM_TYPE_SCO,
 *     which are required to set up Wide-band speech over eSCO.
 */
typedef U16 CsrStreamType;

#define CSR_STREAM_TYPE_PCM     0x0001
#define CSR_STREAM_TYPE_I2S     0x0002
#define CSR_STREAM_TYPE_CODEC   0x0003
#define CSR_STREAM_TYPE_FM      0x0004
#define CSR_STREAM_TYPE_SPDIF   0x0005
#define CSR_STREAM_TYPE_MIC     0x0006
#define CSR_STREAM_TYPE_SCO     0x0009

/* End of CsrStreamType */

/*---------------------------------------------------------------------------
 * CsrDspCapabilityType type 
 *  
 *     Defines DSP capabilities required for setting up Wide-band speech.
 *     Only a few are included for convenience.
 */
typedef U16 CsrDspCapabilityType;

#define CSR_DSP_CAP_SCO_SEND  0x0003
#define CSR_DSP_CAP_SCO_RECV  0x0004
#define CSR_DSP_CAP_WBS_SEND  0x0005
#define CSR_DSP_CAP_WBS_RECV  0x0006

/* End of CsrDspCapabilityType */

/*---------------------------------------------------------------------------
 * CsrCodecConfigKey type 
 *  
 *     Defines the CODEC attributes that are configured when setting up a
 *     wide-band speech connection.  Only a few are included for convenience.
 */
typedef U16 CsrCodecConfigKey;

#define CSR_CFG_KEY_PCM_SYNC_RATE         0x0100
#define CSR_CFG_KEY_PCM_MASTER_CLOCK_RATE 0x0101
#define CSR_CFG_KEY_PCM_MASTER_SLAVE      0x0102
#define CSR_CFG_KEY_PCM_SLOT_COUNT        0x0103
#define CSR_CFG_KEY_PCM_SHORT_SYNC        0x0105
#define CSR_CFG_KEY_PCM_SIGN_EXTEND       0x0107
#define CSR_CFG_KEY_PCM_BIT_SIZE          0x0114
#define CSR_CFG_KEY_CODEC_INPUT_RATE      0x0300
#define CSR_CFG_KEY_CODEC_OUTPUT_RATE     0x0301
#define CSR_CFG_KEY_CODEC_INPUT_GAIN      0x0302
#define CSR_CFG_KEY_CODEC_OUTPUT_GAIN     0x0303

/* End of CsrCodecConfigKey */

/*---------------------------------------------------------------------------
 * CsrEventType 
 *  
 *     Defines the events sent to the CSR callback. 
 */
typedef U8 CsrEventType;

/** A request has completed (see CsrSendBccmd).
 *
 *  During this callback, the 'p.bccmd' parameter is valid.
 */
#define CSR_EVENT_COMMAND_COMPLETE            0x01

/** A HQ request was received. The application should
 *  call CSR_SendHqRsp() to respond to this event.
 *
 *  During this callback, the 'p.hqreq' parameter is valid.
 */
#define CSR_EVENT_HQ_REQUEST                  0x02

/** A HQ response was sent to the controller. 
 *  
 *  During this callback, the 'p.payload' pareamter is valid.
 */
#define CSR_EVENT_HQ_RESPONSE_SENT            0x03

/** A request to get a PSKEY has completed (see CSR_SendPsGet).
 *
 *  During this callback, the 'p.psKeyRsp' parameter is valid.
 */
#define CSR_EVENT_PS_GET_COMPLETE             0x05

/** A request to enable SCO streams has completed (see
 *  CSR_SendEnableScoStreams).
 */
#define CSR_EVENT_ENABLE_SCO_STREAMS_COMPLETE 0x06

/** A request to get the Source ID has completed (see
 *  CSR_SendBccmdGetSource).
 *
 *  During this callback, the 'p.sourceId' parameter is valid.
 */
#define CSR_EVENT_GET_SOURCE_COMPLETE         0x07

/** A request to get the Sink ID has completed (see CSR_SendBccmdGetSink).
 *
 *  During this callback, the 'p.sinkId' parameter is valid.
 */
#define CSR_EVENT_GET_SINK_COMPLETE           0x08

/** A request to connect to streams has completed (see
 *  CSR_SendBccmdStreamConnect).
 *
 *  During this callback, the 'p.transformId' parameter is valid.
 */
#define CSR_EVENT_STREAM_CONNECT_COMPLETE     0x09

/** A request to disconnect a transform has completed (see
 *  CSR_SendBccmdStreamDisconnect).
 */
#define CSR_EVENT_STREAM_DISC_COMPLETE        0x0a

/** A request to stop an operator has completed (see
 *  CSR_SendBccmdStopOperators).
 */
#define CSR_EVENT_STOP_OP_COMPLETE            0x0b

/** A request to create an operator has completed (see
 *  CSR_SendBccmdCreateOpCap).
 *
 *  During this callback, the 'p.operatorId' parameter is valid.
 */
#define CSR_EVENT_CREATE_OP_CAP_COMPLETE      0x0c

/** A request to destroy an operator has completed (see
 *  CSR_SendBccmdDestroyOperators).
 */
#define CSR_EVENT_DESTROY_OP_COMPLETE         0x0d

/** A request to start operators has completed (see
 *  CSR_SendBccmdStartOperators).
 */
#define CSR_EVENT_START_OP_COMPLETE           0x0e

/** A request to configure a stream has completed (see
 *  CSR_SendBccmdStreamConfig).
 */
#define CSR_EVENT_STREAM_CONFIG_COMPLETE      0x0f

/** Audio configuration is complete.  (see CSR_StartAudioConfig)
 * 
 */
#define CSR_EVENT_AUDIO_CONFIG_COMPLETE       0x10

/* Unknown event */
#define CSR_EVENT_UNKNOWN                     0xff

/* End of CsrEventType */

/* Forward References */
typedef struct _CsrContext CsrContext;
typedef struct _CsrHciBccmd CsrHciBccmd;
typedef struct _CsrEvent CsrEvent;
typedef struct _CsrDspPatchData CsrDspPatchData;

/*---------------------------------------------------------------------------
 *  CsrCallback type
 *  
 *      Defines the callback function for the CSR API.
 */
typedef void (*CsrCallback)(CsrContext *ctx, CsrEvent *event);

/* End of CsrCallback */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * CsrContext 
 *  
 * Defines the global context for the CSR API
 */
struct _CsrContext {

    /* Used Internally */
    CsrCallback  callback;
    CsrChipType  chipType;
    ListEntry    bccmdList;
    ListEntry    deferredBccmdList;
    U8           audioInitState;
    U16          seqNo;
    U8           patternBuff[4];
    HciCommand   hcicmd;
    BtHandlerCtx globalHandler;
    BOOL         cmdInProgress;
    ListEntry    csrDspPatchList;
    U16          csrDspNumPatchBlocks;
    U16          audioSourceId;
    U16          audioSinkId;
};

/*---------------------------------------------------------------------------
 * CsrHciBccmd structure
 *  
 *     Defines a BCCMD command or HQ response. 
 */
struct _CsrHciBccmd {

    /* Used internally */
    ListEntry       node;
    CsrContext     *ctx;
    MeCommandToken  hciCmdToken;
    BOOL            internal;

    /* Length of the BCCMD or HQ payload */
    U16             payloadLen;

    /* Pointer to the BCCMD or HQ payload */
    U8             *payload;
};

/*---------------------------------------------------------------------------
 * CsrEvent structure 
 *  
 *     Defines the events and parameters sent to CsrCallback 
 */
struct _CsrEvent {

    /* The event type */
    CsrEventType event;

    /* The status of the event */
    CsrBccmdStatus status;

    /* The value of BCCMD varid */
    CsrBccmdVaridType varid;

    /* Callback parameters object, depending on "event" */
    union _p {

        /* Valid when CSR_EVENT_COMMAND_COMPLETE is received. */
        CsrHciBccmd *bccmd;

        /* Valid when CSR_EVENT_HQ_REQUEST is received. */
        CsrHciBccmd *hqreq;

        /* Valid when CSR_EVENT_GET_SOURCE_COMPLETE is received. */
        U16 sourceId;

        /* Valid when CSR_BCCMD_STREAM_GET_SINK is received. */
        U16 sinkId;

        /* Valid when CSR_EVENT_GET_CONNECT_COMPLETE is received. */
        U16 transformId;

        /* Valid when CSR_EVENT_CREATE_OP_CAP_COMPLETE is received. */
        U16 operatorId;

        /* Valid when CSR_EVENT_HQ_RESPONSE_SENT is received */
        const U8 *payload;

        /* Valid when CSR_EVENT_PS_SET_COMPLETE and
         * CSR_BCCMD_TYPE_GETREQ are received.
         */
        struct _psKeyRsp {
            U16 psKey;
            U16 len;
            U8 *parms;
        } psKeyRsp;
    } p;
};

/*---------------------------------------------------------------------------
 * _CsrDspPatchData 
 *  
 *     Defines a block of DSP configuration data. 
 */
struct _CsrDspPatchData {

    /* Used internally */
    ListEntry  node;

    /* DSP patch data */
    U8         data[CSR_DSP_CONFIG_DATA_SIZE];
};

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * CSR_Init()
 *
 *     Initialize the CSR BCCMD API, and returns the context.  A side-effect
 *     of this is to load a CSR patch file for the DSP (if it exists).  The
 *     format of the patch file is defined by CSR, and has the name 'dsp.pat'.
 *     It must be placed in the directory returned by OS_GetDocumentsDir()
 *     appended with 'csr/8311' or 'csr/8350'.
 * 
 * Parameters:
 * 
 *     callback - A function to receive callback events.
 * 
 *     chipType - The type of chip that will be initialized.
 * 
 * Returns:
 * 
 *     The context that has been allocated for managing the transmission
 *     of BCCMD and HQ packets.  This context must be passed into most
 *     API functions.
 * 
 */
CsrContext * CSR_Init(CsrCallback callback, CsrChipType chipType);

/*---------------------------------------------------------------------------
 * CSR_Deinit()
 *
 *     Deinitialize the CSR BCCMD API, whose context is specified.
 * 
 * Parameters:
 * 
 *     ctx - The context of the BCCMD API
 *
 */
void CSR_Deinit(CsrContext *ctx);

/*---------------------------------------------------------------------------
 * CSR_AllocHciBccmd()
 *
 *     Allocate memory for a BCCMD command.
 * 
 * Parameters:
 * 
 *     payloadSize - The size of the payload for the command.
 * 
 *     payload - A pointer to the payload pointer.  The pointer to the
 *         allocated memory will be placed in this pointer.
 *
 * Returns:
 * 
 *     A pointer to the allocated structure.
 * 
 */
CsrHciBccmd * CSR_AllocHciBccmd(U16 payloadSize, U8 **payload);

/*---------------------------------------------------------------------------
 * CSR_CreateBccmdHeader() 
 *  
 *     Creates the Bccmd header and places it in the Bccmd HCI command. 
 *     The payload length must be an even number, because the Bccmd 
 *     payload is made up of 16 bit values. 
 * 
 * Parameters:
 * 
 *     ctx - The context allocated with CSR_Init()
 * 
 *     bccmd - The BCCMD data structure where the header will be placed
 * 
 *     type - The BCCMD message type.
 * 
 *     varid - The BCCMD varid.
 * 
 *     bccmdStatus - The status of the command/response
 *
 * Returns:
 * 
 *     BT_STATUS_SUCCESS - The header was created.
 * 
 *     BT_STATUS_INVALID_PARM - The context or command pointer was invalid.
 */
BtStatus CSR_CreateBccmdHeader(CsrContext       *ctx, 
                               CsrHciBccmd      *bccmd, 
                               CsrBccmdMsgType   type, 
                               CsrBccmdVaridType varid, 
                               CsrBccmdStatus    bccmdStatus);

/*---------------------------------------------------------------------------
 * CSR_CreateHqHeader() 
 *  
 *     Creates the HQ message header and places it in the HQ HCI 
 *     command. The payload length must be an even number, because
 *     the HQ payload is made up of 16 bit values. 
 * 
 * Parameters:
 * 
 *     bccmd - The BCCMD data structure where the header will be placed.
 * 
 *     type - The BCCMD message type.
 *  
 *     seqNo - The sequence number of the command.
 * 
 *     varid - The BCCMD varid.
 * 
 *     bccmdStatus - The status of the command/response.
 *  
 * Returns:
 * 
 *     BT_STATUS_SUCCESS - The header was created.
 * 
 *     BT_STATUS_INVALID_PARM - The context or command pointer was invalid.
 */
BtStatus CSR_CreateHqHeader(CsrHciBccmd      *bccmd, 
                            CsrBccmdMsgType   type, 
                            U16               seqNo,
                            CsrBccmdVaridType varid, 
                            CsrBccmdStatus    bccmdStatus);

/*---------------------------------------------------------------------------
 * CSR_FreeHciBccmdHq()
 * 
 *     Frees a previously allocated command. 
 * 
 * Parameters:
 * 
 *     bccmd - A pointer to the BCCMD memory that will be freed.
 */
void CSR_FreeHciBccmdHq(CsrHciBccmd *bccmd);

/*---------------------------------------------------------------------------
 * CSR_SendBccmd()
 *
 *     Send a BCCMD command.
 * 
 * Parameters:
 * 
 *     ctx - The context returned by CSR_Init().
 * 
 *     bccmd - A pointer to the BCCMD command.
 * 
 * Returns:
 * 
 *     BT_STATUS_INVALID_PARM - The context or command pointer is invalid.
 * 
 *     BT_STATUS_IN_USE - The command is already queued for sending.
 * 
 *     BT_STATUS_PENDING - The command was sent.
 * 
 *     See ME_SendHciCommandSync() for other codes.
 */
BtStatus CSR_SendBccmd(CsrContext *ctx, CsrHciBccmd *bccmd);

/*---------------------------------------------------------------------------
 * CSR_SendHqRsp()
 *
 *     Send a response to an HQ command.
 * 
 * Parameters:
 * 
 *     ctx - The context returned by CSR_Init().
 * 
 *     bccmd - A pointer to the BCCMD command.
 * 
 * Returns:
 * 
 *     BT_STATUS_INVALID_PARM - The context or command pointer is invalid.
 * 
 *     BT_STATUS_PENDING - The command was sent.
 * 
 *     See ME_SendHciCommandVendorSpecific() for other codes.
 * 
 */
BtStatus CSR_SendHqRsp(CsrContext *ctx, CsrHciBccmd *bccmd);

/*---------------------------------------------------------------------------
 * CSR_SendPsGet()
 *
 *     Gets a PsKey.
 * 
 * Parameters:
 * 
 *     ctx - The context returned by CSR_Init().
 * 
 *     key - The persistent key to get.
 * 
 *     parmLen - The max length of the data to be returned.
 * 
 * Returns:
 * 
 *     BT_STATUS_FAILED - Could not allocate memory for this command.
 * 
 *     BT_STATUS_INVALID_PARM - The context pointer is invalid.
 * 
 *     BT_STATUS_PENDING - The command was sent, when a response is received,
 *         the application will be notified with CSR_EVENT_PS_GET_COMPLETE.
 * 
 *     See ME_SendHciCommandSync() for other codes.
 */
BtStatus CSR_SendPsGet(CsrContext *ctx, U16 key, U16 parmLen);

/*---------------------------------------------------------------------------
 * CSR_SendEnableScoStreams() 
 *  
 * Parameters: 
 *  
 *     ctx - The context returned by CSR_Init().
 *  
 *     enable - TRUE to enable SCO Streams, FALSE to disable.
 * 
 * Returns:
 * 
 *     BT_STATUS_FAILED - Could not allocate memory for this command.
 * 
 *     BT_STATUS_INVALID_PARM - The context pointer is invalid.
 * 
 *     BT_STATUS_PENDING - The command was sent, when a response is received,
 *         the application will be notified with CSR_EVENT_ENABLE_SCO_STREAMS_COMPLETE.
 * 
 *     See ME_SendHciCommandSync() for other codes.
 */
BtStatus CSR_SendEnableScoStreams(CsrContext *ctx, BOOL enable);

/*---------------------------------------------------------------------------
 * CSR_SendStreamConfig()
 * 
 * Parameters: 
 *  
 *     ctx - The context returned by CSR_Init().
 *  
 *     stream - The stream to configure.
 *  
 *     key - The key to configure.
 *  
 *     value - The value to set.
 * 
 * Returns:
 * 
 *     BT_STATUS_FAILED - Could not allocate memory for this command.
 * 
 *     BT_STATUS_INVALID_PARM - The context pointer is invalid.
 * 
 *     BT_STATUS_PENDING - The command was sent, when a response is received,
 *         the application will be notified with CSR_EVENT_STREAM_CONFIG_COMPLETE.
 * 
 *     See ME_SendHciCommandSync() for other codes.
 */
BtStatus CSR_SendStreamConfig(CsrContext *ctx, U16 stream, 
                              U16 key, U16 value);

/*---------------------------------------------------------------------------
 * CSR_SendGetSource()
 * 
 * Parameters: 
 *  
 *     ctx - The context returned by CSR_Init().
 *  
 *     type - The source endpoint type.
 *  
 *     parm1 - Parameter 1 (based on type)
 *  
 *     parm2 - Parameter 2 (based on type)
 * 
 * Returns:
 * 
 *     BT_STATUS_FAILED - Could not allocate memory for this command.
 * 
 *     BT_STATUS_INVALID_PARM - The context pointer is invalid.
 * 
 *     BT_STATUS_PENDING - The command was sent, when a response is received,
 *         the application will be notified with CSR_EVENT_GET_SOURCE_COMPLETE.
 * 
 *     See ME_SendHciCommandSync() for other codes.
 */
BtStatus CSR_SendGetSource(CsrContext *ctx, 
                           CsrStreamType type, 
                           U16 parm1, U16 parm2);

/*---------------------------------------------------------------------------
 * CSR_SendGetSink()
 * 
 * Parameters: 
 *  
 *     ctx - The context returned by CSR_Init().
 * 
 *     type - The source endpoint type.
 *  
 *     parm1 - Parameter 1 (based on type)
 *  
 *     parm2 - Parameter 2 (based on type)
 * 
 * Returns:
 * 
 *     BT_STATUS_FAILED - Could not allocate memory for this command.
 * 
 *     BT_STATUS_INVALID_PARM - The context pointer is invalid.
 * 
 *     BT_STATUS_PENDING - The command was sent, when a response is received,
 *         the application will be notified with CSR_EVENT_GET_SINK_COMPLETE.
 * 
 *     See ME_SendHciCommandSync() for other codes.
 */
BtStatus CSR_SendGetSink(CsrContext  *ctx, U16 type, 
                         U16 parm1, U16 parm2);

/*---------------------------------------------------------------------------
 * CSR_SendStreamConnect()
 * 
 * Parameters: 
 *  
 *     ctx - The context returned by CSR_Init().
 *  
 *     sourceId - The source ID to connect.
 *  
 *     sinkId - The sink ID to connect.
 * 
 * Returns:
 * 
 *     BT_STATUS_FAILED - Could not allocate memory for this command.
 * 
 *     BT_STATUS_INVALID_PARM - The context pointer is invalid.
 * 
 *     BT_STATUS_PENDING - The command was sent, when a response is received,
 *         the application will be notified with CSR_EVENT_STREAM_CONNECT_COMPLETE.
 * 
 *     See ME_SendHciCommandSync() for other codes.
 */
BtStatus CSR_SendStreamConnect(CsrContext  *ctx, 
                               U16 sourceId, U16 sinkId);

/*---------------------------------------------------------------------------
 * CSR_SendCreateOpCap()
 * 
 * Parameters: 
 *  
 *     ctx - The context returned by CSR_Init().
 *  
 *     cap - The capability type for which the operator is created.
 * 
 * Returns:
 * 
 *     BT_STATUS_FAILED - Could not allocate memory for this command.
 * 
 *     BT_STATUS_INVALID_PARM - The context pointer is invalid.
 * 
 *     BT_STATUS_PENDING - The command was sent, when a response is received,
 *         the application will be notified with CSR_EVENT_CREATE_OP_CAP_COMPLETE.
 * 
 *     See ME_SendHciCommandSync() for other codes.
 */
BtStatus CSR_SendCreateOpCap(CsrContext  *ctx, CsrDspCapabilityType cap);

/*---------------------------------------------------------------------------
 * CSR_SendDestroyOperators()
 * 
 * Parameters: 
 *  
 *     ctx - The context returned by CSR_Init().
 *  
 *     op1 - The first operator.
 *  
 *     op2 - The second operator.
 * 
 * Returns:
 * 
 *     BT_STATUS_FAILED - Could not allocate memory for this command.
 * 
 *     BT_STATUS_INVALID_PARM - The context pointer is invalid.
 * 
 *     BT_STATUS_PENDING - The command was sent, when a response is received,
 *         the application will be notified with CSR_EVENT_DESTROY_OP_COMPLETE.
 * 
 *     See ME_SendHciCommandSync() for other codes.
 */
BtStatus CSR_SendDestroyOperators(CsrContext *ctx, U16 op1, U16 op2);

/*---------------------------------------------------------------------------
 * CSR_SendStopOperators()
 * 
 * Parameters: 
 *  
 *     ctx - The context returned by CSR_Init().
 *  
 *     op1 - The first operator.
 *  
 *     op2 - The second operator.
 * 
 * Returns:
 * 
 *     BT_STATUS_FAILED - Could not allocate memory for this command.
 * 
 *     BT_STATUS_INVALID_PARM - The context pointer is invalid.
 * 
 *     BT_STATUS_PENDING - The command was sent, when a response is received,
 *         the application will be notified with CSR_EVENT_STOP_OP_COMPLETE.
 * 
 *     See ME_SendHciCommandSync() for other codes.
 */
BtStatus CSR_SendStopOperators(CsrContext *ctx, U16 op1, U16 op2);

/*---------------------------------------------------------------------------
 * CSR_SendStartOperators()
 * 
 * Parameters: 
 *  
 *     ctx - The context returned by CSR_Init().
 *  
 *     sendOpId - The Send operator to start.
 *  
 *     RecvOpId - The Receive operator to start.
 * 
 * Returns:
 * 
 *     BT_STATUS_FAILED - Could not allocate memory for this command.
 * 
 *     BT_STATUS_INVALID_PARM - The context pointer is invalid.
 * 
 *     BT_STATUS_PENDING - The command was sent, when a response is received,
 *         the application will be notified with CSR_EVENT_START_OP_COMPLETE.
 * 
 *     See ME_SendHciCommandSync() for other codes.
 */
BtStatus CSR_SendStartOperators(CsrContext  *ctx, 
                                U16 sendOpId, U16 recvOpId);

/*---------------------------------------------------------------------------
 * CSR_SendStreamDisconnect()
 * 
 * Parameters: 
 *  
 *     ctx - The context returned by CSR_Init().
 * 
 * Returns:
 * 
 *     BT_STATUS_FAILED - Could not allocate memory for this command.
 * 
 *     BT_STATUS_INVALID_PARM - The context pointer is invalid.
 * 
 *     BT_STATUS_PENDING - The command was sent, when a response is received,
 *         the application will be notified with CSR_EVENT_STREAM_DISC_COMPLETE.
 * 
 *     See ME_SendHciCommandSync() for other codes.
 */
BtStatus CSR_SendStreamDisconnect(CsrContext  *ctx, U16 transformId);

/*---------------------------------------------------------------------------
 * CSR_SendHqDspConfigRsp()
 * 
 * Parameters:
 * 
 *     ctx - The context returned by CSR_Init().
 *  
 *     seqNo - The sequence number of the command/rsp.
 *  
 *     numBlocks - The total number of blocks available to send.
 *  
 *     pPatch - A pointer to the current patch block.
 * 
 * Returns:
 * 
 *     BT_STATUS_INVALID_PARM - The context or command pointer is invalid.
 * 
 *     BT_STATUS_PENDING - The command was sent. When this command has been
 *         sent, the application will be notified with CSR_EVENT_HQ_RESPONSE_SENT.
 * 
 *     See ME_SendHciCommandVendorSpecific() for other codes.
 * 
 */
BtStatus CSR_SendHqDspConfigRsp(CsrContext *ctx, U16 seqNo, U16 numBlocks, 
                                CsrDspPatchData *pPatch);

/*---------------------------------------------------------------------------
 * CSR_GetAudioSource()
 * 
 * Parameters:
 * 
 *     ctx - The context returned by CSR_Init().
 * 
 * Returns: 
 *  
 *     The Audio Source ID.
 */
U16 CSR_GetAudioSource(CsrContext *ctx);

/*---------------------------------------------------------------------------
 * CSR_GetAudioSink()
 * 
 * Parameters:
 * 
 *     ctx - The context returned by CSR_Init().
 * 
 * Returns:
 *  
 *     The Audio Sink ID. 
 */
U16 CSR_GetAudioSink(CsrContext *ctx);

/*---------------------------------------------------------------------------
 * CSR_CalculateOperatorSource()
 * 
 * Parameters:
 * 
 *     The Operator ID.
 *
 * Returns:
 * 
 *     The Operator Source ID.
 */
U16 CSR_CalculateOperatorSource(U16 operatorId, U16 port);

/*---------------------------------------------------------------------------
 * CSR_CalculateOperatorSink()
 * 
 * Parameters:
 * 
 *     The Operator ID.
 *  
 * Returns:
 *  
 *     The Operator Sink ID. 
 */
U16 CSR_CalculateOperatorSink(U16 operatorId, U16 port);

/*---------------------------------------------------------------------------
 * CSR_StartAudioConfig()
 * 
 * Parameters:
 * 
 *     ctx - The context returned by CSR_Init().
 *
 * Returns:
 * 
 *     BT_STATUS_FAILED - Could not allocate memory to send a command.
 * 
 *     BT_STATUS_INVALID_PARM - The context pointer is invalid.
 * 
 *     BT_STATUS_PENDING - Configuration was started, when a response is received,
 *         the application will be notified with CSR_AUDIO_CONFIG_COMPLETE.
 * 
 *     See ME_SendHciCommandSync() for other codes.
 */
BtStatus CSR_StartAudioConfig(CsrContext *ctx);

#endif /* __CSRBCCMD_H */

