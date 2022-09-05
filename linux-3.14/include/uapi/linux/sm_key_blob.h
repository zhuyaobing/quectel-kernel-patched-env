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

/* store the detected attributes of a secure memory page */
struct sm_key_blob {
	u32 plain_key_length;	/* key length in bytes */
	void __iomem *plain_key;/* pointer to plain key */
	u32 crypto_blob_length;	/* length of encrypted blob in bytes, should be at least (key_length_byte + 8) bytes */
	void __iomem *crypto_blob;	/* pointer to encrypted blob */
	u32 unit;		/* key store unit number */
	u32 keyslot;		/* keyslot allocated */
};

/* initialize sm store for blob generation */
#define IOC_KEY_BLOB_INIT      _IOWR('Z', 1, struct sm_key_blob *)

/* encrypt key and generate blob */
#define IOC_KEY_ENCRYPT       _IOWR('Z', 2, struct sm_key_blob *)

/* decrypt blol and get key */
#define IOC_KEY_DECRYPT       _IOWR('Z', 3, struct sm_key_blob *)

/* deinitialize sm store */
#define IOC_KEY_BLOB_DEINIT       _IOR('Z', 4, struct sm_key_blob *)

#endif
