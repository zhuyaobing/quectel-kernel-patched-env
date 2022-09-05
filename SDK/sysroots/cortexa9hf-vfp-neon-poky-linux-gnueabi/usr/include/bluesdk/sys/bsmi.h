/****************************************************************************
 *
 * File:
 *     bsmi.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description: Internal include file for BSM.
 *
 * Copyright 2010-2017 OpenSynergy GmbH.
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
#ifndef __BSMI_H
#define __BSMI_H

#include "bsm.h"
#include "bsm_persmgr.h"
#include "bsm_agentmgr.h"
#include "sec.h"
#include "sdp.h"
#include "did.h"
#include "l2achanmgr.h"
#include "oslib.h"
#if NUM_AMPS > 0
#include "ampmgr.h"
#endif /* NUM_AMPS > 0 */
#include "conmgr.h"

#if defined(NUM_LE_DEVICES) && NUM_LE_DEVICES > 0
#include "gattclient.h"
#include "gattsecurity.h"
#endif /* NUM_LE_DEVICES > 0 */

/****************************************************************************
 *
 * Internal types and defines.
 *
 ****************************************************************************/

#define BSM_REC_LOCALNAME 0x01      /* Record ID for local device name setting */
#define BSM_REC_LOCALMODE 0x02      /* Record ID for local mode bits */
#define BSM_REC_LOCALDEVICE 0x03    /* Record ID for Local device address */
#define BSM_REC_REMOTEDEVICES 0x04  /* Record ID for Remote device IDs */
#define BSM_REC_LOCALSERVICES 0x05  /* Record ID for Services */
#define BSM_REC_GENERAL_INFO 0x06   /* BsmGeneralInfo storage */

/* Additional BsmServiceMode */
#define BSM_LSF_UNMATCHED           0x0001
#define BSM_LSF_ENABLING            0x0002
#define BSM_LSF_USER_ENABLE         0x0004
#define BSM_LSF_INFO_DIRTY          0x0008

#define BSM_RSF_EIR_ONLY            0x0001
#define BSM_RSF_EIR_NONFOUND        0x0002
#define BSM_RSF_SDP_NONFOUND        0x0004
#define BSM_RSF_PENDING_CONNECT     0x0008

#define BSM_NUM_AD_TYPES            6

#define LPS_DISABLED   0 /* No agent yet */
#define LPS_ENABLING   1 /* Waiting for agent */
#define LPS_ENABLED    2 /* Agent present */
#define LPS_DISABLING  3 /* Waiting for agent shutdown */

/* Service modes we actually persist */
#define BSM_S_PERS_MODES (BSM_S_TYPE_MASK | BSM_S_USER_REQUESTED | BSM_S_AVAILABLE | BSM_S_AUTHORIZED)

/* Generic device members */
typedef struct _BsmDevice
{
    ListEntry           node;               /* Must be first */
    BD_ADDR             addr;
    U8                 *friendlyName;
    BtClassOfDevice     deviceClass;        /* Class of device */
} BsmDevice;

/* Local device flags */
#define BLDF_ENABLED                0x0001 /* Device is initialized & usable */
#define BLDF_EIR_OUTSTANDING        0x0002 /* An EIR operation is in progress */
#define BLDF_INQUIRING              0x0004 /* An inquiry is currently in progress */
#define BLDF_DISCOVER_CONNECT       0x0008 /* A discovery-based connection is in progress */
#define BLDF_DISCOVER_NAME          0x0010 /* A discovery-based name query is in progress */
#define BLDF_DISCOVER_SDP           0x0020 /* A discovery-based SDP_Query is in progress */
#define BLDF_LE_SCAN                0x0040 /* LE scanning is in progress */
#define BLDF_DISCOVER_DEVID         0x0080 /* A discovery-based SDP_Query for Device ID info is in progress */

#if NUM_LE_DEVICES > 0
#define BLDF_DISCOVER_LENAME        0x0100 /* A discovery-based LE name query is in progress */
#endif /* NUM_LE_DEVICES > 0 */

/*Local Device Scan stage*/
#define BLSS_IDLE                   0x00
#define BLSS_BEGIN                  0x01
#define BLSS_EDR                    0x02
#define BLSS_LE                     0x03
#define BLSS_DONE                   0x04

/* Local device members */
struct _BsmLocalDevice
{
    BsmDevice           s;                  /* Must be first */
    U16                 flags;              /* BLDF_... */
    BsmDiscoverFlag     reqDiscover;
    BsmDiscoverFlag     activeDiscover;
    U8                  discoverStage;      /* BLSS_...*/
    BtIac               inquiry_iac;        /* BT_IAC_... */
    BsmLocalDeviceMode  mode;               /* BSM_LD_... */
    BsmLocalDeviceMode  reqMode;            /* Requested mode changes that may not be active */
    BsmLocalDeviceMode  activeMode;
    EvmTimer            modeTimer;

