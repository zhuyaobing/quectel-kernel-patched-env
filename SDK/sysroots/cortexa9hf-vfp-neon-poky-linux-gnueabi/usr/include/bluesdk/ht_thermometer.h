/****************************************************************************
 *
 * File:
 *     ht_thermometer.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions for the Thermometer Service
 *     of the LE Health Thermometer profile.
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
#include "ht_services.h"
#include "gattsecurity.h"

#ifndef __HT_THERMOMETER_H_
#define __HT_THERMOMETER_H_

/*---------------------------------------------------------------------------
 * Health Thermometer Profile Server layer
 *
 *     Implements the protocol portion of a Health Thermometer profile
 *     server application. This layer is meant to be an example of
 *     how one might implement the health thermometer profile. This
 *     code may need modified by the end user but can act as a base
 *     for how it could be implemented.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HtThermometerEventType type
 *
 *     The HealthThermometer profile Server Application Event types are
 *     passed to the application handler provided in HTTHERM_Init().
 */
typedef BtEventTypeEx HtThermometerEventType;

/* An incoming connection from a remote device to the Health Thermometer
 * profile server application has occurred. "app->remDev" identifies the
 * remote device.
 */
#define HTTHERM_CONNECT                  (BTE_TYPE_BASE_HTTHERM + 0)

/* An incoming connection from a remote device to the Health Thermometer
 * has occurred and there is no con structure available to handle it.
 * The application is requested to call HTTHERM_AddListenCon() to
 * add one.
 */
#define HTTHERM_NEED_CON                        (BTE_TYPE_BASE_HTTHERM + 1)

/* The incoming connection for a remote device has been disconnected. */
#define HTTHERM_DISCONNECT                      (BTE_TYPE_BASE_HTTHERM + 2)

/* The sending a temperature notification is complete.  */
#define HTTHERM_TEMP_NOTIFY_COMPLETE            (BTE_TYPE_BASE_HTTHERM + 3)

/* The sending a temperature indication is complete.  */
#define HTTHERM_TEMP_INDICATE_COMPLETE          (BTE_TYPE_BASE_HTTHERM + 4)

/* The sending a battery level notification is complete.  */
#define HTTHERM_BATTERY_LEVEL_NOTIFY_COMPLETE   (BTE_TYPE_BASE_HTTHERM + 5)

/* The Battery Level is requested. The application must use the
 * HTTHERM_BatteryLevel() API to provide the current battery level.
 */
#define HTTHERM_BATTERY_LEVEL                   (BTE_TYPE_BASE_HTTHERM + 6)

/* The Temperature Type is requested. The application must use the
 * HTTHERM_TemperatureType() API to provide the current battery level.
 */
#define HTTHERM_TEMP_TYPE                       (BTE_TYPE_BASE_HTTHERM + 7)

/* The remote device has enable/disabled the Temperature Measurement to
 * be indicated. 
 */
#define HTTHERM_TEMP_INDICATE_ENABLE            (BTE_TYPE_BASE_HTTHERM + 8)

/* The remote device has enable/disabled the Intermediate Temperature to
 * be notified. 
 */
#define HTTHERM_INT_TEMP_NOTIFY_ENABLE          (BTE_TYPE_BASE_HTTHERM + 9)

/* The remote device has enable/disabled the Measurement Interval to
 * be indicated. 
 */
#define HTTHERM_MEAS_INTV_INDICATE_ENABLE       (BTE_TYPE_BASE_HTTHERM + 10)

/* The remote device has enable/disabled the Battery Level to be
 * notified.
 */
#define HTTHERM_BATTERY_LEVEL_NOTIFY_ENABLE     (BTE_TYPE_BASE_HTTHERM + 11)

/* The remote device has written a measurement interval */
#define HTTHERM_MEAS_INTV_WRITTEN               (BTE_TYPE_BASE_HTTHERM + 12)

/* End of HtThermometerEventType */

/* Forward reference */
typedef struct _HtThermometerEvent HtThermometerEvent;

/*---------------------------------------------------------------------------
 * HtThermometerEventCallback type
 *
 * A function of this type is called to indicate Health Thermometer profile
 * server application events.
 */
typedef void (*HtThermometerEventCallback)(void *context, HtThermometerEvent *event);

/* End of HtThermometerEventCallback */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HtThermometerApp structure
 *
 *     The HtThermometerApp represents the entire unit of what a Health
 *     Thermomometer server must contain. The HtThermometerApp must be
 *     allocated and initialized by the application, prior to it being
 *     used in any other API's. The HtThermometerApp structure is
 *     initialized by calling HTTHERM_Init().
 */
