/****************************************************************************
 * File:
 *     btsock_i.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Internal (private) includes for btsock module
 *
 * Copyright 2009-2017 OpenSynergy GmbH.
 * All rights reserved.  All unpublished rights reserved.
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
 ****************************************************************************/
#ifndef __BTSOCK_I_H_
#define __BTSOCK_I_H_

#include "btsock.h"
#include "sdp.h"
#include "l2cap.h"
#include "l2cap_a.h"
#include "rfcomm.h"
#include "ring.h"
#include "utils.h"
#include "conmgr.h"
#include "oslib.h"
#include "osapi.h"

#if NUM_LE_DEVICES > 0
#include "lel2cap.h"
#include "conmgr.h"
#include "lesec.h"


#if BTSOCK_LEGATT_ENABLED == XA_ENABLED
#include "blespp_client.h"
#include "blespp_server.h"
#endif /* BTSOCK_LEGATT_ENABLED */

#endif /* NUM_LE_DEVICES > 0 */


/* Size of socket handle pool */
#define BTSOCK_MAX_SOCKETS          100
#define BTSOCK_FIRST_ID             1000
#define BTSOCK_DEFAULT_RX_BUFFER   (1024 * 64)
#define BTSOCK_MAX_QUERY_PARMS      32

/*
 * Types
 */

/*
 * Type of socket entry
 */
typedef enum _BtSockEntryType {

    /* A handle is created but has no type yet */
    BTSOCK_ENTRY_UNKNOWN,

    /* It's a service (user did Listen()) */
    BTSOCK_ENTRY_SERVICE,

    /* It's a server pipe (returned from Accept()) */
    BTSOCK_ENTRY_SERVER,

    /* It's a client pipe (user did Connect()) */
    BTSOCK_ENTRY_CLIENT,

} BtSockEntryType;

/*
 * State of a data channel
 */
typedef enum _BtSockChannelState {
    /* Invalid state, no channel should be in it */
    SOCK_STATE_UNKNOWN,

    /* A client channel is waiting for ACL connection */
    SOCK_STATE_ACL_CONNECTING,

    /* A client channel is waiting for service query to complete */
    SOCK_STATE_QUERY,

    /* A client channel is waiting for user to select a service */
    SOCK_STATE_SERVICE_SELECT,

    /* A client or server channel is waiting for service connection */
    SOCK_STATE_CONNECTING,

    /* Channel is up and able to send/receive data */
    SOCK_STATE_CONNECTED,

    /* The user has requested closure but it has not yet completed */
    SOCK_STATE_PEND_CLOSE,

    /* The user has requested closure but data is still on its way */
    SOCK_STATE_PEND_DATA_CLOSE,

    /* Server data channel is being rejected */
    SOCK_STATE_REJECTED,

    /* Socket is closed */
    SOCK_STATE_CLOSED,
} BtSockChannelState;

typedef struct _BtSocket BtSocket;

typedef BtStatus (*BtSockQueryState)(BtSocket *sk, const BtEvent *event);

/*
 * Storage for query data
 */
typedef struct _BtSockQueryToken {
    /* SDP query token */
    SdpQueryToken           token;

    /* SDP query parameters */
    U8                      parms[BTSOCK_MAX_QUERY_PARMS];

    /* SDP query parameter length */
    U8                      plen;

    /* Current query state */
    BtSockQueryState        state;

    /* Array of remote service descriptors */
    BtSockRfServiceDesc     services[BTSOCK_MAX_REMOTE_SERVICES];

    /* Number of valid entries in services */
    U32                     servicesLen;

} BtSockQueryToken;

/*
 * Context for a data-bearing socket
 */
typedef struct _BtDataSocket {

    /* Current channel state */
    BtSockChannelState      state;

    /* SDAP query token (client-only) */
    BtSockQueryToken        query;

    /* BtSocket, points to owner (server-only) */
    BtSocket               *owner;

    /* Rx buffering */
    void                   *rxData;
    RingBuf                 rx;

    /* Rx credit extended */
    U16                     rxCreditOut;

    /* Write packets waiting for connection */
    ListEntry               txWaiting;

    /* Outstanding unacknowledged packets */
    U16                     txOut;

    /* L2CAP context */
    struct {
        /* Registered PSM */
        L2capPsm            psm;

        /* Connection settings */
        L2capConnSettings   settings;

        /* Channel Id */
        L2capChannelId      cid;

        /* Security record (client-only) */
        BtSecurityRecord    secRec;
    } l2cap;

#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
    /* LEL2CAP context */
    struct {
        /* Registered PSM */    
        LeL2capPsm          psm;

        LeL2capFlowParms    parms;

        /* Field is valid when isSlave is FALSE */
        LeL2capPsmValue     remotePsm;

        /* Channel Id */
        L2capChannelId      cid;

        /* Security record (client-only) */
        LeSecurityRecord    secRec;
    } lel2cap;
#endif /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */

#if BTSOCK_LEGATT_ENABLED == XA_ENABLED
    /* LEGATT context */
    struct {
        U32                  legattId; /* dummy ID to identify a LEGATT "channel" with a socket. For now it is always 1. */
    } legatt;
#endif

    /* RFCOMM context */
    struct {
        /* Registered channel */
        RfChannel        ch;

        /* Security record (client-only) */
        BtSecurityRecord secRec;
    } rfcomm;
} BtDataSocket;

/*
 * Context for a service or "listening" socket
 */
