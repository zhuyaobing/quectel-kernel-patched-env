/****************************************************************************
 *
 * File:
 *     an_services.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions for the Services used in the Alert
 *     Notification profile.
 *
 * Created:
 *     January 9, 2013
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

#ifndef __AN_SERVICES_H__
#define __AN_SERVICES_H__


/*---------------------------------------------------------------------------
 * Alert Notification Services layer
 *
 *     Implements and initializes the database components of the Alert
 *     Notification related services. This layer is meant to be an example of 
 *     how one might implement the Alert Notification profile. This code may 
 *     need to be modified by the end user but can act as a base for how it 
 *     could be implemented.
 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * ALERT_NOTIFY_SERVICE_NEW_ALERT_VAL_INDEX constant
 *     The index for the value attribute of the New Alert characteristic of
 *     the Alert Notification service. 
 */
#define ALERT_NOTIFY_SERVICE_NEW_ALERT_VAL_INDEX 4

/*---------------------------------------------------------------------------
 * ALERT_NOTIFY_SERVICE_UNREAD_ALERT_VAL_INDEX constant
 *     The index for the value attribute of the Unread Alert characteristic of
 *     the Alert Notification service. 
 */
#define ALERT_NOTIFY_SERVICE_UNREAD_ALERT_VAL_INDEX 9
 

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GattAlertCategoryId type
 *
 *     The GattAlertCategoryId type indicates the categories of alerts and
 *     messages. It is an enumeration.
 */
typedef U8 GattAlertCategoryId;

/* Simple Alert General text alert or non-text alert */
#define GACATID_SIMPLE        0

/* Email: Alert when Email messages arrive */
#define GACATID_EMAIL         1

/* News: News feeds such as RSS, Atom */
#define GACATID_NEWS          2

/* Call: Incoming call */
#define GACATID_CALL          3

/* Missed call */
#define GACATID_MISSED_CALL   4

/* SMS/MMS: SMS/MMS message arrives */
#define GACATID_SMS_MMS       5

/* Voice mail */
#define GACATID_VOICE_MAIL    6

/* Schedule: Alert occurred on calendar, planner */
#define GACATID_SCHEDULE      7

/* High Prioritized Alert: an alert that should be handled as high priority */
#define GACATID_HIGH_PRIORITY 8

/* Instant Message: Alert for incoming instant messages */
#define GACATID_INSTANT_MSG   9

/* Last alert value */
#define GACATID_OPEN          10

/* The following range is defined by service specification */
#define GACATID_SERV_DEF_FIRST 251
#define GACITD_SERV_DEF_LAST   255

/* All categories for Alert Notify */
#define GACATID_ALERT_NOTIFY_ALL 255

/* End of GattAlertCategoryId */

/*---------------------------------------------------------------------------
 * GattAlertCategoryIdBm1 type
 *
 *     The GattAlertCategoryIdBm1 type indicates the categories of alerts and
 *     messages. It is a bit mask and represents the first octet. The values 
 *     are ORed together.
 */
typedef U8 GattAlertCategoryIdBm1;

/* Simple Alert */
#define GACATID_BM_SIMPLE        0x01

/* Email */
#define GACATID_BM_EMAIL         0x02

/* News: News feeds such as RSS, Atom */
#define GACATID_BM_NEWS          0x04

/* Call: Incoming call */
#define GACATID_BM_CALL          0x08

/* Missed call */
#define GACATID_BM_MISSED_CALL   0x10

/* SMS/MMS: SMS/MMS message arrives */
#define GACATID_BM_SMS_MMS       0x20

/* Voice mail */
#define GACATID_BM_VOICE_MAIL    0x40

/* Schedule: Alert occurred on calendar, planner */
#define GACATID_BM_SCHEDULE      0x80

/* End of GattAlertCategoryIdBm1 */

/*---------------------------------------------------------------------------
 * GattAlertCategoryIdBm2 type
 *
 *     The GattAlertCategoryIdBm2 type indicates the categories of alerts and
 *     messages. It is a bit mask and represents the second octet. The values 
 *     are ORed together.
 */
typedef U8 GattAlertCategoryIdBm2;

/* High Prioritized Alert: an alert that should be handled as high priority */
#define GACATID_BM_HIGH_PRIORITY 0x01

