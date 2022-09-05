/****************************************************************************
 *
 * File:
 *     gatti.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains internal definitions for the Bluetooth
 *     GATT layer.
 *
 * Created:
 *     August 19, 2010
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

#ifndef __GATTI_H
#define __GATTI_H

#include "gattserver.h"
#include "gattclient.h"
#include "gattsecurity.h"
#include "gattdb.h"
#include "gattcache.h"
#include "sys/le_attop_i.h"
#include "sys/le_atthnd_i.h"
#include "sdp.h"
#include "mele.h"
#include "sys/le_sec_i.h"
#include "sys/smpi.h"

/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Space between the lastHandle of one service and the firstHandle of the
 * the next service when the stack allocates the handles and the new
 * service is added at the end.
 */
#define GATT_HANDLE_END_SPACING 10

/*---------------------------------------------------------------------------
 * GattConnectState type
 *
 *     The GATT Connect State identifies the state of the GATT connection
 */
typedef U8 GattConnectState;

#define GC_STATE_DISCONNECTED  0
#define GC_STATE_CONNECTING    1
#define GC_STATE_CONNECTED     2
#define GC_STATE_DISCONNECTING 3 /* This must be the last state */

/* End of GattConnectState */

/*---------------------------------------------------------------------------
 * GattConnectRole type
 *
 *     The GATT Connect Roles identifies the role the client and server play
 *     in the creation of the connection. These are ORed together
 */
typedef U8 GattConnectRole;

#define GCR_CLIENT_OUT    LEATT_CLIENT   /* 0x01 Client called local GATTC_Connect */
#define GCR_SERVER_OUT    LEATT_SERVER   /* 0x02 Server called local GATTS_Connect */
#define GCR_SERVER_IN     0x04           /* Server received incoming ATT request */
#define GCR_ALL           (GCR_CLIENT_OUT|GCR_SERVER_OUT|GCR_SERVER_IN)

/* End of GattConnectRole */

/*---------------------------------------------------------------------------
 * GattSecurityState type
 *
 *     The GATT Security State identifies the state of GATT Security
 */
typedef U8 GattSecurityState;

/* GATT security is in normal operating state */
#define GSEC_STATE_READY      0

/* GATT Security has sent an event to the security handler to request
 * authorization.
 */
#define GSEC_STATE_SEND_AUTH  1

/* Security handler has responded with authorization during the callback */
#define GSEC_STATE_RCV_AUTH   2

/* Security handler did not respond with authorization during callback so
 * GATT security is waiting for response from security handler.
 */
#define GSEC_STATE_WAIT_AUTH  3

/* Security handler has responded with authorization outside of callback
 * and is calling GATT server to respond
 */
#define GSEC_STATE_AUTH_RSP   4

/* end of GattSecurityState */

/* Forward reference of GattCmacToken. */
typedef struct _GattCmacToken GattCmacToken;

/*---------------------------------------------------------------------------
 * Callback used to notify a GATT layer that CMAC function is complete.
 */
typedef void (*GattCmacComplete)(GattCmacToken*);

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * GattContext contains all the data used internally by the GATT Entity.
 */

typedef struct {
    /* List of registered GATT server handlers */
    ListEntry       servHandlers;

    /* List of allocated but not yet active services */
    ListEntry       allocServices;

    /* The GATT server database */
    BOOL            isDbOpened;
    GattDatabase    gattDb;

    /* LeAttHandler structures used to register */
    LeAttHandler    clientHandler;
    LeAttHandler    serverHandler;

    /* Handler for MELE */
    MeLeHandler     leHandler;

    /* The current context for the MELEE_REMOTE_FEATURES event */
    BtLeAttContext *curCtx;

    /* BtHandlerEx GATT client/server security handlers */
    BtHandlerEx    *secHandler;

    /* Number of active connections */
    U16             numCons;

    /* Current GATT MTU size for LE connections */
    U16             gattLeMTU;

    /* SDP records */
    GattSdpRecord   sdpRecord;
    GattSdpRecord   sdpRecordGap;

    /* List of SDP records holding GATT services */
    ListEntry       sdpRecords;

    /* Gatt Client Cache */
    BOOL            isCacheOpened;
    GattCache       clientCache;

    /* TRUE means use SDP for primary service search on BR/EDR links. FALSE means do not
     * use SDP for primary service search but use GATT service search instead.
     */
    BOOL            useSdpForBrEdr;

    /* GAP Service characteristics */
    U16             appearance;
    U8              periphPrefConnParms[8];

    BOOL            isPrivate;
    BOOL            allowDeviceNameWrite;
    BOOL            allowAppearanceWrite;

    /* Time value used for disconnecting the GATT server when the local
     * device initiates the LE link
     */
    TimeT                initiatorTime;

    /* Time value used for disconnecting the GATT server when the remote
     * device initiates the LE link
     */
    TimeT                responderTime;


} GattContext;

