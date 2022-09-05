/***************************************************************************
 *
 * File:
 *     mesecamp.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Header include information for the portion of the Bluetooth 
 *     Management Entity that handles security for devices that have 
 *     Alternate MAC/Physical (AMP) radios.
 *
 * Created:
 *     May 1, 2009
 *
 * Copyright 2009-2017 OpenSynergy GmbH.
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

#ifndef MESECAMP_H
#define MESECAMP_H

#include "me.h"

/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SEC_REVERSE_BYTE_ORDER constant
 *     This define indicates whether input bytes and output bytes to the H2 
 *     function should have their order reversed.  If the element indexed by 
 *     zero is the most significant byte, then SEC_REVERSE_BYTE_ORDER must 
 *     be defined as XA_ENABLED.  If the element indexed by zero is the 
 *     least significant byte, then SEC_REVERSE_BYTE_ORDER must be defined 
 *     as XA_DISABLED.  
 */
#ifndef SEC_REVERSE_BYTE_ORDER
#define SEC_REVERSE_BYTE_ORDER XA_DISABLED
#endif /* SEC_REVERSE_BYTE_ORDER */ 

/*---------------------------------------------------------------------------
 * SEC_MULTIPLE_AMP_TYPES constant
 *     If the system could connect to AMPs of differing types, then this 
 *     define must be set to XA_ENABLED.  Otherwise, it can optionally be 
 *     set to XA_DISABLED to save some unneeded processing.  
 */
#ifndef SEC_MULTIPLE_AMP_TYPES
#if BT_NUM_AMP_TYPES > 1
#define SEC_MULTIPLE_AMP_TYPES XA_ENABLED
#else /* BT_NUM_AMP_TYPES <= 1 */ 
#define SEC_MULTIPLE_AMP_TYPES XA_DISABLED
#endif /* BT_NUM_AMP_TYPES <= 1 */ 
#endif /* SEC_MULTIPLE_AMP_TYPES */ 
#if BT_NUM_AMP_TYPES > 1
#if SEC_MULTIPLE_AMP_TYPES == XA_DISABLED
#error "SEC_MULTIPLE_AMP_TYPES must be enabled when BT_NUM_AMP_TYPES > 1!"
#endif /* SEC_MULTIPLE_AMP_TYPES == XA_DISABLED */ 
#endif /* BT_NUM_AMP_TYPES > 1 */ 

/*---------------------------------------------------------------------------
 * SEC_GET_GAMP constant
 *     This define exposes the SEC_AK_GetGenericAmpLinkKey() API.  Normally, 
 *     SEC_GET_GAMP should be defined as XA_DISABLED.  It is only defined as 
 *     XA_ENABLED for testing purposes.  
 */
#ifndef SEC_GET_GAMP
#if TEST_STUBS
#define SEC_GET_GAMP XA_ENABLED
#else /* ! TEST_STUBS */ 
#define SEC_GET_GAMP XA_DISABLED
#endif /* else ! TEST_STUBS */ 
#endif /* SEC_GET_GAMP */ 

/*---------------------------------------------------------------------------
 * SEC_STORE_AMP_INFO_IN_DB constant
 *     This define indicates whether the AMP Link Key information should be 
 *     stored to the persistent database file or not.  
 */
#ifndef SEC_STORE_AMP_INFO_IN_DB
#define SEC_STORE_AMP_INFO_IN_DB XA_ENABLED
#endif /* SEC_STORE_AMP_INFO_IN_DB */ 

/*---------------------------------------------------------------------------
 * BT_GAMP_KEY_LENGTH constant
 *
 *     This define indicates the size of a generic AMP Key.  
 */
#define BT_GAMP_KEY_LENGTH 32

/*---------------------------------------------------------------------------
 * BT_802_11_AMP_KEY_LENGTH constant
 *
 *     This define indicates the size of a dedicated 802.11 AMP 
 *     Key.  
 */
#define BT_802_11_AMP_KEY_LENGTH 32

/*---------------------------------------------------------------------------
 * BT_NUM_AMP_CT_RECS constant
 *
 *     This defines the total number of AMP controller records (one for each 
 *     BR/EDR controller plus one for each of the possible AMP types) that 
 *     may be needed to correspond to active connections, if every possible 
 *     connection is open at the same time.  
 */
#define BT_NUM_AMP_CT_RECS (NUM_BT_DEVICES * (1 + BT_NUMBER_AMP_TYPES))

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

