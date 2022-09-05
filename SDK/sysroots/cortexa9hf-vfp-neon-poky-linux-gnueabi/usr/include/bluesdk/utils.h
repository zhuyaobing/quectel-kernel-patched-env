/***************************************************************************
 *
 * File:
 *     utils.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Includes for utility functions.
 *
 * Created:
 *     October 1, 1996
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

#ifndef __UTILS_H
#define __UTILS_H

#include "xatypes.h"
#include "xastatus.h"

/*---------------------------------------------------------------------------
 * Utility API layer
 *
 *     Provides utility functions such as functions to manage doubly linked 
 *     lists; moving, copying and filling memory; and decoding strings. 
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

#if XA_DECODER == XA_ENABLED

/*---------------------------------------------------------------------------
 * XAD_MAX_TYPES constant
 *
 *     Defines the maximum number of types that can be registered
 *     concurrently with XadRegister().
 */
#ifndef XAD_MAX_TYPES
#define XAD_MAX_TYPES 80
#endif

/*---------------------------------------------------------------------------
 * XAD_MAX_STRING_SIZE constant
 *
 *     Defines the maximum size of a string that can be generated at
 *     runtime by Xad(). This limit does not apply to strings that are
 *     "hard-coded" in an XadEntry.
 */
#ifndef XAD_MAX_STRING_SIZE
#define XAD_MAX_STRING_SIZE 30
#endif

/*---------------------------------------------------------------------------
 * XAD_MAX_STRINGS constant
 *
 *     Defines the maximum number of statically-allocated strings to
 *     cycle through. Because the memory for strings generated at run-time
 *     is reused during calls to Xad(), increasing the string count
 *     will reduce the chance of a corrupted printout.
 */
#ifndef XAD_MAX_STRINGS
#define XAD_MAX_STRINGS 16
#endif

#endif /* XA_DECODER == XA_ENABLED */

/*---------------------------------------------------------------------------
 * XLOG_MAX_CUSTOM_LEVELS constant
 *
 *     Defines the upper number of tags that can be customized via
 *     XLOG_SetLevel().
 */
#ifndef XLOG_MAX_CUSTOM_LEVELS
#define XLOG_MAX_CUSTOM_LEVELS 20
#endif

#if XLOG_MAX_CUSTOM_LEVELS < 1
#error XLOG_MAX_CUSTOM_LEVELS must be at least one
#endif

/*---------------------------------------------------------------------------
 * XLOG_MAX_TAG_NAME constant
 *
 *     Defines the maximum supported tag size. Tags may actually be longer but
 *     only this amount of each tag will be matched against custom levels set
 *     with XLOG_SetLevel(). This is also the longest tag that will be detected
 *     in legacy Report(()) statements.
 */
#ifndef XLOG_MAX_TAG_NAME
#define XLOG_MAX_TAG_NAME 20
#endif

/*---------------------------------------------------------------------------
 * XLOG_MAX_MSG constant
 *
 *     Defines the maximum message size that can be sent through the logging
 *     system. Longer messages will be truncated.
 */
#ifndef XLOG_MAX_MSG
#define XLOG_MAX_MSG 512
#endif

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * XlogLevel type
 *
 *     Defines the logging levels available.
 */
typedef U8 XlogLevel;

/* Verbose messages should include everything interesting that is happening
 * within a module. This may include which functions are being entered, each
 * byte of data that is sent or received, etc.
 */
#define XLOG_VERBOSE   1

/* Debug messages should include enough information to track what is going on
 * within a module without overwhelming the log output. This may include
 * significant state transitions, user requests, operation status, etc.
 */
#define XLOG_DEBUG     2

/* Error messages should include only those cases where an error was detected.
 * This may include poorly formed requests, failed operations, unexpected
 * data, etc.
 */
#define XLOG_ERROR     3

/* This error level indicates that NO output is to be delivered. */
#define XLOG_NONE      0xFF

/* End of XlogLevel  */

/*---------------------------------------------------------------------------
 * XlogOutputFunc type
 *
 *     A callback type which outputs a log message for the specified tag.
 *
 * Parameters:
 *     tag - Specifies the module which originated the message.
 *     level - Specifies the level of the message.
 *     file - The full path of the file which originated the message,
 *         if known. Empty string ("") if not known.
 *     line - The line number which originated the message, if known.
 *         0 if not known.
 *     msg - Message to output
 */
typedef void (*XlogOutputFunc)(const char *tag, XlogLevel level, const char *file, U16 line,
                               const char *msg);
/* End of XlogOutputFunc*/

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * ListEntry structure
 *
 *     ListEntry is for creating doubly linked circular lists. They can be 
 *     used as either a list head, or as part of another structure to allow 
 *     that structure to be a link node.
 */
#ifndef LISTS_DEFINED
#define LISTS_DEFINED 

typedef struct  _ListEntry
{
    struct _ListEntry *Flink;
    struct _ListEntry *Blink;

} ListEntry;

#endif 

/*---------------------------------------------------------------------------
 * XaBufferDesc structure
 *
 *     The XA Buffer Descriptor is used for managing data buffers that are 
 *     shared between protocol layers. The number of bytes of data available 
 *     to read is writeOffset - readOffset.
 */
typedef struct _XaBufferDesc {
    ListEntry    link;          /* For Queuing, must be first */
    U8           flags;         /* Flag storage for use by callers */
    U16          writeOffset;   /* Write offset into buff */
    U16          readOffset;    /* Read offset into buff */
    U16          buffSize;      /* Maximum size of associated buffer */
    U8          *buff;
} XaBufferDesc;

/*---------------------------------------------------------------------------
 * XaStager structure
 *
 *     Used to manage the "staging" of input data. "Staging" refers to the
 *     process of collecting input data, which may arrive segmented, into
 *     a single contiguous buffer for processing.
 */
typedef struct _XaStager {
    U8 *buff;
    U16 cur;
    U16 target;
    BOOL skip;
} XaStager;

#if XA_DECODER == XA_ENABLED

/*---------------------------------------------------------------------------
 * XadEntry structure
 *
 *    A single instruction that tells how to decode a value into a string.
 *    Typically formed into an array and passed to XAD_RegisterDecoder().
 *
 *    XadEntries can be written out manually, or the XAD_ENTRY* macros can
 *    be used as a shortcut.
 */
typedef struct _XadEntry {
    U32 value;      /* Value to match */
    void *str;      /* String or special code */
} XadEntry;


/*---------------------------------------------------------------------------
 * XadContext structure
 *
 *     Internal context memory used by the XA_DECODER system. This memory
 *     must be allocated, prefilled to 0 at init time, and supplied to
 *     the XA_DECODER code in debug.c via the XADC() macro. If the XADC
 *     macro is not defined, debug.c will allocate its own RAM.
 */
