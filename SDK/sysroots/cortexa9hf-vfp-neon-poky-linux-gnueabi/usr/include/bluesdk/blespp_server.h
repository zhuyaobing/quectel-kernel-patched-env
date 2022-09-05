/****************************************************************************
 *
 * File:
 *     blespp_server.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions for the Server role of the LE
 *     Spp Service as serial communication.
 *
 * Copyright 2013-2017 OpenSynergy GmbH.
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
#ifndef __BLESPP_SERVER_H
#define __BLESPP_SERVER_H

#if NUM_LE_DEVICES > 0
#if BTSOCK_LEGATT_ENABLED == XA_ENABLED

#include "conmgr.h"
#include "mele.h"
#include "gattserver.h"
#include "conmgrlea.h"
#include "gattsecurity.h"

/*---------------------------------------------------------------------------
 * SPP Profile Server layer
 *
 *     Implements the protocol portion of a Spp profile server
 *     application. This layer is meant to be an example of how one might
 *     implement serial data communication profile. This code may need
 *     modified by the end user but can act as a base for how it could be
 *     implemented.
 */

#define SPP_NUMHANDLESMAX                8 
#define SPPS_RX_DATA_MAX                 512
#define SPPS_TX_DATA_MAX                 512

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/
typedef struct {
    U16 len;
    U8  data[SPPS_RX_DATA_MAX];
} SppsRxData;

/*---------------------------------------------------------------------------
 * SppsEventType type
 *
 *     The DataExchange profile Server Application Event types are passed
 *     to the application handler provided in SppServer_Init().
 */
typedef BtEventTypeEx SppsEventType;

/* An incoming connection from a remote device to the Data Exchange
 * server application has occurred. This event is only sent for remote
 * devices that attempt to read or write raw datas.
 * "e->remDev" identifies the remote device. In order to "hold" the link
 * up an unused SppsCon must have been previously provided using the
 * function SppServer_AddListenCon().
 */
#define SPPSE_CONNECT            (BTE_TYPE_BASE_SPPS + 0)

/* The connection from a remote device to the dataexchange app (as described for
 * for SppServer_CONNECT) has been disconnected. This event will be sent no matter
 * how the link was disconnected incuding for a link loss due to timeout
 */
#define SPPSE_DISCONNECT         (BTE_TYPE_BASE_SPPS + 1)

/* The incoming data from remote device has arrived in the received handle */
#define SPPSE_DATA_IND           (BTE_TYPE_BASE_SPPS + 2)

/* The sending data notification is complete.  */
#define SPPSE_DATA_NOTIFY_COMPLETE     (BTE_TYPE_BASE_SPPS + 3)

/* The sending data indication is complete.  */
#define SPPSE_DATA_INDICATE_COMPLETE   (BTE_TYPE_BASE_SPPS + 4)

/* GATT TX Check event  */
#define SPPSE_CHECKTX            (BTE_TYPE_BASE_SPPS + 5)

/* No more connection available  */
#define SPPSE_NEED_CON           (BTE_TYPE_BASE_SPPS + 6)

/* No more connection available  */
#define SPPSE_INDICATE_ENABLE    (BTE_TYPE_BASE_SPPS + 7)

/* No more connection available  */
#define SPPSE_NOTIFY_ENABLE      (BTE_TYPE_BASE_SPPS + 8)

/* End of SppsEventType */

/* Forward reference */
typedef struct _SppsEvent SppsEvent;

/*---------------------------------------------------------------------------
 * SppsEventCallback type
 *
 * A function of this type is called to indicate Data Exchange profile
 * server application events.
 */
typedef void(*SppsEventCallback)(void *context, SppsEvent *event);

/* End of SppsEventCallback */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SppsApp structure
 *
 *     The SppsApp represents the entire unit of what a Data Exchange
 *     server must contain. The  SppsApp must be allocated and initialized
 *     by the application, prior to it being used in any other API's.  The
 *     SppsApp structure is initialized by calling SppServer_Init().
 */
typedef struct _SppsApp {

    /* === Internal use only === */
    ListEntry           conList;
    CmgrLeAdvertUuid    gsDataExchangeServ;
    BtHandlerEx         gattHandler;
#if LE_SPPS_SUPPORT_SDP == XA_ENABLED
    GattSdpRecord       SppSdpRec;
#endif /*LE_SPPS_SUPPORT_SDP */
    GattService         *gsDataExchange;
    BtHandlerEx         *appHandler;
} SppsApp;

/*---------------------------------------------------------------------------
 * SppsCon structure
 *
 *     The SppsCon is used to manage Data Exchange connections. When a remote
 *     remote device accesses rx data value attribute in the Data Exchange service
 *     (connects to Data Exchange) the Data Exchange profile
 *     will "hold" the connection so that disconnects issued by other profiles
 *     on the local device will not result in link disconnect. If multiple
 *     devices connect to the Data Exchange profile at the same time each remote
 *     device will need a SppsCon structure to "hold" the connection.
 */
