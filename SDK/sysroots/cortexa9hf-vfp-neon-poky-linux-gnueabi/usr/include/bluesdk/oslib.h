/****************************************************************************
 *
 * File:
 *     oslib.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Defines the APIs used by the stack to access system services.
 *
 * Copyright 2001-2017 OpenSynergy GmbH.
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

#ifndef __OSLIB_H
#define __OSLIB_H

#include "xatypes.h"


/*---------------------------------------------------------------------------
 * Operating System Library API layer
 *
 *     The OS API is a set of functions used by the stack to access
 *     system services. If you are porting to a new operating system,
 *     you must provide the functions described here.
 *
 *     The functions in this API are intended for exclusive use by the
 *     stack. Applications should use the native operating system interface
 *     and avoid making OS_ calls themselves.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * OS_Malloc()
 *
 *     Allocate a block of memory.
 *
 * Parameters:
 *     Size - Bytes to allocate.
 *
 * Returns:
 *     Zero -  if allocation failed.
 *     MemBlockPtr - if allocation succeeded.
 */
void *OS_Malloc(U32 Size);


/*---------------------------------------------------------------------------
 * OS_ReAlloc()
 *
 *     Reallocates the contents of a block of memory to a new size.
 *
 * Parameters:
 *     Block - Previously allocated memory block to be resized
 *     newSize - Size for new block
 *
 * Returns:
 *     New address of memory. Memory may have been moved to a new location.
 *     If so, the previous block's contents are copied to the new location.
 *     The new location has newSize bytes available. If a move was not
 *     required, simply returns Block.
 *
 *     Returns 0 if re-allocation failed, in which case the original block
 *     is left alone. May also return 0 if newSize is zero.
 */
void *OS_ReAlloc(void *Block, U32 newSize);

/*---------------------------------------------------------------------------
 * OS_Free()
 *
 *     Deallocates or frees a memory block.
 *
 * Parameters:
 *     MemBlock - Previously allocated memory block to be freed
 */
void OS_Free(void *MemBlock);

/*---------------------------------------------------------------------------
 * OS_Atoi()
 *
 *    Converts a base-10 number string to an unsigned 32-bit value.
 *
 * Parameters:
 *     buff - a pointer to the sting based representation of the number.
 * Returns:
 *     The 32 bit integer value.
 */
U32 OS_Atoi(const char *buff);

/*---------------------------------------------------------------------------
 * OS_Itoa()
 *
 *    Converts a in integer value to base 10 ASCII. Returns the number of
 *    bytes consumed by the string (NOT null-terminated).
 *
 * Parameters:
 *     buff - Pointer to a memory buffer for holding the resulting string.
 *     val  - The 32 bit integer value to be converted to a string.
 *
 * Returns:
 *    The number of bytes of the buffer used to represent the number.
 */
U8 OS_Itoa(char *buff, U32 val);

/*---------------------------------------------------------------------------
 * OS_LibCheck()
 *
 *    Check if the OS Library has any memory blocks in use. Error checking only.
 *
 * Parameters:
 *     none  
 *
 */
void OS_LibCheck(void);


#endif /* __OSLIB_H */