#if SEC_GET_GAMP == XA_ENABLED

/*---------------------------------------------------------------------------
 * BtAmpGenKey structure
 *     This structure is used to output generic AMP link key information.  
 *     It is normally not available, and only needed for testing purposes.  
 *
 */
typedef struct _BtAmpGenKey
{
    U8 len;
    U8 key[BT_GAMP_KEY_LENGTH];
} BtAmpGenKey;

#endif /* SEC_GET_GAMP == XA_ENABLED */ 

/*---------------------------------------------------------------------------
 * BtDAmpNdx structure
 *     This structure is used to output dedicated AMP link key index 
 *     information.  
 *
 */
typedef struct _BtDAmpNdx
{
    BOOL isReal; /* TRUE if the index field has any meaning, or FALSE 
                  * otherwise.  This field will be FALSE for the Fake PAL 
                  * controller type.  
                  */ 
    U8   index;
} BtDAmpNdx;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *            SEC_AK_CalcGenericAmpLinkKey
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function calculates the initial generic AMP Link Key 
 *     value.  It should be called any time the BR/EDR link key changes.  
 *     This function does not need to be invoked directly, since 
 *     SEC_AK_StoreGenericAmpKey(), SEC_AK_CalcDedicatedAmpLK(), and 
 *     SEC_AK_GetDedicatedAmpLK() will invoke it when needed.  
 *
 * Requires:  This code requires that the AMP Link Key information be zeroed 
 *     out in the remote device fields before a new BR\EDR connection is 
 *     established.  It also requires that the AMP Link Key information be 
 *     zeroed out in the remote device fields between BR/EDR connections.  
 *     If this is not the case, then this code will not work correctly.  
 *
 * Parameters:
 *     remDev - This parameter must point to a structure of type 
 *         BtRemoteDevice corresponding to the remote device for which the 
 *         AMP Link Key information is applicable.  
 *
 * Returns:  
 *     BT_STATUS_SUCCESS - The initial generic AMP Link Key value was 
 *         successfully calculated.  
 *
 *     BT_STATUS_FAILED - The initial generic AMP Link Key value could not 
 *         be written to the database.
 *
 *     BT_STATUS_NO_RESOURCES - The database is full so the initial generic 
 *         AMP Link Key value could not be written to the database.
 */
BtStatus SEC_AK_CalcGenericAmpLinkKey(BrEdrRemoteDevice *remDev);

#if SEC_GET_GAMP == XA_ENABLED

/*---------------------------------------------------------------------------
 *            SEC_AK_GetGenericAmpLinkKey
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function gets the generic AMP link key.  This function is 
 *     only needed for testing purposes.  
 *
 * Requires:  This code requires that the AMP Link Key information be zeroed 
 *     out in the remote device fields before a new BR\EDR connection is 
 *     established.  It also requires that the AMP Link Key information be 
 *     zeroed out in the remote device fields between BR/EDR connections.  
 *     If this is not the case, then this code will not work correctly.  
 *
 * Parameters:
 *     remDev - This parameter must point to a structure of type 
 *         BtRemoteDevice corresponding to the remote device for which the 
 *         AMP Link Key information is applicable.  
 *
 *     genericAmpKeyInfo - This parameter must point to a structure of type 
 *         BtAmpGenKey where the Generic AMP Link Key information can be 
 *         output.  
 *
 * Return:  
 *     BT_STATUS_SUCCESS - The structure pointed at by genericAmpKeyInfo has 
 *         been filled in with the generic AMP Link Key information.  
 *
 *     BT_STATUS_NOT_FOUND - Either no initial generic AMP Key record was 
 *         found, or the temporary copy of the initial generic AMP link key 
 *         was not usable (implying it was never calculated).  
 */
BtStatus SEC_AK_GetGenericAmpLinkKey(BrEdrRemoteDevice *remDev, 
                                     BtAmpGenKey    *genericAmpKeyInfo);

#endif /* SEC_GET_GAMP == XA_ENABLED */ 

