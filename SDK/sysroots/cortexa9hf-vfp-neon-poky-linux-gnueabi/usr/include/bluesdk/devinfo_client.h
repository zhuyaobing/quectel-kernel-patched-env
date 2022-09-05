/****************************************************************************
 *
 * File:
 *     devinfo_client.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions for a device information 
 *     client.
 *
 * Created:
 *     July 12, 2016
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

#include "conmgr.h"
#include "mele.h"
#include "gattclient.h"
#include "conmgrlea.h"
#include "devinfo_service.h"
#include "gattsecurity.h"

#ifndef __DEVINFO_CLIENT_H_
#define __DEVINFO_CLIENT_H_

/*---------------------------------------------------------------------------
 * Device Information Profile Collector layer
 *
 *     Implements a client to detect the device information service on a 
 *     remote device. This is an example of how one might implement an
 *     application that utilizes the profile information service.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * DevInfoClientEventType type
 *
 *     The Device Info Client Event types are
 *     passed to the application handler provided in DEVINFOCLIENT_Init().
 */
typedef BtEventTypeEx DevInfoClientEventType;

/* A connection request has completed. The "e.status" field indicates success
 * or failure of the operation. "app->remDev" identifies the remote device.
 */
#define DEVINFOCLIENT_CONNECT_COMPLETE    (BTE_TYPE_BASE_DEVINFOCLIENT + 0)

/* The connection has been disconnected. */
#define DEVINFOCLIENT_DISCONNECT          (BTE_TYPE_BASE_DEVINFOCLIENT + 1)

/* End of DevInfoClientEventType */

/* Forward reference */
typedef struct _DevInfoClientEvent DevInfoClientEvent;

/*---------------------------------------------------------------------------
 * DevInfoClientEventCallback type
 *
 * A function of this type is called to indicate device information 
 * application events.
 */
typedef void (*DevInfoClientEventCallback)(void *context, DevInfoClientEvent *event);

/* End of DevInfoClientEventCallback */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * DevInfoClientApp structure
 *
 *     The DevInfoClientApp represents the entire unit of what a Device Info
 *     application must contain. The DevInfoClientApp must be allocated
 *     and initialized by the application, prior to it being used in any
 *     other API's. The DevInfoClientApp structure is initialized by calling
 *     DEVINFOCLIENT_Init().
 */
typedef struct _DevInfoClientApp {

    /* Group: Read only fields */

    /* Represents the remote device in which a connection has been made. It
     * is valid during the DEVINFOCLIENT_CONNECT_COMPLETE event
     */
    BtRemoteDevice         *remDev;

    /* Address currently being used by the remote device connected to
     * the GATT server. It valid for all events
     */
    BtDevAddr               devAddr;

    /* Identity address used to index into DDB. This is the "real" address.
     * Valid for all events.
     */
    BtDevAddr               identityAddr;

    /* Contains Device Information characteristic values.
     */
    GattDevInfo             devInfo;

    /* === Internal use only === */

    CmgrHandlerCtx          cmgrHandler;
    GattClientToken         gattToken;
    GattSecurityToken       gattSecToken;
    BtEventTypeEx           eventType;
    BtHandlerEx            *appHandler;
    void                   *deviceInfoSvcRef;
    U16                     systemIdValHandle;
    U16                     modelNumValHandle;
    U16                     serialNumValHandle;
    U16                     firmwareRevValHandle;
    U16                     hardwareRevValHandle;
    U16                     softwareRevValHandle;
    U16                     manufactureNameValHandle;
    BOOL                    gattConnected;
    U16                     state;
    U16                     retryState;
    U8                      notifyCfg[2];
    GattSearchType          stype;

} DevInfoClientApp;


/*---------------------------------------------------------------------------
 * DevInfoClientEvent structure
 *
 *     Used to pass device information client application events to
 *     the applications.
 */
struct _DevInfoClientEvent {
    /* Inherit from BtEventEx. Note that "e.handler" contains a pointer
     * to the handler which is the CmgrLeAdvertHandler
     */
    BtEventEx                   e;

    /* Pointer to application structure. Valid for all events */
    DevInfoClientApp                 *app;
};


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * DEVINFOCLIENT_Init()
 *
 *     Initialize the Device Info Client.
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
BtStatus DEVINFOCLIENT_Init(DevInfoClientApp *app, BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * DEVINFOCLIENT_Deinit()
 *
 *     Deinitialize the device information client application.
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
BtStatus DEVINFOCLIENT_Deinit(DevInfoClientApp *app);

/*---------------------------------------------------------------------------
 * DEVINFOCLIENT_Connect()
 *
 *     Initiate a connection to a remote device. This includes creating an
 *     ACL connection if needed then a GATT connection. As well as finding
 *     the Device Information service and current values.
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
 *         DEVINFOCLIENT_CONNECT_COMPLETE event will be sent to the handler when
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
BtStatus DEVINFOCLIENT_Connect(DevInfoClientApp *app, BtDevAddr *devAddr);

/*---------------------------------------------------------------------------
 * DEVINFOCLIENT_Disconnect()
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
 *         or write attempt is started. A DEVINFOCLIENT_CONNECT_COMPLETE event with
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
BtStatus DEVINFOCLIENT_Disconnect(DevInfoClientApp *app);

#endif /* __DEVINFO_CLIENT_H_ */
