/****************************************************************************
 *
 * File:
 *     hr_collector.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions for the Collector role of the LE
 *     Heart Rate profile.
 *
 * Created:
 *     December 7,2012
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
#include "hr_services.h"
#include "devinfo_service.h"
#include "gattsecurity.h"

#ifndef __HR_COLLECTOR_H_
#define __HR_COLLECTOR_H_

/*---------------------------------------------------------------------------
 * Heart Rate Profile Collector layer
 *
 *     Implements the protocol portion of a Heart Rate profile
 *     Collector application. This layer is meant to be an example of 
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
 * HrClctrEventType type
 *
 *     The Heart Rate profile Collector Application Event types are
 *     passed to the application handler provided in HRCLCTR_Init().
 */
typedef BtEventTypeEx HrClctrEventType;

/* A connection request has completed. The "e.status" field indicates success
 * or failure of the operation. "app->remDev" identifies the remote device.
 */
#define HRCLCTR_CONNECT_COMPLETE    (BTE_TYPE_BASE_HRCLCTR + 0)

/* The connection has been disconnected. */
#define HRCLCTR_DISCONNECT          (BTE_TYPE_BASE_HRCLCTR + 1)

/* Enabling/disabling the heart rate measurement is complete */
#define HRCLCTR_HR_ENABLE_COMPLETE  (BTE_TYPE_BASE_HRCLCTR + 2)

/* A Heart rate measurement has been notified by the remote device */
#define HRCLCTR_HEART_RATE_NOTIFY   (BTE_TYPE_BASE_HRCLCTR + 3)

/* Resetting the energy expended value is complete */
#define HRCLCTR_RESET_EE_COMPLETE   (BTE_TYPE_BASE_HRCLCTR + 4)

/* End of HrClctrEventType */

/* Forward reference */
typedef struct _HrClctrEvent HrClctrEvent;

/*---------------------------------------------------------------------------
 * HrClctrEventCallback type
 *
 * A function of this type is called to indicate Heart Rate profile
 * Collector application events.
 */
typedef void (*HrClctrEventCallback)(void *context, HrClctrEvent *event);

/* End of HrClctrEventCallback */

/*---------------------------------------------------------------------------
 * HrClctrServiceFlags type
 *
 * The HrClctrServiceFlags type indicates which Heart Rate services
 * are available on the remote device along with any important
 * characteristics. The flags are ORed together.
 */
typedef U8 HrClctrServiceFlags;

/* The Heart Rate service is present on the remote device */
#define HRCLCTRCSF_HEART_RATE       0x01

/* The Body Sensor Location characteristic is present on the remote device */
#define HRCLCTRCSF_BODY_LOCP        0x02

/* The Heart Rate Sensor control point characteristic is present on the remote device */
#define HRCLCTRCSF_CONTROL_PT       0x04

/* The Device Information service is present on the remote device */
#define HRCLCTRCSF_DEV_INFO         0x08

/* All the services */
#define HRCLCTRCSF_ALL_SERVICES    (HRCLCTRCSF_HEART_RATE | HRCLCTRCSF_BODY_LOCP |\
                                    HRCLCTRCSF_CONTROL_PT | HRCLCTRCSF_DEV_INFO )

/* End of HrClctrServiceFlags */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HrClctrApp structure
 *
 *     The HrClctrApp represents the entire unit of what a Heart Rate
 *     Collector application must contain. The HrClctrApp must be allocated
 *     and initialized by the application, prior to it being used in any
 *     other API's. The HrClctrApp structure is initialized by calling
 *     HRCLCTR_Init().
 */
