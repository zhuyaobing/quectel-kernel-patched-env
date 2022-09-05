/****************************************************************************
 *
 * File:
 *     gd_client.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Defines a fake GATT client.
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
 * trademark and other intellectual property rights*
 ****************************************************************************/

#ifndef _GD_CLIENT_H__
#define _GD_CLIENT_H__

#include "mele.h"
#include "gattclient.h"
#include "conmgrlea.h"
#include "gattdb.h"
#include "gd_services.h"
#include "gattsecurity.h"

/*---------------------------------------------------------------------------
 * GATT Demo Profile Client layer
 *
 *     Implements the protocol portion of a GATT Demo profile
 *     client application. This GATT Demo profile is an example
 *     of how to use the GATT API's.
 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

#define GD_NUM_REFERENCES     1024


/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GdClientEventEventType type
 *
 *     The GATT Demo profile Client Application Event types are
 *     passed to the application handler provided in GDCLIENT_Init().
 */
typedef BtEventTypeEx GdClientEventEventType;

/* A connection request has completed. The "e.status" field indicates success
 * or failure of the operation. "app->remDev" identifies the remote device.
 */
#define GDCLIENT_CONNECT_COMPLETE           (BTE_TYPE_BASE_GDCLIENT + 0)

/* The connection has been disconnected. */
#define GDCLIENT_DISCONNECT                 (BTE_TYPE_BASE_GDCLIENT + 1)

/* The indicate a Primary Service to the application. */
#define GDCLIENT_PRIMARY_SERVICE            (BTE_TYPE_BASE_GDCLIENT + 2)

/* The indicate a Primary Service to the application. */
#define GDCLIENT_PRIMARY_SERVICE_UUID       (BTE_TYPE_BASE_GDCLIENT + 3)

/* The indicate an Included Service to the application. */
#define GDCLIENT_INCLUDED_SERVICE           (BTE_TYPE_BASE_GDCLIENT + 4)

/* The all Included Services have been found. */
#define GDCLIENT_INC_SERVICE_COMPLETE       (BTE_TYPE_BASE_GDCLIENT + 5)

/* The indicate a Characteristic to the application. */
#define GDCLIENT_CHARACTERISTIC             (BTE_TYPE_BASE_GDCLIENT + 6)

/* The all Characteristics have been found. */
#define GDCLIENT_CHARACTERISTIC_COMPLETE    (BTE_TYPE_BASE_GDCLIENT + 7)

/* The indicate a Characteristic to the application. */
#define GDCLIENT_CHARACTERISTIC_UUID        (BTE_TYPE_BASE_GDCLIENT + 8)

/* The all Characteristics have been found. */
#define GDCLIENT_CHARACTER_UUID_COMPLETE    (BTE_TYPE_BASE_GDCLIENT + 9)

/* The indicate a descriptor to the application. */
#define GDCLIENT_CHAR_DESCRIPTOR            (BTE_TYPE_BASE_GDCLIENT + 10)

/* The all descriptors have been found. */
#define GDCLIENT_CHAR_DESCRIPTOR_COMPLETE   (BTE_TYPE_BASE_GDCLIENT + 11)

/* The indicate a Characteristic Value to the application. */
#define GDCLIENT_VALUE                      (BTE_TYPE_BASE_GDCLIENT + 12)

/* The indicate multiple Characteristic Values to the application. */
#define GDCLIENT_MULTI_VALUE                (BTE_TYPE_BASE_GDCLIENT + 13)

/* The pending write is complete. */
#define GDCLIENT_WRITE_COMPLETE             (BTE_TYPE_BASE_GDCLIENT + 14)

/* Indicate a security error to the application. The "errCode" field
 * in the event contains the security error.
 */
#define GDCLIENT_SECURITY_ERROR             (BTE_TYPE_BASE_GDCLIENT + 15)

/* Indicate a security operation is complete */
#define GDCLIENT_SECURITY_COMPLETE          (BTE_TYPE_BASE_GDCLIENT + 16)

/* End of GdClientEventEventType */

/* Forward reference */
typedef struct _GdClientEvent GdClientEvent;

/*---------------------------------------------------------------------------
 * GdClientEventEventCallback type
 *
 * A function of this type is called to indicate GATT Demo profile
 * Client application events.
 */
