/****************************************************************************
 *
 * File:
 *     ht_collector.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions for the Collector role of the LE
 *     Health Thermometer profile.
 *
 * Created:
 *     June 25,2012
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
#include "ht_services.h"
#include "devinfo_service.h"
#include "gattsecurity.h"

#ifndef __HT_COLLECTOR_H_
#define __HT_COLLECTOR_H_

/*---------------------------------------------------------------------------
 * Health Thermometer Profile Collector layer
 *
 *     Implements the protocol portion of a Health Thermometer profile
 *     Collector application. This layer is meant to be an example of
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
 * HtClctrEventType type
 *
 *     The Health Thermometer profile Collector Application Event types are
 *     passed to the application handler provided in HTCLCTR_Init().
 */
typedef BtEventTypeEx HtClctrEventType;

/* A connection request has completed. The "e.status" field indicates success
 * or failure of the operation. "app->remDev" identifies the remote device.
 */
#define HTCLCTR_CONNECT_COMPLETE    (BTE_TYPE_BASE_HTCLCTR + 0)

/* The connection has been disconnected. */
#define HTCLCTR_DISCONNECT          (BTE_TYPE_BASE_HTCLCTR + 1)

/* Intermediate Temperature Notification has been received */
#define HTCLCTR_INTERMEDIATE_TEMP   (BTE_TYPE_BASE_HTCLCTR + 2)

/* Temperature Indication has been received */
#define HTCLCTR_TEMPERATURE_IND     (BTE_TYPE_BASE_HTCLCTR + 3)

/* Battery Level Notification has been received */
#define HTCLCTR_BATTERY_LEVEL       (BTE_TYPE_BASE_HTCLCTR + 4)

/* End of HtClctrEventType */

/* Forward reference */
typedef struct _HtClctrEvent HtClctrEvent;

/*---------------------------------------------------------------------------
 * HtClctrEventCallback type
 *
 * A function of this type is called to indicate Health Thermometer profile
 * Collector application events.
 */
typedef void (*HtClctrEventCallback)(void *context, HtClctrEvent *event);

/* End of HtClctrEventCallback */

/*---------------------------------------------------------------------------
 * HtClctrServiceFlags type
 *
 * The HtClctrServiceFlags type indicates which Health Temperature services
 * are available on the remote device along with any important
 * characteristics. The flags are ORed together.
 */
typedef U8 HtClctrServiceFlags;

/* The Health Thermometer service is present on the remote device */
#define HTCLCTRCSF_HEALTH_THERM     0x01

/* The Intermediate Temperature characteristic is present on the remote device */
#define HTCLCTRCSF_INTER_TEMP       0x02

/* The Measurement Interval characteristic is present on the remote device */
#define HTCLCTRCSF_MEAS_INTVL       0x04

/* The Health Device Info service is present on the remote device */
#define HTCLCTRCSF_HEALTH_DEV_INFO  0x08

/* The Battery service is present on the remote device */
#define HTCLCTRCSF_BATTERY          0x10

/* All the services */
#define HTCLCTRCSF_ALL_SERVICES    (HTCLCTRCSF_HEALTH_THERM | HTCLCTRCSF_INTER_TEMP |\
                                    HTCLCTRCSF_MEAS_INTVL | HTCLCTRCSF_HEALTH_DEV_INFO | HTCLCTRCSF_BATTERY)

/* End of HtClctrServiceFlags */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HtClctrApp structure
 *
 *     The HtClctrApp represents the entire unit of what a Health Thermometer
 *     Collector application must contain. The HtClctrApp must be allocated
 *     and initialized by the application, prior to it being used in any
 *     other API's. The HtClctrApp structure is initialized by calling
 *     HTCLCTR_Init().
 */
typedef struct _HtClctrApp {

    /* Group: Read only fields */

    /* Represents the remote device in which a connection has been made. It
     * is valid during the HTCLCTR_CONNECT_COMPLETE event
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
    HtClctrServiceFlags     flags;

    /* Contains temperature value result. Valid for HTCLCTR_INTERMEDIATE_TEMP
     * or HTCLCTR_TEMPERATURE_IND events.
     */
    BtTemperature           temperature;

    /* Contains Health Device Information characteristic values. Valid if
     * HTCLCTRCSF_HEALTH_DEV_INFO flag is set.
     */
    GattDevInfo             hdInfo;

    /* Contains Battery Level Service characteristic value. Valid for the 
     * HTCLCTR_BATTERY_LEVEL event. Valid if HTCLCTRCSF_BATTERY flag is set.
     */
    U8                      batteryLevel;

    /* Contains Measurement Interval Valid Range characteristic value.
     * Valid if HTCLCTRCSF_MEAS_INTVL flag is set.
     */
    U16                     measurementIntervalLowRange;
    U16                     measurementIntervalHiRange;

    /* === Internal use only === */

    CmgrHandlerCtx          cmgrHandler;
    GattClientToken         gattToken;
    GattClientToken         gattTemperatureToken;
    GattClientToken         gattMeasurementIntervalToken;
    GattClientToken         gattMeasurementIntervalValueToken;
    GattClientToken         gattIntermediateToken;
    GattClientToken         gattBatteryToken;
    GattSecurityToken       gattSecToken;
    BtEventTypeEx           eventType;
    BtHandlerEx            *appHandler;
    void                   *healthThermometerSvcRef;
    void                   *healthDeviceInfoSvcRef;
    void                   *batterySvcRef;
    void                   *temperatureMeasurmentChrRef;
    void                   *intermediateTemperatureChrRef;
    void                   *measurementIntervalChrRef;
    void                   *batteryLevelChrRef;
    U16                     temperatureMeasurmentConfig;
    U16                     intermediateTemperatureConfig;
    U16                     batteryLevelConfig;
    U16                     measurementIntervalConfig;
    U16                     measurementIntervalValHandle;
    U16                     measurementIntervalValidRangeHandle;
    U16                     measurementInterval;
    U16                     systemIdValHandle;
    U16                     modelNumValHandle;
    U16                     serialNumValHandle;
    U16                     firmwareRevValHandle;
    U16                     hardwareRevValHandle;
    U16                     softwareRevValHandle;
    U16                     manufactureNameValHandle;
    U16                     batteryLevelValHandle;
    BOOL                    gattConnected;
    U16                     state;
    U16                     retryState;
    GattClientCharConfig    indicateCfg;
    GattClientCharConfig    indicateMICfg;
    GattClientCharConfig    notifyCfg;
    GattSearchType          stype;

} HtClctrApp;


