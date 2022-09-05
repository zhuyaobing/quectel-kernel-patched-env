/****************************************************************************
 *
 * File:
 *     melei.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains internal definitions for the Bluetooth
 *     LE Management Enity.
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

#ifndef __MELEI_H
#define __MELEI_H

#include "mele.h"
#include "sec.h"
#include "sys/hci.h"
#include "sys/le_sec_i.h"

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * BtOpType defines Bluetooth operations. The typedef is in me.h
 * LE operations are defined in melei.h and start at 0x80
 */
#define BOP_LE_SET_RAND_ADDR          0x80
#define BOP_LE_SET_ADVERT_PARMS       0x81
#define BOP_LE_SET_ADVERT_DATA        0x82
#define BOP_LE_SET_SCAN_DATA          0x83
#define BOP_LE_SET_SCAN_PARMS         0x84
#define BOP_LE_SET_ADVERT_ENABLE      0x85
#define BOP_LE_SET_SCAN_ENABLE        0x86
#define BOP_LE_LINK_CONNECT           0x87
#define BOP_LE_READ_REM_FEATURES      0x88
#define BOP_LE_CANCEL_LINK_CONNECT    0x89
#define BOP_LE_CLEAR_WHITE_LIST       0x8A
#define BOP_LE_ADD_WHITE_LIST         0x8B
#define BOP_LE_REMOVE_WHITE_LIST      0x8C
#define BOP_LE_CONNECT_UPDATE         0x8D
#define BOP_LE_SET_HOST_CHANNEL_CLASS 0x8E
#define BOP_LE_READ_CHANNEL_MAP       0x8F
#define BOP_LE_START_ENCRYPTION       0x90
#define BOP_LE_RAND                   0x91
#define BOP_LE_ENCRYPT                0x92
#define BOP_LE_RX_TEST                0x93
#define BOP_LE_TX_TEST                0x94
#define BOP_LE_TEST_END               0x95
#define BOP_LE_REPLY_LTK              0x96
#define BOP_LE_NEG_REPLY_LTK          0x97
#define BOP_LE_GEN_RAND_ADDR          0x98
#define BOP_LE_RESOLVE_RAND_ADDR      0x99
#define BOP_LE_CONNECT_PARM_REPLY     0x9C
#define BOP_LE_CONNECT_PARM_NEG_REPLY 0x9D
#define BOP_LE_SET_DATA_LENGTH        0x9E
#define BOP_LE_READ_SUGG_DEFAULT_LEN  0x9F
#define BOP_LE_WRITE_SUGG_DEFAULT_LEN 0xA0
#define BOP_LE_READ_LOC_P256_PUB_KEY  0xA1
#define BOP_LE_GEN_DHKEY              0xA2
#define BOP_LE_ADD_RESOLVING_LIST     0xA3
#define BOP_LE_REMOVE_RESOLVING_LIST  0xA4
#define BOP_LE_CLEAR_RESOLVING_LIST   0xA5
#define BOP_LE_READ_RESOLVING_SIZE    0xA6
#define BOP_LE_READ_PEER_RPA          0xA7
#define BOP_LE_READ_LOCAL_RPA         0xA8
#define BOP_LE_SET_ADDR_RESOLVE_ENBL  0xA9
#define BOP_LE_SET_RESOLVE_TIMEOUT    0xAA
#define BOP_LE_READ_MAX_DATA_LENGTH   0xAB
#define BOP_LE_SET_DEFAULT_DATA_LEN   0xAC


/* These must be modified to the first and last LE ops */
#define BOP_LE_FIRST_OP               0x80
#define BOP_LE_LAST_OP                0xAC


/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HciLeSubEventCode type
 *
 *     These sub-event codes are indicated by HCI to the Management Entity
 *     during an HCI_CONTROLLER_EVENT callback when the HciEventType is
 *     HCE_LE_META_EVENT. The sub_event code is found in the
 *     "hciEvent->parms[0]" field of the callback parameters structure
 *     (HciCallbackParms) when hciEvent->event field equals
 *     HCE_LE_META_EVENT.
 */
typedef U8 HciLeSubEventCode;

#define HLSE_CONNECTION_COMPLETE           0x01
#define HLSE_ADVERT_REPORT                 0x02
#define HLSE_CONNECTION_UPDATE_COMPLETE    0x03
#define HLSE_READ_REMOTE_FEATURES_COMPLETE 0x04
#define HLSE_LT_KEY_REQ                    0x05
#define HLSE_REMOTE_CONN_PARM_REQUEST      0x06
#define HLSE_LE_DATA_LENGTH_CHANGE         0x07 /* 4.2 */
#define HLSE_LE_READ_LOCAL_P256_COMPLETE   0x08 /* 4.2 */
#define HLSE_LE_GEN_DHKEY_COMPLETE         0x09 /* 4.2 */
#define HLSE_LE_ENHANCED_CONNECT_COMPLETE  0x0A /* 4.2 */
#define HLSE_LE_DIRECT_ADVERT_REPORT       0x0B /* 4.2 */

