/****************************************************************************
 *
 * File:
 *     mei.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file contains internal definitions for the Bluetooth
 *     Management Enity.
 *
 * Created:
 *     October 18, 1999
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

#ifndef __MEI_H
#define __MEI_H

#include "me.h"
#include "sec.h"
#include "sys/mexp.h"
#include "utils.h"
#include "hcitrans.h"
#include "sys/hci.h"
#include "loglink.h"
#include "brphylink.h"
#if NUM_LE_DEVICES > 0
#include "sys/lephylink.h"
#endif /* NUM_LE_DEVICES > 0 */
#include "a2me.h"
#include "ampcontrol.h"
#include "l2cap_i.h"

#if NUM_LE_DEVICES > 0
#include "sys/melei.h"
#endif /* NUM_LE_DEVICES > 0 */

#if NUM_SCO_CONNS > 0
#include "mesco.h"
#endif
#if NUM_KNOWN_DEVICES > 0
#include "medev.h"
#endif

#include "sys/mesecamp.h"

/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * BtOpMask is used to indicate which operations are in progress.
 */
typedef U16 BtOpMask;    /* Used internally */
#define BOM_INQUIRY            0x0001
#define BOM_CANCEL_INQUIRY     0x0002
#define BOM_ACCESS_MODE        0x0004
#define BOM_ACCESS_MODE_AUTO   0x0008
#define BOM_HCICOMMAND_BUSY    0x0010
#define BOM_INITRADIO_BUSY     0x0020
#define BOM_LE_INITRADIO_BUSY  0x0040

/*---------------------------------------------------------------------------
 *
 * BtOpType defines Bluetooth operations. The typedef is in me.h
 * LE operations are defined in melei.h and start at 0x80
 */
#define BOP_NOP                     0x00  /* No operation */
#define BOP_INQUIRY                 0x01  /* Inquiry operation */

#define BOP_LINK_CONNECT            0x05  /* Initiate a link connection */
#define BOP_LINK_ACCEPT             0x06  /* Accept an incoming link */
#define BOP_LINK_REJECT             0x07  /* Reject an incoming link */
#define BOP_LINK_DISCONNECT         0x08  /* Disconnet a link */
#define BOP_AUTHENTICATE            0x09  /* Authenticate the link */
#define BOP_LINK_KEY_RSP            0x0a  /* Respond to link key request */
#define BOP_USER_IO_RSP             0x0b  /* Respond to user IO request */
#define BOP_AUTHORIZE               0x0c  /* Authorization  operation */
#define BOP_ENCRYPTION              0x0d  /* Encryption operation */
#define BOP_SECURITY_MODE           0x0e  /* Set security mode operation */
#define BOP_ACCESS_MODE             0x0f  /* Change accessability mode operation */
#define BOP_CANCEL_INQUIRY          0x10  /* Cancel inquiry operation */
#define BOP_REM_NAME_REQ            0x11  /* Remote name request operation */
#define BOP_LOCAL_NAME_CHG          0x12  /* Local name change */

/* Mode change operations must be continguous with no other operations
 * between. Hold must be first and exit park mode must be last.
 */
#define BOP_HOLD_MODE               0x13  /* Hold mode operation */
#define BOP_SNIFF_MODE              0x14  /* Enter sniff mode operation */
#define BOP_EXIT_SNIFF_MODE         0x15  /* Exit sniff mode */
#define BOP_PARK_MODE               0x16  /* Enter park mode */
#define BOP_EXIT_PARK_MODE          0x17  /* Exit park mode */
/* end of mode change operations */

#define BOP_SWITCH_ROLE             0x18
#define BOP_WRITE_COD               0x19  /* Write class of device */
#define BOP_GEN_COMMAND             0x1a  /* General command */
#define BOP_SCO_CONNECT             0x1b  /* Initiate a SCO link connection */
#define BOP_SCO_ACCEPT              0x1c  /* Accept an incoming link */
#define BOP_SCO_REJECT              0x1d  /* Reject an incoming link */
#define BOP_SCO_DISCONNECT          0x1e  /* Disconnet a link */
#define BOP_LINK_IN_REJECT          0x1f  /* Internal link reject */
#define BOP_SCO_VOICESET            0x20  /* Set SCO voice parameters */
#define BOP_WRITE_LINK_POLICY       0x21  /* Set link policy */

