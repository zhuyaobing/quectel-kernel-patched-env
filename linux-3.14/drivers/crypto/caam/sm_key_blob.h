/*
 * Copyright (C) 2016 OpenSynergy GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef SM_KEY_BLOB_H
#define SM_KEY_BLOB_H

#include <linux/ioctl.h>

/*
 * The blob overhead taken from drivers/crypto/caam/sm.h
 */
#define BLOB_OVERHEAD_BYTE 6
#define MAX_DATA_LENGTH 512
#define MAX_BLOB_LENGTH (MAX_DATA_LENGTH + BLOB_OVERHEAD_BYTE)

/* store the detected attributes of a secure memory page */
struct sm_key_blob
{
  /* Internal to SM */
  u32 _internal_unit;		                 /* key store unit number */
  u32 _internal_keyslot;		             /* keyslot allocated */

  /* User data buffer */
  u32 length;                           /* buffer length in bytes */
  u8 __iomem buffer[MAX_BLOB_LENGTH];   /* Plain data in when encrypting blob after and vice versa for decrypt  */

};

/* encrypt key and generate blob */
#define IOC_KEY_ENCRYPT       _IOWR('Z', 2, struct sm_key_blob *)

/* decrypt blol and get key */
#define IOC_KEY_DECRYPT       _IOWR('Z', 3, struct sm_key_blob *)

#endif
