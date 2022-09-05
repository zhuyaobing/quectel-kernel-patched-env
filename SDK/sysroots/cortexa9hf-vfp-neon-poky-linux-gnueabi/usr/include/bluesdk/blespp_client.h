/****************************************************************************
 *
 * File:
 *     blespp_client.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions for the Client role of the LE
 *     Data Exchange Service as serial communication.
 *
 * Copyright 2013-2017 OpenSynergy GmbH.
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
#ifndef __BLESPP_CLIENT_H
#define __BLESPP_CLIENT_H

#if BTSOCK_LEGATT_ENABLED == XA_ENABLED

#include "conmgr.h"
#include "mele.h"
#include "gattclient.h"
#include "conmgrlea.h"
#include "gattsecurity.h"


/*---------------------------------------------------------------------------
 * SPP Profile Client layer
 *
 *     Implements the protocol portion of a Serial data profile client
 *     application. This layer is meant to be an example of how one might
 *     implement serial data communication profile. This code may need
 *     modified by the end user but can act as a base for how it could be
 *     implemented.
 */
#define SPPC_RX_DATA_MAX                  512
#define SPPC_TX_DATA_MAX                  512

 /* Send data mode */
#define SPPC_TX_MODE_INDICATION           1
#define SPPC_TX_MODE_NOTIFICATION         2

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/
typedef struct {
    U16 len;
    U8  data[SPPC_RX_DATA_MAX];
} SppcRxData;

/* A connection request has completed. The "e.status" field indicates success
 * or failure of the operation. "app->remDev" identifies the remote device.
 */
#define SPPCE_CONNECT_COMPLETE    (BTE_TYPE_BASE_SPPC + 0)

/* The connection has been disconnected. */
#define SPPCE_DISCONNECT          (BTE_TYPE_BASE_SPPC + 1)

/* The incoming data from remote device has arrived
 */
#define SPPCE_DATA_IND           (BTE_TYPE_BASE_SPPC + 2)

/* The sending data without response is complete.  */
#define SPPCE_DATA_TX_NORESP_COMPLETE     (BTE_TYPE_BASE_SPPC + 3)

/* The sending data characteristic is complete.  */
#define SPPCE_DATA_TX_COMPLETE   (BTE_TYPE_BASE_SPPC + 4)

/* Forward reference */
typedef struct _SppcEvent SppcEvent;

/*---------------------------------------------------------------------------
 * SppcEventCallback type
 *
 * A function of this type is called to indicate Data Exchange profile
 * client application events.
 */
typedef void(*SppcEventCallback)(void *context, SppcEvent *event);

/* End of SppcEventCallback */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SppcApp structure
 *
 *     The SppcApp represents the entire unit of what data exchange
 *     client application must contain. The SppcApp must be allocated
 *     and initialized by the application, prior to it being used in any
 *     other API's. The SppcApp structure is initialized by calling
 *     SppClient_Init().
 */
typedef struct _SppcApp {

    /* Group: Read only fields */

    /* Represents the remote device in which a connection has been made. It
     * is valid during the SPPCE_CONNECT_COMPLETE event
     */
    BtRemoteDevice         *remDev;

    /* Address currently being used by the remote device connected to
     * the GATT server. It is valid for all events
     */
    BtDevAddr               devAddr;

    /* Identity address used to index into DDB. This is the "real" address.
     * Valid for all events.
     */
    BtDevAddr               identityAddr;

    /* The current incoming data from the remote device. This only valid
     * during the SPPCE_DATA_IND event
     */
    SppcRxData             rxdata;

    /* Contains the current Characteristic Client Configuration value for the
     * Data Exchange Server TX. This indicates of notificaons are enabled or not.
     */
    GattClientCharConfig    txCcc;

    /* === Internal use only === */

    CmgrHandlerCtx          cmgrHandler;
    GattClientToken         gattToken;
    GattSecurityToken       gattSecToken;
    BtEventTypeEx           eventType;
    BtHandlerEx            *appHandler;
    void                   *SppSvcRef;
    void                   *SppRxChrRef;
    void                   *SppTxChrRef;
    U16                     SppRxValHandle;
    U16                     SppTxValHandle;
    U16                     SppTxConfig;
    U16                     state;
    U16                     retryState;
    BOOL                    gattConnected;
    BOOL                    SppTxConfigFound;
} SppcApp;

/*---------------------------------------------------------------------------
 * SppcEvent structure
 *
 *     Used to pass Data Exchange profile server application events to
 *     applications.
 */
struct _SppcEvent {
    /* Inherit from BtEventEx. Note that "e.handler" contains a pointer
     * to the handler which is the CmgrLeAdvertHandler
     */
    BtEventEx              e;

    /* Pointer to application structure. Valid for all events */
    SppcApp          *app;

    /* Events SPPCE_DATA_TX_COMPLETE and SPPCE_DATA_TX_NORESP_COMPLETE:
     * pointer to a data buffer that must be freed by the application.
     */
    U8               *pkt;

};

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SppClient_Init()
 *
 *     Initialize the Data Exchange Client application.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     appHandler - Pointer to the handler to receive events.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed.
 *
 *     BT_STATUS_INVALID_PARM - app, handler or callback in handler is 0
 *         (XA_ERROR_CHECK only).
 *
 */
BtStatus SppClient_Init(SppcApp *app, BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * SppClient_Deinit()
 *
 *     Deinitialize the Data Exchange Client application.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - app, handler or callback in handler is 0
 *         (XA_ERROR_CHECK only).
 *
 */
BtStatus SppClient_Deinit(SppcApp *app);

/*---------------------------------------------------------------------------
 * SppClient_Connect()
 *
 *     Initiate a connection to a remote device. This includes creating an
 *     ACL connection if needed then a GATT connection. As well as finding
 *     the Heart Rate and Device Information services and current values.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     devAddr - Pointer to address of remote device.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated. The
 *         SPPCE_CONNECT_COMPLETE event will be sent to the handler when
 *         the connection attempt is completed.
 *
 *     BT_STATUS_BUSY - operation failed because the application is already
 *         busy connecting or is connected.
 *
 *     BT_STATUS_FAILED - operation failed
 *
 *     BT_STATUS_INVALID_PARM - app, devAddr is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus SppClient_Connect(SppcApp *app, BtDevAddr *devAddr);

/*---------------------------------------------------------------------------
 * SppClient_Disconnect()
 *
 *     Cancel a connection attempt or Disconnect an existing connection to a
 *     remote device.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request to cancel an existing connection attempt
 *         or write attempt is started. A SPPCE_CONNECT_COMPLETE event with
 *         "e.status" set to BT_STATUS_CANCELLED will be sent to the handler
 *         if a connection attempt was canceled.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending disconnect is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus SppClient_Disconnect(SppcApp *app);

/*---------------------------------------------------------------------------
 * SppClient_SendData()
 *
 *     Send data to the server.
 *
 * Parameters:
 *
 *
 *     app - Pointer to a structure that represents the application.
 *
 *     mode - transmission mode: indication or notification
 *
 *     txData - Pointer to the data to be sent
 *
 *     txDataLen - Length of the data to be sent
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed because structure has not been
 *                        previously added.
 *
 *     BT_STATUS_INVALID_PARM - app or con is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus SppClient_SendData(SppcApp *app, U8 mode,
                            U8 *txData, U16 txDataLen);


#endif /* BTSOCK_LEGATT_ENABLED == XA_ENABLED */

#endif /*__BLESPP_CLIENT_H */
