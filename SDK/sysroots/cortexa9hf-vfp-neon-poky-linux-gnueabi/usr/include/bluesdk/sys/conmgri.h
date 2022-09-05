/****************************************************************************
 *
 * File:
 *     conmgri.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Private definitions for the Connection Manager.
 *
 * Created:
 *     August 5, 2005
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

#ifndef __CONMGRI_H_
#define __CONMGRI_H_

#include "conmgr.h"
#if NUM_LE_DEVICES > 0
#include "conmgrled.h"
#include "conmgrlea.h"
#include "conmgrlep.h"
#include "gattclient.h"
#include "gattsecurity.h"
#endif /* NUM_LE_DEVICES > 0 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

#if NUM_LE_DEVICES > 0
/*---------------------------------------------------------------------------
 * CmgrLeState type
 *
 * State used to setup LE controller. These are ORed together.
 */
typedef U32 CmgrLeState;

/* LE controller is idle */
#define CMGR_LEST_IDLE                      0x00000000

/* LE Controller is waiting for Set Scan Data Confirm for connection */
#define CMGR_LEST_CON_SCAN_PARMS_CNF        0x00000001

/* LE Controller is waiting for Scan Enable Confirm for connection*/
#define CMGR_LEST_CON_SCAN_ENBL_CNF         0x00000002

/* LE Controller is Scanning for a connection procedure */
#define CMGR_LEST_CON_SCANNING              0x00000004

/* LE Controller is canceling scanning for timeouts */
#define CMGR_LEST_CANCEL_SCAN_TIMEOUT       0x00000008

/* LE Controller is waiting for Scan to be disabled */
#define CMGR_LEST_CON_SCAN_DIS_CNF          0x00000010

/* LE Controller is waiting for Connection Confirm */
#define CMGR_LEST_WAIT_CON_CNF              0x00000020

/* Scan parameters need to be changed */
#define CMGR_LEST_SCAN_PARMS_CHANGE         0x00000040

/* LE Controller is waiting for Connection Cancel confirm */
#define CMGR_LEST_CAN_CON_NO_ADDR           0x00000080

/* LE Controller is stopping scanning so it can start a connection */
#define CMGR_LEST_SCAN_DISC_FOR_CON         0x00000100

/* CMGR is to use the second interval and window for connecting */
#define CMGR_LEST_SECOND_INT_FOR_CON        0x00000200

/* Scan Parameters have been sent to the LE controller */
#define CMGR_LEST_SCAN_PARMS_SENT           0x00000400

/* LE Controller is canceling a connection because of a timeout */
#define CMGR_LEST_CANCEL_CON_TIMEOUT        0x00000800

/* CMGR is to use the second interval and window for scanning
 * while creating a connection
 */
#define CMGR_LEST_SECOND_INT_CON_SCAN       0x00001000

/* New request occurred while MELE_CancelCreateLink is pending
 * due to a timer expiration or while "discovery" scanning
 */
#define CMGR_LEST_CANCEL_CON_NEW            0x00002000

/* LE Controller is stopping scanning so it can start a connection */
#define CMGR_LEST_SCAN_DISC_FOR_WT          0x00004000

/* LE controller is canceling a connection so an item can be added to the white list */
#define CMGR_LEST_CANCEL_CON_FOR_WT         0x00008000

/* LE controller is adding an item to the white list */
#define CMGR_LEST_ADD_WHITE_LIST            0x00010000

/* LE controller is clearing the white list */
#define CMGR_LEST_CLEAR_WHITE_LIST          0x00020000

/* Removed an item so clear the white list */
#define CMGR_LEST_REMOVE_CLEAR_WL           0x00040000

/* CMGR is calling handlers callback to report results */
#define CMGR_LEST_CALLING_HANDLERS          0x00080000

/* LE Controller is waiting for Set Scan Data Confirm for
 * "discovery" scanning
 */
#define CMGR_LEST_WAIT_SCAN_PARMS_CNF       0x00100000

/* LE Controller is waiting for Scan Enable Confirm for "discovery"
 * scanning
 */