typedef void (*GdClientEventEventCallback)(void *context, GdClientEvent *event);

/* End of GdClientEventEventCallback */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 * GdClientApp structure
 *
 *     The GdClientApp represents the entire unit of a general purpose GATT
 *     client application used to browse, read and write to any remote GATT
 *     database. The GdClientApp must be allocated and initialized by the
 *     application, prior to it being used in any other API's. The
 *     GdClientApp structure is initialized by calling GDCLIENT_Init().
 */
typedef struct _GdClientApp {
    /* Represents the remote device in which a connection has been made. It
     * is valid during the GDCLIENT_CONNECT_COMPLETE event
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

    /* Parameters passed in or out for APIs or Events respectively.
       See the API or Event documentation for specific details.
     */
    U16                     handle;
    U16                     incHandle;
    U16                     endHandle;
    U16                     valHandle;
    U16                     offset;
    U8                      props;
    GattUuid                uuid;
    AttValueType            valType;
    AttValue                value;
    U16                    *handles;
    U16                     num;

    U16                          itemCount;
    GattClientWriteReliableItem *itemArray;

    /* Error used for GDCLIENT_SECURITY_ERROR event. Also used
     * when calling GDCLIENT_FixGattSecurityError()
     */
    AttErrorCode            errCode;

    /* === Internal use only === */

    U16                     state;
    U16                     retryState;
    BOOL                    gattConnected;
    CmgrHandlerCtx          cmgrHandler;
    GattClientToken         gattToken;
    GattSecurityToken       secToken;
    BtHandlerEx            *appHandler;
    BtEventTypeEx           eventType;
    DbCacheRef              ref[GD_NUM_REFERENCES];
    ListEntry               refList;
    ListEntry               servRef;
    ListEntry               charRef;
    DbCacheRef             *curRef;
} GdClientApp;

/*---------------------------------------------------------------------------
 * GdClientEvent structure
 *
 *     Used to pass GATT Demo profile client application events to
 *     the applications.
 */
struct _GdClientEvent {
    /* Inherit from BtEventEx. Note that "e.handler" contains a pointer
     * to the handler which is the CmgrLeAdvertHandler
     */
    BtEventEx               e;

    /* Pointer to application structure */
    GdClientApp            *app;
};


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GDCLIENT_Init()
 *
 *     Initialize the GATT Demo profile client application.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 *     Handler - Pointer to the handler to receive events.
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
BtStatus GDCLIENT_Init(GdClientApp *App, BtHandlerEx *Handler);

/*---------------------------------------------------------------------------
 * GDCLIENT_Deinit()
 *
 *     Deinitialize the GATT Demo profile client application.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - app, handler or callback in handler is 0
 *         (XA_ERROR_CHECK only).
 *
 */
BtStatus GDCLIENT_Deinit(GdClientApp *App);