#define BOP_WRITE_DEF_LNK_POL       0x23  /* Set default link policy */
#define BOP_SCO_SETUP               0x24  /* Initiate a SCO link setup (BT v1.2) */
#define BOP_CANCEL_CONNECT          0x25  /* Cancel a link connection */
#define BOP_CANCEL_REM_NAME         0x26  /* Cancel a remote name request */
#define BOP_CHANNEL_CLASS           0x27  /* Set channel classification for AFH */
#define BOP_DISCOVER_ROLE           0x28  /* Discover Role */
#define BOP_CANCEL_AUTHORIZE        0x29  /* Cancel Authorization request */
#define BOP_CHANGE_PKT_TYPE         0x2a  /* Change connection packet type */
#define BOP_SCO_PACKET_TYPE         0x2b  /* Change SCO connection packet type */
#define BOP_IO_CAP_RSP              0x2d  /* IO Capability Response */
#define BOP_REM_OOB_DATA_RSP        0x30  /* Respond to a remote OOB data request */
#define BOP_DENY_OOB_DATA_RSP       0x31  /* Negative reply to an OOB data request */
#define BOP_GET_LOCAL_OOB_DATA      0x32  /* Get the local OOB data */
#define BOP_READ_REM_FEATURES       0x33  /* Read the remote features */
#define BOP_READ_REM_EXT_FEAT       0x34  /* Read the remote extended features */
#define BOP_REFRESH_ENCRYPT_KEY     0x35  /* Refresh the encryption key */
#define BOP_SNIFF_SUBR_PARMS        0x36  /* Enable sniff subrating */
#define BOP_SET_INQ_MODE            0x37  /* Set the inquiry response mode */
#define BOP_SET_EXT_INQ_RESP        0x38  /* Set the extended inquiry response */
#define BOP_SET_ERR_DATA_REP        0x39  /* Set the erroneous data reporting */
#define BOP_SET_INQ_TX_PWR          0x40  /* Set the inquiry response TX power level */
#define BOP_SET_SMP_DEBUG_MODE      0x41  /* Set the simple pairing debug mode */
#define BOP_ENH_SCO_SETUP           0x42  /* Initiate a SCO link setup (BT v4.0 CSA2) */
#define BOP_ENH_SCO_ACCEPT          0x43  /* Accept an incoming SCO link (BT v4.0 CSA2) */
#define BOP_READ_CODECS             0x44  /* Read Local Supported Codecs (BT v4.0 CSA2) */
#define BOP_IO_CAP_REJ              0x45  /* IO Capability Rejection */
#define BOP_GET_LOCAL_OOB_EXT_DATA  0x46  /* Get the local OOB extended data */
#define BOP_REM_OOB_EXT_DATA_RSP    0x47  /* Respond to a remote OOB extended data request */
#define BOP_WRITE_AUTH_TIMEOUT      0x48  /* Write the authentication payload timeout */
#define BOP_READ_AUTH_TIMEOUT       0x49  /* Read the authentication payload timeout */
#define BOP_BUSY                    0xfe  /* Indicate op queue is busy */

/* LE operations are defined in melei.h and start at 0x80 */

/*---------------------------------------------------------------------------
 *
 * BtOpEvent defines events for the operation state machine.
 */
typedef U8 BtOpEvent;

#define BOE_NOP        0x00  /* No operation */
#define BOE_START      0x01  /* Start of operation */
#define BOE_CONT       0x02  /* Continue the operation */
#define BOE_TASK_END   0x03  /* Task finished successfully */
#define BOE_TASK_ERR   0x04  /* Task ended because of an error */

/*---------------------------------------------------------------------------
 *
 * Authenticate State of a Remote Device (BtAuthState). The typedef is in
 * me.h
 */
#define BAS_NOT_AUTHENTICATING   0x00 /* Start state */
#define BAS_START_AUTHENTICATE   0x01
#define BAS_REAUTHENTICATING     0x02
#define BAS_WAITING_FOR_CAP_IO_R 0x03
#define BAS_WAITING_KEY_REQ      0x04
#define BAS_SENDING_KEY          0x05
#define BAS_WAITING_CONF_R       0x07
#define BAS_WAITING_FOR_IO       0x08
#define BAS_WAITING_FOR_IO_R     0x09
#define BAS_WAITING_FOR_KEY      0x0A
#define BAS_WAITING_FOR_KEY_R    0x0B

/*---------------------------------------------------------------------------
 *
 * Authorize State of a Remote Device (BtAuthorizeState). The typedef is in
 * me.h
 */
#define BAS_NOT_AUTHORIZED     0x00
#define BAS_START_AUTHORIZE    0x01
#define BAS_AUTHORIZED         0x02
#define BAS_AUTHORIZED_SERVICE 0x03

/*---------------------------------------------------------------------------
 *
 * Encyrption State of a Remote Device (BtEncryptState). The typedef is in
 * me.h
 */
#define BES_NOT_ENCRYPTED      0x00
#define BES_START_ENCRYPT      0x01
#define BES_ENCRYPTED          0x02
#define BES_END_ENCRYPT        0x03

/*---------------------------------------------------------------------------
 *
 * Security Access State is used to keep track of the access procedure. The
 * typedef is in me.h.
 */

#define BAS_NO_ACCESS          0x00
#define BAS_AUTH_COMPLETE      0x01
#define BAS_AUTHORIZE_COMPLETE 0x02
#define BAS_ENCRYPT_COMPLETE   0x04
#define BAS_AUTH_REJECTED      0x08
#define BAS_AUTH_IO_PENDING    0x10

/*---------------------------------------------------------------------------
 *
 * Security Mode state is used to keep track of the security state.
 */

typedef U8 BtSecurityModeState;

#define BSMS_LEVEL2       0x00
#define BSMS_START_LEVEL3 0x01
#define BSMS_LEVEL3       0x02
#define BSMS_END_LEVEL3   0x03

/*---------------------------------------------------------------------------
 *
 * Link Receive Buffer State (BtLinkRxState) is used to track the L2CAP
 * receive buffer state. The typedef is in me.h.
 */

#define BRXS_COMPLETE      0x00
#define BRXS_COPYING       0x01
#define BRXS_FORWARDING    0x02
#define BRXS_FLUSH         0x03
#define BRXS_BROADCAST     0x04
#define BRXS_DISCONNECTING 0x05

/*---------------------------------------------------------------------------
 *
 *     Support for the different states necessary to perform radio
 *     initialization. The MeInitRadio function cycles through the states 
 *     incrementing the state each time the previous task completes and state
 *     order matters.
 */