typedef struct _HtThermometerApp {

    /* Measurement Interval */
    U16                 measurementInterval;

    /* === Internal use only === */

    ListEntry           conList;
    CmgrLeAdvertUuid    auHealthThermometer;
    CmgrLeAdvertUuid    auBattery;
    BtHandlerEx         gattHandler;
    GattService        *gsHealthThermometer;
    GattService        *gsBattery;
#if LE_HEALTH_THERMOMETER_SUPPORT_SDP == XA_ENABLED
    GattSdpRecord       sdpHealthThermometer;
    GattSdpRecord       sdpBattery;
#endif /* LE_HEALTH_THERMOMETER_SUPPORT_SDP == XA_ENABLED */
    BtHandlerEx        *appHandler;

} HtThermometerApp;


/*---------------------------------------------------------------------------
 * HtThermometerCon structure
 *
 *     The HtThermometerCon is used to manage Thermometer connection. When a remote
 *     remote device accesses the Thermometer characteristic value attribute. 
 */
typedef struct _HtThermometerCon {

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

    /* Set to TRUE if the Client Characteristic Configuration value for the 
     * Temperature measurement is enabled for indications. It has the current setting.
     */
    BOOL                tempMeasIndicateEnabled;

    /* Set to TRUE if the Client Characteristic Configuration value for the 
     * Intermediate Temperature is enabled for notifications.
     */
    BOOL                intTempNotifyEnabled;

    /* Set to TRUE if the Client Characteristic Configuration value for the 
     * Measurement Interval is enabled for indications.
     */
    BOOL                measIntvIndicateEnabled;

    /* Set to TRUE if the Client Characteristic Configuration value for the
     * Battery Level is enabled for notifications.
     */
    BOOL                battLevelNotifyEnabled;

    /* === Internal use only === */
    CmgrHandlerCtx      cmgrHandler;
    GattServerToken     gattToken;
    GattSecurityToken   secToken;
    HtThermometerApp   *app;

    EvmTimer            discTimer;
    BOOL                sendTempNotify;
    BOOL                sendBattNotify;
    BOOL                notifyIndicateSent;

} HtThermometerCon;

/*---------------------------------------------------------------------------
 * HtThermometerEvent structure
 *
 *     Used to pass Health Thermometer profile server application events to
 *     applications.
 */
struct _HtThermometerEvent {
    /* Inherit from BtEventEx. Note that "e.handler" contains a pointer
     * to the handler which is the CmgrLeAdvertHandler
     */
    BtEventEx              e;

    /* Pointer to connection structure valid during HTTHERM_CONNECT, 
     * HTTHERM_DISCONNECT, HTTHERM_TEMP_INDICATE_ENABLE, 
     * HTTHERM_INT_TEMP_NOTIFY_ENABLE, HTTHERM_MEAS_INTV_INDICATE_ENABLE, 
     * HTTHERM_BATTERY_LEVEL_NOTIFY_ENABLE, HTTHERM_TEMP_NOTIFY_COMPLETE,
     * HTTHERM_BATTERY_LEVEL_NOTIFY_COMPLETE, HTTHERM_TEMP_INDICATE_COMPLETE, 
     * and HTTHERM_MEAS_INTV_WRITTEN. If con is 0 for HTTHERM_CONNECT then
     * there is not connection structure available. The application can call
     * HTTHERM_AddListenCon() to provide a structure before returning from
     * the callback. If one is not provided then the link will not be "held"
     * and the tempature nor disconect events will not be generated.
     */
    HtThermometerCon     *con;

    /* Pointer to appication structure valid during all events */
    HtThermometerApp     *app;

    /* Represents the remote device in which a connection has been made. It
     * is valid during all events. Intended to be used for HTTHERM_BATTERY_LEVEL.
     */
    BtRemoteDevice     *remDev;

};


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HTTHERM_Init()
 *
 *     Initialize the Health Thermometer profile server application.
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
BtStatus HTTHERM_Init(HtThermometerApp *App, BtHandlerEx *Handler);