#define CMGR_LEST_WAIT_SCAN_ENBL_CNF        0x00200000

/* LE Controller is performing "Discovery" Scanning */
#define CMGR_LEST_SCANNING                  0x00400000

/* LE Controller is waiting for "discovery" scanning to be disabled */
#define CMGR_LEST_WAIT_SCAN_DIS_CNF         0x00800000

/* CMGR is to use the second interval and window for "discovery"
 * scanning
 */
#define CMGR_LEST_SECOND_INT_SCAN           0x01000000

/* CMGR is calling scan handlers callback to report results */
#define CMGR_LEST_CALL_SCAN_HANDLERS        0x02000000

/* While performing a discovery scanning operation a connection request has
 * this flag tells CMGR to stop discovery scanning and switch over to connection
 * request when the current operation is complete.
 */
#define CMGR_LEST_CANCEL_SCAN_CON           0x04000000

/* Start discovery scan is waiting for the white list to complete. */
#define  CMGR_LEST_SCAN_WAIT_START_WL       0x08000000 

/* Cancel Link has been called */
#define CMGR_LEST_CANCEL_LINK               0x10000000

/* While performing an advertising operation a connection request has
 * this flag tells CMGR to stop advertising and switch over to connection
 * request when the current operation is complete.
 */
#define CMGR_LEST_CANCEL_ADVERT_CON         0x20000000

/* LE Controller is performing a set random address command */
#define CMGR_LEST_WAIT_RANDOM_ADDR_CON      0x40000000

/* LE Controller is performing a set random address command for discover */
#define CMGR_LEST_WAIT_RANDOM_ADDR          0x80000000

/* LE Controller is not in a state to take commands used for connection
 * request
 */
#define CMGR_LEST_NO_CON_CMDS  (CMGR_LEST_SCAN_DISC_FOR_WT |\
    CMGR_LEST_CANCEL_CON_FOR_WT | CMGR_LEST_ADD_WHITE_LIST | \
    CMGR_LEST_CLEAR_WHITE_LIST | CMGR_LEST_CALLING_HANDLERS)

#define CMGR_LEST_NO_CON_CMDS_WL (CMGR_LEST_SCAN_DISC_FOR_WT | \
    CMGR_LEST_CANCEL_CON_FOR_WT | CMGR_LEST_CLEAR_WHITE_LIST |\
    CMGR_LEST_CALLING_HANDLERS)

#define CMGR_LEST_CON_ACTIVE (CMGR_LEST_WAIT_CON_CNF | \
    CMGR_LEST_CAN_CON_NO_ADDR | CMGR_LEST_CANCEL_CON_TIMEOUT |\
    CMGR_LEST_CANCEL_CON_FOR_WT)

#define CMGR_LEST_SCAN_ENABLE_BUSY (CMGR_LEST_CON_SCAN_ENBL_CNF |\
     CMGR_LEST_CON_SCAN_DIS_CNF | CMGR_LEST_WAIT_SCAN_ENBL_CNF | \
     CMGR_LEST_WAIT_SCAN_DIS_CNF)

#define CMGR_LEST_ACTIVE (CMGR_LEST_CON_SCAN_PARMS_CNF | \
    CMGR_LEST_CON_SCAN_ENBL_CNF | CMGR_LEST_CON_SCANNING | \
    CMGR_LEST_CON_SCAN_DIS_CNF |\
    CMGR_LEST_WAIT_CON_CNF | CMGR_LEST_CAN_CON_NO_ADDR | \
    CMGR_LEST_SCAN_DISC_FOR_CON | CMGR_LEST_CANCEL_CON_TIMEOUT |\
    CMGR_LEST_SCAN_DISC_FOR_WT | CMGR_LEST_CANCEL_CON_FOR_WT |\
    CMGR_LEST_ADD_WHITE_LIST | CMGR_LEST_CLEAR_WHITE_LIST |\
    CMGR_LEST_CALLING_HANDLERS | CMGR_LEST_WAIT_RANDOM_ADDR_CON)

