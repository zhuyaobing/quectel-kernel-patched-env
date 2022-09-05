/****************************************************************************
 *
 * File:
 *     gd_server.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Defines a fake GATT server.
 *
 * Copyright 2010-2017 OpenSynergy GmbH. All rights reserved. All
 * unpublished rights reserved. Unpublished Confidential Information of
 * OpenSynergy GmbH. Do Not Disclose. No part of this work may be
 * used or reproduced in any form or by any means, or stored in a database
 * or retrieval system, without prior written permission of OpenSynergy
 * GmbH. Use of this work is governed by a license granted by
 * OpenSynergy GmbH.  This work contains confidential and proprietary
 * information of OpenSynergy GmbH. which is protected by copyright,
 * trade secret, trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef _GD_SERVER_H__
#define _GD_SERVER_H__

#include "mele.h"
#include "gattserver.h"
#include "conmgrlea.h"
#include "gattdb.h"
#include "gattsecurity.h"
#include "gd_remote_client.h"

/*---------------------------------------------------------------------------
 * GATT Demo Profile Server layer
 *
 *     Implements the protocol portion of a GATT Demo profile
 *     server application. This is meant to be an example of
 *     how one might implement a GATT based profile using the
 *     GATT API's.
 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

#define GD_NUM_SERVICES     12


/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GD_DB_ID type
 *
 *     The GD_DB_ID type indicates the type of GATT DB to load.
 */
typedef U8 GD_DB_ID;

#define DB_PTS_SMALL        0x01
#define DB_PTS_LARGE1       0x02
#define DB_PTS_LARGE2       0x03
#define DB_PTS_LARGE3       0x04
#define DB_TEST             0x05

/* End of GD_DB_ID */

/*---------------------------------------------------------------------------
 * GdServerEventType type
 *
 *     The GATT Server Application Event types are passed to the application 
 *     handler provided in GDSERVER_Init().
 */
typedef BtEventTypeEx GdServerEventType;

/* An incoming connection from a remote device to the GATT Demo
 * profile server application has occurred. "app->remDev" identifies the
 * remote device.
 */
#define GDSERVER_CONNECT                (BTE_TYPE_BASE_GDSERVER + 0)

/* The incoming connection for a remote device has been disconnected. */
#define GDSERVER_DISCONNECT             (BTE_TYPE_BASE_GDSERVER + 1)

/* To indicate a Primary Service to the application. */
#define GDSERVER_PRIMARY_SERVICE        (BTE_TYPE_BASE_GDSERVER + 2)

/* To indicate an Included Service to the application. */
#define GDSERVER_INCLUDED_SERVICE       (BTE_TYPE_BASE_GDSERVER + 3)

/* To indicate a Characteristic to the application. */
#define GDSERVER_CHARACTERISTIC         (BTE_TYPE_BASE_GDSERVER + 4)

/* To indicate a Characteristic Value to the application. */
#define GDSERVER_VALUE                  (BTE_TYPE_BASE_GDSERVER + 5)

/* The application must use the GDSERVER_Notify() API to complete the
 * notification. The GDSERVER_NOTIFY_COMPLETE event will be indicated
 * when the notification has been transmitted.
 */
#define GDSERVER_SEND_NOTIFY            (BTE_TYPE_BASE_GDSERVER + 6)

/* The notification has been transmitted. */
#define GDSERVER_NOTIFY_COMPLETE        (BTE_TYPE_BASE_GDSERVER + 7)

/* The application must use the GDSERVER_Indicate() API to complete the
 * indication. The GDSERVER_INDICATE_COMPLETE event will be indicated
 * when the indication has been transmitted.
 */
#define GDSERVER_SEND_INDICATION        (BTE_TYPE_BASE_GDSERVER + 8)

/* The indication has been transmitted. */
#define GDSERVER_INDICATE_COMPLETE      (BTE_TYPE_BASE_GDSERVER + 9)

/* An incoming connection from a remote device to the server
 * has occurred and there is no client structure available to 
 * handle it. The application is requested to call 
 * GDSERVER_AddRemoteClient() to add one
 */
#define GDSERVER_NEED_REMOTE_CLIENT     (BTE_TYPE_BASE_GDSERVER + 10)

/* End of GdServerEventType */

/* Forward reference */
typedef struct _GdServerEvent GdServerEvent;

/*---------------------------------------------------------------------------
 * GdServerEventCallback type
 *
 * A function of this type is called to indicate GATT Demo profile
 * server application events.
 */
typedef void (*GdServerEventCallback)(void *context, GdServerEvent *event);

/* End of GdServerEventCallback */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GdServerApp structure
 *
 *     The GdServerApp represents the entire unit of a general purpose GATT 
 *     server application. It allows a GATT database to be built with
 *     services, include services, characteristics, etc. for testing
 *     with PTS and other GATT clients. It also provides methods for
 *     sending indications/notifications. The GdServerApp must be allocated 
 *     and initialized by the application, prior to it being used in any 
 *     other API's. The GdServerApp structure is initialized by calling 
 *     GDSERVER_Init().
 */
