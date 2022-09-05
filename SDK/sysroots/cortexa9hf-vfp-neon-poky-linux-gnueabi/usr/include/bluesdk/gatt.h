/****************************************************************************
 *
 * File:
 *     gatt.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Public types, defines, and prototypes common to both
 *     GATT servers and Clients.
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

#ifndef __GATT_H
#define __GATT_H

#include "mele.h"
#include "gattuuid.h"

/*---------------------------------------------------------------------------
 * Generic Attribute Profile (GATT) layer
 *
 *     The Generic Attribute Profile layer provides types and structures
 *     common to both the GATT Server and GATT Client layers.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 * ATT_MAX_ATTRIB_VALUE_SIZE constant
 *
 *     Defines the maximum size of an attribute value as defined by the
 *     Bluetooth Core Specification.
 */
#define ATT_MAX_ATTRIB_VALUE_SIZE  512

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AttErrorCode type
 *
 *     Error codes for the Attribute Protocol (ATT)
 */
typedef U8 AttErrorCode;

/* No errors */
#define ATTEC_NO_ERROR                       0x00

/* The attribute handle given was not valid on this server */
#define ATTEC_INVALID_HANDLE                 0x01

/* The attribute cannot be read */
#define ATTEC_READ_NOT_PERMITTED             0x02

/* The attribute cannot be written */
#define ATTEC_WRITE_NOT_PERMITTED            0x03

/* The attribute PDU was invalid. */
#define ATTEC_INVALID_PDU                    0x04

/* The attribute requires authentication before it can be read or written. */
#define ATTEC_INSUFFICIENT_AUTHENTICATION    0x05

/* Attribute server does not support the request received from the client. */
#define ATTEC_REQUEST_NOT_SUPPORTED          0x06

/* Offset specified was past the end of the attribute. */
#define ATTEC_INVALID_OFFSET                 0x07

/* The attribute requires authorization before it can be read or written */
#define ATTEC_INSUFFICIENT_AUTHORIZATION     0x08

/* Too many prepare writes have been queued. */
#define ATTEC_PREPARE_QUEUE_FULL             0x09

/* No attribute found within the given attribute handle range. */
#define ATTEC_ATTRIBUTE_NOT_FOUND            0x0A

/* The attribute cannot be read or written using the Read Blob Request */
#define ATTEC_ATTRIBUTE_NOT_LONG             0x0B

/* The Encryption Key Size used for encrypting this link is insufficient */
#define ATTEC_INSUFFICIENT_ENCRYPT_KEY_SIZE  0x0C

/* The attribute value length is invalid for the operation */
#define ATTEC_INVALID_ATTRIBUTE_VALUE_LENGTH 0x0D

/* The attribute request that was requested has encountered an error that
 * was unlikely, and therefore could not be completed as requested.
 */
#define ATTEC_UNLIKELY_ERROR                 0x0E

/* The attribute requires encryption before it can be read or written */
#define ATTEC_INSUFFICIENT_ENCRYPTION        0x0F

/* The attribute type is not a supported grouping attribute as defined by a
 * higher layer specification
 */
#define ATTEC_UNSUPPORTED_GROUP_TYPE         0x10

/* Insufficient Resources to complete the request */
#define ATTEC_INSUFFICIENT_RESOURCES         0x11

/* Group: Application error codes defined by a higher layer are 0x80 - 0x9F */

/* Value of the first application error */
#define ATTEC_APPLICATION_ERROR_FIRST        0x80

/* Generic application error */
#define ATTEC_APPLICATION_ERROR              0x80

/* Value of the last application error */
#define ATTEC_APPLICATION_ERROR_LAST         0x9F

/* Group: Common Profile and Service Error Codes  are 0xE0 - 0xFF */

#define ATTEC_PROFSERV_ERROR_FIRST           0xE0

/* The Client Characteristic Configuration Descriptor Improperly Configured 
 * error code is used when a Client Characteristic Configuration descriptor 
 * is not configured according to the requirements of the profile or service.
 */
#define ATTEC_PROFSERV_CCCD_IMPROPER         0xFD

/* The Procedure Already in Progress error code is used when a profile or 
 * service request cannot be serviced because an operation that has been 
 * previously triggered is still in progress.
 */
#define ATTEC_PROFSERV_IN_PROGRESS           0xFE

/* The Out of Range error code is used when an attribute value is out of 
 * range as defined by a profile or service specification.
 */
#define ATTEC_PROFSERV_OUT_OF_RANGE          0xFF


#define ATTEC_PROFSERV_ERROR_LAST            0xFF
/* End of AttErrorCode */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GattRemoteDevice structure
 *
 *     Hold information for identifying a remote device independent of having
 *     a connection to it. Works for both LE and BR/EDR devices.
 */
typedef struct _GattRemoteDevice {

    /* BD_ADDR and type of remote device  */
    BtDevAddr       bdAddr;

    /* Pointer BtRemoteDevice structure. This is a pointer to the base class
     * the actual structure is determined by remDev->type. Only valid if
     * device is connected. This field is set and used by the stack it must
     * not be modified by the user.
     */
    BtRemoteDevice *remDev;

} GattRemoteDevice;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GATT_GetDefaultMtu()
 *
 *     Get the global MTU for future LE GATT connections.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     The current global LE GATT MTU.
 */
U16 GATT_GetDefaultMtu(void);

/*---------------------------------------------------------------------------
 * GATT_SetDefaultMtu()
 *
 *     Set the global MTU for future LE GATT connections.
 *
 * Parameters:
 *     GattMTU - The new global MTU for future LE GATT connections.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request completed.
 *
 *     BT_STATUS_FAILED - request failed.
 */
BtStatus GATT_SetDefaultMtu(U16 GattMTU);

/*---------------------------------------------------------------------------
 * GATT_GetUuid16()
 *
 *     Converts any GATT UUID to its 16-bit form.
 *
 * Parameters:
 *     Uuid - pointer to the GATT UUID (16, 32, or 128-bit) to convert.
 *
 * Returns:
 *     GATT UUID represented as a U16 or 0 if failure.
 *
 */
U16 GATT_GetUuid16(GattUuidTypes *Uuid);

/*---------------------------------------------------------------------------
 * GATT_GetUuid32()
 *
 *     Converts any GATT UUID to its 32-bit form.
 *
 * Parameters:
 *     Uuid - pointer to the GATT UUID (16, 32, or 128-bit) to convert.
 *
 * Returns:
 *     GATT UUID represented as a U32 or 0 if failure.
 */
U32 GATT_GetUuid32(GattUuidTypes *Uuid);

/*---------------------------------------------------------------------------
 * GATT_GetUuid128()
 *
 *     Converts any GATT UUID to its 128-bit form.
 *
 * Parameters:
 *     Uuid - pointer to the GATT UUID (16, 32, or 128-bit) to convert.
 *     
 *     Uuid128 - 128-bit array to hold the converted UUID in 
 *         Little Endian format.
 *
 * Returns:
 *     TRUE or FALSE
 */
BOOL GATT_GetUuid128(GattUuidTypes *Uuid, U8 Uuid128[16]);

#endif /* __GATT_H */