/*---------------------------------------------------------------------------
 * HTTHERM_Deinit()
 *
 *     Deinitialize the Health Thermometer profile server application.
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
BtStatus HTTHERM_Deinit(HtThermometerApp *App, BOOL IsShutDown);

/*---------------------------------------------------------------------------
 * HTTHERM_Disconnect()
 *
 *     Disconnects the Health Thermometer profile server from a remote
 *     device specified by'con'. If the Health Thermometer server connection
 *     is the only active connection, the ACL will be disconnected with this call.
 *
 * Parameters:
 *     Con - Pointer to a structure that represents a connection.
 *
 *     Delay - Time to wait in seconds before disconnecting.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - Con is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus HTTHERM_Disconnect(HtThermometerCon *Con, U16 DelaySeconds);

/*---------------------------------------------------------------------------
 * HTTHERM_IndicateTemperature()
 *
 *     Transmits the final temperature measurement to the Collector.
 *     The HTTHERM_TEMP_INDICATE_COMPLETE event will be indicated when the
 *     temperature measurement has been transmitted.
 *
 * Parameters:
 *     Con - Pointer to a structure that represents the connection.
 *
 *     Temp - The final temperature measurement.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - app, handler or callback in handler is 0
 *         (XA_ERROR_CHECK only).
 *
 */
BtStatus HTTHERM_IndicateTemperature(HtThermometerCon *Con, BtTemperature *Temp);

/*---------------------------------------------------------------------------
 * HTTHERM_NotifyTemperature()
 *
 *     Transmits an intermediate temperature measurement to the Collector. 
 *     The HTTHERM_TEMP_NOTIFY_COMPLETE event will be indicated when the
 *     temperature measurement has been transmitted.
 *
 * Parameters:
 *     Con - Pointer to a structure that represents the connection.
 *
 *     Temp - The intermediate temperature measurement.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - Con or Temp is 0  (XA_ERROR_CHECK only).
 *
 */
BtStatus HTTHERM_NotifyTemperature(HtThermometerCon *Con, BtTemperature *Temp);

/*---------------------------------------------------------------------------
 * HTTHERM_BatteryLevel()
 *
 *     This API is used to provide the current Battery Level when requested
 *     with the HTTHERM_BATTERY_LEVEL event.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 *     remDev - Pointer to structure representing the connection.
 *
 *     Level - The current battery level.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - Con or remDev is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus HTTHERM_BatteryLevel(HtThermometerApp *App, BtRemoteDevice *remDev, U8 Level);

/*---------------------------------------------------------------------------
 * HTTHERM_NotifyBatteryLevel()
 *
 *     Transmits a battery level measurement to the Collector.
 *     The HTTHERM_BATTERY_LEVEL_NOTIFY_COMPLETE event will be indicated 
 *     when the battery level has been transmitted.
 *
 * Parameters:
 *     Con - Pointer to a structure that represents the connection.
 *
 *     Level - The battery level measurement.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - Con or Temp is 0  (XA_ERROR_CHECK only).
 *
 */
BtStatus HTTHERM_NotifyBatteryLevel(HtThermometerCon *Con, U8 Level);

/*---------------------------------------------------------------------------
 * HTTHERM_TemperatureType()
 *
 *     This API is used to provide the current Temperature Type when requested
 *     with the HTTHERM_TEMP_TYPE event.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 *     remDev - Pointer to structure representing the connection.
 *
 *     Type - The current temperature type.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - Con or remDev is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus HTTHERM_TemperatureType(HtThermometerApp *App, BtRemoteDevice *remDev, U8 Type);

/*---------------------------------------------------------------------------
 * HTTHERM_AddListenCon()
 *
 *     Add a HtThermometerCon structure to the Thermometer server that is used
 *     to "hold" the link up while thermometer information is being indicated
 *     to the remote device so that other local profiles and applications
 *     will not cause the link to disconnect unexpectedly. Also used to
 *     disconnect link when the operation is complete.
 *
 *     The best time to provide a HtThermometerCon structure is when HTTHERM_Init
 *     is called to handle the first incoming connection and then when
 *     the HTTHERM_CONNECT event is received to handle the next connection.
 *     HTTHERM_AddListenCon() can also be used at init time to provide
 *     a number of HtThermometerCon structures to the proximity server up front.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     con - Pointer to a HtThermometerCon structure.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed.
 *
 *     BT_STATUS_INVALID_PARM - app or con is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus HTTHERM_AddListenCon(HtThermometerApp *app, HtThermometerCon *con);

/*---------------------------------------------------------------------------
 * HTTHERM_RemoveListenCon()
 *
 *     Remove a previously added HtThermometerCon structure. The HtThermometerCon
 *     structure should not be removed while it is involved in a connection.
 *     Doing so will most likely result in the link being disconnected.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     con - Pointer to a HtThermometerCon structure.
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
BtStatus HTTHERM_RemoveListenCon(HtThermometerApp *app, HtThermometerCon *con);


#endif /* __HT_THERMOMETER_H_ */
