/****************************************************************************
 *
 * File:
 *     gattdb.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Definitions and the API used by the stack to access (open and close)
 *     Gatt database in a system-specific persistent storage.
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

#ifndef __GATTDB_H
#define __GATTDB_H

#include "gattserver.h"

/*---------------------------------------------------------------------------
 * GATT Database API layer
 *
 *     The GATT Database holds GATT services and must be stored in a
 *     non-volatile way while the device is switched off. This can be done
 *     by storing the complete database in non-volatile memory or having
 *     a portion stored in ROM while only the dynamic portion is stored in
 *     non-volatile memory.
 *
 *     The GATT database is a list of services where each service has
 *     an attribute array. Static services can be stored in ROM. The
 *     GattDatabase structure must be stored in RAM. It is assumed that
 *     services will be added rarely (only when a server application is
 *     installed) and even more rarely will services be removed. It is
 *     implementation dependent on how server applications are added and
 *     removed.
 *
 *     One of the more complicated aspects of the GATT database is attributes
 *     where each bonded client has its own instance ("client own instance"
 *     attributes). An example is the Client Characteristic Configuration
 *     declaration where each bonded client has its own instantiation and the
 *     value written by each client must be persistent across connections.
 *     The approach taken is to only keep values persistent for clients that
 *     actually write to the attribute. Therefore, the list of bonded clients
 *     stored in the GATT database is a subset of the total number of bonded
 *     clients.
 *
 *     Note that values (not strings) are stored in the database in little
 *     endian format which is how they are transmitted and received over the 
 *     air.
 *
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GATT_VALUE_ENTRY_LEN_MASK  constant
 *
 *     Used to mask out bits in the GattValueEntry structure "len" field
 *     that are used for other purposes than the length.
 */
#define GATT_VALUE_ENTRY_LEN_MASK  0x0FFF

/*---------------------------------------------------------------------------
 * GATT_VALUE_ENTRY_ROM  constant
 *
 *     If set in a GattValueEntry structure "len" field then the value field is
 *     stored in ROM. This bit only applies to values stored in the fixedTable.
 *     It is ignored for values stored in the dynamic table.
 */
#define GATT_VALUE_ENTRY_ROM       0x8000

/*---------------------------------------------------------------------------
 * GATT_VALUE_ENTRY_IN_USE  constant
 *
 *     If set in a GattValueEntry structure "len" field then the value is
 *     currently in use otherwise the value can be recovered. This is used
 *     by the stack to clean up the dynamic and static tables and should be
 *     ignored by the GATTDB functions.
 */
#define GATT_VALUE_ENTRY_IN_USE    0x4000

/*---------------------------------------------------------------------------
 * GATT_VALUE_HANDLE_ALLOC constant
 *
 *     A service->startHandle is set to this value to indicate the service
 *     is allocated but not active. A service->startHandle equal to 0
 *     indicates the service is free.
 */
#define GATT_VALUE_HANDLE_ALLOC    0xFFFF


/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AttFlags type
 *
 *     The ATT Flags are used to describe features and behaviors an attribute.
 */

typedef U16 AttFlags;

/* No flags are set */
#define AFF_NONE              0x0000

/* If set, the attribute type is a 128-bit UUID and the "type" field is
 * an index into the static table. Otherwise the type is a 16-bit
 * UUID.
 */
#define AFF_TYPE_UUID128      0x0001

/* If set, the server application's handler for this attribute is called
 * with the GATTS_WRITE_ALERT event when a remote client writes the
 * attribute's value. When the valType is AVT_STORED_SERV_APP this
 * flags is ignored. It is also ignored if AFF_CHECK_WRITE is set.
 */
#define AFF_ALERT_ON_WRITE    0x0002

/* If set, the server application's handler for this attribute is called
 * with the GATTS_READ_ALERT event when a read request from a remote
 * device is received. When the valType is AVT_STORED_SERV_APP this
 * flags is ignored.
 */
#define AFF_ALERT_ON_READ     0x0004

/* If set, the value can be changed by the remote device. Some values
 * such as v8, v16, v32, v48 are stored locally in the attribute structure.
 * This only works if the attribute structure itself is in RAM. If an
 * attribute structure is stored is ROM then all dynamic values must be
 * string octets where the valType is set to AVT_OCTET_SEQ or 
 * AVT_OCTET_SEQ_FIXED and attFlags has AFF_VAL_DYNAMIC set.
 */
