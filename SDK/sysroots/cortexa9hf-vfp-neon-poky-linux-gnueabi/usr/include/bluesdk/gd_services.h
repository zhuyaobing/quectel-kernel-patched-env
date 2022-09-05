/****************************************************************************
 *
 * File:
 *     gd_services.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions for a test GATT service.
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

#ifndef __GD_SERVICES_H__
#define __GD_SERVICES_H__


/*---------------------------------------------------------------------------
 * GATT Demo Services layer
 *
 *     Implements and initializes the database components of a test GAP and  
 *     GATT service layer which can be used for PTS testing.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 * DbCacheRef structure
 *
 *     The DbCacheRef structure provides a way to keep a list of references
 *     to cached GATT items (services, characteristics, etc.). 
 */

typedef struct _DbCacheRef {
    ListEntry   node;
    void       *ref;
    U16         sh;  /* Start handle 0--*/
    U16         evh; /* End handle or Value Handle */
} DbCacheRef;


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SVC_GAP()
 *
 *     Initialize the GAP test service database.
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
BtStatus SVC_GAP(BtHandlerEx *Handler, GattService **Service, CmgrLeAdvertUuid *AdvertUuid, U16 StartHandle);

/*---------------------------------------------------------------------------
 * SVC_GATT()
 *
 *     Initialize the GATT test service database.
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
BtStatus SVC_GATT(BtHandlerEx *Handler, GattService **Service, CmgrLeAdvertUuid *AdvertUuid, U16 StartHandle);

/*---------------------------------------------------------------------------
 * SVC_StopService()
 *
 *     Deactivate a specific GATT test service.
 *
 * Parameters:
 *     Service - Pointer to service pointer to deactivate.
 *
 *     AdvertUuid - Pointer to the service UUID to remove from the
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
BtStatus SVC_StopService(GattService *Service, CmgrLeAdvertUuid *AdvertUuid);

#endif /* __GD_SERVICES_H__ */
