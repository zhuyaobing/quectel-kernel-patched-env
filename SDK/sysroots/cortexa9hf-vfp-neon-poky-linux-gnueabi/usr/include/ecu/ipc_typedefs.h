/*
***************************************************************************************
***************************************************************************************
***
***     File: typedefs.h
***
***     Project: Alexandra
***
***     Copyright (C) Stoneridge Electronics AB, 1990 - 2016
***
***     The information in this document belongs to Stoneridge, Inc. or its
***     subsidiaries and may be confidential. This document and information
***     in it may not be copied, reproduced, revealed or appropriated, either
***     in whole or in part, without the prior written consent of Stoneridge, Inc.
***
***************************************************************************************
**************************************************************************************/
/** \file

   \ingroup    COMMONDEF

   \Subject    Systemwide definitions

   \Reference  N/A

   \Abstract

   This header file contains Stoneridge Electronics standardized storage class
   definitions, data types and constants for use systemwide.


   \Version Id:

   "$Id: typedefs.h 447 2014-10-21 12:22:33Z PSM $"


@{
***************************************************************************************
*/




/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#if 0
#include <vm.h>
#endif

namespace ecu {
namespace lapi {
namespace ipc {

/*
**=====================================================================================
** Multibyte value storge style: BIG_ENDIAN/LITTLE_ENDIAN
**=====================================================================================
*/

#define IPC_BYTE_ORDER  LITTLE_ENDIAN

/*
**=====================================================================================
** Identifier scope control
**=====================================================================================
*/

#define Public
#define Private            static


/*
**=====================================================================================
** Data type definitions
**=====================================================================================
*/

typedef signed char        S8;
typedef unsigned char      U8;
typedef signed short       S16;
typedef unsigned short     U16;
typedef signed long        S32;
typedef unsigned long      U32;
typedef signed long long   S64;
typedef unsigned long long U64;

typedef union {
   U16 u16;
   U8 u8[2];
} Split16;

typedef union {
   U32 u32;
   U16 u16[2];
   U8 u8[4];
} Split32;

typedef union {
   U64 u64;
   U32 u32[2];
   U16 u16[4];
   U8 u8[8];
} Split64;

/* TODO: Removed below since TRUE and FALSE is defined by p4types.h which is included through vm.h */
/*
typedef enum {
   FALSE = 0,
   OFF   = 0,
   TRUE  = 1,
   ON    = 1
} Boolean;
*/
typedef int Boolean;

typedef enum {
   XFALSE = 0,
   XTRUE  = 1,
   XERROR = 2,
   XNA    = 3
} XBool;


/*
**====================================================================================
** Constants for use with split data types according to BIG_ENDIAN storage style
**====================================================================================
*/

# if IPC_BYTE_ORDER == BIG_ENDIAN

#define SPLIT16_MS_U8      0u /* Most significant U8   */
#define SPLIT16_LS_U8      1u /* Least significant U8  */

#define SPLIT32_MS_U16     0u /* Most significant U16  */
#define SPLIT32_LS_U16     1u /* Least significant U16 */
#define SPLIT32_MS_U8      0u /* Most significant U8   */
#define SPLIT32_2S_U8      1u /* 2:nd significant U8   */
#define SPLIT32_3S_U8      2u /* 3:rd significant U8   */
#define SPLIT32_LS_U8      3u /* Least significant U8  */

#define SPLIT64_MS_U32     0u /* Most significant U32  */
#define SPLIT64_LS_U32     1u /* Least significant U32 */
#define SPLIT64_MS_U16     0u /* Most significant U16  */
#define SPLIT64_2S_U16     1u /* 2:nd significant U16  */
#define SPLIT64_3S_U16     2u /* 3:rd significant U16  */
#define SPLIT64_LS_U16     3u /* Least significant U16 */
#define SPLIT64_MS_U8      0u /* Most significant U8   */
#define SPLIT64_2S_U8      1u /* 2:nd significant U8   */
#define SPLIT64_3S_U8      2u /* 3:rd significant U8   */
#define SPLIT64_4S_U8      3u /* 4:th significant U8   */
#define SPLIT64_5S_U8      4u /* 5:th significant U8   */
#define SPLIT64_6S_U8      5u /* 6:th significant U8   */
#define SPLIT64_7S_U8      6u /* 7:th significant U8   */
#define SPLIT64_LS_U8      7u /* Least significant U8  */


#endif


/*
**====================================================================================
** Constants for use with split data types according to LITTLE_ENDIAN storage style
**====================================================================================
*/

#if IPC_BYTE_ORDER == LITTLE_ENDIAN

#define SPLIT16_MS_U8      1u /* Most significant U8   */
#define SPLIT16_LS_U8      0u /* Least significant U8  */

#define SPLIT32_MS_U16     1u /* Most significant U16  */
#define SPLIT32_LS_U16     0u /* Least significant U16 */
#define SPLIT32_MS_U8      3u /* Most significant U8   */
#define SPLIT32_2S_U8      2u /* 2:nd significant U8   */
#define SPLIT32_3S_U8      1u /* 3:rd significant U8   */
#define SPLIT32_LS_U8      0u /* Least significant U8  */

#define SPLIT64_MS_U32     1u /* Most significant U32  */
#define SPLIT64_LS_U32     0u /* Least significant U32 */
#define SPLIT64_MS_U16     3u /* Most significant U16  */
#define SPLIT64_2S_U16     2u /* 2:nd significant U16  */
#define SPLIT64_3S_U16     1u /* 3:rd significant U16  */
#define SPLIT64_LS_U16     0u /* Least significant U16 */
#define SPLIT64_MS_U8      7u /* Most significant U8   */
#define SPLIT64_2S_U8      6u /* 2:nd significant U8   */
#define SPLIT64_3S_U8      5u /* 3:rd significant U8   */
#define SPLIT64_4S_U8      4u /* 4:rd significant U8   */
#define SPLIT64_5S_U8      3u /* 5:rd significant U8   */
#define SPLIT64_6S_U8      2u /* 6:rd significant U8   */
#define SPLIT64_7S_U8      1u /* 7:rd significant U8   */
#define SPLIT64_LS_U8      0u /* Least significant U8  */
#endif


/*
**====================================================================================
** symbolic constants
**====================================================================================
*/

#ifndef NULL
   #define NULL            0
#endif

#define NOT_USED(x) (void)x


}}} // end namespace


#endif


/************************************************************************************/
/** @}
************************************ End of file ************************************/
