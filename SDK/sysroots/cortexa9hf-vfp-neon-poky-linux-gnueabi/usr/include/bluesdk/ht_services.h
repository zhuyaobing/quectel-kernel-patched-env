/****************************************************************************
 *
 * File:
 *     ht_services.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions for the Services used in the LE
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

#include "mele.h"
#include "gattserver.h"
#include "conmgrlea.h"

#ifndef __HT_SERVICES_H__
#define __HT_SERVICES_H__


/*---------------------------------------------------------------------------
 * Health Thermometer Services layer
 *
 *     Implements and initializes the database components of the Health
 *     Thermometer related services. This layer is meant to be an example
 *     of how one might implement the health thermometer profile. This
 *     code may need modified by the end user but can act as a base
 *     for how it could be implemented.
 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/* Maximum number of intermediate values to indicate for each
 * temperature meaturement.
 */
#define BT_TEMP_MAX_INTERMEDIATE_INTERVALS  5


/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtTempFlags type
 *
 *     The BtTempFlags type provides details about a temperature
 *     measurement.
 */
typedef U8 BtTempFlags;

#define BT_TEMP_FLAG_CELSIUS                0x00
#define BT_TEMP_FLAG_FAHRENHEIT             0x01
#define BT_TEMP_FLAG_TIMESTAMP              0x02
#define BT_TEMP_FLAG_LOCATION               0x04

/* End of BtTempFlags */

/*---------------------------------------------------------------------------
 * BtTempLocation type
 *
 *     The BtTempLocation type indicates the location of a temperature
 *     measurement.
 */
typedef U8 BtTempLocation;

#define BT_TEMP_LOCATION_ARMPIT             0x01
#define BT_TEMP_LOCATION_BODY               0x02
#define BT_TEMP_LOCATION_EAR                0x03
#define BT_TEMP_LOCATION_FINGER             0x04
#define BT_TEMP_LOCATION_GASTRO             0x05
#define BT_TEMP_LOCATION_MOUTH              0x06
#define BT_TEMP_LOCATION_RECTUM             0x07
#define BT_TEMP_LOCATION_TOE                0x08
#define BT_TEMP_LOCATION_EAR_DRUM           0x09

/* End of BtTempLocation */

/* Out of Range value as specified in HTS v1.0 specification */
#define BT_TEMP_OUT_OF_RANGE                0x80


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtFloat structure
 *
 *     The BtFloat structure contains the Temperature Measurment value. The
 *     value is transmitted as a signed 8 bit integer exponent and a 24 bit
 *     mantissa. The exponent and mantissa are transcoded from or to a
 *     floating point value at the sending or receiving end points.
 *
 *     Special Values:
 *         +INFINITY
 *             [exponent 0, mantissa +(2^23-2) (0x007FFFFE)]
 *
 *         NaN (not a number)
 *             [exponent 0, mantissa +(2^23-1) (0x007FFFFF)]
 *
 *         NRes (not at this resolution)
 *             [exponent 0, mantissa -(2^23)   (0x00800000)]
 *
 *         Reserved for future use
 *             [exponent 0, mantissa -(2^23-1) (0x00800001)]
 *
 *         -INFINITY
 *             [exponent 0, mantissa -(2^23-2) (0x00800002)]
 */
typedef struct _BtFloat
{
    S8 exponent;

    /* Big-Endian least significant 24 bits. The most significant 8 bits are
     * not transmitted and must be 0.
     */
    S8 mantissa[4];
} BtFloat;

/*---------------------------------------------------------------------------
 * BtTime structure
 *
 *     The BtTime structure contains the timestamp when the Temperature
 *     Measurement value was measured.
 */
typedef struct _BtTime
{
    /* Big-Endian */
    U8 year[2];
    U8 month;
    U8 day;
    U8 hour;
    U8 minute;
    U8 second;
} BtTime;

/*---------------------------------------------------------------------------
 * BtTemperature structure
 *
 *     The BtTemperature structure contains all information related to a
 *     Temperature Measurement transaction.
 */
typedef struct _BtTemperature
{
    BtTempFlags     flags;
    BtFloat         value;
    BtTime          timestamp;
    BtTempLocation  location;
} BtTemperature;

/*---------------------------------------------------------------------------
 * HdInfo structure
 *
 *     The HdInfo structure contains parameters for the Health Device
 *     Information service.
 */
typedef struct _HdInfo {
    U8  systemID[8];
    U8  modelNumberLen;
    U8  modelNumber[128];
    U8  serialNumberLen;
    U8  serialNumber[128];
    U8  firmwareRevisionLen;
    U8  firmwareRevision[128];
    U8  hardwareRevisionLen;
    U8  hardwareRevision[128];
    U8  softwareRevisionLen;
    U8  softwareRevision[128];
    U8  manufacturerNameLen;
    U8  manufacturerName[128];
    U8  registrationCertificateDataLen;
    U8  registrationCertificateData[128];
} HdInfo;


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SVC_HealthThermometer()
 *
 *     Initialize the Health Thermometer Service database.
 *
 * Parameters:
 *     Handler - Pointer to the handler to receive events.
 *
 *     Service - Pointer to service pointer in which handler is to be set.
 *
 *     AdvertUuid - Pointer to the service UUID to be added to the
 *         Connection Manager advertising data.
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
BtStatus SVC_HealthThermometer(BtHandlerEx *Handler, GattService **Service,
                               CmgrLeAdvertUuid *AdvertUuid);

