/****************************************************************************
 *
 * File:
 *     gattserver.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     upper layer of Generic Attribute Profile (GATT) for server
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

#ifndef __GATTSERVER_H
#define __GATTSERVER_H

#include "mele.h"
#include "gatt.h"
#include "gattuuid.h"
#include "sdp.h"

/*---------------------------------------------------------------------------
 * Generic Attribute Profile (GATT) Server layer
 *
 *     The Generic Attribute Profile Server layer provides functions that
 *     allow server applications to setup a service, modify service
 *     information and values on the fly and issue notifications and
 *     indications to GATT clients.
 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GATT_MIN_SERVICE_ATTRIB constant
 *
 *     Defines the minimum number of attributes in a service.
 */
#define GATT_MIN_SERVICE_ATTRIBS  1

/*---------------------------------------------------------------------------
 * GATT_INVALID_ATTRIB_HANDLE constant
 *
 *     Defines the invalid handle (from the GATT perspective) for an invalid
 *     handle. Attributes with GATT_INVALID_HANDLE are allowed in an
 *     attribute array in any slot in the array except the first slot (index
 *     0) for future expansion.
 */
#define GATT_INVALID_ATTRIB_HANDLE 0xFFFF

/*---------------------------------------------------------------------------
 * GATT_SDP_REC_BUFF_SIZE constant
 *
 *     Size of the buffer used in GattSdpRecord for holding SDP attributes.
 */
#define GATT_SDP_REC_BUFF_SIZE  40

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AttPermissions type
 *
 *     The ATT Permissions types are used to describe the attribute
 *     permissions.
 */
typedef U16 AttPermissions;

/* No permissions */
#define ATTP_NONE                   0x0000

/* If set the attribute can be written by a remote client */
#define ATTP_WRITEABLE              0x0001

/* If set the attribute can be read by a remote client */
#define ATTP_READABLE               0x0002

/* If set the attribute requires authentication in order to be written
 * by a remote client. On an LE link this means pairing is required but
 * encryption does not need to be enabled. This setting represents
 * security mode 2 in LE (signed write). If both pairing and
 * encryption are required then ATTP_ENCRYPT_REQ_WRITE must also be set. 
 * An easy way to set both is to use ATTP_AUTHEN_REQ_WRITE. On a BR/EDR 
 * link encryption is always enabled. This setting only takes affect if 
 * ATTP_WRITEABLE is also set.
 */
#define ATTP_AUTHEN_REQ_WRITE_NOEC  0x0004

/* If set the attribute requires authentication in order to be read by a
 * remote client or to be notified/indicated by the local server. On an LE
 * link an attribute that requires authentication must also require encryption
 * (security mode 1) thus ATTP_ENCRPT_REQ_READ must also be set. This 
 * setting only takes affect if ATTP_READABLE is also set.
 */
#define ATTP_AUTHEN_REQ_READ_NOEC   0x0008

/* This permission only takes affect if ATTP_AUTHEN_REQ_WRITE_NOEC is set.
 * If set the attribute requires MITM authentication otherwise NO MITM
 * (Just Works) is required in order to be written by a remote client.
 * It only takes affect if ATTP_WRITEABLE is also set.
 */
#define ATTP_MITM_WRITE             0x0010

/* This permission only takes affect if ATTP_AUTHEN_REQ_READ_NOEC is set.
 * If set the attribute requires MITM authentication otherwise NO MITM
 * (Just Works) in order to be read by a remote client or notified/indicated
 * by the local server. In the case of being read by a remote client it only
 * takes affect if ATTP_READABLE is also set.
 */
#define ATTP_MITM_READ              0x0020

/* If set the attribute requires authorization to be written by a remote
 * client. It only takes affect if ATTP_WRITEABLE is also set. Care must
 * be taken when using this permission for attributes that can be written
 * using a signed write or a write with no response because the GATT
 * server will ignore the command if the device has not been authorized
 * before receiving the write request.
 */
#define ATTP_AUTHOR_REQ_WRITE       0x0040

/* If set the attribute requires authorization to read by a remote client or
 * to be notified/indicated by the local server. In the case of read by
 * a remote client it only takes affect if ATTP_READABLE is also set.
 */
#define ATTP_AUTHOR_REQ_READ        0x0080

/* If set the attribute requires that encryption is enabled to be written 
 * by a remote client. It only takes affect if ATTP_WRITEABLE and 
 * ATTP_AUTHEN_REQ_WRITE_NOEC are also set.
 */
#define ATTP_ENCRYPT_REQ_WRITE      0x0100

/* If set the attribute requires that encryption is enabled to be read by
 * a remote client or to be notified/indicated by the local server. This
 * setting only takes effect if ATTP_READABLE and ATTP_AUTHEN_REQ_READ_NOEC
 * are also set.
 */
#define ATTP_ENCRYPT_REQ_READ       0x0200   

 /* If set the attribute requires Secure Connections MITM authentication
 * with encryption be in place in order to be written by a remote client.
 * It only takes affect if ATTP_WRITEABLE is also set.
 */
#define ATTP_SC_MITM_WRITE          0x0400

 /* If set the attribute requires Secure Connections MITM authentication 
 * with encryption be in place in order to be read by a remote client or 
 * notified/indicated by the local server. In the case of being read by a 
 * remote client it only takes affect if ATTP_READABLE is also set.
 */
#define ATTP_SC_MITM_READ           0x0800

/* This setting is used to require that pairing has occurred and encryption 
 * is enabled. This setting represents GAP security Mode 1 for LE. 
 */
#define ATTP_AUTHEN_REQ_WRITE       (ATTP_AUTHEN_REQ_WRITE_NOEC | ATTP_ENCRYPT_REQ_WRITE)

/* This setting is used to require that pairing has occurred encryption is enabled. 
 * This setting represents GAP security Mode 1. 
 */
#define ATTP_AUTHEN_REQ_READ        (ATTP_AUTHEN_REQ_READ_NOEC | ATTP_ENCRYPT_REQ_READ)

/* Short version for readable and writeable */
#define ATTP_READWRITEABLE          (ATTP_WRITEABLE | ATTP_READABLE)

/* End of AttPermissions */

/*---------------------------------------------------------------------------
 * AttValueType type
 *
 *     The ATT Value Type indicates the type of the attribute value
 */
typedef U8 AttValueType;

/* Attribute value is an 8-bit entity stored in value.v8. The value can
 * be configured to be dynamic where a remote device can change the value
 * or static where it cannot be changed depending on how it is added to
 * the database - GATTS_AddStatic() or GATTS_AddDynamic().
 */
#define  AVT_V8               0

/* Attribute value is a 16-bit entity stored in value.v16. The value can
 * be configured to be dynamic where a remote device can change the value
 * or static where it cannot be changed depending on how it is added to
 * the database - GATTS_AddStatic() or GATTS_AddDynamic().
 */
#define AVT_V16               1

/* Attribute value is a 32-bit entity stored in value.v32. The value can
 * be configured to be dynamic where a remote device can change the value
 * or static where it cannot be changed depending on how it is added to
 * the database - GATTS_AddStatic() or GATTS_AddDynamic().
 */
#define  AVT_V32              2

/* Attribute value is a 48-bit entity stored in value.v48. The value can
 * be configured to be dynamic where a remote device can change the value
 * or static where it cannot be changed depending on how it is added to
 * the database - GATTS_AddStatic() or GATTS_AddDynamic().
 */
#define AVT_V48               3

/* Attribute value is a 16-bit attribute handle stored value.handle.
 * The handle value is an offset from the service's startHandle which is
 * assigned to when it is first registered with the GATT server. When the
 * handle is sent over the air the actual handle value is equal to the
 * service startHandle + value.handle. The value cannot be change
 * dynamically and can only be added the database via GATTS_AddStatic().
 */
#define AVT_HANDLE            4

/* Attribute value is a 128-bit UUID stored in value.ptr. The value is
 * static and must be added to the database via GATTS_AddStatic(). 128-bit
 * UUIDs are stored in little endian format.
 */
#define AVT_UUID128           5

/* Attribute value is stored as an array of octets in value.ptr. It can
 * be configured to be dynamic where a remote device can change the value
 * or static where it cannot be changed depending on how it is added to
 * the database - GATTS_AddStatic() or GATTS_AddDynamic(). If the value
 * is dynamic then it has a maximum size but the current size may be smaller.
 * If a remote device writes to the value the current size will be truncated
 * to the amount written. All dynamic values can be stored in this way
 * including structures, strings, integers, floating point values. Values with
 * endianness must be stored in little endian format (least significant octet
 * first).
 */
#define AVT_OCTET_SEQ         6

/* Attribute value is stored as an array of octets in value.ptr. It can
 * be configured to be dynamic where a remote device can change the value
 * or static where it cannot be changed depending on how it is added to
 * the database - GATTS_AddStatic() or GATTS_AddDynamic(). The size of the
 * value is fixed therefore the maximum size and current size are the same.
 * If a remote device writes to this value with a sequence smaller than the
 * size the unwritten values will not be modified but remain as they
 * were before the write. All dynamic values can be stored in this way
 * including structures, strings, integers, floating point values. Values with
 * endianness must be stored in little endian format (least significant octet
 * first).
 */
