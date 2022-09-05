/****************************************************************************
 *
 * File:
 *     an_server.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions for the Server role of the LE
 *     Alert Notification Profile.
 *
 * Created:
 *     June 22,2012
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
#include "gattserver.h"
#include "conmgrlea.h"
#include "an_services.h"
#include "devinfo_service.h"
#include "gattsecurity.h"

#ifndef __HT_ALERTNOTIFYSERVER_H_
#define __HT_ALERTNOTIFYSERVER_H_

/*---------------------------------------------------------------------------
 * Alert Notification Profile Server layer
 *
 *     Implements the protocol portion of an Alert Notification profile
 *     server application. This layer is meant to be an example of 
 *     how one might implement the Alert Notification profile. This
 *     code may need to be modified by the end user but can act as a base
 *     for how it could be implemented.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AnServerEventType type
 *
 *     The Alert Notification profile event types are passed to the 
 *     application handler provided in ANSERVER_Init().
 */
typedef BtEventTypeEx AnServerEventType;

/* An incoming connection from a remote device to the Alert Notification
 * profile server application has occurred. "con->remDev" identifies the
 * remote device.
 */
#define ANSERVER_CONNECT                    (BTE_TYPE_BASE_ANSERVER + 0)

/* An incoming connection from a remote device to the Alert Notification server
 * has occurred and there is no con structure available to handle it.
 * The application is requested to call ANSERVER_AddListenCon() to
 * add one.
 */
#define ANSERVER_NEED_CON                   (BTE_TYPE_BASE_ANSERVER + 1)

/* The incoming connection for a remote device has been disconnected. */
#define ANSERVER_DISCONNECT                 (BTE_TYPE_BASE_ANSERVER + 2)

/* The Alert Notification Control Point has been written.
 */
#define ANSERVER_CONTROL_POINT_WRITE        (BTE_TYPE_BASE_ANSERVER + 3)

/* The remote device has enabled/disabled the New Alert to
 * be notified. If "e.notify" is TRUE then New Alert Notifications have 
 * been enabled. If "e.enable" is FALSE then New Alert notifications 
 * have been disabled.
 */
#define ANSERVER_NEW_ALERT_NOTIFY_ENABLE    (BTE_TYPE_BASE_ANSERVER + 4)

/* The remote device has enabled/disabled the Unread Alert Status to
 * be notified. If "e.notify" is TRUE then Unread Alert Notifications have 
 * been enabled. If "e.enable" is FALSE then Unread Alert notifications 
 * have been disabled.
 */
#define ANSERVER_UNREAD_ALERT_NOTIFY_ENABLE (BTE_TYPE_BASE_ANSERVER + 5)

/* The New Alert Notify operation has completed. "e.status" indicates
 * if the operation was successful or not.
 */
#define ANSERVER_NOTIFY_NEW_COMPLETE        (BTE_TYPE_BASE_ANSERVER + 6)

/* The Unread Alert Notify operation has completed. "e.status" indicates
 * if the operation was successful or not.
 */
#define ANSERVER_NOTIFY_UNREAD_COMPLETE     (BTE_TYPE_BASE_ANSERVER + 7)

/* End of AnServerEventType */

/* Forward reference */
typedef struct _AnServerEvent AnServerEvent;

/*---------------------------------------------------------------------------
 * AnServerEventCallback type
 *
 * A function of this type is called to indicate Alert Notification profile
 * server application events.
 */
typedef void (*AnServerEventCallback)(void *context, AnServerEvent *event);

/* End of AnServerEventCallback */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AnServerApp structure
 *
 *     The AnServerApp represents the entire unit of what an Alert
 *     Notification server must contain. The AnServerApp must be
 *     allocated and initialized by the application, prior to it being
 *     used in any other API's. The AnServerApp structure is
 *     initialized by calling ANSERVER_Init().
 */
typedef struct _AnServerApp {

    /* === Internal use only === */

    ListEntry           conList;
    CmgrLeAdvertUuid    auAlertNotify;
    BtHandlerEx         gattHandler;
    GattService        *gsAlertNotify;
#if LE_ALERT_NOTIFY_SUPPORT_SDP == XA_ENABLED
    GattSdpRecord       sdpAlertNotify;
#endif /* LE_ALERT_NOTIFY_SUPPORT_SDP == XA_ENABLED */
    BtHandlerEx        *appHandler;

} AnServerApp;


/*---------------------------------------------------------------------------
 * AnServerCon structure
 *
 *     The AnServerCon is used to manage an Alert Notification connection. 
 *     An AnServerCon structure is needed for each remote device connected
 *     to Alert Notification server. An Alert Notification server application
 *     can initiate a connection to a remote device or the remote device
 *     can connect to the Alert Notification server by accessing one of
 *     the characteristics in the Alert Notification Service.
 */
