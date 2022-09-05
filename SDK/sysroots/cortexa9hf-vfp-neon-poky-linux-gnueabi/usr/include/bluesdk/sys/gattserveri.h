/****************************************************************************
 *
 * File:
 *     gattserveri.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
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

#ifndef __GATTSERVERI_H
#define __GATTSERVERI_H

#include "mele.h"
#include "gattserver.h"
#include "sys/gattclienti.h"

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GattServerState type
 *
 *     The GATT Server State identifies the state of GATT Server 
 */
typedef U8 GattServerState;

/* Server is in normal operating state */
#define GS_STATE_READY            0

/* Server in the read value function having sent an event to the server 
 * application indicating need for read value from the server application
 */
#define GS_STATE_SEND_SSA_READ  1

/* Server Application has responded with read value during the callback */
#define GS_STATE_RCV_SSA_READ   2

/* Server Application did not respond with value during callback so the
 * GATT server is waiting for response from server application for the
 * read value.
 */
#define GS_STATE_WAIT_SSA_READ  3

/* Server in the Write value function having sent an event to the server 
 * application indicating that a write has occurred and the server app
 * must provide the response.
 */
#define GS_STATE_SEND_SSA_WRITE 4

/* Server Application has responded with write response during the callback */
#define GS_STATE_RCV_SSA_WRITE  5

/* Server Application did not respond during callback so the
 * GATT server is waiting for response from server application for the
 * write.
 */
#define GS_STATE_WAIT_SSA_WRITE 6

/* end of GattServerState */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GattServerPrepWrite structure
 *
 *     GATT Server Prep Write structure holds a single Prepare Write Request.
 */
typedef struct _GattServerPrepWrite {
    U16              handle;
    U16              offset;
    U16              len;
    U8              *val;

} GattServerPrepWrite;

/*---------------------------------------------------------------------------
 * GattServerContext structure
 *
 *     GATT Server context to be included in an ATT context which represents
 *     an ATT/GATT connection to a remote device. 
 */
typedef struct _GattServerContext {
    GattServerState     state;
    BtPacket            pkt;
    LeAttOp             attOp;

    /* Used by read/find operations to create a transmit buffer. Also used
     * by write operations to store the data to write while waiting for
     * authorization.
     */
    U8                  txBuff[GATT_MAX_MTU];

    /* Buffer for signed write */
    U8                  swBuff[GATT_SERVER_MAX_SIGNED_WRITE];

    BOOL                initialized;

    /* Used for handling GATTSE_READ_VALUE_REQ  event and other events */
    U8                  *buff;       /* Pointer to where app should write value */
    U16                  size;       /* The size that is expected for the value */
    U16                  index;      /* Index of attribute whose value is requested. */
    U8                   opCode;     /* LeAttOpCode opcode of packet causing event */

    /* Values set by the app during a read value request */
    U8                   err;        /* Error code to send in response packet */

    /* Values used for read by type, read multile and write execute */
    GattService         *curServ;
    U16                  j;
    U16                  i;
    U16                  n;
    U16                  sizeStored;
    U16                  end;
    LeAttReadReq         readReq;
    U16                  handles[GATT_MAX_MTU/2];

    /* Flag used to signal a flush on link close */
    BOOL                 flushDb;

    /* List of tokens registered for the connection */
    ListEntry            tokens;

    /* List of notifcation and indications requests */
    ListEntry            requests;

    /* Current token waiting for a response */
    GattServerToken      *curToken;

    /* Packet and flag for sending notifications and indications */
    BtPacket             niPkt;
    LeAttOp              niAttOp;
  
    /* If TRUE an indication packet has been sent and waiting for a response */
    BOOL                 waitRsp;

    /* Timer for disconnecting GATT server */
    EvmTimer             timer;

    /* Timer for delaying disconnecting GATT server */
    EvmTimer             delay;

    /* Number of "active" items in prepared write table */
    U16                  pwNum;

    /* Offset into pwBuff for the next prepared write value */
    U16                  pwOffset;

    /* Table of prepared write structures */
    GattServerPrepWrite  pwTable[GATT_SERVER_NUM_PREPARED_WRITE];

    /* Buffer for prepared write values */
    U8                   pwBuff[GATT_SERVER_PREP_WRITE_BUFF_SIZE];

    /* Token for doing a signed write */
    GattCmacToken        cmacToken;

    /* Pointer to RemDev used in signed write */
    BtRemoteDevice      *remDev;

    /* Received sign on a signed write */
    U8                   rcvSign[8];

    /* Service Change indication has been sent */
    BOOL                 servChgSent;

    /* Finished checking for Change Indication */
    BOOL                 servChgComplete;

    /* Buffer to hold service change value for sending indication */
    U8                   servChgBuff[4];
} GattServerContext;

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/

void GattServerCallHandlers(GattServerEvent *gsEvent);
void GattServerEventRemDev(GattServerEvent *gsEvent, BtRemoteDevice *remDev);
void GattServerAttHandler(BtLeAttContext *ctx, LeAttEvent *Event);
void GattServerBtSecHandler(const BtEvent *Event);
ListEntry *GattServerFindHandleGap(U16 numHandles, U16 *startHandle);
ListEntry *GattServerFindHandleLocation(U16 startHandle, U16 numHandles);
U16 GattServerAddStaticItem(U8 *ptr, U16 len);
U16 GattServerAddDynamicItem(U8 *ptr, U16 max, U16 len);
void GattServerInitContext(BtLeAttContext *ctx);
BtStatus GattServerHandlePrv(BtLeAttContext *ctx, BtRemoteDevice *remDev);
BtStatus GattServerHandlePwr(BtLeAttContext *ctx, BtRemoteDevice *remDev);
BtStatus GattServerHandleAuth(BtLeAttContext *ctx, BtRemoteDevice *remDev);
BtStatus GattServerCheckToken(GattServerToken *token);
BtStatus GattServerAddTokenStartRequest(GattServerToken *token);
void GattServerStartDelay(BtLeAttContext *ctx, U32 delay);
BtStatus GattServerAllocService(U16 startHandle, U16 numHandles, BOOL primary, 
                                GattUuid *uuid, GattServiceTrans transport, 
                                GattService **service);
BtStatus GattServerUpdateServiceChange(U16 startHandle, U16 endHandle);
BtStatus GattServerAddServiceToSdp(GattService *service, GattSdpRecord *sdpRecord,
                             BtClassOfDevice cod, U16 serviceNameLen, const U8 *serviceName);
BtStatus GattServerRemoveServiceFromSdp(GattSdpRecord *sdpRecord);
BOOL GattServerCheckPermissionsForRead(BtLeAttContext *ctx, AttAttribute *attr, 
                                       BtRemoteDevice *remDev, AttErrorCode *err,
                                       BOOL ignoreRead, BOOL skipSendEvent);

#endif /* __GATTSERVERI_H */