#define CMGR_LEST_SCAN_BUSY (CMGR_LEST_WAIT_SCAN_PARMS_CNF |\
    CMGR_LEST_WAIT_SCAN_ENBL_CNF | CMGR_LEST_SCANNING |\
    CMGR_LEST_WAIT_SCAN_DIS_CNF | CMGR_LEST_WAIT_RANDOM_ADDR |\
    CMGR_LEST_SCAN_WAIT_START_WL)

#define CMGR_LEST_SCAN_BUSY_CONFIG (CMGR_LEST_WAIT_SCAN_PARMS_CNF |\
    CMGR_LEST_WAIT_SCAN_ENBL_CNF |\
    CMGR_LEST_WAIT_SCAN_DIS_CNF | CMGR_LEST_WAIT_RANDOM_ADDR)

/* End of CmgrLeState */

/*---------------------------------------------------------------------------
 * CmgrLePrivState type
 *
 * State used for LE Privacy. These are ORed together.
 */
typedef U32 CmgrLePrivState;

/* LE Controller is performing a set random address command
 * for connection scanning
 */
#define CMGR_LEST_WAIT_RAND_ADDR_CON_SCAN    0x00000001

/* LE Controller is performing a set random address command
 * and new white list items can be added
 */
#define CMGR_LEST_WAIT_RANDOM_ADDR_ADDWL     0x00000002

/* LE Controller is performing a set random address command
 * for discovery scanning
 */
#define CMGR_LEST_WAIT_RAND_ADDR_DSCV_SCAN   0x00000004

/* Changing the privacy setting */
#define CMGR_LEST_CHANGE_PRIVACY             0x00000008

/* Scan response data needs to be sent */
#define CMGR_LEST_SEND_SCAN_RSP_DATA         0x00000010
/* End of CmgrLePrivState */

#if LL_PRIVACY == XA_ENABLED
/*---------------------------------------------------------------------------
 * CmgrLeLLPrivState type
 *
 * State used for LE Link Layer Privacy. These are ORed together.
 */
typedef U32 CmgrLeLLPrivState;

/* LE controller is adding an item to the resolving list */
#define CMGR_LEST_ADD_RESOLVING_LIST         0x00000001

/* LE controller is clearing the resolving list */
#define CMGR_LEST_CLEAR_RESOLVING_LIST       0x00000002

/* LE controller is removing an item from the resolving list */
#define CMGR_LEST_REMOVE_RESOLVING_LIST      0x00000004

/* LE controller is performing a read local RPA command */
#define CMGR_LEST_READ_LOCAL_RPA             0x00000008

/* LE controller is performing a read peer RPA command */
#define CMGR_LEST_READ_PEER_RPA              0x00000010

/* LE controller is performing a read new local RPA command */
#define CMGR_LEST_READ_NEW_LOCAL_RPA         0x00000020

/* LE controller is performing a read new peer RPA command */
#define CMGR_LEST_READ_NEW_PEER_RPA          0x00000040

 /* LE Controller needs to perform a set advertising enable command 
 * after the resolving list operations are complete.
 */
#define CMGR_LEST_WAIT_ADVERT_ENABLE         0x00000080

/* LE Controller needs to perform a set scanning enable command 
 * after the resolving list operations are complete.
 */
#define CMGR_LEST_WAIT_SCAN_ENABLE           0x00000100

/* LE controller has added the local IRK into the resolving list */
#define CMGR_LEST_ADDED_LOCAL_IRK            0x00000200

/* An Add operation for a device already in the resolving list triggered the 
 * LE controller to first remove the item from the resolving list
 */
#define CMGR_LEST_ADD_TRIGGERED_REMOVE       0x00000400

/* End of CmgrLeLLPrivState */
#endif /* LL_PRIVACY == XA_ENABLED */

/*---------------------------------------------------------------------------
 * CmgrLeAdvertState type
 *
 * State used to setup LE controller. These are ORed together.
 */
typedef U16 CmgrLeAdvertState;

