/****************************************************************************
 *
 * File:
 *     bsm_persmgr.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description: Defines the Persistence Manager interface, which must be
 *     implemented by the target platform.
 *
 * Copyright 2009-2017 OpenSynergy GmbH.  
 * All rights reserved.  All unpublished rights reserved.  
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
#ifndef __BSMPERSMAN_H
#define __BSMPERSMAN_H

#include "bsm.h"

/*---------------------------------------------------------------------------
 * BSM Persistence Manager layer
 *
 *     The persistence manager allows the BSM to store and retrieve 
 *     persistable data records somewhere outside of main memory. In this
 *     way, user settings and device information can be recovered in the
 *     event of BSM shutdown and subsequent restart.
 */

/*---------------------------------------------------------------------------
 * BsmPersRecordId type
 *
 *     Identifies a unique record in the persistence database which can
 *     be operated upon by the BsmPersistenceManager.
 */
typedef U32 BsmPersRecordId;

/** An invalid record id */
#define BSMP_INVALID       0

/** The first "fixed" record ID. Fixed records may be modified (using
 * BsmPmWrite and BsmPmSetLength) without having been created explicitly.
 * Users of the persistence manager may use any record ID between
 * BSMP_FIRST_FIXED and BSMP_LAST_FIXED as a well-known storage location.
 */
#define BSMP_FIRST_FIXED   1

/** The last "fixed" record id. */
#define BSMP_LAST_FIXED    255

/** The first "dynamic" record ID. Dynamic record IDs can only be accessed
 * after first having been created with BsmPmCreate.
 */
#define BSMP_FIRST_DYNAMIC (BSMP_LAST_FIXED + 1)

/* End of BsmPersRecordId type */

/****************************************************************************
 *
 * Section: BsmPersistenceManager class
 *
 ****************************************************************************
 *
 * Defines methods (function pointer typedefs) and the class structure  
 * (a structure containing a set of function pointer members) for the BSM
 * Persistence Manager.
 *
 */

/*---------------------------------------------------------------------------
 * BsmPmCreate()
 *
 *     Creates an empty record in the dynamically-assigned ID range 
 *     (after BSMP_FIRST_DYNAMIC).
 *
 * Parameters:
 *     pmgr - Persistence Manager of interest.
 *
 * Returns:
 *      The ID of the newly-created record, or BSMP_INVALID if no more
 *      records could be created. 
 */
typedef BsmPersRecordId (*BsmPmCreate)(BsmPersistenceManager *pmgr);

/*---------------------------------------------------------------------------
 * BsmPmRemove()
 *
 *     Deletes the specified record.
 *
 *     The record id for a removed dynamic record may be reused at create()
 *     time. For fixed records, remove() is equivalent to setLength(0).
 * 
 * Parameters:
 *     pmgr - Persistence Manager of interest.
 *
 *     id - Identifies the desired record.
 *
 * Returns:
 *     Success. Fails only if the record did not exist.
 */
typedef BOOL (*BsmPmRemove)(BsmPersistenceManager *pmgr, BsmPersRecordId id);

/*---------------------------------------------------------------------------
 * BsmPmRead()
 *     
 *     Reads previously stored data from a record.
 * 
 * Parameters:
 *     pmgr - Persistence Manager of interest.
 *
 *     id - Identifies the desired record.
 *
 *     data - Destination for recalled data (may be 0 if len is 0).
 * 
 *     len - Maximum size of data to read.
 *
 *     offset - Offset to read the data from, or 0 to read from the beginning
 *          of the record.
 *
 * Returns:
 *     Actual number of bytes read. The return value is less
 *     than "len" only if the end of the record is reached.
 */
typedef U32 (*BsmPmRead)(BsmPersistenceManager *pmgr, BsmPersRecordId id, void *data, U32 len, U32 offset); 

/*---------------------------------------------------------------------------
 * BsmPmWrite()
 *
 *     Writes data into the specified record. Automatically extends the size
 *     of the record as necessary.
 *
 *     The persistence manager need not write the data to persistent storage
 *     immediately. However, all data written must be immediately readable
 *     using the read() method.
 *
 * Parameters:
 *     pmgr - Persistence Manager of interest.
 *
 *     id - Identifies the desired record.
 *
 *     data - Pointer to data to store in this record.
 *
 *     len - Length of data to store in this record.
 * 
 *     offset - offset at which to begin writing data, or 0 to write from the
 *         start of the record.
 *
 * Returns:
 *     Success in writing. Fails if sufficient storage space cannot be
 *     allocated or if the record ID is invalid.
 */
typedef BOOL (*BsmPmWrite)(BsmPersistenceManager *pmgr, BsmPersRecordId id, const void *data,
    U32 len, U32 offset); 

/*---------------------------------------------------------------------------
 * BsmPmGetLength()
 *
 *     Returns the length of the specified record id.
 *
 * Parameters:
 *     pmgr - Persistence Manager of interest.
 *
 *     id - Identifies the desired record.
 *
 * Returns:
 *     Length of the record, or 0 if it is empty or does not exist.
 */
typedef U32 (*BsmPmGetLength) (BsmPersistenceManager *pmgr, BsmPersRecordId id);

/*---------------------------------------------------------------------------
 * BsmPmSetLength()
 *
 * Sets the length of a record.
 *
 * Parameters:
 *     pmgr - Persistence Manager of interest.
 *
 *     id - Identifies the desired record.
 *
 *     length - New length for the record.
 *
 * Returns:
 *     TRUE if the record length was successfully changed. Setting an
 *     existing record to a shorter or equal length always succeeds.
 */
typedef BOOL (*BsmPmSetLength) (BsmPersistenceManager *pmgr, BsmPersRecordId id,
    U32 length);   


/*---------------------------------------------------------------------------
 * BsmPersistenceManager structure
 *
 *     Represents a persistent data store as seen by BSM.
 *
 *     Implementers must fill in each member with an appropriate function
 *     pointer.
 */
struct _BsmPersistenceManager 
{
    /* Creates a record */
    BsmPmCreate create;
    
    /* Removes a record */
    BsmPmRemove remove;

    /* Reads data from a record */
    BsmPmRead   read;

    /* Writes data into a record */
    BsmPmWrite  write;

    /* Returns the current length of a record */
    BsmPmGetLength getLength;

    /* Sets the length of a record */
    BsmPmSetLength setLength;
};


#endif /* __BSMPERSMAN_H */