typedef struct _AnServerCon {

    ListEntry           node;   /* Internal use only  */

    /* Group: Read only fields */

    /* Represents the remote device in which a connection has been made. It
     * is valid during all events.
     */
    BtRemoteDevice     *remDev;

    /* Address currently being used by the remote device connected to
     * the GATT server. It valid for all events.
     */
    BtDevAddr           devAddr;

    /* Identity address used to index into DDB. This is the "real" address. Valid
     * for all events.
     */
    BtDevAddr           identityAddr;

    /* MTU used on the GATT connection. Valid while a connection exists */
    U16                 mtu;

    /* Contains the current Characteristic Client Configuration value for the New Alert.
     * This indicates if notifications are enabled or not.
     */
    GattClientCharConfig     newAlertCcc;

    /* Contains the current Characteristic Client Configuration value for the Unread Alert.
     * status. This indicates if notifications are enabled or not.
     */
    GattClientCharConfig     unreadAlertCcc;

    /* === Internal use only === */
    CmgrHandlerCtx      cmgrHandler;
    GattServerToken     gattToken;
    GattSecurityToken   secToken;
    AnServerApp   *app;

    EvmTimer            timer;
    BOOL                sendNotify;
    BOOL                notifySent;

} AnServerCon;

/*---------------------------------------------------------------------------
 * AnServerEvent structure
 *
 *     Used to pass Alert Notification profile server events to
 *     applications.
 */
struct _AnServerEvent {
    /* Inherit from BtEventEx. Note that "e.handler" contains a pointer
     * to the handler which is the CmgrLeAdvertHandler
     */
    BtEventEx        e;

    /* Pointer to connection structure valid during ANSERVER_CONNECT, 
     * HTTHERM_SEND_TEMPERATURE, HTTHERM_SEND_INTERMEDIATE, 
     * HTTHERM_TEMPERATURE_COMPLETE. If con is 0 for ANSERVER_CONNECT then
     * there is not connection structure available. The application can call 
     * ANSERVER_AddListenCon() to provide a structure before returning from 
     * the callback. If one is not provided then the link will not be "held" 
     * and the temperature nor disconnect events will not be generated.
     */
    AnServerCon     *con;

    /* Pointer to application structure valid during all events */
    AnServerApp     *app;

    /* Represents the remote device in which a connection has been made. It
     * is valid during all events.
     */
    BtRemoteDevice  *remDev;

    /* Values valid for ANSERVER_CONTROL_POINT_WRITE event */
    GattAlertNotifyCommand  cmd;
    GattAlertCategoryId     catId;

    /* Indicates if New Alert or Unread Alert Status notify has been 
     * enabled or not. Valid for ANSERVER_NEW_ALERT_NOTIFY_ENABLE and 
     * ANSERVER_UNREAD_ALERT_NOTIFY_ENABLE.
     */
    BOOL             notify;
};


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * ANSERVER_Init()
 *
 *     Initialize the Alert Notification profile server application.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 *     Handler - Pointer to the handler to receive events.
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
BtStatus ANSERVER_Init(AnServerApp *App, BtHandlerEx *Handler);

/*---------------------------------------------------------------------------
 * ANSERVER_Deinit()
 *
 *     Deinitialize the Alert Notification profile server application.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 *     IsShutdown - TRUE indicates that the application is being shutdown
 *         FALSE otherwise.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - app, handler or callback in handler is 0
 *         (XA_ERROR_CHECK only).
 *
 */
BtStatus ANSERVER_Deinit(AnServerApp *App, BOOL IsShutDown);