/*---------------------------------------------------------------------------
 *
 *     GATT context to be included in an ATT context which represents an
 *     ATT/GATT connection to a remote device.
 */
typedef struct {
    GattConnectState            state;

    /* TRUE indicates that the connection was initiated by remote device */
    BOOL                        conIncoming;

    /* TRUE means that GATT has sent an MTU response */
    BOOL                        sentMtuRsp;

    /* Indicates if client and/or server has called connect */
    GattConnectRole             conRole;

    /* SMP security failure reason code */
    LeSmpPairingFailureReason   smpFailureReason;

    /* SMP received Pairing Confirm value that needs to be verified */
    SmpKeyInformation           rcvdConfirmValue;

    /* SMP received Pairing Randomv value used for verification and STK generation */
    SmpKeyInformation           rcvdRandomValue;

    /* SMP generated Pairing Random value to be issued later */
    SmpKeyInformation           genRandomValue;

    /* TRUE means context is inititalized FALSE otherwise */
    BOOL                        initialized;

    /* State of GATT security */
    GattSecurityState           secState;

    /* List of GATT security tokens */
    ListEntry                   secTokens;

    /* List of GATT authorize tokens */
    ListEntry                   authTokens;

    /* Current token waiting for a response */
    GattSecurityToken          *cur;

    /* BR/EDR security handler */
    BtHandler                   btSecHandler;

    /* Security level used when a security token is cancelled. */
    LeSecSecurityLevel          secLevel;

    /* GATT client token used to read the central devices address resolution characteristic */
    GattClientToken             centralAddrClientToken;

    /* Holds the state for reading the central device address resolution characteristic */
    U8                          centralAddrState;

    /* Holds a reference to the gap service used for reading the central device address resolution
     * characteristic 
     */
     void                      *gapServRef;

     /* Holds the handle to the Central Address Resolution characteristic value */
     U16                        centralAddrValHandle;

     /* Holds the security context needed when finishing pairing. It can
      * be either BtLeSEcContext or BtBrSecContext 
      */
     void                       *secCtx;

} GattConnectContext;

/*---------------------------------------------------------------------------
 *
 *     GATT CMAC Token is used when calling GATT_CMAC().
 */
struct _GattCmacToken {
    /* Must be first item. Used internally by GATT */
    MeLeOpHandler     opHandler;

    /* Context area for use by caller */
    void             *context;

    /* Function to call when CMAC is complete */
    GattCmacComplete  func;

    /**************** Input fields ************************************/
    /* Total amount of data including data pointed to by "data" and
     * data in firstDataBlock
     */
    U16               dataLen;

    /* Points to the data beyond the data in the firstDataBlock */
    U8               *data;

    /* First block of data. dataLen includes the data in this block */
    U8                firstDataBlock[16];

    /* Holds the key and the counter. Both must be in big endian format */
    U8                key[16];
    U8                counter[4];

    /********************** Output Fields *****************************/
    /* Status of the operation */
    BtStatus          status;

    /* The result of the CMAC function */
    U8                mac[8];

    /* === Internal use only === */

     /* Tmp and scratch are used to hold temporary values in the calculation. */
    U8                tmp[16];
    U8                scratch[16];

    /* n1 holds the amount of data in firstDataBlock (not including the counter) which
     * is used in the last bock sent to CMAC. 16 - n1 is the amount of data left which
     * must be combined with octets in data to form a complete block (in the case where
     * the dataLen+4 is greater than 16).
     */
    U8                n1;

