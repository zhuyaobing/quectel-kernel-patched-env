/****************************************************************************
 *
 * File:
 *     an_client.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions for the Client role of the LE
 *     Alert Notification Profile.
 *
 * Created:
 *     January 9,2013
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
#include "an_services.h"
#include "devinfo_service.h"
#include "gattsecurity.h"

#ifndef __AN_CLIENT_H_
#define __AN_CLIENT_H_

/*---------------------------------------------------------------------------
 * Alert Notification profile Client layer
 *
 *     Implements the protocol portion of an Alert Notification profile
 *     Client application. This layer is meant to be an example of 
 *     how one might implement the Alert Notification profile. This
 *     code may need modified by the end user but can act as a base
 *     for how it could be implemented.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AnClientEventType type
 *
 *     The Alert Notification profile Client Application Event types are
 *     passed to the application handler provided in ANCLIENT_Init().
 */
typedef BtEventTypeEx AnClientEventType;

/* A connection request has completed. The "e.status" field indicates success
 * or failure of the operation. "app->remDev" identifies the remote device.
 */
#define ANCLIENT_CONNECT_COMPLETE       (BTE_TYPE_BASE_ANCLIENT + 0)

/* An incoming connect request has been received and the remote device supports
 * the Alert Notification service. This event is only sent to Listeners
 * (AnClientListener).  
 */
#define ANCLIENT_CONNECT_IND            (BTE_TYPE_BASE_ANCLIENT + 1)

/* The connection has been disconnected. */
#define ANCLIENT_DISCONNECT             (BTE_TYPE_BASE_ANCLIENT + 2)

/* Writing to the Alert Notification control point is complete */
#define ANCLIENT_WRITE_CP_COMPLETE      (BTE_TYPE_BASE_ANCLIENT + 3)

/* Enabling/disabling the Notifications for the New Alert is complete */
#define ANCLIENT_NEW_ALERT_COMPLETE     (BTE_TYPE_BASE_ANCLIENT + 4)

/* Enabling/disabling the Notifications for the Unread Alert status is complete */
#define ANCLIENT_UNREAD_ALERT_COMPLETE  (BTE_TYPE_BASE_ANCLIENT + 5)

/* New Alert notification has been received */
#define ANCLIENT_NEW_ALERT_NOTIFY       (BTE_TYPE_BASE_ANCLIENT + 6)

/* Unread Alert notification has been received */
#define ANCLIENT_UNREAD_ALERT_NOTIFY    (BTE_TYPE_BASE_ANCLIENT + 7)

/* End of AnClientEventType */

/* Forward reference */
typedef struct _AnClientEvent AnClientEvent;

/*---------------------------------------------------------------------------
 * AnClientEventCallback type
 *
 * A function of this type is called to indicate Alert Notification profile
 * Client application events.
 */
typedef void (*AnClientEventCallback)(void *context, AnClientEvent *event);

/* End of AnClientEventCallback */

/*---------------------------------------------------------------------------
 * AnClientServiceFlags type
 *
 * The AnClientServiceFlags type indicates which Heart Rate services
 * are available on the remote device along with any important
 * characteristics. The flags are ORed together.
 */
typedef U8 AnClientServiceFlags;

/* The Supported New Alert Category field on the remote device */
#define ANCLIENTSF_SUPPORT_NEW_ALERT_CAT     0x01

/* The New Alert characteristic CCC has been successfully read from the remote device */
#define ANCLIENTSF_NEW_ALERT                 0x02

/* The Supported Unread Alert Category field on the remote device */
#define ANCLIENTSF_SUPPORT_UNREAD_ALERT_CAT  0x04

/* The Unread Alert status characteristic CCC has been successfully read from the remote device */
#define ANCLIENTSF_UNREAD_ALERT_STATUS       0x08

/* The Alert Notification control point characteristic is present on the remote device */
#define ANCLIENTSF_CONTROL_PT                0x10

/* All the services */
#define ANCLIENTSF_ALL_SERVICES    (ANCLIENTSF_SUPPORT_NEW_ALERT_CAT | ANCLIENTSF_NEW_ALERT |\
                                    ANCLIENTSF_SUPPORT_UNREAD_ALERT_CAT | ANCLIENTSF_UNREAD_ALERT_STATUS |\
                                    ANCLIENTSF_CONTROL_PT)