typedef U8 MeInitRadioState;

#define MIRS_WRITE_PAGE_TIMEOUT  0x00
#define MIRS_READ_PAGE_TIMEOUT   0x01
#define MIRS_SET_EVENT_MASK      0x02
#define MIRS_READ_LOCAL_VERSION  0x03
#define MIRS_READ_BD_ADDR        0x04
#define MIRS_READ_INQ_MODE       0x05
#define MIRS_READ_ERR_DATA_RPT   0x06
#define MIRS_INQ_RESP_TX_PWR     0x07
#define MIRS_WRITE_SIMPLE_PAIR   0x08
#define MIRS_SECURE_CONN_SETUP   0x09
#define MIRS_READ_LOCAL_CMDS     0x0A
#define MIRS_MELE_INIT_RADIO     0x0B
#define MIRS_READ_SUP_CODECS     0x0C
#define MIRS_INITIALIZED         0x0D

/*---------------------------------------------------------------------------
 *
 * Values used for the Scan_Enable parameter of the Write_Scan_Enable command.
 */
#define BSE_INQUIRY    0x01
#define BSE_PAGE       0x02

/*---------------------------------------------------------------------------
 *
 * Values used in flags field of MeCommandToken.
 */
#define MCTF_NEED_CHECK  0x01
#define MCTF_ASYNC       0x02

/*---------------------------------------------------------------------------
 *
 * Values used to determine the handling of simple pairing
 */
#define IO_USE_NONE          0
#define IO_USE_PIN_CODE      1
#define IO_USE_PASS_KEY      2
#define IO_USE_CONFIRM       3
#define IO_USE_DISPLAY       4
#define IO_USE_CAP           5
#define IO_USE_RETRY         7

/*---------------------------------------------------------------------------
 * IO Capabilities State Flags in BtRemoteDevice.
 */
#define IO_CAP_RSP_RECVD  0x01
#define IO_CAP_REQ_RECVD  0x02
#define IO_CAP_REJECTED   0x04
#define IO_CAP_BOND_SENT  0x08


/*---------------------------------------------------------------------------
 *
 * Indicates that an HCI number of completed packets event has occurred.
 * "p.hciCompletePkts" is valid for this internal event. The typedef
 * associated with this event is in me.h (BtEventType).
 */
#define BTEVENT_HCI_NUM_COMPLETED_PKTS        199

/*---------------------------------------------------------------------------
 * Combined flags for testing for either dedicated or general bonding
 */
#define ANY_BONDING     (DEDICATED_BONDING | GENERAL_BONDING)

/*---------------------------------------------------------------------------
 *
 * MeTaskHandler is a pointer to a function to handle the HCI event for a
 * given task.
 */
typedef void (*MeTaskHandler)(U8 event, U8 len, U8* data);

/*---------------------------------------------------------------------------
 *
 * MeOpHandler is a pointer to a function to handle events for a
 * given operation.
 */
typedef void (*MeOpHandler)(BtOpEvent event);

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * BtInquiryOp is used to hold an inquiry operation on a queue.
 */
typedef struct {
    BtOperation op;
    BtIac       lap;
    U8          len; /* Duration of Inquiry */
    U8          max; /* Maximum number of responses */
} BtInquiryOp;

/*---------------------------------------------------------------------------
 *
 * BtScanOp is used to hold an inquiry or page scan operation on a queue.
 */
typedef struct {
    BtOperation        op;  /* This must be the first item */
    BOOL               nc;  /* TRUE means no connection. FALSE means connection */
    BtAccessibleMode   mode;
    BtAccessModeInfo    i;
} BtScanOp;

/*---------------------------------------------------------------------------
 * BtExtInqModeOp is used to hold Inquiry Mode and Extended Inquiry
 * operations on a queue.
 */
typedef struct {
    BtOperation        op;

    /* Informatino used for setting the inquiry mode */
    BtInquiryMode inqMode;

} BtInqModeOp;

/*---------------------------------------------------------------------------
 * BtExtInqModeOp is used to hold Inquiry Mode and Extended Inquiry
 * operations on a queue.
 */
typedef struct {
    BtOperation op;

    /* Determines if FEC will be used on the extende inquiry
     * response.  Use 0 means disabled, and 1 means enabled.
     */
    U8 fec;

    /* The extended inquiry response for the local device */
    U8 inqResp[240];

} BtExtInqRespOp;

/*---------------------------------------------------------------------------
 * BtErrDataRepOp is used to hold the operation and parameters for erroneous
 * data reporting.
 */
typedef struct {
    BtOperation      op;

    /* Defines the state of erroneous data reporting */
    BtErrDataReporting errDataRep;

} BtErrDataRepOp;

/*---------------------------------------------------------------------------
 * BtSmpDbgModeOp is used to hold the operation and parameters for Secure
 * Simple Pairing debug mode.
 */
typedef struct _BtSmpDbgModeOp {
    BtOperation op;

    /* Defines the simple pairing debug mode */
    BOOL mode;
} BtSmpDbgModeOp;

/*---------------------------------------------------------------------------
 * BtInqTxPwrOp is use to hold the operation and parameters for setting the
 * Inquiry Response TX Power Level.
 */
typedef struct {
    BtOperation      op;

    /* Defines the Inquiry Responst TX Power Level in dBm */
    BtInqTxPower  power;
} BtInqTxPwrOp;

