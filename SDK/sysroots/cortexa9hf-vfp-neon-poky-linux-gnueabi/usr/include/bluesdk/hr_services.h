/****************************************************************************
 *
 * File:
 *     hr_services.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions for the Services used in the LE
 *     Heart Rate profile.
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

#ifndef __HR_SERVICES_H__
#define __HR_SERVICES_H__


/*---------------------------------------------------------------------------
 * Heart Rate Services layer
 *
 *     Implements and initializes the database components of the Heart Rate
 *     related services. This layer is meant to be an example of how one might 
 *     implement the heart rate profile. This code may need to be modified 
 *     by the end user but can act as a base for how it could be implemented.
 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/* Maximum number of RR-intervals that can exist in a Heart Rate Measurment.
 * Other factors such as size of Heart Rate Measurement value in includsion
 * of the Energy Expended feild effect the actual number of RR-intervals
 * that can be put into a Heart Rate measurment.
 */
#define GATT_HR_MAX_NUM_RR_INTERVALS  ((GATT_MAX_MTU - 5)/2)


/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GattBodySensorLocaton type
 *
 *     The GattBodySensorLocation type indicates the location of a sensor
 *     (e.g. heart rate) on the body in which a measurement is made.
 */
typedef U8 GattBodySensorLocaton;

#define GATT_BODY_SENSOR_LOC_OTHER          0
#define GATT_BODY_SENSOR_LOC_CHEST          1
#define GATT_BODY_SENSOR_LOC_WRIST          2
#define GATT_BODY_SENSOR_LOC_FINGER         3
#define GATT_BODY_SENSOR_LOC_HAND           4
#define GATT_BODY_SENSOR_LOC_EAR_LOBE       5
#define GATT_BODY_SENSOR_LOC_FOOT           6
#define GATT_BODY_SENSOR_LOC_RESERVED       7 /* 7 - 255 are reserved for future use */

/* End of GattBodySensorLocaton */


/*---------------------------------------------------------------------------
 * GattHeartRateMeasurementFlags type
 *
 *     The GattHeartRateMeasurementFlags type indicates the format and 
 *     presence of information in the Heart Rate Measurement value. THe
 *     values are ORed together
 */
typedef U8 GattHeartRateMeasurementFlags;

/* If set the size of the Heart Rate Measurement value is U16 otherwise
 * it is U8
 */
#define GATTHRMF_HRM_FORMAT_U16        0x01

/* Set to 1 if sensor is in contact with the skin otherwise
 * it is 0. This field is only valid if the sensor contact feature
 * exists.
 */
#define GATTHRMF_SENSOR_CONTACT        0x02

/* Set to 1 if sensor contact features exists otherwise is set to 0 */
#define GATTHRMF_SENSOR_CONTACT_EXISTS 0x04

/* Set to 1 if the energy expended field is present in the heart rate
 * measurement value.
 */
#define GATTHRMF_ENERGY_EXP_EXISTS    0x08

/* Set 1 if the RR-interval values are present in the Heart Rate
 * measurement value.
 */
#define GATTHRMF_RR_INTERVAL_EXISTS   0x10


/* End of GattHeartRateMeasurementFlags */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GattHrMeasurement structure
 *
 *     The GattHrMeasurement structure contains all information related to a
 *     Heart Rate Measurment transaction.
 */
typedef struct _GattHrMeasurement
{
    /* Format of the data */
    GattHeartRateMeasurementFlags flags;

    /* Heart Rate */
    U16    hrVal;

    /* Energy Expended field in kilo Joules - only valid if GATTHRMF_ENERGY_EXP_EXISTS
     * is set in flags.
     */
    U16     energyExp;

    /* RR interval Field - only valid if GATTHRMF_RR_INTERVAL_EXISTS is set in 
     * flags. The 'size' field indicates number of RR items in the array.
     */
    U16             numRR;
    U16             rr[GATT_HR_MAX_NUM_RR_INTERVALS];
} GattHrMeasurement;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SVC_HeartRate()
 *
 *     Initialize the Heart Rate Service database.
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
BtStatus SVC_HeartRate(BtHandlerEx *Handler, GattService **Service,
                       CmgrLeAdvertUuid *AdvertUuid);

/*---------------------------------------------------------------------------
 * SVC_StopHeartRate()
 *
 *     Deactivate the Heart Rate Service database.
 *
 * Parameters:
 *     Service - Pointer to service pointer to deactivate.
 *
 *     AdvertUuid - Pointer to the service UUID to remove from the
 *         Connection Manager advertising data.
 *
 *     isShutdown - if TRUE the application is being shutdown and the service should
 *         not removed otherwise the service should be removed.
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
BtStatus SVC_StopHeartRate(GattService *Service, CmgrLeAdvertUuid *AdvertUuid, 
                           BOOL isShutdown);


/*---------------------------------------------------------------------------
 * SVC_GetHeartRateCCC()
 *
 *     Get the Client Characteristic Config (CCC) values for the Heart Rate
 *     Measurement characteristic in Heart Rate Service for the specified 
 *     remote device.
 *
 * Parameters:
 *     Service - Pointer to Alert Notification service.
 *
 *     heartRateCCC - pointer to memory to receive the Heart Rate CCC. 
 *
 *     devAddr - pointer to address of remote device. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - a pointer is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus SVC_GetHeartRateCCC(GattService *service, 
                               GattClientCharConfig *heartRateCCC, 
                               BtDevAddr *devAddr);       

#endif /* __HR_SERVICES_H__ */