/* End of HciLeSubEventCode */

/*---------------------------------------------------------------------------
 * MeLePairingState type
 *
 *     Pairing State of a Remote Device connected over LE
 */
typedef U8 MeLePairingState;

#define MLPS_NOT_PAIRED            0x00
#define MLPS_START_PAIRING         0x01
#define MLPS_START_REPAIRING       0x02
#define MLPS_PAIRED                0x03

/* End of MeLePairingState */

/*---------------------------------------------------------------------------
 * MeLeAuthorizeState type
 *
 *     Authorization State of a Remote Device connected over LE
 */
typedef U8 MeLeAuthorizeState;

#define MLAS_NOT_AUTHORIZED     0x00
#define MLAS_START_AUTHORIZE    0x01
#define MLAS_AUTHORIZED         0x02

/* End of MeLeAuthorizeState */

/*---------------------------------------------------------------------------
 * MeLeInitRadioState definitions
 *
 *     Support for the different states necessary to perform LE radio
 *     initialization. The init function cycles through the states by
 *     incrementing the state after the previous command is completed and
 *     state order matters.
 */
typedef U8 MeLeInitRadioState;

#define MLIRS_LE_LOCAL_FEATURES         0x00
#define MLIRS_ENABLE_LE_EVENTS          0x01
#define MLIRS_READ_ADVERT_TX_PWR        0x02
#define MLIRS_SUPPORTED_STATES          0x03
#define MLIRS_WHITE_LIST_SIZE           0x04
#define MLIRS_LOCAL_PUBLIC_KEY          0x05
#define MLIRS_READ_MAX_DATA_LEN         0x06
#define MLIRS_READ_DEFAULT_DATA_LEN     0x07
#define MLIRS_RESOLVING_LIST_SIZE       0x08
#define MLIRS_ENABLE_ADDR_RESOLUTION    0x09

#define MLIRS_INIT_DONE                 0xFF

/* End of MeLeInitRadioState */

/*---------------------------------------------------------------------------
 *
 * MeLeSetRandAddrOp is used to hold a Set Random Address operation on
 * a queue.
 */
typedef struct {
    BtOperation      op;
    BD_ADDR          bdAddr;
} MeLeSetRandAddrOp;

/*---------------------------------------------------------------------------
 *
 * MeLeResolvingRandAddrOp is used to hold a Resolve Random Address operation on
 * a queue.
 */
typedef struct {
    MeLeOpHandler   *handler;
    U32              hash;      /* Stores the generated random hash */
} MeLeResolvingRandAddrOp;

/*---------------------------------------------------------------------------
 *
 * MeLeAdvertParmsOp is used to hold a Set Advertising Parameter operation on
 * a queue.
 */
typedef struct {
    BtOperation      op;
    MeLeAdvertParms  parms;
} MeLeAdvertParmsOp;

/*---------------------------------------------------------------------------
 *
 * MeLeAdvertDataOp is used to hold an Advertising Data operation on
 * a queue.
 */
typedef struct {
    BtOperation      op;
    U8               len;
    U8               data[31];
} MeLeAdvertDataOp;

/*---------------------------------------------------------------------------
 *
 * MeLeScanDataOp is used to hold an Scan Data operation on a queue.
 */
typedef struct {
    BtOperation      op;
    U8               len;
    U8               data[31];
} MeLeScanDataOp;

/*---------------------------------------------------------------------------
 *
 * MeLeScanParmsOp is used to hold a Set Scan Parameter operation on
 * a queue.
 */
typedef struct {
    BtOperation      op;
    MeLeScanParms    parms;
} MeLeScanParmsOp;

/*---------------------------------------------------------------------------
 *
 * MeLeAdvertEnableOp is used to hold a Set Advertising enable operation on
 * a queue.
 */
typedef struct {
    BtOperation      op;
    MeLeEnable       enable;
} MeLeAdvertEnableOp;

/*---------------------------------------------------------------------------
 *
 * MeLeScanEnableOp is used to hold a Set Scan enable operation on
 * a queue.
 */
typedef struct {
    BtOperation      op;
    MeLeEnable       enable;
    MeLeEnable       filterDuplicates;
} MeLeScanEnableOp;

/*---------------------------------------------------------------------------
 *
 * MeLeWhiteListeOp is used for White List operations.
 */
typedef struct {
    BtOperation      op;
    BD_ADDR          bdAddr;
    MeLeAddressType  addrType;
} MeLeWhiteListOp;

/*---------------------------------------------------------------------------
 *
 * MeLeSetChanClassOp is used for Set Channel Classification operations.
 */
