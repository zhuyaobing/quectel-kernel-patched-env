/****************************************************************************
 *
 * File:
 *     stacklib_override.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Override values for the stack library.
 *
 * Copyright 2010-2017 OpenSynergy GmbH.
 * All rights reserved.  All unpublished rights reserved.
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

#ifndef __STACKLIB_OVERRIDE_H
#define __STACKLIB_OVERRIDE_H

/* WARNING: Do not change the values in this file. The library 'libbluesdk.a'
 * was generated using these values.  Changing this values without generating
 * a new library will cause applications to fail.
 * 
 * To generate a libary with different values, please contact your
 * support representative.
 */

#define BT_STACK               XA_ENABLED
#define PLATFORM_BYTE_ORDER    IS_LITTLE_ENDIAN
#define XA_INTEGER_SIZE        4

#if defined(XA_ENABLE_DEBUG)
#define XA_DEBUG          XA_ENABLED
#define XA_DEBUG_PRINT    XA_ENABLED
#else                     
#define XA_DEBUG          XA_DISABLED
#define XA_DEBUG_PRINT    XA_DISABLED
#endif

/*
 * Core stack module initialization.
 *
 * Profile- or application-specific init at OS_Init time can be
 * called directly from the application, before using any API call.
 */
#define XA_LOAD_LIST XA_MODULE(CMGR)

/* Enable the maximum number of active devices */
#define NUM_BT_DEVICES                  7
#define NUM_KNOWN_DEVICES               200
#define NUM_LE_DEVICES                  1       /* Change to 0 to disable LE support */
#define DS_NUM_SERVICES                 20

/* Delcares that stack is in library form */
#define BT_STACK_LIB                    XA_ENABLED

/* A2DP needs a larger packet header */
#define BT_PACKET_HEADER_LEN            28

/* Enable statistics gathering */
#define XA_STATISTICS                   XA_ENABLED

/* Enable HCI decoding */
#define XA_SNIFFER                      XA_ENABLED

/* Enable FTE CPAS Capture functionality in Blue Manager */
#define FTE_CPAS_CAPTURE                XA_ENABLED

/* Allow paging and inquiry during connections */
#define BT_ALLOW_SCAN_WHILE_CON         XA_ENABLED

/*
 * Page and Inquiry Scan Parameters
 */
#define BT_ALLOW_SETTING_SCAN_PARMS     XA_ENABLED
#define BT_DEFAULT_INQ_SCAN_INTERVAL    0x0000
#define BT_DEFAULT_INQ_SCAN_WINDOW      0x0000
#define BT_DEFAULT_PAGE_SCAN_INTERVAL   0x0000
#define BT_DEFAULT_PAGE_SCAN_WINDOW     0x0000

/* Allow a SCO connection for audio based profiles */
#define NUM_SCO_CONNS                   1

/* Enable SCO over HCI */
#define BT_SCO_HCI_DATA                 XA_ENABLED
#define SCO_ERRONEOUS_DATA              XA_ENABLED

/*
 * HCI configuration
 */

#define HCI_NUM_SCO_BUFFERS             8
#define BT_SCO_HCI_NUM_PACKETS          8
#define HCI_SCO_DATA_SIZE               (NUM_SCO_CONNS * 60)
#define HCI_NUM_PACKETS                 20
#define HCI_NUM_COMMANDS                3
#define HCI_NUM_EVENTS                  10

/* HCI Three-Wire */
#define TWIRE_UART_MULTIPLIER           1
#define TWIRE_SLIDING_WINDOW            3

/* #define TWIRE_RETRANSMIT_TIME           150 */
#define TWIRE_RX_BUFF_SIZE              1024
#define TWIRE_ACK_TIME                  10

/*
 * L2CAP configuration
 */

#define L2CAP_NUM_ENHANCED_CHANNELS     8

/* Set prelude size for BNEP */
#define L2CAP_PRELUDE_SIZE              7

/* Allow L2CAP deregistration */
#define L2CAP_DEREGISTER_FUNC           XA_ENABLED

/* Allow L2CAP_GetInfo() */
#define L2CAP_GET_INFO_SUPPORT          XA_ENABLED

/* ERTM sizes are increased to support HDP profile */
#define L2CAP_ERTM_TX_WIN_SIZE          31
#define L2CAP_ERTM_RX_WIN_SIZE          31