#define AFF_VAL_DYNAMIC       0x0008

/* If set, the server application's handler for this attribute is called
 * with the GATTS_CHECK_WRITE_REQ event when a write request from a
 * remote GATT client is received. The GATT server will not return a
 * response to the remote client until the server application calls
 * GATTS_WriteResponse() with the desired response code. AFF_ALERT_ON_WRITE
 * is ignored if this flag is set.  When the valType is AVT_STORED_SERV_APP
 * this flag is ignored. This flag can be used by applications to reject
 * write requests that don't meet the application's criteria. This flag only
 * applies if the attribute can be written.
 */
#define AFF_CHECK_WRITE      0x0010

/* End of AttFlags */

/*---------------------------------------------------------------------------
 * GattServFlags type
 *
 *     The GATT Service Flags define internal properties of a service
 *     structure.
 */
typedef U8  GattServFlags;

/* If set, the attribute array is stored in ROM and will not be written
 * to persistent storage when GATTDB_Flush() is called by the stack. The
 * stack will set the attrArray pointer to the correct value when GATTDB_Open
 * is called by the stack. The stack will not free the attribute array.
 */
#define GSF_ATTARRAY_ROM  0x01

/* If set, the service is active and is "visible" to remote clients. It is
 * written to the persistent store when GATTDB_Flush() is called. If not set,
 * the service it not active and is not visible remote clients. It is not
 * written to the persistent storage when GATTDB_Flush() is called.
 */
#define GSF_ACTIVE        0x02

/* If set, indicates that the service is accessible by the LE transport.
 * All access to this service or its attributes from BR/EDR will be rejected
 * with ATTEC_APPLICATION_ERROR.
 */
#define GSF_LE_ONLY       0x04

/* If set, indicates that the service is accessible by the BR/EDR transport
 * only.  All access to this service or its attributes from BR/EDR will be
 * rejected with ATTEC_APPLICATION_ERROR.
 */
#define GSF_BR_EDR_ONLY   0x08
/* End of GattServFlags */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AttValuei union
 *
 *     A union for holding the value field of an attribute based on
 *     AttValueType.
 */
