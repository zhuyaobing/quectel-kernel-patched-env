/****************************************************************************
 *
 * File:
 *     aes_encrypt.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Definitions of the API used by the stack for AES encryption.
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

#ifndef __AES_ENCRYPT
#define __AES_ENCRYPT

#include "bttypes.h"
#include "me.h"

/*---------------------------------------------------------------------------
 * Advanced Encryption Standard (AES) API layer
 *
 *     The Advanced Encryption Standard (AES) API is called by the stack
 *     to perform encryption and generate random numbers for the LE Privacy
 *     feature.
 *
 */
/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AES_Init()
 *
 *     Called by the stack to initialize the AES layer.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if operation was successful.
 *
 *     BT_STATUS_FAILED - Return this if the operation failed.
 */
BtStatus AES_Init(void);

/*---------------------------------------------------------------------------
 * AES_Deinit()
 *
 *     Called by the stack to deinitialize the AES layer.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if operation was successful.
 *
 *     BT_STATUS_FAILED - Return this if the operation failed.
 */
BtStatus AES_Deinit(void);


/*---------------------------------------------------------------------------
 * AES_Encrypt()
 *
 *     Called by the stack to encrypt data blocks using the AES algorithm.
 *     The format of the parameters given to this function and the result
 *     are the same as the HCI LE encrypt function.
 *
 * Parameters:
 *     key - Pointer to a 128-bit (16 octets) key in little endian format
 *        where octet 0 is the least significant octet of the key. This is the
 *        same format that keys are transported by SMP.
 *
 *     plainText - Pointer to 128 bit data block that is to be encrypted in
 *         little endian format where octet 0 is the least significant octet.
 *
 *     out - Pointer to a location to receive the 128 bit result. The result
 *        is in little endian format where octet 0 is the least significant
 *        octet.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if operation was successful.
 *
 *     BT_STATUS_FAILED - Return this if the operation failed.
 */
BtStatus AES_Encrypt(const U8 *key, const U8 *plainText, U8 *out);

/*---------------------------------------------------------------------------
 * AES_EncryptBE()
 *
 *     Called by the stack to encrypt data blocks using the AES algorithm.
 *     The format of the parameters given to this function and the result
 *     are in Big Endian format. This is the format used by cryptographic
 *     functions such as those used to generate CMAC.
 *
 * Parameters:
 *     key - Pointer to a 128-bit (16 octets) key in big endian format
 *        where octet 0 is the most significant octet of the key. 
 *
 *     plainText - Pointer to 128 bit data block that is to be encrypted
 *         in big endian format.
 *
 *     out - Pointer to a location to receive the 128 bit result. The result
 *        is in big endian format where octet 0 is the most significant
 *        octet.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if operation was successful.
 *
 *     BT_STATUS_FAILED - Return this if the operation failed.
 */
BtStatus AES_EncryptBE(const U8 *key, const U8 *plainText, U8 *out);

/*---------------------------------------------------------------------------
 * AES_Random()
 *
 *     Called by the stack to generate a 128-bit random number.
 *
 * Parameters:
 *     out - Pointer to location to receive the 128 bit result.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if operation was successful.
 *
 *     BT_STATUS_FAILED - Return this if the operation failed.
 */
BtStatus AES_Random(U8* out);

/*---------------------------------------------------------------------------
 * AES_GenerateIRK()
 *
 *     Called by the stack when the device is initialized to retrieve or
 *     generate an Identity Resolving Key (IRK). The generation method
 *     of the IRK is platform dependent. The IRK should be unique to each
 *     device and may be retrieved from a fixed location in device memory or
 *     generated randomly at runtime. The stack will call this function only
 *     at initialization time before any bonds exist in the device database.
 *
 * Parameters:
 *     Key - Pointer to location to receive the IRK.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if operation was successful.
 *
 *     BT_STATUS_FAILED - Return this if the operation failed.
 */
BtStatus AES_GenerateIRK(U8 *Key);

#endif /* __AES_ENCRYPT */
