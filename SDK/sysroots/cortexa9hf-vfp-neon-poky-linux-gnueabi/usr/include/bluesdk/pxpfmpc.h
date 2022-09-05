/****************************************************************************
 *
 * File:
 *     pxpfmpc.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions for the Client role of the LE 
 *     Proximity and Find ME profiles.
 *
 * Created:
 *     May 22,2012
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
#include "gattsecurity.h"

#ifndef __PXPFMPC_H_
#define __PXPFMPC_H_

/*---------------------------------------------------------------------------
 * Proximity/Find Me Profile Client layer
 *
 *     Implements the protocol portion of a Proximity/Find Me profile client
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
 * PxpFmpcEventType type
 *
 *     The Proximity/Find Me profile Client Application Event types are passed 
 *     to the application handler provided in PXPFMPC_Init(). 
 */
typedef BtEventTypeEx PxpFmpcEventType;

/* A connection request has completed. The "e.status" field indicates success
 * or failure of the operation. "app->remDev" identifies the remote device.
 */
#define PXPFMPCE_CONNECT_COMPLETE    (BTE_TYPE_BASE_PXPFMPC + 0)

/* The connection has been disconnected. */
#define PXPFMPCE_DISCONNECT          (BTE_TYPE_BASE_PXPFMPC + 1)

/* The RSSI value has changed or an error has occurred in trying
 * get RSSI and the process is stopped. The "e.status" field indicates
 * if the RSSI is valid or not. If "e.status" is not BT_STATUS_SUCCESS
 * the process for reading RSSI has been stopped.
 */
#define PXPFMPCE_RSSI                (BTE_TYPE_BASE_PXPFMPC + 2)

/* Writing the Link Loss alert value has completed. The "e.status" field
 * indicates success or failure.
 */
#define PXPFMPCE_WRITE_LINK_LOSS     (BTE_TYPE_BASE_PXPFMPC + 3)

/* Writing the Immediate Alert value has completed. */
#define PXPFMPCE_WRITE_IMMED_ALERT   (BTE_TYPE_BASE_PXPFMPC + 4)

/* The connection to the remote device has been disconnected unexpectedly
 * due to link loss. A PXPFMPCE_DISCONNECT event will also be sent. 
 */
#define PXPFMPCE_LINK_LOSS           (BTE_TYPE_BASE_PXPFMPC + 5)

/* End of PxpFmpcEventType */

/* Forward reference */
typedef struct _PxpFmpcEvent PxpFmpcEvent;

/*---------------------------------------------------------------------------
 * PxpFmpcEventCallback type
 *
 * A function of this type is called to indicate Proximity/Find Me profile
 * client application events.
 */
typedef void (*PxpFmpcEventCallback)(void *context, PxpFmpcEvent *event);

/* End of PxpFmpcEventCallback */

/*---------------------------------------------------------------------------
 * PxpFmpcServiceFlags type
 *
 * The PxpFmpcServiceFlags type indicates which Proximity/Find Me services are
 * available on the remote device along with any important characteristics.  
 * The flags are ORed together.
 */
typedef U8 PxpFmpcServiceFlags;

/* The Link Loss service is present on the remote device */
#define PXPFMPCSF_LINK_LOSS       0x01

/* The Immediate Alert Service is present on the remote device */
#define PXPFMPCSF_IMMED_ALERT     0x02

/* The TX power service is present on the remote device */
#define PXPFMPCSF_TX_POWER        0x04

/* The Link Loss alert level has been retrieved from the remote device and currently
 * is stored in the application structure
 */
#define PXPFMPCSF_LINK_LOSS_LEVEL 0x08

/* The Tx Power value has been retrieved from the remote device and currently
 * is stored in the application structure
 */
#define PXPFMPCSF_TX_POWER_VAL    0x10

/* All the services */
#define PXPFMPCSF_ALL_SERVICES    (PXPFMPCSF_LINK_LOSS | PXPFMPCSF_IMMED_ALERT | PXPFMPCSF_TX_POWER)
/* End of PxpFmpcServiceFlags */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * PxpFmpcApp structure
 *
 *     The PxpFmpcApp represents the entire unit of what a Proximity/Find Me 
 *     client application must contain. The PxpFmpcApp must be allocated and 
 *     initialized by the application, prior to it being used in any other 
 *     API's.  The PxpFmpcApp structure is initialized by calling 
 *     PXPFMPCAPP_Init(). 
 */
typedef struct _PxpFmpcApp {

    /* Group: Read only fields */

    /* Represents the remote device in which a connection has been made. It
     * is valid during PXPFMPSE_CONNECT and PXPFMPSE_IMMED_ALERT 
     */
    BtRemoteDevice      *remDev;

    /* Address currently being used by the remote device connected to
     * the GATT server. It valid for all events
     */
    BtDevAddr            devAddr;

    /* Identity address used to index into DDB. This is the "real" address. Valid
     * for all events.
     */
    BtDevAddr            identityAddr;

    /* Current RSSI level */
    S32                  rssi;

    /* Indicates which services and values are present on the remote device.
     * Valid for PXPFMPCE_CONNECT_COMPLETE event
     */
    PxpFmpcServiceFlags  flags;

    /* The current link loss alert level on the remote device. This only valid 
    * if PXPFMPCSF_LINK_LOSS_LEVEL is set in 'flags'.
     */
    GattAlertLevel       linkLossAlertLevel;

    /* The current TX power level on the remote device. This is only valid
     * if PXPFMPCSF_TX_POWER_VAL is set in 'flags'.
     */
    S32                  txPowerVal;
    /* === Internal use only === */

    GattClientToken   mToken;
    GattSecurityToken secToken;
    MeCmdTokenCtx     meCmdToken;
    BtHandlerCtx      meHandler;
    CmgrHandlerCtx    cmgrHandler;
    MeLeHandler       meleHandler;
    GattClientToken   gattToken;
    EvmTimer          timer;
    BtHandlerEx      *appHandler;
    void             *linkLossServRef;
    void             *immedAlertServRef;
    void             *txPowerServRef;
    BtEventTypeEx     eventType;
    U16               linkLossValHandle;
    U16               immedAlertValHandle;
    U16               txPowerValHandle;
    BOOL              gattConnected;
    U8                state;
    U8                retryState;
    U8                rssiState;
    U8                writeFlags;
    U8                linkLossLevel;
    U8                immedAlertLevel;
    U8                meCmdParms[3];

} PxpFmpcApp;


