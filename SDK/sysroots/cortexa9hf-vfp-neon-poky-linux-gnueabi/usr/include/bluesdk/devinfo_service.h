/****************************************************************************
 *
 * File:
 *     devinfo_service.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions and API's for the 
 *     Device Information Service.
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

#ifndef __DEVINFO_SERVICE_H__
#define __DEVINFO_SERVICE_H__

/*---------------------------------------------------------------------------
 * Device Information Service layer
 *
 *     Implements and initializes the database components of the Device
 *     Information Service.
 *
 */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GattDevInfo structure
 *
 *     The GattDevInfo structure contains fields for the Device Information
 *     service.
 */
typedef struct _GattDevInfo {
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
} GattDevInfo;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SVC_DeviceInformation()
 *
 *     Initialize the Device Information Service and add it to the GATT 
 *     database.
 *
 * Parameters:
 *     Service - Pointer to service pointer in which will receive a pointer
 *        to the service. If set to 0 the service is not returned.  
 *
 *     Info - Pointer to DevInfo structure initialized with valid parameters.
 *
 *     AdvertUuid - Pointer to the service UUID to be added to the
 *         Connection Manager advertising data. If set to 0 the service
 *         UUID is not added.
 *
 *     sdpRecord - pointer to an structure to be used for registering
 *         the service into SDP. The structure must be persistent until
 *         SVC_StopDeviceInformation() is called. If set to 0 the service
 *         is not registered with SDP.
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
BtStatus SVC_DeviceInformation(GattService **Service, GattDevInfo *Info,
                               CmgrLeAdvertUuid *AdvertUuid, 
                               GattSdpRecord *sdpRecord);

/*---------------------------------------------------------------------------
 * SVC_StopDeviceInformation()
 *
 *     Deactivate the Device Information Service.
 *
 * Parameters:
 *     AdvertUuid - Pointer to the service UUID that was added in the call to
 *         SVC_DeviceInformation(). If 0 then the AdvertUuid is not removed. 
 *
 *     sdpRecord - pointer to the structure passed to SVC_DeviceInformation().
 *         If 0 then sdpRecord is not removed form SDP. 
 *
 *     isShutDown - TRUE means the app is shutting down while FALSE means
 *         the service is to be removed.
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
BtStatus SVC_StopDeviceInformation(CmgrLeAdvertUuid *AdvertUuid, 
                                   GattSdpRecord *sdpRecord,
                                   BOOL isShutdown);


#endif /* __DEVINFO_SERVICE_H__ */