typedef struct _XadContext  {
    /* === Internal use only === */
    const XadEntry *entries[XAD_MAX_TYPES];
    U16             entryLens[XAD_MAX_TYPES];
    const char     *typeNames[XAD_MAX_TYPES];

    char decodeStrings[XAD_MAX_STRINGS][XAD_MAX_STRING_SIZE];
    U8 curDecodeString;
} XadContext;

#endif /* XA_DECODER == XA_ENABLED */

/*---------------------------------------------------------------------------
 * XlogCustomLevel structure
 *
 *     Internal use only.
 */
typedef struct _XlogCustomLevel {
    ListEntry node;
    char tag[XLOG_MAX_TAG_NAME + 1];
    XlogLevel level;
} XlogCustomLevel;

/*---------------------------------------------------------------------------
 * XlogContext structure
 *
 *     Internal context memory used by the XLOG system. Initialized by
 *     XLOG_Init().
 */
typedef struct _XlogContext  {
    /* === Internal use only === */
    XlogLevel defaultLevel; /* Must be first */
    XlogOutputFunc outputFunc;
    XlogCustomLevel customAlloc[XLOG_MAX_CUSTOM_LEVELS];
    ListEntry customList, customFree;
} XlogContext;

/* ---------------------------------------------------------------------------
 * Base64Context structure
 *
 *     This structure is used to provide context during Base64 encode and 
 *     decode operations.
 */
typedef struct _Base64Context {
    U8      stageBuff[4];
    U8      stageOffset;
} Base64Context;

/****************************************************************************
 *
 * Function and Macro Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * B64_InitTable()
 *
 *     Initializes table used for encoding binary values in B64 format 
 *
 *
 * Parameters:
 *     table - buffer to hold B64 encoding values
 *     len - length of buffer, should be > 66 bytes
 *
 */
void B64_InitTable(U8 *table, U8 len);

/*---------------------------------------------------------------------------
 * B64_Encode()
 *
 *     Encodes binary data into the B64 format 
 *
 *
 * Parameters:
 *     b64Table - buffer holding B64 encoding values initialized with B64_InitTable()
 *     Out - buffer to hold B64 encoded string (Out->buff len should be 4*n/3 rounded
 *           up to the nearest multiple of 4, where n is the number of input bytes)
 *     In - buffer holding data to be encoded
 *     Bp - stucture to provide context during B64 enconde / decode
 *     Last - True
 *
 * Returns:
 *     True if the data was encoded successfully
 */
BOOL B64_Encode(U8 *b64Table, XaBufferDesc *Out, XaBufferDesc *In, 
                Base64Context *Bp, BOOL Last);

/*---------------------------------------------------------------------------
 * B64_Decode()
 *
 *     Decodes B64 string into binary data. 
 *
 *
 * Parameters:
 *     Out - buffer to hold decoded data (Out->buf len should be 3*n/4 rounded
 *           up to the nearest multiple of 3, where n is the number of input chars)
 *     In - buffer holding B64 encoded string
 *     Bp - stucture to provide context during B64 enconde / decode
 *
 * Returns:
 *     True if string was decoded successfully
 */
BOOL B64_Decode(XaBufferDesc *Out, XaBufferDesc *In, Base64Context *Bp);

/* For backward compatibility */
BOOL StrnCmp(const U8* str1, U16 len1, const U8* str2, U16 len2);
#define StrnCmp(s1, l1, s2, l2) OS_MemCmp(s1, l1, s2, l2)

/*---------------------------------------------------------------------------
 * max()
 *
 *     Macro returns the larger of the two items passed to it. 
 *
 *
 * Parameters:
 *     a - first item
 *     b - second item
 *
 * Returns:
 *     Item that is the largest
 */
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif /* max */

/*---------------------------------------------------------------------------
 * min()
 *
 *     Macro returns the smaller of the two items passed to it. 
 *
 *
 * Parameters:
 *     a - first item
 *     b - second item
 *
 * Returns:
 *     Item that is the smallest
 */
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif /* min */

/*---------------------------------------------------------------------------
 * ToUpper()
 *
 *     Macro to convert lower case ASCII characters to upper case. 
 *
 *
 * Parameters:
 *     ch - character to convert
 *
 * Returns:
 *     Converted character
 */
#ifndef ToUpper
#define ToUpper(ch)  (U8)((((ch) >= 'a' && (ch) <= 'z') ? ((ch) - 'a' + 'A') : (ch)))
#endif

/*---------------------------------------------------------------------------
 * ALIGN()
 *
 *     Data structure alignment macro. Some modules aggregate data allocations,
 *     care must be taken to insure that dynamically allocated structures fall
 *     on appropriate byte boundries. The configuration constant XA_INTEGER_SIZE
 *     is used to force alignment on 16 or 32 bit boundries. All data structures
 *     will be sub-allocated on byte boundries that match the XA_INTEGER_SIZE.
 *
 *     The ALIGN() macro will grow lengths to a multiple of
 *     XA_INTEGER_SIZE byte. When XA_INTEGER_SIZE == 4 the macro will look like:
 *     ((length + 3) & 0xFFFFFFFC) 
 *
 *
 * Parameters:
 *     _LEN - length in bytes of structure
 *
 * Returns:
 *     The new length
 */

#ifndef ALIGN
#define ALIGN(_LEN)     (((_LEN) + XA_INTEGER_SIZE - 1) & (U32)(0-XA_INTEGER_SIZE))
#endif

/*---------------------------------------------------------------------------
 * CheckUnlockedParm()
 *
 *     Verifies the validity of calling parameters outside
 *     of a function's OS_LockStack/OS_UnlockStack pair. The behavior of
 *     this macro depends on both XA_ERROR_CHECK and XA_DEBUG.
 *
 *     If XA_ERROR_CHECK is enabled, the expression will be evaluated. If the
 *     expression is false, the macro will return from the current function
 *     with the specified return code.
 *
 *     If XA_DEBUG is enabled, but XA_ERROR_CHECK is disabled, the expression
 *     will be evaluated. If the expression is false, the macro will assert.
 *
 *     If XA_DEBUG and XA_ERROR_CHECK are both disabled, the expression will
 *     not be evaluated.
 *
 * Parameters:
 *     code - Status code to return when "exp" is false (only when
 *         XA_ERROR_CHECK is enabled)
 *     exp - Expression to evaluate
 */
#if XA_ERROR_CHECK == XA_ENABLED
#define CheckUnlockedParm(code, exp) if (!(exp)) return (code)
#else /* XA_ERROR_CHECK is disabled */
#if XA_DEBUG == XA_ENABLED
#define CheckUnlockedParm(code, exp) Assert(exp)
#else /* XA_DEBUG is disabled as well */
#define CheckUnlockedParm(code, exp) (void)0
#endif /* XA_DEBUG */
#endif /* XA_ERROR_CHECK */

