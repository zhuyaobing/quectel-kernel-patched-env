/****************************************************************************
 *
 * File:
 *     hr_sensor.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions for the Heart Rate Sensor
 *     of the LE Heart Rate profile.
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
#include "hr_services.h"
#include "devinfo_service.h"
#include "gattsecurity.h"

#ifndef __HT_HEARTRATESENSOR_H_
#define __HT_HEARTRATESENSOR__H_

/*---------------------------------------------------------------------------
 * Heart Rate Profile Server (Sensor) layer
 *
 *     Implements the protocol portion of a Heart Rate profile
 *     server (sensor) application. This layer is meant to be an example of
 *     how one might implement the Heart Rate profile. This
 *     code may need modified by the end user but can act as a base
 *     for how it could be implemented.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HrSensorEventType type
 *
 *     The Heart Rate profile Sensor Application Event types are
 *     passed to the application handler provided in HRSENSOR_Init().
 */
typedef BtEventTypeEx HrSensorEventType;

/* An incoming connection from a remote device to the Heart Rate Sensor
 * application has occurred. "con->remDev" identifies the remote device.
 */
#define HRSENSOR_CONNECT                 (BTE_TYPE_BASE_HRSENSOR + 0)

/* An incoming connection from a remote device to the Heart Rate Sensor
 * has occurred and there is no con structure available to handle it.
 * The application is requested to call HRSENSOR_AddListenCon() to
 * add one.
 */
#define HRSENSOR_NEED_CON                (BTE_TYPE_BASE_HRSENSOR + 1)

/* The incoming connection for a remote device has been disconnected. */
#define HRSENSOR_DISCONNECT              (BTE_TYPE_BASE_HRSENSOR + 2)

/* The Heart Rate Notify operation has completed. "e.status" indicates
 * if the operation was successful or not.
 */
#define HRSENSOR_NOTIFY_HR_COMPLETE      (BTE_TYPE_BASE_HRSENSOR + 3)

/* The remote device has enable/disabled the Heart Measurement to
 * be notified. if "e.enableHr" is TRUE then heart rate measurements
 * notify have been enabled and the sensor should send heart measurements
 * by calling HRSENSOR_NotifyHeartRate(). If "e.enableHr" is FALSE then
 * the app should stop sending heart rate measurements.
 */
#define HRSENSOR_HR_NOTIFY_ENABLE        (BTE_TYPE_BASE_HRSENSOR + 4)

/* The remote device has reset the energy expended field by writing to
 * the Heart Rate control point attribute.
 */
#define HRSENSOR_HR_RESET_EE             (BTE_TYPE_BASE_HRSENSOR + 5)

/* End of HrSensorEventType */

/* Forward reference */
typedef struct _HrSensorEvent HrSensorEvent;

/*---------------------------------------------------------------------------
 * HrSensorEventCallback type
 *
 * A function of this type is called to indicate Heart Rate profile
 * sensor application events.
 */
typedef void (*HrSensorEventCallback)(void *context, HrSensorEvent *event);

/* End of HrSensorEventCallback */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HrSensorApp structure
 *
 *     The HrSensorApp represents the entire unit of what a Heart Rate
 *     Sensor (server) must contain. The HrSensorApp must be
 *     allocated and initialized by the application, prior to it being
 *     used in any other API's. The HrSensorApp structure is
 *     initialized by calling HRSENSOR_Init().
 */
typedef struct _HrSensorApp {

    /* === Internal use only === */

    ListEntry           conList;
    CmgrLeAdvertUuid    auHeartRate;
    BtHandlerEx         gattHandler;
    GattService        *gsHeartRate;
#if LE_HEART_RATE_SUPPORT_SDP == XA_ENABLED
    GattSdpRecord       sdpHeartRate;
#endif /* LE_HEART_RATE_SUPPORT_SDP == XA_ENABLED */
    BtHandlerEx        *appHandler;

} HrSensorApp;


/*---------------------------------------------------------------------------
 * HrSensorCon structure
 *
 *     The HrSensorCon is used to manage a Heart Rate connection.
 */
typedef struct _HrSensorCon {

    ListEntry            node;   /* Internal use only  */

    /* Group: Read only fields */

    /* Represents the remote device in which a connection has been made. It
     * is valid during all events.
     */
    BtRemoteDevice      *remDev;

    /* Address currently being used by the remote device connected to
     * the GATT server. It valid for all events.
     */
    BtDevAddr            devAddr;

    /* Identity address used to index into DDB. This is the "real" address. Valid
     * for all events.
     */
    BtDevAddr            identityAddr;

    /* MTU used on the GATT connection. Valid while a connection exists */
    U16                  mtu;

    /* Contains the current Client Characteristic Configuration value for the Heart
     * Rate measurement characteristic. This indicates if notification are enabled or not.
     */
    BOOL                notifyEnabled;

    /* === Internal use only === */
    CmgrHandlerCtx      cmgrHandler;
    GattServerToken     gattToken;
    GattSecurityToken   secToken;
    HrSensorApp   *app;

    EvmTimer            timer;
    BOOL                sendNotify;
    BOOL                notifySent;
    BOOL                disconnect;

} HrSensorCon;