/*---------------------------------------------------------------------------
 * ANSERVER_Connect()
 *
 *     Connect the Alert Notification profile server to a remote device
 *     device specificed by'devAddr'. This includes creating an
 *     ACL connection if needed then a GATT connection.
 *
 * Parameters:
 *     Con - Pointer to a structure that represents a connection.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - Con is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus ANSERVER_Connect(AnServerCon *Con, BtDevAddr *devAddr);

/*---------------------------------------------------------------------------
 * ANSERVER_Disconnect()
 *
 *     Disconnects the Alert Notification profile server from a remote
 *     device specificed by'con'. If the Alert Notification server connection
 *     is the only active connection, the ACL will be disconnected with this call.
 *
 * Parameters:
 *     Con - Pointer to a structure that represents a connection.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - Con is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus ANSERVER_Disconnect(AnServerCon *Con);

/*---------------------------------------------------------------------------
 * ANSERVER_GetCategories()
 *
 *     Get the Alert Notification Supported New Alert and Supported Unread 
 *     Alert categories currently stored in the GATT database.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 *     anCat1 - pointer to memory to receive the first octet of Supported
 *        New Alert categories.
 *
 *     anCat2 - pointer to memory to receive the second octet of Supported
 *        New Alert categories.
 *
 *     unCat1 - pointer to memory to receive the first octet of Supported
 *        Unread Alert categories.
 *
 *     unCat2 - pointer to memory to receive the second octet of Supported
 *        Unread Alert categories.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - a pointer is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus ANSERVER_GetCategories(AnServerApp *App, GattAlertCategoryIdBm1 *anCat1, 
                                GattAlertCategoryIdBm2 *anCat2, 
                                GattAlertCategoryIdBm1 *unCat1, 
                                GattAlertCategoryIdBm2 *unCat2);

/*---------------------------------------------------------------------------
 * ANSERVER_SetNewAlertCategories()
 *
 *      Set the Supported New Alert Categories for the Alert Notification 
 *      service in the GATT database.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 *     cat1 - value for the first category octet. 
 *
 *     cat1 - value for the second category octet. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAIALED - operation failed because a connection is active
 *
 *     BT_STATUS_INVALID_PARM - a pointer is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus ANSERVER_SetNewAlertCategories(AnServerApp *App, GattAlertCategoryIdBm1 cat1, 
                                        GattAlertCategoryIdBm2 cat2);

/*---------------------------------------------------------------------------
 * ANSERVER_SetUnreadAlertCategories()
 *
 *      Set the Supported Unread Alert Categories for the Alert Notification 
 *      service in the GATT database.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 *     cat1 - value for the first category octet. 
 *
 *     cat1 - value for the second category octet. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAIALED - operation failed because a connection is active
 *
 *     BT_STATUS_INVALID_PARM - a pointer is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus ANSERVER_SetUnreadAlertCategories(AnServerApp *App, GattAlertCategoryIdBm1 cat1, 
                                           GattAlertCategoryIdBm2 cat2);


/*---------------------------------------------------------------------------
 * ANSERVER_NotifyNewAlert()
 *
 *     Transmits a New Alert Notification to the remote device. 
 *
 * Parameters:
 *     con - Pointer to a structure that represents the connection.
 *
 *     id - Category ID of the alert.
 *
 *     num - number of alerts
 *
 *     infoLen - number of octets in "info". If greater than 18 then only the first 
 *         octets will be sent. It can be set to 0.
 *
 *     info - pointer to string containing text information for the last alert.
 *         If no information then this can be 0.
 *
 * Returns:
 *     BT_STATUS_PENDING - request was started successfully. When complete
 *         the handler will be sent the ANSERVER_NOTIFY_NEW_COMPLETE event.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT client or a connection that is in the process
 *         of coming up.
 *
 *     BT_STATUS_BUSY - request failed because the a previous request has not
 *         yet completed.
 *
 *     BT_STATUS_INVALID_PARM - con or hr is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus ANSERVER_NotifyNewAlert(AnServerCon *con, GattAlertCategoryId id,
                                 U8 num, U8 infoLen, U8 *info);

/*---------------------------------------------------------------------------
 * ANSERVER_NotifyUnreadAlert()
 *
 *     Transmits a Unread Alert Status Notification to the remote device. 
 *
 * Parameters:
 *     con - Pointer to a structure that represents the connection.
 *
 *     id - Category ID of the alert.
 *
 *     num - number of alerts
 *
 * Returns:
 *     BT_STATUS_PENDING - request was started successfully. When complete
 *         the handler will be sent the ANSERVER_NOTIFY_UNREAD_COMPLETE event.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT client or a connection that is in the process
 *         of coming up.
 *
 *     BT_STATUS_BUSY - request failed because the a previous request has not
 *         yet completed.
 *
 *     BT_STATUS_INVALID_PARM - con or hr is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus ANSERVER_NotifyUnreadAlert(AnServerCon *con, GattAlertCategoryId id, U8 num);

/*---------------------------------------------------------------------------
 * ANSERVER_AddListenCon()
 *
 *     Add a AnServerCon structure to the Alert Notification server that is used
 *     to "hold" the link up while alert information is being indicated
 *     to the remote device so that other local profiles and applications
 *     will not cause the link to disconnect unexpectedly. Also used to
 *     disconnect link when the operation is complete. 
 *
 *     The best time to provide a AnServerCon structure is when ANSERVER_Init
 *     is called to handle the first incoming connection and then when 
 *     the ANSERVER_CONNECT event is received to handle the next connection.
 *     ANSERVER_AddListenCon() can also be used at init time to provide
 *     a number of AnServerCon structures to the proximity server up front.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     con - Pointer to a AnServerCon structure. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed.
 *
 *     BT_STATUS_INVALID_PARM - app or con is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus ANSERVER_AddListenCon(AnServerApp *app, AnServerCon *con);

/*---------------------------------------------------------------------------
 * ANSERVER_RemoveListenCon()
 *
 *     Remove a previously added AnServerCon structure. The AnServerCon
 *     structure should not be removed while it is involved in a connection.
 *     Doing so will most likely result in the link being disconnected.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     con - Pointer to a AnServerCon structure. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed because structure has not been
 *         previously added.
 *
 *     BT_STATUS_INVALID_PARM - app or con is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus ANSERVER_RemoveListenCon(AnServerApp *app, AnServerCon *con);


#endif /* __HT_ALERTNOTIFYSERVER_H_ */