/*---------------------------------------------------------------------------
 * CheckUnlockedParmVoid()
 *
 *     Verifies the validity of calling parameters outside
 *     of a function's OS_LockStack/OS_UnlockStack pair. The behavior of
 *     this macro depends on both XA_ERROR_CHECK and XA_DEBUG.
 *
 *     If XA_ERROR_CHECK is enabled, the expression will be evaluated. If the
 *     expression is false, the macro will return from the current function.
 *
 *     If XA_DEBUG is enabled, but XA_ERROR_CHECK is disabled, the expression
 *     will be evaluated. If the expression is false, the macro will assert.
 *
 *     If XA_DEBUG and XA_ERROR_CHECK are both disabled, the expression will
 *     not be evaluated.
 *
 * Parameters:
 *     exp - Expression to evaluate
 */
#if XA_ERROR_CHECK == XA_ENABLED
#define CheckUnlockedParmVoid(exp) if (!(exp)) return
#else /* XA_ERROR_CHECK is disabled */
#if XA_DEBUG == XA_ENABLED
#define CheckUnlockedParmVoid(exp) Assert(exp)
#else /* XA_DEBUG is disabled as well */
#define CheckUnlockedParmVoid(exp) (void)0
#endif /* XA_DEBUG */
#endif /* XA_ERROR_CHECK */

/*---------------------------------------------------------------------------
 * CheckLockedParm()
 *
 *     Verifies the validity of calling parameters inside
 *     of a function's OS_LockStack/OS_UnlockStack pair. This macro
 *     acts just as CheckUnlockedParm does, except that it will
 *     call OS_UnlockStack before returning with the specified return code.
 *
 * Parameters:
 *     code - Status code to return when "exp" is false (only when
 *         XA_ERROR_CHECK is enabled)
 *     exp - Expression to evaluate
 */
#if XA_ERROR_CHECK == XA_ENABLED
#define CheckLockedParm(code, exp) do { if (!(exp)) { OS_UnlockStack(); \
                        return (code);} } while(0)
#else /* XA_ERROR_CHECK is disabled */
#if XA_DEBUG == XA_ENABLED
#define CheckLockedParm(code, exp) Assert(exp)
#else /* XA_DEBUG is disabled as well */
#define CheckLockedParm(code, exp) (void)0
#endif /* XA_DEBUG */
#endif /* XA_ERROR_CHECK */

/*---------------------------------------------------------------------------
 * CheckParmExit()
 *
 *     Verifies the validity of calling parameters. The behavior of
 *     this macro depends on both XA_ERROR_CHECK and XA_DEBUG.
 *
 *     If XA_ERROR_CHECK is enabled, the expression will be evaluated. If the
 *     expression is false, this macro will set the "status" variable to
 *     the specified error code, then goto the "exit" label.
 *
 *     If XA_DEBUG is enabled, but XA_ERROR_CHECK is disabled, the expression
 *     will be evaluated. If the expression is false, the macro will assert.
 *
 *     If XA_DEBUG and XA_ERROR_CHECK are both disabled, the expression will
 *     not be evaluated.
 *
 *     IMPORTANT--This macro requires a local variable (called "status")
 *     to be defined by the function in which this macro occurs. The
 *     "status" variable is referenced by this macro only when XA_ERROR_CHECK
 *     is enabled. This macro requires the exit label "exit", for convenience
 *     the label "Check_Parm_Exit" can be used.
 *
 * Parameters:
 *     code - Status code to assign to "status" when "exp" is false (only when
 *         XA_ERROR_CHECK is enabled).
 *
 *     label - The name of a goto label. Code will jump to this label
 *         when "exp" is false (only when XA_ERROR_CHECK is enabled).
 *
 *     exp - Expression to evaluate
 */
#if XA_ERROR_CHECK == XA_ENABLED
#define CheckParmExit(code, exp) do { if (!(exp)) { status = (code); \
                                      goto exit; } } while(0)
#define Check_Parm_Exit  exit:
#else /* XA_ERROR_CHECK is disabled */
#if XA_DEBUG == XA_ENABLED
#define CheckParmExit(code, exp) Assert(exp)
#define Check_Parm_Exit
#else /* XA_DEBUG is disabled as well */
#define CheckParmExit(code, exp) (void)0
#define Check_Parm_Exit
#endif /* XA_DEBUG */
#endif /* XA_ERROR_CHECK */

/*---------------------------------------------------------------------------
 * LEtoHost16()
 *
 *     Converts the two octets that are stored the buffer pointed to by 
 *     le_value in little endian format to a 16-bit unsigned integer. 
 *
 * Parameters:
 *     le_value - Pointer to two octets in little endian format
 *
 * Returns:
 *     U16 value
 */
U16 LEtoHost16(const U8 *le_value);

/*---------------------------------------------------------------------------
 * LEtoHost32()
 *
 *     Converts the four octets that are stored the buffer pointed to by 
 *     le_value in little endian format to a 32-bit unsigned integer. 
 *
 * Parameters:
 *     le_value - Pointer to four octets in little endian format
 *
 * Returns:
 *     U32 value
 */
U32 LEtoHost32(const U8 *le_value);

/*---------------------------------------------------------------------------
 * BEtoHost16()
 *
 *     Converts the two octets that are stored the buffer pointed to by 
 *     be_value in big endian format to a 16-bit unsigned integer. 
 *
 * Parameters:
 *     be_value - Pointer to two octets in big endian format
 *
 * Returns:
 *     U16 value
 */
U16 BEtoHost16(const U8* be_ptr);

/*---------------------------------------------------------------------------
 * BEtoHost32()
 *
 *     Converts the four octets that are stored the buffer pointed to by 
 *     be_value in big endian format to a 32-bit unsigned integer. 
 *
 * Parameters:
 *     be_value - Pointer to two octets in big endian format
 *
 * Returns:
 *     U32 value
 */
U32 BEtoHost32(const U8* be_ptr);

/*---------------------------------------------------------------------------
 * StoreLE16()
 *
 *     Stores an unsigned 16-bit integer into a buffer in little endian 
 *     format.  
 *
 * Parameters:
 *     buff - pointer to buffer to receive the bytes
 *     le_value - unsigned 16-bit integer
 *
 * Returns:
 *     void
 */
void StoreLE16(U8 *buff, U16 le_value);

/*---------------------------------------------------------------------------
 * StoreLE32()
 *
 *     Stores an unsigned 32-bit integer into a buffer in little endian 
 *     format.  
 *
 * Parameters:
 *     buff - pointer to buffer to receive the bytes
 *     le_value - unsigned 32-bit integer
 *
 * Returns:
 *     void
 */
