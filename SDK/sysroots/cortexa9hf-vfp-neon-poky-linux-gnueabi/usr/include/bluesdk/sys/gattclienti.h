/****************************************************************************
 *
 * File:
 *     gattclienti.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Internal types, defines, and prototypes for implementing the
 *     GATT client protocol layer.
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

#ifndef __GATTCLIENTI_H
#define __GATTCLIENTI_H

#include "mele.h"
#include "gattclient.h"
#include "sys/gatti.h"
#include "sys/le_attop_i.h"
#include "sys/le_sec_i.h"
#include "gattcache.h"

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

/*---------------------------------------------------------------------------
 * GattClientContext structure
 *
 *     GATT Client context to be included in an ATT context which represents
 *     an ATT/GATT connection to a remote device.
 */
typedef struct _GattClientContext {
    /* List of tokens associated with the ATT/GATT connection (context) */
    ListEntry              tokens;

    /* TRUE means context is intitialized */
    BOOL                   initialized;

    /* List containing tokens making a client request */
    ListEntry              requests;

    /* Current token waiting for a response */
    GattClientToken       *cur;

    /* Expected ATT response for the current operation. This is an
     * LeAttOpCode as defined in le_att.h
     */
    U8                     rspOpc;

    /* Packet used to send client requests */
    BtPacket               clientPkt;

    /* Used for client ATT operations */
    LeAttOp                attOp;

    /* Packet used to send client confirmation responses */
    BtPacket               clientCnfPkt;

    /* Used for client ATT confirmation operations */
    LeAttOp                attCnfOp;

    /* If TRUE a packet has been sent and waiting for a response */
    BOOL                   waitRsp;

    /* If TRUE an indication confirm packet has been sent and waiting for a
     * completion.
     */
    BOOL                   waitPkt;

    /* Cache record for current connect device */
    GattCacheDeviceRecord *record;

    /* State variable for searching the remote GATT service */
    U8                     searchGattServiceState;

    /* Variable used to detect when MTU exchange is occurring */
    BOOL                   exchangeGattMtu;

    /* Variable used to hold the GATT service's endHandle when searching
     * for characteristic descriptors
     */
    U16                    gattServEndHandle;

    /* Buffer used for sending data for GATT Services */
    U8                     gattServBuff[2];

    /* Timer for delaying openning GATT Client connection to allow
     * remote service to send an indication for the Gatt Service
     * service change characteristic
     */
    EvmTimer               delay;

    /* BR/EDR security handler */
    BtHandler              btSecHandler;

    /* Token for doing a signed write */
    GattCmacToken          cmacToken;

    /* Current token to process security */
    GattClientToken       *curSecToken;

    /* Security state */
    U8                     securityState;

    U8                     clientPktCnt;

    /* Info for SDP query */
    struct {
        GattClientToken *curToken;
        SdpQueryToken    token;
        U8               state;
        U8               i;
        U8               buff[30];
        void            *service;
        U8               query[18];       /* Used for specific UUID queries */
    } sdp;

} GattClientContext;

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/
void GattClientAttHandler(BtLeAttContext *ctx, LeAttEvent *Event);
void GattClientBtSecHandler(const BtEvent *Event);
void GattClientInitToken(GattClientToken *token, BtRemoteDevice *remDev);
BtStatus GattClientSendMtuRequest(BtLeAttContext *ctx, BtRemoteDevice *remDev);
BtStatus GattClientSetupGpsResult(GattClientToken *token);
BtStatus GattClientSendGpsRequest(GattClientToken *token);
BtStatus GattClientAddTokenStartRequest(GattClientToken *token);
void GattClientSendEventToToken(GattClientToken *token, BtStatus status, BOOL keepToken);
void GattClientSecurityComplete(BtLeAttContext *attCtx, GattClientToken *token,
                                BtStatus status, BtRemoteDevice *remDev);