/* End of AnClientServiceFlags */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AnClientApp structure
 *
 *     The AnClientApp represents the entire unit of what an Alert Notification
 *     Client application must contain. The AnClientApp must be allocated
 *     and initialized by the application, prior to it being used in any
 *     other API's. The AnClientApp structure is initialized by calling
 *     ANCLIENT_Init().
 */
typedef struct _AnClientApp {

    /* Group: Read only fields */

    /* Represents the remote device in which a connection has been made. It
     * is valid during the ANCLIENT_CONNECT_COMPLETE event
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

    /* Indicates which services and values are present on the remote device.
     * Valid for all events.
     */
    AnClientServiceFlags     flags;

    /* Contains the current Characteristic Client Configuration value for the New Alert.
     * This indicates if notifications are enabled or not.
     */
    GattClientCharConfig     newAlertCcc;

    /* Contains the current Characteristic Client Configuration value for the Unread Alert.
     * status. This indicates if notifications are enabled or not.
     */
    GattClientCharConfig     unreadAlertCcc;

    /* Contains the current value for the supported New Alert Categories first octet */
    GattAlertCategoryIdBm1  supNewAlertCat1;

    /* Contains the current value for the supported New Alert Categories second octet */
    GattAlertCategoryIdBm2  supNewAlertCat2;

    /* Contains the current value for the supported Unread Alert Categories first octet */
    GattAlertCategoryIdBm1  supUnreadAlertCat1;

    /* Contains the current value for the supported Unread Alert Categories second octet */
    GattAlertCategoryIdBm2  supUnreadAlertCat2;

    /* === Internal use only === */

    CmgrHandlerCtx          cmgrHandler;
    GattClientToken         gattToken;
    GattSecurityToken       gattSecToken;
    BtEventTypeEx           eventType;
    BtHandlerEx            *appHandler;
    EvmTimer                timer;
    void                   *alertNotifySvcRef;
    void                   *newAlertChrRef;
    void                   *unreadAlertStatusChrRef;
    void                   *alertNotifyControlPointChrRef;
    U16                     newAlertConfig;
    U16                     unreadAlertStatusConfig;
    U16                     supportNewAlertCatValHandle;
    U16                     supportUnreadAlertCatValHandle;
    U16                     newAlertValHandle;
    U16                     unreadAlertValHandle;
    U16                     alertNotifyControlPointHandle;
    BOOL                    gattConnected;
    BOOL                    asListener;
    U16                     state;
    U16                     retryState;
    U8                      notifyCfg[2];
    U8                      controlPoint[2];
    GattSearchType          stype;

} AnClientApp;


/*---------------------------------------------------------------------------
 * AnClientListener structure
 *
 *     The AnClientLister is used to handle the case where the client
 *     is the responder to a connection instead of the initiator. The 
 *     AnClientLlister structure is initialized by calling
 *     ANCLIENT_ListenerInit().
 */
typedef struct _AnClientListener {

    /* Group: Read only fields */

    /* Represents the remote device in which a connection has been made. It
     * is valid during the ANCLIENT_CONNECT_COMPLETE event
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

    /* === Internal use only === */

    CmgrHandlerCtx          cmgrHandler;
    GattClientToken         gattToken;
    GattSecurityToken       gattSecToken;
    BtEventTypeEx           eventType;
    BtHandlerEx            *appHandler;
    void                   *alertNotifySvcRef;
    void                   *newAlertChrRef;
    void                   *unreadAlertStatusChrRef;
    void                   *alertNotifyControlPointChrRef;
    U16                     newAlertConfig;
    U16                     unreadAlertStatusConfig;
    U16                     supportNewAlertCatValHandle;
    U16                     supportUnreadAlertCatValHandle;
    U16                     alertNotifyControlPointHandle;
    BOOL                    gattConnected;
    U16                     state;
    U16                     retryState;
    U8                      notifyCfg[2];
    U8                      controlPoint[2];

} AnClientListener;

/*---------------------------------------------------------------------------
 * AnClientEvent structure
 *
 *     Used to pass Alert Notification profile Client application events to
 *     the applications.
 */
struct _AnClientEvent {
    /* Inherit from BtEventEx. Note that "e.handler" contains a pointer
     * to the handler which is the CmgrLeAdvertHandler
     */
    BtEventEx                   e;

    /* Pointer to application structure. Valid for all events */
    AnClientApp                 *app;

    /* The following are used for the ANCLIENT_CONNECT_IND event */
    BtDevAddr devAddr;