void StoreLE32(U8 *buff, U32 le_value);

/*---------------------------------------------------------------------------
 * StoreBE16()
 *
 *     Stores an unsigned 16-bit integer into a buffer in big endian format.  
 *
 * Parameters:
 *     buff - pointer to buffer to receive the bytes
 *     le_value - unsigned 16-bit integer
 *
 * Returns:
 *     void
 */
void StoreBE16(U8 *ptr, U16 be_value);

/*---------------------------------------------------------------------------
 * StoreBE32()
 *
 *     Stores an unsigned 32-bit integer into a buffer in big endian format.  
 *
 * Parameters:
 *     buff - pointer to buffer to receive the bytes
 *     le_value - unsigned 32-bit integer
 *
 * Returns:
 *     void
 */
void StoreBE32(U8 *ptr, U32 be_value);

/*---------------------------------------------------------------------------
 * ConvertToBigEndian()
 *
 *     Converts a buffer (of fixed size) from Little Endian to Big Endian
 *     format.
 *
 * Parameters:
 *     dest - pointer to buffer to receive the bytes
 *     source - pointer to buffer containing bytes to convert
 *     length - length of array to convert
 *
 * Returns:
 *     void
 */
void ConvertToBigEndian(U8 *dest, const U8 *source, U16 length);

/*---------------------------------------------------------------------------
 * ConvertToLittleEndian()
 *
 *     Converts a buffer (of fixed size) from Big Endian to Little Endian
 *     format.
 *
 * Parameters:
 *     dest - pointer to buffer to receive the bytes
 *     source - pointer to buffer containing bytes to convert
 *     length - length of array to convert
 *
 * Returns:
 *     void
 */
void ConvertToLittleEndian(U8 *dest, const U8 *source, U16 length);

#if XA_USE_ENDIAN_MACROS == XA_ENABLED

/* Little Endian to Host integer format conversion macros */
#define LEtoHost16(ptr)  (U16)( ((U16) *((U8*)(ptr)+1) << 8) | \
                                ((U16) *((U8*)(ptr))) )

#define LEtoHost32(ptr)  (U32)( ((U32) *((U8*)(ptr)+3) << 24) | \
                                ((U32) *((U8*)(ptr)+2) << 16) | \
                                ((U32) *((U8*)(ptr)+1) << 8)  | \
                                ((U32) *((U8*)(ptr))) )

/* Big Endian to Host integer format conversion macros */
#define BEtoHost16(ptr)  (U16)( ((U16) *((U8*)(ptr)) << 8) | \
                                ((U16) *((U8*)(ptr)+1)) )

#define BEtoHost32(ptr)  (U32)( ((U32) *((U8*)(ptr)) << 24)   | \
                                ((U32) *((U8*)(ptr)+1) << 16) | \
                                ((U32) *((U8*)(ptr)+2) << 8)  | \
                                ((U32) *((U8*)(ptr)+3)) )

/* Store value into a buffer in Little Endian format */
#define StoreLE16(buff,num) ( ((buff)[1] = (U8) ((num)>>8)),    \
                              ((buff)[0] = (U8) (num)) )

#define StoreLE32(buff,num) ( ((buff)[3] = (U8) ((num)>>24)),  \
                              ((buff)[2] = (U8) ((num)>>16)),  \
                              ((buff)[1] = (U8) ((num)>>8)),   \
                              ((buff)[0] = (U8) (num)) )

/* Store value into a buffer in Big Endian format */
#define StoreBE16(buff,num) ( ((buff)[0] = (U8) ((num)>>8)),    \
                              ((buff)[1] = (U8) (num)) )

#define StoreBE32(buff,num) ( ((buff)[0] = (U8) ((num)>>24)),  \
                              ((buff)[1] = (U8) ((num)>>16)),  \
                              ((buff)[2] = (U8) ((num)>>8)),   \
                              ((buff)[3] = (U8) (num)) )

#endif /* XA_USE_ENDIAN_MACROS == XA_ENABLED */


/*---------------------------------------------------------------------------
 * XaBufferReadLen()
 *
 *     Macro returns the length of data available to read from an XA Buffer.  
 *
 * Parameters:
 *     b - pointer to XA buffer
 *
 * Returns:
 *     length
 */
#define XaBufferReadLen(b) ((U16)((b)->writeOffset - (b)->readOffset))

/*---------------------------------------------------------------------------
 * XaBufferWrite()
 *
 *     Macro writes from specified src into XaBufferDesc.  
 *
 * Parameters:
 *     b - pointer to XA buffer
 *     src - pointer to source of bytes to write
 *     len - number of bytes to write
 *
 * Returns:
 *     void
 */
#define XaBufferWrite(b, src, len) \
    do { \
        OS_MemCopy((b)->buff + (b)->writeOffset, (const U8 *)(src), (len)); \
        (b)->writeOffset = (U16)((b)->writeOffset + (len)); } \
    while (0)

/*---------------------------------------------------------------------------
 * XaBufferWriteChar()
 *
 *     Macro writes a single char into the buffer.  
 *
 * Parameters:
 *     b - pointer to XA buffer
 *     c - character to write
 *
 * Returns:
 *     void
 */
#define XaBufferWriteChar(b, c)  (b)->buff[(b)->writeOffset++] = (c)

#define          XA_WBXML_BUFFER      0
#define          XA_NOT_WBXML_BUFFER  1

/*---------------------------------------------------------------------------
 * UNUSED_PARAMETER()
 *
 *     Macro sed to define unused function parameters. Some compilers warn if this
 *     is not done.  
 *
 * Parameters:
 *     _PARM - unused parameter
 *
 * Returns:
 *     void
 */
#ifndef UNUSED_PARAMETER /* CESL defines UNUSED_PARAMETER in basewin.h */
#define UNUSED_PARAMETER(_PARM)     ((_PARM) = (_PARM))
#endif

#ifndef LIST_MACROS
/*---------------------------------------------------------------------------
 * InitializeListHead()
 *
 *     Initialize a ListEntry structure to be the head of a doubly link list.
 *     The ListEntry is initialized to be an empty list. 
 *
 * Parameters:
 *     head - Pointer to ListEntry structure to be the head
 *
 * Returns:
 *     void
 */
void InitializeListHead(ListEntry *head);
#define InitializeListHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead) )

/*---------------------------------------------------------------------------
 * InitializeListEntry()
 *
 *     Initialize a ListEntry structure to be an item on a doubly link list.
 *     The back and forward pointers are set to 0. This is not required for
 *     an item to be on a list but is useful in some functions to verify the
 *     item is not already on a list.
 *
 * Parameters:
 *     Entry - Pointer to Entry 
 *
 * Returns:
 *     void
 */