/*---------------------------------------------------------------------------
 * PxpFmpcEvent structure
 *
 *     Used to pass Proximity/Find Me profile client application events to 
 *     the applications.
 */
struct _PxpFmpcEvent {
    /* Inherit from BtEventEx. Note that "e.handler" contains a pointer
     * to the handler which is the CmgrLeAdvertHandler
     */
    BtEventEx                   e;

    /* Pointer to application structure */
    PxpFmpcApp                 *app;

};

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * PXPFMPC_Init()
 *
 *     Initialize the Proximity/Find Me profile client application.
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
BtStatus PXPFMPC_Init(PxpFmpcApp *app, BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * PXPFMPC_Deinit()
 *
 *     Deinitialize the Proximity/Find Me profile client application.
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
BtStatus PXPFMPC_Deinit(PxpFmpcApp *app);

/*---------------------------------------------------------------------------
 * PXPFMPC_Connect()
 *
 *     Initiate a connection to a remote device. This includes creating an
 *     ACL connection if needed then a GATT connection. As well as
 *     finding the Link Loss, Immediate Alert and Tx Power services
 *     and current values for Link Loss and Tx Power.
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
 *         PXPFMPCE_CONNECT_COMPLETE event will be sent to the handler when
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
BtStatus PXPFMPC_Connect(PxpFmpcApp *app, BtDevAddr *devAddr);

/*---------------------------------------------------------------------------
 * PXPFMPC_Disconnect()
 *
 *     Cancel a connection attempt or Disconnect an existing connection to a 
 *     remote device. This will also stop the reading RSSI process if it is 
 *     active. 
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request to cancel an existing connection attempt 
 *         or write attempt is started. A PXPFMPCE_CONNECT event with 
 *         "e.status" set to BT_STATUS_CANCELLED will be sent to the handler 
 *         if a connection attempt was canceled. A PXPFMPCSF_LINK_LOSS_LEVEL 
 *         and/or PXPFMPCSF_TX_POWER_VAL with "e.status" set to 
 *         BT_STATUS_CANCELLED will be sent if write operations are in 
 *         progress.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending disconnect is in 
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus PXPFMPC_Disconnect(PxpFmpcApp *app);

/*---------------------------------------------------------------------------
 * PXPFMPC_StartRssi()
 *
 *     Start a process for reading RSSI. When the first time RSSI is read and
 *     any time it changes a PXPFMPCE_RSSI event will be sent to the
 *     handler provided to PXPFMPC_Init(). A connection to a remote device must
 *     exist.
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request successfully started. 
 *
 *     BT_STATUS_FAILED - operation failed.  
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there
 *         is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a connection attempt
 *         is in progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus PXPFMPC_StartRssi(PxpFmpcApp *app);

/*---------------------------------------------------------------------------
 * PXPFMPC_StopRssi()
 *
 *     Stop the process for reading RSSI. 
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - RSSI operation successfully stopped. 
 *
 *     BT_STATUS_FAILED - operation failed because RSSI operation
 *        is not active.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus PXPFMPC_StopRssi(PxpFmpcApp *app);

/*---------------------------------------------------------------------------
 * PXPFMPC_WriteLinkLossAlert()
 *
 *     Write the given value to the remote device's Link Loss Alert
 *     level characteristic. 
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     level - level to write.
 *
 * Returns:
 *     BT_STATUS_PENDING - request successfully started. When
 *         complete the PXPFMPCSF_LINK_LOSS_LEVEL event will be sent
 *         to the handler.
 *
 *     BT_STATUS_FAILED - operation failed because Link Loss service or
 *         Alert Level characteristic does not exist.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there
 *         is no connection.
 *
 *     BT_STATUS_IN_PROGRESS - operation failed because a Write
 *         Link Loss operation is already in progress.
 *
 *     BT_STATUS_BUSY - operation failed because a connection attempt
 *         or disconnect is in progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus PXPFMPC_WriteLinkLossAlert(PxpFmpcApp *app, GattAlertLevel level);

/*---------------------------------------------------------------------------
 * PXPFMPC_WriteImmedAlert()
 *
 *     Write the given value to the remote device's Immediate Alert
 *     Alert level characteristic. 
 *
 * Parameters:
 *     app - Pointer to a structure that represents the application.
 *
 *     level- level to write.
 *
 * Returns:
 *     BT_STATUS_PENDING - request successfully started. When
 *         complete the PXPFMPCE_WRITE_IMMED_ALERT event will be sent
 *         to the handler.
 *
 *     BT_STATUS_FAILED - operation failed because the Immediate Alert
 *         service or Alert level characteristic does not exist. 
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there
 *         is no connection.
 *
 *     BT_STATUS_IN_PROGRESS - operation failed because a Write
 *         Immediate Alert operation is already in progress.
 *
 *     BT_STATUS_BUSY - operation failed because a connection attempt
 *         or disconnect is in progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus PXPFMPC_WriteImmedAlert(PxpFmpcApp *app, GattAlertLevel level);


#endif /* __PXPFMPC_H_ */
