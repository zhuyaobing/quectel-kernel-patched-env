/****************************************************************************
 *
 * File:
 *     gattclient.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     upper layer of Generic Attribute Profile (GATT) for client
 *     applications.
 *
 * Copyright 1999-2017 OpenSynergy GmbH.
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

#ifndef __GATTCLIENT_H
#define __GATTCLIENT_H

#include "mele.h"
#include "gatt.h"
#include "gattuuid.h"
#include "sdp.h"

/*---------------------------------------------------------------------------
 * Generic Attribute Profile (GATT) Client layer
 *
 *     The Generic Attribute Profile Client layer provides functions that
 *     allow client applications to make GATT requests to remote GATT
 *     servers using the ATT protocol. The functions parse the results.
 *
 *     There are two main groups of functions. The first group of functions
 *     is known as "discovery" functions and provides the ability to discover
 *     primary services, included services, characteristics and
 *     characteristic descriptors on a remote GATT server. The second group of
 *     functions provides the ability to read and write the characteristic
 *     values and descriptors on a remote GATT server.
 *
 *     The GATT Client Layer provides the ability to cache the primary
 *     services, included services, characteristics and characteristic
 *     descriptors found during discovery. Each remote device will have
 *     its own cache record. There are two sets of discovery functions. The
 *     first set of discovery functions uses the cache. If the cache is empty
 *     then these functions will first fill the cache to the level needed to
 *     complete the request. Subsequent calls will pull information from the
 *     cache. The second set of discovery functions always makes requests over
 *     the air. These functions neither use the cache nor fill the cache. It
 *     is highly recommended that applications use the cache based discovery
 *     functions.
 *
 *     Some GATT profiles require that SDP be used for primary service
 *     discovery when the devices are connected over BR/EDR.  The
 *     "UseSdpForBrEdr" flag controls whether GATTC_GetCachedPrimaryService()
 *     and GATTC_GetCachedPrimaryServiceByUUID() use SDP when discovering
 *     primary services. Also special functions that use SDP exclusively for
 *     primary service discovery are provided. These functions use and fill the
 *     cache.
 *
 */


/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GattClientEventType type
 *
 *     The GATT Client Event types are passed to the token handler to
 *     indicate completion of operations and for other events. Applications
 *     pass in the token handler when making the GATT client request. The
 *     event types are offsets from BTE_TYPE_BASE_GATTC defined in bttypes2.h.
 */
typedef BtEventTypeEx GattClientEventType;

/* The Connect operation has completed or an incoming connection has
 * occurred.
 */
#define GATTCE_CONNECT_COMPLETE                 (BTE_TYPE_BASE_GATTC + 0)

/* The GATT connection was disconnected. "e.status" gives the
 * disconnect reason.
 */
#define GATTCE_DISCONNECT                       (BTE_TYPE_BASE_GATTC + 1)

/* The Get Primary Service operation has completed. "e.status"
 * indicates the success or failure of the operation. If "e.status"
 * is BT_STATUS_FAILED then "token->errCode" indicates the reason for the
 * failure from the remote device. If "token->errCode" is
 * ATTEC_ATTRIBUTE_NOT_FOUND the search is complete. If "token->errCode"
 * is ATTEC_NO_ERROR and "e.status" is BT_STATUS_FAILED then a failure
 * occurred while trying to send the queued request. If "e.status" is
 * BT_STATUS_SUCCESS then "token->result.gps" contains the primary service
 * information. If "e.status" is BT_STATUS_PROTOCOL_ERROR then the response
 * from the server contained an error.
 */
#define GATTCE_GET_PRIM_SERV_COMPLETE           (BTE_TYPE_BASE_GATTC + 2)

/* The Get Primary Service by UUID operation has completed. "e.status"
 * indicates the success or failure of the operation. If "e.status"
 * is BT_STATUS_FAILED then "token->errCode" indicates the reason for the
 * failure from the remote device. If "token->errCode" is ATTEC_NO_ERROR
 * and "e.status" is BT_STATUS_FAILED then a failure occurred while trying
 * to send the queued request. If "e.status" is BT_STATUS_SUCCESS then
 * "token->result.gpsbu" contains primary service information. If
 * "e.status" is BT_STATUS_PROTOCOL_ERROR the response from the server
 * contained an error.
 */
#define GATTCE_GET_PRIM_SERV_UUID_COMPLETE      (BTE_TYPE_BASE_GATTC + 3)

/* The Get Included Service operation has completed. "e.status"
 * indicates the success or failure of the operation. If "e.status"
 * is BT_STATUS_FAILED then "token->errCode" indicates the reason for the
 * failure from the remote device. If "token->errCode" is ATTEC_NO_ERROR
 * and "e.status" is BT_STATUS_FAILED then a failure occurred while trying
 * to send the request when the token was queued. If "e.status" is
 * BT_STATUS_SUCCESS then "token->result.gis" contains the included service
 * information. If "e.status" is BT_STATUS_PROTOCOL_ERROR then the response
 * from the server contained an error.
 */
#define GATTCE_GET_INCLUDED_SERV_COMPLETE       (BTE_TYPE_BASE_GATTC + 4)

/* The Get Characteristic operation has completed. "e.status"
 * indicates the success or failure of the operation. If "e.status"
 * is BT_STATUS_FAILED then "token->errCode" indicates the reason for the
 * failure from the remote device. If "token->errCode" is ATTEC_NO_ERROR
 * and "e.status" is BT_STATUS_FAILED then a failure occurred while trying
 * to send the request when the token was queued. If "e.status" is
 * BT_STATUS_SUCCESS then "token->result.gc" contains the characteristic
 * information.  If "e.status" is BT_STATUS_PROTOCOL_ERROR then the
 * response from the server contained an error.
 */
#define GATTCE_GET_CHARACTERISTIC_COMPLETE      (BTE_TYPE_BASE_GATTC + 5)

/* The Get Characteristic by UUID operation has completed. "e.status"
 * indicates the success or failure of the operation. If "e.status"
 * is BT_STATUS_FAILED then "token->errCode" indicates the reason for the
 * failure from the remote device. If "token->errCode" is ATTEC_NO_ERROR
 * and "e.status" is BT_STATUS_FAILED then a failure occurred while trying
 * to send the request when the token was queued. If "e.status" is
 * BT_STATUS_SUCCESS, "token->result.gc" contains the characteristic
 * information.  If "e.status" is BT_STATUS_PROTOCOL_ERROR the response
 * from the server contained an error.
 */
#define GATTCE_GET_CHARACTER_UUID_COMPLETE      (BTE_TYPE_BASE_GATTC + 6)

/* The Get Characteristic Descriptor operation has completed. "e.status"
 * indicates the success or failure of the operation. If "e.status"
 * is BT_STATUS_FAILED then "token->errCode" indicates the reason for the
 * failure from the remote device. If "token->errCode" is ATTEC_NO_ERROR
 * and "e.status" is BT_STATUS_FAILED then a failure occurred while trying
 * to send the request when the token was queued. If "e.status" is
 * BT_STATUS_SUCCESS then "token->result.gcd" contains the characteristic
 * descriptor information.  If "e.status" is BT_STATUS_PROTOCOL_ERROR
 * then the response from the server contained an error.
 */
#define GATTCE_GET_CHARACTER_DESC_COMPLETE      (BTE_TYPE_BASE_GATTC + 7)

/* The Get Characteristic Value operation has completed. "e.status"
 * indicates the success or failure of the operation. If "e.status"
 * is BT_STATUS_FAILED then "token->errCode" indicates the reason for the
 * failure from the remote device. If "token->errCode" is ATTEC_NO_ERROR
 * and "e.status" is BT_STATUS_FAILED then a failure occurred while trying
 * to send the request when the token was queued. If "e.status" is
 * BT_STATUS_SUCCESS then "token->result.gcv" contains the characteristic
 * value information.  If "e.status" is BT_STATUS_PROTOCOL_ERROR
 * then the response from the server contained an error.
 */
#define GATTCE_GET_CHARACTER_VAL_COMPLETE       (BTE_TYPE_BASE_GATTC + 8)

/* The Get Characteristic Value by UUID operation has completed. "e.status"
 * indicates the success or failure of the operation. If "e.status"
 * is BT_STATUS_FAILED then "token->errCode" indicates the reason for the
 * failure from the remote device. If "token->errCode" is ATTEC_NO_ERROR
 * and "e.status" is BT_STATUS_FAILED then a failure occurred while trying
 * to send the request when the token was queued. If "e.status" is
 * BT_STATUS_SUCCESS then "token->result.gcvbu" contains the characteristic
 * value information. If "e.status" is BT_STATUS_PROTOCOL_ERROR then the
 * response from the server contained an error.
 */
#define GATTCE_GET_CHARACTER_VAL_UUID_COMPLETE  (BTE_TYPE_BASE_GATTC + 9)

/* The Get Characteristic Long Value operation has completed. "e.status"
 * indicates the success or failure of the operation. If "e.status"
 * is BT_STATUS_FAILED then "token->errCode" indicates the reason for the
 * failure from the remote device. If "token->errCode" is ATTEC_NO_ERROR
 * and "e.status" is BT_STATUS_FAILED then a failure occurred while trying
 * to send the request when the token was queued. If "e.status" is
 * BT_STATUS_SUCCESS then "token->result.gcv" contains the characteristic
 * value information. If "e.status" is BT_STATUS_PROTOCOL_ERROR then the
 * response from the server contained an error.
 */
#define GATTCE_GET_CHARACTER_LONG_VAL_COMPLETE  (BTE_TYPE_BASE_GATTC + 10)

/* The Get Multiple Characteristic Values operation has completed. "e.status"
 * indicates the success or failure of the operation. If "e.status"
 * is BT_STATUS_FAILED then "token->errCode" indicates the reason for the
 * failure from the remote device. If "token->errCode" is ATTEC_NO_ERROR
 * and "e.status" is BT_STATUS_FAILED then a failure occurred while trying
 * to send the request when the token was queued. If "e.status" is
 * BT_STATUS_SUCCESS then "token->result.gcv" contains the characteristic
 * value information. If "e.status" is BT_STATUS_PROTOCOL_ERROR then the
 * response from the server contained an error.
 */
#define GATTCE_GET_MULTI_CHARACTER_VAL_COMPLETE (BTE_TYPE_BASE_GATTC + 11)

/* The Write Characteristic Value Without Response operation has been sent
 * and the token is available.
 */
#define GATTCE_WRITE_CHARACTER_VAL_NO_RESP      (BTE_TYPE_BASE_GATTC + 12)

/* The Write Characteristic Value Without Signing operation has
 * completed and the token is available. "e.status" indicates
 * the success or failure of the operation.
 */