#define InitializeListEntry(Entry) (\
    (Entry)->Flink = (Entry)->Blink = 0 )

/*---------------------------------------------------------------------------
 * IsEntryAvailable()
 *
 *     Returns TRUE if the Entry is available to be put on a list (i.e. not
 *     already on another list. Otherwise it returns FALSE
 *
 * Parameters:
 *     Entry - Pointer to Entry 
 *
 * Returns:
 *     TRUE or FALSE
 */
#define IsEntryAvailable(Entry) (\
    ((Entry)->Flink == 0))

#ifndef IsListEmpty
/*---------------------------------------------------------------------------
 * IsListEmpty()
 *
 *     Returns TRUE if the list is empty otherwise returns FALSE. 
 *
 * Parameters:
 *     head - Pointer to head of the list 
 *
 * Returns:
 *     TRUE or FALSE
 */
BOOL IsListEmpty(ListEntry *head);
#define IsListEmpty(ListHead) (\
    ((ListHead)->Flink == (ListHead)))

#endif /* IsListEmpty */

/*---------------------------------------------------------------------------
 * GetHeadList()
 *
 *     Returns a pointer to the first item on the list. The item is not
 *     removed from the list.
 *
 * Parameters:
 *     ListHead - Pointer to head of the list 
 *
 * Returns:
 *     Pointer to first item on the list
 */
ListEntry *GetHeadList(ListEntry *ListHead);
#define GetHeadList(ListHead) (ListHead)->Flink

/*---------------------------------------------------------------------------
 * GetTailList()
 *
 *     Returns a pointer to the last item on the list. The item is not
 *     removed from the list. 
 *
 * Parameters:
 *     ListHead - Pointer to head of the list 
 *
 * Returns:
 *     Pointer to last item on the list
 */
ListEntry *GetTailList(ListEntry *ListHead);
#define GetTailList(ListHead) (ListHead)->Blink

/*---------------------------------------------------------------------------
 * GetNextNode()
 *
 *     Returns a pointer to the item on the list that comes after the given
 *     node. The item is not removed from the list
 *
 * Parameters:
 *     Node - Pointer to a node on a list 
 *
 * Returns:
 *     Pointer to next item
 */
ListEntry *GetNextNode(ListEntry *Node);
#define GetNextNode(Node)     (Node)->Flink

/*---------------------------------------------------------------------------
 * GetPriorNode()
 *
 *     Returns a pointer to the item on the list that comes before the given
 *     node. The item is not removed from the list
 *
 * Parameters:
 *     Node - Pointer to a node on a list 
 *
 * Returns:
 *     Pointer to previous item
 */
ListEntry *GetPriorNode(ListEntry *Node);
#define GetPriorNode(Node)    (Node)->Blink

/*---------------------------------------------------------------------------
 * IsNodeConnected()
 *
 *     Returns TRUE if the give node is connected on a list otherwise it
 *     return FALSE.
 *
 * Parameters:
 *     Node - Pointer to node 
 *
 * Returns:
 *     TRUE or FALSE
 */
BOOL IsNodeConnected(ListEntry *Node);
#define IsNodeConnected(n) ((n)->Blink && (n)->Flink && ((n)->Blink->Flink == (n)) && ((n)->Flink->Blink == (n)))

/*---------------------------------------------------------------------------
 * InsertTailList()
 *
 *     Insert the given entry onto the end (tail) of the list where head
 *     is a pointer to the head of the list.
 *
 * Parameters:
 *     head - Pointer to head of the list 
 *     entry - Pointer to entry to add to the list
 *
 * Returns:
 *     void
 */
void InsertTailList(ListEntry *head, ListEntry *entry);
void _InsertTailList(ListEntry* head, ListEntry* entry);
#define InsertTailList(head, entry) (Assert(IsListCircular(head)), \
                            _InsertTailList(head, entry), \
                            Assert(IsListCircular(head)))

/*---------------------------------------------------------------------------
 * InsertHeadList()
 *
 *     Insert the given entry onto the front(head) of the list where head
 *     is a pointer to the head of the list.
 *
 * Parameters:
 *     head - Pointer to head of the list 
 *     entry - Pointer to entry to add to the list
 *
 * Returns:
 *     void
 */
void InsertHeadList(ListEntry *head, ListEntry *entry);
void _InsertHeadList(ListEntry* head, ListEntry* entry);
#define InsertHeadList(head, entry) (Assert(IsListCircular(head)), \
                            _InsertHeadList(head, entry), \
                            Assert(IsListCircular(head)))

/*---------------------------------------------------------------------------
 * InsertBeforeNode()
 *
 *     Insert a node before another node on a list. 
 *
 * Parameters:
 *     target - Pointer to node already on the list
 *     toAdd - Pointer to node to add to the list
 *
 * Returns:
 *     void
 */
void InsertBeforeNode(ListEntry *target, ListEntry *toAdd);
#define InsertBeforeNode(target, toAdd) InsertTailList(target, toAdd)

/*---------------------------------------------------------------------------
 * InsertAfterNode()
 *
 *     Insert a node after another node on a list. 
 *
 * Parameters:
 *     target - Pointer to node already on the list
 *     toAdd - Pointer to node to add to the list
 *
 * Returns:
 *     void
 */
void InsertAfterNode(ListEntry *target, ListEntry *toAdd);
#define InsertAfterNode(target, toAdd) InsertHeadList(target, toAdd)

/*---------------------------------------------------------------------------
 * RemoveHeadList()
 *
 *     Remove the first item (head) from a list and return a pointer to it.  
 *
 * Parameters:
 *     head - Pointer to head of the list 
 *
 * Returns:
 *     Pointer to item removed from the list.
 */
ListEntry *RemoveHeadList(ListEntry *head);
ListEntry *_RemoveHeadList(ListEntry* head);
#define RemoveHeadList(head) (Assert(IsListCircular(head)), \
                            _RemoveHeadList(head))

/*---------------------------------------------------------------------------
 * RemoveEntryList()
 *
 *     Returns TRUE if the list is empty otherwise returns FALSE. 
 *
 * Parameters:
 *     head - Pointer to head of the list 
 *
 * Returns:
 *     TRUE or FALSE
 */
void RemoveEntryList(ListEntry* entry);

/*---------------------------------------------------------------------------
 * IsNodeOnList()
 *
 *     Returns TRUE if the given node is on the list pointed to by head
 *     otherwise FALSE is returned.
 *
 * Parameters:
 *     head - Pointer to head of the list 
 *     node - Pointer to a node.
 *
 * Returns:
 *     TRUE or FALSE
 */
BOOL IsNodeOnList(ListEntry* head, ListEntry* node);