    /* The number of rounds to perform in the data */
    U8                nData;

    /* The number of rounds to perform from the tmp buffer */
    U8                nTmp;

    /* Block of data being passed to MELE_Encrypt */
    U8                block[16];

    /* Current round */
    U8                i;

    /* Holds the state */
    U8                state;
};

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/
BtStatus GATT_Init(void);
BtStatus GATT_OpenDb(const BtDevAddr *bdAddr);
void     GATT_CloseDb(void);
void     GATT_FlushDb(void);
void     GATT_ResetDb(void);
BtStatus GATT_OpenClientCache(const BtDevAddr *bdAddr);
void     GATT_CloseClientCache(void);
void     GATT_FlushClientCache(void);
GattClientOwnInstHead *GattFindCoiHead(BtDevAddr *devAddr);
GattClientOwnInstItem *GattFindCoiItem(U16 startHandle, U16 index,
                                       BtDevAddr *devAddr);
GattClientOwnInstItem *GattAddCoiItem(U16 startHandle, U16 index,
                                      BtDevAddr *devAddr,
                                      GattClientOwnInstType type,
                                      BtRemoteDevice *remDev);
void GattFreeCoiHead(GattClientOwnInstHead *coiHead);
GattClientOwnInstHead *GattAllocCoiHead(BtDevAddr *devAddr,
                                        BtRemoteDevice *remDev);
void GattRemoveAllCoiAttribForService(U16 startHandle, U16 index);
BtStatus GattRemoveAllCoiAttribForRemDev(BtDevAddr *devAddr);
BOOL GattIsBonded(BtDevAddr *devAddr);
BOOL GattIsClientReady(BtLeAttContext *ctx);
BtStatus GattConnect(BtRemoteDevice *remDev, LeAttRole role, BtLeAttContext **ctx);
BtStatus GattDisconnect(BtLeAttContext *ctx, LeAttRole role);
void GattSetConServerRoleIn(BtLeAttContext *ctx);
void GattSetConIncoming(BtLeAttContext *ctx, BtRemoteDevice *remDev);
void GattGenSubKeys(U8 *L, U8 *K1, U8 *K2);
void GattAesAddPadding(U8 *in, U8 *out, U8 len);
void GattLeftShift128(const U8 *in, U8 *out);
void GattXor128(const U8 *in1, const U8 *in2, U8 *out);
BtStatus GattGenCMAC(GattCmacToken *token);
BtDevAddr *GattGetIdentityAddr(BtRemoteDevice *remDev);
void GattSecurityInitContext(BtLeAttContext *ctx);
void GattSecurityCallAndRemoveRequests(BtLeAttContext *ctx);
void GattSecurityComplete(BtLeAttContext *attCtx, GattSecurityToken *token,
                          BtStatus status, BtRemoteDevice *remDev);
void GattSecurityCallSecHandler(GattSecurityEvent *gcEvent);
BtStatus GattSecurityFixSecurityError(BtLeAttContext *ctx, GattSecurityToken *token,
                                      AttErrorCode securityError);
void GattSecurityProcessAuthTokens(BtLeAttContext *ctx, BtRemoteDevice *remDev);
BtStatus GattSecurityStartLeEncryption(BtLeAttContext *ctx, BtRemoteDevice *remDev);
void GattHandleSecurityRequest(BtRemoteDevice *remDev);
void GattHandleSecurityComplete(BtRemoteDevice *remDev, BtStatus status);

void GATT_EnableRemoteDeviceNameWrite(BOOL Enable);
void GATT_EnableRemoteAppearanceWrite(BOOL Enable);
void GATT_SetDeviceName(void);
void GATT_SetDeviceNamePermissions(BOOL discoverable);
void GATT_SetAppearance(void);
void GATT_SetCentralAddrResolution(BOOL rpaResolutionSupported);
void GATT_EnablePrivacy(BOOL Enable);
void GATT_SetPeferredConnParms(void);
BOOL GATT_IsPrivacy(void);
BtStatus GattStartReadCentralAddrResolution(void *secCtx, BtRemoteDevice *remDev);

#endif /* __GATTI_H */