#define GATTCE_WRITE_CHARACTER_VAL_SIGNED       (BTE_TYPE_BASE_GATTC + 13)

/* The Write Characteristic Value operation has completed. "e.status"
 * indicates the success or failure of the operation. If "e.status"
 * is BT_STATUS_FAILED then "token->errCode" indicates the reason for the
 * failure from the remote device. If "token->errCode" is ATTEC_NO_ERROR
 * and "e.status" is BT_STATUS_FAILED then a failure occurred while trying
 * to send the request when the token was queued. If "e.status" is
 * BT_STATUS_PROTOCOL_ERROR then the response from the server contained
 * an error.
 */
#define GATTCE_WRITE_CHARACTER_VAL              (BTE_TYPE_BASE_GATTC + 14)

/* The Write Characteristic Value Reliable operation has completed.
 * "e.status" indicates the success or failure of the operation. If
 * "e.status" is BT_STATUS_FAILED then "token->errCode" indicates the reason
 * for the failure from the remote device. If "token->errCode" is
 * ATTEC_NO_ERROR and "e.status" is BT_STATUS_FAILED then a failure occurred
 * while trying to send the request when the token was queued. If "e.status"
 * is BT_STATUS_PROTOCOL_ERROR the response from the server contained an
 * error (e.g. values received in a "prepareWrite" response did not match
 * the values in the "prepareWrite" request so reliable write failed).
 */
#define GATTCE_WRITE_CHARACTER_VAL_RELIABLE     (BTE_TYPE_BASE_GATTC + 15)

/* A notification from a remote GATT server has been received.
 * "token->result.notify" contains the notification information. Only
 * the first ATT_MTU-3 octets of the attribute value are sent. The client
 * can call GATTC_GetLongCharacteristicValue to get the remaining
 * portion of the value.
 *
 * The client application must write the appropriate value to the
 * Characteristic Configuration descriptor for the desired characteristic
 * to configure the server to send notifications.
 */
#define GATTCE_NOTIFICATION                     (BTE_TYPE_BASE_GATTC + 16)

/* An indication from a remote GATT server has been received.
 * "token->result.notify" contains the indication information. Only
 * the first ATT_MTU-3 octets of the attribute value are sent. The client
 * can call GATTC_GetLongCharacteristicValue to get the remaining
 * portion of the value. The difference between a notification and
 * indication is that a confirmation is sent by the ATT protocol
 * back to server.
 *
 * The client application must write the appropriate value to the
 * Characteristic Configuration descriptor for the desired
 * characteristic to configure the server to send indications.
 */
#define GATTCE_INDICATION                       (BTE_TYPE_BASE_GATTC + 17)

/* The Get Cached Primary Service operation has completed. "e.status"
 * indicates the success or failure of the operation. If "e.status"
 * is BT_STATUS_FAILED then "token->errCode" indicates the reason for the
 * failure from the remote device. If "token->errCode" is ATTEC_NO_ERROR
 * and "e.status" is BT_STATUS_FAILED then a failure occurred while
 * trying to send the queued request. If "e.status" is BT_STATUS_SUCCESS
 * then all services for the remote device will be in the cache and the
 * user must call GATTC_GetCachedPrimaryService() again to get the
 * cached information. If "e.status" is BT_STATUS_PROTOCOL_ERROR then
 * a response from the server contained an error. If "e.status" is
 * BT_STATUS_CACHE_CLEARED then the remote server has sent a service
 * change indication and the cache has been cleared. The operation
 * has been aborted and must be restarted.
 */
#define GATTCE_GET_CACHED_PRIM_SERV             (BTE_TYPE_BASE_GATTC + 20)

/* The Get Cached Primary Service by UUID operation has completed. "e.status"
 * indicates the success or failure of the operation. If "e.status"
 * is BT_STATUS_FAILED then "token->errCode" indicates the reason for the
 * failure from the remote device. If "token->errCode" is ATTEC_NO_ERROR
 * and "e.status" is BT_STATUS_FAILED then a failure occurred while trying
 * to send the queued request. If "e.status" is BT_STATUS_SUCCESS then
 * all services for the remote device will be in the cache and the
 * user must call GATTC_GetCachedPrimaryServiceByUuid() again to get the
 * cached information. If "e.status" is BT_STATUS_PROTOCOL_ERROR then
 * a response from the server contained an error. If "e.status" is
 * BT_STATUS_CACHE_CLEARED then the remote server has sent a service
 * change indication and the cache has been cleared. The operation
 * has been aborted and must be restarted.
 */
#define GATTCE_GET_CACHED_PRIM_SERV_UUID        (BTE_TYPE_BASE_GATTC + 21)

/* The Get Cached Included Service operation has completed. "e.status"
 * indicates the success or failure of the operation. If "e.status"
 * is BT_STATUS_FAILED then "token->errCode" indicates the reason for the
 * failure from the remote device. If "token->errCode" is ATTEC_NO_ERROR
 * and "e.status" is BT_STATUS_FAILED then a failure occurred while trying
 * to send the queued request. If "e.status" is BT_STATUS_SUCCESS then
 * all include service declarations for the specified service are in the
 * cache and the user must call GATTC_GetCachedIncludedService again to
 * get the cached information. If "e.status" is BT_STATUS_PROTOCOL_ERROR
 * then a response from the server contained an error. If "e.status" is
 * BT_STATUS_CACHE_CLEARED then the remote server has sent a service
 * change indication and the cache has been cleared. All cache references
 * (servRef and charRef) are invalid and must be obtained again.
 */
#define GATTCE_GET_CACHED_INCLUDED_SERV         (BTE_TYPE_BASE_GATTC + 22)

/* The Get Cached Characteristic operation has completed. "e.status"
 * indicates the success or failure of the operation. If "e.status"
 * is BT_STATUS_FAILED then "token->errCode" indicates the reason for the
 * failure from the remote device. If "token->errCode" is ATTEC_NO_ERROR
 * and "e.status" is BT_STATUS_FAILED then a failure occurred while trying
 * to send the request when the token was queued. If "e.status" is
 * BT_STATUS_SUCCESS then all the characteristic declarations for the
 * specified service are in the cache and the user must call
 * GATTC_GetCachedCharacteristic() again to get the cached information.
 * If "e.status" is BT_STATUS_PROTOCOL_ERROR then the response from the
 * server contained an error. If "e.status" is BT_STATUS_CACHE_CLEARED
 * then the remote server has sent a service change indication
 * and the cache has been cleared. All cache references (servRef and
 * charRef) are invalid and must be obtained again.
 */
#define GATTCE_GET_CACHED_CHARACTERISTIC        (BTE_TYPE_BASE_GATTC + 23)

/* The Get Cached Characteristic by UUID operation has completed. "e.status"
 * indicates the success or failure of the operation. If "e.status"
 * is BT_STATUS_FAILED then "token->errCode" indicates the reason for the
 * failure from the remote device. If "token->errCode" is ATTEC_NO_ERROR
 * and "e.status" is BT_STATUS_FAILED then a failure occurred while trying
 * to send the request when the token was queued. If "e.status" is
 * BT_STATUS_SUCCESS then all the characteristic declarations for the
 * specified service are in the cache and the user must call
 * GATTC_GetCachedCharacteristicByUuid() again to get the cached information.
 * If "e.status" is BT_STATUS_PROTOCOL_ERROR then the response from the
 * server contained an error. If "e.status" is BT_STATUS_CACHE_CLEARED
 * then the remote server has sent a service change indication
 * and the cache has been cleared. All cache references (servRef and
 * charRef) are invalid and must be obtained again.
 */
#define GATTCE_GET_CACHED_CHARACTERISTIC_UUID   (BTE_TYPE_BASE_GATTC + 24)

/* The Get Cached Characteristic Descriptor operation has completed.
 * "e.status" indicates the success or failure of the operation. If
 * "e.status" is BT_STATUS_FAILED then "token->errCode" indicates the
 * reason for the failure from the remote device. If "token->errCode" is
 * ATTEC_NO_ERROR and "e.status" is BT_STATUS_FAILED then a failure occurred
 * while trying to send the request when the token was queued. If "e.status"
 * is BT_STATUS_SUCCESS then all the characteristic descriptors for the
 * specified characteristic are in the cache and the user must call
 * GATTC_GetCachedCharacteristicDescriptor() again to get the cached
 * information. If "e.status" is BT_STATUS_PROTOCOL_ERROR  then the response
 * from the server contained an error. If "e.status" is BT_STATUS_CACHE_CLEARED
 * then the remote server has sent a service change indication and the
 * cache has been cleared. All cache references (servRef and charRef)
 * are invalid and must be obtained again.
 */
#define GATTCE_GET_CACHED_CHARACTER_DESC        (BTE_TYPE_BASE_GATTC + 25)

/* Invalid value used for initialization purposes */
#define GATTCE_INVALID                          (BTE_TYPE_BASE_GATTC + 0xFF)

/* End of GattClientEventType */

/*---------------------------------------------------------------------------
 * GattSearchType type
 *
 *     The GATT Search Type indicates the type of search for some of the
 *     GATT search operations.
 */
typedef U8 GattSearchType;

#define GST_FIRST  0x00    /* Find first item - start the search */
#define GST_NEXT   0x01    /* Find subsequent items */

/* End of GattSearchType */

/* Forward declaration of GattClientToken structure */
typedef struct _GattClientToken GattClientToken;

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GattClientWriteReliableItem structure
 *
 *     GattClientWriteReliableItem contains one item of a
 *     GATTC_WriteCharacteristicValueReliable call.
 */
typedef struct _GattClientWriteReliableItem {
    U16           handle; /* Handle of characteristic */
    U16           offset; /* Offset in value where write is to start */
    U16           len;    /* Length of value to write in octets */

    /* Pointer to value - must be able to persist between calls. This
     * pointer can be NULL if len is 0. A write with len 0 can be used to
     * truncate a variable length value. 
     *
     * Note that the value is sent over the air starting with array index 0. 
     * The GATT specification indicates that all multi-octet values (not strings) 
     * are to be sent in little endian format therefore, values must be in 
     * little endian format (least significant octet at array index 0).
     */
    U8           *value;
} GattClientWriteReliableItem;

/*---------------------------------------------------------------------------
 * GattClientToken structure
 *
 *     GattClientToken is used to make GATT client requests and to capture
 *     the results.
 */
struct _GattClientToken {
    /* Inherit from BtHandlerEx */
    BtHandlerEx      h;

    /* Request information */
    union {
        /* Used for GATTC_GetPrimaryServiceByUuid() and
         * GATTC_GetCachedPrimaryServiceByUuid(). 128-bit UUIDs
         * must be in little endian format.
         */
        GattUuid   uuid;

