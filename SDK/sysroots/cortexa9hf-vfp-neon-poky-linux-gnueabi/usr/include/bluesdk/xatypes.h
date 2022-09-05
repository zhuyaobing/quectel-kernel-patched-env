/***************************************************************************
 *
 * File:
 *     xatypes.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file contains declarations for OpenSynergy protocol stacks.
 *
 * Created:
 *     November 19, 1996
 *
 * Copyright 1996-2017 OpenSynergy GmbH.
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

#ifndef __XATYPES_H
#define __XATYPES_H

#include "config.h"

/****************************************************************************
 *
 * Base definitions used by the stack and framer
 *
 ****************************************************************************/

#ifndef BOOL_DEFINED
typedef int BOOL;   /* IGNORESTYLE */
#define BOOL_DEFINED
#endif

/* The typedef statements below define the base data types used by the stack.
 * The provided definitions are common for a 32 bit system and some 64 bit 
 * systems.  For systems where the defines below do not equate to 32, 16, and 
 * 8 bit sizes, you must override these typedef values. 
 *  
 * If it is necessary to overide these values, add the necessary typedef 
 * statements to your overide.h file.  You must then define each accompanying 
 * compiler directive to prevent redefinition (i.e. "#define U32_DEFINED"). 
 *  
 * It is important to define each data type to the correct size.  U32 must be 
 * 32 bits, U16 must be 16 bits, and U8 must be 8 bits. 
 */

#ifndef U32_DEFINED
typedef unsigned int U32;
#define U32_DEFINED
#endif

#ifndef U16_DEFINED
typedef unsigned short U16;
#define U16_DEFINED
#endif

#ifndef U8_DEFINED
typedef unsigned char U8;
#define U8_DEFINED
#endif

#ifndef S32_DEFINED
typedef signed int S32;
#define S32_DEFINED
#endif

#ifndef S16_DEFINED
typedef signed short S16;
#define S16_DEFINED
#endif

#ifndef S8_DEFINED
typedef signed char S8;
#define S8_DEFINED
#endif

/* Defines storage wide enough for either a 32-bit integer OR a pointer.
 * Normally (on a 32 bit system) pointers are 32 bit so this can just be a U32.
 *
 * In 64-bit systems, U32_PTR must be as wide as pointer variables, which
 * are 64 bits.  To override this value, add a typedef statment to overide.h,
 * and define U32_PTR_DEFINED to prevent redefinition.
 */
#ifndef U32_PTR_DEFINED
#ifdef _WIN64
typedef unsigned long long U32_PTR;
#else
typedef unsigned long U32_PTR;
#endif
#define U32_PTR_DEFINED
#endif /* U32_PTR_DEFINED */

/* Variable sized integers. Used to optimize processor efficiency by
 * using the most efficient data size for counters, arithmatic, etc.
 */
typedef unsigned long  I32;

#if XA_INTEGER_SIZE == 8
typedef unsigned long  I16;
typedef unsigned long  I8;
#elif XA_INTEGER_SIZE == 4
typedef unsigned long  I16;
typedef unsigned long  I8;
#elif XA_INTEGER_SIZE == 2
typedef unsigned short I16;
typedef unsigned short I8;
#elif XA_INTEGER_SIZE == 1
typedef unsigned short I16;
typedef unsigned char  I8;
#else
#error No XA_INTEGER_SIZE specified!
#endif /* XA_INTEGER_SIZE == 8 */

typedef void (*PFV) (void);

/* Boolean Definitions */
#ifndef TRUE
#define TRUE  (1==1)
#endif /* TRUE */

#ifndef FALSE
#define FALSE (0==1)  
#endif /* FALSE */

#endif /* __XATYPES_H */