typedef struct {
    BtOperation      op;
    U8               channelMap[5];
} MeLeSetChanClassOp;

/*---------------------------------------------------------------------------
 *
 * MeLeAuthorizeOp is used to hold an Authorization operation on
 * a queue.
 */
typedef struct {
    BtOperation      op;
    BD_ADDR          bdAddr;
} MeLeAuthorizeOp;

/*---------------------------------------------------------------------------
 *
 * MeLeP256PublicKeyOp is used to hold a Read P-256 Public key operation on
 * a queue.
 */
typedef struct {
    BtOperation      op;
    MeLeOpHandler   *opHandler;

} MeLeP256PublicKeyOp;

/*---------------------------------------------------------------------------
 *
 * MeLeSecurityKeys is used to store SMP security keys for the local and
 * remote devices.
 */
typedef struct {
    /* Describes the LE security keys (LTK, CSRK, and IRK) that are stored */
    LeSmpKeyDistributionFormat storedKeys;

    U16              ediv;                          /* EDIV related to the LTK for this connection */
    BOOL             isSTK;                         /* TRUE if stored key is STK. FALSE if LTK */
    U8               rand[8];                       /* RAND related to the LTK for this connection */
    U8               linkKey[BT_LE_KEY_LENGTH];     /* STK or LTK link key for this connection */
    U8               signingKey[BT_LE_KEY_LENGTH];  /* CSRK signing key for this connection */
    U32              signedCounter;                 /* Last received signed counter value */
    BOOL             signedCounterValid;            /* TRUE means value in signedCounter is valid */
    U8               identityKey[BT_LE_KEY_LENGTH]; /* IRK identity key for this connection */
} MeLeSecurityKeys;

/*---------------------------------------------------------------------------
 *
 * MeLeResolvingListOp is used to store identity information for a local
 * and remote device pair for controller add/remove resolving list operations.
 */
typedef struct {
    BtOperation       op;
    BtDevAddr         peerIdentityAddr;
    U8                peerIrk[BT_LE_KEY_LENGTH];
    U8                localIrk[BT_LE_KEY_LENGTH];
} MeLeResolvingListOp;

/*---------------------------------------------------------------------------
 *
 * MeLeClearResolvingListOp is used to clear all local and device pair 
 * information for controller clear resolving list operations.
 */
typedef struct {
    BtOperation       op;
} MeLeClearResolvingListOp;

/*---------------------------------------------------------------------------
 *
 * MeReadResolveAddrOp is used to hold the identity address for which the host is
 * requesting either the local or peer resolvable private address
 */
typedef struct {
    BtOperation       op;
    BtDevAddr         peerIdentityAddr;
} MeLeReadResolveAddrOp;

/*---------------------------------------------------------------------------
 *
 * MeLeAddrResolveEnableOp is used to hold a Set Address Resolution enable 
 * operation on a queue.
 */
typedef struct {
    MeLeOpHandler   *handler;
    BtOperation      op;
    MeLeEnable       enable;
} MeLeAddrResolveEnableOp;

/*---------------------------------------------------------------------------
 *
 * MeLeSetRpaTimeoutOp is used to set the timeout value the controller uses
 * to change the local and peer resolvable private addresses.
 */
typedef struct {
    BtOperation     op;
    U16             rpaTimeout;
} MeLeSetRpaTimeoutOp;

/*---------------------------------------------------------------------------
 *
 * MeLeRemoteDevice represents a link to an LE remote device.
 */