/*---------------------------------------------------------------------------
 * HtClctrEvent structure
 *
 *     Used to pass Health Thermometer profile Collector application events to
 *     the applications.
 */
struct _HtClctrEvent {
    /* Inherit from BtEventEx. Note that "e.handler" contains a pointer
     * to the handler which is the CmgrLeAdvertHandler
     */
    BtEventEx                   e;

    /* Pointer to application structure */
    HtClctrApp                 *app;
};


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HTCLCTR_Init()
 *
 *     Initialize the Health Thermometer profile Collector application.
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
BtStatus HTCLCTR_Init(HtClctrApp *app, BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * HTCLCTR_Deinit()
 *
 *     Deinitialize the Health Thermometer profile Collector application.
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
BtStatus HTCLCTR_Deinit(HtClctrApp *app);

/*---------------------------------------------------------------------------
 * HTCLCTR_Connect()
 *
 *     Initiate a connection to a remote device. This includes creating an
 *     ACL connection if needed then a GATT connection. As well as finding
 *     the Health Thermometer, Health Device Information and Battery
 *     services and current values.
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
 *         HTCLCTR_CONNECT_COMPLETE event will be sent to the handler when
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
BtStatus HTCLCTR_Connect(HtClctrApp *app, BtDevAddr *devAddr);

/*---------------------------------------------------------------------------
 * HTCLCTR_Disconnect()
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
 *         or write attempt is started. A HTCLCTR_CONNECT_COMPLETE event with
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
BtStatus HTCLCTR_Disconnect(HtClctrApp *app);

/*---------------------------------------------------------------------------
 * HTCLCTR_ConfigureTemperature()
 *
 *     Configure the Temperature Measurement characteristic on the
 *     connected Thermometer Service. If the connection is up and stable,
 *     this service is available.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 *     Enable - If TRUE, the Temperature Measurement service is enabled. If
 *              FALSE, the Temperature Measurement service is disabled.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus HTCLCTR_ConfigureTemperature(HtClctrApp *App, BOOL Enable);

/*---------------------------------------------------------------------------
 * HTCLCTR_ConfigureIntermediate()
 *
 *     Configure the Intermediate Temperature Measurement characteristic on
 *     the connected Thermometer Service. This API is available if the
 *     HTCLCTRCSF_INTER_TEMP flag is set in the flags field. Otherwise the
 *     BT_STATUS_NOT_FOUND status will be returned.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 *     Enable - If TRUE, the Intermediate Temperature service is enabled. If
 *              FALSE, the Intermediate Temperature service is disabled.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_NOT_FOUND - the Intermediate Temperature Measurement
 *         characteristic was not found.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no
 *         connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus HTCLCTR_ConfigureIntermediate(HtClctrApp *App, BOOL Enable);

/*---------------------------------------------------------------------------
 * HTCLCTR_ConfigureMeasurementInterval()
 *
 *     Configure the Measurement Interval characteristic and delay on
 *     the connected Thermometer Service. This API is available if the
 *     HTCLCTRCSF_MEAS_INTVL flag is set in the flags field. Otherwise the
 *     BT_STATUS_NOT_FOUND status will be returned.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 *     Enable - If TRUE, the Measurement Interval service is enabled. If
 *              FALSE, the Measurement Interval service is disabled.
 *
 *     Delay - The interval between measurements in seconds. Set to 0 to
 *             disable Measurement Interval indications.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_NOT_FOUND - the Intermediate Temperature Measurement
 *         characteristic was not found.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no
 *         connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus HTCLCTR_ConfigureMeasurementInterval(HtClctrApp *App, BOOL Enable, U16 Delay);

/*---------------------------------------------------------------------------
 * HTCLCTR_ConfigureBatteryLevel()
 *
 *     Configure the Battery Level characteristic on the
 *     connected Battery Service. If the connection is up and stable,
 *     this service is available.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 *     Enable - If TRUE, the Battery Level service is enabled. If
 *              FALSE, the Battery Level service is disabled.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus HTCLCTR_ConfigureBatteryLevel(HtClctrApp *App, BOOL Enable);
#endif /* __HT_COLLECTOR_H_ */