/*---------------------------------------------------------------------------
 *
 * BtSecurityOp is used to deal with asynchronous security requests.
 */
typedef struct {
    BtOperation         op;
    BrEdrRemoteDevice  *remDev;
    BD_ADDR             bdAddr;
    U8                  data[16];
    U8                  ioType;
    BOOL                rejectKey;

    /* Used for writing/reading authentication payload timeout */
    U16                 authPayloadTimeout;
    BtRemoteDevice     *rd;

    /* Extra data */
    union {
        U8                  pLen;
        BtEncryptMode       eMode;
        BtSimplePairingMode pMode;
        BtSecurityMode      secMode;
    } x;

} BtSecurityOp;

#if NUM_BT_DEVICES == 1
/* Add one more BtSecurityOp if NUM_BT_DEVICES == 1.
 * Some radios require at least two.
 */
#define NUM_SEC_OPS     2
#else
#define NUM_SEC_OPS     NUM_BT_DEVICES
#endif

/*---------------------------------------------------------------------------
 *
 * BtConRejectOp is used to reject ACL and SCO connections.
 */
typedef struct {
    BtOperation    op;
    BD_ADDR        bdAddr;
    HciCommandType command;
    U8             rejectReason;
} BtConRejectOp;

/*---------------------------------------------------------------------------
 *
 * BtMeContext contains all the data used internally by the
 * Management Enity.
 */