typedef struct _SppsCon {
    ListEntry         node;   /* Internal use only  */

    /* Represents a remote device in which a connection has been made. It
     * is valid during SPPSE_CONNECT and SPPSE_DATA_IND
     */
    BtRemoteDevice   *remDev;

    /* Address currently being used by the remote device connected to
     * Proximity server. It valid for all events
     */
    BtDevAddr         devAddr;

    /* Identity address of a remote device connected to DataExchange serverand
     * is used to index into DDB. This is the "real" address. Valid for all
     * events.
     */
    BtDevAddr         identityAddr;

    /* Set to TRUE if the Client Characteristic Configuration value for the
     * data exchange TX is enabled for indications. It has the current setting.
     */
    BOOL                Spp_txIndicateEnabled;

    /* Pointer to application structure */
    SppsApp       *app;

    /* === Internal use only === */
    CmgrHandlerCtx      cmgrHandler;
    GattServerToken     gattToken;
    GattSecurityToken   secToken;
    BOOL                sendNotify;
    BOOL                notifyIndicateSent;

} SppsCon;

/*---------------------------------------------------------------------------
 * SppsEvent structure
 *
 *     Used to pass Data Exchange profile server application events to
 *     applications.
 */
struct _SppsEvent {
    /* Inherit from BtEventEx. Note that "e.handler" contains a pointer
     * to the handler which is the CmgrLeAdvertHandler
     */
    BtEventEx        e;

    /* Pointer to the remote device structure identifying the device. Valid
     * during SPPSE_CONNECT.
     */
    BtRemoteDevice  *remDev;

    /* Pointer to connection structure valid during SPPSE_CONNECT,
     * SPPSE_DISCONNECT. If con is 0 when the event is SPPSE_CONNECT
     * it indicates that there is not connection structure available
     * the application can call SppServer_AddListenCon() to
     * provide a structure before returning from the callback to provide
     * one for this connection. If one is not provided then the link will not
     * be "held" and disconnect and link loss events for this link will not be
     * sent.
     */
    SppsCon      *con;

    /* Pointer to application structure valid during all events */
    SppsApp     *app;


    /* valid during SPPSE_DATA_IND */
    SppsRxData   rxdata;
};

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SppServer_Init()
 *
 *     Initialize the Data Exchange profile server application.
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
 *     BT_STATUS_INVALID_PARM - app, appHandler or callback in appHandler is 0
 *     (XA_ERROR_CHECK only).
 *
 */
BtStatus SppServer_Init(SppsApp *app, BtHandlerEx *appHandler);

/*---------------------------------------------------------------------------
 * SppServer_Deinit()
 *
 *     Deinitialize the Data Exchange profile server application.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     isShutdown - TRUE indicates that the application is being shutdown
 *                  FALSE otherwise.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - app, handler or callback in handler is 0
 *     (XA_ERROR_CHECK only).
 *
 */
BtStatus SppServer_Deinit(SppsApp *app, BOOL isShutDown);

/*---------------------------------------------------------------------------
 * SppServer_AddListenCon()
 *
 *     Add a PxpFmpsCon structure to the proximity server that is used to
 *     "hold" the link up so that other local profiles and applications
 *     will not cause the link to disconnect when they disconnect from
 *     the remote device.
 *
 *     The best time to provide a PxpFmpsCon structure is when PXPFMPS_Init
 *     is called to handle the first incoming connection and then when
 *     the PXPFMPSE_CONNECT event is received to handle the next connection.
 *     FXPFMPS_AddListenCon() can also be used at init time to provide
 *     a number of PxpFmpsCon structures to the proximity server up front.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     con - Pointer to a SppsCon structure.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed.
 *
 *     BT_STATUS_INVALID_PARM - app or con is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus SppServer_AddListenCon(SppsApp *app, SppsCon *con);

/*---------------------------------------------------------------------------
 * SppServer_RemoveListenCon()
 *
 *     Remove a previously added SppsCon structure. The SppsCon
 *     structure should not be removed while it is involved in a connection.
 *     Doing so will most likely result in the link being disconnected.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     con - Pointer to a SppsCon structure.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed because structure has not been
 *                        previously added.
 *
 *     BT_STATUS_INVALID_PARM - app or con is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus SppServer_RemoveListenCon(SppsApp *app, SppsCon *con);

/*---------------------------------------------------------------------------
 * SppServer_SendData()
 *
 *     Send data to the client.
 *
 * Parameters:
 *
 *     con - Pointer to a SppsCon structure.
 *
 *     txData - Pointer to the data to be sent
 *
 *     txDataLen - Length of the data to be sent
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed because structure has not been
 *                        previously added.
 *
 *     BT_STATUS_INVALID_PARM - app or con is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus SppServer_SendData(SppsCon *con,
                            U8 *txData, U16 txDataLen);

#endif /* BTSOCK_LEGATT_ENABLED == XA_ENABLED  */
#endif /* NUM_LE_DEVICES > 0 */


#endif /* __BLESPP_SERVER_H */