#define AVT_OCTET_SEQ_FIXED   7

/* Attribute value is "stored" in the server application and not in
 * the attribute array. Information about the value including its
 * size, maximum size and whether or not it is fixed is stored in
 * "value.ssApp". If the attribute is readable and a read request is
 * made by the remote device, the server application will receive a
 * GATTSE_READ_VALUE_REQ event. The server application must call
 * GATTS_ProvideReadValue() to provide the value to the GATT server.
 * If the attribute is writeable and a write request is received from
 * the remote device the server application will receive a
 * GATTSE_CHECK_WRITE_REQ event with the value written by the remote
 * device. The server application must call GATT_ProvideWriteResponse()
 * to provide the response to the GATT server. The flags
 * AFF_ALERT_ON_WRITE, AFF_ALERT_ON_READ and AFF_CHECK_WRITE, are
 * ignored and the events are always sent. Note that in order to receive
 * an event a handler must be set for the service by calling
 * GATTS_SetServiceHandler().
 */
#define AVT_STORED_SERV_APP   8

/* Attribute value is stored in a client instance table. The default value
 * is accessed by using value.coi. A specific client's value requires client
 * link information. An example of this type of value is the Client
 * Characteristic Configuration declaration.
 */
#define AVT_CLIENT_OWN_INST   9

/* Attribute value is a tuple consisting of an included service attribute
 * handle, an end group handle and a 16-bit UUID. It is stored in
 * value.incDef and cannot be changed dynamically.
 */
#define AVT_INC_DEF_UUID      10

/* Attribute value is a tuple consisting of an included service attribute
 * handle and an end group handle. It is stored in value.incDef and
 * cannot be changed dynamically. This is for included services that
 * have a 128 bit UUID
 */
#define AVT_INC_DEF_NO_UUID   11

/* Attribute value is a tuple consisting of GattCharProps, Characteristic
 * Value attribute handle and the characteristic 16-bit UUID. The value is
 * stored in value.charDecl and cannot be changed dynamically.
 */
#define AVT_CHAR_DECL_UUID16  12

/* Attribute value is a tuple consisting of GattCharProps, Characteristic
 * Value attribute handle and an index into the static table holding the
 * characteristic's 128-bit UUID. The value is stored in value.charDecl and
 * cannot be changed dynamically.
 */
#define AVT_CHAR_DECL_UUID128 13

/* Attribute value is an array of attribute handles which are offsets from
 * the service's startHandle. The list is stored in the
 * static table and the index into the table is value.i. The handles are stored
 * Little Endian format. The size in the table is the total number of octets in
 * the array. This type is intended for the Characteristic Aggregate Format
 * attribute. When the handle array is sent over the air the actual value for each
 * is sent as "handle" + startHandle. The value cannot be changed dynamically.
 */
#define AVT_HANDLE_ARRAY      14

/* Attribute value is an enumeration stored as an 8-bit value. The valid
 * range is stored in the attribute array but is not sent over the air
 * Only the value is sent. The range is used by the ATT protocol to detect
 * attempts to write invalid values (values outside of the range). The actual
 * value is stored in the dynamic table with the index and range
 * stored in value.enum8. This type can also be used for integers
 * that have limited values.
 */
#define AVT_ENUM8             15

/* Attribute value is an enumeration stored as a 16-bit value. The valid
 * range is stored in the attribute array but is not sent over the air
 * Only the value is sent. The range is used by the ATT protocol to detect
 * attempts to write invalid values (values outside of the range). The actual
 * value is stored in the GattServerDynamicTable with the index and range
 * stored in value.enum16. This can type can also be used for integers
 * that have limited values.
 */
#define AVT_ENUM16            16

/* End of AttValueType */

/*---------------------------------------------------------------------------
 * GattServerEventType type
 *
 *     The GATT Server Event types are passed to the registered handler with
 *     the GATT server.  This is the handler registered with GATTS_Register
 *     for all events. The event types are defined below. They are offsets 
 *     from BTE_TYPE_BASE_GATTS defined in bttypes2.h.
 */
typedef BtEventTypeEx GattServerEventType;

/* A connection to the GATT server has occurred. "gRemDev" identifies the
 * remote device.
 */
#define GATTSE_CONNECT                 (BTE_TYPE_BASE_GATTS + 0)

/* The GATT connection has been disconnected. "gRemDev" identifies the remote
 * device. "gRemdev.remDev" should not be used. All tokens associated with
 * that GATT connection are now available.
 */
#define GATTSE_DISCONNECT              (BTE_TYPE_BASE_GATTS + 1)

/* A remote client has attempted to write to an attribute. "service" identifies
 * the service and "attrIndex" identifies the attribute within the service's
 * attrArray. "info.write.val.ptr" holds the value written by the remote device.
 * Note that in order to receive an event a handler must be set for the
 * service by calling GATTS_SetServiceHandler().
 */
#define GATTSE_WRITE_ALERT             (BTE_TYPE_BASE_GATTS + 2)

/* A remote client has attempted to read an attribute. "gRemDev" including
 * "gRemDev.remDev" identify the remote device. "service" identifies the service
 * and "attrIndex" identifies the attribute within the service's attrArray.
 * Note that in order to receive an event a handler must be set for the
 * service by calling GATTS_SetServiceHandler().
 */
#define GATTSE_READ_ALERT              (BTE_TYPE_BASE_GATTS + 3)

/* A remote client has attempted to read an attribute. "gRemDev" including
 * "gRemDev.remDev" identify the remote device. "service" identifies the service
 * and "attrIndex" identifies the attribute within the service's attrArray.
 * "info.rvr" contains information describing the value requested. The
 * GATT server will not respond to the remote client until the application
 * calls GATTS_ProvideReadValue() to provide the value that will be sent
 * to the remote client in response to the client's read request.
 * Note that in order to receive an event a handler must be set for the
 * service by calling GATTS_SetServiceHandler().
 */
#define GATTSE_READ_VALUE_REQ          (BTE_TYPE_BASE_GATTS + 4)

/* A remote client has attempted to write an attribute. "gRemDev" including
 * "gRemDev.remDev" identify the remote device. "service" identifies the service,
 * "attrIndex" identifies the attribute within the service's attrArray and
 * "info.write.val.ptr" provides the value. If "info.write.needRsp" is TRUE then
 * the GATT server will not respond to the remote client until the application
 * calls GATTS_ProvideWriteRsp() with the response (AttErrorCode) that is to be
 * sent back to the remote client. The value is not written to the GATT server
 * database. It is up to the server application to call GATTS_SetAttribValue()
 * or GATTS_SetClientOwnInstValue() appropriately if the value is to be stored
 * in the GATT server database.
 * Note that in order to receive an event a handler must be set for the
 * service by calling GATTS_SetServiceHandler().
 */
#define GATTSE_CHECK_WRITE_REQ         (BTE_TYPE_BASE_GATTS + 5)

/* A Notification has been sent and the token is available. "info.token"
 * contains a pointer to the token. "e.status" indicates the
 * success or failure of the operation. If "e.status" is BT_STATUS_FAILED
 * then an error occurred when trying to send the queued notification.
 */
#define GATTSE_NOTIFICATION_COMPLETE   (BTE_TYPE_BASE_GATTS + 6)

/* An indication operation is complete and the token is available.
 * "info.token" contains a pointer to the token. "e.status" indicates the
 * success or failure of the operation. If "e.status" is BT_STATUS_FAILED
 * then an error occurred when trying to send the queued indication. This
 * could happen if the indication was sent but no response was received
 * within 30 seconds.
 */
#define GATTSE_INDICATION_COMPLETE     (BTE_TYPE_BASE_GATTS + 7)

/* Invalid value used for initialization purposes */
#define GATTSE_INVALID                 (BTE_TYPE_BASE_GATTS + 0xFF)

/* End of GattServerEventType */

/*---------------------------------------------------------------------------
 * GattService type
 *
 *     A handle to a GATT service.
 */
typedef struct _GattService GattService;
/* End of GattService type */

/*---------------------------------------------------------------------------
 * GattCharProps type
 *
 *     The GATT Characteristic Properties determine how the Characteristic
 *     Value can be used or how the characteristic descriptors can be
 *     accessed. The properties are ORed together.
 */
typedef U8  GattCharProps;

/* If set, permits broadcasts of the Characteristic Value using
 * Characteristic Configuration Descriptor
 */
#define GCP_BROADCAST     0x01

/* If set, permits reads of the Characteristic Value */
#define GCP_READ          0x02

/* If set, permit writes of the Characteristic Value without response */
#define GCP_WRITE_NO_RSP  0x04

/* If set, permits writes of the Characteristic Value with response */
#define GCP_WRITE         0x08

/*  If set, permits notifications of a Characteristic Value without
 * acknowledgement
 */
#define GCP_NOTIFY        0x10

/* If set, permits indications of a Characteristic Value with
 * acknowledgement
 */
#define GCP_INDICATE      0x20