/*---------------------------------------------------------------------------
 * IsListCircular()
 *
 *     Returns TRUE if the list is circular. This function is used to
 *     check the integrity of a list. It starts at the head and verifies
 *     that each node is connected properly. It returns FALSE if the list
 *     is corrupt.  
 *
 * Parameters:
 *     list - Pointer to head of the list 
 *
 * Returns:
 *     TRUE or FALSE
 */
BOOL IsListCircular(ListEntry* list);

/*---------------------------------------------------------------------------
 * MoveList()
 *
 *     Move all the items from one list to another list.  
 *
 * Parameters:
 *     dest - Pointer to head of the destination list 
 *     src - Pointer to head of the source list.
 *
 * Returns:
 *     void
 */
void MoveList(ListEntry* dest, ListEntry* src);
#endif /* LIST_MACROS */

/*---------------------------------------------------------------------------
 * IterateListSafe()
 *
 *     Sets up safe traversal of a list, preserving the "next" member so
 *     that the current member can be removed safely (with RemoveEntryList).
 *     Must be followed by a block of code containing the body of the
 *     iteration.
 *
 *     For example:
 *
 *     BtSecurityRecord *current, *next;
 *     IterateListSafe(MEC(secList), current, next, BtSecurityRecord *) {
 *         [...do something with "current". May issue callbacks on it, or
 *             remove it from the list...]
 *     }
 *
 * Parameters:
 *     head - Head of list (address of ListEntry structure)
 *     cur - Variable to use for current list member
 *     next - Variable to use for temporary storage of next list member
 *     type - Structure type of cur and next.
 */
#define IterateListSafe(head, cur, next, type) \
    for ( (cur) = (type) GetHeadList(&(head)) ; \
          (next) = (type) GetNextNode(&(cur)->node), \
            (cur) != (type) &(head); \
          (cur) = (next))

/*---------------------------------------------------------------------------
 * IterateList()
 *
 *     Sets up ordinary traversal of a list. The current member must NOT
 *     be removed during iteration. Must be followed by a block of code
 *     containing the body of the iteration.
 *
 *     For example:
 *     BtSecurityRecord *record;
 *     IterateList(MEC(secList), record, BtSecurityRecord *) {
 *         [...do something with "record", but do not remove it!...]
 *     }
 *
 * Parameters:
 *     head - Head of list (address of ListEntry structure)
 *     cur - Variable to use for current list member
 *     type - Structure type of cur and next.
 */
#define IterateList(head, cur, type) \
    for ( (cur) = (type) GetHeadList(&(head)) ; \
          (cur) != (type) &(head); \
          (cur) = (type) GetNextNode(&(cur)->node) )

#define IterateListAs(head, cur, nodeType, type) \
    for ( (cur) = (type) GetHeadList(&(head)) ; \
          (cur) != (type) &(head); \
          (cur) = (type) GetNextNode(&((nodeType)cur)->node) )

#if XA_DEBUG == XA_ENABLED
void VerifyMacros(void);
#endif /* XA_DEBUG == XA_ENABLED */

/*---------------------------------------------------------------------------
 * XA_StageInit()
 *
 *     Configures a staging buffer to read a certain number of bytes.
 *
 * Parameters:
 *     stager - Staging structure
 *
 *     buffer - Memory buffer used for staging the incoming data
 *
 *     len - Length of "buffer"
 */
void XA_StageInit(XaStager *stager, U8 *buffer, U16 len);
#define XA_StageInit(s,b,l) do { (s)->cur = 0; (s)->target = (l); (s)->skip=FALSE; (s)->buff = (b); } while (0)

/*---------------------------------------------------------------------------
 * XA_StageExtend()
 *
 *     Increases the expected number of bytes to be staged
 *
 * Parameters:
 *     stager - Staging structure
 *
 *     len - New, larger number of bytes to be staged in the buffer. The
 *         originally-supplied buffer must be large enough to accommodate this
 *         new length.
 */
void XA_StageExtend(XaStager *stager, U16 len);
#define XA_StageExtend(s,l) (s)->target = (l)

/*---------------------------------------------------------------------------
 * XA_StageSkip()
 *
 *     Configures a staging buffer to skip a certain number of bytes.
 *
 * Parameters:
 *     stager - Staging structure
 *
 *     len - Number of bytes to skip.
 */
void XA_StageSkip(XaStager *stager, U16 len);
#define XA_StageSkip(s,l) do { (s)->cur = 0; (s)->target = (l); (s)->skip=TRUE; (s)->buff = 0; } while (0)

/*---------------------------------------------------------------------------
 * XA_StageGetRead()
 *
 *     Returns number of bytes currently staged
 *
 * Parameters:
 *     stager - Staging structure
 *
 *     len - Number of bytes to skip.
 */
U16 XA_StageGetRead(XaStager *stager);
#define XA_StageGetRead(s) ((s)->cur)


/*---------------------------------------------------------------------------
 * XA_StageInput()
 *
 *     Stages input into a staging buffer.
 *
 * Parameters:
 *     stager - Staging structure.
 *
 *     inputBuff - On input, address of pointer to memory to read. On output,
 *         filled with the new starting address of input data.
 *
 *     inputLen - On input, pointer to length of data in inputBuff. On
 *         output, filled with number of unprocessed bytes.
 *
 * Returns:
 *     XA_STATUS_PENDING - More input data is required.
 *
 *     XA_STATUS_SUCCESS - Current stager has all the data it needs.
 *         Subsequent calls for this stager will also return _SUCCESS until
 *         the stager is reconfigured with a new target.
 *
 *     XA_STATUS_FAILED - Incorrect state of the staging structure so the
 *         staging failed.
 */
XaStatus XA_StageInput(XaStager *stager, const U8 **inputBuff, U16 *inputLen);


/****************************************************************************
 *
 * XA_DECODER elements
 *
 ****************************************************************************/


#if XA_DECODER == XA_ENABLED

/* Internal use - We assume that no legitimate pointers live at these memory
 * locations so that we can overload the void * as an opcode.
 */
#define XAD_OPCODE_HEXVALUE ((void *)0x01)
#define XAD_OPCODE_DROPBITS ((void *)0x02)

/*---------------------------------------------------------------------------
 * XAD_ENTRY()
 *
 *     A macro that results in a XadEntry structure which will convert
 *     a #define'd value into a string. For example:
 *
 *     XAD_ENTRY(BT_STATUS_SUCCESS),
 *
 *     will generate an entry that converts the #define'd value
 *     BT_STATUS_SUCCESS into the string "BT_STATUS_SUCCESS".
 *
 * Requires:
 *     XA_DECODER enabled.
 *
 * Parameters:
 *     value - The #define for the value in question.
 */
#define XAD_ENTRY(value)                { value, #value }