typedef union _AttValuei {
    /* Storage for 16-bit values. Used when valType is AVT_V16 */
    U16 v16;

    /* Storage for 8-bit values. Used when valType is AVT_V8. */
    U8  v8;

    /* Storage for 32-bit values. Used when valType is AVT_V32 */
    U32 v32;

    /* Storage for 48-bit values. Used when valType is AVT_48 */
    U8  v48[6];

    /* Storage for a handle which is an offset from the service's
     * startHandle. Handle values start at 0 where 0 is the handle for the
     * service descriptor attribute. Used when valType is AVT_HANDLE.
     */
    U16 handle;

    /* An index used for a number of types to index into a table for the
     * actual value. Indexes start at 0 and must be sequential for each
     * table. The Gatt Server assigns the index value. Used when valType is
     * AVT_UUID128, AVT_OCTET_SEQ, AVT_OCTET_SEQ_FIXED and AVT_HANDLE_ARRAY.
     */
    struct {
        /* Length of value currently stored. When accessing "len"
         * the upper bits must be masked off using GATT_VALUE_ENTRY_LEN_MASK
         * before the len is used. These upper bits are flags (e.g.
         * GATT_VALUE_ENTRY_ROM ).
         */
        U16 len;
        /* Maximum size allowed - the size allocated. For items stored
         * in the static table max is ignored since the amount allocated
         * is "len".
         */
        U16 max;
        /* Index into the table */
        U16 i;
    } i;

    /* Storage for the len and max for AVT_STORED_SERV_APP */
    struct {
        /* Length of the attribute. */
        U16  len;

        /* Max length that can be written */
        U16  max;

        /* Set to TRUE if value is fixed length or FALSE if variable length */
        BOOL fixed;
    } ssApp;

    /* Indicates the way a "Client Own Instance" is stored and has the
     * the default value. Used when valType is AVT_CLIENT_OWN_INST.
     */
    struct {
        GattClientOwnInstType type;

        /* The default value. This is the initial value for all clients. It
         * is the initial valued used each time the link is created to a
         * non-bonded client.
         */
        union {
            /* Client Own Instance is a Client Characteristic Configuration bit
             * field
             */
            GattClientCharConfig ccc;

            /* Client Own Instance is an octet string */
            struct {
                /* Current length of default value */
                U16  len;
                /* Maximum size allowed for this value. This must be greater than
                 * 0 but not greater than ATT_SERV_MAX_ATTRIB_VALUE_SIZE.
                 */
                U16  max;

                /* Index in dynamic array for value */
                U16  i;
            } i;

            /* Client Own instance is two U16 values. Used for Service Change
             * characteristic value
             */
            struct {
                U16   start;
                U16   end;
            } twoU16;
        } val;
    } coi;

    /* Structure for included services. Used when valType is AVT_INC_DEF_UUID and
     * AVT_INC_DEF_NO_UUID
     */
    struct {
        /* incServHandle is the included service's startHandle which
         * handle for the service declaration */
        U16  incServHandle;

        /* endGroupHandle is the included service's endHandle  */
        U16  endGroupHandle;

        /* serviceUUID is the service's 16-bit UUID. For services with 128-bit
         * this field is ignored (valType == AVT_INC_DEF_NO_UUID).
         */
        U16  serviceUUID;
    } incDef;

    /* Characteristic Declaration value used when valType is AVT_CHAR_DECL_UUID16 and
     * AVT_CHAR_DECL_UUID128
     */
    struct {
        /* The characterisitc's properties */
        GattCharProps  charProps;

        /* Attribute handle for the Characteristic Value declaration. The handle
         * is an offset from the service's startHandle.
         */
        U16            valHandle;

        /* Characteristic's 16-bit UUID or an index into the static table */
        U16            uuid;
    } charDecl;

    /* Holds 8-bit enumeration value. Used when valType is AVT_ENUM8. The
     * value is stored in the dynamic table at index "i"
     */
    struct {
        U16   i;    /* Index into dynamic table */
        U8    min;  /* Allowed minimum value */
        U8    max;  /* Allowed maximum value */
    } enum8;

    /* Holds 16-bit enumeration value. Used when valType is AVT_ENUM16. The
     * value is stored in the dynamic table at index "i" in little endian
     * format (the format it is sent over the air)
     */
    struct {
        U16  i;     /* index into dynamic table */
        U16  min;   /* Allowed minimum value */
        U16  max;   /* Allowed maximum value */
    } enum16;

} AttValuei;

/*---------------------------------------------------------------------------
 * AttAttribute structure
 *
 *     Holds a single ATT attribute
 */
typedef struct _AttAttribute {
    AttPermissions permissions;
    U8             encryptKeySize;
    AttValueType   valType;
    AttFlags       flags;

    /* The Attribute Handle. This is an offset from the service's
     * startHandle. A value of 0xFFFF is an illegal value and indicates the
     * attribute is not in use.
     */
    U16            handle;

    /* Type is a UUID. There is only storage for a 16-bit UUID. If
     * AFF_TYPE_UUID128 is set in "flags" then the type is an index into
     * the static table.
     */
    U16 type;

    /* The value */
    AttValuei value;

} AttAttribute;

/*---------------------------------------------------------------------------
 * GattService structure
 *
 *     Holds a GATT service. Fields must not be modified by server
 *     applications.
 */
struct _GattService {
    /* Used internally to keep the service on a linked list */
    ListEntry     node;

    /* The handle of the first attribute in the service. This is the Service
     * Declaration attribute.
     */
    U16           startHandle;

    /* The handle of last valid attribute in the service. "numAttr" specifies
     * the number of attributes allocated for this service while "endHandle"
     * is the value for the last attribute with a valid handle.
     */
    U16           endHandle;

    /* The attribute array. When constructing an array it is legal to leave
     * gaps (attributes with handle equal to GATT_INVALID_ATTRIB_HANDLE).
     * The attribute array cannot be accessed directly when the service
     * is active. Use GATTS_GetAttribValue() or GATT_SetAttribValue() to
     * access an attribute value when the service is active.
     */
    AttAttribute *attrArray;

    /* Number of attributes in the array */
    U16           numAttr;