/* If set, permits signed writes to the Characteristic Value */
#define GCP_SIGNED_WRITES 0x40

/* If set, additional characteristic properties are defined in the
 * Characteristic Extended Properties Descriptor
 */
#define GCP_EXT_PROPS     0x80

/* End of GattCharProps */

/*---------------------------------------------------------------------------
 * GattExtCharProps type
 *
 *     The GATT Characteristic Properties defines additional properties on
 *     how Characteristic Values can be used or how the characteristic
 *     descriptors can be accessed. The properties are ORed together.
 */
typedef U16  GattExtCharProps;

/* If set, permits reliable writes of the Characteristic Value */
#define GECP_RELIABLE_WRITE 0x0001

/* If set, permits writes to the characteristic descriptor */
#define GECP_WRITE_AUX      0x0002

/* End of GattExtCharProps */

/*---------------------------------------------------------------------------
 * GattClientCharConfig type
 *
 *     The GATT Client Characteristic Configuration bits define how the
 *     characteristic may be configured for a specific client. The bits
 *     are ORed together.
 */
typedef U16 GattClientCharConfig;

/* Default value when initialized by server */
#define GCCC_DEFAULT        0x0000

/* When set the characteristic value is notified */
#define GCCC_NOTIFICATION   0x0001

/* When set the characteristic value is indicated */
#define GCCC_INDICATION     0x0002

/* End of GattClientCharConfig */

/*---------------------------------------------------------------------------
 * GattClientOwnInstType type
 *
 *     Indicates the type of storage for the value of a "Client Own Instance"
 *     attribute.
 */
typedef U8 GattClientOwnInstType;

/* Attribute value is stored as a Client Characteristic Configuration
 * declaration in the GattClientOwnInstItem.
 */
#define GCOI_CLIENT_CHAR_CONFIG 0x00

/* Attribute value is stored as an octet array in the dynamicTable */
#define GCOI_DYNAMIC_TABLE      0x01

/* Attribute value is stored as two U16 values */
#define GCOI_TWO_U16            0x02

/* End of GattClientOwnInstType */

/*---------------------------------------------------------------------------
 * GattServiceTrans type
 *
 *     Indicates the type of transports supported for a GATT Service.
 */
typedef U8 GattServiceTrans;

/* All transports (LE and BR/EDR) are supported */
#define GST_ALL                 0x00

/* Only the LE transport is supported */
#define GST_LE_ONLY             0x01

/* Only the BR/EDR transport is supported */
#define GST_BR_EDR_ONLY         0x02

/* End of GattServiceTrans */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AttValue structure
 *
 *     A union for holding the value field of an attribute based on
 *     AttValueType. This union is used both for providing a value to the
 *     stack (input) and for receiving values from the stack (output).
 */
typedef union _AttValue {
    /* Storage for 16-bit values. Used when valType is AVT_V16 */
    U16 v16;

    /* Storage for 8-bit values. Used when valType is AVT_V8. */
    U8  v8;

    /* Storage for 32-bit values. Used when valType is AVT_V32 */
    U32 v32;

    /* Storage for 48-bit values. Used when valType is AVT_48. The
     * value must be stored in little endian format where the least
     * significant octet is at array index 0.
     */
    U8  v48[6];

    /* Storage for a handle which is an offset from the service's startHandle.
     * handle values start at 0 where 0 is the handle for the service descriptor
     * attribute. Used when valType is AVT_HANDLE.
     */
    U16 handle;

    /* Pointer to an octet array holding the value, the number of octets and
     *  the maximum size of the array allocated.  Used when valType is AVT_UUID128,
     * AVT_OCTET_SEQ, AVT_OCTET_SEQ_FIXED, AVT_HANDLE_ARRAY.
     */
    struct {
        /* Current length of data stored in val */
        U16  len;
        /* Maximum size allowed for this value. In the case of static values
         * max is ignored on input and will be equal to len on output. In the case
         * of dynamic values this must be greater than 0 but not greater than
         * ATT_SERV_MAX_ATTRIB_VALUE_SIZE.
         */
        U16  max;

        /* Memory to hold the value. All values that have endianness (not strings)
         * must be stored in little endian format where the least significant octet
         * is stored at array index 0.
         */
        U8   val[ATT_SERV_MAX_ATTRIB_VALUE_SIZE];
    } ptr;

    /* Storage for the len and max for AVT_STORED_SERV_APP */
    struct {
        /* Length of the attribute. */
        U16  len;

        /* Max length can be written. If fixed is TRUE then max must
         * be equal to len
         */
        U16  max;

        /* Set to TRUE if value is fixed length or FALSE if variable length */
        BOOL fixed;
    } ssApp;

    /* Indicates the way a "Client Own Instance" is stored. Used when
     * valType is AVT_OWN_CLIENT_INST. When passed to GATTS_AddDynamic() the
     * value will be the default value. This is the initial value for
     * all clients and for non-bonded clients it is the initial valued used
     * each time the link is to the non-bonded client is created.
     */
    struct {
        GattClientOwnInstType type;

        union {
            /* Client Own Instance is a Client Characteristic Configuration bit field */
            GattClientCharConfig ccc;

            /* Client Own Instance is an octet string */
            struct {
                /* Current length of data stored in val */
                U16  len;
                /* Maximum size allowed for this value. This must be greater than 0
                 * but not greater than ATT_SERV_MAX_ATTRIB_VALUE_SIZE.
                 */
                U16  max;

                /* Memory to hold the value. All values that have endianness (not strings)
                 * must be stored in little endian format where the least significant octet
                 * is stored at array index 0.
                 */
                U8   val[ATT_SERV_MAX_ATTRIB_VALUE_SIZE];
            } ptr;

            /* Client Own instance is two U16 values. Used for Service Change
             * characteristic value
             */
            struct {
                U16   start;
                U16   end;
            } twoU16;
        } val;
    } coi;

    /* Structure for included services when valType is AVT_INC_DEF_UUID or
     * AVT_INC_DEF_NO_UUID
     */
    struct {
        /* incServHandle is the included service's startHandle which is an
         * handle for the service declaration
         */
        U16  incServHandle;

        /* endGroupHandle is the included service's endHandle  */
        U16  endGroupHandle;

        /* serviceUUID is the service's 16-bit UUID. For services with 128-bit
         * this field is ignored (valType == AVT_INC_DEF_NO_UUID).
         */
        U16  serviceUUID;
    } incDef;

    /* Characteristic Declaration value used when valType ==AVT_CHAR_DECL_UUID16 or
     * AVT_CHAR_DECL_UUID128
     */
    struct {
        /* The characteristic's properties */
        GattCharProps  charProps;

        /* Attribute handle for the Characteristic Value declaration. The handle
         * is an offset from the service's startHandle.
         */
        U16            valHandle;

        /* Characteristic's UUID. 128-bit UUIDs must be stored in little
         * endian format
         */
        GattUuid       uuid;
    } charDecl;

    /* Holds 8-bit enumeration value. Used when valType is AVT_ENUM8 */
    struct {
        U8    val;  /* enum value */
        U8    min;  /* Allowed minimum value */
        U8    max;  /* Allowed maximum value */
    } enum8;

    /* Holds 16-bit enumeration value. Used when valType is AVT_ENUM16 */
    struct {
        U16  val;   /* enum value */
        U16  min;   /* Allowed minimum value */
        U16  max;   /* Allowed maximum value */
    } enum16;

} AttValue;

/*---------------------------------------------------------------------------
 * AttValueDesc structure
 *
 *     ATT Value Descriptor contains the attribute UUID, value type and value.
 */
typedef struct _AttValueDesc {
    AttValueType   valType;
    AttValue       value;

    /* The following are used for GATTS_GetAttribType() only */
    GattUuid       attribType;
    AttPermissions permissions;

} AttValueDesc;

/*---------------------------------------------------------------------------
 * GattStaticAttrTp structure
 *
 *     The GATT Static Attribute Template is used to describe a static
 *     attribute to be added to a service. The attribute's handle is based
 *     on the index in the service's "attrArray" where the attribute is
 *     stored and the service's  startHandle.
 */
typedef struct _GattStaticAttrTp {
    /* If set to true the application will be alerted via a
     * GATTSE_READ_ALERT whenever a remote client reads the attribute.
     * This is to be used for attributes with valType set to something other
     * than AVT_STORED_SERV_APP since this valType already results in
     * GATTSE_READ_VALUE_REQ being sent.
     */
    BOOL           alertRead;

    /* Permission settings for attribute. Static attribute cannot be
     * writeable but must be readable.
     */
    AttPermissions permissions;

    /* Encryption key size for attribute. (7-16 bytes is the valid range,
     * or zero can be used to represent no key size requirement).
     */
    U8             encryptKeySize;

    /* The "type" of an attribute is defined as a UUID (16 or 128 bit) */
    GattUuid       attribType;

    /* Defines the type of the value field of the attribute. Value types
     * AVT_ENUM8, AVT_ENUM16, AVT_STORED_SERV_APP and AVT_CLIENT_OWN_INST
     * are not allowed for static attributes.
     */
    AttValueType   valType;

    /* The value itself based on valType */
    AttValue       value;

} GattStaticAttrTp;