/* LE controller is idle */
#define CMGR_LEST_ADVERT_IDLE                   0x0000

/* LE Controller is waiting for Set Advertising Data confirm */
#define CMGR_LEST_ADVERT_WAIT_DATA_CNF          0x0001

/* LE Controller is waiting for Advertising Enable Confirm */
#define CMGR_LEST_ADVERT_WAIT_ENBL_CNF          0x0002

/* LE Controller is waiting for Set Advertising Parameters confirm */
#define CMGR_LEST_ADVERT_WAIT_PARMS_CNF         0x0004

/* LE Controller is waiting for Set Random Address confirm */
#define CMGR_LEST_ADVERT_WAIT_RANDOM_CNF        0x0008

/* LE Controller is Advertising  */
#define CMGR_LEST_ADVERTISING                   0x0010

/* LE Controller is waiting for Advertising cancel confirm */
#define CMGR_LEST_ADVERT_CANCEL_CNF             0x0020

/* CMGR is adjusting its active Advertising interval */
#define CMGR_LEST_ADVERT_ADJUST_INTERVAL        0x0040

/* CMGR is overriding an active Advertising handler */
#define CMGR_LEST_ADVERT_OVERRIDE               0x0080

/* CMGR is disabling Advertising for all handlers */
#define CMGR_LEST_ADVERT_DISABLING              0x0100

/* CMGR is stopping Advertising for a specific handler */
#define CMGR_LEST_ADVERT_STOPPING               0x0200

/* CMGR is stopping Advertising due to timeout */
#define CMGR_LEST_ADVERT_TIMEOUT                0x0400

/* A fatal error has occured. */
#define CMGR_LEST_ADVERT_FATAL_ERROR            0x0800

/* CMGR is setting the Scan Response data, outside
 * of an enable Advertising operation
 */
#define CMGR_LEST_SET_SCAN_RESP_CNF             0x1000

/* CMGR is updating the Advertising data, outside
 * of an enable Advertising operation
 */
#define CMGR_LEST_ADVERT_UPDATE_CNF             0x2000

/* CMGR is modifying the white list and advertising is waiting
 * to start when the white list operations are complete
 */
#define CMGR_LEST_ADVERT_WAIT_START_WL          0x4000

/* CMGR is waiting for a clear white list to complete after
 * canceling advertising in order to start a connection
 */
#define CMGR_LEST_ADVERT_WAIT_CLEAR_WL_FOR_CON  0x8000
/* End of CmgrLeAdvertState */

/*---------------------------------------------------------------------------
 * CmgrLeAdvertDataFlags type
 *
 * LE Advertising Data flag values. These are ORed together.
 */
typedef U16 CmgrLeAdvertDataFlags;

/* Advertising data with limited discovery mode */
#define CMGR_LE_AD_FLAG_LIMITED_DISC            0x01

/* Advertising data with general discovery mode */
#define CMGR_LE_AD_FLAG_GENERAL_DISC            0x02

/* Advertising data with BR/EDR not supported */
#define CMGR_LE_AD_FLAG_NO_BREDR                0x04

/* Advertising data with LE and BR/EDR supported controller */
#define CMGR_LE_AD_FLAG_LE_BREDR_CTRL           0x08

/* Advertising data with LE and BR/EDR supported host */
#define CMGR_LE_AD_FLAG_LE_BREDR_HOST           0x10

/* End of CmgrLeAdvertDataFlags */

#if LL_PRIVACY == XA_ENABLED
/*---------------------------------------------------------------------------
 * CmgrLePrivResolveOpType type
 *
 * LE Advertising Data flag values. These are ORed together.
 */
typedef U8 CmgrLePrivResolveOpType;

#define CMGR_LE_PRIV_RESOLVE_NONE               0x00
#define CMGR_LE_PRIV_RESOLVE_ADD_LIST           0x01
#define CMGR_LE_PRIV_RESOLVE_REMOVE_LIST        0x02

/* End of CmgrLePrivResolveOpType */