/* Instant Message: Alert for incoming instant messages */
#define GACATID_BM_INSTANT_MSG   0x02

/* End of GattAlertCategoryIdBm2 */

/*---------------------------------------------------------------------------
 * GattAlertNotifyCommand type
 *
 *     The GattAlertNotifyCommand type specifies the command used when
 *     writing to the Alert Notification Control Point.
 */
typedef U8 GattAlertNotifyCommand;

/* Enable New Incoming Alert Notification */
#define GANCMD_ENABLE_NEW_ALERT      0

/* Enable Unread Category Status Notification */
#define GANCMD_ENABLE_UNREAD_ALERT   1

/* Disable New Incoming Alert Notification */
#define GANCMD_DISABLE_NEW_ALERT     2

/* Disable Unread Category Status Notification */
#define GANCMD_DISABLE_UNREAD_ALERT  3

/* Notify New Incoming Alert Immediately */
#define GANCMD_NOTIFY_NEW_ALERT      4

/* Notify New Incoming Alert Immediately */
#define GANCMD_NOTIFY_UNREAD_ALERT   5

/* End of GattAlertNotifyCommand */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SVC_AlertNotification()
 *
 *     Initialize the Alert Notification Service database.
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
BtStatus SVC_AlertNotification(BtHandlerEx *Handler, GattService **Service,
                               CmgrLeAdvertUuid *AdvertUuid);

/*---------------------------------------------------------------------------
 * SVC_StopAlertNotification()
 *
 *     Deactivate the Alert Notification Service database.
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
BtStatus SVC_StopAlertNotification(GattService *Service, CmgrLeAdvertUuid *AdvertUuid, 
                           BOOL isShutdown);


/*---------------------------------------------------------------------------
 * SVC_GetAlertNotifyCategories()
 *
 *     Get the Alert Notification Supported New Alert and Supported Unread 
 *     Alert categories currently stored in the GATT database.
 *
 * Parameters:
 *     Service - Pointer to Alert Notification service.
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
BtStatus SVC_GetAlertNotifyCategories(GattService *service, 
                                      GattAlertCategoryIdBm1 *anCat1, 
                                      GattAlertCategoryIdBm2 *anCat2, 
                                      GattAlertCategoryIdBm1 *unCat1, 
                                      GattAlertCategoryIdBm2 *unCat2);

/*---------------------------------------------------------------------------
 * SVC_SetNewAlertCategories()
 *
 *      Set the Supported New Alert Categories for the Alert Notification 
 *      service in the GATT database.
 *
 * Parameters:
 *     Service - Pointer to Alert Notification service.
 *
 *     cat1 - value for the first category octet. 
 *
 *     cat1 - value for the second category octet. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - a pointer is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus SVC_SetNewAlertCategories(GattService *service, GattAlertCategoryIdBm1 cat1, 
                                    GattAlertCategoryIdBm2 cat2); 

/*---------------------------------------------------------------------------
 * SVC_SetUnreadAlertCategories()
 *
 *      Set the Supported Unread Alert Categories for the Alert Notification 
 *      service in the GATT database.
 *
 * Parameters:
 *     Service - Pointer to Alert Notification service.
 *
 *     cat1 - value for the first category octet. 
 *
 *     cat1 - value for the second category octet. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - a pointer is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus SVC_SetUnreadAlertCategories(GattService *service, GattAlertCategoryIdBm1 cat1, 
                                      GattAlertCategoryIdBm2 cat2); 


/*---------------------------------------------------------------------------
 * SVC_GetAlertNotifyCCC()
 *
 *     Get the Characteristic Client Config (CCC) values for the New Alert and
 *     Unread Alert Status characteristics in the Alert Notification Service for
 *     the specified remote device.
 *
 * Parameters:
 *     Service - Pointer to Alert Notification service.
 *
 *     newAlertCCC - pointer to memory to receive the New Alert CCC. 
 *
 *     unreadAlertCCC - pointer to memory to receive the Unread Alert CCC. 
 *
 *     devAddr - pointer to address of remote device. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - a pointer is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus SVC_GetAlertNotifyCCC(GattService *service, 
                               GattClientCharConfig *newAlertCCC, 
                               GattClientCharConfig *unreadAlertCCC, 
                               BtDevAddr *devAddr);

#endif /* __AN_SERVICES_H__ */