        /* Used for GATTC_GetIncludedService(), GATTC_GetCharacteristic() */
        struct {
            U16  startHandle;  /* Start attribute handle of service */
            U16  endHandle;    /* End attribute handle of service */
        } servId;

        /* Used for GATTC_GetCharacteristicByUuid(), GATTC_GetCharacteristicValueByUuid() 
        *  128-bit UUIDs must be in little endian format.
        */
        struct {
            U16      startHandle; /* Start attribute handle of service */
            U16      endHandle;   /* End attribute handle of service */
            GattUuid uuid;        /* UUID of desired characteristic */
        } gcbu;

        /* Used for GATTC_GetCharacteristicDescriptor() */
        struct {
            U16      startHandle; /* Handle of characteristic value + 1 */
            U16      endHandle;   /* ending handle of characteristic */
        } gcd;

        /* Used for GATTC_GetCharacteristicValue() */
        U16    charHandle;        /* Handle for characteristic value or descriptor */

        /* Used for GATTC_GetCharacteristicLongValue() */
        struct {
            U16   handle;         /* Handle for characteristic value or descriptor */
            U16   offset;         /* Offset for portion of value to read */
        } longVal;

        /* Used for GATTC_GetMultipleCharacteristicValues() */
        struct {
            U16  *handles;        /* Pointer to array of characteristic handles */
            U16   num;            /* Number of handles in the array */
        } handleArray;

        /* Used for GATTC_WriteCharacteristicValueNoResp(), GATTC_WriteCharacteristicValueSigned(),
         * GATTC_WriteCharacteristicValue()
         */
        struct {
            U16   handle;         /* Handle of characteristic */
            U16   len;            /* Length of value to write in octets */
            U16   offset;         /* Offset in value where write is to start (long values only) */

            /* Pointer to value - must be able to persist between calls. This
             * pointer can be NULL if len is 0. A write with len 0 can be used to
             * truncate a variable length value.
             *
             * Note that the value is sent over the air starting with array index 0. 
             * The GATT specification indicates that all multi-octet values (not strings) 
             * are to be sent in little endian format therefore, values must be in 
              * little endian format (least significant octet at array index 0).
             */
            U8   *value;
        } writeVal;

        /* Used for GATTC_WriteCharacteristicValueReliable() */
        struct {
            /* Number of items in "itemArray" */
            U16                          num;

            /* A GattClientWriteReliableItem array of size "num". */
            GattClientWriteReliableItem *itemArray;
        } writeValReliable;

        /* Used for GATTC_GetCachedIncludedService(), GATTC_GetCachedCharacteristic() */
        void *servRef;      /* Reference to service containing the item */

        /* Used for GATTC_GetCachedCharacteristicByUuid().128-bit UUIDs
         * must be in little endian format. 
         */
        struct {
            void *servRef;  /* Reference to service containing the characteristic */
            GattUuid uuid;  /* UUID of desired characteristic */
        } gccbu;

        /* Used for GATTC_GetCachedCharacteristicDescriptor() */
        struct {
            void *servRef;  /* Reference to service containing the item */
            void *charRef;  /* Reference to a characteristic definition containing the item */
        } gccd;
    } request;

    /* Results for GATT operations. Note that results from a request may be used by the stack
     * when performing another request so the result must not be modified
     */
    AttErrorCode  errCode;
    U16           errHandle;   /* Handle that caused the error if appropriate */
    union {
        /* Result for GATTC_GetPrimaryService(). 128-bit UUIDs
         * will be in little endian format. 
         */
        struct {
            U16      handle;          /* Attribute handle */
            U16      endGroupHandle;  /* end group handle */
            GattUuid uuid;            /* Service's UUID */
        } gps;

        /* Result for GATTC_GetPrimaryServiceByUuid() */
        struct {
            U16  handle;          /* Found attribute handle */
            U16  endGroupHandle;  /* Group end handle */
        } gpsbu;

        /* Result for GATTC_GetIncludedService() 128-bit UUIDs
         * will be in little endian format. 
         */
        struct {
            U16      handle;          /* handle of include service entry within service */
            U16      startHandle;     /* Included service's starting attribute handle */
            U16      endGroupHandle;  /* End group handle of included service */
            GattUuid uuid;            /* UUID of the included service */
        } gis;

        /* Result for GATTC_GetCharacteristic(), GATTC_GetCharacteristicByUuid. 
         * 128-bit UUIDs will be in little endian format. 
         */
        struct {
            U16      handle;          /* Handle of the characteristic declaration */
            U8       properties;      /* Characteristic properties */
            U16      valueHandle;     /* Handle of the characteristic value */
            GattUuid uuid;            /* Characteristic UUID */
        } gc;

        /* Result for GATTC_GetCharacteristicDescriptor().  128-bit UUIDs
         * will be in little endian format. 
         */
        struct {
            U16      handle;          /* Handle of the characteristic descriptor */
            GattUuid uuid;            /* UUID of characteristic descriptor */
        } gcd;

        /* Result for GATTC_GetCharacteristicValue(), GATTC_GetCharacteristicLongValue(),
         * GATTC_GetMultipleCharacteristicValues()
         */
        struct {
            U16   len;     /* Length of value or portion of value in val */

            /* Pointer to array holding value which must be used or copied before
             * returning back from the event.
             *
             * Note that values (not strings) are received over the air are in little 
             * endian format with the least significant octet starting at array index 0.
             */ 
            U8   *val;

            /* TRUE if there is more data for value possible and FALSE otherwise */
            BOOL  more;
        } gcv;

        /* Result for GATTC_GetCharacteristicValueByUuid() */
        struct {
            U16   handle;  /* Handle of the characteristic whose value is returned */
            U16   len;     /* Length of value or portion of value in val */

            /* Pointer to array holding value which must be used or copied before
             * returning back from the event.
             *
             * Note that values (not strings) are received over the air are in little endian 
             * format with the least significant octet starting at array index 0.
             */ 
            U8   *val;

            /* TRUE if there is more data for value possible and FALSE otherwise */
            BOOL  more;
        } gcvbu;

        /* Result for GATTCE_NOTIFICATION and GATTCE_INDICATION events */
        struct {
            U16   handle;  /* Handle of the characteristic whose value is returned */
            U16   len;     /* Length of value or portion of value in val */

            /* Pointer to array holding value which must be used or copied before
             * returning back from the event.
             *
             * Note that values (not strings) are received over the air are in little 
             * endian format with the least significant octet starting at array index 0.
             */ 
            U8   *val;
        } notify;

        /* Result for GATTC_GetCachedPrimaryService(),
         * GATTC_GetCachedPrimaryServiceByUuid(). 128-bit UUIDs
         * will be in little endian format. 
         */
        struct {
            U16       handle;          /* Attribute handle */
            U16       endGroupHandle;  /* end group handle */
            GattUuid  uuid;            /* Service's UUID */
            void     *service;         /* Identifier for service */
        } cacheds;

        /* Result for GATTC_GetCachedIncludedService(). 128-bit UUIDs
         * will be in little endian format. 
         */
        struct {
            U16       handle;          /* handle of include service entry within service */
            U16       startHandle;     /* Included service's starting attribute handle */
            U16       endGroupHandle;  /* End group handle of included service */
            GattUuid  uuid;            /* UUID of the included service */
            void     *service;         /* Identifier for service */
        } gcis;

        /* Result for GATTC_GetCachedCharacteristic(), GATTC_GetCachedCharacteristicByUuid. 
         * 128-bit UUIDs will be in little endian format. 
         */
        struct {
            U16       handle;          /* Handle of the characteristic declaration */
            U8        properties;      /* Characteristic properties */
            U16       valueHandle;     /* Handle of the characteristic value */
            GattUuid  uuid;            /* Characteristic value UUID */
            void     *character;       /* Reference to characteristic */
        } gcc;
    } result;

    /* === Internal use only === */

    /* Pointer to remote device (either BR/EDR or LE )*/
    BtRemoteDevice        *remDev;

    /* Pointer to record for cache operations (GattCacheDeviceRecord *) */
    void                  *record;

    /* Pointer to ATT context */
    void                  *attCtx;

    /* Used during SDP service search to keep track of attCtx */
    BOOL                   attCtx0;

    /* Node for keeping token on the request queue */
    ListEntry              rnode;

    /* Information used in attribute searching */
    union {
        /* Buffer for holding results and length of data in the buffer */
        struct {
            U8      buff[GATT_MAX_MTU];
            U16     buffLen;
            U8      buffOp;   /* The opcode for the data in the buffer.  */

            /* Current index into the buff */
            U16                    bi;
        } b;
    } search;

    /* Event that should be sent to handler when operation completes */
    GattClientEventType    eType;

    /* Function that is called by stack to start the operation */
    BtStatus (*reqFunc) (GattClientToken *token);

    /* Function to call to put result into the token */
    BtStatus (*resultFunc) (GattClientToken *token);

    /* Function to call to handle error response. If no special error
     * handling is needed it can be set to 0.
     */
    BtStatus (*errorFunc) (GattClientToken *token);

    /* Information used as part of an operation */
    union {
        /* Information used for Get Primary Service and Get
         * Primary Service by UUID
         */
        struct {
            U16 startHandle;
            U8  size;        /* Size of the response */
        } gps;

        /* Read By Type Req/Rsp */
        struct {
            U16 startHandle;
            U16 endHandle;
            GattUuid uuid;
            U8  size;         /* Size of the response */
        } rbt;

        /* Used for Find Info Req/Rsp */
        struct {
            U16 startHandle;
            U16 endHandle;
            U8  format;
        } fi;

        /* Used for Write Long Characteristic Value Req/Rsp and
          */
        struct {
            /* Status of the operation to be sent to the app when the
             * the "writeExecute" response is received
             */
            BtStatus status;
            U16      offset;  /* Current offset into "value" */
            U16      len;     /* Length written so far */
            U16      i;       /* Index into itemArray (reliable only) */
            U16      rHandle; /* Handle received in PrepareWrite Rsp */
            U16      rOffset; /* Offset received in PrepareWrite Rsp */
        } wcv;

        /* Information used for Get Cached Primary Service and Get
         * Cached Primary Service by UUID
         */
        struct {
            ListEntry *cur;
        } gcps;

        /* Information used for Get Cached Included Service */
        struct {
            void *service;
            void *attrib;
            ListEntry *cur;
        } gcis;

        /* Information used for Get Cached Characteristic and
         * Get Cached characteristic descriptor
         */
        struct {
            ListEntry *cur;
        } gcc;
    } opi;

};


/*---------------------------------------------------------------------------
 * GattClientEvent structure
 *
 *     Used to pass information from the GATT layer to GATT client applications.
 */