/*---------------------------------------------------------------------------
 * CmgrLePrivResolveOpState definitions
 *
 *     Support for the different states necessary to perform LE radio
 *     initialization. The init function cycles through the states by
 *     incrementing the state after the previous command is completed and
 *     state order matters.
 */
typedef U8 CmgrLePrivResolveOpState;

#define CLPROS_INITIAL_STATE            0x00
#define CLPROS_DISABLE_ADVERTISING      0x01
#define CLPROS_DISABLE_SCANNING         0x02
#define CLPROS_DISABLE_RESOLUTION       0x03
#define CLPROS_PERFORM_RESOLVE_OPER     0x04
#define CLPROS_ENABLE_RESOLUTION        0x05
#define CLPROS_ENABLE_ADVERTISING       0x06
#define CLPROS_ENABLE_SCANNING          0x07

/* End of CmgrLePrivResolveOpState */

#endif /* LL_PRIVACY == XA_ENABLED */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 * CmgrLeConParmsInteral structure
 *
 *     Used to hold LE connection parameters internally.
 */
typedef struct _CmgrLeConParmsInternal {

    /* === Internal use only === */

    CmgrLeConType  type;

    CmgrLeWinInt interval1;
    CmgrLeWinInt interval12;
    CmgrLeWinInt window1;
    TimeT        time1;

    CmgrLeWinInt interval2;
    CmgrLeWinInt window2;
    TimeT        time2;

    BtDevAddr *devAddrList;
    U16        devAddrListLen;

} CmgrLeConParmsInternal;

/*---------------------------------------------------------------------------
 * CmgrLeAdvertParmsInternal structure
 *
 *     Used to hold LE advertising parameters internally.
 */
typedef struct _CmgrLeAdvertParmsInternal {

    /* === Internal use only === */
    MeLeAdvertParms         parms;                /* Advertising parameters */

    /* Advertising data fields */
    CmgrLeAdvertVendorData  vendor;               /* Vendor specific data */
    CmgrLeAdvertServiceData service;              /* Service specific data */
    ListEntry               services;             /* Services used in advertising data */
    ListEntry               otherAdTypes;         /* List of other AD types to include */
} CmgrLeAdvertParmsInternal;

#endif /* NUM_LE_DEVICES > 0 */

/* The BtCmgrContext structure contains the global context of the
 * connection manager.
 */