void GattClientStartNext(BtLeAttContext *ctx);
BtStatus GattClientCheckToken(GattClientToken *token);
BtStatus GattClientSendGpsbuRequest(GattClientToken *token);
BtStatus GattClientSetupGpsbuResult(GattClientToken *token);
BtStatus GattClientSetupGcvbuResult(GattClientToken *token);
BtStatus GattClientSendRbtRequest(GattClientToken *token);
BtStatus GattClientSetupGcvResult(GattClientToken *token);
BtStatus GattClientSendRbhRequest(GattClientToken *token);
BtStatus GattClientSendBlobRequest(GattClientToken *token);
BtStatus GattClientSendRbmhRequest(GattClientToken *token);
BtStatus GattClientSendGisUuid128Request (GattClientToken *token);
BtStatus GattClientSendWriteRequest(GattClientToken *token);
BtStatus GattClientSetupWriteResult(GattClientToken *token);
BtStatus GattClientSendWriteCommand(GattClientToken *token);
BtStatus GattClientSendSignedWriteCommand(GattClientToken *token);
BtStatus GattClientSendPrepareWriteRequest(GattClientToken *token);
BtStatus GattClientSetupPrepareWriteResult(GattClientToken *token);
BtStatus GattClientSendExecuteWriteRequest(GattClientToken *token);
BtStatus GattClientSetupPrepareWriteResultReliable(GattClientToken *token);
BtStatus GattClientSendPrepareWriteRequestReliable(GattClientToken *token);
BtStatus GattClientSetupExecuteWriteResult(GattClientToken *token);
BtStatus GattClientSetupGisResult(GattClientToken *token);
BtStatus GattClientSetupGcResult(GattClientToken *token);
BtStatus GattClientSetupGcbuResult(GattClientToken *token);
BtStatus GattClientSendGcdRequest(GattClientToken *token);
BtStatus GattClientSetupGcdResult(GattClientToken *token);
BtStatus GattClientSendIndicationCnf(BtLeAttContext *ctx);
BtStatus GattClientSendSecurityRequest(GattClientToken *token);
BtStatus GattClientSetupSecurityResult(GattClientToken *token);
BtStatus GattClientInitSecurity(BtLeAttContext *ctx, GattClientToken *token);
void GattClientInitContext(BtLeAttContext *ctx);
void GattClientRemoveCacheTokens(BtLeAttContext *ctx);
BtStatus GattClientGetCachedPrimaryService(GattClientToken *token, GattSearchType stype);
BtStatus GattClientCheckFindPrimaryService(GattClientToken *token);
BtStatus GattClientStartPrimaryServiceSearch(GattClientToken *token, GattClientEventType type);
BtStatus GattClientGetCachedPrimaryServiceUuid(GattClientToken *token, GattSearchType stype);
BtStatus GattClientStartPrimaryServiceSearchSdp(GattClientToken *token, GattClientEventType type);

GattCacheDeviceRecord *GattCCacheFindRecord(BtDevAddr *devAddr);
GattCacheDeviceRecord *GattCCacheAllocRecord(BtDevAddr *devAddr);
GattCacheAttribute *GattCCacheAllocAttribute(void);
GattCacheService *GattCCacheFindService(GattCacheDeviceRecord *record, U16 handle);
GattCacheService *GattCCacheFindServiceByUuid(GattCacheDeviceRecord *record, GattUuid *uuid);
GattCacheCharacteristic *GattCCacheAllocCharacter(void);
void GattCCacheUpdateCacheWithServChg(GattCacheDeviceRecord *record,
                                      U16 start, U16 end);
BtStatus GattClientSendGcpsRequest(GattClientToken *token);
BtStatus GattClientHandleGcpsError(GattClientToken *token);
BtStatus GattClientSetupGcpsResult(GattClientToken *token);
BtStatus GattClientSendGcisRequest(GattClientToken *token);
BtStatus GattClientHandleGcisError(GattClientToken *token);
BOOL GattClientCleanCacheRecordNoBond(GattCacheDeviceRecord *record);
void GattClientCleanCache(void);
void GattClientCleanCacheRecord(GattCacheDeviceRecord *record,
                                BOOL clearGattService);
GattCacheService *GattCCacheAllocService(BOOL isPrimary);
BtStatus GattClientSetupGcisResult(GattClientToken *token);
void GattClientCleanIncludesFromService(GattCacheDeviceRecord *record,
                                        GattCacheService *service);
BtStatus GattClientSendGccRequest(GattClientToken *token);
BtStatus GattClientSetupGccResult(GattClientToken *token);
BtStatus GattClientHandleGccError(GattClientToken *token);
void GattClientCleanCharsFromService(GattCacheService *service);
BtStatus GattClientSendGccdRequest(GattClientToken *token);
BtStatus GattClientSetupGccdResult(GattClientToken *token);
BtStatus GattClientHandleGccdError(GattClientToken *token);
void GattClientCleanDescsFromCharacter(GattCacheCharacteristic *character);
BOOL GattClientCmpUuid(GattUuid *uuid1, GattUuid *uuid2);
BtStatus GattClientStartGattServiceForBrEdr(BtLeAttContext *ctx, U8 nextState);
void GattClientGattServResult(BrEdrRemoteDevice *remDev, BtStatus status);
void GattClientFinishOpenInit(BtLeAttContext *ctx, BtRemoteDevice *remDev);
void GattClientCallHandlers(BtLeAttContext *ctx, GattClientEvent *gcEvent);

#endif /* __GATTCLIENTI_H */