typedef struct _GattClientEvent {
    /* Inherit from BtEventEx. Note that "e.handler" contains a pointer
     * to the handler which is the GattClientToken
     */
    BtEventEx         e;

    union {
        /* Used for GATTCE_CONNECT_COMPLETE event */
        struct {
            BtRemoteDevice             *remDev;
        } connect;
    } info;

} GattClientEvent;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GATTC_InitToken()
 *
 *     Initialize the client token for operation over a link.
 *
 * Parameters:
 *     token - Pointer to token with handler fields initialized (especially
 *         h.callback).
 *
 *     remDev - Pointer to remote device structure representing the link to
 *         remote GATT server.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed because remDev is not valid.
 *
 *     BT_STATUS_INVALID_PARM - token, link or h.callback in token is 0
 *         (XA_ERROR_CHECK only).
 *
 */
BtStatus GATTC_InitToken(GattClientToken *token, BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * GATTC_Connect()
 *
 *     Create a connection to remote GATT server. If connection is started
 *     successfully or already open the handler is registered to the
 *     connection.
 *
 * Parameters:
 *     token - Pointer to a token initialized using GATTC_InitToken().
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been started and when complete a
 *         GATTCE_CONNECT_COMPLETE or GATTCE_DISCONNECT event will be sent
 *         to the token handler.
 *
 *     BT_STATUS_FAILED - request failed because remDev is not valid
 *
 *      BT_STATUS_IN_PROGRESS - operation failed because the GATT
 *         connection is in the process of disconnecting.
 *
 *     BT_STATUS_INVALID_PARM - token or h.callback in token is 0 or token is
 *        already registered or not initialized properly (XA_ERROR_CHECK only).
 */
BtStatus GATTC_Connect(GattClientToken *token);

/*---------------------------------------------------------------------------
 * GATTC_Disconnect()
 *
 *     Deregister the token from the connection to the remote GATT server.
 *     If the link is LE this may result in the LE ACL link being disconnect.
 *     For BR/EDR this may result in the L2CAP channel being disconnected and
 *     the BR/EDR ACL being disconnected.
 *
 * Parameters:
 *     token - Pointer to token.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_ERROR - request failed because a connection does not exist
 *         or token is not associated with a connection.
 *
 *     BT_STATUS_BUSY - request failed because the token is involved in an
 *         operation.
 *
 *     BT_STATUS_INVALID_PARM - token, link or h.callback in token is 0
 *         (XA_ERROR_CHECK only).
 *
 */
BtStatus GATTC_Disconnect(GattClientToken *token);

/*---------------------------------------------------------------------------
 * GATTC_Cancel()
 *
 *     Cancel the token's current operation. This cannot be used to cancel
 *     Connect or Disconnect.
 *
 * Parameters:
 *     token - Pointer to token that has the current operation cancelled.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request completed. The token is available for
 *         another request. No events will be generated.
 *
 *     BT_STATUS_INVALID_PARM - token is 0 (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - token is not currently in an operation.
 */
BtStatus GATTC_Cancel(GattClientToken *token);

/*---------------------------------------------------------------------------
 * GATTC_IsTokenActive()
 *
 *     Return TRUE if the token is connected is active (attached to a connection)
 *     otherwise return FALSE.
 *
 * Parameters:
 *     token - Pointer to token with handler fields initialized (especially
 *         h.callback).
 *
 * Returns: TRUE if active otherwise FALSE;
 */
BOOL GATTC_IsTokenActive(GattClientToken *token);

/*---------------------------------------------------------------------------
 * GATTC_GetMtu()
 *
 *     Get the MTU for the current GATT connection.
 *
 * Parameters:
 *     token - Pointer to a connected token.
 *
 * Returns:
 *     The MTU or 0 if a GATT connection does not exist.
 */
U16 GATTC_GetMtu(GattClientToken *token);

/*---------------------------------------------------------------------------
 * GATTC_AreServicesCached()
 *
 *     Returns TRUE if the GATT services for the remote device identified by
 *     "devAddr" are in the cache. Otherwise returns FALSE.
 *
 * Parameters:
 *     devAddr - Pointer to a BtDevAddr structure identifying the remote
 *         device. This should be an identity address for efficiency purposes.

 *
 * Returns:
 *     TRUE or FALSE.
 */
BOOL GATTC_AreServicesCached(BtDevAddr *devAddr);

/*---------------------------------------------------------------------------
 * GATTC_SetUseSdpForBrEdr()
 *
 *     Set the flag that controls whether GATTC_GetCachedPrimaryService()
 *     and GATTC_GetCachedPrimaryServiceByUUID() will use SDP or GATT to query
 *     for primary services when running over a BR/EDR link. TRUE means use
 *     SDP while FALSE means use GATT. The default is TRUE.
 *
 *     Note the flag cannot be changed while a GATT connection exists.
 *
 * Parameters:
 *     val - TRUE means use SDP while FALSE means use GATT.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request completed successfully.
 *
 *     BT_STATUS_BUSY - operation failed because a GATT connection exists.
 */
BtStatus GATTC_SetUseSdpForBrEdr(BOOL val);

/*---------------------------------------------------------------------------
 * GATTC_GetUseSdpForBrEdr()
 *
 *     Get the flag that controls whether GATTC_GetCachedPrimaryService()
 *     GATTC_GetCachedPrimaryServiceByUUID() will use SDP or GATT to query
 *     for primary services when running over a BR/EDR link. TRUE means use
 *     SDP while FALSE means use GATT. The default is TRUE.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     TRUE or FALSE
 */
BOOL GATTC_GetUseSdpForBrEdr(void);

/*---------------------------------------------------------------------------
 * GATTC_GetCachedPrimaryService()
 *
 *     Discover a primary service from the cache for a remote GATT server.
 *     This function can be used to discover all primary services from the
 *     cache for a remote GATT server. The search is started by setting the
 *     "stype" parameter to GST_FIRST. Subsequent services are found by
 *     calling the function with the "stype" parameter to GST_NEXT until
 *     BT_STATUS_NOT_FOUND is returned.
 *
 *     If there is no cached information to satisfy this request then
 *     BT_STATUS_PENDING is returned and the service information will be
 *     retrieved from the remote device. When the operation is complete
 *     GATTCE_GET_CACHED_PRIM_SERV will be sent to the token handler. This
 *     function must be called again to retrieve the cached information.
 *
 *     Note that information used to find the next service is stored in the
 *     token. If the token is passed to another function then this information
 *     will be lost or corrupted. Calling this function with stype set to
 *     GST_FIRST will reset the "next" information.
 *
 *     Note that when running over BR/EDR, SDP is used to retrieve the
 *     primary services if the "UseSdpForBrEdr" flag is TRUE. This flag
 *     is set by calling GATTC_SetUseSdpForBrEdr(). The default is to use
 *     SDP. 
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device.
 *
 *     stype - Specifies if the first or next service is to be found.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request completed. "token->result.cacheds" will
 *         contain the primary service information.
 *
 *     BT_STATUS_PENDING - There is no cached information for this device
 *         so an attempt will be made to retrieve it. When complete a
 *         GATTCE_GET_CACHED_PRIM_SERV event will be sent to the token
 *         handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no
 *         connection to the remote GATT server. A connection that
 *         is in the process of coming up or being disconnected is
 *         not considered a connection.
 *
 *     BT_STATUS_NO_RESOURCES - request failed because cached information
 *         for this device does not exist and error occurred while trying
 *         to allocate a cache item.
 *
 *     BT_STATUS_INVALID_PARM - token is 0 or stype is invalid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use GATTC_Cancel
 *         to cancel the current operation for this token.
 *
 *     BT_STATUS_NOT_FOUND - the request failed because all handles have been
 *         searched.  The operation is complete.
 *
 *     BT_STATUS_PROTOCOL_ERROR - operation failed because the response
 *         received by the remote device contains a protocol error.
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform
 *         operation. For example if stype is set to GST_NEXT but
 *         GATTC_GetCachedPrimaryService() has not been called on this token
 *         with stype set to GST_FIRST.
 */
BtStatus GATTC_GetCachedPrimaryService(GattClientToken *token, GattSearchType stype);

/*---------------------------------------------------------------------------
 * GATTC_GetCachedPrimaryServiceByUuid()
 *
 *     Gets a primary service from the cache for a remote GATT server given
 *     the service UUID. This function can be used to discover all services
 *     from the cache for a remote GATT server with the given UUID. The
 *     search is started by initializing token->request.uuid with the desired
 *     uuid and setting the "stype" parameter to GST_FIRST. Subsequent
 *     services are found by calling the function with the "stype" parameter
 *     set to GST_NEXT until BT_STATUS_NOT_FOUND is returned.
 *
 *     If there is no cached information to satisfy this request then
 *     BT_STATUS_PENDING is returned and the service information will be
 *     retrieved from the remote device. When the operation is complete
 *     GATTCE_GET_CACHED_PRIM_SERV_UUID will be sent to the token handler.
 *     This function must be called again to retrieve the cached information.
 *
 *     Note that information used to find the next service is stored in the
 *     token. If the token is passed to another function then this information
 *     will be lost or corrupted. Calling this function with stype set to
 *     GST_FIRST will reset the "next" information.
 *
 *     Note that when running over BR/EDR, SDP is used to retrieve the
 *     primary services if the "UseSdpForBrEdr" flag is TRUE. This flag
 *     is set by calling GATTC_SetUseSdpForBrEdr(). The default is to use
 *     SDP. 
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device. The
 *         "request.uuid" field must be initialized with the UUID of the primary
 *         service to get.
 *
 *     stype - Specifies if the first or a next service is to be found.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request completed. token->result.cacheds will contain
 *         the primary service information.
 *
 *     BT_STATUS_PENDING - There is no cached information for this device
 *         so an attempt will be made to retrieve it. When complete a
 *         GATTCE_GET_CACHED_PRIM_SERV_UUID event will be sent to the token
 *         handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no cached
 *         information for this device and there is no connection to the remote
 *         GATT server in order to retrieve it. This includes a connection
 *         that is in the process of coming up or being disconnected.
 *
 *     BT_STATUS_NO_RESOURCES - request failed because cached information
 *         for this device does not exist and error occurred while trying
 *         to allocate a cache item.
 *
 *     BT_STATUS_INVALID_PARM - token is 0, stype is invalid or token->request.uuid
 *         is invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use GATTC_Cancel
 *         to cancel the current operation for this token.
 *
 *     BT_STATUS_NOT_FOUND - the request failed because all handles have been
 *         searched. The operation is complete.
 *
 *     BT_STATUS_PROTOCOL_ERROR - operation failed because the response
 *         received by the remote device contains a protocol error.
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform
 *         operation. For example if stype is set to GST_NEXT but
 *         GATTC_GetPrimaryServiceByUuid() has not been called on this token
 *         with stype set to GST_FIRST.
 *
 */
BtStatus GATTC_GetCachedPrimaryServiceByUuid(GattClientToken *token, GattSearchType stype);

/*---------------------------------------------------------------------------
 * GATTC_GetCachedIncludedService()
 *
 *     Gets an included service declaration within a service definition from
 *     the cache for a remote GATT server. This function can be used to
 *     discover all included services within a service definition. The search
 *     is started by initializing token->request.servRef with the reference
 *     to the desired primary service and setting the "stype" parameter to
 *     GST_FIRST. A reference to a primary service is obtained by calling
 *     GATTC_GetCachedPrimaryService() or GATTC_GetCachedPrimaryServiceByUuid().
 *     Subsequent included services are found by calling the function with
 *     the "stype" parameter set to GST_NEXT until BT_STATUS_NOT_FOUND is
 *     returned.
 *
 *     If there is no cached information to satisfy this request then
 *     BT_STATUS_PENDING is returned and the service information will be
 *     retrieved from the remote device. When the operation is complete
 *     GATTCE_GET_CACHED_INCLUDED_SERV event will be sent to the token handler.
 *     This function must be called again to retrieve the cached information.
 *
 *     Note that information used to find the next service is stored in the
 *     token. If the token is passed to another function then this information
 *     will be lost or corrupted. Calling this function with stype set to
 *     GST_FIRST will reset the "next" information.
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device. The
 *         "reqest.servRef" field must be initialized with a reference to the
 *         service of the service to be searched for included services.
 *
 *     stype - Specifies if the first or next service is to be found.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request completed. "token->result.gcis" contains
 *         the included service information.
 *
 *     BT_STATUS_PENDING - There is no cached information for this device
 *         so an attempt will be made to retrieve it. When complete a
 *         GATTCE_GET_CACHED_INCLUDED_SERV event will be sent to the token
 *         handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no cached
 *         information for this device and there is no connection to the remote
 *         GATT server in order to retrieve it. This includes a connection
 *         that is in the process of coming up or being disconnected.
 *
 *     BT_STATUS_NO_RESOURCES - request failed because cached information
 *         for this device does not exist and error occurred while trying
 *         to allocate a cache item.
 *
 *     BT_STATUS_INVALID_PARM - token is 0, stype is invalid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use
 *         GATTC_Cancel to cancel the current operation for this token.
 *
 *     BT_STATUS_NOT_FOUND - the request failed because all handles have been
 *         searched. The operation is complete.
 *
 *     BT_STATUS_PROTOCOL_ERROR - operation failed because the response
 *         received by the remote device contains a protocol error.
 *
 *     BT_STATUS_FAILED - token is invalid (e.g. token->servRef is
 *         invalid) or token is in the wrong state to perform  operation.
 *         For example if stype is set to GST_NEXT but
 *         GATTC_GetCachedIncludedService() has not been called on this
 *         token with stype set to GST_FIRST.
 *
 */
BtStatus GATTC_GetCachedIncludedService(GattClientToken *token, GattSearchType stype);

/*---------------------------------------------------------------------------
 * GATTC_GetCachedCharacteristic()
 *
 *     Gets a characteristic declaration within a service definition from the
 *     cache for a remote GATT server. This function can be used to discover
 *     all the characteristic declarations within a service definition. The
 *     search is started by initializing token->request.servRef with the
 *     reference to the desired service and setting the "stype" parameter to
 *     GST_FIRST. A reference to a service is obtained by calling
 *     GATTC_GetCachedPrimaryService(), GATTC_GetCachedPrimaryServiceByUuid(),
 *     or GATTC_GetCachedIncludedService. Subsequent characteristics are found
 *     by calling the function with the "stype" parameter set to GST_NEXT
 *     until BT_STATUS_NOT_FOUND is returned.
 *
 *     If there is no cached information to satisfy this request then
 *     BT_STATUS_PENDING is returned and the characteristic information will
 *     be retrieved from the remote device. When the operation is complete
 *     GATTCE_GET_CACHED_CHARACTERISTIC event will be sent to the token
 *     handler. This function must be called again to retrieve the cached
 *     information.
 *
 *     Note that information used to find the next characteristic is stored in
 *     the token. If the token is passed to another function then this
 *     information will be lost or corrupted. Calling this function with stype
 *     set to GST_FIRST will reset the "next" information.
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device. The
 *         "reqest.servRef" field must be initialized with a reference to the
 *         service of the service to be searched for included services.
 *
 *     stype - Specifies if the first or next service is to be found.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request completed. token->result.gcc contains the
 *         characteristic information.
 *
 *     BT_STATUS_PENDING - There is no cached information for this device
 *         so an attempt will be made to retrieve it. When complete a
 *         GATTCE_GET_CACHED_CHARACTERISTIC event will be sent to the token
 *         handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no cached
 *         information for this device and there is no connection to the remote
 *         GATT server in order to retrieve it. This includes a connection
 *         that is in the process of coming up or being disconnected.
 *
 *     BT_STATUS_NO_RESOURCES - request failed because cached information
 *         for this device does not exist and an error occurred while trying
 *         to allocate a cache item.
 *
 *     BT_STATUS_INVALID_PARM - token is 0, stype is invalid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use
 *         GATTC_Cancel to cancel the current operation for this token.
 *
 *     BT_STATUS_NOT_FOUND - the request failed because all handles have been
 *         searched. The operation is complete.
 *
 *     BT_STATUS_PROTOCOL_ERROR - operation failed because the response
 *         received by the remote device contains a protocol error.
 *
 *     BT_STATUS_FAILED - token is invalid (e.g. token->servRef is
 *         invalid) or token is in the wrong state to perform  operation.
 *         For example if stype is set to GST_NEXT but
 *         GATTC_GetCachedCharacteristic() has not been called on this
 *         token with stype set to GST_FIRST.
 *
 */
BtStatus GATTC_GetCachedCharacteristic(GattClientToken *token, GattSearchType stype);

/*---------------------------------------------------------------------------
 * GATTC_GetCachedCharacteristicByUuid()
 *
 *     Gets a characteristic declaration within a service from the cache given
 *     its UUID for a remote GATT server. This function can be used to
 *     discover all the characteristic declarations with a given UUID within
 *     a service definition. The search is started by initializing
 *     token->request.gccbu with the reference to the desired service along
 *     with the desired UUID and setting the "stype" parameter to GST_FIRST.
 *     A reference to a service is obtained by calling
 *     GATTC_GetCachedPrimaryService(), GATTC_GetCachedPrimaryServiceByUuid(),
 *     or GATTC_GetCachedIncludedService. Subsequent characteristics are
 *     found by calling the function with the "stype" parameter set to
 *     GST_NEXT until BT_STATUS_NOT_FOUND is returned.
 *
 *     If there is no cached information to satisfy this request then
 *     BT_STATUS_PENDING is returned and the characteristic information will
 *     be retrieved from the remote device. When the operation is complete
 *     GATTCE_GET_CACHED_CHARACTERISTIC_UUID event will be sent to the
 *     token handler. This function must be called again to retrieve the
 *     cached information.
 *
 *     Note that information used to find the next characteristic is stored in
 *     the token. If the token is passed to another function then this
 *     information will be lost or corrupted. Calling this function with stype
 *     set to GST_FIRST will reset the "next" information.
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device. The
 *         "reqest.gccbu" field must be initialized with the a reference to the
 *         desired service and the desired uuid.
 *
 *     stype - Specifies if the first or next service is to be found.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request completed. token->result.gcc contains the
 *         characteristic information.
 *
 *     BT_STATUS_PENDING - There is no cached information for this device
 *         so an attempt will be made to retrieve it. When complete a
 *         GATTCE_GET_CACHED_CHARACTERISTIC_UUID event will be sent to the
 *         token handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no cached
 *         information for this device and there is no connection to the remote
 *         GATT server in order to retrieve it. This includes a connection
 *         that is in the process of coming up or being disconnected.
 *
 *     BT_STATUS_NO_RESOURCES - request failed because cached information
 *         for this device does not exist and an error occurred while trying
 *         to allocate a cache item.
 *
 *     BT_STATUS_INVALID_PARM - token is 0, stype is invalid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use
 *         GATTC_Cancel to cancel the current operation for this token.
 *
 *     BT_STATUS_NOT_FOUND - the request failed because all handles have been
 *         searched. The operation is complete.
 *
 *     BT_STATUS_PROTOCOL_ERROR - operation failed because the response
 *         received by the remote device contains a protocol error.
 *
 *     BT_STATUS_FAILED - token is invalid (e.g. token->servRef is
 *         invalid) or token is in the wrong state to perform  operation.
 *         For example if stype is set to GST_NEXT but
 *         GATTC_GetCachedCharacteristicByUuid() has not been called on
 *         this token with stype set to GST_FIRST.
 *
 */
BtStatus GATTC_GetCachedCharacteristicByUuid(GattClientToken *token, GattSearchType stype);

/*---------------------------------------------------------------------------
 * GATTC_GetCachedCharacteristicDescriptor()
 *
 *     Gets a characteristic descriptor declaration within a characteristic
 *     definition from the cache for a remote GATT server. This function can
 *     be used to discover all the characteristic descriptors within a
 *     characteristic definition. The search is started by
 *     initializing token->request.gccd with a reference to the desired
 *     service definition containing the characteristic definition and a
 *     reference to the characteristic definition and then setting the "stype"
 *     parameter to GST_FIRST. A reference to a service is obtained by calling
 *     GATTC_GetCachedPrimaryService(), GATTC_GetCachedPrimaryServiceByUuid(),
 *     or GATTC_GetCachedIncludedService. A reference to a characteristic
 *     definition is obtained by calling GATTC_GetCachedCharacteristic(), or
 *     GATTC_GetCachedCharacteristicByUuid(). Subsequent characteristics
 *     descriptors are found by calling the function with the "stype"
 *     parameter set to GST_NEXT until BT_STATUS_NOT_FOUND is returned.
 *
 *     If there is no cached information to satisfy this request then
 *     BT_STATUS_PENDING is returned and the characteristic information will
 *     be retrieved from the remote device. When the operation is complete
 *     GATTCE_GET_CACHED_CHARACTERISTIC event will be sent to the token
 *     handler. This function must be called again to retrieve the cached
 *     information.
 *
 *     Note that information used to find the next service is stored in the
 *     token. If the token is passed to another function then this information
 *     will be lost or corrupted. Calling this function with stype set to
 *     GST_FIRST will reset the "next" information.
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device, info
 *         identifying the characteristic definition, info for getting the "next"
 *         characteristic declaration, storage for the result and handler to receive
 *         events if  BT_STATUS_PENDING is returned.
 *
 *     stype - Specifies if the first or next service is to be found.
 *
 * Returns:
  *     BT_STATUS_SUCCESS - request completed. token->result.gcd contains the
 *         characteristic descriptor information.
 *
 *     BT_STATUS_PENDING - There is no cached information for this device
 *         so an attempt will be made to retrieve it. When complete a
 *         GATTCE_GET_CACHED_CHARACTER_DESC  event will be sent to the
 *         token handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no cached
 *         information for this device and there is no connection to the remote
 *         GATT server in order to retrieve it. This includes a connection
 *         that is in the process of coming up or being disconnected.
 *
 *     BT_STATUS_NO_RESOURCES - request failed because cached information
 *         for this device does not exist and an error occurred while trying
 *         to allocate a cache item.
 *
 *     BT_STATUS_INVALID_PARM - token is 0, stype is invalid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use
 *         GATTC_Cancel to cancel the current operation for this token.
 *
 *     BT_STATUS_NOT_FOUND - the request failed because all handles have been
 *         searched. The operation is complete.
 *
 *     BT_STATUS_PROTOCOL_ERROR - operation failed because the response
 *         received by the remote device contains a protocol error.
 *
 *     BT_STATUS_FAILED - token is invalid (e.g. token->charRef is
 *         invalid) or token is in the wrong state to perform  operation.
 *         For example if stype is set to GST_NEXT but
 *         GATTC_GetCachedCharacteristicByUuid() has not been called on
 *         this token with stype set to GST_FIRST.
 */
BtStatus GATTC_GetCachedCharacteristicDescriptor(GattClientToken *token, GattSearchType stype);

/*---------------------------------------------------------------------------
 * GATTC_ClearRecordFromCache()
 *
 *     Clear the cache record for the remote device identified by "devAddr".
 *     This function should be called when the local device detects that the
 *     bond with a remote device has been lost (e.g. the local device believes
 *     the bond exists but the remote device does not) or any other time the
 *     cache is believed to be invalid.
 *
 *     Note that any pending tokens that reference the cache record such as
 *     GATTC_GetCachedIncludedService(), GATTC_GetCachedCharacterictic(),
 *     GATTC_GetCachedCharacteristicByUuid and
 *     GATTC_GetCachedCharacteristicDescriptor() will be aborted with
 *     "token->e.status" set to BT_STATUS_CACHE_CLEARED.
 *
 *
 * Parameters:
 *     devAddr - pointer to structure identifying the remote device.
 *         This should be an identity address for efficiency purposes.
 *
 *     clearGattService - triggers if the GATT service information is
 *         cleared for the record associated with this remote device.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request completed.
 *
 *     BT_STATUS_INVALID_PARM - devAddr is 0 (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - the request failed because a cache
 *         device record does not exist for the device identified by
 *         "devAddr"
 *
 */
BtStatus GATTC_ClearRecordFromCache(BtDevAddr *devAddr, BOOL clearGattService);

/*---------------------------------------------------------------------------
 * GATTC_GetPrimaryService()
 *
 *     Discover a primary service from the remote GATT server. This function
 *     can be used to discover all primary services on the remote GATT server.
 *     The search is started by setting the "stype" parameter to GST_FIRST.
 *     Subsequent services are found by calling the function with the "stype"
 *     parameter to GST_NEXT until the GATTCE_GET_PRIM_SERV_COMPLETE event
 *     is sent with token->errCode set to ATTEC_ATTRIBUTE_NOT_FOUND or
 *     BT_STATUS_NOT_FOUND is returned. Note that information  used to find
 *     the next service is stored in the token. If the token is passed to
 *     another function then this information will be lost or corrupted.
 *     Calling this function with stype set to GST_FIRST will reset the "next"
 *     information.
 *
 *     Note that this function does not use the client cache nor does it fill
 *     the cache. It will cause traffic to be exchanged over the air. It is
 *     recommended that GATTC_GetCachedPrimaryService() be used instead.
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device.
 *
 *     stype - Specifies if the first or next service is to be found.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request completed. "token->result.gps" will
 *         contain the primary service information.
 *
 *     BT_STATUS_PENDING - request was started and when complete a
 *         GATTCE_GET_PRIM_SERV_COMPLETE event will be sent to the token
 *         handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT server including a connection that is in the process
 *         of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - token is 0 or stype is invalid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use GATTC_Cancel
 *         to cancel the current operation for this token.
 *
 *     BT_STATUS_NOT_FOUND - the request failed because all handles have been
 *         searched.  The operation is complete.
 *
 *     BT_STATUS_PROTOCOL_ERROR - operation failed because the response
 *         received by the remote device contains a protocol error.
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform
 *         operation. For example if stype is set to GST_NEXT but
 *         GATTC_GetPrimaryService() has not been called on this token
 *         with stype set to GST_FIRST.
 */
BtStatus GATTC_GetPrimaryService(GattClientToken *token, GattSearchType stype);

/*---------------------------------------------------------------------------
 * GATTC_GetPrimaryServiceByUuid()
 *
 *     Gets a primary service from the remote GATT server given the service
 *     UUID. This function can be used to discover all services on the remote
 *     GATT server with the given UUID. The search is started by initializing
 *     token->request.uuid with the desired uuid and setting the "stype"
 *     parameter to GST_FIRST. Subsequent services are found calling the
 *     function with the "stype" parameter to GST_NEXT until the
 *     GATTCE_GET_PRIM_SERV_UUID_COMPLETE event is sent with token->errCode
 *     set to ATTEC_ATTRIBUTE_NOT_FOUND or BT_STATUS_NOT_FOUND is returned.
 *     Note that information  used to find the next service is stored in the
 *     token. If the token is passed to another function then this information
 *     will be lost or corrupted. Calling this function with stype set to
 *     GST_FIRST will reset the "next"  information.
 *
 *     Note that this function does not use the client cache nor does it fill
 *     the cache. It will cause traffic to be exchanged over the air. It is
 *     recommended that GATTC_GetCachedPrimaryServiceByUuid() be used instead.
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device. The
 *         "request.uuid" field must be initialized with the UUID of the primary
 *         service to get.
 *
 *     stype - Specifies if the first or a next service is to be found.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request completed. token->result.gpsbu will contain
 *         the primary service information.
 *
 *     BT_STATUS_PENDING - request was started and when complete a
 *         GATTCE_GET_PRIM_SERV_UUID_COMPLETE event will be sent to the token
 *         handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT server including a connection that is in the process
 *         of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - token is 0, stype is invalid or token->request.uuid
 *         is invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use GATTC_Cancel
 *         to cancel the current operation for this token.
 *
 *     BT_STATUS_NOT_FOUND - the request failed because all handles have been
 *         searched. The operation is complete.
 *
 *     BT_STATUS_PROTOCOL_ERROR - operation failed because the response
 *         received by the remote device contains a protocol error.
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform
 *         operation. For example if stype is set to GST_NEXT but
 *         GATTC_GetPrimaryServiceByUuid() has not been called on this token
 *         with stype set to GST_FIRST.
 *
 */
BtStatus GATTC_GetPrimaryServiceByUuid(GattClientToken *token, GattSearchType stype);

/*---------------------------------------------------------------------------
 * GATTC_GetIncludedService()
 *
 *     Gets an included service declaration within a service definition from
 *     the remote GATT server. This function can be used to discover all
 *     included services within a service definition. The search is started
 *     by initializing token->request.servId with the starting handle
 *     and ending handle of the desired service and setting the "stype"
 *     parameter to GST_FIRST. Subsequent services are found by calling the
 *     function with the "stype" parameter to GST_NEXT until
 *     BT_STATUS_NOT_FOUND is returned or the
 *     GATTCE_GET_INCLUDED_SERV_COMPLETE event is sent with token->errCode
 *     set to ATTEC_ATTRIBUTE_NOT_FOUND. Note that information used to find the
 *     next service is stored in the token. If the token is passed to another
 *     function then this information may be lost or corrupted. Calling this
 *     function with stype set to GST_FIRST will reset the "next" information.
 *
 *     Note that this function does not use the client cache nor does it fill
 *     the cache. It will cause traffic to be exchanged over the air. It is
 *     recommended that GATTC_GetCachedIncludedService() be used instead.
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device. The
 *         "reqest.servId" field must be initialized with the start and end handle
 *         of the service to be searched for included services.
 *
 *     stype - Specifies if the first or next service is to be found.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request completed. "token->result.gis" contains
 *         the included service information.
 *
 *     BT_STATUS_PENDING - request was started and when complete a
 *         GATTCE_GET_INCLUDED_SERV_COMPLETE event will be sent to the token
 *         handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT server including a connection that is in the process
 *         of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - token is 0, stype is invalid or token->servId
 *         is invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use
 *         GATTC_Cancel to cancel the current operation for this token.
 *
 *     BT_STATUS_NOT_FOUND - the request failed because all handles have been
 *         searched. The operation is complete.
 *
 *     BT_STATUS_PROTOCOL_ERROR - operation failed because the response
 *         received by the remote device contains a protocol error.
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform
 *         operation. For example if stype is set to GST_NEXT but
 *         GATTC_GetIncludedService() has not been called on this token
 *         with stype set to GST_FIRST.
 *
 */
BtStatus GATTC_GetIncludedService(GattClientToken *token, GattSearchType stype);

/*---------------------------------------------------------------------------
 * GATTC_GetCharacteristic()
 *
 *     Gets a characteristic declaration within a service from a remote
 *     GATT server. This function can be used to discover all the characteristic
 *     declarations within a service definition. The search is started by
 *     initializing token->request.servId with the starting handle and ending
 *     handle of the desired service and setting the "stype" parameter to
 *     GST_FIRST. Subsequent characteristics are found by calling the function
 *     with the "stype" parameter to GST_NEXT until BT_STATUS_NOT_FOUND is
 *     returned or the GATTCE_GET_CHARACTERISTIC_COMPLETE event is sent with
 *     "token->errCode" set to ATTEC_ATTRIBUTE_NOT_FOUND. Note that information
 *     used to find the next characteristic is stored in the token. If the
 *     token is passed to another function then this information may be lost
 *     or corrupted. Calling this function with stype set to GST_FIRST will
 *     reset the "next" information.
 *
 *     Note that this function does not use the client cache nor does it fill
 *     the cache. It will cause traffic to be exchanged over the air. It is
 *     recommended that GATTC_GetCachedCharacteristic() be used instead.
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device. The
 *         "reqest.servId" field must be initialized with the start and end handle
 *         of the service to be searched for included services.
 *
 *     stype - Specifies if the first or next service is to be found.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request completed. token->result.gc contains the
 *         characteristic information.
 *
 *     BT_STATUS_PENDING - request was started and when complete a
 *         GATTCE_GET_CHARACTERISTIC_COMPLETE event will be sent to the
 *         token handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT server including a connection that is in the process
 *         of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - token is 0, stype is invalid 0r token->servId
 *         is invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use
 *         GATTC_Cancel to cancel the current operation for this token.
 *
 *     BT_STATUS_NOT_FOUND - the request failed because all handles have been
 *         searched. The operation is complete.
 *
 *     BT_STATUS_PROTOCOL_ERROR - operation failed because the response
 *         received by the remote device contains a protocol error.
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform
 *         operation. For example if stype is set to GST_NEXT but
 *         GATTC_GetCharacteristic() has not been called on this token
 *         with stype set to GST_FIRST.
 *
 */
BtStatus GATTC_GetCharacteristic(GattClientToken *token, GattSearchType stype);

/*---------------------------------------------------------------------------
 * GATTC_GetCharacteristicByUuid()
 *
 *     Gets a characteristic within a service given its UUID from a remote
 *     GATT server. This function can be used to discover all the characteristics
 *     with a given UUID within a service definition. The search is started by
 *     initializing token->request.gcbu with the starting handle and ending
 *     handle of the desired service along with the characteristic UUID and
 *     setting the "stype" parameter to GST_FIRST. Subsequent characteristics
 *     are found by calling the function with the "stype" parameter to GST_NEXT
 *     until BT_STATUS_NOT_FOUND is returned or the
 *     GATTCE_GET_CHARACTER_UUID_COMPLETE event is sent with "token->errCode"
 *     set to ATTEC_ATTRIBUTE_NOT_FOUND. Note that information used to find the
 *     next characteristic is stored in the token. If the token is passed to
 *     another function then this information may be lost or corrupted.
 *     Calling this function with stype set to GST_FIRST will reset the "next"
 *     information.
 *
 *     Note that this function does not use the client cache nor does it fill
 *     the cache. It will cause traffic to be exchanged over the air. It is
 *     recommended that GATTC_GetCachedCharacteristicByUuid() be used instead.
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device. The
 *         "reqest.gcbu" field must be initialized with the start and end handle
 *         of the service along with the characteristic UUID.
 *
 *     stype - Specifies if the first or next service is to be found.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request completed. "token->result.gc" contains
 *         the characteristic information.
 *
 *     BT_STATUS_PENDING - request was started and when complete a
 *         GATTCE_GET_CHARACTER_UUID_COMPLETE event will be sent to the
 *         token handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT server including a connection that is in the process
 *         of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - token is 0, stype is invalid or token->gcbu
 *         is invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use
 *         GATTC_Cancel to cancel the current operation for this token.
 *
 *     BT_STATUS_NOT_FOUND - the request failed because all handles have been
 *         searched. The operation is complete.
 *
 *     BT_STATUS_PROTOCOL_ERROR - operation failed because the response
 *         received by the remote device contains a protocol error.
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform
 *         operation. For example if stype is set to GST_NEXT but
 *         GATTC_GetCharacteristicByUuid() has not been called on this token
 *         with stype set to GST_FIRST.
 *
 */
BtStatus GATTC_GetCharacteristicByUuid(GattClientToken *token, GattSearchType stype);

/*---------------------------------------------------------------------------
 * GATTC_GetCharacteristicDescriptor()
 *
 *     Gets a characteristic descriptor declaration within a characteristic
 *     definition of a service from a remote GATT server. This function can be
 *     used to discover all the characteristic descriptors within a
 *     characteristic definition of a service. The search is started by
 *     initializing token->request.gcd with "startHandle" set to the
 *     handle of the desired characteristic's value declaration + 1 along with
 *     "endHandle" with the ending handle of the characteristic definition and
 *     setting the "stype" parameter to GST_FIRST.
 *
 *     The endHandle of the characteristic definition is the handle of the next
 *     characteristic declaration - 1 or the endHandle of the service if this is
 *     the last characteristic in the service. GATTC_GetCharacteristic() can be
 *     used to get the handles of all the characteristic declarations.
 *
 *     Subsequent characteristics declarations are found by calling the function
 *     with the "stype" parameter to GST_NEXT until BT_STATUS_NOT_FOUND is
 *     returned or the GATTCE_GET_CHARACTER_DESC_COMPLETE event is sent with
 *     "token->errCode" set to ATTEC_ATTRIBUTE_NOT_FOUND. Note that information used
 *     to find the next characteristic descriptor is stored in the token. If the
 *     token is passed to another function then this information may be lost or
 *     corrupted. Calling this function with stype set to GST_FIRST will reset
 *     the "next" information.
 *
 *     Note that this function does not use the client cache nor does it fill
 *     the cache. It will cause traffic to be exchanged over the air. It is
 *     recommended that GATTC_GetCachedCharacteristicDescriptor() be used
 *     instead.
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device, info
 *         identifying the characteristic definition, info for getting the "next"
 *         characteristic declaration, storage for the result and handler to receive
 *         events if  BT_STATUS_PENDING is returned.
 *
 *     stype - Specifies if the first or next service is to be found.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request completed. "token->result.gcd" contains the
 *         characteristic descriptor information.
 *
 *     BT_STATUS_PENDING - request was started and when complete a
 *         GATTCE_GET_CHARACTER_DESC_COMPLETE event will be sent to the
 *         token handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT server including a connection that is in the process
 *         of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - token is 0, stype is invalid or token->gcd
 *         is invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use
 *         GATTC_Cancel() to cancel the current operation for this token.
 *
 *     BT_STATUS_NOT_FOUND - the request failed because all handles have been
 *         searched. The operation is complete.
 *
 *     BT_STATUS_PROTOCOL_ERROR - operation failed because the response
 *         received by the remote device contains a protocol error.
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform
 *         operation. For example if stype is set to GST_NEXT but
 *         GATTC_GetCharacteristicDescriptor() has not been called on this
 *         token with stype set to GST_FIRST.
 *
 */
BtStatus GATTC_GetCharacteristicDescriptor(GattClientToken *token, GattSearchType stype);

/*---------------------------------------------------------------------------
 * GATTC_GetCharacteristicValue()
 *
 *     Gets a characteristic value given the characteristic value handle from
 *     a remote GATT server. The token.request.charHandle must be initialized
 *     with the characteristic value handle. The result is returned in
 *     token.result.gcv. If token.result.gcv.more is TRUE then only a portion
 *     of the value has been returned and GATTC_GetCharacteristicLongValue()
 *     can be used to read the rest of the value.
 *
 *     This function can be used to get a characteristic descriptor from a
 *     remote GATT server by setting token.request.charHandle to the
 *     characteristic descriptor declaration's handle.
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device,
 *         characteristic value handle, storage for the result and handler to
 *         receive events.
 *
 * Returns:
 *     BT_STATUS_PENDING - request was started and when complete a
 *         GATTCE_GET_CHARACTER_VAL_COMPLETE event will be sent to the
 *         token handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT server including a connection that is in the process
 *         of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - token is 0 or token->request.charHandle is 0
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use
 *         GATTC_Cancel() to cancel the current operation for this token.
 *
 *     BT_STATUS_PROTOCOL_ERROR - operation failed because the response
 *         received by the remote device contains a protocol error.
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform
 *         operation.
 *
 */
BtStatus GATTC_GetCharacteristicValue(GattClientToken *token);

/*---------------------------------------------------------------------------
 * GATTC_GetCharacteristicValueByUuid()
 *
 *     Gets a characteristic value given the characteristic UUID from a remote
 *     GATT server. This function can be used to read all the characteristic
 *     values with a given UUID within a service definition. The search is
 *     started by initializing token->request.gcbu with the starting handle
 *     and ending handle of the desired service along with the characteristic
 *     UUID and setting the "stype" parameter to GST_FIRST. Note the starting
 *     handle and ending handle can also be set to range of the characteristic
 *     definition (see GATTC_GetCharacteristicByUuid for more info on this
 *     range).
 *
 *     Subsequent characteristic values are found by calling the function
 *     with the "stype" parameter to GST_NEXT until token->errCode is set to
 *     ATTEC_ATTRIBUTE_NOT_FOUND. Note that information used to find the next
 *     characteristic is stored in the token. If the token is passed to another
 *     function then this information will be lost or corrupted. Calling this
 *     function with stype set to GST_FIRST will reset the "next" information.
 *
 *     The result is returned in token.result.gcvbu. If token.result.gcvbu.more
 *     is TRUE then only a portion of the value has been returned and
 *     GATTC_GetCharacteristicLongValue() can be used to read the rest of the value.
 *     Note that if GATTC_GetCharacteristicLongValue() is called the "next" information
 *     will not be lost therefore it is possible to read the remaining portion of
 *     a value before reading the "next" characteristic value.
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device,
 *         characteristic value handle, storage for the result and handler to
 *         receive events.
 *
 *     stype - Specifies if the first or next service is to be searched.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request completed. "token->errCode" indicates
 *         if the operation was successful or not. If successful
 *         token->result.gcvbu will contain the characteristic value information.
 *
 *     BT_STATUS_PENDING - request was started and when complete a
 *         GATTCE_GET_CHARACTER_VAL_UUID_COMPLETE event will be sent to the
 *         token handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT server including a connection that is in the process
 *         of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - token is 0 or token->request.charHandle is 0
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use
 *         GATTC_Cancel() to cancel the current operation for this token.
 *
 *     BT_STATUS_PROTOCOL_ERROR - operation failed because the response
 *         received by the remote device contains a protocol error.
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform
 *         operation.
 *
 */
BtStatus GATTC_GetCharacteristicValueByUuid(GattClientToken *token, GattSearchType stype);

/*---------------------------------------------------------------------------
 * GATTC_GetCharacteristicLongValue()
 *
 *     Gets a characteristic long value given the characteristic value handle
 *     from a remote GATT server. The token.request.longVal must be initialized
 *     with the characteristic value handle and value offset. The result is
 *     returned in token.result.gcv. A value offset of 0 will retrieve the
 *     first portion of the value. To retrieve the "next" portion of a value
 *     set the offset to the token->result.gcv.len field of a previous call to
 *     GATTC_GetCharacterValue() or GATTC_GetCharacteristicLongValue(). If
 *     token.result.gcv.more is TRUE then likely more data exists to be
 *     retrieved. GATTC_GetCharacteristicLongValue() can be used to get the
 *     remaining data. The last portion of the data is received when
 *     token.result.gcv.more is FALSE. The last portion of the data may be zero
 *     length if the previous portion fit exactly into a GATT packet. If the value
 *     is not really a long value then token->errCode will be set to
 *     ATTEC_ATTRIBUTE_NOT_LONG and GATTC_GetCharacteristicValue() must be used
 *     to get the value.
 *
 *     This function can be used to get a long characteristic descriptor
 *     from a remote GATT server by setting token.request.charHandle to the
 *     characteristic descriptor declaration's handle.
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device,
 *         characteristic value handle plus offset, storage for the result and
 *         handler to receive events.
 *
 * Returns:
 *     BT_STATUS_PENDING - request was started and when complete a
 *         GATTCE_GET_CHARACTER_LONG_VAL_COMPLETE event will be sent to the
 *         token handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT server including a connection that is in the process
 *         of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - token is 0 or token->request.charHandle is 0
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use
 *         GATTC_Cancel() to cancel the current operation for this token.
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform
 *         operation.
 *
 */
BtStatus GATTC_GetCharacteristicLongValue(GattClientToken *token);

/*---------------------------------------------------------------------------
 * GATTC_GetMultipleCharacteristicValues()
 *
 *     Gets multiple characteristic values given an array of characteristic
 *     value handles from a remote GATT server. The token.request.handleArray
 *     must be initialized with a pointer to a handle array and the number
 *     of items in the array. The result is returned in token.result.gcv as a
 *     "set of values". The "set of values" is limited to ATT_MTU-1 octets so
 *     if the "set of values" is greater than ATT_MTU-1 then only ATT_MTU-1
 *     octets are returned. The caller is responsible for knowing the size
 *     of each value based on the order of the handles in the handle array.
 *
 *     The attribute values in the response do not have to be the same size
 *     and the order of the handles in the handle array does not have to
 *     be in any specific order. If any of the attributes have insufficient
 *     authorization or security to read the value then an error will be
 *     returned. If any of the handles are invalid then an error will be sent.
 *     If an error is sent the first handle causing the error will be returned
 *     in token->errHandle.
 *
 *     Note the caller should not use this function if the "set of values" could
 *     be ATT_MTU-1 octets because it will not be possible to determine if the
 *     last attribute value is complete or if it overflowed.
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device,
 *         characteristic value handle, storage for the result and handler to
 *         receive events.
 *
 * Returns:
 *     BT_STATUS_PENDING - request was started and when complete a
 *         GATTCE_GET_MULTI_CHARACTER_VAL_COMPLETE event will be sent to the
 *         token handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT server including a connection that is in the process
 *         of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - token is 0, token->request.handleArray.handles
 *         is 0 or token->request.handleArray.num < 2 (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use
 *         GATTC_Cancel() to cancel the current operation for this token.
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform
 *         operation.
 *
 */
BtStatus GATTC_GetMultipleCharacteristicValues(GattClientToken *token);

/*---------------------------------------------------------------------------
 * GATTC_WriteCharacteristicValueNoResp()
 *
 *     Write a characteristic value with the given characteristic value handle
 *     on a remote GATT server. There will be no acknowledgement from the
 *     server and no error is generated by the server. The
 *     token.request.writeVal must be initialized with the characteristic
 *     value handle and the value to write. The token handler will be called
 *     with GATTCE_WRITE_CHARACTER_VAL_NO_RESP when the token is available.
 *
 *     Note that this function only writes the first ATT_MTU - 3 octets of
 *     the characteristic value. It cannot be used to write long characteristic
 *     values; instead use GATTC_WriteCharacteristicValue(). Use
 *     GATTC_GetMtu() to determine the MTU size of the GATT link.
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device,
 *         characteristic value handle and value to write, and handler to
 *         receive events.
 *
 * Returns:
 *     BT_STATUS_PENDING - request was started and when token is available a
 *         GATTCE_WRITE_CHARACTER_VAL_NO_RESP event will be sent to the
 *         token handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT server including a connection that is in the process
 *         of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - token is 0 or token->request.writeVal.handle
 *         is 0 or len > ATT_MAX_ATTRIB_VALUE_SIZE (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use
 *         GATTC_Cancel() to cancel the current operation for this token.
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform
 *         operation or token->request.writeVal.len > mtu - 3.
 *
 */
BtStatus GATTC_WriteCharacteristicValueNoResp(GattClientToken *token);

/*---------------------------------------------------------------------------
 * GATTC_WriteCharacteristicValueSigned()
 *
 *     Write a characteristic value with the given characteristic value handle
 *     on a remote GATT server. There will be no acknowledgement from the
 *     server and no error is generated by the server. This function can only 
 *     be used when the client and server share a bond. If the connection is 
 *     encrypted, a normal write without response is performed. 
 *     token.request.writeVal must be initialized with the characteristic
 *     value handle and the value to write. The token handler will be called
 *     with GATTCE_WRITE_CHARACTER_VAL_SIGNED when the token is available.
 *
 *     Note that this function only writes the first ATT_MTU - 15 octets of
 *     the characteristic value. It cannot be used to write long characteristic
 *     values. Use GATTC_GetMtu() to determine the MTU size of the GATT link.
 *     This function can be used on BR/EDR link, but because it is always
 *     encrypted, it will only perform a normal write without response.
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device,
 *         characteristic value handle and value to write, and handler to
 *         receive events.
 *
 * Returns:
 *     BT_STATUS_PENDING - request was started and when token is available a
 *         GATTCE_WRITE_CHARACTER_VAL_SIGNED event will be sent to the
 *         token handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT server including a connection that is in the process
 *         of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - token is 0 or token->request.writeVal.handle
 *         is 0 (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use GATTC_Cancel
 *         to cancel the current operation for this token.
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform
 *     operation or token->request.writeVal.len > mtu - 15
 *
 */
BtStatus GATTC_WriteCharacteristicValueSigned(GattClientToken *token);

/*---------------------------------------------------------------------------
 * GATTC_WriteCharacteristicValue()
 *
 *     Write a characteristic value with the given characteristic value handle
 *     on a remote GATT server. This function can be used to write both long
 *     and short characteristic values.  The token.request.writeVal must be
 *     initialized with the characteristic value handle, the value offset, and
 *     value to write. An offset is used only when writing long values
 *     (larger than ATT_MTU - 3 in size), however, it must be set to zero
 *     for all other cases. GATTC_GetMtu() can be used to determine the MTU
 *     size of the GATT link. The token handler will be called with
 *     GATTCE_WRITE_CHARACTER_VAL when the write is complete. token->errCode
 *     will contain the result of the write.
 *
 *     When writing a long value, this function can be used to write the complete
 *     value or a portion of the value. token.request.writeVal.offset is the offset
 *     within the characteristic value to be written. To write the complete
 *     value (or when writing non-long values) set the offset to 0.
 *
 *     This function can be used to write a characteristic descriptor value
 *     as well by setting token.request.writeVal.handle to the characteristic
 *     descriptor handle. The maximum size of an attribute value is
 *     ATT_MAX_ATTRIB_VALUE_SIZE.
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device,
 *         characteristic value handle, value and optionally offset to write,
 *         and handler to receive events.
 *
 * Returns:
 *     BT_STATUS_PENDING - request was started and when complete a
 *         GATTCE_WRITE_CHARACTER_VAL event will be sent to the token handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no
 *         connection to the remote GATT server including a connection that
 *         is in the process of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - token is 0 or token->request.writeVal.handle
 *         is 0 or token->request.writeVal.len > ATT_MAX_ATTRIB_VALUE_SIZE
 *         or token->request.writeVal.offset > ATT_MAX_ATTRIB_VALUE_SIZE
 *         or token->request.writeVal.len + offset > ATT_MAX_ATTRIB_VALUE_SIZE
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use
 *         GATTC_Cancel() to cancel the current operation for this token.
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform
 *         operation.
 *
 */
BtStatus GATTC_WriteCharacteristicValue(GattClientToken *token);

/*---------------------------------------------------------------------------
 * GATTC_WriteCharacteristicValueReliable()
 *
 *     Write one or more characteristic value reliably with the given
 *     characteristic value handles on a remote GATT server. The reliability
 *     is achieved by having the server send back the values to be written
 *     before they are actually written to the database using "PrepareWrite"
 *     requests. This function can be used to write a single value reliably
 *     or multiple values that must be written, in order, in a single
 *     operation.
 *
 *     Each item to be written is defined by a GattClientWriteReliableItem
 *     structure. All the items to be written are contained in an array of
 *     GattClientWriteReliableItem structures. The
 *     "token.request.writeValReliable.num" field is initialized with the
 *     number of items in the array and
 *     "token.request.writeValReliable.itemArray" points to the array.
 *     A single item may be a long write which will be broken into
 *     multiple "PrepareWrite" requests. When all items have been prepared
 *     and verified an "ExecuteWrite" request will be made to the server
 *     to write all the items. When the operation is complete the token
 *     handler will be called with the GATTCE_WRITE_CHARACTER_VAL_RELIABLE
 *      event. "token->errCode" indicates if any errors have occurred. If
 *     errors have occurred the operation is aborted and the state of the
 *     characteristic values on the server that were prepared is undefined
 *
 *     To perform the reliable write the server must queue the "PrepareWrite"
 *     requests servers have limited space. If token->errCode is returned
 *     with ATTEC_PREPARE_QUEUE_FULL this is an indication that the server
 *     cannot perform the request and the request will need to be broken into
 *     smaller sequences of writes.
 *
 *     If a Characteristic Value is prepared two or more times then all
 *     prepared values are written multiple times in the same characteristic
 *     value in the order that they were prepared.
 *
 *     This function can be used to write a long characteristic descriptor
 *     values as well by setting the "handle" of a
 *     GattClientWriteReliableItem structure to the characteristic descriptor
 *     handle. The maximum size of an attribute value is ATT_MAX_ATTRIB_VALUE_SIZE.
 *
 * Parameters:
 *     token - Pointer to token identifying the link to the remote device,
 *         characteristic value information, and handler to receive events.
 *
 * Returns:
 *     BT_STATUS_PENDING - request was started and when complete a
 *         GATTCE_WRITE_CHARACTER_VAL_RELIABLE event will be sent to the token
 *         handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT server including a connection that is in the process
 *         of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - token is 0 or token->request.writeValReliable.num
 *         is 0 or token->request.writeValReliable.itemArray is 0
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use GATTC_Cancel
 *         to cancel the current operation for this token.
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform operation
 *         this includes if the values in the token->request.writeValReliable.itemArray
 *         are invalid.
 *
 */
BtStatus GATTC_WriteCharacteristicValueReliable(GattClientToken *token);

#endif /* __GATTCLIENT_H */
