/****************************************************************************
 *
 * File:
 *     ddb.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Definitions the API used by the stack to access a system-specific
 *     device database.
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

#ifndef __DDBH
#define __DDBH

#include "bttypes.h"
#include "me.h"


/*---------------------------------------------------------------------------
 * Device Database API layer
 *
 *     The Device Database is used by the Management Entity to store link
 *     keys and other information in a non-volatile way. This allows security
 *     information about a device to be used in later sessions, even if the
 *     device is switched off.
 *
 *     The database accessed by these calls must be maintained in
 *     non-volatile memory, such as flash RAM or a hard disk drive.
 *
 *     The size of the device must be NUM_DDB_RECORDS which is defined in
 *     btconfig.h.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * DDB_INVALID_INDEX constant
 *
 *     Defines an invalid index into the Device Database. 
 */
#define DDB_INVALID_INDEX        0xFFFF

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * DDB_Open()
 *
 *     Called by the stack to open the device database. This function is
 *     called during stack initialization, before any other DDB_ calls
 *     are made.
 *
 * Parameters:
 *     bdAddr - 48-bit address and type of the local radio.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if operation was successful to
 *         indicate that other device database calls may be made.
 *
 *     BT_STATUS_FAILED - Return this if the operation failed. Overall
 *         stack initialization (EVM_Init) will fail as a result.
 */
BtStatus DDB_Open(const BtDevAddr *bdAddr);


/*---------------------------------------------------------------------------
 * DDB_Close()
 *
 *     Called by the stack to close the device database. This function is
 *     called during stack deinitialization.
 *
 * Returns:
 *     Ignored by the stack.
 */
BtStatus DDB_Close(void);


/*---------------------------------------------------------------------------
 * DDB_Flush()
 *
 *     Called by the stack to flush the device database to disk. This function 
 *     is called when new devices are added to the database. 
 *
 * Returns:
 *     Ignored by the stack.
 */
BtStatus DDB_Flush(void);


/*---------------------------------------------------------------------------
 * DDB_AddRecord()
 *
 *     Called by the stack to add or replace a record in the database.
 *     If the device record indicates a BD_ADDR that is already in the
 *     database, this function should replace the existing record.
 *
 * Parameters:
 *     record - Pointer to the record which is to be copied into the
 *         database. The structure should not be used by DDB after this call
 *         has returned.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this to indicate that the record
 *         was successfully written to the database.
 *
 *     BT_STATUS_FAILED - Return this to indicate that the record could
 *         not be written to the database.
 *
 *     BT_STATUS_NO_RESOURCES - Return this to indicate that the database
 *         is full and the record could not be written. This will allow
 *         upper layer code to remove a less important record to make room 
 *         for the new record.
 */
BtStatus DDB_AddRecord(BtDeviceRecord *record);


/*---------------------------------------------------------------------------
 * DDB_FindRecord()
 *
 *     Called by the stack to copy a BR/EDR record out of the database. The 
 *     BR/EDR record is identified by the BD_ADDR that represents the device.
 *     LE records are not found with this function instead use 
 *     DDB_FindRecordEx().
 *
 * Parameters:
 *     bdAddr - 48-bit address and type that identifies the desired record.
 *
 *     crlType - The type of record that should be found.  This field 
 *         distinguishes different records with the same Bluetooth Device 
 *         Address.  
 *
 *     record - pointer to the record structure to fill with the
 *         specified record's information.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if the record was found and copied
 *         into the "record" parameter.
 *
 *     BT_STATUS_FAILED - Returns this if the record could not be found
 *         or an error occurred while accessing non-volatile memory.
 */
BtStatus DDB_FindRecord(const BtDevAddr *bdAddr,
                        BtControllerType crlType, 
                        BtDeviceRecord  *record);

/*---------------------------------------------------------------------------
 * DDB_FindRecordEx()
 *
 *     Called by the stack to copy a record out of the database. The record
 *     is identified by the BD_ADDR, bdAddrType and possible the leAddrType 
 *     that represents the device. This function can be used to find all
 *     device records including LE.
 *
 * Parameters:
 *     bdAddr - 48-bit address and type that identifies the desired record.
 *
 *     crlType - The type of record that should be found.  This field 
 *         distinguishes different records with the same Bluetooth Device 
 *         Address.  
 *
 *     record - pointer to the record structure to fill with the
 *         specified record's information.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if the record was found and copied
 *         into the "record" parameter.
 *
 *     BT_STATUS_FAILED - Returns this if the record could not be found
 *         or an error occurred while accessing non-volatile memory.
 */
BtStatus DDB_FindRecordEx(const BtDevAddr *bdAddr,
                          BtControllerType crlType, 
                          BtDeviceRecord  *record);

/*---------------------------------------------------------------------------
 * DDB_DeleteRecord()
 *
 *     Called by the stack to remove a record from the database. The record
 *     is identified by its index in the Device Database.
 * 
 * Parameters:
 *     index - index in the Device Database of the record to delete. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if the record was found and deleted.
 *
 *     BT_STATUS_FAILED - Return this if the record could not be found or
 *         deleted.
 */
BtStatus DDB_DeleteRecord(I16 index);

/*---------------------------------------------------------------------------
 * DDB_EnumDeviceRecords()
 *
 *     Called by the stack to read a specific record from the device
 *     database. Records are stored with indexes between 0 and n-1 where n
 *     is the number of records in the database.
 *
 *     The stack calls this function with the bdAddr parameter set to 0 to 
 *     read device records from 0 to n-1 in increasing sequence. The stack 
 *     does not call other DDB functions during this use of enumeration.
 *
 *     The stack also calls this function with the bdAddr parameter set
 *     to a non-0 value. The intent is that this function will start at the
 *     index and return the first record it finds with the given bdAddr. The
 *     function will return DDB_INVAID_INDEX if the index passed to the
 *     function is greater than the last record in the database or in if
 *     the end of the database is reached before a record with bdAddr is found.
 *     The stack may call other DDB functions such as DDB_DeleteRecord during
 *     this use of enumeration.
 *
 * Parameters:
 *     index - index of desired record. The first record is at index 0.
 *         If bdAddr is non-0 the index is the first record checked for a
 *         matching bdAddr.
 *
 *     record - pointer to the record structure to fill with the
 *         specified record's information.
 *
 *     bdAddr - BtDevAddr of desired record. May be set to 0 in which case
 *         the BD_ADDR of the record is not checked.
 *
 * Returns:
 *     index of the record returned or DDB_INVALID_INDEX if the record could not be
 *     found or copied (e.g. an index greater than the index of the last
 *     record in the database).
 */
I16 DDB_EnumDeviceRecords(I16 i, BtDeviceRecord* record, const BtDevAddr *bdAddr);

/*---------------------------------------------------------------------------
 * DDB_NumRecords()
 *
 *     Called by the stack to verify that the size of the Device Database
 *     is NUM_DDB_RECORDS. If it is not the stack will close the database.
 *
 * Returns:
 *     Number of records in the device database. 
 */
I16 DDB_NumRecords(void);

#endif /* __DDBH */