    /* Used internally */
    GattServFlags flags;

    /* Handler to server application to receive events for this service */
    BtHandlerEx  *appHandler;

};

/*---------------------------------------------------------------------------
 * GattClientOwnInstHead structure
 *
 *     The head structure used for each bonded client that needs to have
 *     a "client own instance" attribute kept persistent.
 */
typedef struct _GattCientOwnInstHead {
    /* Used internally to keep on a linked list */
    ListEntry        node;

    /* If set to TRUE the client identified by gRemDev has a bond with the
     * local device otherwise it does not. Only structures with isBonded
     * set to TRUE are written to persistent storage.
     */
    BOOL             isBonded;

    /* Identifies the client (remote device) independent of connection. */
    BtDevAddr        devAddr;
    /*GattRemoteDevice gRemDev;*/

    /* List head of items containing client own instance attribute values */
    ListEntry        attribs;

    /* Number of attributes in the "attribs" list */
    U16              numAttribs;

} GattClientOwnInstHead;

/*---------------------------------------------------------------------------
 * GattClientOwnInstItem structure
 *
 *     Holds the instance data for a "client own instance" attribute.
 */
typedef struct _GattCientOwnInstItem {
    /* Used internally to keep on a linked list */
    ListEntry             node;

    /* Identifies the service */
    U16                   startHandle;

    /* Identifies the "client own instance" attribute within a service */
    U16                   attribIndex;

    /* Identifies the type that is stored in val */
    GattClientOwnInstType type;

    /* The union holds the value. Either the value is stored directly in this
     * structure or it is an index into the dynamicTable which is determined
     * by "type".
     */
    union {
        /* Client a characteristic configuration bits */
        GattClientCharConfig ccc;

        /* Index into dynamicTable where the value is stored and its current
         * length. The maximum length of the value is stored in the
         * attribute structure.
         */
        struct {
            U16    i;
            U16    len;
        } i;

        /* Client Own instance is two U16 values. Used for Service Change
         * characteristic value
         */
        struct {
            U16   start;
            U16   end;
        } twoU16;
    } val;

} GattClientOwnInstItem;

/*---------------------------------------------------------------------------
 * GattValueEntry structure
 *
 *     Hold information for a GATT attribute value that is stored in a static
 *     or dynamic table.
 */
typedef struct _GattValueEntry {
    /* The maximum length of a value in ATT is 512 octets so a 16-bit
     * variable is needed to hold the length. When accessing the length
     * the upper bits must be masked off using GATT_VALUE_ENTRY_LEN_MASK
     * before the len is used. "len" is the maximum size of "val" (i.e. the
     * amount of space allocated.
     */
    U16   len;
    U8   *val;

} GattValueEntry;

/*---------------------------------------------------------------------------
 * GattDatabase structure
 *
 *     The GATT Database is a list of services along with tables.
 */

typedef struct _GattDatabase {
    /* Identity Resolving Key (IRK) */
    U8              irk[16];

    /* Address ID Counter for creating Device Addresses of type BTADDR_LE_ID */
    U32             addrIdCounter;

    /* Number of active services */
    U16             numActiveServices;

    /* List of all services */
    ListEntry       services;

    /* Number of static items in the staticTable. This value includes "free"
     * entries (those with "val" and "len" equal to 0 but have an index value
     * less than "numStatic").
     */
    U16             numStatic;

    /* Static String Table */
    GattValueEntry *staticTable;

    /* Number of dynamic items in the dynamicTable. This value includes "free"
     * entries (those with "val" and "len" equal to 0 but have an index value
     * less than "numDynamic").
     */
    U16             numDynamic;

    /* The Dynamic Table. */
    GattValueEntry *dynamicTable;

    /* Number of bonded items on the coiList. */
    U16             numBondedCoi;

    /* List of "Client Own Instance" head structures. */
    ListEntry       coiList;

    /* Pointer to Generic Access Profile (GAP) Service */
    GattService    *gapService;

    /* Handler for GAP service */
    BtHandlerEx     gapHandler;

    /* Pointer to Generic Attribute Profile (GATT) Service */
    GattService    *gattService;

} GattDatabase;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * GATTDB_Open()
 *
 *     Called by the stack to open and load into memory the GATT database.
 *     If no persistent database exists then an empty one is created and
 *     BT_STATUS_SUCCESS is returned. The staticTable and dynamicTable are
 *     initialized with pointers to the internal tables.
 *
 * Parameters:
 *     gattDb - pointer to GATT Database.
 *
 *     bdAddr - 48-bit address of the local radio.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if operation was successful and gattDb
 *         has been initialized with the database information.
 *
 *     BT_STATUS_FAILED - Return this if the operation failed. Overall
 *         stack initialization will fail as a result.
 */