typedef struct _BtServiceSocket {

    /* The server data channel waiting for connection */
    BtSocket   *listener;

    /* Server data channels waiting for accept() */
    ListEntry           acceptors;

    /* Server data channels in any connection state */
    ListEntry           connections;

    /* If true, user closed but still pending ops */
    U16                 zombie;

    /* RFCOMM transport fields */
    struct {
        /* Registered (or unregistered?) service */
        RfService           service;
        /* Security record */
        BtSecurityRecord    secRec;
    } rfcomm;

    /* L2CAP transport fields */
    struct {
        /* Registered PSM */
        L2capPsm            psm;
        /* Connection settings */
        L2capConnSettings   settings;
        /* Channel Id */
        L2capChannelId      cid;
        /* Security record */
        BtSecurityRecord    secRec;
    } l2cap;


#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
    /* LEL2CAP transport fields */
    struct {
        /* Registered PSM */
        LeL2capPsm          psm;
        /* Connection settings */
        LeL2capFlowParms    parms;
        /* Channel Id */
        L2capChannelId      cid;
        /* Security record */
        LeSecurityRecord    secRec;
    } lel2cap;
#endif /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */

#if BTSOCK_LEGATT_ENABLED == XA_ENABLED
    /* LEGATT context */
    struct {
        U32                  legattId; /* dummy ID to identify a LEGATT "channel" with a socket. For now it is always 1. */
    } legatt;
#endif


} BtServiceSocket;


/*
 * Socket, including data for whatever type it might be
 */
struct _BtSocket {
    /* Item if the socket needs to be kept on a list */
    ListEntry       node;

    /* Type of socket entry */
    BtSockEntryType type;

    /* Supported Socket transport types */
    BtSockType      suppTpTypes;

    /* Socket identifier */
    U16             socketId;

    /* Active Socket transport */
    BtSockType      activeTpType;

    /* CMGR handle */
    CmgrHandler      cmgr;

    /* Options that can be set on a socket */
    struct {
        /* Desired address */
        BtDevAddr       addr;        

        /* Channel number (if known) */
        U8              channelNum;

        /* PSM value (if known) */
        U16             psmValue;

        /* Service UUID */
        U8              uuid[16];

        /* Length of the service UUID */
        U8              uuidLen;

        /* UTF-8 service name */
        char            serviceName[BTSOCK_MAX_NAME];

        /* Amount of rx data to allow for */
        U32             rxBufferSize;

        /* User-requested security level */
        U8              securityLevel;

        /* If true, user would like to select service */
        BOOL            serviceSelect;

        /* Specified service id or 0 to use socket ptr */
        BtServiceId     serviceId;
    } opt;

    union {
        BtServiceSocket service;
        BtDataSocket data;
    } u;

    /* last known remote modem status */
    RfModemStatus rModemStatus;

    /* last known line status */
    RfLineStatus lineStatus;

    /* Serial port settings. Initially local settings and possibly later on
     * what has been negotiated with the remote.
     */
    RfPortSettings portSettings;
    
};

/*
 * Slot that corresponds socket IDs with BtSocket structures
 */
typedef struct _BtSockSlot {
    ListEntry   node;
    U16         id;
    BtSocket   *socket;
    BOOL        readyToFree;        /* Socket is ready to be freed */
} BtSockSlot;

/*
 * BtSock context structure
 */
typedef struct _BtSockContext {
    /* Slots are used to manage availability of id values */
    BtSockSlot      slots[BTSOCK_MAX_SOCKETS];

    /* Free slots identify unused ids */
    ListEntry       freeSlots;

    /* User-supplied callback if any */
    BtSockCallback  cb;

    /* Records */
    ListEntry       recordList;

    /* Hooks to call */
    BtSockHooks     hooks;

#if BTSOCK_LEGATT_ENABLED == XA_ENABLED
    /* SPP over LE GATT application contexts - server and client */
    /* NOTE: These need to be allocated on a per socket basis to enable multiple simultaneous connections */
    SppsApp         sppsApp;                
    SppcApp         sppcApp;                
    SppsCon          sppsCon;
#endif

} BtSockContext;

/* No socket type */
#define BTSOCK_NONE     0

/* Uncomment to get verbose output for every function called */
/* #define BTSOCK_LOG_FUNCTIONS XA_ENABLED */

#if BTSOCK_LOG_FUNCTIONS == XA_ENABLED
/* Announce each function verbosely */
#define D_ENTER() Xlogv(TAG, "%s {{", __FUNCTION__)
#define D_LEAVE() Xlogv(TAG, "}} %s", __FUNCTION__)
#define DONE(val) do { \
        if (val < 0) { Xloge(TAG, "%s completing with err=%s\n", __FUNCTION__, strerror(-val)); }\
        else { Xlogv(TAG, "%s completing with %d\n", __FUNCTION__, val); }\
        status = val; goto done; } while (0)
#else
#define D_ENTER()
#define D_LEAVE()
#define DONE(val) do { status = val; goto done; } while (0)
#endif /* BTSOCK_LOG_FUNCTIONS == XA_ENABLED */


/*
 * Internal prototypes
 */
BtStatus btsockAddServiceRecord(U8 channel, U16 psm, const U8 *uuid, U8 uuidLen,
                                const char* serviceName);
void btsockRemoveServiceRecord(U8 channel);
BtSockContext *btsockContext(void);
BtStatus btsockClientQuery(BtSocket *sk, BtRemoteDevice *remDev);
void btsockClosedClient(BtSocket *sk);
BtStatus btsockClientConnect(BtSocket *sk, BtRemoteDevice *remDev);
void btsockDeliverEvent(BtSockEventType type, U16 handle, BtStatus status);
void btsockFreeSocket(BtSocket *sk, BOOL force);
BtSocket *btsockFindSocket(U16 id);
void btsockRfcommCb(RfChannel *channel, RfCallbackParms *parms);

#endif /* __BTSOCK_I_H_ */