/*---------------------------------------------------------------------------
 * HrSensorEvent structure
 *
 *     Used to pass Heart Rate profile sensor application events to
 *     applications.
 */
struct _HrSensorEvent {
    /* Inherit from BtEventEx. Note that "e.handler" contains a pointer
     * to the handler which is the CmgrLeAdvertHandler
     */
    BtEventEx        e;

    /* Pointer to connection structure valid for most events */
    HrSensorCon     *con;

    /* Pointer to appication structure valid during all events */
    HrSensorApp     *app;

    /* Represents the remote device in which a connection has been made. It
     * is valid during all events.
     */
    BtRemoteDevice  *remDev;
};


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HRSENSOR_Init()
 *
 *     Initialize the Heart Rate profile sensor application.
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
BtStatus HRSENSOR_Init(HrSensorApp *App, BtHandlerEx *Handler);

/*---------------------------------------------------------------------------
 * HRSENSOR_Deinit()
 *
 *     Deinitialize the Heart Rate profile sensor application.
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
BtStatus HRSENSOR_Deinit(HrSensorApp *App, BOOL IsShutDown);

/*---------------------------------------------------------------------------
 * HRSENSOR_Disconnect()
 *
 *     Disconnects the Heart Rate profile sensor from a remote
 *     device specificed by'con'. If the Heart Rate sensor connection
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
BtStatus HRSENSOR_Disconnect(HrSensorCon *Con);

/*---------------------------------------------------------------------------
 * HRSENSOR_NotifyHeartRate()
 *
 *     Transmits the Heart Rate measurement to the Collector. This
 *     API must be called only after the application receives an
 *     HRSENSOR_HR_NOTIFY_ENABLE event.  The HRSENSOR_NOTIFY_HR_COMPLETE 
 *     event will be indicated when the heart rate measurement has been 
 *     transmitted.
 *
 * Parameters:
 *     con - Pointer to a structure that represents the connection.
 *
 *     hr - Pointer to heart rate measurement to notify.
 *
 * Returns:
 *     BT_STATUS_PENDING - request was started successfully. When complete
 *         the handler will be sent the HRSENSOR_NOTIFY_HR_COMPLETE event.
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
BtStatus HRSENSOR_NotifyHeartRate(HrSensorCon *con, GattHrMeasurement *hr);

/*---------------------------------------------------------------------------
 * HRSENSOR_SetSensorLocation()
 *
 *     This API is used to set the current Body Sensor Location. It cannot be
 *     called while a connection exists to the heart rate service.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     loc - Body Sensor location.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed because a connection to the heart
 *         rate service exists.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 or loc is invalid (XA_ERROR_CHECK only).
 *
 */
BtStatus HRSENSOR_SetSensorLocation(HrSensorApp *app, GattBodySensorLocaton loc);

/*---------------------------------------------------------------------------
 * HRSENSOR_GetSensorLocation()
 *
 *     This API is used to get the current Body Sensor Location.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     loc - Pointer to location to place body Sensor location.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed
 *
 *     BT_STATUS_INVALID_PARM - app or loc is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus HRSENSOR_GetSensorLocation(HrSensorApp *app, GattBodySensorLocaton *loc);

/*---------------------------------------------------------------------------
 * HRSENSOR_AddListenCon()
 *
 *     Add a HrSensorCon structure to the Heart Rate sensor that is used
 *     to "hold" the link up while heart rate information is being indicated
 *     to the remote device so that other local profiles and applications
 *     will not cause the link to disconnect unexpectedly. Also used to
 *     disconnect link when the operation is complete.
 *
 *     The best time to provide a HrSensorCon structure is when HRSENSOR_Init
 *     is called to handle the first incoming connection and then when
 *     the HRSENSOR_CONNECT event is received to handle the next connection.
 *     HRSENSOR_AddListenCon() can also be used at init time to provide
 *     a number of HrSensorCon structures to the server up front.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     con - Pointer to a HrSensorCon structure.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed.
 *
 *     BT_STATUS_INVALID_PARM - app or con is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus HRSENSOR_AddListenCon(HrSensorApp *app, HrSensorCon *con);

/*---------------------------------------------------------------------------
 * HRSENSOR_RemoveListenCon()
 *
 *     Remove a previously added HrSensorCon structure. The HrSensorCon
 *     structure should not be removed while it is involved in a connection.
 *     Doing so will most likely result in the link being disconnected.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     con - Pointer to a HrSensorCon structure.
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
BtStatus HRSENSOR_RemoveListenCon(HrSensorApp *app, HrSensorCon *con);


#endif /* __HT_HEARTRATESENSOR__H_ */