typedef struct {
    ListEntry     regList;     /* List of registered global handlers */
    ListEntry     opList;      /* Operation queue */
    ListEntry     secList;     /* Security record list */
    ListEntry     cmdList;     /* List of pending commands */
    ListEntry     asyncList;   /* List of pending Async commands */
    ListEntry     pendList;    /* Pending operation list */
#if NUM_LE_DEVICES > 0
#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
    ListEntry     leSecList;   /* LE Security record list */
#endif
#endif /* NUM_LE_DEVICES > 0 */
    BtEvent       btEvent;     /* Bluetooth event */
    BtOpMask      opMask;      /* Indicates which operations are in progress */

    BtOperation*  curOp;       /* Current operation */

    HciCommand    hciGenCmd;   /* For sending general HCI commands */
    MeTaskHandler taskHandler; /* Handle HCI events */
    U8            taskEvent;   /* Event expected for taskHandler */
    MeOpHandler   opHandler;   /* Handle operation events */

    BtHandler    *authorizeHandler;
    BtHandler    *secModeHandler;

    /* Handler for the pairing operation */
    BtHandler    *pairingHandler;

    /* Incoming connection fields */
    BtHandler    *acceptHandler;
    BtConnectionRole connectionPolicy;

    U8                 stackState;
    U8                 opState;
    MeInitRadioState   taskState;
#if NUM_LE_DEVICES > 0
    MeLeInitRadioState leTaskState;
#endif /* NUM_LE_DEVICES > 0 */

#if NUM_SCO_CONNS > 0
    BtScoConnect            scoTable[NUM_SCO_CONNS];
    BtScoAudioSettings      vSettings;
#endif    /* NUM_SCO_CONNS > 0 */

    /* Device table */
    BrEdrRemoteDevice devTable[NUM_BT_DEVICES];
    BtSecurityOp      secOpTable[NUM_SEC_OPS];

#if NUM_KNOWN_DEVICES > 0
    /* Non-Connected Device Selection list */
    ListEntry       deviceList;
    ListEntry       btDeviceFreeList;
    BtDeviceContext btDevices[NUM_KNOWN_DEVICES];

    BtHandler*      dsMonitor;
#endif

    BtConRejectOp   rejectOp;

    U8              pendCons;   /* Number of outgoing and incoming connections in process */
    U8              activeCons; /* Number of active connections */
    U8              inHoldCons; /* Number of incoming holds */
    U8              holdCons;   /* Number of outgoing holds */

    /* TRUE indicates that accessibilty should be enabled if the pending connection fails because
     * an active connection was disconnected but did not enable accessibility because of the pending
     * connection
     */
    BOOL            enableAccForPend;

    /* Access Mode info */
    /* Current settings */
    BOOL               isLiac;   /* TRUE if LIAC or FALSE if GIAC */
    BtAccessibleMode   accModeCur;
    BtAccessModeInfo   accInfoCur;

    /* Non connected accessible info */
    BtAccessibleMode   accModeNC;
    BtAccessModeInfo   accInfoNC;

#if BT_ALLOW_SCAN_WHILE_CON == XA_ENABLED
    /* Connected accessible info */
    BtAccessibleMode    accModeC;
    BtAccessModeInfo    accInfoC;
#endif /* BT_ALLOW_SCAN_WHILE_CON */

    /* Operations */
    BtInquiryOp     inquiryOp;   /* Inquiry operation */
    BtScanOp        accOp;
    BtScanOp        accUpOp;
    BtScanOp        accDownOp;
    BtOperation     codOp;         /* Class of device operation */
    BtOperation     nameOp;        /* Write Local name operation */
    BtOperation     policyOp;      /* Write Default Link Policy operation */
    BtOperation     channelOp;     /* Write Channel Classification (AFH) operation */
    BtInqModeOp     inqModeOp;     /* Write Inquiry Mode operation */
    BtExtInqRespOp  extInqRespOp;  /* Write Extended Inquiry Response operation */
    BtErrDataRepOp  errDataRepOp;  /* Write Erroneous Data Reporting operation */
    BtInqTxPwrOp    inqTxPwrOp;    /* Write the Inquiry TX Power Level */
    BtSmpDbgModeOp  smpDbgModeOp;  /* Write the simple pairing debug mode */

    /* Security mode state and encryption setting */
    BtSecurityModeState         secModeState;        /* Security mode state */
    BtSimplePairingMode         secSimplePairMode;   /* Simple Pairing mode state */
    BtSecureConnectionMode      secConnectionMode;   /* Secure Connection mode state */
    BtIoCapabilities            secIoCap;            /* Default global IO capabilities */
    BtBondingMode               secBondingMode;      /* The current Bonding mode */
    BtAuthRequirements          secAuthRequirements; /* Default global Authentication reqs */
    BOOL                        secDebugKeyAccept;   /* TRUE if it is OK to accept debug keys */
    BtOobData                   secOobData;          /* Received remote OOB data  */
    BtOperation                 secOobOp;            /* Op to get the local OOB data */
    BtOobExtData                secOobExtData;       /* Received remote OOB extended data  */
    BtOperation                 secOobExtOp;         /* Op to get the local OOB extended data */
    BtAuthorizeRequirements     secAuthorizeRequirements; /* Default global authorization reqs */
    BtSecureConnRequirements    secSecureConnRequirement; /* Default global secure connections req */
    BOOL                        secGenJwEvent;       /* Generate Just Works events if TRUE */
    BOOL                        secMitmNoIo;         /* Allow pairing to continue with MITM set to a NO_IO device if TRUE */
    BtSecurityLevel             secDefaultLevel;     /* Default securitiy level for a service with no security record */

    BtClassOfDevice             classOfDevice;
    BtClassOfDevice             sdpServCoD;
    BtClassOfDevice             writeCoD;

    U16             curTask;

    BtLinkPolicy    inAclPolicy;
    BtLinkPolicy    outAclPolicy;

    /* Local radio settings */
    U16             radioPageTimeout;
    BtDevAddr       bdaddr;
    const U8*       localName;
    U8              localNameLen;
    BtVersionInfo   btVersionInfo;     /* Version Info from Read_Local_Version
                                        * HCI command  (HCC_READ_LOCAL_VERSION) 
                                        */
    U8              btFeatures[8];     /* BT features mask */
    U8              btExtFeatures[2][8];  /* BT extended features mask (pages 1-2) */
    U8              hciSupportedCommands[64]; /* HCI commands supported by the local device */
    U8              numLocalSupportedCodecs;
    U8              localSupportedCodecs[8];
    BtInquiryMode   inqMode;           /* Current inquiry mode */
    BtErrDataReporting errDataRep;     /* Current state of Erroneous Data Rep */
    BtInqRspTxPower inqRspTxPwr;       /* Current value for Inq Rsp TX Power */

    EvmTimer        timer;          /* Timer for ME_RadioShutdown */
    U16             pendingEvent;   /* Pending ME_RadioShutdown event */
    BtChannelClass  channelClass;   /* Channel Classification Map for AFH */
    U8              autoMode;       /* AFH auto mode enabled */
    BOOL            commandPending;

    BrPhyLink       brPhyLinks[NUM_BT_DEVICES];
#if NUM_LE_DEVICES > 0
    LePhyLink       lePhyLinks[NUM_LE_DEVICES];
#endif /* NUM_LE_DEVICES > 0 */

    /* Handler used for link events (physical and logical links)
     * intended to pass events from Physical and logical links
     * to L2CAP.
     */
    BtHandlerEx    *linkHandler;

    L2capAgent      agents[L2CAP_NUM_AGENTS];
    ListEntry       freeAgentList;            /* List of free agents */
    ListEntry       agentList;                /* List of active agents */

    /* MEAMP */
#if NUM_AMPS > 0
    BtHandlerEx    *l2aChanMgrHandler;        /* Channel manager handler */
    U8              numAmps;
    U8              iA2me;                    /* index of A2me Fixed Channel Client */
    ListEntry       controllers;              /* Active controllers */
    ListEntry       pendControllers;          /* Controllers not yet active */
    ListEntry       disabledControllers;      /* Disabled Controllers */
    ListEntry       pendPhyLinks;
    ListEntry       ampMgrClients;
    BtHandlerEx     crlHandler;
    BtHandlerEx     a2meHandler;
    BtLogicalLink   ampLogLinks[NUM_AMP_LOGICAL_LINKS];
#endif /* NUM_AMPS > 0 */

    /* Used to indicate that the EIR needs to be updated. */
    EvmTask         eirUpdateTask;

    /* **************** LE **************************/
#if NUM_LE_DEVICES > 0
    /* Used to indicate that the Advertising data needs to be updated. */
    EvmTask                     advertDataUpdateTask;
    EvmTask                     gapServiceUpdateTask;

    MeLeRemoteDevice            leDevices[NUM_LE_DEVICES];
    ListEntry                   leHandlers;     /* List of registered LE handlers */

    /* RemoteDevice structure used to disconnect link when out of resources */
    BtRemoteDevice              leDiscRemDev;

    /* Current random address programmed in the controller */
    BtDevAddr                   leRandomAddr;

    /* Handler to receive link update requests from remote slaves */
    BtHandlerEx                *updateLinkHandler;

    /* Handler to receive BR/EDR fixed security channel events */
    BtHandlerEx                 brSecHandler;

    MeLeAdvertParmsOp           leAdvertParmsOp;
    MeLeAdvertDataOp            leAdvertDataOp;
    MeLeScanParmsOp             leScanParmsOp;
    MeLeScanDataOp              leScanDataOp;
    MeLeAdvertEnableOp          leAdvertEnableOp;
    MeLeScanEnableOp            leScanEnableOp;
    MeLeWhiteListOp             leWhiteListOp;
    MeLeSetChanClassOp          leSetChanClassOp;
    MeLeSetRandAddrOp           leSetRandAddrOp;
    MeLeOpHandler               leSetDefaultDataLength;
    MeLeOpHandler              *leP256PublicKeyOp;
    MeLeOpHandler              *leDHKeyOp;
#if LL_PRIVACY == XA_ENABLED
    MeLeResolvingListOp         leResolvingListOp;      /* Add/Remove operations */
    MeLeClearResolvingListOp    leClearResolvingListOp;
    MeLeAddrResolveEnableOp     leAddrResolveEnableOp;
    MeLeSetRpaTimeoutOp         leSetRpaTimeoutOp;
#endif /* LL_PRIVACY == XA_ENABLED */

    MeLeEvent               leEvent;

    /* Controller supported max values for transmit and receive octects and time */
    U16                     leSupportedMaxTxOctets;
    U16                     leSupportedMaxTxTime;        /* Time in milliseconds */
    U16                     leSupportedMaxRxOctets;    
    U16                     leSupportedMaxRxTime;        /* Time in milliseconds */

    /* Default values in the controller for transmit octets and time used for
     * all new connections. These are stored in the controller 
     */
    U16                     leDefaultMaxTxOctets;
    U16                     leDefaultMaxTxTime;          /* Time in milliseconds */
                                                         
    BOOL                    leP256PublicKeyExists;
    U8                      leP256PublicKey[64];
    U8                      leDHKey[32];
    U8                      leFeatures[8];
    U8                      leStates[8];            /* States supported by LE Controller */
    U8                      leAdvertChanTxPower;
    U8                      leWhiteListSize;
#if LL_PRIVACY == XA_ENABLED
    U8                      leResolvingListSize;    /* Size of the resolving list in the LE Controller */
    BOOL                    llPrivacyEnabled;       /* Link Layer Privacy is enabled in the LE Controller */
#endif
#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
    U8                      leSecDefaultSecLevelIn;  /* Type is LeSecSecurityLevel */
    U8                      leSecDefaultSecLevelOut; /* Type is LeSecSecurityLevel */
#endif

    /* Last advertising own address type programmed into the controller */
    MeLeAddressType         leAdvertOwnAddrType;

    /* Default Encryption key length requirement */
    U8                      leEncryptKeyLength;
#endif /* NUM_LE_DEVICES > 0 */
} BtMeContext;


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Prototype:     void ME_AddOperation(BtOperation* op)
 *
 * Description:   Add an operation to the operation queue.
 *
 * Parameters:    op - pointer to operation to add
 *
 * Returns:       void
 */