/*---------------------------------------------------------------------------
 *            SEC_AK_StoreGenericAmpKey
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function saves a previously calculated initial generic 
 *     AMP Link Key value.  It should be called after a connection has 
 *     successfully been established.  This function does not need to be 
 *     invoked directly, since SEC_AK_CalcDedicatedAmpLK() will invoke it 
 *     when needed.  
 *
 * Requires:  This code requires that the AMP Link Key information be zeroed 
 *     out in the remote device fields before a new BR\EDR connection is 
 *     established.  It also requires that the AMP Link Key information be 
 *     zeroed out in the remote device fields between BR/EDR connections.  
 *     If this is not the case, then this code will not work correctly.  
 *
 * Parameters:
 *     remDev - This parameter must point to a structure of type 
 *         BtRemoteDevice corresponding to the remote device for which the 
 *         AMP Link Key information is applicable.  
 *
 * Returns:  
 *     BT_STATUS_SUCCESS - The previously calculated initial generic AMP 
 *         Link Key value has been successfully saved.  
 *
 *     BT_STATUS_FAILED - The previously calculated initial generic AMP Link 
 *         Key value could not be written to the database.
 *
 *     BT_STATUS_NO_RESOURCES - The database is full so the previously 
 *         calculated initial generic AMP Link Key value could not be 
 *         written to the database.
 */
BtStatus SEC_AK_StoreGenericAmpKey(BrEdrRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 *            SEC_AK_CalcDedicatedAmpLK
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function checks for an existing usable dedicated AMP Link 
 *     Key for the given AMP type.  If one does not exist for the given AMP 
 *     type, it creates one.  This function does not need to be invoked 
 *     directly, since SEC_AK_GetDedicatedAmpLK() will invoke it when 
 *     needed.  This function must not be called unless a BR/EDR connection 
 *     is successfully set up.
 *
 * Requires:  This code requires that the AMP Link Key information be zeroed 
 *     out in the remote device fields before a new BR\EDR connection is 
 *     established.  It also requires that the AMP Link Key information be 
 *     zeroed out in the remote device fields between BR/EDR connections.  
 *     If this is not the case, then this code will not work correctly.  
 *
 * Parameters:
 *     remDev - This parameter must point to a structure of type 
 *         BtRemoteDevice corresponding to the remote device for which the 
 *         AMP Link Key information is applicable.  
 *
 *     crlType - This must contain an AMP controller type for which 
 *         the dedicated AMP link key is to be calculated.  
 *
 * Return:  
 *     BT_STATUS_SUCCESS - Either the dedicated AMP Link Key already exists, 
 *         or it was successfully created.  
 *
 *     BT_STATUS_FAILED - The dedicated AMP Link Key does not exist and 
 *         could not be created.  
 */
BtStatus SEC_AK_CalcDedicatedAmpLK(BrEdrRemoteDevice *remDev, 
                                   BtControllerType   crlType);

/*---------------------------------------------------------------------------
 *            SEC_AK_GetDedicatedAmpLK
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function returns information about the dedicated AMP Link 
 *     Key for the given AMP type, if it exists.  If the dedicated AMP Link 
 *     Key has not been calculated, then SEC_AK_GetDedicatedAmpLK() invokes 
 *     SEC_AK_CalcDedicatedAmpLK() to do the calculation so that the value 
 *     can be returned.  This function must not be called unless a BR/EDR 
 *     connection is successfully set up.
 *
 * Requires:  This code requires that the AMP Link Key information be zeroed 
 *     out in the remote device fields before a new BR\EDR connection is 
 *     established.  It also requires that the AMP Link Key information be 
 *     zeroed out in the remote device fields between BR/EDR connections.  
 *     If this is not the case, then this code will not work correctly.  
 *
 * Parameters:
 *     remDev - This parameter must point to a structure of type 
 *         BtRemoteDevice corresponding to the remote device for which the 
 *         AMP Link Key information is applicable.  
 *
 *     crlType - This must contain an AMP controller type for which 
 *         the dedicated AMP link key is to be output.  
 *
 *     dedicatedAmpKeyInfo - This parameter must point to a structure of 
 *         type BtAmpDedKey where the dedicated AMP link key information can 
 *         be output.  
 *
 * Return:  
 *     BT_STATUS_SUCCESS - Either the dedicated AMP Link Key already exists, 
 *         or it was successfully created.  
 *
 *     BT_STATUS_FAILED - The dedicated AMP Link Key does not exist and 
 *         could not be created.  
 */
BtStatus SEC_AK_GetDedicatedAmpLK(BrEdrRemoteDevice  *remDev, 
                                  BtControllerType crlType, 
                                  BtAmpDedKey     *dedicatedAmpKeyInfo);

#endif /* MESECAMP_H */ 