/*---------------------------------------------------------------------------
 * XAD_ENTRY_PREFIX()
 *
 *     A macro that results in a XadEntry structure which will convert
 *     a #define'd value into a string, WITHOUT its prefix. For example:
 *
 *     XAD_ENTRY_PREFIX(BT_STATUS_, SUCCESS),
 *
 *     will generate an entry that converts the #define'd value for
 *     BT_STATUS_SUCCESS into the string "SUCCESS".
 *
 * Requires:
 *     XA_DECODER enabled.
 *
 * Parameters:
 *     prefix - Prefix of the define value, which will be ommitted
 *         from the output.
 *
 *     value - Remainder of the define value, which will be included in
 *         the output.
 */
#define XAD_ENTRY_PREFIX(prefix, value) { prefix##value, #value }

/*---------------------------------------------------------------------------
 * XAD_ENTRY_HEX()
 *
 *     A macro that results in a XadEntry structure which will convert
 *     any unmatched value into a hexadecimal display of the defined width.
 *     For example, XAD_ENTRY_HEX(16) would cause the unmatched value 0x44
 *     to decode into "0x0044".
 *
 *     Note that XAD_ENTRY_HEX halts further processing of the XadEntries
 *     for the type. Therefore it should only be found at the end of the
 *     XadEntry array.
 *
 * Requires:
 *     XA_DECODER enabled.
 *
 * Parameters:
 *     width - Width of the hex output, in bits. May be 8, 16, or 32.
 */
#define XAD_ENTRY_HEX(width)            { width, XAD_OPCODE_HEXVALUE }

/*---------------------------------------------------------------------------
 * XAD_ENTRY_DROPBITS()
 *
 *     A macro that results in a XadEntry structure which forces the
 *     decoder to ignore a certain set of bits. During iteration through
 *     the entries, reaching this line will cause the bits specified
 *     to be zeroed out. Processing then continues normally.
 *
 * Requires:
 *     XA_DECODER enabled.
 *
 * Parameters:
 *     ignore - Bits to ignore.
 */
#define XAD_ENTRY_DROPBITS(bits)        { (U32)bits, XAD_OPCODE_DROPBITS }

/*---------------------------------------------------------------------------
 * XAD_RegisterDecoder()
 *
 *     Registers a decoder with the XA_DECODE mechanism so that XA_Decode()
 *     or Xad() can be used.
 *
 * Requires:
 *     XA_DECODER enabled.
 *
 * Parameters:
 *     entries - Array of XadEntry structures. When decoding, XA_Decode
 *         will iterate through this array to determine how to decode the
 *         value.
 *
 *     entriesLen - Number of XadEntries in the entries array.
 *
 *     typeName - The name of the type of interest, as a string.
 *
 */
void XAD_RegisterDecoder(const XadEntry *entries,
                         U16 entriesLen, const char *type);

/*---------------------------------------------------------------------------
 * XadRegister()
 *
 *     A shortcut for registering a type. Assumes the existance of an array
 *     called entries_TypeName containing the XadEntry array. For example,
 *     to register a decoder for an integer type called "MyType" with two
 *     possible values, define an array:
 *
 *     static const XadEntry entries_MyType[] = {
 *     XAD_ENTRY(MY_TYPE_YES), XAD_ENTRY(MY_TYPE_NO) };
 *
 *     Then in your routine's _Init() code, call "XadRegister(MyType);"
 *     Later, calling "Xad(var, MyType);" will return the string
 *     "MY_TYPE_YES" or "MY_TYPE_NO" depending on the value of var.
 *
 * Requires:
 *     XA_DECODER enabled.
 *
 * Parameters:
 *     type - The actual type name. This is not provided as a string but
 *         entered normally, for example: XadRegister(MyTypeName);
 *
 */
