/****************************************************************************
 *
 * File:
 *     le_attop_i.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Internal types, defines, and prototypes for implementing
 *     ATT operations.
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

#ifndef __LE_ATTOP_I_H
#define __LE_ATTOP_I_H

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/* Forward reference to LeAttHandler */
typedef struct _LeAttHandler LeAttHandler;

/* Forward reference to BtLeAttContext */
typedef struct _BtLeAttContext BtLeAttContext;

/* Forward reference to BtBrAttContext */
typedef struct _BtBrAttContext BtBrAttContext;

/* Forward reference to LeAttEvent */
typedef struct _LeAttEvent LeAttEvent;

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

typedef struct _LeAttError {
    U8    errOp;  /* LeAttOpCode */
    U16   handle;
    U8    error;  /* AttErrorCode */
} LeAttError;

typedef struct _LeAttMtuExchange {
    U16             size;
} LeAttMtuExchange;

typedef struct _LeAttFindReq {
    union {
        struct {
            U16     hndlStart;
            U16     hndlEnd;
        } info;

        struct {
            /* For GATT, Find By Type Value Request is only used to find
             * primary services by UUID. So value holds a UUID (2 or 16 octets)
             * Therefore, len and value are defined with that in mind
             */
            U16     hndlStart;
            U16     hndlEnd;
            U16     uuid;
            U8      len;
            U8     *value;

            /* Scratch buff for 16-bit UUID stored in LE format. value will point to it
             * in case where value is a 16-bit UUID.
             */
            U8      val2[2];
        } type;
    } req;
} LeAttFindReq;

typedef struct _LeAttFindRsp {
    union {
        struct {
            U16     len;
            U8      format;
            U8     *info;
        } info;

        struct {
            U16     len;
            U8     *list;
        } type;
    } rsp;
} LeAttFindRsp;

typedef struct _LeAttReadReq {
    union {
        struct {
            U16     hndlStart;
            U16     hndlEnd;
            U8      uuidLen;
            U8      uuid[16];
        } type;

        struct {
            U16     handle;
        } hnd;

        struct {
            U16     handle;
            U16     offset;
        } blob;

        struct {
            U16     numHandles;
            U16    *handles;
        } multi;

        struct {
            U16     hndlStart;
            U16     hndlEnd;
            U8      uuidLen;
            U8      uuid[16];
        } group;
    } req;
} LeAttReadReq;

typedef struct _LeAttReadRsp {
    union {
        struct {
            U8      size;   /* Size of each handle-value pair. */
            U16     len;    /* Number of octets in data */
            U8     *data;   /* List of handle-value pairs. */
        } type;

        struct {
            U16     len;    /* Length of value. */
            U8     *value;  /* Value data. */
        } hnd;

        struct {
            U16     len;    /* Length of value. */
            U8     *value;  /* Value data. */
        } blob;

        struct {
            U16     len;    /* Length of value. */
            U8     *values; /* Set of 2 or more values. */
        } multi;

        struct {
            U8      size;   /* Size of each group set. */
            U16     len;    /* Size of data */
            U8     *data;   /* List of group sets. */
        } group;
    } rsp;
} LeAttReadRsp;

typedef struct _LeAttWriteReq {
    union {
        struct {
            U16     len;
            U16     handle;
            U8     *value;
        } hnd;

        struct {
            U16     len;
            U16     handle;
            U8     *value;
        } cmd;

        struct {
            U16     len;
            U8      sig[12];
            U16     handle;
            U8     *value;
        } signedCmd;

        struct {
            U16     len;
            U16     handle;
            U16     offset;
            U8     *value;
        } prep;

        struct {
            U8      flags;
        } exec;
    } req;
} LeAttWriteReq;

typedef struct _LeAttWriteRsp {
    union {
        struct {
            U16     len;
            U16     handle;
            U16     offset;
            U8     *value;
        } prep;
    } rsp;
} LeAttWriteRsp;

typedef struct _LeAttServer {
    struct {
        U16     len;
        U16     handle;
        U8     *value;
    } init;
} LeAttServer;

typedef struct _LeAttOp {

    /* LeAttOpCode */
    U8            opCode;

    /* TRUE if req is valid */
    BOOL          isReqValid;

    /* Operation data */
    union {
        LeAttError          err;
        LeAttMtuExchange    mtu;
        LeAttFindReq        findReq;
        LeAttFindRsp        findRsp;
        LeAttReadReq        readReq;
        LeAttReadRsp        readRsp;
        LeAttWriteReq       writeReq;
        LeAttWriteRsp       writeRsp;
        LeAttServer         server;
    } op;

    /* === Internal use only === */
    void         *opCtx; 
    LeAttHandler *handler;

} LeAttOp;


BtLeAttContext *LeAttAllocFixedClientInstance(U8 RemDevIndex, U8 iClient, void *Channel);
void LeAttDeallocFixedClientInstance(BtLeAttContext *ctx, BtEventEx *Event);

#endif /* __LE_ATTOP_I_H */