typedef struct _BtCmgrContext {
    BtHandler                   globHandler; /* This must be the first item in the structure */
    ListEntry                   handlerList;
    U8                          linkCount;

#if NUM_LE_DEVICES > 0
    ListEntry                   leConRequests;
    ListEntry                   leAddWhiteList; /* List of items being added to whitelist */
    ListEntry                   leScanList;
    ListEntry                   leWhiteList;    /* List of addrs for white list */
    MeLeHandler                 leGlobHandler;
    MeLeLinkHandler             meleConHandler;
    BtDevAddr                   leConAddr;
    BtDevAddr                   leConIdAddr;
    CmgrLeGlobalConParms        leConParmsTmp;
    CmgrLeConParmsInternal      leConParms;
    CmgrLeScanParms             leScanParmsTmp;
    MeLeCreateLinkParms         leCreateConParms;
    MeLeScanParms               leScanParms;
    U8                          leScanRspData[31];
    U8                          leScanRspDataLen;
    EvmTimer                    leTimer;
    CmgrHandler                *leSetConParmHandler;
    ListEntry                  *leNext;
    ListEntry                   leScanHead;
    BtHandlerEx                *leScanNext;
    BtRemoteDevice             *leCancelRemDev;
    CmgrLeState                 leState;
    BOOL                        enableScan;
    U8                          leLinkCount;
    U8                          leNumInWhiteList;
    MeLeInitFilterPolicy        leConScanFilterPolicy;
    CmgrLeAdvertState           leAdvertState;
    CmgrLeAdvertParms           leAdvertParmsTmp;
    CmgrLeAdvertParmsInternal   leAdvertParms;
    ListEntry                   leAdvertHandlerList;
    BOOL                        leAdvertisingEnabled;
    U8                          leAdvertData[31];
    U8                          leAdvertDataLen;
    BOOL                        leMoreAdvertData;
    CmgrLeAdvertHandler        *leAdvertHandler;
    BOOL                        leOnlyMode;
    CmgrLePrivState             lePrivState;
    BtStatus                    leScanStatus;
    CmgrLeAdTypeMask            leAdTypeMask;
    BOOL                        leWhiteListChange;
    BOOL                        leFatalError;

#if (LE_PRIVACY == XA_ENABLED) || (LL_PRIVACY == XA_ENABLED)
    ListEntry                   lePrivHandlers;
#endif

#if LE_PRIVACY == XA_ENABLED
    CmgrHandler                *leCurGenHandler;
    BtDevAddr                   leCurRandomAddr;
    BtDevAddr                   leRpa; /* Resolvable Private Address */
    BOOL                        lePrivacy;
    CmgrLeAdTypeMask            leAdTypePrivMask;
    TimeT                       leAddrChangeTime;
    EvmTimer                    leAddrChangeTimer;
    U8                          lePrivacySM;   /* LE privacy state machine state */
#endif /* LE_PRIVACY == XA_ENABLED */

#if LL_PRIVACY == XA_ENABLED
    BtDeviceContext             localDeviceBdc;             /* BDC used to add our local IRK to the Resolving List */
    U8                          leNumInResolvingList;       /* Number of items in Resolving List */
    U8                          leNumRemovingRL;            /* Number of items pending removal from the Resolving List */
    ListEntry                   leAddrResolveList;          /* Items in Resolving List */
    ListEntry                   lePendResolveList;          /* Pending items to put in Resolving List */
    BtDeviceContext            *llResolveBdc;               /* Pointer to BDC for the ongoing Add/Remove Resolving Operation */
    MeLeOpHandler               llReadLocalRpa;             /* Read local RPA operation handler */
    MeLeOpHandler               llReadPeerRpa;              /* Read peer RPA operation handler */
    MeLeOpHandler               llReadNewLocalRpa;          /* Read the newly changed local RPA operation handler */
    MeLeOpHandler               llReadNewPeerRpa;           /* Read the newly received peer RPA operation handler */
    CmgrLePrivResolveOpState    leLLPrivacySM;              /* LE link layer privacy state machine state */
    MeLeAddressType             leConAddrType;              /* Full address type of the connecting device */
    CmgrLeLLPrivState           leLLPrivState;              /* Current state of Link Layer Privacy */
    ListEntry                   leAdvertReportResolveList;  /* Items in the Advertising Report Resolving List */
    BtDeviceContext            *llAdvertReportResolveBdc;   /* Pointer to BDC for the ongoing Advert Report Resolving Operation */
#endif /* LL_PRIVACY == XA_ENABLED */

#endif /* NUM_LE_DEVICES > 0 */

#if NUM_SCO_CONNS > 0
    BtHandler                   scoHandler;
    BOOL                        scoRequest;
    CmgrAudioParms              scoParms;
    CmgrAudioParms              scoDefaultParms;
    CmgrAudioParms              scoAltDefaultParms;
    BtScoTxParms                scoCustomParms;
    EvmTimer                    scoRetryTimer;
#endif /* NUM_SCO_CONNS > 0 */

    U8                          hciSupportedCommands[64];
    U8                          numLocalSupportedCodecs;
    U8                          localSupportedCodecs[8];

    BtRemoteDevice             *cancelRemDev;
} BtCmgrContext;


#if CMGR_MEMORY_EXTERNAL == XA_DISABLED
#if XA_CONTEXT_PTR == XA_ENABLED
extern BtCmgrContext  cmgrCtxTmp;
extern BtCmgrContext *cmgrContext = &cmgrCtxTmp;
#define CMGR(s) (cmgrContext->s)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern BtCmgrContext cmgrContext;
#define CMGR(s) (cmgrContext.s)
#endif /* XA_CONTEXT_PTR */
#endif /* CMGR_MEMORY_EXTERNAL == XA_DISABLED */


