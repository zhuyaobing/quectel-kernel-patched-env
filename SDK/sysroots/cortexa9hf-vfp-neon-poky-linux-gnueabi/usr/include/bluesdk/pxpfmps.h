/****************************************************************************
 *
 * File:
 *     pxpfmps.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions for the Server role of the LE 
 *     Proximity and Find ME profiles.
 *
 * Created:
 *     May 18,2012
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

#ifndef __PXPFMPS_H_
#define __PXPFMPS_H_

/*---------------------------------------------------------------------------
 * Proximity/Find Me Profile Server layer
 *
 *     Implements the protocol portion of a Proximity/Find Me profile server
 *     application. This layer is meant to be an example of how one might 
 *     implement the Proximity or Find Me profiles. This code may need 
 *     modified by the end user but can act as a base for how it could be 
 *     implemented. 
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * PxpFmpsEventType type
 *
 *     The Proximity/Find Me profile Server Application Event types are passed 
 *     to the application handler provided in PXPFMPS_Init(). 
 */
typedef BtEventTypeEx PxpFmpsEventType;

/* An incoming connection from a remote device to the Proximity profile
 * server application has occurred. This event is only sent for remote
 * devices that attempt to read or write the link loss alert attribute.
 * "e->remDev" identifies the remote device. In order to "hold" the link
 * up an unused PxpFmpsCon must have been previously provided using the
 * function PXPFMPS_AddListenCon().
 */
#define PXPFMPSE_CONNECT         (BTE_TYPE_BASE_PXPFMPS + 0)

 /* An incoming connection from a remote device to the Proximity profile
  * has occurred and there is no con structure available to handle it.
  * The application is requested to call PXPFMSE_AddListenCon() to
  * add one.
  */
#define PXPFMPSE_NEED_CON        (BTE_TYPE_BASE_PXPFMPS + 1)

/* The connection from a remote device to the proximity app (as described for
 * PXPFMPSE_CONNECT) has been disconnected. This event will be sent no matter
 * how the link was disconnected including for a link loss due to timeout
 */
#define PXPFMPSE_DISCONNECT      (BTE_TYPE_BASE_PXPFMPS + 2)

/* The incoming connection for remote device connect to the proximity profile
 * (as described for PXPFMPSE_CONNECT) has been disconnected unexpectedly
 * due to link loss. An alarm should be sounded based on the value in
 * "alertLevel" field. A PXPFMPSE_DISCONNECT event will also be sent. 
 */
#define PXPFMPSE_LINK_LOSS       (BTE_TYPE_BASE_PXPFMPS + 3)

/* The remote device has told the server to set the immediate alert level
 * to the value in "alertLevel".
 */
#define PXPFMPSE_IMMED_ALERT     (BTE_TYPE_BASE_PXPFMPS + 4)   
/* End of PxpFmpsEventType */

/* Forward reference */
typedef struct _PxpFmpsEvent PxpFmpsEvent;

/*---------------------------------------------------------------------------
 * PxpFmpsEventCallback type
 *
 * A function of this type is called to indicate Proximity/Find Me profile
 * server application events.
 */
typedef void (*PxpFmpsEventCallback)(void *context, PxpFmpsEvent *event);

/* End of PxpFmpsEventCallback */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * PxpFmpsApp structure
 *
 *     The PxpFmpsApp represents the entire unit of what a Proximity/Find Me 
 *     server must contain. The  PxpFmpsApp must be allocated and initialized 
 *     by the application, prior to it being used in any other API's.  The 
 *     PxpFmpsApp structure is initialized by calling PXPFMPSAPP_Init(). 
 */
typedef struct _PxpFmpsApp {

    /* === Internal use only === */

    ListEntry         conList;
    CmgrLeAdvertUuid  mLinkLossServ;
    CmgrLeAdvertUuid  mAlertServ;
    CmgrLeAdvertUuid  mTxPowerServ;
    BtHandlerCtx      meHandler;
    MeLeHandler       meleHandler;
    BtHandlerEx       gattHandler;
#if LE_PROXIMITY_SUPPORT_SDP == XA_ENABLED
    GattSdpRecord     linkLossSdpRec;
    GattSdpRecord     immedAlertSdpRec;
    GattSdpRecord     txPowerSdpRec;
#endif /*LE_PROXIMITY_SUPPORT_SDP */
    GattService      *gsLinkLoss;
    GattService      *gsImmedAlert;
    GattService      *gsTxPower;
    BtHandlerEx      *appHandler;

} PxpFmpsApp;


/*---------------------------------------------------------------------------
 * PxpFmpsCon structure
 *
 *     The PxpFmpsCon is used to manage Proximity connections. When a remote
 *     remote device accesses the alert characteristic value attribute in
 *     the Link Loss service (connects to Proximity) the Proximity profile 
 *     will "hold" the connection so that disconnects issued by other profiles
 *     on the local device will not result in link disconnect. If multiple 
 *     devices connect to the Proximity profile at the same time each remote 
 *     device will need a PxpFmpsCon structure to "hold" the connection. 
 */