    EvmTimer            discoveryTimer;     /* Discovery timer */

#if NUM_LE_DEVICES > 0
    MeLeEnable          advertEnable;       /* Current state of LE advertisement mode */
    MeLeAdvertType      advertType;         /* Current LE advertisement type selected */
    CmgrLeAdvertType    leDiscAdvType;      /* The discovery advertisement type */
    BOOL                autoRestartAdvert;  /* Automatically restart advertising on disconnect. */
    BOOL                advertLeOnly;       /* Advertising indicating LE only support */
    EvmTimer            advertRetryTimer;   /* Advertising retry timer */
    BOOL                advertInitRetry;    /* Advertising initialization retry option */
    BOOL                restartingAdvert;   /* Advertising is being restarted internally */
    BOOL                allPrivacyAdMask;   /* All AD Tags are included when privacy is enabled */
    BOOL                advertRetryAfterUpdate; /* Retry advertising after ongoing data update completes */
    MeLeEnable          pendingAdvertEnable;    /* Pending LE advertisement mode (waiting on data update) */
#endif /* NUM_LE_DEVICES > 0 */
};

/* Remote device information (persisted) */
typedef struct _BsmRemoteDeviceInfo
{
    /* Note: if the format of this structure is changed,
     * BSM_PERSISTENCE_FORMAT_VERSION should be incremented.
     */
    BtDevAddr           addr;        /* Current Device address */
    BtDevAddr           addri;       /* Identity address */
    BtDevAddr           directedRpa; /* Directed RPA address */

    BtClassOfDevice     deviceClass; /* Class of device */
    U8                  psRepMode;   /* Most recently observed page scan repetition mode */
    BsmRemoteDeviceMode mode;        /* Mode bits */
    DevIdInfo           devId;       /* Device ID information */
    U16                 dualModeId;  /* Dual Mode unique identifier */
} BsmRemoteDeviceInfo;

#define BRDF_INFO_DIRTY     0x0001      /* Info struct needs committing */
#define BRDF_NAME_DIRTY     0x0002      /* Friendly name needs committing */
#define BRDF_SERVICES_DIRTY 0x0004      /* Services need committing */
#define BRDF_UNANNOUNCED    0x0008      /* BSM user should be told */
#define BRDF_HANDLER_BOUND  0x0010      /* aclHandler is in use by ME */
#define BRDF_AUTHENTICATED  0x0020      /* Connected remdev is currently authenticated */
#define BRDF_MODE_INHIBIT   0x0040      /* Do not announce mode changes */
#define BRDF_MODE_DIRTY     0x0080      /* An inhibited mode update is pending */

/* Remote device mode bits that are persistable */
#define BSM_RD_PERSISTABLES (BSM_RD_BLOCKED)

/* Internal remote device members */
struct _BsmRemoteDevice
{
    ListEntry           node;            /* Must be first */
    U8                 *friendlyName;    /* Name of this device as advertised */
    S8                  rssi;            /* Current RSSI level */
    U16                 flags;           /* Flags (BRDF_*) */
    U16                 clockOffset;     /* Most recently observed clock offset */
    ListEntry           services;        /* List of known remote services */
    MeCommandToken      nameToken;       /* Token to retrieve name */

    /* Related to sdp queries during discovery */
    struct {
        SdpQueryToken       token;        /* Token to retrieve services */
        U8                  query[12];    /* Storage for current SDP service query request */
        BsmLocalService    *localService; /* Current local service driving SDP queries */
        U8                 *serviceName;  /* Handle to the service name being parsed via continuation state */
    } sdp;

    ListEntry           discoverNode;    /* Node used to manage outstanding discoveries */

    CmgrHandler         cmgrHandler;     /* Handler used for ACL creation. */

    BtRemoteDevice     *remDev;          /* Pointer to ME's rem dev, if connected */

    ListEntry           remoteAmps;      /* List of known AMP controllers on this dev */

#if defined(NUM_LE_DEVICES) && NUM_LE_DEVICES > 0
    GattClientToken     gattToken;
    BOOL                isGattConnected;
#endif /*NUM_LE_DEVICES > 0*/

    /* Persisted information stored here */
    BsmPersRecordId     rec;
    U32                 recLen;
    BsmRemoteDeviceInfo p;

    BsmDiscoverFlag discoverFlags;
};

