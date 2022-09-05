/****************************************************************************
 *
 * File:
 *     gattcache.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Definitions and the API used by the stack to access (open and close)
 *     the Gatt cache in a system-specific persistent storage.
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

#ifndef __GATTCACHE_H
#define __GATTCACHE_H

#include "gattclient.h"

/*---------------------------------------------------------------------------
 * GATT Cache API layer
 *
 *     The GATT Cache holds remote GATT service information and must be
 *     stored in a non-volatile way while the device is switched off.
 *
 *     The GATT Cache consists of a list of GATT Cache Remote Device
 *     structures. There is GATT Cache Remote Device structure for each bonded
 *     remote device in which GATT operations have been performed as well as
 *     non-bonded devices with a current connection. Only the records of bonded
 *     devices are persisted in non-volatile memory.
 *
 *     A GATT Cache Remote Device structure contains a list of GATT Cache
 *     Service Structures which in turn can an array of attributes. While the
 *     handle and types of all attributes are cached not all information
 *     in the remote GATT database is cached. The purpose of the cache is
 *     to eliminate the need to repeatedly discover a remote devices services
 *     and characteristics.
 *
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 * GattCacheServFlags type
 *
 *     Flags to indicate what is cached in a service. They are ORed together.
 */
typedef U8 GattCacheServFlags;

/* Indicates Include Declarations are cached. This also means that
 * service structures exist for each included service.
 */
#define GCSF_INCLUDED_DECL_CACHED  0x01

/* Indicates Characteristic Declarations are cached */
#define GCSF_CHARACTER_DECL_CACHED 0x02

/* End of GattCacheServFlags */

/* Forward declaration of GattCacheService structure */
typedef struct _GattCacheService GattCacheService;

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 * GattCacheAttribute structure
 *
 *     The GattCacheAttribute structure is used to hold an attribute's
 *     information in the GATT Client Cache.
 */
typedef struct _GattCacheAttribute {
    /* Used internally to keep on a linked list */
    ListEntry               node;

    /* Handle */
    U16                     handle;

    /* Additional info based on type. Type is determined by which list
     * it is on
     */
    union {
        /* For an include definition attribute */
        GattCacheService   *incServ;

        /* For characteristic descriptors */
        GattUuid            descUuid;
    } info;
} GattCacheAttribute;

/*---------------------------------------------------------------------------
 * GattCacheCharacteristic structure
 *
 *     The GattCacheCharacteristic structure is used to hold characteristic
 *     declaration information in the GATT Client Cache.
 */
typedef struct _GattCacheCharacteristic {
    /* Used internally to keep on a linked list */
    ListEntry    node;

    /* Handle for the characteristic declaration */
    U16          handle;

    /* Characteristic properties */
    U8           properties;

    /* Handle the value attribute */
    U16          valHandle;

    /* UUID for characteristic value */
    GattUuid     valUuid;

    /* TRUE if the characteristic descriptors are cached FALSE otherwise. */
    BOOL         areDescCached;

    /* Number of characteristic descriptors */
    U16          numDesc;

    /* List to hold descriptors */
    ListEntry    descs;
} GattCacheCharacteristic;

/*---------------------------------------------------------------------------
 * GattCacheDeviceRecord structure
 *
 *     The GattCacheDeviceRecord structure is used to cache GATT Server
 *     database information for a bonded devices or a currently connected
 *     non-bonded device.
 */
typedef struct _GattCacheDeviceRecord {
    /* Used internally to keep on a linked list */
    ListEntry        node;

    /* If set to TRUE the client identified by devAddr has a bond with the
     * local device otherwise it does not. Only structures with isBonded
     * set to TRUE are written to persistent storage.
     */
    BOOL             isBonded;

    /* Identifies the server (remote device) independent of connection. */
    BtDevAddr        devAddr;

    /* Set to TRUE if primary services have been cached otherwise FALSE */
    BOOL             areServicesCached;

    /* Number of primary services in the "services" list */
    U16              numServices;

    /* List head of primary services on the remote GATT server */
    ListEntry        services;

    /* Number of secondary services in the "secServices" list */
    U16              numSecServices;

    /* List head of secondary services on the remote GATT server. Note
     * that this list may not be complete. Only secondary services
     * of primary services that have include definitions cached are in
     * the list.
     */
    ListEntry        secServices;

    /* Group: Information cached for remote GATT service */

    /* TRUE if GATT service info has been cached. This includes if it has
     * been read but not found or does not have characteristics
     */
    BOOL             isGattServCached;

    /* Contains the handle to the GATT service's "service changed"
     * characteristic value. This is the handle that will be indicated
     * if a change occurs. If it is 0 and isCached is TRUE then this
     * device does not support service changed.
     */
    U16              servChgHandle;

    /* Contains the properties of the "service changed" characteristic
     * value.
     */
    U8               servChgProps;

} GattCacheDeviceRecord;

/*---------------------------------------------------------------------------
 * GattCacheService structure
 *
 *     The GattCacheService structure is used to hold the cached information
 *     for remote GATT service.
 */
struct _GattCacheService {
    /* Used internally to keep on a linked list */
    ListEntry          node;