#define MeAddOperation(o) (InsertTailList(&MEC(opList), &((o)->op.node)))
#define MeAddOperationFront(o) (InsertHeadList(&MEC(opList), &((o)->op.node)))
#define MeAddOperationPend(o) (InsertTailList(&MEC(pendList), &((o)->op.node)))

/*---------------------------------------------------------------------------
 *
 * Prototype:     void MeStartOperation(void)
 *
 * Description:   Add an operation to the operation queue.
 *
 * Parameters:    none
 *
 * Returns:       void
 */
void MeStartOperation(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     BOOL  MeIsLeSecureConSupported(void)
 *
 * Description:   Return TRUE if LE Secure Connections is supported by the controller
 *                otherwise return FALSE.
 *
 * Parameters:    none
 *
 * Returns:       BOOL
 */
#define MeIsLeSecureConSupported() ((MEC(hciSupportedCommands)[34] & 0x06) == 0x06)

void MeOperationEnd(void);
void MeStartInquiry(BtInquiryOp* op);
void MeReportResults(BtEventMask mask, BtEventMask2 mask2);
void MeHandleCompleteTask(U8 event, U8  len, U8* parm);
void MeHandleAccessModeOp(BtOpEvent event);
BOOL MeIsScanValsLegal(U16 interval, U16 window);
void MeChangeAccessibleMode(BOOL firstCon);
BOOL MeIsAccessModeInfoEqual(BtAccessModeInfo* info1, BtAccessModeInfo* info2);
BrEdrRemoteDevice* MeFindBrEdrRemoteDevice(U8* bdAddr);
BrEdrRemoteDevice* MeGetFreeBrEdrRemoteDevice(void);
void MeHandleLinkConnectOp(BtOpEvent event);
void MeHandleConnectionStatusOnly(U8 event, U8  len, U8* parm);
void MeCallLinkHandlers(BtRemoteDevice* remDev);
BOOL MeHandleEvents(U8 event, U8  len, U8* parm);
void MeHandleLinkConnectReq(U8 len, U8* parm);
void MeHandleLinkAcceptOp(BtOpEvent event);
void MeHandleConnectComplete(U8 len, U8* parm);
void MeReportLinkUp(BrEdrRemoteDevice* remDev, BtEventType eType);
BrEdrRemoteDevice* MeMapHciToRemoteDevice(HciHandle hciHandle);
BtPhysicalLink* MeMapHciToPhysicalLink(HciHandle hciHandle);
void MeDisconnectLink(BtRemoteDevice* remDev);
void MeHandleDisconnectComplete(U8 len, U8* parm);
void MeHandleLinkDisconnectOp(BtOpEvent event);
void MePendingCommandHandler(EvmTimer *timer);
void ME_InitTypes(void);

BtSecurityOp* SecBtGetFreeSecurityOp(void);
void SecHandleLinkKeyReq(U8 len, U8* parm);
void SecHandleUserIoReq(U8 len, U8* parm, U8 type);
void SecHandleAuthenticateOp(BtOpEvent event);
void SecHandleLinkKeyRspOp(BtOpEvent event);
void SecHandleUserIoRspOp(BtOpEvent event);
void SecHandleSimplePairingComplete(U8 len, U8 *parm);
void SecHandleAuthenticateComplete(U8  len, U8* parm);
void SecHandleLinkKeyNotify(U8 len, U8* parm);
BtStatus SecAddAuthenticateOpToQueue(BrEdrRemoteDevice* remDev, BtSecurityOp **returnOp);
BtStatus SecProcessSecurityToken(BtSecurityToken* token);
BOOL SecIsCurrentAuthenticationGoodEnough(BtSecurityToken* token);
#if BT_SECURITY_TIMEOUT != 0
void SecAccessTimeoutHandler(EvmTimer *timer);
#endif /* BT_SECURITY_TIMEOUT */
void SecHandleAuthorizeOp(BtOpEvent event);
void SecHandleAuthorizeRequest(BrEdrRemoteDevice *remDev);
BtStatus SecAuthenticateLink(BtHandler *handler, BtRemoteDevice *remDev, BOOL genLEKey);
void SecHandleEncryptOp(BtOpEvent event);
void SecProcessAllSecurityTokens(BrEdrRemoteDevice* remDev);
void SecHandleEncryptChange(U8 len, U8* parm, BOOL refresh);
BtStatus SecSetSecurityMode(BtHandler* handler, BtSecurityMode mode);
void SecHandleSecurityModeOp(BtOpEvent event);
void SecReportAuthenticateResult(BrEdrRemoteDevice* remDev);
void SecReportEncryptionResult(BrEdrRemoteDevice* remDev, BtEncryptMode mode);
void SecReportSecurityModeResult(void);
void SecHandleReturnLinkKeys(U8 len, U8* parm);
void SecHandleIoCapReq(U8 len, U8* parm);
void SecHandleIoCapRspOp(BtOpEvent event);
void SecHandleIoCapRejectOp(BtOpEvent event);
void SecHandleGetOobDataOp(BtOpEvent event);
void SecHandleGetOobExtDataOp(BtOpEvent event);
void SecHandleIoCapRsp(U8 len, U8* parm);
void SecHandlePassKeyReq(U8 len, U8* parm);
void SecHandleKeypress(U8 len, U8* parm);
void SecHandleUserPassKeyNotify(U8 len,U8* parm);
void SecHandleConfirmReq(U8 len, U8* parm);
void SecHandleRemOobDataReqRspOp(BtOpEvent event);
void SecHandleRemOobExtDataReqRspOp(BtOpEvent event);
void SecHandleRemOobDataReq(U8 len, U8* parm);
void SecHandleDenyOobDataReqRspOp(BtOpEvent event);
void SecHandleSetSmpDbgModeOp(BtOpEvent event);
U8 SecGetPinLen(BrEdrRemoteDevice* remDev);
BOOL SecIsLinkKeyGoodEnough(BrEdrRemoteDevice* remDev, BtDeviceRecord *record);
BtStatus SecFindDeviceRecordEx(const BtDevAddr *devAddr, BtDeviceRecord *record);
BtStatus SecFindDualDeviceRecord(const BtDevAddr *devAddr, BtDeviceRecord *record);
BOOL SecFindIdentityAddress(const BtDevAddr *devAddr, BtDevAddr *out);

BrEdrRemoteDevice* MeEnumerateRemoteDevices(I8 i);
void MeReportInqCancelComplete(void);
void MeHandleRemNameReqOp(BtOpEvent event);
MeCommandToken* MeFindMatchingToken(MeCommandToken* token, ListEntry* list);
void MeReportMeCommandComplete(MeCommandToken* token);
void MeHandleRemNameReqComplete(U8 len, U8* parm);
void MeProcessToken(MeCommandToken *token);
void MeHandleLinkRejectOp(BtOpEvent event);
void MeSuspendTransmitters(void);
void MeRestartTransmitters(void);
void MeHandleHoldModeOp(BtOpEvent event);
void MeHandleModeChange(U8 len, U8* parm);
void MeHandleSniffModeOp(BtOpEvent event);
void MeHandleParkModeOp(BtOpEvent event);
void MeHandleExitParkModeOp(BtOpEvent event);
void MeHandleExitSniffModeOp(BtOpEvent event);
void MeHandleSetSniffSubratingParmsOp(BtOpEvent event);
void MeHandleSwitchRoleOp(BtOpEvent event);
void MeHandleRoleChange(U8 len, U8* parm);
void MeHandleConnPacketTypeChange(U8 len, U8* parm);
void MeHandleQosSetupComplete(U8 event, U8 len, U8* parm);
void MeHandleDiscoverRole(BtOpEvent event);
void MeHandleReadRemFeatures(U8 len, U8* parm);
void MeHandleReadRemExtFeatures(U8 len, U8* parm);
void MeHandleRemHostSuppFeatures(U8 len, U8* parm);
void MeHandleLinkSupervTimeout(U8 len, U8* parm);
void MeHandleSniffSubrating(U8 len, U8* parm);
void MeHandlePsRepModeChange(U8 len, U8* parm);
void MeHandleAuthPayloadTimeout(U8 len, U8* parm);
void MeWriteClassOfDevice(void);
void MeWriteDefaultLinkPolicy(void);
void MeWriteLocalName(void);
void MeChangeConnPacketTypeOp(BtOpEvent event);
void MeHandleReadRemFeaturesOp(BtOpEvent event);
void MeHandleReadRemExtFeaturesOp(BtOpEvent event);
void SecMeHandleRefreshEncryptKeyOp(BtOpEvent event);
void SecHandleWriteAuthTimeoutOp(BtOpEvent event);
void SecHandleReadAuthTimeoutOp(BtOpEvent event);
BOOL MeIsValidGeneralToken(MeCommandToken* token);
void MeCheckRemDevToken(MeCommandToken *token);
BtStatus ME_SendHciCommand(MeCommandToken *token);
BOOL MeMatchVendorSpecificPattern(MeGeneralHciCommand *cmd, U8 len, U8 *parms);
MeCommandToken* MeFindGeneralEvent(BtOpEvent event, U8 len, U8 *parm);
void MeProcessGenToken(MeCommandToken *token);
void MeReportNMResults(void);
BtStatus MeSendHciCommand(HciCommandType opCode, U8 parmLen);
void MeHandleWriteLinkPolicyOp(BtOpEvent event);
void MeHandleCancelConnectOp(BtOpEvent event);
void MeHandleCancelRemNameOp(BtOpEvent event);
BtStatus MeCheckModeChange(BtRemoteDevice *remDev, BtOpType bop);
void MeHandleDataReqWhileNotActive(BrPhyLink *Plink);
U8 MeSetBits(U8 byte, U8 bit, U8 n, BOOL state);
#define MeSetBits(a, b, n, u) ((a & ((~0 << (b)) | (~(~0 << (b - n))))) |  \
                                   (((u ? 0xff : 0x00) & ~(~0 << n)) << (b - n)));