/* Generic service information (persisted) */
typedef struct _BsmServiceInfo
{
    /* Note: if the format of this structure is changed,
     * BSM_PERSISTENCE_FORMAT_VERSION should be incremented.
     */
    BsmServiceClassUuid type;        /* Bluetooth profile type */
    BsmServiceMode      mode;        /* Service mode bits. */
    U32                 id;          /* ID value */
    BsmPersRecordId     agentRec;    /* Agent-specific record ID for this service */
    BsmSecuritySettings secSettings;    /* Required security level parameters */
} BsmServiceInfo;

/* Internal generic service fields */
typedef struct _BsmService
{
    ListEntry node;             /* Must be first */
    U8 *name;                   /* Service name */
    BsmServiceInfo p;           /* Persistable info */
} BsmService;

/* Internal local service fields */
struct _BsmLocalService
{
    BsmService          s;
    U8                  enableState;    /* LPS_... */
    U16                 flags;          /* Local service flags BSM_LSF_* */
    BsmServiceAgent    *agent;
    U8                 *remoteName;
    BsmServiceClassUuid remoteType;
    ListEntry           rservices;      /* Connected BsmRemoteService objects */
    BsmPersRecordId     rec;            /* Associated record ID if any */
    BtSecurityRecord    secRecord;
    BOOL                bGatt;          /* If TRUE, use GATT for discovery.  Otherwise
                                           use SDP.
                                        */
};

/* Internal remote service fields */
struct _BsmRemoteService
{
    BsmService           s;             /* Superclass data */
    U16                  flags;         /* Remote service flags BSM_RSF_* */
    BsmRemoteDevice     *remoteDevice;  /* Remote device owning this service */
    ListEntry            lsNode;        /* Node used by BsmLocalService for list management */
    ListEntry            l2capAgentTrackers;   /* List of trackers known L2CAP agents */
    BsmLocalService     *localService;  /* Local service with which we are connected */
};

/* A structure used to track other structures */
typedef struct _BsmTracker
{
    ListEntry node;

    /* Item being tracked */
    union {
        L2capAgent *l2capAgent;
        void       *item;
    } item;

    /* Extra data which may be associated with the item */
    BsmAmpPair pair;

} BsmTracker;

/* General information (persisted) */
typedef struct _BsmGeneralInfo
{
    /* Note: if the format of this structure is changed,
     * BSM_PERSISTENCE_FORMAT_VERSION should be incremented.
     */
    BsmSecuritySettings         secDefault;         /* Required security level parameters */
    BtIoCapabilities            secIoCapBr;         /* IO Capability setting for BR/EDR */
#if defined(NUM_LE_DEVICES) && NUM_LE_DEVICES > 0
    U8                          secLeKeySize;       /* LE Encryption Key Size */
    BsmLeKeyDistribution        secLeKeyDistrib;    /* LE key distribution - local/remote devices */
    MeLeAdvertType              advertisingType;    /* LE Advertising type */
    CmgrLeAdvertDiscoveryMode   leDiscoveryMode;    /* LE Discovery mode */
    BOOL                        lePrivacyEnabled;   /* LE Privacy enabled */
#endif /* NUM_LE_DEVICES */
} BsmGeneralInfo;

typedef struct _BsmContext
{
    BsmAgentManager *agentMgr;
    BsmPersistenceManager *persMgr;
    BtHandler meHandler;
    BtHandlerEx *handler;
    BtHandlerEx agentMgrHandler;
    CmgrHandler cmgrHandler;
#if defined(NUM_LE_DEVICES) && NUM_LE_DEVICES > 0
    MeLeHandler meleHandler;
    BtHandlerEx    leSecHandler;
    GattClientToken gattCliToken;
    CmgrLeScanHandler cmgrLeScanHandler;
    CmgrLeAdvertHandler cmgrLeAdvertHandler;
    CmgrLeAdvertUuid cmgrLeAdvertUuid;
    CmgrLeAdvertUuid devInfoAdvertUuid;
    /* add TLV for AD Types set by CMGR_AddLeAdvertTLV */
    CmgrLeAdvertTLV cmgrLeAdvertTLV[BSM_NUM_AD_TYPES];
    GattSdpRecord    devInfoSdpRecord;
    void *lookupCtx;
    MeLeUpdateLinkParms leConnParms;
#if LE_PRIVACY
    BtHandlerEx cmgrLePrivacyHandler;
#endif /* LE_PRIVACY */
#endif /*NUM_LE_DEVICES*/
    BtHandlerEx channelMgrHandler;
    ListEntry remoteDevices;
    ListEntry localServices;
    ListEntry notifications;
    ListEntry toDiscover;
    ListEntry userRequests;
    ListEntry localAmps;
    BsmRemoteDevice *discovering;
    EvmTimer timer;
    U16 pubCall;
    BOOL memoryError;
    BsmEvent memoryEvt;
    BsmLocalDevice localDevice;
    BsmGeneralInfo p; /* Persisted */
    BOOL inited;
    BOOL startComplete;
#if NUM_AMPS > 0
    AmpMgrHandler ampMgrHandler;
#endif /* NUM_AMPS > 0 */

} BsmContext;