#define XadRegister(type) \
        XAD_RegisterDecoder(entries_##type, \
        sizeof(entries_##type)/sizeof(*entries_##type), #type)


/*---------------------------------------------------------------------------
 * XAD_Decode()
 *
 *     Converts a value into a string according to the rules for its type.
 *
 * Requires:
 *     XA_DECODE enabled.
 *
 * Parameters:
 *     value - Value to be decoded into a string. If the value is not
 *         recognized, the string returned will look similar to
 *         "Unknown: 0x0123"
 *
 *     typeName - String which specifies the type. If the type name is not
 *         recognized, the string returned will look similar to
 *         "Unknown type: 0x0123"
 *
 * Returns:
 *     A string which describes the value. The string should be used soon after
 *     this routine returns to avoid corrupted output. For example, passing
 *     this return value into a call to printf() will make a safe copy of the
 *     buffer. If output corruption occurs, consider increasing XAD_MAX_STRING.
 */
const char *XAD_Decode(U32 value, const char *typeName);

/*---------------------------------------------------------------------------
 * Xad()
 *
 *     A shortcut for invoking XAD_Decode.
 *
 * Requires:
 *     XA_DECODE enabled.
 *
 * Parameters:
 *     value - Value to be decoded into a string.
 *
 *     typeName - Type name. This is not provided as a string but simply
 *         typed out: Xad(17, TypeName);
 *
 * Returns:
 *     See XAD_Decode().
 */
const char *Xad(U32 value, void *typeName);
#define Xad(value, typeName) XAD_Decode(value, #typeName)

#else  /*  XA_DECODER == XA_ENABLED */

/* Some defines to quietly turn off decodes */
#define Xad(value,typeName)        "?"
#define XadRegister(t)             (void)0
#define XAD_Decode(value,typeName) "?"
#define XAD_RegisterDecoder(a,b,c) XadRegister(a)

#endif /*  XA_DECODER == XA_ENABLED */


/* Aliases for old-style decode functions - new code should use Xad() */
#define pHciCommand(a)          Xad(a, HciCommandType)
#define pHciEvent(a)            Xad(a, HciEventType)
#define pHC_Status(a)           Xad(a, HcStatus)
#define pBT_Status(a)           Xad(a, BtStatus)
#define pME_Event(a)            Xad(a, BtEventType)
#define pLLC_Event(a)           Xad(a, L2capSignalCode)
#define pL2CAP_SignalOpcode(a)  Xad(a, L2capSignalCode)
#define pL2CAP_ConnStatus(a)    Xad(a, L2capConnStatus)
#define pL2CAP_MoveRspStatus(a) Xad(a, L2capMoveRspStatus)
#define pL2CAP_MoveCnfStatus(a) Xad(a, L2capMoveCnfStatus)
#define pL2CAP_DiscReason(a)    Xad(a, L2capDiscReason)
#define pL2CAP_Event(a)         Xad(a, L2capEventType)
#define pSdpService(a)          Xad(a, SdpServiceClassUuid)
#define pCodMajorClass(a)       Xad(a, BtClassOfDeviceMajor)
#define pRFC_Command(a)         Xad(a, RfCommand)
#define pRFC_MuxCommand(a)      Xad(a, RfMuxCommand)
#define pRFC_BaudRate(a)        Xad(a, RfBaudrate)
#define pL2CAP_A2mpOpcode(a)    Xad(a, L2capA2mpOpcode)

/****************************************************************************
 *
 * XLOG elements
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * XLOG_Init()
 *
 *     Initializes the XLOG system. This clears all previously set levels,
 *     (if any) and defaults to XLOG_DEBUG for all log output. This function
 *     should be invoked before any Report(()) calls are made.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     void
 */
void XLOG_Init(void);

/*---------------------------------------------------------------------------
 * XLOG_SetLevel()
 *
 *     Sets the output logging level.
 *
 * Parameters:
 *     tag - If non-0, modules that output with this tag will be subject
 *         to the logging level as specified. If 0, updates the
 *         default level for all tags.
 *     level - Specifies the level of interest for the tag. Log entries
 *         with a lower level will not be sent to output.
 *
 * Returns:
 *     TRUE if the level is successfully applied, FALSE if a custom
 *     logging level could not be applied (because XLOG_MAX_CUSTOM_LEVELS
 *     is exceeded)
 */
BOOL XLOG_SetLevel(const char *tag, XlogLevel level);

/*---------------------------------------------------------------------------
 * XLOG_Test()
 *
 *     Indicates whether debug output is desired for this tag and level.
 *
 * Parameters:
 *     tag - The tag to test against.
 *     level - The level to test against.
 *
 * Returns:
 *     TRUE if output for the tag/level will be delivered, FALSE if it
 *     will be inhibited, according to current settings.
 */
BOOL XLOG_Test(const char *tag, XlogLevel level);

/*---------------------------------------------------------------------------
 * XLOG_SetOutputFunc()
 *
 *     Specifies a function to receive all Xlog...() output. If this function
 *     is not given or is 0, output is divered to OS_Report(...).
 *
 * Parameters:
 *     func - Function to use for Xlog output, or 0 to use OS_Report().
 *
 * Returns:
 *     The previously set output function or 0 if previously unspecified.
 */
XlogOutputFunc XLOG_SetOutputFunc(XlogOutputFunc func);

/*---------------------------------------------------------------------------
 * Xlogv()
 *
 *     Logs a XLOG_VERBOSE-level message using the currently-defined
 *     XLOG_TAG.
 *
 * Parameters:
 *     fmt - printf-style formatting string.
 *     ... - Additional arguments to the formatting string. These arguments
 *         will evaluated only if the XLOG_TAG's current output level is
 *         sufficient.
 */
void Xlogv(const char *tag, const char *fmt, ...);

/*---------------------------------------------------------------------------
 * Xlogd()
 *
 *     Logs a XLOG_DEBUG-level message using the currently-defined
 *     XLOG_TAG.
 *
 * Parameters:
 *     fmt - printf-style formatting string.
 *     ... - Additional arguments to the formatting string. These arguments
 *         will evaluated only if the XLOG_TAG's current output level is
 *         sufficient.
 */
void Xlogd(const char *tag, const char *fmt, ...);

/*---------------------------------------------------------------------------
 * Xloge()
 *
 *     Logs a XLOG_ERROR-level message using the currently-defined
 *     XLOG_TAG.
 *
 * Parameters:
 *     fmt - printf-style formatting string.
 *     ... - Additional arguments to the formatting string. These arguments
 *         will evaluated only if the XLOG_TAG's current output level is
 *         sufficient.
 */
void Xloge(const char *tag, const char *fmt, ...);

/*---------------------------------------------------------------------------
 * XLOG_PrintBuffer()
 *
 *     Prints up to the first 16 bytes of a buffer into an output
 *     string.
 *
 * Parameters:
 *     out - Target string for write; must contain room for at least 70 chars
 *         of output.
 *     buff - Pointer to first byte of buffer to print.
 *     buffLen - Maximum length of buffer to print.
 *
 * Returns:
 *     out which now contains a null-terminated string
 */
char *XLOG_PrintBuffer(char *out, const U8 *buff, U32 buffLen);

#if XA_DEBUG_PRINT == XA_ENABLED

/* Internal prototypes */
void XlogOut(const char *tag, U8 level, const char *file, U16 line, const char *fmt, ...);
void XlogReport(const char *fmt, ...);

#define Xlogv(tag, fmt, ...) \
    (XLOG_Test(tag, XLOG_VERBOSE) ? (XlogOut(tag, XLOG_VERBOSE, __FILE__, __LINE__, fmt, ##__VA_ARGS__),0) : 0)
#define Xlogd(tag, fmt, ...) \
    (XLOG_Test(tag, XLOG_DEBUG) ? (XlogOut(tag, XLOG_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__),0) : 0)
#define Xloge(tag, fmt, ...) \
    (XLOG_Test(tag, XLOG_ERROR) ? (XlogOut(tag, XLOG_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__),0) : 0)
#else
#define Xlogv(tag, fmt, ...) (void)0
#define Xlogd(tag, fmt, ...) (void)0
#define Xloge(tag, fmt, ...) (void)0
#endif /* XA_DEBUG_PRINT == XA_ENABLED */

/* When adding Xlog-based logging to modules that must compile with
 * earlier versions of utils.h, the following code can be used to re-route
 * logging back to OS_Report
 */

/* Allow backwards-compatible use of Xlog...() */
#if !defined(Xlogv)
#if XA_DEBUG_PRINT == XA_ENABLED
#if XLOG_VERBOSE == XA_ENABLED
#define Xlogv(tag, fmt, ...) OS_Report( tag " (/V): " fmt, ##__VA_ARGS__ )
#define XLOG_Test(tag, level) (level >= XLOG_VERBOSE)
#else /* XLOG_VERBOSE == XA_ENABLED */
#define Xlogv(tag, fmt, ...) 0
#define XLOG_Test(tag, level) (level >= XLOG_DEBUG)
#endif /* XLOG_VERBOSE == XA_ENABLED */
#define Xlogd(tag, fmt, ...) OS_Report( tag " (/D): " fmt, ##__VA_ARGS__ )
#define Xloge(tag, fmt, ...) OS_Report( tag " (/E): " fmt, ##__VA_ARGS__ )
#else /* XA_DEBUG_PRINT == XA_ENABLED */
#define Xlogv(tag, fmt, ...) (void)0
#define Xlogd(tag, fmt, ...) (void)0
#define Xloge(tag, fmt, ...) (void)0
#define XLOG_Test(tag, level) (void)0
#endif /* XA_DEBUG_PRINT == XA_ENABLED */
#endif /* !defined(Xlogv) */

#endif /* __UTILS_H */