/*---------------------------------------------------------------------------
 * GattDynamicAttrTp structure
 *
 *     The GATT Dynamic Attribute Template is used to describe a dynamic
 *     attribute to be added to a service. The attribute's handle is based
 *     on the index in the service's "attrArray" where the attribute is
 *     stored and the service's  startHandle.
 */
typedef struct _GattDynamicAttrTp {
    /* If set to TRUE, the application will be alerted via a
     * GATTSE_READ_ALERT whenever a remote client reads the attribute.
     * This is to be used for attributes with valType set to something other
     * than AVT_STORED_SERV_APP since this valType already results in
     * GATTSE_READ_VALUE_REQ being sent.
     */
    BOOL           alertRead;

    /* If set to TRUE the application will be alerted via a
     * GATTSE_WRITE_ALERT whenever a remote client writes the attribute.
     * This is to be used for attributes with valType set to something other
     * than AVT_STORED_SERV_APP since this valType already results in
     * GATTSE_CHECK_WRITE_REQ being sent.
     */
    BOOL           alertWrite;

    /* If set to TRUE the application will be alerted via
     * GATTSE_CHECK_WRITE_REQ event. If "info.write.needRsp" in the event is
     * TRUE then the GATT server will not respond to the remote client until
     * the application calls GATTS_ProvideWriteRsp(). This flag is to be used
     * for attributes with valType set to something other than
     * AVT_STORED_SERV_APP since this valType already results in
     * GATTSE_CHECK_WRITE_REQ being sent. If the application wants
     * the value to be stored in the GATT server database it must set the value
     * using GATTS_SetAttribValue() or GATTS_SetClientOwnInstValue() as appropriate.
     */
    BOOL           checkWrite;

    /* Permission settings for attribute. */
    AttPermissions permissions;

    /* Encryption key size for attribute. (7-16 bytes is the valid range,
     * or zero can be used to represent no key size requirement).
     */
    U8             encryptKeySize;

    /* The "type" of an attribute is defined as a UUID (16 or 128 bit) */
    GattUuid       attribType;

    /* Defines the type of the value field of the attribute. Value types
     * AVT_HANDLE, AVT_UUID128, AVT_INC_DEF_UUID, AVT_INC_DEF_NO_UUID,
     * AVT_CHAR_DECL_UUID16, AVT_CHAR_DECL_UUID128, and
     * AVT_HANDLE_ARRAY are not allowed for dynamic attributes.
     */
    AttValueType   valType;

    /* The value itself based on valType */
    AttValue       value;

} GattDynamicAttrTp;

/*---------------------------------------------------------------------------
 * GattServerToken structure
 *
 *     GattServerToken is used to make GATT server value notifications and
 *     value indications and, in the case of indications to capture the
 *     confirmation. 
 */
typedef struct _GattServerToken {
    ListEntry          node;  /* For internal use to keep token on a list */
    ListEntry          rnode; /* For internal use to keep on request list */

    /* Handler to received events. Can be the same handler registered with
     * GATTS_Register()
     */
    BtHandlerEx       *h;

    /* Group: Request information */

    /* Pointer to service containing characteristic that is being notified
     * or indicated.
     */
    GattService      *service;

    /* Index of characteristic attribute within the service that is being
     * notified or indicated.
     */
    U16               index;

    /* Value of characteristic for attributes of type AVT_STORED_SERV_APP. */
    U16               len;
    U8                val[ATT_SERV_MAX_ATTRIB_VALUE_SIZE];

    /* Group: Indication result information */

    /* Status of an indication operation */
    BtStatus          status;

    /* Error response sent by the remote Client in response to an
     * indication
     */
    AttErrorCode      errCode;

    /* === Internal use only === */

    /* Pointer to remote device (either BR/EDR or LE )*/
    BtRemoteDevice  *remDev;

    /* Pointer to ATT context */
    void            *attCtx;

    /* Opcode of the operation */
    U8               opCode;

    /* Event that should be sent to handler when operation completes */
    GattServerEventType eType;

} GattServerToken;

/*---------------------------------------------------------------------------
 * GattServerEvent structure
 *
 *     Used to pass information from the GATT layer to GATT server applications.
 */
typedef struct _GattServerEvent {
    /* Inherit from BtEventEx */
    BtEventEx        e;

    /* Identifies the remote device independent of connection */
    GattRemoteDevice gRemDev;

    /* Pointer to service in which event applies */
    GattService     *service;

    /* Index to attribute in service's attrArray for GATTSE_WRITE_ALERT,
     * GATTSE_READ_ALERT, GATTSE_CHECK_WRITE_REQ, and GATTSE_READ_VALUE_REQ
     * events
     */
    U16             attrIndex;

    union {
        /* Used for GATTSE_WRITE_ALERT and GATTSE_CHECK_WRITE_REQ events. */
        struct {
            /* Contains the value in "val.ptr" */
            AttValue        val;

            /* Offset of value being written */
            U16             offset;

            /* TRUE indicates a response is needed to proceed so
             * GATTS_ProvideWriteRsp() must be called. FALSE indicates
             * no response is needed.
             */
            BOOL            needRsp;
        } write;

        /* Used for GATTSE_READ_VALUE_REQ event */
        struct {
            U16  offset; /* Offset for value */
            U16  size;   /* Maximum size allowed in the GATTS_ProvideReadValue() */
        } rvr;

        /* Used for GATTSE_NOTIFICATION_COMPLETE and GATTSE_INDICATION_COMPLETE */
        GattServerToken *token;
    } info;

} GattServerEvent;

/*---------------------------------------------------------------------------
 * GattCoiRecord structure
 *
 *     Used to hold the remote device and value for "Client Only Instance"
 *     attributes.
 */
typedef struct _GattCoiRecord {
    /* Identifies the remote device independent of connection */
    BtDevAddr     devAddr;

    /* Contains the value in "val.coi" */
    AttValue      val;
} GattCoiRecord;


/*---------------------------------------------------------------------------
 * GattEnumServiceContext structure
 *
 *     Used to hold the context and return service information for
 *     GATT_EnumServices().
 */
typedef struct _GattEnumServiceContext {
    /* Group: Values filled in by GATT_EnumServices() */

    /* Handle to the GATT Service */
    GattService     *service;

    /* The handle of the first attribute in the service. */
    U16           startHandle;

    /* The handle of last valid attribute in the service. */
    U16           endHandle;

    /* Service UUID. 128-bit UUIDs will be in little endian format */
    GattUuid     uuid;

    /* Set to TRUE if active */
    BOOL         isActive;

    /* === Internal use only === */
    BOOL       allServices;
    ListEntry *next;

} GattEnumServiceContext;

/*---------------------------------------------------------------------------
 * GattSdpRecord structure
 *
 *     Structure used in creation of an SDP record for a GATT service.
 */
typedef struct _GattSdpRecord {
    /* === Internal use Only === */
    ListEntry    node;
    U8           buff[GATT_SDP_REC_BUFF_SIZE];
    SdpAttribute attribs[5];
    SdpRecord    record;

} GattSdpRecord;

/*---------------------------------------------------------------------------
 * GattServiceParms structure
 *
 *     Structure used to specify how a GATT Service should be formatted.
 *     These field are used when GATTS_AllocService is invoked.
 */