#if NUM_KNOWN_DEVICES > 0
void MeDevEventHandler(void);
void MeDevInquiryStart(void);
#if SDP_CLIENT_SUPPORT == XA_ENABLED
void MeDevSdpServiceFound(const SdpQueryToken *SdpToken);
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */
#else
#define MeDevEventHandler()         while (0)
#define MeDevInquiryStart()         while (0)
#define MeDevSdpServiceFound(_SDP)  while (0)
#endif /* NUM_KNOWN_DEVICES > 0 */

#if NUM_SCO_CONNS > 0
void ScoHandleLinkAcceptOp(BtOpEvent event);
BtScoConnect *ScoMapHciToConnect(HciHandle hcihandle);
void MeScoDisconnectLink(BtScoConnect *scoConnect);
void ScoHandleLinkRejectOp(BtOpEvent event);
void ScoHandleVoiceSettingsOp(BtOpEvent event);
void ScoChangeConnPacketTypeOp(BtOpEvent event);
void ScoHandleConnectComplete(U8 len, U8* parm);
void ScoHandleConnChanged(U8 len, U8* parm);
void ScoHandleLinkConnectOp(BtOpEvent event);
void ScoHandleLinkSetupOp(BtOpEvent event);
void ScoHandleLinkDisconnectOp(BtOpEvent event);
BOOL ScoHandleLinkConnectReq(U8 len, U8* parm);
BOOL ScoHandleDisconnectComplete(U8 len, U8* parm);
void ScoHandleDisconnectAcl(BrEdrRemoteDevice *remDev);
void ScoReportScoLinkUp(BtScoConnect *scocon, BtEventType evnt);
BtScoConnect *ScoGetFreeConnect(void);
void ScoHandleEnhancedSetupOp(BtOpEvent Event);
void ScoHandleEnhancedAcceptOp(BtOpEvent Event);
void ScoHandleReadSupportedCodecsOp(BtOpEvent Event);

#if BT_SCO_HCI_DATA == XA_ENABLED
void SCO_Init(void);
BtStatus Sco_Send(BtScoConnect *scocon, BtPacket *Packet);
void ScoDataCallback(U8 event, HciCallbackParms *parms);
#endif /* BT_SCO_HCI_DATA == XA_ENABLED */

#endif /* NUM_SCO_CONNS */

#if NUM_AMPS > 0
void MeAmpHandleBtEvent(const BtEvent* e);
#endif /* NUM_AMPS > 0 */

#if NUM_LE_DEVICES > 0
void MeSecHandlerBrSecEvents(void *ctx, BtEventEx *e);
#if LL_PRIVACY == XA_ENABLED
BtStatus SecFindDeviceRecordExx(const BtDevAddr *devAddr, MeLeAddressType meAddrType, BtDeviceRecord *record);
#endif
#endif

/* -- From ME tester --------------------------------------------------- */
#if JETTEST == XA_ENABLED
extern void (*Tester_MeReceive)(U8 event, HciCallbackParms *parms);
#endif /* JETTEST == XA_ENABLED */

#endif /* __MEI_H */
