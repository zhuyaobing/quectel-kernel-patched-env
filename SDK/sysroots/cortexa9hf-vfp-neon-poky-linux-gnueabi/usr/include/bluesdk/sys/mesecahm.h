/***************************************************************************
 *
 * File:
 *     mesecahm.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     HMAC header information for the portion of the Bluetooth Management 
 *     Entity that handles security for devices that have Alternate 
 *     MAC/Physical (AMP) radios.
 *
 * Created:
 *     2002
 *
 * Copyright 2002-2003 Brian Gladman, Worcester, UK. Refer to the original
 * source header below for original Copyright information, disclaimer, 
 * and terms of use.
 * 
 * Portions copyright 2010-2017 OpenSynergy GmbH.  
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

/*
 ---------------------------------------------------------------------------
 Copyright (c) 2002, Dr Brian Gladman, Worcester, UK.   All rights reserved.

 LICENSE TERMS

 The free distribution and use of this software in both source and binary
 form is allowed (with or without changes) provided that:

   1. distributions of this source code include the above copyright
      notice, this list of conditions and the following disclaimer;

   2. distributions in binary form include the above copyright
      notice, this list of conditions and the following disclaimer
      in the documentation and/or other associated materials;

   3. the copyright holder's name is not used to endorse products
      built using this software without specific written permission.

 ALTERNATIVELY, provided that this notice is retained in full, this product
 may be distributed under the terms of the GNU General Public License (GPL),
 in which case the provisions of the GPL apply INSTEAD OF those given above.

 DISCLAIMER

 This software is provided 'as is' with no explicit or implied warranties
 in respect of its properties, including, but not limited to, correctness
 and/or fitness for purpose.
 ---------------------------------------------------------------------------
 Issue Date: 26/08/2003

 This is an implementation of HMAC, the FIPS standard keyed hash function
*/

#ifndef MESECAHM_H
#define MESECAHM_H

#include "config.h"
#include "osapi.h"
#include "btconfig.h"
#define USE_SHA256

/* #include <memory.h> */

#if defined(__cplusplus)
extern "C"
{
#endif

#if !defined(USE_SHA1) && !defined(USE_SHA256)
#error define USE_SHA1 or USE_SHA256 to set the HMAC hash algorithm
#endif

#ifdef USE_SHA1

#include "sha1.h"

#define HASH_INPUT_SIZE     SHA1_BLOCK_SIZE
#define HASH_OUTPUT_SIZE    SHA1_DIGEST_SIZE
#define sha_ctx             sha1_ctx
#define sha_begin           sha1_begin
#define sha_hash            sha1_hash
#define sha_end             sha1_end

#endif

#ifdef USE_SHA256

#include "sys/mesecash.h"

#define HASH_INPUT_SIZE     SHA256_BLOCK_SIZE
#define HASH_OUTPUT_SIZE    SHA256_DIGEST_SIZE
#define sha_ctx             sha256_ctx
#define sha_begin           sha256_begin
#define sha_hash            sha256_hash
#define sha_end             sha256_end

#endif

#define HMAC_OK                0
#define HMAC_BAD_MODE         -1
#define HMAC_IN_DATA  0xffffffff

typedef struct
{   unsigned char   key[HASH_INPUT_SIZE];
    sha_ctx         ctx[1];
    unsigned long   klen;
} hmac_ctx;

void hmac_sha_begin(hmac_ctx cx[1]);

int  hmac_sha_key(const unsigned char key[], unsigned long key_len, hmac_ctx cx[1]);

void hmac_sha_data(const unsigned char data[], unsigned long data_len, hmac_ctx cx[1]);

void hmac_sha_end(unsigned char mac[], unsigned long mac_len, hmac_ctx cx[1]);

void hmac_sha(const unsigned char key[], unsigned long key_len,
          const unsigned char data[], unsigned long data_len,
          unsigned char mac[], unsigned long mac_len);

#if defined(__cplusplus)
}
#endif

#endif /* MESECAHM_H */