typedef struct _MeLeRemoteDevice {
    /* Inherit from BtRemoteDevice */
    BtRemoteDevice       rd;
    BtOperation          remFeaturesOp;
    BtOperation          updateLinkOp;
    BtOperation          readChannelMapOp;
    BtOperation          startEncryptOp;
    BtOperation          cancelOp;
    BtOperation          replyLTKey;
    BtOperation          setDataLenOp;

    /* Parameters used in Link creation and update */
    BD_ADDR              createPeerAddr;
    MeLeAddressType      createPeerAddrType;
    MeLeAddressType      createOwnAddrType;
    U16                  createScanInterval;
    U16                  createScanWindow;
    MeLeInitFilterPolicy createFilterPolicy;
    U16                  createConnIntervalMin;
    U16                  createConnIntervalMax;
    U16                  createConnLatency;
    U16                  createSuperTimeout;
    U16                  createMinCeLength;
    U16                  createMaxCeLength;

    /* Parameters used in encrypting the link */
    U8                   rand[8];
    U16                  encryptDiv;
    U8                   LTKey[16];

    /* Parameters of the active link */
    /*MeLeAddressType      remAddrType;*/   /* Type of address for remote device */
    U16                  connInterval;  /* Connection interval used on this connection */
    U16                  connLatency;   /* Connection latency for this connection */
    U16                  superTimeout;  /* Connection supervision timeout. */

    U16                  sugMaxTxOctets;   /* Suggested max tx octets for this connection */
    U16                  sugMaxTxTime;     /* Suggested max tx time for this connection */

    /* Accuracy of the master's clock. The masterClockAccuracy parameter is
     * only valid for a slave. On a master, this parameter is set to 0x00.
     */
    MeLeMasterClockAccuracy masterClockAccuracy;

    /* Security parameters */
    U8                          encryptKeySize;     /* Encryption key size of LTK */
    BtIoCapabilities            ioCap;               /* IO Capabilities of the remote device */
    MeLeEncryptState            encryptState;       /* Current encryption state */
    MeLePairingState            pairingState;       /* Current pairing state */
    MeLeAuthorizeState          authorizeState;     /* Current authorization state */
    MeLeKeyType                 keyType;            /* Type of link keys for this connection */
    BOOL                        secureCon;          /* TRUE indicates secure connections was used to generate the keys */
    MeLeSecurityKeys            localKeys;          /* Security keys for the local device */
    MeLeSecurityKeys            remoteKeys;         /* Security keys for the remote device */
    LeSmpAuthRequirements       curAuthReq;         /* Auth Requirement currently being attempted */
    BOOL                        smpSecPairingMode;  /* LE Security Pairing mode */
    LeSmpPairingFailureReason   smpFailureReason;   /* SMP security failure reason code */
    ListEntry                   secTokens;          /* List of security tokens */
    ListEntry                   fixSecHandlers;     /* List of bound LE fix Security handlers */

    /* Table to hold pointer to the fixed channel clients. Call FIXMGR_FindClientIndex() to
     * get the index of a specific fixed channel client.
     */
    void           *fixedLeClients[LEL2CAP_NUM_FIXED_CHANNELS];

    /* The identity address is the address used for identifying this remote device in the DDB. This most
     * often the public/static address of the device except in cases where the remote device does not provide a
     * public/static address and therefore its private address is used.
     */
    BtDevAddr            identityAddr;

    /* Local device address used during the connection */
    BtDevAddr            localDevAddr;
  
#if LL_PRIVACY == XA_ENABLED
    /* Link Layer privacy local resolvable device address used during the connection.  
     * NOTE: This value may vary from the RPA used when advertising or connected to other devices.
     */
    BtDevAddr            localResolvableAddr;
    MeLeAddressType      conAddrType;
#endif /* LL_PRIVACY == XA_ENABLED */

    BOOL                 isRemFeaturesComplete;
    U8                   remoteFeatures[8];

    MeLeUpdateLinkParms  reqConnParms;      /* Requested connection parameters from remote device */
    BOOL                 rcvdReqConnParms;  /* Have we received a connection parameters request */

    /* Maximum length and timer parameters currently used on the link */
    MeLeDataLengthParms  dataLenParms;

} MeLeRemoteDevice;


/* Function prototypes */
void MeLeStartOperation(void);
BtStatus MELE_Init(void);
BOOL MeLeHandleMetaEvent(U8 len, U8* parm);
void MeLeHandleAuthPayloadTimeout(U8 len, U8* parm);
MeLeRemoteDevice *MeLeFindRemoteDevice(BtDevAddr *BdAddr);
MeLeRemoteDevice *MeLeGetFreeRemoteDevice(void);
void MeLeDisconnectLink(MeLeRemoteDevice *remDev);
void MeLeHandleDisconnectResult(MeLeRemoteDevice *remDev);
void MeLeHandleDisconnectComplete(MeLeRemoteDevice *remDev, U8* parm);
void MeLeFreeRemoteDevice(MeLeRemoteDevice *remDev);
void MeLeDevEventHandler(BOOL checkSecurity, BtDeviceRecord *drec);
void MeLeReportLinkUpdate(MeLeRemoteDevice* remDev);
MeLeRemoteDevice* MeLeMapHciToRemoteDevice(HciHandle hciHandle);
void MeLeCallLinkHandlers(MeLeRemoteDevice* remDev);
void MeLeReportResults(MeLeEventMask mask);
BtStatus MeleCreateLink(MeLeLinkHandler *handler, MeLeCreateLinkParms *parms, BtDevAddr *identityAddr);
BtStatus MeLeStartEncryption(MeLeRemoteDevice *remDev,U8 *rand,
                             U16 encryptDiv,  U8 *LTKey);
void MeLeSetEncryptInRemoteFeatures(MeLeRemoteDevice *remDev);
#define MeLeSetEncryptInRemoteFeatures(r) (r)->remoteFeatures[0] |= 1
void MeLeReportSecEncryptionResult(MeLeRemoteDevice* remDev, BtEncryptMode mode);

#endif /* __MELEI_H */