/* User request internals */
struct _BsmUserRequest {
    ListEntry node;

    /* Information concerning the request. Not persisted. */
    BsmUserRequestInfo info;

    /* TRUE if the user has supplied a required response already. */
    BOOL responded;
};

/* Local AMP */
struct _BsmLocalAmp {
    ListEntry node;
    U8 enableId;
    U8 disableId;
    BtAmpStatus status;
    BtControllerType type;
    U32 bandwidth;
    ListEntry remoteAmps;
};

/* Remote AMP */
struct _BsmRemoteAmp {
    ListEntry node;
    U8 id;
    BtAmpStatus status;
    BtControllerType type;
    U32 bandwidth;
    BsmRemoteDevice *rdev;
    ListEntry lampNode; /* Used in BsmLocalAmp.remoteAmps */
    BsmLocalAmp *localAmp;
};

/* At present, it is assumed that bsm is defined statically */
extern BsmContext bsm;
#define BSM(s) (bsm.s)

/* Type for iterating with nodes that don't have "node" */
typedef struct _ListEntryEntry {
    ListEntry node;
} ListEntryEntry;

/* Defines for easily entering/leaving BSM locked context */
#define BSM_IN() BSM_Lock(); do
#define BSM_OUT() while(0); BSM_Unlock()

/* Handy macros */
#define bsmIsFlag(item, flag) (((item)->flags & (flag)) == (flag))
#define bsmSetFlag(item, flag) (item)->flags |= (flag)
#define bsmClearFlag(item, flag) (item)->flags &= ~(flag)

/* Prototypes/abbreviations for common persistence manager calls */
BOOL BPM_Write(BsmPersRecordId id, const void *data, U32 dataLen, U32 recordOffset);
#define BPM_Write(id, data, dataLen, recordOffset) \
        BSM(persMgr)->write(BSM(persMgr), id, data, dataLen, recordOffset)

U32 BPM_Read(BsmPersRecordId id, const void *data, U32 dataLen, U32 recordOffset);
#define BPM_Read(id, data, dataLen, recordOffset) \
        BSM(persMgr)->read(BSM(persMgr), id, data, dataLen, recordOffset)

BOOL BPM_SetLength(BsmPersRecordId id, U32 newLength);
#define BPM_SetLength(id, newLength) \
        BSM(persMgr)->setLength(BSM(persMgr), id, newLength)

U32 BPM_GetLength(BsmPersRecordId id);
#define BPM_GetLength(id) BSM(persMgr)->getLength(BSM(persMgr), id)

BOOL BPM_Create(void);
#define BPM_Create() BSM(persMgr)->create(BSM(persMgr))

BOOL BPM_Remove(BsmPersRecordId id);
#define BPM_Remove(id) BSM(persMgr)->remove(BSM(persMgr), id)


/****************************************************************************
 *
 * Prototypes
 *
 ****************************************************************************/

/* Service constructor/destructors and other internal methods */
void bsmServiceDestroy(BsmService *service);
void bsmServiceCreate(BsmService *service, BsmServiceClassUuid type);
U16  bsmServiceGetFriendlyName(BsmService *service, U8 *name, U16 nameSize);


/* Generic internal/utility methods */
#if BT_STACK_LE_ONLY == XA_DISABLED
void bsmUpdateEirData(BsmLocalDevice *ldev);
#endif
void bsmAssignDualModeId(BsmRemoteDevice *rdev);
ListEntry *bsmGetNextFromList(ListEntry *list, ListEntry *cur);
BsmRemoteDevice *bsmRdGetByRemDev(BtRemoteDevice *remDev);
BsmRemoteDevice *bsmRdGetByRemDevNoAddrChg(BtRemoteDevice *remDev);
BsmRemoteDevice *bsmRdGet(const BtDevAddr *addr);

/* Remote service internal methods */
BOOL     bsmRsConnect(BsmRemoteService *rservice, BOOL notify);
void bsmRsDestroy(BsmRemoteService *service);