BtStatus GATTDB_Open(GattDatabase *gattDb, const BtDevAddr *bdAddr);

/*---------------------------------------------------------------------------
 * GATTDB_Close()
 *
 *     Called by the stack to close the GATT database. This function is
 *     called during stack deinitialization. It cleans up any allocated
 *     memory that needs to be cleaned up.
 *
 * Parameters:
 *     gattDb - pointer to GATT Database.
 *
 * Returns:
 *     Ignored by the stack.
 */
void GATTDB_Close(GattDatabase *gattDb);


/*---------------------------------------------------------------------------
 * GATTDB_Flush()
 *
 *     Called by the stack to flush the GATT database to the non-volatile
 *     memory. This function is called when new services have been added or
 *     removed and the changes need to stored back into non-volatile memory.
 *     It is also called when values have been modified by local and remote
 *     entities.
 *
 *     Note that service->flags provide information about a service. Services
 *     with GSF_ATTARRAY_ROM and GSF_ACTIVE set are not written to
 *     the persistent storage.
 *
 * Parameters:
 *     gattDb - pointer to GATT Database.
 *
 * Returns:
 *     Ignored by the stack.
 */
BtStatus GATTDB_Flush(GattDatabase *gattDb);

/*---------------------------------------------------------------------------
 * GATTDB_AllocService()
 *
 *     Called by the stack to allocate a structure for a new service.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     Pointer to allocated service or 0 if unable to allocate.
 */
GattService *GATTDB_AllocService(void);

/*---------------------------------------------------------------------------
 * GATTDB_FreeService()
 *
 *     Called by the stack to free the service structure.
 *
 * Parameters:
 *     service - pointer to service structure to be freed.
 *
 * Returns:
 *     void
 */
void GATTDB_FreeService(GattService *service);

/*---------------------------------------------------------------------------
 * GATTDB_AllocAttribArray()
 *
 *     Called by the stack to allocate an attribute array.
 *
 * Parameters:
 *     num - Number of attributes to allocate in the attribute array. It
 *         must be equal at least GATT_MIN_SERVICE_ATTRIBS.
 *
 * Returns:
 *     Pointer to attribute array or 0 if an error occurs.
 */
AttAttribute *GATTDB_AllocAttribArray(U16 num);

/*---------------------------------------------------------------------------
 * GATTDB_FreeAttribArray()
 *
 *     Called by the stack to free an attribute array.
 *
 * Parameters:
 *     attribArray - pointer to the attribute array to be freed.
 *
 * Returns:
 *     void.
 */
void GATTDB_FreeAttribArray(AttAttribute *attribArray);

/*---------------------------------------------------------------------------
 * GATTDB_AllocCientOwnInstHead()
 *
 *     Called by the stack to allocate a ClientOwnInstHead structure.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     Pointer to structure or 0 if an error occurs.
 */
GattClientOwnInstHead *GATTDB_AllocClientOwnInstHead(void);

/*---------------------------------------------------------------------------
 * GATTDB_FreeCientOwnInstHead()
 *
 *     Called by the stack to free a ClientOwnInstHead structure.
 *
 * Parameters:
 *     coiHead - pointer to the ClientOwnInst head to be freed.
 *
 * Returns:
 *     void.
 */
void GATTDB_FreeClientOwnInstHead(GattClientOwnInstHead *coiHead);

/*---------------------------------------------------------------------------
 * GATTDB_AllocCientOwnInstItem()
 *
 *     Called by the stack to allocate a ClientOwnInstItem structure.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     Pointer to structure or 0 if an error occurs.
 */
GattClientOwnInstItem *GATTDB_AllocClientOwnInstItem(void);