typedef struct _GattServiceParms {
    /* Desired value for the starting attribute handle for the service or 0.
     * If 0, then a start handle is assigned to the service by this layer.
     * 0xFFFF is an invalid start handle.
     */
    U16                 startHandle;

    /* Number of handles to allocate for this service. This value must be
     * greater than or equal to GATT_MIN_SERVICE_ATTRIBS.
     */
    U16                 numHandles;

    /* If set to TRUE the service declaration is set to
     * GU16_PRIMARY_SERVICE otherwise it is set to GU16_SECONDARY_SERVICE.
     */
    BOOL                primary;

    /* Identifies the service's UUID. It will be the service declaration
     * attribute's value. 128-bit UUIDs must be in little endian format.
     */
    GattUuid            uuid;

    /* Transport supported for this service (LE or BR/EDR).  Both transports
     * are supported by default.
     */
    GattServiceTrans    transport;

} GattServiceParms;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GATTS_Register()
 *
 *     Register a handler with the GATT server. The handler will be called back
 *     on GattServer events.
 *
 * Parameters:
 *     handler - Pointer to handler with fields initialized (especially
 *         callback).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - callback in handler is 0 (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - request failed because the handler is already
 *         registered.
 *
 */
BtStatus GATTS_Register(BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * GATTS_Unregister()
 *
 *     Unregisters a handler from the GATT Server. If successful the handler
 *     will receive no further events. The operation will fail if there are
 *     active services with the handler.
 *
 * Parameters:
 *     handler - Pointer to a registered handler.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed because the handler was not
 *         registered or active services exist with the handler.
 *
 */
BtStatus GATTS_Unregister(BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * GATTS_GetService()
 *
 *     Get the service with the given start handle.
 *
 * Parameters:
 *     startHandle - start handle of the desired service.
 *
 *     allServices - TRUE to find all services or FALSE to find only active
 *         services.
 *
 *
 * Returns:
 *     Pointer to service or 0 if no service exists with "startHandle".
 */
GattService *GATTS_GetService(U16 startHandle);

/*---------------------------------------------------------------------------
 * GATTS_FindServices()
 *
 *     Find all the services with the given UUID in the local GATT database.
 *     If "allServices" is set to TRUE then both active and inactive services
 *     will found otherwise only active services will be found. The service's
 *     UUID is the attribute value of the first attribute in its attribute
 *     array which is the Service Declaration. Up to "size" number of service
 *     pointers are copied into the memory pointed to by servArray if it is
 *     not 0. The total number of services with the given UUID is copied into
 *     the memory pointed to by "total" if total is not 0. Note that this
 *     function can be used to get the total number of services with a given
 *     UUID without returning an array of pointers by setting size or
 *     servArray to 0. It then can be called again with an array that can
 *     hold the total size.
 *
 * Parameters:
 *     uuid - pointer to a structure holding the UUID.
 *
 *     size - number of items in the array pointed to by servArray. If the
 *         size is 0 then no service pointers are returned.
 *
 *     servArray - array of GattService pointers. If the array pointer is 0
 *         then service pointers are not returned.
 *
 *     total - pointer to memory to receive the total number of services
 *         with the given UUID. If the pointer is 0 then total is not
 *         returned.
 *
 *     allServices - TRUE to find all services or FALSE to find only active
 *         services.
 *
 *
 * Returns:
 *     Number of service pointers returned in desc.
 */
U16 GATTS_FindServices(GattUuid *uuid, U16 size, GattService *servArray[],
                       U16 *total, BOOL allServices);

/*---------------------------------------------------------------------------
 * GATTS_EnumServices()
 *
 *     Enumerate all services in the GATT database. If "ctx is initailized
 *     with "allServices" set to TRUE then both active and inactive services
 *     will be found otherwise only active services will be found. The intent
 *     of this function is to enumerate all services in the GATT database in
 *     a loop. If other services beside the current one returned are added
 *     or removed in between calls to this function then all services may
 *     not be returned or invalid services returned. The "current" service
 *     (the one returned in the context) can be added or removed.
 *
 *     A pointer to a GattEnumServiceContext structure ("ctx") is passed to
 *     the function to perform the enumeration and to return information for
 *     a service. To start the search "ctx" must be initialized by passing
 *     it to GATTS_InitServiceEnumContext().
 *
 * Parameters:
 *     ctx - context returning service information and used to find
 *         the next service.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_NOT_FOUND - request failed - no more services
 *
 *     BT_STATUS_INVALID_PARM - ctx is 0 or invalid (XA_ERROR_CHECK only).
 */
BtStatus GATTS_EnumServices(GattEnumServiceContext *ctx);

/*---------------------------------------------------------------------------
 * GATTS_InitEnumServiceContext()
 *
 *     Enumerate all services in the GATT database. If "allServices" is set
 *     to TRUE then both active and inactive services will be found otherwise
 *     only active services will be found. The intent of this function is to
 *     enumerate all services in the GATT database in a loop. If services are
 *     added or removed in between calls to this function then all services
 *     may not be returned or invalid services returned.
 *
 *     A pointer to a GattEnumServiceContext structure ("ctx") is passed to
 *     the function to perform the enumeration. To start the search initialize
 *     "ctx" by passing it GATTS_InitEnumServiceContext(). "ctx" should not be
 *     modified by the caller.
 *
 * Parameters:
 *     ctx - context used to hold information to obtaining the next service.
 *         ctx must be initialized with GATTS_InitEnumServiceContext.
 *
 *     allServices - TRUE to find all services or FALSE to find only active
 *         services.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - ctx is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GATTS_InitEnumServiceContext(GattEnumServiceContext *ctx, BOOL allServices);

/*---------------------------------------------------------------------------
 * GATTS_SetServiceHandler()
 *
 *     Set a handler for the service so the application can receive events
 *     for GATTSE_WRITE_ALERT, GATTSE_READ_ALERT, GATTSE_READ_VALUE_REQ and
 *     GATTSE_CHECK_WRITE_REQ. To unset the handler set "handler" to 0.
 *
 * Parameters:
 *     service- pointer to service in which handle is to be set.
 *
 *     handler - pointer to handler to set. This can be the same handler
 *         registered with GATTS_Register().
 *
 * Returns:
 *     The previous handler. Note that if a value other than 0 is returned
 *     then it's possible another application has set a handler for this
 *     service.
 */
BtHandlerEx *GATTS_SetServiceHandler(GattService *service, BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * GATTS_GetServiceStartHandle()
 *
 *     Return the start Handle of a service. This the handle of the first
 *     attribute of the service as seen by a remote device.
 *
 * Parameters:
 *     service - pointer to GATT service.
 *
 * Returns:
 *     Returns the start handle or 0 if the service is invalid.
 */
U16 GATTS_GetServiceStartHandle(GattService *service);

/*---------------------------------------------------------------------------
 * GATTS_GetServiceLastAttribIndex()
 *
 *     Return the index of the last attribute in the service. Attribute
 *     indexes for a service start at 0. This function returns the index
 *     of the last attribute allocated. The handle for an attribute is equal
 *     to the service's start handle plus the index.
 *
 * Parameters:
 *     service - pointer to GATT service.
 *
 * Returns:
 *     Returns the last valid index or 0xFFFF if the service is invalid.
 */
U16 GATTS_GetServiceLastAttribIndex(GattService *service);

/*---------------------------------------------------------------------------
 * GATTS_GetAttibValue()
 *
 *     Get the current value stored in an attribute of a local service. In
 *     the case of an attribute with valType of AVT_CLIENT_OWN_INST the
 *     default value is retrieved. To get the value for a specific client
 *     use GATTS_GetClientOwnInstValue(). In the case of AVT_STORED_SERV_APP
 *     only the length is retrieved. The attribute type is also retrieved.
 *
 *     Use GATTS_GetServiceLastAttribIndex() to find the index of the last
 *     attribute in the service. The first index is 0. It is possible that
 *     some of the attributes in the service are invalid (have not been added).
 *     If an invalid index is passed to this function BT_STATUS_FAILED is
 *     returned. If an index that is beyond the last index is passed into this
 *     function BT_STATUS_NOT_FOUND is returned. This allows the caller to
 *     process all valid attributes without having to call
 *     GATTS_GetServiceLastAttribIndex().
 *
 * Parameters:
 *     service - pointer to service containing the attribute
 *
 *     index - index of the attribute in the attribute array (not
 *             the value.i index.)
 *
 *     value - pointer to memory to receive the value and value type.
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - service or value pointer is 0.
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - request failed because the index is for an
 *     invalid attribute.
 *
 *     BT_STATUS_NOT_FOUND - request failed because the index is out of range.
 */
BtStatus GATTS_GetAttribValue(GattService *service, U16 index, AttValueDesc *value);

/*---------------------------------------------------------------------------
 * GATTS_SetAttribValue()
 *
 *     Set the current value of an attribute of a local service. Only the
 *     value is written. Some valTypes have min/max values which cannot be
 *     changed with this function (e.g. AVT_OCTET_SEQ, AVT_OCTET_SEQ_FIXED,
 *     AVT_ENUM8, AVT_ENUM16, and AVT_CLIENT_OWN_INST). In the case of
 *     AVT_CLIENT_OWN_INST only the default value can be changed not a
 *     specific client instance. To set the value for a specific client use
 *     GATTS_SetClientOwnInstValue(). In the case of AVT_STORED_SERV_APP only
 *     the length can be set.
 *
 *     Use GATTS_GetServiceLastAttribIndex() to find the index of the last
 *     attribute in the service. The first index is 0. It is possible that
 *     some of the attributes in the service are invalid (have not been added).
 *     If an invalid index is passed to this function BT_STATUS_FAILED is
 *     returned. If an index that is beyond the last index is passed into this
 *     function BT_STATUS_NOT_FOUND is returned. This allows the caller to
 *     process all valid attributes without having to call
 *     GATTS_GetServiceLastAttribIndex().
 *
 * Parameters:
 *     service - pointer to service containing the attribute
 *
 *     index - index of the attribute in the attribute array (not
 *             the value.i index.)
 *
 *     value - pointer to memory holding the value Descriptor to set. Only
 *         value.valType and value.value are to be initialized.
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - service or value pointer is 0
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - request failed because the index is out of range.
 *
 *     BT_STATUS_FAILED - request failed because the index is for an invalid
 *         attribute or value->valType does not match the attribute's valType
 *         or the value is static and cannot be changed or the value is outside
 *         the min/max range for values with min/max or the value is fixed
 *         and the length cannot be changed
 */
BtStatus GATTS_SetAttribValue(GattService *service, U16 index, AttValueDesc *value);

/*---------------------------------------------------------------------------
 * GATTS_GetClientOwnInstValue()
 *
 *     Get the current value stored in an attribute of a local service with
 *     a valType of AVT_CLIENT_OWN_INST for the specific client identified by
 *     "devAddr". This can get the value for both connected and non-connected
 *     devices. The attribute type is also retrieved.
 *
 *     Use GATTS_GetServiceLastAttribIndex() to find the index of the last
 *     attribute in the service. The first index is 0. It is possible that
 *     some of the attributes in the service are invalid (have not been added).
 *     If an invalid index is passed to this function BT_STATUS_FAILED is
 *     returned. If an index that is beyond the last index is passed into this
 *     function BT_STATUS_NOT_FOUND is returned. This allows the caller to
 *     process all valid attributes without having to call
 *     GATTS_GetServiceLastAttribIndex().
 *
 * Parameters:
 *     service - pointer to service containing the attribute
 *
 *     index - index of the attribute in the attribute array
 *
 *     devAddr - pointer to structure identifying the remote device.
 *
 *     value - pointer to memory to receive the value and value type.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - service or value pointer is 0 or
 *         index is invalid (0xFFFF). (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - request failed because the index is for an
 *         invalid attribute or the attribute's valType is not
 *         AVT_CLIENT_OWN_INST.
 *
 *     BT_STATUS_NOT_FOUND - request failed because the index is out of range
 *         or there is no record for the remote device (remote device is
 *         not bonded).
 */
BtStatus GATTS_GetClientOwnInstValue(GattService *service, U16 index,
                                     BtDevAddr *devAddr, AttValueDesc *value);

/*---------------------------------------------------------------------------
 * GATTS_SetClientOwnInstValue()
 *
 *     Set the current value of an attribute of a local service with valType
 *     of AVT_CLIENT_OWN_INST for the specific client identified by "devAddr".
 *     Only the value is written "max" cannot be changed. If a record for the
 *     client identified by devAddr does not exist then one will be created.
 *     It is assumed that the server application is changing the value from
 *     the default value for this specific client.
 *
 *     Use GATTS_GetServiceLastAttribIndex() to find the index of the last
 *     attribute in the service. The first index is 0. It is possible that
 *     some of the attributes in the service are invalid (have not been added).
 *     If an invalid index is passed to this function BT_STATUS_FAILED is
 *     returned. If an index that is beyond the last index is passed into this
 *     function BT_STATUS_NOT_FOUND is returned. This allows the caller to
 *     process all valid attributes without having to call
 *     GATTS_GetServiceLastAttribIndex().
 *
 * Parameters:
 *     service - pointer to service containing the attribute
 *
 *     index - index of the attribute in the attribute array
 *
 *     devAddr - pointer to device address of remote device.
 *
 *     value - pointer to memory holding the value Descriptor to set.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - service or value pointer is 0
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - request failed because the index is for an invalid
 *         attribute or the value descriptor's valType is does not match the
 *         attribute's or the attribute's valType is not AVT_CLIENT_OWN_INST  or
 *         the value length is greater than "max".
 *
 *     BT_STATUS_NOT_FOUND - request failed because the index is out of range
 *
 *     BT_STATUS_NO_RESOURCES - request failed because a "Client Own Instance"
 *        item could not be allocated.
 */
BtStatus GATTS_SetClientOwnInstValue(GattService *service, U16 index,
                                     BtDevAddr *devAddr, AttValueDesc *value);

/*---------------------------------------------------------------------------
 * GATTS_FindClientOwnInstRecords()
 *
 *     Find all the "client own instance" records for the attribute
 *     identified by "service" and "index". A "client own instance" record
 *     contains the remote device information and current value. Up to "size"
 *     number of records are copied into the memory pointed to by
 *     "coiRecArray" if it is not 0. The total number of records is copied
 *     into the memory pointed to by "total" if total is not 0. Note that
 *     this function can be used to get the total number of records without
 *     returning an array of records by setting "size" or "coiRecArray" to 0.
 *     It then can be called again with an array that can hold the total size.
 *
 * Parameters:
 *     service - pointer to service containing the attribute
 *
 *     index - index of the attribute in the attribute array
 *
 *     size - number of items in the array pointed to by coiRecArray. If the
 *         size is 0 then no service pointers are returned.
 *
 *     coiRecArray - array of GattCoiRecord structures. If the array
 *         pointer is 0 then records are not returned.
 *
 *     total - pointer to memory to receive the total number of records. If
 *         the pointer is 0 then total is not returned.
 *
 * Returns:
 *     Number of service pointers returned in desc.
 */
U16 GATTS_FindClientOwnInstRecords(GattService *service, U16 index, U16 size,
                                   GattCoiRecord coiRecArray[], U16 *total);

/*---------------------------------------------------------------------------
 * GATTS_ProvideReadValue()
 *
 *     Provide the value for an attribute to the GATT server in response to
 *     a GATTSE_READ_VALUE_REQ event. This function can be called during
 *     the callback providing the event or after the callback. The value is
 *     not sent to the remote device until this function is called which means
 *     the protocol between the remote GATT client and the local GATT server is
 *     blocked.
 *
 * Parameters:
 *     service - pointer to service containing the attribute
 *
 *     index - index of the attribute in the attribute array
 *
 *     gRemDev - pointer to structure identifying the remote device. This have
 *         all the fields set as they were received by in the
 *         GATTSE_READ_VALUE_REQ event including "gRemDev.remDev".
 *
 *     error - error code used in the response to the remote client.
 *         If "error" is ATTEC_NO_ERROR then "value" is sent in
 *         the response otherwise an error response is sent.
 *
 *     len - number of octets in "value". A length of 0 is valid even
 *         if error is ATTEC_NO_ERROR. The length must be equal to
 *         "info.rvr.size" in the GATTSE_READ_VALUE_REQ event.
 *
 *     value - pointer to memory holding the provided value response. If
 *         len is not 0 then value must not be 0.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - service is 0 or index is invalid or len is non-0
 *         and value pointer is 0 (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - request failed because the index is out of range
 *
 *     BT_STATUS_FAILED - request failed because the index is for an invalid
 *         attribute or the attribute does not match or
 *         the GATT server is not in a state expecting a value or
 *         gRemDev.remDev is not valid or "len" exceeds the max size
 *         indicated in the GATTSE_READ_VALUE_REQ event.
 */
BtStatus GATTS_ProvideReadValue(GattService *service, U16 index,
                                GattRemoteDevice *gRemDev,
                                AttErrorCode error, U16 len, U8 *value);

/*---------------------------------------------------------------------------
 * GATTS_ProvideWriteRsp()
 *
 *     Provide the error code to the GATT server in response to a
 *     GATTSE_CHECK_WRITE_REQ event where "info.write.needRsp" is TRUE. This
 *     function can be called during the callback providing the event or after
 *     the callback. The response is not sent to the remote device until this
 *     function is called which means the protocol between the remote GATT
 *     client and the local GATT server is blocked.
 *
 * Parameters:
 *     service - pointer to service containing the attribute
 *
 *     index - index of the attribute in the attribute array
 *
 *     error - error code used in the response to the remote client.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - service or value pointer is 0
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - request failed because the index is out of range
 *
 *     BT_STATUS_FAILED - request failed because the index is for an invalid
 *         attribute or GATT server is not in a state expecting a value or
 *         gRemDev is not valid.
 */
BtStatus GATTS_ProvideWriteRsp(GattService *service, U16 index,
                               GattRemoteDevice *gRemDev,
                               AttErrorCode error);

/*---------------------------------------------------------------------------
 * GATTS_InitToken()
 *
 *     Initialize the server token for operation.
 *
 * Parameters:
 *     token - Pointer to token
 *
 *     handler - pointer to a registered handler to receive events. This
 *         must be the handler registered with GATTS_Register().
 *
 *     remDev - Pointer to BtRemoteDevice structure for the active link to
 *         the remote GATT client.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed because remDev is not valid or
 *         handler is not registered.
 *
 *     BT_STATUS_INVALID_PARM - token, handler, handler->callback, or remDev
 *        is 0 (XA_ERROR_CHECK only).
 *
 */
BtStatus GATTS_InitToken(GattServerToken *token, BtHandlerEx *handler,
                         BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * GATTS_Connect()
 *
 *     Create a connection to a remote GATT client in order to send a
 *     notification or indication. If a connection is started successfully
 *     or already open then the handler is registered to the connection.
 *     The token is associated (registered) with the connection.
 *
 * Parameters:
 *     token - Pointer to token initialize using GATTS_InitToken.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_PENDING - request has been started and when complete a
 *         GATTSE_CONNECT or GATTSE_DISCONNECT event will be sent
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
BtStatus GATTS_Connect(GattServerToken *token);

/*---------------------------------------------------------------------------
 * GATTS_Disconnect()
 *
 *     Deregister the token from the connection to the remote GATT client.
 *     If the link is LE this may result in the LE ACL link being
 *     disconnected. For BR/EDR this may result in the L2CAP channel being
 *     disconnected and the BR/EDR ACL being disconnected. NOTE: The 
 *     GATTSE_DISCONNECT event signals that the GATT connection has been
 *     brought down, but this call does not guarantee that the underlying 
 *     GATT connection will be disconnected.
 *
 * Parameters:
 *     token - Pointer to token.
 *
 *     delay - Time in milliseconds to delay before disconnecting the
 *         underlying GATT connection. This only applies if this is the
 *         last server token associated with the GATT connection.
 *         If "delay" is 0 then there is no delay.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request to deregister the token from the 
 *         GATT connection succeeded.
 *
 *     BT_STATUS_ERROR - request failed because a GATT connection does not 
 *         exist or the token is not associated with a GATT connection.
 *
 *     BT_STATUS_INVALID_PARM - token, link or h.callback in token is 0
 *         (XA_ERROR_CHECK only).
 *
 */
BtStatus GATTS_Disconnect(GattServerToken *token, U32 delay);

/*---------------------------------------------------------------------------
 * GATTS_GetMtu()
 *
 *     Get the MTU for the current GATT connection.
 *
 * Parameters:
 *     remDev - Pointer to structure representing remote device.
 *
 * Returns:
 *     The MTU or 0 if a GATT connection does not exist.
 */
U16 GATTS_GetMtu(BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * GATTS_SetIdleTimerValues()
 *
 *     Set the timeout values for the idle timer used to disconnect the GATT 
 *     server if there is no activity for a certain amount of time. This timer 
 *     is only started if a remote device actually accesses the server. 
 *
 *     There are two values: 
 *         The initiator value is used in the case where the local device 
 *         initiated the LE link. The default is GATT_SERVER_INITIATOR_TIMEOUT.
 *
 *         The responder value is used in the case where the remote device
 *         initiated the LE link. The default is GATT_SERVER_RESPONDER_TIMEOUT.
 *
 * Parameters:
 *     initiatorTime - Contains the time in milliseconds to be used for the
 *         timer when the local device initiates the LE link. A value of 0
 *         disables the timer
 *
 *     responderTime - Contains the time in milliseconds to be used for the 
 *         timer when the remote device initiates the LE link. A value of 0
 *         disables the timer.
 *
 * Returns:
 *     void
 */
void GATTS_SetIdleTimerValues(TimeT initiatorTime, TimeT responderTime);

/*---------------------------------------------------------------------------
 * GATTS_GetTimerIdleValues()
 *
 *     Get the timeout values for the idle timer used to disconnect the GATT 
 *     server if there is no activity for a certain amount of time.  
 *
 *
 * Parameters:
 *     initiatorTime - Points to memory to receive the time in milliseconds  
 *         used for the timer when the local device initiates the LE link. 
 *
 *     responderTime - Points to memory to receive the time in milliseconds 
 *          used for the timer when the remote device initiates the LE link. 
 *
 * Returns:
 *     void
 */
void GATTS_GetIdleTimerValues(TimeT *initiatorTime, TimeT *responderTime);

/*---------------------------------------------------------------------------
 * GATTS_SendNotification()
 *
 *     Send a GATT Notification to a remote client. A GATT connection to the
 *     remote client must exist. The value sent in the notification is
 *     identified by "token->service" and "token->index". If the type of the
 *     value as stored in the GATT server database is AVT_STORED_SERV_APP
 *     then "token->len" and "token->val" must contain the value otherwise
 *     the value stored in the GATT server database will be used and
 *     "token->len" and "token->val" are ignored. Only ATT_MTU-3 octets of
 *     the value are sent. When the token is free the callback in
 *     "token->handler" will be called with the GATTSE_NOTIFICATION_COMPLETE
 *     event.
 *
 * Parameters:
 *     token - Pointer to a token initialized by calling GATTS_InitToken().
 *         The "service", "index", and in some cases "len" and "val" fields
 *         must be initialized to identify the attribute and provide the
 *         value to be sent in the notification.
 *
 * Returns:
 *     BT_STATUS_PENDING - request was started and when complete a
 *         GATTSE_NOTIFICATION_COMPLETE event will be sent to the token
 *         handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT client or a connection that is in the process
 *         of coming up.
 *
 *     BT_STATUS_INVALID_PARM - token is 0, "token->service" is 0, or
 *         "token->index" is invalid  (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use GATTS_Cancel
 *         to cancel the current operation for this token.
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform
 *         operation.
 *
 *     BT_STATUS_FAIL_SECURITY - request failed because the required security
 *         is not in place. GATTSEC_SetupSecurityNI() should be used to
 *         setup the required security.
 */
BtStatus GATTS_SendNotification(GattServerToken *token);

/*---------------------------------------------------------------------------
 * GATTS_SendIndication()
 *
 *     Send a GATT Indication to a remote client. A GATT connection to the
 *     remote client must exist. The value sent in the indication is
 *     identified by "token->service" and "token->index". If the type of the
 *     value as stored in the GATT server database is AVT_STORED_SERV_APP
 *     then "token->len" and "token->val" must contain the value otherwise
 *     the value stored the GATT server database will be used and
 *     "token->len" and "token->val" are ignored. Only ATT_MTU-3 octets of
 *     the value are sent. When the indication operation is complete the
 *     callback in "token->handler" will be called with the
 *     GATTSE_INDICATION_COMPLETE event.
 *
 * Parameters:
 *     token - Pointer to a token initialized by calling GATTS_InitToken().
 *         The "service", "index", and in some cases "len" and "val" fields
 *         must be initialized to identify the attribute and provide the
 *         value to be sent in the indication.
 *
 * Returns:
 *     BT_STATUS_PENDING - request was started and when complete a
 *         GATTSE_INDICATION_COMPLETE event will be sent to the token
 *         handler.
 *
 *     BT_STATUS_NO_CONNECTION - request failed because there is no connection
 *         to the remote GATT client or a connection that is in the process
 *         of coming up.
 *
 *     BT_STATUS_INVALID_PARM - token is 0  (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - the token is busy in another operation. Use GATTS_Cancel
 *         to cancel the current operation for this token.
 *
 *     BT_STATUS_FAILED - token is invalid or in wrong state to perform
 *         operation.
 *
 *     BT_STATUS_FAIL_SECURITY - request failed because the required security
 *         is not in place. GATTSEC_SetupSecurityNI() should be used to
 *         setup the required security.
 */
BtStatus GATTS_SendIndication(GattServerToken *token);

/*---------------------------------------------------------------------------
 * GATTS_Cancel()
 *
 *     Cancel the token's current operation. This cannot be used to cancel
 *     Connect or Disconnect.
 *
 * Parameters:
 *     token - Pointer to token that has the current operation cancelled.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request completed.
 *
 *     BT_STATUS_INVALID_PARM - token is 0 or type is invalid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - token is not currently in an operation.
 */
BtStatus GATTS_Cancel(GattServerToken *token);

/*---------------------------------------------------------------------------
 * GATTS_AllocService()
 *
 *     Allocate a structure for a new service. The attribute handle values
 *     are assigned to the service structure and an attribute array of size
 *     numHandles is allocated. The service declaration attribute which
 *     is at index 0 is created. The service is not active. To make the
 *     service active call GATTS_AddService(). An inactive service will
 *     not be seen by remote devices and will not be persisted in the
 *     GATT server database.
 *
 *     Note that if the startHandle is 0 the GATT server will assign startHandle
 *     value. It keeps the service list in by startHandle so it will
 *     start at the end of the list to assign startHandle values. If it
 *     cannot find a space at the end it will then search for space
 *     in between other services.
 *
 * Parameters:
 *     parms - pointer to the service's parameter. See GattServiceParms
 *         for descriptions of the available parameters.
 *
 *     service - pointer to memory to receive a GattService pointer.
 *
 * Returns:
 *     BT_STATUS_NO_RESOURCES -  operation failed due to a memory
 *         allocation error.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the parameters
 *         are invalid (e.g. numHandles < GATT_MIN_SERVICE_ATTRIBS).
 *         (XA_ERROR_CHECK only)
 *
 *     BT_STATUS_NOT_FOUND - range of handles requested could not be
 *         allocated.
 */
BtStatus GATTS_AllocService(GattServiceParms *parms, GattService **service);

/*---------------------------------------------------------------------------
 * GATTS_AddService()
 *
 *     Make the service active and persist it to the database.
 *
 * Parameters:
 *     service - pointer to service structure to add.
 *
 *
 * Returns:
 *     BT_STATUS_SUCCESS - service successfully added.
 *
 *     BT_STATUS_BUSY - operation failed because one or more connections
 *         to the GATT server exist.
 *
 *     BT_STATUS_FAILED - operation failed because service is already
 *         active or not valid (i.e. service has not been allocated from
 *         the GATT server or has already been freed).
 *
 *     BT_STATUS_NO_RESOURCE - operation failed because resources could
 *         not be allocated to store GATT Service Change information.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because service is 0
 *         (XA_ERROR_CHECK only)
 */
BtStatus GATTS_AddService(GattService *service);

/*---------------------------------------------------------------------------
 * GATTS_InactivateService()
 *
 *     Make the service inactive. This is required to add attributes. Note
 *     this removes the service from being persisted in the database. Also
 *     calling this function will cause GATT service change information to
 *     be modified which will cause a service change indication to be sent
 *     to remote GATT clients who have registered for the indication when a
 *     connection is created to the GATT server.  This call will fail if
 *     a GATT connection exists, but the GATTSE_DISCONNECT event signals 
 *     when the GATT connection has been brought down.
 *
 * Parameters:
 *     service - pointer to service structure to add.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - service successfully made inactive.
 *
 *     BT_STATUS_BUSY - operation failed because one or more connections
 *         to the GATT server exist.
 *
 *     BT_STATUS_NO_RESOURCE - operation failed because resources could
 *         not be allocated to store GATT Service Change information.
 *
 *     BT_STATUS_FAILED - operation failed because service is already
 *         inactive or not valid (i.e. service has not been allocated from
 *         the GATT server or has already been freed).
 *
 *     BT_STATUS_INVALID_PARM - operation failed because service is 0
 *         (XA_ERROR_CHECK only)
 */
BtStatus GATTS_InactivateService(GattService *service);

/*---------------------------------------------------------------------------
 * GATTS_RemoveService()
 *
 *     Remove the service from the database and free the memory assigned
 *     to the service and its attribute array. It is not possible to remove
 *     a ROM based service. The service must be inactive. An active service
 *     can be made inactive by calling GATTS_InactivateService().
 *
 * Parameters:
 *     service - pointer to service structure with attribute array to
 *         be freed.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation was successful.
 *
 *     BT_STATUS_BUSY - operation failed because the service is active.
 *
 *     BT_STATUS_FAILED - operation failed because service is not valid
 *         (i.e. service has not been allocated from the GATT server or
 *         has already been freed).
 *
 *     BT_STATUS_INVALID_PARM - operation failed because service is 0
 *         (XA_ERROR_CHECK only)
 */
BtStatus GATTS_RemoveService(GattService *service);

/*---------------------------------------------------------------------------
 * GATTS_AddServiceToSdp()
 *
 *     Create an SDP Record that corresponds to the GATT server. Some GATT
 *     profiles require that an SDP record exist for the service if the
 *     service is exposed over BR/EDR. This function will create an SDP
 *     record for the service based on the "service". Note that the
 *     caller must provide the memory used for the SDP record.
 *
 * Parameters:
 *     service - pointer to service structure used in creation of the
 *        SDP record.
 *
 *     sdpRecord - pointer GattSdpRecord structure used in creation
 *        of the SDP record. This structure will be passed to SDP
 *        so it must exist until GATTS_RemoveServiceFromSDP() is
 *        called.
 *
 *     cod - value for Class of Device. Set to 0 if none.
 *
 *     serviceNameLen - length of optional service name. If set to 0
 *        then a service name attribute is not added to the SDP record.
 *
 *     serviceName - pointer to a string containing an optional service
 *         name in the proper SDP format. The memory for this string must
 *         exist until GATTS_RemoveServiceFromSDP() is called. If the
 *         pointer is  0 then a service name attribute is not added to
 *         the SDP record. An example of the proper format for the
 *         "serviceName" is given below:
 *
 *         static const U8 GattServiceName[] = {
 *             SDP_TEXT_8BIT(12),
 *             'G', 'A', 'T', 'T', ' ', 'S', 'e', 'r', 'v', 'e', 'r', '\0'
 *          };
 *
 * Returns:
 *     BT_STATUS_SUCCESS - service successfully added.
 *
 *     BT_STATUS_FAILED - operation failed because "sdpRecord" is
 *         already in use.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because service or
 *         sdpRecord is 0 (XA_ERROR_CHECK only)
 */
BtStatus GATTS_AddServiceToSdp(GattService *service, GattSdpRecord *sdpRecord,
                               BtClassOfDevice cod, U16 serviceNameLen, U8 *serviceName);


/*---------------------------------------------------------------------------
 * GATTS_RemoveServiceFromSdp()
 *
 *     Remove a GATT service record from SDP.
 *
 * Parameters:
 *     sdpRecord - pointer GattSdpRecord structure that was used in
 *        creating the SDP record by calling GATTS_AddServiceToSdp().
 *
 * Returns:
 *     BT_STATUS_SUCCESS - record successfully removed.
 *
 *     BT_STATUS_FAILED - operation failed because sdpRecord does not exist
 *         in the SDP database.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because sdpRecord is 0
 *         (XA_ERROR_CHECK only)
 */
BtStatus GATTS_RemoveServiceFromSdp(GattSdpRecord *sdpRecord);

/*---------------------------------------------------------------------------
 * GATTS_AddStatic()
 *
 *     Add a static attribute to the service's attribute array at the given
 *     index. This function cannot be called on an active service. Static
 *     attributes cannot be modified once they have been added either by the
 *     local device or a remote device. They can be read but not written by
 *     remote clients. Static attributes are intended for things like service,
 *     include and characteristic declarations and Characteristic Extended
 *     Properties, static Characteristic User Description, Characteristic
 *     Presentation Format and Characteristic Aggregate Format declarations.
 *
 *     The attribute at index 0 has already been added via
 *     GATTS_AllocService() and cannot be added with this function.
 *
 * Parameters:
 *     service - pointer to service in which attribute is to be added.
 *
 *     index - index of attribute in the attribute array to added. The first
 *         attribute index is 0 has already been added via
 *         GATTS_AllocService(). 0xFFFF is an invalid index.
 *
 *     aTemplate - pointer to a static attribute template.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation completed successfully.
 *
 *     BT_STATUS_NOT_FOUND - operation failed because index is out of
 *         range (i.e. greater than number of attributes)
 *
 *     BT_STATUS_BUSY - operation failed because the service is active.
 *
 *     BT_STATUS_IN_USE - operation failed because the attribute has already
 *         been added.
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because a static item could
 *         not be allocated.
 *
 *     BT_STATUS_FAILED - operation failed because the aTemplate is not valid
 *         for a static attribute (for example aTemplate->valType is
 *         AVT_ENUM8).
 *
 *     BT_STATUS_INVALID_PARM - operation failed because service is 0
 *         aTemplate is 0 or index is 0 (XA_ERROR_CHECK only)
 */
BtStatus GATTS_AddStatic(GattService *service, U16 index, GattStaticAttrTp *aTemplate);

/*---------------------------------------------------------------------------
 * GATTS_AddDynamic()
 *
 *     Add a dynamic attribute to the service's attribute array at the given
 *     index. This function cannot be called on an active service. Dynamic
 *     attributes can be modified while the service is active by either
 *     the local application and/or the remote client. Dynamic attributes are
 *     intended for things like Characteristic Value, dynamic Characteristic
 *     User Description, Client Characteristic Configuration and Server
 *     Characteristic Configuration declarations.
 *
 * Parameters:
 *     service - pointer to service in which attribute is to be added.
 *
 *     index - index in the attribute array to added. The first attribute
 *         index is 0 and this must be the service declaration which is a
 *         static attribute. 0xFFFF is an invalid index.
 *
 *     aTemplate - pointer to an dynamic attribute template.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation completed successfully.
 *
 *     BT_STATUS_NOT_FOUND - operation failed because index is out of
 *         range (i.e. greater than number of attributes)
 *
 *     BT_STATUS_BUSY - operation failed because the service is active.
 *
 *     BT_STATUS_IN_USE - operation failed because the attribute has already
 *         been added.
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because a dynamic item could
 *         not be allocated.
 *
 *     BT_STATUS_FAILED - operation failed because the aTemplate is not valid
 *         for a dynamic attribute (for example aTemplate->valType is
 *         AVT_HANDLE).
 *
 *     BT_STATUS_INVALID_PARM - operation failed because service is 0
 *         or aTemplate is 0 (XA_ERROR_CHECK only)
 */
BtStatus GATTS_AddDynamic(GattService *service, U16 index, GattDynamicAttrTp *aTemplate);

/*---------------------------------------------------------------------------
 * GATTS_GetIRK()
 *
 *     Retrieve the Identity Resolving Key (IRK) from the database.
 *
 * Parameters:
 *     Key - pointer to memory pointer to receive the 16 byte IRK.
 *
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Key successfully retrieved.
 *
 *     BT_STATUS_FAILED - operation failed
 *
 *     BT_STATUS_INVALID_PARM - operation failed because Key is 0
 *         (XA_ERROR_CHECK only)
 */
BtStatus GATTS_GetIRK(U8 *Key);

/*---------------------------------------------------------------------------
 * GATTS_RemoveClientOwnAttribForRemDev()
 *
 *     Remove the client owned attributes that are stored in the GATT database
 *     for the given remote device. These should only be saved for devices that
 *     have a bond so if a bond is removed these should be removed as well. 
 *
 *     Note that when a GATT connection is made to a remote device and the
 *     GATT server finds that there is not a bond with the remote device it will
 *     remove these attributes.
 *
 * Parameters:
 *     devAddr - device address of the remote device.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation successful.
 *
 *     BT_STATUS_NOT_FOUND - operation failed because attributes were not found.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because Key is 0
 *         (XA_ERROR_CHECK only)
 */
BtStatus GATTS_RemoveClientOwnAttribForRemDev(BtDevAddr *devAddr);

#endif /* __GATTSERVER_H */
