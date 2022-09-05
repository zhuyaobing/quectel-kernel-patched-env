/****************************************************************************
 *
 * File:
 *     gd_remote_client.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Defines a remote device client connection to the local GATT
 *     server.
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

#ifndef _GD_REMOTE_CLIENT_H__
#define _GD_REMOTE_CLIENT_H__

#include "conmgr.h"
#include "mele.h"
#include "gattserver.h"
#include "conmgrlea.h"
#include "gattdb.h"
#include "gattsecurity.h"

/*---------------------------------------------------------------------------
 * GATT Demo Profile Server Connected Client layer
 *
 *     Implements the connected client protocol portion of a GATT Demo 
 *     profile server application. This is meant to be an example of
 *     how one might implement a GATT based profile using the
 *     GATT API's.
 */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/


typedef struct _GdRemoteClient {
    
    ListEntry         node;  /* Internal use only */

    /* Group: Read only fields */

    /* Represents the remote device which is connected to the 
     * GATT server. It is valid during all events. */
    BtRemoteDevice   *remDev;

    /* Address currently being used by the remote device connected
     * to the GATT server. It is valid for all events. 
     */
    BtDevAddr         devAddr;

    /* Identity address used to index into DDB. This is the "real" address. 
     * Valid for all events. 
     */
    BtDevAddr         identityAddr;

    /* === Internal use only === */
    CmgrHandlerCtx    cmgrHandler;
    BtHandlerEx      *serverHandler;
    BtHandlerEx      *gattHandler;
    GattService      *gsGatt;
    GattServerToken   gattToken;
    GattSecurityToken secToken;
    BOOL              sendNotify;
} GdRemoteClient;

#endif