/*---------------------------------------------------------------------------
 * GDCLIENT_Connect()
 *
 *     Initiate a connection to a remote device. This includes creating an
 *     ACL connection if needed then a GATT connection and caching the
 *     GATT Demo services and characteristics.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 *     DevAddr - Pointer to address of remote device.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated. The
 *         GDCLIENT_CONNECT_COMPLETE event will be sent to the handler when
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
BtStatus GDCLIENT_Connect(GdClientApp *App, BtDevAddr *DevAddr);

/*---------------------------------------------------------------------------
 * GDCLIENT_Disconnect()
 *
 *     Cancel a connection attempt or Disconnect an existing connection to a
 *     remote device.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request to cancel an existing connection attempt
 *         or write attempt is started. A GDCLIENT_CONNECT_COMPLETE event with
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
BtStatus GDCLIENT_Disconnect(GdClientApp *App);

/*---------------------------------------------------------------------------
 * GDCLIENT_ClearCache()
 *
 *     Clear the GATT cache associated with the device address specified
 *     by App->identityAddr.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application with
 *         App->identityAddr set to the device address to clear.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GDCLIENT_ClearCache(GdClientApp *App);

/*---------------------------------------------------------------------------
 * GDCLIENT_PrimaryServices()
 *
 *     Discover the primary services at the connected remote device. Each
 *     service will be indicated by the GDCLIENT_PRIMARY_SERVICE event.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GDCLIENT_PrimaryServices(GdClientApp *App);

/*---------------------------------------------------------------------------
 * GDCLIENT_PrimaryServicesUuid()
 *
 *     Discover the primary services that match the UUID set in App->uuid
 *     at the connected remote device. Each matching service will be
 *     indicated by the GDCLIENT_PRIMARY_SERVICE_UUID event.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application with
 *         App->uuid set to the desired UUID.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GDCLIENT_PrimaryServicesUuid(GdClientApp *App);

/*---------------------------------------------------------------------------
 * GDCLIENT_IncludedServices()
 *
 *     Discover the included services of the service at the connected remote
 *     device. The service to search is specified by setting App->handle
 *     to the handle of the service to search. Each included service will
 *     be indicated by the GDCLIENT_INCLUDED_SERVICE event. When all
 *     included services associated with a service have been found, the
 *     GDCLIENT_INC_SERVICE_COMPLETE event will be indicated.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application with
 *         App->handle set to the handle of the service to search.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GDCLIENT_IncludedServices(GdClientApp *App);

/*---------------------------------------------------------------------------
 * GDCLIENT_Characteristics()
 *
 *     Discover the characteristics of the service at the connected remote
 *     device. The service to search is specified by setting App->handle
 *     to the handle of the service to search. Each characteristic will
 *     be indicated by the GDCLIENT_CHARACTERISTIC event. When all
 *     characteristics associated with a service have been found, the
 *     GDCLIENT_CHARACTERISTIC_COMPLETE event will be indicated.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application with
 *         App->handle set to the handle of the service to search.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GDCLIENT_Characteristics(GdClientApp *App);

/*---------------------------------------------------------------------------
 * GDCLIENT_CharacteristicsUuid()
 *
 *     Discover the characteristics of the service that match a UUID at
 *     the connected remote device. The service to search is specified by
 *     setting App->handle to the handle of the service to search. The UUID
 *     is specified by setting App->uuid to the desired UUID. Each
 *     matching characteristic will be indicated by the
 *     GDCLIENT_CHARACTERISTIC_UUID event. When all characteristics
 *     associated with a service have been found, the
 *     GDCLIENT_CHARACTER_UUID_COMPLETE event will be indicated.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application with
 *         App->handle set to the handle of the service to search and
 *         App->uuid set to the desired UUID.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GDCLIENT_CharacteristicsUuid(GdClientApp *App);

/*---------------------------------------------------------------------------
 * GDCLIENT_Descriptors()
 *
 *     Discover the descriptors of the characteristic at the connected remote
 *     device. The characteristic to search is specified by setting App->handle
 *     to the handle of the characteristic to search. Each descriptor will
 *     be indicated by the GDCLIENT_CHAR_DESCRIPTOR event. When all
 *     descriptors associated with a characteristic have been found, the
 *     GDCLIENT_CHAR_DESCRIPTOR_COMPLETE event will be indicated.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application with
 *         App->handle set to the handle of the characteristic to search.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GDCLIENT_Descriptors(GdClientApp *App);

/*---------------------------------------------------------------------------
 * GDCLIENT_Value()
 *
 *     Read the characteristic value of a characteristic value handle at
 *     the connected remote device. The desired characteristic value is
 *     specified by setting App->handle to the value handle. The value
 *     will be indicated with the GDCLIENT_VALUE event.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application with
 *         App->handle set to the handle of the desired characteristic value.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GDCLIENT_Value(GdClientApp *App);

/*---------------------------------------------------------------------------
 * GDCLIENT_Values()
 *
 *     Read multiple characteristic values of characteristic value handles
 *     at the connected remote device. The desired characteristic values
 *     are specified by setting App->handles to the value handle array and
 *     App->num to the count of handles in the array. The values will be
 *     indicated with the GDCLIENT_VALUE event.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application with
 *         App->handles set to the handle array of the desired
 *         characteristic values and App->num set to the count of
 *         values to read.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GDCLIENT_Values(GdClientApp *App);

/*---------------------------------------------------------------------------
 * GDCLIENT_ValueUuid()
 *
 *     Read the characteristic value(s) matching a characteristic value UUID at
 *     the connected remote device. The desired characteristic value UUID is
 *     specified by setting App->handle and App->endHandle to the range to
 *     search and App->uuid to the desired UUID. The value(s) will be indicated
 *     with the GDCLIENT_VALUE event.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application with
 *         App->handle and App->endHandle set to the range to search and
 *         App->uuid set to the desired UUID.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GDCLIENT_ValueUuid(GdClientApp *App);

/*---------------------------------------------------------------------------
 * GDCLIENT_ValueLong()
 *
 *     Read the characteristic long value of a characteristic value handle at
 *     the connected remote device. The desired characteristic long value is
 *     specified by setting App->handle to the value handle and App->offset
 *     to the desired offset. The value will be indicated with the
 *     GDCLIENT_VALUE event.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application with
 *         App->handle set to the handle of the desired characteristic value
 *         and App->offset set to the desired offset.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GDCLIENT_ValueLong(GdClientApp *App);

/*---------------------------------------------------------------------------
 * GDCLIENT_WriteValue()
 *
 *     Write the characteristic value of a characteristic value handle at
 *     the connected remote device. The desired characteristic value is
 *     specified by setting App->handle to the value handle and App->value
 *     to the value to write. If the value is a long value, App->offset must
 *     be set to the offset of the long value. Otherwise, App->offset must
 *     be set to 0.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application with
 *         App->handle set to the handle of the desired characteristic value,
 *         App->value set to the value and with App->offset set to 0 or
 *         the offset.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GDCLIENT_WriteValue(GdClientApp *App);

/*---------------------------------------------------------------------------
 * GDCLIENT_WriteValueNoResponse()
 *
 *     Write the characteristic value of a characteristic value handle at
 *     the connected remote device with no expected response. The desired
 *     characteristic value is specified by setting App->handle to the
 *     value handle and App->value to the value to write. The value must
 *     not be a long value.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application with
 *         App->handle set to the handle of the desired characteristic value
 *         and App->value set the the value.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GDCLIENT_WriteValueNoResponse(GdClientApp *App);

/*---------------------------------------------------------------------------
 * GDCLIENT_WriteValueSigned()
 *
 *     Write the characteristic value of a characteristic value handle at
 *     the connected remote device using a Signed Write. The desired
 *     characteristic value is specified by setting App->handle to the
 *     value handle and App->value to the value to write. The value must
 *     not be a long value.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application with
 *         App->handle set to the handle of the desired characteristic value
 *         and App->value set to the value.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GDCLIENT_WriteValueSigned(GdClientApp *App);

/*---------------------------------------------------------------------------
 * GDCLIENT_WriteValueReliable()
 *
 *     Reliably Write a set of characteristic values at a characteristic
 *     value handle on the connected remote device. The desired set of
 *     characteristic values is specified by setting App->itemArray to point
 *     to a GattClientWriteReliableItem structure filled with the value
 *     handles, offsets and values to write. This structure must persist
 *     through callbacks. The parameter App->itemCount must be set to the
 *     count of values in the referenced App->itemArray structure.
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application with
 *         App->itemArray pointing to a GattClientWriteReliableItem structure
 *         and App->itemCount set to the count of items in the structure.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been initiated.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GDCLIENT_WriteValueReliable(GdClientApp *App);

/*---------------------------------------------------------------------------
 * GDCLIENT_FixGattSecurityError()
 *
 *     Fix the GATT security error signaled by the GDCLIENT_SECURITY_ERROR
 *     event. It is possible that fixing a security error then retrying the
 *     operation may result in a second security error (e.g. first fix
 *     results in "just works" pairing when what is needed is "MITM" pairing).
 *
 * Parameters:
 *     App - Pointer to a structure that represents the application with
 *         App->errCode containing the security error. Note that this is
 *         set to error when the GDCLIENT_SECURITY_ERROR event is sent to
 *         the app.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded (the correct security is in
 *         place).
 *
 *     BT_STATUS_PENDING - request has been initiated when complete the
 *         GDCLIENT_SECURITY_COMPLETE event will be sent.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because there is no connection.
 *
 *     BT_STATUS_BUSY - operation failed because a pending operation is in
 *         progress.
 *
 *     BT_STATUS_INVALID_PARM - app is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GDCLIENT_FixGattSecurityError(GdClientApp *App);

#endif /* _GD_CLIENT_H__ */