typedef struct _GdServerApp {

    /* Pointer to an attribute in the GATT database. Valid for the 
     * GDSERVER_PRIMARY_SERVICE and GDSERVER_ATTRIBUTE events.
     */
    GattService            *curService;

    /* Parameters passed in or out for APIs or Events respectively.
       See the API or Event documentation for specific details.
     */
    U16                     handle;
    U16                     incHandle;
    U16                     endHandle;
    U16                     incUuid;
    GattUuid                uuid;
    AttValueType            valType;
    AttValue                value;

    /* === Internal use only === */

    ListEntry               clientList;
    CmgrLeAdvertUuid        advertUuid[GD_NUM_SERVICES];
    BtHandlerEx             gattHandler;
    GattService            *gattService[GD_NUM_SERVICES];
#if LE_GATTDEMO_SUPPORT_SDP == XA_ENABLED
    GattSdpRecord          sdpRecord[GD_NUM_SERVICES];
#endif /* LE_GATTDEMO_SUPPORT_SDP == XA_ENABLED */
    BtHandlerEx            *appHandler;
} GdServerApp;

/*---------------------------------------------------------------------------
 * GdServerEvent structure
 *
 *     Used to pass GATT Demo profile server application events to
 *     applications.
 */
struct _GdServerEvent {
    /* Inherit from BtEventEx. Note that "e.handler" contains a pointer
     * to the handler which is the CmgrLeAdvertHandler
     */
    BtEventEx               e;

    /* Pointer to application structure */
    GdServerApp            *app;

    /* Pointer to remote client structure */
    GdRemoteClient         *client;
};


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GDSERVER_Init()
 *
 *     Initialize the GATT Demo profile server application.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 *     Handler - Pointer to the handler to receive events.
 *
 *     Dbid - Database ID.
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
BtStatus GDSERVER_Init(GdServerApp *App, BtHandlerEx *Handler, U8 DbId);

/*---------------------------------------------------------------------------
 * GDSERVER_Deinit()
 *
 *     Deinitialize the GATT Demo profile server application.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - app, handler or callback in handler is 0
 *         (XA_ERROR_CHECK only).
 *
 */
BtStatus GDSERVER_Deinit(GdServerApp *App);

/*---------------------------------------------------------------------------
 * GDSERVER_Disconnect()
 *
 *     Disconnects the specified client from GATT Demo profile server 
 *     application. If the GATT Demo server application is the only active
 *     connection, the ACL will be disconnected with this call.
 *
 * Parameters:
 *     Client - Pointer to a structure that represents the remote client.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - app, handler or callback in handler is 0
 *         (XA_ERROR_CHECK only).
 *
 */
BtStatus GDSERVER_Disconnect(GdRemoteClient *Client);

/*---------------------------------------------------------------------------
 * GDSERVER_Discover()
 *
 *     Discover all services, characteristics and values on the local
 *     GATT Demo server. The items will be indicated with
 *     GDSERVER_PRIMARY_SERVICE, GDSERVER_INCLUDED_SERVICE,
 *     GDSERVER_CHARACTERISTIC and GDSERVER_VALUE events as they are
 *     discovered.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - app, handler or callback in handler is 0
 *         (XA_ERROR_CHECK only).
 *
 */
BtStatus GDSERVER_Discover(GdServerApp *App);

/*---------------------------------------------------------------------------
 * GDSERVER_Indicate()
 *
 *     Send an indication for the attribute specified by App->curService and
 *     App->handle.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application with
 *         App->curService and App->handle set to the attribute to indicate.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated.
 *
 *     BT_STATUS_INVALID_PARM - app, handler or callback in handler is 0
 *         (XA_ERROR_CHECK only).
 *
 */
BtStatus GDSERVER_Indicate(GdServerApp *App);

/*---------------------------------------------------------------------------
 * GDSERVER_Notify()
 *
 *     Send an notification for the attribute specified by App->curService and
 *     App->handle.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application with
 *         App->curService and App->handle set to the attribute to notify.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated.
 *
 *     BT_STATUS_INVALID_PARM - app, handler or callback in handler is 0
 *         (XA_ERROR_CHECK only).
 *
 */
BtStatus GDSERVER_Notify(GdServerApp *App);

/*---------------------------------------------------------------------------
 * GDSERVER_AddRemoteClient()
 *
 *     Add a GdRemoteClient structure to the GATT server that is used to
 *     "hold" the link up while GATT information is being indicated to the
 *     remote device so that other local profiles and applications will not
 *     cause the link to disconnect unexpectedly. Also used to disconnect
 *     link when the operations is completed.
 *
 * Parameters:
 *     App    - Pointer to a structure that represents the application
 *
 *     Client - Pointer to the GdRemoteClient structure.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded and the remote client structure
 *                         was added to the list of available clients.
 *
 *     BT_STATUS_FAILED  - request failed.
 *
 *     BT_STATUS_INVALID_PARM - App or Client is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GDSERVER_AddRemoteClient(GdServerApp *App, GdRemoteClient *Client);

/*---------------------------------------------------------------------------
 * GDSERVER_RemoveRemoteClient()
 *
 *     Remove a previously added GdRemoteClient structure. The GdRemoteClient
 *     structure should not be removed while it is involved in a connection.
 *     Doing so will most likely result in the link being disconnected.
 *
 * Parameters:
 *     App    - Pointer to a structure that represents the application.
 *
 *     Client - Pointer to a GdRemoteClient structure.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed because structure has not been
 *         previously added.
 *
 *     BT_STATUS_INVALID_PARM - App or Client is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GDSERVER_RemoveRemoteClient(GdServerApp *App, GdRemoteClient *Client);

#endif /*_GD_SERVER_H__ */