    /* TRUE if primary service. FALSE if secondary service */
    BOOL               isPrimary;

    /* Reference count */
    U16                refCount;

    /* Service UUID */
    GattUuid           uuid;

    /* Start Handle */
    U16                startHandle;

    /* End Handle */
    U16                endHandle;

    /* Flags to indicate what has been cached */
    GattCacheServFlags flags;

    /* Number of included services */
    U16                numIncServices;

    /* List of included definitions */
    ListEntry          incServices;

    /* Number of characteristics */
    U16                numChars;

    /* Pointer to array of attributes */
    ListEntry          chars;

};


/*---------------------------------------------------------------------------
 * GattCache structure
 *
 *     The GattCache structure is used to hold a list of
 *     GattCacheDeviceRecord structures.
 */
typedef struct _GattCache {

    /* Number of "bonded" records in the cache */
    U16           numRecords;

    /* List head for records. There may be more records in this list
     * than numRecords since non-bonded records may also be in the list.
     * Only bonded records are persistently stored.
     */
    ListEntry     records;

} GattCache;

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GATTCACHE_Open()
 *
 *     Called by the stack to open and load into memory the GATT client
 *     cache. If no persistent cache exists then an empty one is created and
 *     BT_STATUS_SUCCESS is returned. Note that if this function returns
 *     BT_STATUS_FAILED the stack will clean up the cache as long as it is
 *     consistent.
 *
 * Parameters:
 *     gattCache - pointer to GATT Cache structure.
 *
 *     bdAddr - 48-bit address of the local radio.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if operation was successful and
 *         gattCache has been initialized cache information.
 *
 *     BT_STATUS_FAILED - Return this if the operation failed. Overall
 *         stack initialization will fail as a result.
 */
BtStatus GATTCACHE_Open(GattCache *gattCache, const BtDevAddr *bdAddr);

/*---------------------------------------------------------------------------
 * GATTCACHE_Close()
 *
 *     Called by the stack to close the GATT client cache. This function is
 *     called during stack deinitialization. The stack will first call flush
 *     and after calling close will clean up the allocated memory in the cache.
 *
 * Parameters:
 *     gattCache - pointer to GATT Cache.
 *
 * Returns:
 *     Ignored by the stack.
 */
BtStatus GATTCACHE_Close(GattCache *gattCache);

/*---------------------------------------------------------------------------
 * GATTCACHE_Flush()
 *
 *     Called by the stack to flush the GATT client cache to the non-volatile
 *     memory. This function is called when new items have been added or
 *     removed and the changes need to stored back into non-volatile memory.
 *     It is also called when values have been modified. Only bonded records
 *     shall be stored in non-volatile memory. Note that non-bonded records
 *     may be in the cache when this function is called.
 *
 * Parameters:
 *     gattCache - pointer to GATT Cache.
 *
 * Returns:
 *     Ignored by the stack.
 */
BtStatus GATTCACHE_Flush(GattCache *gattCache);

/*---------------------------------------------------------------------------
 * GATTCACHE_AllocRecord()
 *
 *     Called by the stack to allocate a GattCacheDeviceRecord structure.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     Pointer to allocated record or 0 if unable to allocate.
 */
GattCacheDeviceRecord *GATTCACHE_AllocRecord(void);

/*---------------------------------------------------------------------------
 * GATTCACHE_FreeRecord()
 *
 *     Called by the stack to free a GattCacheDeviceRecord structure.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     void
 */
void GATTCACHE_FreeRecord(GattCacheDeviceRecord *record);

/*---------------------------------------------------------------------------
 * GATTCACHE_AllocService()
 *
 *     Called by the stack to allocate a GattCacheService structure.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     Pointer to allocated record or 0 if unable to allocate.
 */
GattCacheService *GATTCACHE_AllocService(void);

/*---------------------------------------------------------------------------
 * GATTCACHE_FreeService()
 *
 *     Called by the stack to free a GattCacheService structure.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     void
 */
void GATTCACHE_FreeService(GattCacheService *service);

/*---------------------------------------------------------------------------
 * GATTCACHE_AllocAttribute()
 *
 *     Called by the stack to allocate a GattCacheAttribute structure.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     Pointer to allocated record or 0 if unable to allocate.
 */
GattCacheAttribute *GATTCACHE_AllocAttribute(void);

/*---------------------------------------------------------------------------
 * GATTCACHE_FreeAttribute()
 *
 *     Called by the stack to free a GattCacheAttribute structure.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     void
 */
void GATTCACHE_FreeAttribute(GattCacheAttribute *attribute);

/*---------------------------------------------------------------------------
 * GATTCACHE_AllocCharacteristic()
 *
 *     Called by the stack to allocate a GattCacheCharacteristic structure.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     Pointer to allocated record or 0 if unable to allocate.
 */
GattCacheCharacteristic *GATTCACHE_AllocCharacteristic(void);

/*---------------------------------------------------------------------------
 * GATTCACHE_FreeCharacteristic()
 *
 *     Called by the stack to free a GattCacheCharacteristic structure.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     void
 */
void GATTCACHE_FreeCharacteristic(GattCacheCharacteristic *characteristic);

#endif /* __GATTCACHE_H */