/********* Function prototypes ********************/
BOOL ConnMgrIsLinkUp(CmgrHandler *Handler);
void CMGR_InitTypes(void);
BOOL ConnMgrIsBdcBusy(BtDeviceContext *bdc);
void ConnManager(const BtEvent *Event);
BOOL CmgrIsLe(CmgrHandler *Handler);
void ConnMgrCheckStartSniffTimer(BtRemoteDevice *RemDev);
CmgrSniffExitPolicy ConnMgrGetSniffExitPolicy(CmgrHandler *Handler);

#if NUM_SCO_CONNS > 0
void ConnMgrScoRetryTimeout(EvmTimer *timer);
BtStatus ConnMgrCreateAudioLink(BtScoConnect **scoConnect,
                                BtRemoteDevice *RemDev, U8 flag);
BOOL ConnMgrScoNotify(const BtEvent *btEvent,
                             CmgrEvent Event, BtStatus Status);
BOOL ConnMgrIsAudioUp(BtRemoteDevice *RemDev);
BtStatus ConnMgrAcceptAudioLink(BtScoConnect *scoConnect);
#endif /* NUM_SCO_CONNS > 0 */

#if NUM_LE_DEVICES > 0
void CmgrLeInit(void);
void CmgrLeSetHandlerDefaults(CmgrHandler *Handler);
BtStatus CmgrLeCreateLink(CmgrHandler *Handler);
void ConnLeManager(void *ctx, MeLeEvent *Event);
BtStatus CmgrLeRemoveDataLink(CmgrHandler *handler);
BtStatus CmgrLeFinishCreateLink(CmgrHandler *Handler, BOOL reqOnList);
BtStatus CmgrLeCheckIfConRequest(void);
void CmgrLeFatalError(BtStatus status, BtErrorCode ec);
void CmgrLeClearState(void);
BOOL IsCmgrAdvertBusy(CmgrLeAdvertState excludedState);
BOOL CmgrLeSetupAdvert(CmgrLeAdvertHandler *handler);
BtStatus CmgrLeSetScanRspData(void);
U8 CmgrLeBuildResponseData(U8 *data, U8 dataLen, BOOL advertise, BOOL *more);
void CmgrLeAdvertTimerHandler(EvmTimer *timer);
void CmgrLeStartAdvertTimer(void);
void CmgrLeCancelAdvertTimer(void);
CmgrLeAdvertDataFlags CmgrLeGetAdvertDataFlags(void);
void CmgrLeSendAdvertEvent(CmgrEventEx eType, BtStatus status,
                           BtErrorCode errCode, CmgrLeStopAdvertReason reason);
BtStatus CmgrLeSetAdvertParms(BOOL checkParms);
BtStatus CmgrLeSetAdvertData(void);
BtStatus CmgrLeAdvertise(BOOL enable);
BtStatus CmgrLeCheckPrivacyAdvertParms(void);
BtStatus CmgrLeFinishStartAdvertising(void);
BOOL CmgrLeHandleAdvertEnableConfirm(MeLeEvent *Event);

#if LE_PRIVACY == XA_ENABLED
BtStatus CmgrLeEnableDisablePrivacy(BtStatus status);
void     CmgrLeStartPrivTimer(void);
#endif /* LE_PRIVACY == XA_ENABLED */

#if LL_PRIVACY == XA_ENABLED
void     CmgrLeCreateRpa(BtDevAddr *devAddr);
void CmgrLeClearLLPrivacyState(void);
BtStatus CmgrLeStartAddToResolveList(BtDeviceContext *bdc);
BtStatus CmgrLeStartRemoveFromResolveList(BtDeviceContext *bdc);
BtStatus CmgrLeStartLLPrivacy(BtStatus status);
#endif /* LL_PRIVACY == XA_ENABLED */

#endif /* NUM_LE_DEVICES > 0 */
#endif /* __CONMGRI_H_ */