/* Adjust these for tests as necessary */
#define L2CAP_ERTM_SREJ_ENABLE          XA_ENABLED
#define L2CAP_EXT_WINDOW_SIZE           XA_DISABLED

/* Allow a number of simultaneous users of l2cap */
#define L2CAP_NUM_PROTOCOLS             24

/* Give the user time to respond to L2CAP commands */
#define L2CAP_RTX_TIMEOUT               60

/* Increase the number of retransmissions before disc */
#define L2CAP_ERTM_MAX_TRANSMIT         32

/*
 * Ensure the L2CAP_MPS is optimized to fit in an HCI receive buffer. The
 * MPS should be HCI_ACL_DATA_SIZE less HCI and L2CAP headers and FCS.
 *
 * L2CAP_MPS = (HCI_ACL_DATA_SIZE - 4(HCI) - 8(L2CAP) - 2(FCS))
 */

/* Uncomment these lines if L2CAP_MPS is defined in this file */
/*
#if L2CAP_MPS > HCI_ACL_DATA_SIZE
#error L2CAP_MPS Should be less than HCI_ACL_DATA_SIZE
#endif
*/

/* Uncomment for small MTU tests. */
/*#define L2CAP_MTU (L2CAP_MPS*2)*/

/*
 * SDP configuration
 */
#define SDP_SERVER_SEND_SIZE            L2CAP_MPS

/*
 * RFCOMM configuration
 */

#define NUM_RF_SERVERS                  12

/*
 * BTSOCK configuration
 */

/* NOTE: Enabling this option also requires NUM_LE_DEVICES to be > 0 */
#define BTSOCK_LEGATT_ENABLED           XA_ENABLED

/* Enable to process LE encrypt information passed to/from the LE controller
 * as big-endian, rather than little-endian.  Most LE controllers use little-endian
 * currently, although there is a conflicting statement in the LE specification.
 * This define will remain in place until this issue is fully resolved.
 */
/* #define ENCRYPT_USING_BIG_ENDIAN */

/* Enable for internal testing purposes */
#define JETTEST                         XA_DISABLED

/* Settings for the Userheap memory allocation. By default, the oslib calls OS_Malloc,
 * OS_Free and OS_ReAlloc use the malloc, free and realloc calls provided by the
 * operating system. By setting this option, these oslib calls will use the internal
 * userheap allocator instead. These allocate memory blocks from a static memory pool
 * that is by default allocated in oslib.c. The integrator can change the size of this
 * pool by changing USERHEAP_DEFAULT_POOL_SIZE or even allocate the memory differently:
 * on an embedded system it could even be a physical memory location.
 *
 * To help determine a heap size for userheap, you can enable the XA_DEBUG option, 
 * which allows you to use the UH_DumpStatistics call. This call reports the heap
 * usage statistics, including the highest amount of memory used so far.
 *
 * Whether or not oslib has been configured to use userheap or not, you can still also
 * use userheap directly. In this case you need to allocate one or more of your own heaps,
 * which you can then supply in the UH_Alloc function call.
 */

#define OSLIB_USE_USERHEAP_ALLOCATOR    XA_DISABLED
#define OSLIB_USERHEAP_POOL_SIZE        51200 /* 50 kilobytes */

#if XA_DEBUG == XA_ENABLED
/* if debug is enabled, then userheap statistics are also enabled. */
#define UH_STATISTICS                   XA_ENABLED
#endif

/* Enable for various PTS testing tweaks    */
/* #define PTS_CONNECT_TWEAKS                */
/* #define PTS_SIGNED_WRITE_TWEAK           */      /* For SMP PTS tests */
/* #define PTS_FORCE_WRITE_LONG             */      /* For GATT PTS tests */
/* #define PTS_RELIABLE_WRITE_TWEAKS        */      /* For GATT PTS tests */
/* #define PTS_TWEAK_PRIV_and_GCEP_BV_03_C  */
/* #define PTS_TWEAK_NO_PRIV_SEARCH_ON_CONN */
/* #define PTS_TWEAK_FIXED_LOCAL_CSRK       */
/* #define PTS_ENCRYPT_TWEAKS               */

/* Now that our selections are made, enforce them by locking down
 * all of the core stack config parameters to their final values.
 * Changing any of them in overide.h will cause a macro-redefinition
 * warning.
 */
#undef __BTCONFIG_H
#undef __CONFIG_H
#include "btconfig.h"

#endif /* __STACKLIB_OVERRIDE_H */