typedef struct _PxpFmpsCon {
    ListEntry         node;   /* Internal use only  */

    /* Represents a remote device in which a connection has been made. It
     * is valid during PXPFMPSE_CONNECT and PXPFMPSE_IMMED_ALERT 
     */
    BtRemoteDevice   *remDev;

    /* Address currently being used by the remote device connected to
     * Proximity server. It valid for all events
     */
    BtDevAddr         devAddr;

    /* Identity address of a remote device connected to Proximity server and  
     * is used to index into DDB. This is the "real" address. Valid for all 
     * events.
     */
    BtDevAddr         identityAddr;

    /* Pointer to application structure */
    PxpFmpsApp       *app;

    /* === Internal use only === */
    CmgrHandlerCtx    cmgrHandler;
    GattServerToken   gattToken;
    MeCmdTokenCtx     meCmdToken;
    BOOL              txPowerPend;
    BOOL              txPowerGattReadReq;
    BOOL              txPowerValid;
    U8                meCmdParms[3];
    U8                txPowerResult;

    EvmTimer          discTimer;

} PxpFmpsCon;
 

/*---------------------------------------------------------------------------
 * PxpFmpsEvent structure
 *
 *     Used to pass Proximity/Find Me profile server application events to 
 *     applications.
 */
struct _PxpFmpsEvent {
    /* Inherit from BtEventEx. Note that "e.handler" contains a pointer
     * to the handler which is the CmgrLeAdvertHandler
     */
    BtEventEx        e;

    /* Pointer to the remote device structure identifying the device. Valid
     * during PXPFMPSE_CONNECT.
     */
    BtRemoteDevice  *remDev;

    /* Pointer to connection structure valid during PXPFMPSE_CONNECT, 
     * PXPFMPSE_LINK_LOSS, PXPFMPSE_DISCONNECT. If con is 0 when the
     * event is PXPFMPSE_CONNECT it indicates that there is not connection
     * structure available the application can call PXPFMPS_AddListenCon() to
     * provide a structure before returning from the callback to provide
     * one for this connection. If one is not provided then the link will not
     * be "held" and disconnect and link loss events for this link will not be 
     * sent.
     */
    PxpFmpsCon      *con;

    /* Alert level valid during PXPFMPSE_LINK_LOSS and PXPFMPSE_IMMED_ALERT */
    GattAlertLevel   alertLevel;
};


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * PXPFMPS_Init()
 *
 *     Initialize the Proximity/Find Me profile server application.
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
BtStatus PXPFMPS_Init(PxpFmpsApp *app, BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * PXPFMPS_Deinit()
 *
 *     Deinitialize the Proximity/Find Me profile server application.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     isShutdown - TRUE indicates that the application is being shutdown
 *         FALSE otherwise.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - app, handler or callback in handler is 0 
 *         (XA_ERROR_CHECK only).
 *
 */
BtStatus PXPFMPS_Deinit(PxpFmpsApp *app, BOOL isShutDown);

/*---------------------------------------------------------------------------
 * PXPFMPS_Disconnect()
 *
 *     Disconnects the proximity/FindMe profile server from a remote
 *     device specified by'con'. If the Proximity/Findme server connection
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
BtStatus PXPFMPS_Disconnect(PxpFmpsCon *Con, U16 DelaySeconds);

/*---------------------------------------------------------------------------
 * PXPFMPS_AddListenCon()
 *
 *     Add a PxpFmpsCon structure to the proximity server that is used to
 *     "hold" the link up so that other local profiles and applications
 *     will not cause the link to disconnect when they disconnect from
 *     the remote device. 
 *
 *     The best time to provide a PxpFmpsCon structure is when PXPFMPS_Init
 *     is called to handle the first incoming connection and then when 
 *     the PXPFMPSE_CONNECT event is received to handle the next connection.
 *     PXPFMPS_AddListenCon() can also be used at init time to provide
 *     a number of PxpFmpsCon structures to the proximity server up front.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     con - Pointer to a PxpFmpsCon structure. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed.
 *
 *     BT_STATUS_INVALID_PARM - app or con is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus PXPFMPS_AddListenCon(PxpFmpsApp *app, PxpFmpsCon *con);

/*---------------------------------------------------------------------------
 * PXPFMPS_RemoveListenCon()
 *
 *     Remove a previously added PxpFmpsCon structure. The PxPFmpsCon
 *     structure should not be removed while it is involved in a connection.
 *     Doing so will most likely result in the link being disconnected.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     con - Pointer to a PxpFmpsCon structure. 
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
BtStatus PXPFMPS_RemoveListenCon(PxpFmpsApp *app, PxpFmpsCon *con);


#endif /* __PXPFMPS_H_ */