/*---------------------------------------------------------------------------
 * GATTDB_FreeCientOwnInstItem()
 *
 *     Called by the stack to free a ClientOwnInstItem structure.
 *
 * Parameters:
 *     coiItem - pointer to the ClientOwnInstItem to be freed.
 *
 * Returns:
 *     void.
 */
void GATTDB_FreeClientOwnInstItem(GattClientOwnInstItem *coiItem);

/*---------------------------------------------------------------------------
 * GATTDB_AddStatic()
 *
 *     Called by the stack to add a static item to the internal staticTable.
 *     The index to the new static item is returned. "gattDb->numStatic" must
 *     be updated by this function to reflect the number of static items.
 *
 *     Note that the stack does not hold pointers to values in the GATT database
 *     thus it is possible for this call to result in tables being moved or
 *     reallocated including attribute array tables, the staticTable and the
 *     dynamicTable. However, if the tables are moved the indexes of items in
 *     the table cannot change.
 *
 * Parameters:
 *     gattDb - pointer to GATT database
 *
 *     len - length of static item.
 *
 *     val - pointer to string to copy into staticTable entry.
 *
 * Returns:
 *     Index into the staticTable or 0xFFFF if unable to add the item.
 */
U16 GATTDB_AddStatic(GattDatabase *gattDb, U16 len, U8 *val);

/*---------------------------------------------------------------------------
 * GATTDB_AddDynamic()
 *
 *     Called by the stack to add a dynamic item to the GATT Database's
 *     dynamicTable. The index to the new dynamic item is returned.
 *     "gattDb->numDynamic" must be updated by this function to reflect the
 *     number of dynamic items.
 *
 *     Note that the stack does not hold pointers to values in the GATT database
 *     thus it is possible for this call to result in tables being moved or
 *     reallocated including attribute array tables, the staticTable and the
 *     dynamicTable. However, if the tables are moved the indexes of items in
 *     the table cannot change.
 *
 * Parameters:
 *     gattDb - pointer to GATT Database.
 *
 *     len - amount of space to allocate for the dynamic item.
 *
 * Returns:
 *     Index into the dynamicTable or 0xFFFF if unable to add the item.
 */
U16 GATTDB_AddDynamic(GattDatabase *gattDb, U16 len);

/*---------------------------------------------------------------------------
 * GATTDB_RemoveStatic()
 *
 *     Called by the stack to remove an item from the Gatt Database's
 *     staticTable. If the item is at the end of the table then the index
 *     must be recovered otherwise the item should be left in the table with
 *     the entry set to 0 ("val" and "len" set to 0). The memory for the
 *     value is freed. The index for the item may be reassigned to a new item.
 *     "gattDb->numStatic" is decremented only if the removed item is at the
 *     end of the table.
 *
 *     Note that the stack does not hold pointers to values in the GATT database
 *     thus it is possible for this call to result in tables being moved or
 *     reallocated including attribute array tables, the staticTable and the
 *     dynamicTable. However, if the tables are moved the indexes of items in
 *     the table cannot change.
 *
 * Parameters:
 *     gattDb - pointer to GATT Database.
 *
 *     index - index of item to remove.
 *
 * Returns:
 *     void
 */
void GATTDB_RemoveStatic(GattDatabase *gattDb, U16 index);

/*---------------------------------------------------------------------------
 * GATTDB_RemoveDynamic()
 *
 *     Called by the tstack to remove a dynamic item from the GATT Database's
 *     dynamicTable. If the item is at the end of table then the index must
 *     be recovered otherwise the item is left in the table with entry set
 *     to 0 ("val" and "len" set to 0). The memory for the value is freed.
 *     The index for the item may be reassigned to a new item.
 *     "gattDb->numDynamic" is decremented only if the removed item is at the
 *     end of the table.
 *
 *     Note that the stack does not hold pointers to values in the GATT database
 *     thus it is possible for this call to result in tables being moved or
 *     reallocated including attribute array tables, the staticTable and the
 *     dynamicTable. However, if the tables are moved the indexes of items in
 *     the table cannot change.
 *
 * Parameters:
 *     gattDb - pointer to GATT Database.
 *
 *     index - index of item to remove.
 *
 * Returns:
 *     void
 */
void GATTDB_RemoveDynamic(GattDatabase *gattDb, U16 index);

#endif /* __GATTDB_H */