    /* The following are valid for notifications ANCLIENT_NEW_ALERT_NOTIFY and
     * ANCLIENT_UNREAD_ALERT_NOTIFY 
     */
    GattAlertCategoryId id;
    U8                  count;
    U8                  infoLen;
    U8                  info[19];
};


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * ANCLIENT_Init()
 *
 *     Initialize the Alert Notification profile Client application.
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
BtStatus ANCLIENT_Init(AnClientApp *app, BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * ANCLIENT_Deinit()
 *
 *     Deinitialize the Alert Notification profile Client application.
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
BtStatus ANCLIENT_Deinit(AnClientApp *app);

/*---------------------------------------------------------------------------
 * ANCLIENT_ListenerInit()
 *
 *     Initialize the Alert Notification profile Client listener.
 *
 * Parameters:
 *     lis - Pointer to a structure that represents the listener.
 *
 *     handler - Pointer to the handler to receive events.
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
BtStatus ANCLIENT_ListenerInit(AnClientListener *lis, BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * ANCLIENT_ListenerDeinit()
 *
 *     Deinitialize the Alert Notification profile Client listener.
 *
 * Parameters:
 *     lis - Pointer to a structure that represents the listener.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - app, handler or callback in handler is 0
 *         (XA_ERROR_CHECK only).
 *
 */
BtStatus ANCLIENT_ListenerDeinit(AnClientListener *lis);

/*---------------------------------------------------------------------------
 * ANCLIENT_Connect()
 *
 *     Initiate a connection to a remote device. This includes creating an
 *     ACL connection if needed then a GATT connection. As well as finding
 *     the Alert Notification service and current values.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     devAddr - Pointer to address of remote device.
 *
 *     asListener - TRUE if connecting as a listener.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated. The
 *         ANCLIENT_CONNECT_COMPLETE event will be sent to the handler when
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
BtStatus ANCLIENT_Connect(AnClientApp *app, BtDevAddr *devAddr, BOOL asListener);

/*---------------------------------------------------------------------------
 * ANCLIENT_Disconnect()
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
 *         or write attempt is started. A ANCLIENT_CONNECT_COMPLETE event with
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
BtStatus ANCLIENT_Disconnect(AnClientApp *app);

/*---------------------------------------------------------------------------
 * ANCLIENT_EnableNewAlertNotify(()
 *
 *     Enable/Disable the sending of New Alert notifications from the remote
 *     device. This function writes the Characteristic Client Configuration.
 *     and will enable/disable all notifications. Individual categories are
 *     enabled or disabled using ANCLIENT_WriteControlPoint().
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 *     Enable - If TRUE, the New Alert characteristic notification is enabled.
 *              If FALSE, the New Alert characteristic notification is disabled.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation was successfully started. The 
 *         ANCLIENT_NEW_ALERT_COMPLETE will be sent when the request completes.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus ANCLIENT_EnableNewAlertNotify(AnClientApp *App, BOOL Enable);

/*---------------------------------------------------------------------------
 * ANCLIENT_EnableUnreadAlertNotify()
 *
 *     Enable/Disable the sending of Unread Alert notifications from the remote
 *     device. This function writes the Characteristic Client Configuration
 *     and will enable/disable all notifications. Individual categories are
 *     enabled or disabled using ANCLIENT_WriteControlPoint().
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 *     Enable - If TRUE, the Unread Alert characteristic notification is enabled.
 *              If FALSE, the Unread Alert characteristic notification is disabled.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation was successfully started. The 
 *         ANCLIENT_UNREAD_ALERT_COMPLETE will be sent when the request completes.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus ANCLIENT_EnableUnreadAlertNotify(AnClientApp *App, BOOL Enable);

/*---------------------------------------------------------------------------
 * ANCLIENT_WriteControlPoint()
 *
 *     Write the given command to the Alert Notification Control Point on
 *     the remote device. 
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 *     cmd - Command to be written
 *
 *     id - the category id to be written
 *
 * Returns:
 *     BT_STATUS_PENDING - operation was successfully started. The 
 *         ANCLIENT_WRITE_CP_COMPLETE will be sent when the request completes.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus ANCLIENT_WriteControlPoint(AnClientApp *App, GattAlertNotifyCommand cmd,
                                    GattAlertCategoryId id);

#endif /* __AN_CLIENT_H_*/