/* Local service internal methods */
BsmLocalService *bsmLsGetOrCreate(const BsmAgentDescriptor *desc, U16 requiredFlags);
void bsmLsHandleEnabled(BsmLocalService *lservice);
BsmServiceMode bsmLsGetRemoteMode(BsmLocalService *lservice);
void bsmLsHandleDisabled(BsmLocalService *lservice, BtStatus status);
void bsmLsSetMode(BsmLocalService *lservice, BsmServiceMode mode);
void bsmLsDestroy(BsmLocalService *service);
void bsmLsSetEnabled(BsmLocalService *service, BOOL enable, BOOL isShutdown);
void bsmLsUpdateSecurity(BsmLocalService *lservice);
#if NUM_LE_DEVICES > 0
void bsmUpdateLESecurity(const BsmSecuritySettings *settings);
#endif /*NUM_LE_DEVICES*/

/* Prototypes from bsm_discover.c */
void     bsmHandleInquiryResult(const BtInquiryResult *inq);
void     bsmAdvanceDiscovery(BOOL stop);
void bsmDiscoverNext(BsmRemoteDevice *rdev);
void     bsmRdGetName(BsmRemoteDevice *rdev);
void     bsmDiscoveryTimer(EvmTimer *Timer);
BtStatus bsmProcessSdpRsp(BsmRemoteDevice *rdev, SdpQueryToken *token);
void     bsmRdGetServices(BsmRemoteDevice *rdev);
void     bsmRdQueryDevId(BsmRemoteDevice *rdev);
void     bsmRdHandleDevIdQueryRsp(BtRemoteDevice *remDev, DevIdCallbackParms *cbParms);
void     bsmAddL2capAgent(BsmRemoteDevice *rdev);
#if defined(NUM_LE_DEVICES) && NUM_LE_DEVICES > 0
void bsmHandleAdvertReport(CmgrLeScanEvent *e);
void bsmAddGattAgent(BsmRemoteDevice *rdev);
void bsmGattHandler(void *context, BtEventEx *event);
BtStatus bsmProcessGattRsp(BsmRemoteDevice *rdev, GattClientToken *token);
BtStatus bsmAddServiceByUuid(BsmRemoteDevice *rdev, U16 uuid);
void bsmMeleHandler(void *context, BtEventEx *event);
void bsmLeSecHandler(void *context, BtEventEx *event);
void bsmGattClientHandler(void *context, BtEventEx *event);
void bsmGattServerHandler(void *context, BtEventEx *event);
void bsmCmgrLeHandler(void *context, BtEventEx *event);
#endif /*NUM_LE_DEVICES > 0*/

/* General prototypes */
void bsmMeHandler(const BtEvent *evt);
void bsmCmgrHandler(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status);
void bsmPost(BsmEventType eType, BtStatus status, void *data);
BOOL bsmUpdateName(U8 **curName, const U8 *newName, U16 newNameLen);

void bsmRdUpdateName(BsmRemoteDevice *rdev, const U8 *name, U16 len, BOOL commit);
void bsmRdCommit(BsmRemoteDevice *rdev);
void bsmRdUpdateMode(BsmRemoteDevice *rdev, BsmRemoteDeviceMode newMode);
void bsmRdCreateAcl(BsmRemoteDevice *rdev, BtDevAddr *devAddr);
void bsmRdDropAcl(BsmRemoteDevice *rdev, BOOL force);

BsmRemoteService *bsmRdCreateService(BsmRemoteDevice *rdev, BsmLocalService *lservice,
        const U8 *name, U16 nameLen, BsmServiceMode mode, BOOL discovered);
BsmRemoteDevice *bsmRdGetOrCreate(const BtDevAddr *addr, BsmRemoteDeviceMode initMode, BOOL commit);
BsmRemoteService *bsmRdGetOrCreateService(BsmRemoteDevice *rdev,
                BsmLocalService *lserv, const U8 *rname, BsmServiceMode mode, BOOL discovered);

void bsmRsUpdateMode(BsmRemoteService *rservice, BsmServiceMode newMode);
BOOL bsmRsIsValid(BsmRemoteService *rservice);

#if XA_DECODER == XA_ENABLED
void BSM_InitTypes(void);
#endif

/****************************************************************************
 *
 * AMP-related code
 *
 ****************************************************************************/

#if NUM_AMPS > 0
void bsmChannelMgrCallback(void *ctx, L2aChanMgrEvent *evt);
BsmLocalAmp *bsmLaCreate(void);
void bsmRdGetAmps(BsmRemoteDevice *rdev);
void bsmAmpMgrCallback(void *ctx, AmpMgrEvent *evt);
void bsmRaDestroy(BsmRemoteAmp *ramp);
#endif

#endif /* __BSMI_H */