typedef struct _HrClctrApp {

    /* Group: Read only fields */

    /* Represents the remote device in which a connection has been made. It
     * is valid during the HRCLCTR_CONNECT_COMPLETE event
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
    HrClctrServiceFlags     flags;

    /* Contains the body sensor location if HRCLCTRCSF_BODY_LOCP is set */
    GattBodySensorLocaton   bodyLoc;

    /* Contains heart rate measurement. Valid for HRCLCTR_HEART_RATE_NOTIFY
     * event.
     */
    GattHrMeasurement       hrMeasurement;

    /* Contains Health Device Information characteristic values. Valid if
     * HRCLCTRCSF_DEV_INFO flag is set.
     */
    GattDevInfo             hdInfo;

    /* Contains the current Characteristic Client Configuration value for the 
     * Heart Rate Measurement. This indicates if notifications are enabled or not.
     */
    GattClientCharConfig     hrCcc;

    /* === Internal use only === */

    CmgrHandlerCtx          cmgrHandler;
    GattClientToken         gattToken;
    GattSecurityToken       gattSecToken;
    BtEventTypeEx           eventType;
    BtHandlerEx            *appHandler;
    void                   *heartRateSvcRef;
    void                   *deviceInfoSvcRef;
    void                   *heartRateMeasurementChrRef;
    void                   *bodySensorLocationChrRef;
    void                   *heartRateControlPointChrRef;
    U16                     heartRateMeasurmentConfig;
    U16                     bodySensorLocationValHandle;
    U16                     heartRateControlPointHandle;
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
    U8                      notifyCfg[2];
    GattSearchType          stype;

} HrClctrApp;


/*---------------------------------------------------------------------------
 * HrClctrEvent structure
 *
 *     Used to pass Heart Rate profile Collector application events to
 *     the applications.
 */
struct _HrClctrEvent {
    /* Inherit from BtEventEx. Note that "e.handler" contains a pointer
     * to the handler which is the CmgrLeAdvertHandler
     */
    BtEventEx                   e;

    /* Pointer to application structure. Valid for all events */
    HrClctrApp                 *app;
};


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HRCLCTR_Init()
 *
 *     Initialize the Heart Rate profile Collector application.
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
BtStatus HRCLCTR_Init(HrClctrApp *app, BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * HRCLCTR_Deinit()
 *
 *     Deinitialize the Heart Rate profile Collector application.
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
BtStatus HRCLCTR_Deinit(HrClctrApp *app);

/*---------------------------------------------------------------------------
 * HRCLCTR_Connect()
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
 *         HRCLCTR_CONNECT_COMPLETE event will be sent to the handler when
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
BtStatus HRCLCTR_Connect(HrClctrApp *app, BtDevAddr *devAddr);

/*---------------------------------------------------------------------------
 * HRCLCTR_Disconnect()
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
 *         or write attempt is started. A HRCLCTR_CONNECT_COMPLETE event with
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
BtStatus HRCLCTR_Disconnect(HrClctrApp *app);

/*---------------------------------------------------------------------------
 * HRCLCTR_ConfigureHeartRate()
 *
 *     Configure the Heart Rate Measurement characteristic on the
 *     connected Heart Rate Sensor Service. If the connection is up and stable,
 *     this service is available. This will result in heart rate notifications
 *     to be sent by the remote device.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 *     Enable - If TRUE, the Heart Rate Measurement characteristic is enabled. If
 *              FALSE, the Heart Rate Measurement service is disabled.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation was successfully started. The 
 *         HRCLCTR_HR_ENABLE_COMPLETE will be sent when the request completes.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus HRCLCTR_ConfigureHeartRate(HrClctrApp *App, BOOL Enable);

/*---------------------------------------------------------------------------
 * HRCLCTR_ResetEnergyExp()
 *
 *     Reset the Energy Expended value on the connected Heart Rate Sensor 
 *     Service. If the connection is up and stable and the Heart Rate control
 *     point characteristic is available, this service is available. This will 
 *     result in Energy Expended value being reset to 0. 
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation was successfully started. The 
 *         HRCLCTR_RESET_EE_COMPLETE will be sent when the request completes.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_FAILED - operation failed because the Heart Rate Control point
 *         characteristic is not supported by the remote device.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus HRCLCTR_ResetEnergyExp(HrClctrApp *App);

#endif /* __HR_COLLECTOR_H_ */