/*---------------------------------------------------------------------------
 * SVC_StopHealthThermometer()
 *
 *     Deactivate the Health Thermometer Service database.
 *
 * Parameters:
 *     Service - Pointer to service pointer to deactivate.
 *
 *     AdvertUuid - Pointer to the service UUID to remove from the
 *         Connection Manager advertising data.
 *
 *     isShutDown - TRUE means the app is shutting down while FALSE means
 *         the service is to be removed.
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed.
 *
 *     BT_STATUS_INVALID_PARM - app, handler or callback in handler is 0
 *         (XA_ERROR_CHECK only).
 *
 */
BtStatus SVC_StopHealthThermometer(GattService *Service, CmgrLeAdvertUuid *AdvertUuid,
                                   BOOL isShutdown);

/*---------------------------------------------------------------------------
 * SVC_Battery()
 *
 *     Initialize the Battery Service database.
 *
 * Parameters:
 *     Handler - Pointer to the handler to receive events.
 *
 *     Service - Pointer to service pointer in which handler is to be set.
 *
 *     AdvertUuid - Pointer to the service UUID to be added to the
 *         Connection Manager advertising data.
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
BtStatus SVC_Battery(BtHandlerEx *Handler, GattService **Service, CmgrLeAdvertUuid *AdvertUuid);

/*---------------------------------------------------------------------------
 * SVC_StopBattery()
 *
 *     Deactivate the Battery Service database.
 *
 * Parameters:
 *     Service - Pointer to service pointer to deactivate.
 *
 *     AdvertUuid - Pointer to the service UUID to remove from the
 *         Connection Manager advertising data.
 *
 *     isShutDown - TRUE means the app is shutting down while FALSE means
 *         the service is to be removed.
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed.
 *
 *     BT_STATUS_INVALID_PARM - app, handler or callback in handler is 0
 *         (XA_ERROR_CHECK only).
 *
 */
BtStatus SVC_StopBattery(GattService *Service, CmgrLeAdvertUuid *AdvertUuid,
                                   BOOL isShutdown);

/*---------------------------------------------------------------------------
 * SVC_GetHealthThermTempCCC()
 *
 *     Get the Client Characteristic Config (CCC) values for the Health
 *     Thermometer Temperature characteristic in in Health Thermometer 
 *     Service for the specified remote device.
 *
 * Parameters:
 *     Service - Pointer to Health Thermometer service.
 *
 *     tempCCC - pointer to memory to receive the CCC. 
 *
 *     devAddr - pointer to address of remote device. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - a pointer is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus SVC_GetHealthThermTempCCC(GattService *service, 
                                   GattClientCharConfig *tempCCC, 
                                   BtDevAddr *devAddr);       
/*---------------------------------------------------------------------------
 * SVC_GetHealthThermIntTempCCC()
 *
 *     Get the Client Characteristic Config (CCC) values for the Health
 *     Thermometer Intermediate Temperature characteristic in Health 
 *     Thermometer Service for the specified remote device.
 *
 * Parameters:
 *     Service - Pointer to Health Thermometer service.
 *
 *     intTempCCC - pointer to memory to receive the CCC. 
 *
 *     devAddr - pointer to address of remote device. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - a pointer is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus SVC_GetHealthThermIntTempCCC(GattService *service, 
                                      GattClientCharConfig *intTempCCC, 
                                      BtDevAddr *devAddr);       

/*---------------------------------------------------------------------------
 * SVC_GetHealthThermMeasIntCCC()
 *
 *     Get the Client Characteristic Config (CCC) values for the Health
 *     Thermometer Measurement Interval characteristic in Health Thermometer 
 *     Service for the specified remote device.
 *
 * Parameters:
 *     Service - Pointer to Health Thermometer service.
 *
 *     measIntCCC - pointer to memory to receive the CCC. 
 *
 *     devAddr - pointer to address of remote device. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - a pointer is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus SVC_GetHealthThermMeasIntCCC(GattService *service, 
                                      GattClientCharConfig *measIntCCC, 
                                      BtDevAddr *devAddr);       

/*---------------------------------------------------------------------------
 * SVC_GetBatteryLevelCCC()
 *
 *     Get the Client Characteristic Config (CCC) values for the Battery
 *     Level characteristic in Battery Service for the specified remote 
 *     device.
 *
 * Parameters:
 *     Service - Pointer to Battery service.
 *
 *     battLevelCCC - pointer to memory to receive the CCC. 
 *
 *     devAddr - pointer to address of remote device. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - a pointer is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus SVC_GetBatteryLevelCCC(GattService *service,
                                GattClientCharConfig *battLevelCCC,
                                BtDevAddr *devAddr);

#endif /* __HT_SERVICES_H__ */
