/****************************************************************************
 *
 * File:
 *     btconfig.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Default constants for configuring the Bluetooth protocol stack.
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

#ifndef __BTCONFIG_H
#define __BTCONFIG_H

#include "config.h"

#if !defined(BT_STACK) || BT_STACK == XA_DISABLED
#error "Bluetooth Stack must be enabled!"
#endif

/*---------------------------------------------------------------------------
 * Bluetooth Configuration API layer
 *
 *     The constants in this layer configure the layers of the Bluetooth
 *     stack. To change a constant, simply #define it in your overide.h
 *     include file.
 *
 *     Configuration constants here and in the General Configuration API
 *     (config.h) are used to make the stack more appropriate for a
 *     particular environment. Constants can be modified to allow tradeoffs
 *     between code size, RAM usage, functionality, and throughput.
 *
 *     Some constants are numeric, and others indicate whether a feature
 *     is enabled (defined as XA_ENABLED) or disabled (defined as
 *     XA_DISABLED).
 */

/****************************************************************************
 *
 * Section: General Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BT_STACK_VERSION
 *     Defines the stack's version number.
 */
#define BT_STACK_VERSION 552

/*---------------------------------------------------------------------------
 * NUM_BT_DEVICES constant
 *
 *     The number of devices with which we can connect. This value
 *     represents the maximum size of the piconet in which this device is
 *     the master, plus any master devices for which we are concurrently a
 *     slave. This value includes devices which may be parked, holding,
 *     or sniffing as well any active devices.
 *
 *     The default value is 2.
 */
#ifndef NUM_BT_DEVICES
#define NUM_BT_DEVICES  2
#endif

#if NUM_BT_DEVICES < 1
#error NUM_BT_DEVICES must be greater than 0
#endif

#if NUM_BT_DEVICES > 255
#error NUM_BT_DEVICES must be less than 256
#endif

/*---------------------------------------------------------------------------
 * NUM_SCO_CONNS constant
 *
 *     Defines the number of SCO connections supported by this device.
 *     When NUM_SCO_CONNS is set to 0, SCO capabilities are disabled
 *     to save code size.
 *
 *     The default value is 0, and the maximum is 3.
 */
#ifndef NUM_SCO_CONNS
#define NUM_SCO_CONNS 0
#endif

#if NUM_SCO_CONNS > 3
#error NUM_SCO_CONNS must be 3 or less
#endif

/*---------------------------------------------------------------------------
 * NUM_AMP_HCI_CONTROLLERS constant
 *
 *     Defines the number of AMP HCI Controllers to be allocated. The memory
 *     is only allocated if NUM_AMP_HCI_CONTROLLERS is > 0. Note that the
 *     specific type of AMP controller must be set as well. For example
 *     if the AMP controller is of type 802.11 and it is an HCI controller
 *     (i.e. uses AmpHciController class) then both NUM_AMP_HCI_CONTROLLERS
 *     would be set as well as NUM_AMP_802_CONTROLLERS.
 *
 *
 *     The default is 0.
 */
#ifndef NUM_AMP_HCI_CONTROLLERS
#define NUM_AMP_HCI_CONTROLLERS 0
#endif

/*---------------------------------------------------------------------------
 * NUM_AMP_802_CONTROLLERS constant
 *
 *     Defines the number of AMP Controllers using the 802 PAL. If there
 *     802.11 controllers that use AMP HCI then NUM_AMP_HCI_CONTROLLERS
 *     must be set as well.
 *
 *     The default is 0.
 */
#ifndef NUM_AMP_802_CONTROLLERS
#define NUM_AMP_802_CONTROLLERS 0
#endif

/*---------------------------------------------------------------------------
 * NUM_AMP_UDP_CONTROLLERS constant
 *
 *     Defines the number of AMP UDP Controllers (Fake PAL) to be allocated.
 *     The memory is only allocated if NUM_AMP_UDP_CONTROLLERS is > 0.
 *
 *     The default is 0.
 */
#ifndef NUM_AMP_UDP_CONTROLLERS
#define NUM_AMP_UDP_CONTROLLERS 0
#endif

#if NUM_AMP_UDP_CONTROLLERS > 1
#error NUM_AMP_UDP_CONTROLLERS cannot be > 1
#endif

/*---------------------------------------------------------------------------
 * NUM_AMP_TCP_CONTROLLERS constant
 *
 *     Defines the number of AMP TCP Controllers to be allocated.
 *     The memory is only allocated if NUM_AMP_TCP_CONTROLLERS is > 0.
 *
 *     The default is 0.
 */
#ifndef NUM_AMP_TCP_CONTROLLERS
#define NUM_AMP_TCP_CONTROLLERS 0
#endif

#if NUM_AMP_TCP_CONTROLLERS > 1
#error NUM_AMP_TCP_CONTROLLERS cannot be > 1
#endif

/*---------------------------------------------------------------------------
 * NUM_AMPS constant
 *
 *     Defines the number of AMP Controllers supported by the device.
 *     When NUM_AMPS is set to 0, AMP support is disabled. NUM_AMPS
 *     is derived from the number of controllers.
 *
 *     The default value is 0 (AMP controllers are not supported).
 */
#define NUM_AMPS (NUM_AMP_802_CONTROLLERS + NUM_AMP_UDP_CONTROLLERS + \
    NUM_AMP_TCP_CONTROLLERS + NUM_AMP_HCI_CONTROLLERS)

/*---------------------------------------------------------------------------
 * NUM_AMP_LOGICAL_LINKS constant
 *
 *     Defines the number of AMP Guaranteed Logical links available for all 
 *     AMPs. There is one Best Effort Logical per physical link and the
 *     PHYLINK structure has a logical link for this purpose.
 *
 *     The default is the same as the number of AMPS - one Logical Link
 *     per AMP.
 */
#ifndef NUM_AMP_LOGICAL_LINKS
#define NUM_AMP_LOGICAL_LINKS NUM_AMPS
#endif

/*---------------------------------------------------------------------------
 * NUM_LE_DEVICES constant
 *
 *     The number of LE devices with which we can connect. This value
 *     represents the maximum size of the LE piconet in which this device is
 *     the master. It also controls if LE code compiled and LE memory allocated.
 *
 *     The default value is 0 (LE is not supported).
 */
#ifndef NUM_LE_DEVICES
#define NUM_LE_DEVICES  0
#endif

/*---------------------------------------------------------------------------
 * NUM_DDB_RECORDS constant
 *
 *     The number of records in the Device Database (DDB). The size of the 
 *     DDB database must include both the head record and AMP records.  
 *
 *     The default value is 20.
 */
#ifndef NUM_DDB_RECORDS
#define NUM_DDB_RECORDS  20
#endif

/* The number DDB records must be at least the number of BR/EDR and LE devices */
#if NUM_DDB_RECORDS < (NUM_BT_DEVICES + NUM_LE_DEVICES)
#error NUM_DDB_RECORDS must be at least NUM_BT_DEVICES + NUM_LE_DEVICES
#endif

/****************************************************************************
 *
 * Section: L2CAP Channel Configuation Constants
 *
 *     L2CAP_NUM_CHANNELS and L2CAP_NUM_ENHANCED_CHANNELS must be
 *     configured early to prevent preprocessor errors.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * L2CAP_PING_SUPPORT constant
 *
 *     Controls whether the Ping operation (L2CAP_Ping) is supported.
 *     When disabled, the L2CAP_Ping() API is not included in the stack,
 *     resulting in code size savings.
 *
 *     By default, Ping support is enabled.
 */
#ifndef L2CAP_PING_SUPPORT
#define L2CAP_PING_SUPPORT          XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * L2CAP_GET_INFO_SUPPORT constant
 *
 *     Controls whether the GetInfo operation (L2CAP_GetInfo) is supported.
 *     When disabled, the L2CAP_GetInfo() API is not included in the stack,
 *     resulting in code size savings.
 *
 *     By default, GetInfo support is disabled.
 */
#ifndef L2CAP_GET_INFO_SUPPORT
#define L2CAP_GET_INFO_SUPPORT      XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * L2CAP_NUM_CHANNELS constant
 *
 *     Defines the number of L2CAP Channels, based on number of protocols and
 *     devices. This constant must be large enough to support all the channels
 *     that are needed by the upper layer protocols. There is typically one
 *     channel per protocol per supported device.
 */
#ifndef L2CAP_NUM_CHANNELS
/*
 * First calculate the number of L2CAP Auxiliary Channels. Auxiliary channels
 * are used to support the Echo (Ping) and Info (GetInfo) signaling procedures.
 */
#if (L2CAP_PING_SUPPORT == XA_ENABLED) && (L2CAP_GET_INFO_SUPPORT == XA_ENABLED)
#define AUX_CHANNELS    2
#elif (L2CAP_PING_SUPPORT == XA_ENABLED) || (L2CAP_GET_INFO_SUPPORT == XA_ENABLED)
#define AUX_CHANNELS    1
#else
#define AUX_CHANNELS    0
#endif

#define L2CAP_NUM_CHANNELS          ((L2CAP_NUM_PROTOCOLS * NUM_BT_DEVICES) + AUX_CHANNELS)
#endif

#if L2CAP_NUM_CHANNELS < 1
#error L2CAP_NUM_CHANNELS must be at least 1
#endif

#if (NUM_AMPS > 0) && (L2CAP_NUM_ENHANCED_CHANNELS == 0) 
#error L2CAP_NUM_ENHANCED_CHANNELS must be greater than 0 if NUM_AMPS is greater than 0
#endif

/*---------------------------------------------------------------------------
 * L2CAP_NUM_ENHANCED_CHANNELS constant
 *
 *     Configures the number of L2CAP Enhanced Channels.
 *
 *     By default this option is set to L2CAP_NUM_CHANNELS; since enhanced
 *     channels are required for AMP support.
 */
#ifndef L2CAP_NUM_ENHANCED_CHANNELS
#define L2CAP_NUM_ENHANCED_CHANNELS     L2CAP_NUM_CHANNELS
#endif

#if L2CAP_NUM_ENHANCED_CHANNELS > L2CAP_NUM_CHANNELS
#error L2CAP_NUM_ENHANCED_CHANNELS must not be greater than L2CAP_NUM_CHANNELS
#endif

/****************************************************************************
 *
 * Section: LE L2CAP Configuration Constants
 *
 *     LEL2CAP_TOT_NUM_DYNAMIC_CHNLS must be configured early to prevent 
 *     preprocessor errors.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * LEL2CAP_NUM_PROTOCOLS constant
 *
 *     Defines the number of protocol services/applications that may be 
 *     layered above LEL2CAP.
 *
 */
#ifndef LEL2CAP_NUM_PROTOCOLS
#define LEL2CAP_NUM_PROTOCOLS         5
#endif

/*---------------------------------------------------------------------------
 * LEL2CAP_TOT_NUM_DYNAMIC_CHNLS constant
 *
 *    Defines the total number of Dynamic channels available. This must account
 *    for the number of dynamic channels per LE remote device. In real systems
 *    it is unlikely that all protocol services/applications available will run
 *    on all connected devices simultaneously therefore, the number of
 *    Dynamic channels can be reduced to a value that is more reasonable.
 *
 *    The default is the number of protocols multiplied by the number of LE 
 *    devices.
 */
#if NUM_LE_DEVICES > 0
#ifndef LEL2CAP_TOT_NUM_DYNAMIC_CHNLS
/* Need dynamic channel memory for each device. */
#define LEL2CAP_TOT_NUM_DYNAMIC_CHNLS   (LEL2CAP_NUM_PROTOCOLS * NUM_LE_DEVICES)
#endif

#if (LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0) && (LEL2CAP_NUM_PROTOCOLS == 0)
#error LEL2CAP_NUM_PROTOCOLS must be greater than 0 if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS is greater than 0
#endif
#else
#undef LEL2CAP_TOT_NUM_DYNAMIC_CHNLS
#define LEL2CAP_TOT_NUM_DYNAMIC_CHNLS   0
#endif

/****************************************************************************
 *
 * Section: Management Entity Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BT_SCO_HCI_DATA constant
 *
 *     Determines whether code to send and receive SCO is included. This
 *     constant should be defined only when SCO audio data is sent through
 *     the HCI interface. SCO connections are always supported when
 *     NUM_SCO_CONNS is greater than zero.
 *
 *     By default, this constant is disabled.
 */
#ifndef BT_SCO_HCI_DATA
#define BT_SCO_HCI_DATA XA_DISABLED
#endif

#if BT_SCO_HCI_DATA == XA_ENABLED && NUM_SCO_CONNS == 0
#error NUM_SCO_CONNS must be > 0 when BT_SCO_HCI_DATA is enabled
#endif

/*---------------------------------------------------------------------------
 * BT_SCO_HCI_NUM_PACKETS constant
 *
 *     Represents the number of HCI packets allocated for sending SCO data to
 *     the Host Controller. These packets are used for SCO data only. If the
 *     HCI layer runs out of SCO packets, no more data will be sent until the
 *     transport layer returns one.
 *
 *     The default number of packets is 20 for each potential SCO connection.
 */
#ifndef BT_SCO_HCI_NUM_PACKETS
#define BT_SCO_HCI_NUM_PACKETS  (20 * NUM_SCO_CONNS)
#endif

/*---------------------------------------------------------------------------
 * BT_SCO_USE_LEGACY_CONNECT constant
 *
 *     Determines whether code to create SCO connections uses the new
 *     HCC_SETUP_SYNC_CONNECTION command or the legacy HCC_ADD_SCO_CONNECTION
 *     command when SCO connections are created.  Some radios have difficulty
 *     negotiating a SCO connection using the HCC_SETUP_SYNC_CONNECTION command.
 *     Enabling this option will force the stack to use the legacy command for
 *     creating a SCO connection.  Certain versions of the CSR firmware have
 *     this problem.
 *
 *     By default, this constant is disabled.
 */
#ifndef BT_SCO_USE_LEGACY_CONNECT
#define BT_SCO_USE_LEGACY_CONNECT XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * BT_SECURITY constant
 *
 *     Determines whether code to handle BT security features is included.
 *     This constant cannot be changed in this version of the stack. It is
 *     here to allow profiles written for other versions of the stack to use
 *     the constant. It always enabled in this version of the stack.
 */
#ifdef BT_SECURITY
#if BT_SECURITY != XA_ENABLED
#error BT_SECURITY must be set to XA_ENABLED
#endif
#undef BT_SECURITY
#endif

#define BT_SECURITY XA_ENABLED

/*---------------------------------------------------------------------------
 * BT_SECURITY_TIMEOUT constant
 *
 *     Specifies the number of seconds to elapse before a service-specific
 *     access request (see SEC_AccessRequest) is automatically cancelled.
 *     The stack's security manager will cancel the request regardless of
 *     what is causing the delay. Cancelling the request will cause the
 *     security request to fail, which in most cases will prevent the
 *     requesting service from connecting.
 *
 *     This value does not affect authentication or encryption operations
 *     requested directly through APIs such as SEC_AuthenticateLink or
 *     SEC_SetLinkEncryption; it only affects operations initiated through
 *     SEC_AccessRequest.
 *
 *     Set this value to 0 to disable security timeouts. When disabled, it
 *     may be necessary to drop the ACL connection in order to safely cancel
 *     the security request.
 *
 *     By default, this value is set to 80 seconds. Although any time value
 *     may be used, sufficient time should be allowed for the user of both
 *     the local and remote devices to enter PIN codes and select
 *     authorization settings if required.
 */
#ifndef BT_SECURITY_TIMEOUT
#define BT_SECURITY_TIMEOUT 80
#endif /* BT_SECURITY_TIMEOUT */

/*---------------------------------------------------------------------------
 * BT_SECURITY_CHECK_COMBO_KEY constant
 *
 *     If set to XA_ENABLED then the function
 *     SecIsCurrentAuthenticationGoodEnough() checks for the situation where
 *     a remote device indicates support for Secure Simple Pairing (SSP) but
 *     has paired using legacy pairing resulting in the creation of a
 *     Combination key.  This can occur if the remote device has enabled
 *     Security mode 3 (in violation of the Bluetooth Core Specification). If
 *     this situation occurs then the device is treated as a legacy device.
 *
 *     There may be cases where this could cause problems such as if the
 *     remote device decides it wants to later upgrade the key using SSP.
 *     Therefore, the default is set to XA_DISABLED.
 */
#ifndef BT_SECURITY_CHECK_COMBO_KEY
#define BT_SECURITY_CHECK_COMBO_KEY  XA_DISABLED
#endif /* BT_SECURITY_CHECK_COMBO_KEY */

/*---------------------------------------------------------------------------
 * BT_ALLOW_SCAN_WHILE_CON constant
 *
 *     Determines whether page and inquiry scans are allowed during a
 *     connection. Performing Page and Inquiry scans while connected takes
 *     bandwidth away from the connections, so it should only be used in
 *     circumstances where scatternets or master/slave switching is required
 *     (for instance, when implementing a LAN access port).
 *
 *     When BT_ALLOW_SCAN_WHILE_CON is XA_ENABLED, the Management Entity
 *     includes code for page and inquiry scans during a connection. When
 *     it is XA_DISABLED, scans are not allowed during a connection.
 *
 *     By default, this constant is disabled.
 */
#ifndef BT_ALLOW_SCAN_WHILE_CON
#define BT_ALLOW_SCAN_WHILE_CON XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * BT_ALLOW_SETTING_SCAN_PARMS constant
 *
 *    Specifies whether to allow the host to set the Inquiry and Page scan
 *    parameters via the ME_SetAccessibleModeNC() and ME_SetAccessibleModeC()
 *    functions, which in turn generate the Write_Scan_Activity HCI command
 *    before setting the accessibilty mode.
 *
 *    When enabled the scan parameters may be sent to the controller. When
 *    disabled the scan parameters are never sent to the controller.
 *
 *    By default this value is disabled. When enabling you will want to also
 *    determine appropriate default scan parameter values with the following
 *    defines: BT_DEFAULT_PAGE_SCAN_WINDOW, BT_DEFAULT_PAGE_SCAN_INTERVAL,
 *    BT_DEFAULT_INQ_SCAN_WINDOW, BT_DEFAULT_INQ_SCAN_INTERVAL.
 */
#ifndef BT_ALLOW_SETTING_SCAN_PARMS
#define BT_ALLOW_SETTING_SCAN_PARMS XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * BT_DEFAULT_PAGE_SCAN_WINDOW constant
 *
 *     The following are used as the default values for the host to set on
 *     the controller via the Write_Scan_Activity command. The values for
 *     these settings must be valid as defined by the Bluetooth Core
 *     Specification. They may also be set to zero to indicate that the
 *     host should not set the default values on the controller.
 *
 *     By default the values are set to zero to indicate that that the host
 *     will not set the values and simply allow the controller to use the
 *     default configuration.
 *
 *     In order to set these values to non-zero values,
 *     BT_ALLOW_SETTING_SCAN_PARMS must be set to XA_ENABLED.
 */
#ifndef BT_DEFAULT_PAGE_SCAN_WINDOW
#define BT_DEFAULT_PAGE_SCAN_WINDOW 0
#endif

/*---------------------------------------------------------------------------
 * BT_DEFAULT_PAGE_SCAN_INTERVAL constant
 *
 *     See BT_DEFAULT_PAGE_SCAN_WINDOW.
 */
#ifndef BT_DEFAULT_PAGE_SCAN_INTERVAL
#define BT_DEFAULT_PAGE_SCAN_INTERVAL 0
#endif

/*---------------------------------------------------------------------------
 * BT_DEFAULT_INQ_SCAN_WINDOW constant
 *
 *     See BT_DEFAULT_PAGE_SCAN_WINDOW.
 */
#ifndef BT_DEFAULT_INQ_SCAN_WINDOW
#define BT_DEFAULT_INQ_SCAN_WINDOW 0
#endif

/*---------------------------------------------------------------------------
 * BT_DEFAULT_INQ_SCAN_INTERVAL constant
 *
 *     See BT_DEFAULT_PAGE_SCAN_WINDOW.
 */
#ifndef BT_DEFAULT_INQ_SCAN_INTERVAL
#define BT_DEFAULT_INQ_SCAN_INTERVAL 0
#endif

/* Verify that the values are in the legal range. Note that these macros
 * cannot be broken over more than one line or some compilers break (gcc) */
#define BT_PSW_TEST ((BT_DEFAULT_PAGE_SCAN_WINDOW != 0) && ((BT_DEFAULT_PAGE_SCAN_WINDOW < 0x0011) || (BT_DEFAULT_PAGE_SCAN_WINDOW > 0x1000)))

#if BT_PSW_TEST
#error BT_DEFAULT_PAGE_SCAN_WINDOW is not legal
#endif

#define BT_PSI_TEST ((BT_DEFAULT_PAGE_SCAN_INTERVAL != 0) && ((BT_DEFAULT_PAGE_SCAN_INTERVAL < 0x0012) || (BT_DEFAULT_PAGE_SCAN_INTERVAL > 0x1000)))

#if BT_PSI_TEST
#error BT_DEFAULT_PAGE_SCAN_INTERVAL is not legal
#endif

#define BT_ISW_TEST ((BT_DEFAULT_INQ_SCAN_WINDOW != 0) && ((BT_DEFAULT_INQ_SCAN_WINDOW < 0x0011) || (BT_DEFAULT_INQ_SCAN_WINDOW > 0x1000)))

#if BT_ISW_TEST
#error BT_DEFAULT_INQ_SCAN_WINDOW is not legal
#endif

#define BT_ISI_TEST ((BT_DEFAULT_INQ_SCAN_INTERVAL != 0) && ((BT_DEFAULT_INQ_SCAN_INTERVAL < 0x0012) || (BT_DEFAULT_INQ_SCAN_INTERVAL > 0x1000)))

#if BT_ISI_TEST
#error BT_DEFAULT_INQ_SCAN_INTERVAL is not legal
#endif

/*---------------------------------------------------------------------------
 * BT_DEFAULT_ACCESS_MODE_NC constant
 *
 *     Specifies the default accessibility mode when not-connected that
 *     the ME should set the radio to during initialization. This value
 *     must be of type BtAccessibleMode, found in me.h.
 */
#ifndef BT_DEFAULT_ACCESS_MODE_NC
#define BT_DEFAULT_ACCESS_MODE_NC  BAM_GENERAL_ACCESSIBLE
#endif /* BT_DEFAULT_ACCESS_MODE_NC */

/*---------------------------------------------------------------------------
 * BT_DEFAULT_ACCESS_MODE_C constant
 *
 *     Specifies the default accessibility mode when connected that the
 *     ME should set the radio to during initialization. This value
 *     must be of type BtAccessibleMode, found in me.h.
 *
 *     This default is applied only when BT_ALLOW_SCAN_WHILE_CON is XA_ENABLED.
 */
#ifndef BT_DEFAULT_ACCESS_MODE_C
#define BT_DEFAULT_ACCESS_MODE_C  BAM_NOT_ACCESSIBLE
#endif /* BT_DEFAULT_ACCESS_MODE_C */

/*---------------------------------------------------------------------------
 * BT_HCI_NUM_INIT_RETRIES constant
 *
 *     Specifies the number of times the Radio Manager should retry the
 *     initialization of HCI when it fails to initialize.
 */
#ifndef BT_HCI_NUM_INIT_RETRIES
#define BT_HCI_NUM_INIT_RETRIES 0xFF
#endif

/*---------------------------------------------------------------------------
 * BT_HCI_RETRY_DELAY constant
 *
 *     Specifies the amount of time to delay before retrying initialization
 *     after an initialization failure.
 */
#ifndef BT_HCI_RETRY_DELAY
#define BT_HCI_RETRY_DELAY 5000
#endif

/*---------------------------------------------------------------------------
 * BT_DEFAULT_PAGE_TIMEOUT constant
 *
 *     The BT_DEFAULT_PAGE_TIMEOUT constant is the maximum time the
 *     local Link Manager will wait for a response from the remote
 *     device on a connection attempt. The default is 0x2000 = 5.12 sec.
 */
#ifndef BT_DEFAULT_PAGE_TIMEOUT
#define BT_DEFAULT_PAGE_TIMEOUT 0x2000
#endif

/*---------------------------------------------------------------------------
 * BT_PACKET_HEADER_LEN constant
 *
 *     The BT_PACKET_HEADER_LEN constant is the size of the BtPacket
 *     header.  The default size is the max(L2CAP) header size (8 bytes),
 *     plus the max(RFCOMM,SDP,TCS) header size (6 bytes).  If BNEP is used,
 *     you must increase the BT_PACKET_HEADER_LEN by 5 bytes: (8+6+5) in
 *     your configuration override file.  With Bluetooth HS, L2CAP needs a
 *     minimum of 16 bytes to handle the Fixed_Channels_Supported Info
 *     Response.  L2CAP needs an additional 2 bytes to support the
 *     Extended_Window_Size option.
 */
#ifndef BT_PACKET_HEADER_LEN
#define BT_PACKET_HEADER_LEN (16+2)
#endif

#if BT_PACKET_HEADER_LEN < 16 || BT_PACKET_HEADER_LEN > 255
#error BT_PACKET_HEADER_LEN must be greater than 15 and less than 256
#endif


/* Pre-set BT_PRESCAN if HCI_ALLOW_PRESCAN is set */
#ifdef HCI_ALLOW_PRESCAN
#ifndef BT_PRESCAN
#define BT_PRESCAN HCI_ALLOW_PRESCAN
#endif
#endif

/*---------------------------------------------------------------------------
 * BT_PRESCAN constant
 *
 *     (Was HCI_ALLOW_PRESCAN) Controls whether prescan support is built
 *     into the SDK. When enabled, prescan "producers" such as HCI, AMP, and
 *     others expose the data they send and receive. This data can then be
 *     inspected, displayed, and/or modified by prescan "consumers".
 *
 *     By default, BT_PRESCAN is enabled. Note that if HCI_ALLOW_PRESCAN
 *     is defined, BT_PRESCAN will be set to the same configuration.
 */
#ifndef BT_PRESCAN
#define BT_PRESCAN XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * BT_SINGLE_HW_INTERRUPT constant
 *
 *     If enabled, this define forces all transports to use a single
 *     semaphore or hardware interrupt to synchronize access to data
 *     structures accessed by the HCI or AMP controllers (see
 *     OS_StopHardware()).
 *
 *     If disabled, then the HCI and AMP controller code may each use their
 *     own synchronization methods.
 *
 *     By default, BT_SINGLE_HW_INTERRUPT is disabled.
 */
#define BT_SINGLE_HW_INTERRUPT XA_DISABLED

/*---------------------------------------------------------------------------
 * NUM_KNOWN_DEVICES constant
 *
 *     Defines the maximum number of devices that the ME Device Selection
 *     manager can track. If this value is zero, the MEDEV component is
 *     disabled, resulting in a code size savings. If LE is used this
 *     value cannot be set to 0. If the value is greater than 0 then it must
 *     be at least (NUM_DDB_RECORDS + NUM_BT_DEVICES + NUM_LE_DEVICES). 
 *
 *     The default value is NUM_DDB_RECORDS + NUM_BT_DEVICES + NUM_LE_DEVICES
 */
#ifndef NUM_KNOWN_DEVICES
#define NUM_KNOWN_DEVICES  (NUM_DDB_RECORDS + NUM_BT_DEVICES + NUM_LE_DEVICES)
#endif

#if NUM_LE_DEVICES > 0
#if NUM_KNOWN_DEVICES == 0
#error NUM_KNOWN_DEVICES must be > 0 if LE is enabled
#endif
#endif

#if NUM_KNOWN_DEVICES > 0
#if NUM_KNOWN_DEVICES < (NUM_DDB_RECORDS + NUM_BT_DEVICES + NUM_LE_DEVICES)
#error NUM_KNOWN_DEVICES must be at least (NUM_DDB_RECORDS + NUM_BT_DEVICES + NUM_LE_DEVICES)
#endif
#endif

/*---------------------------------------------------------------------------
 * MIN_EIR_NAME_LEN constant
 *
 *     Defines the minimum number of bytes to be reserved for the device
 *     name when using ME_AutoCreateExtInquiryRsp. If non-0, the local device
 *     name may be truncated to this size to allow sufficient room for other
 *     EIR elements to be written. If 0, no space is reserved for the local
 *     device name, and other elements may crowd it out.
 */
#ifndef MIN_EIR_NAME_LEN
#define MIN_EIR_NAME_LEN       60
#endif


/*---------------------------------------------------------------------------
 * DS_NUM_SERVICES constant
 *
 *     This constant represents the maximum number of services that can
 *     be tracked within a BtDeviceContext. Each time a successful service
 *     search attribute request is performed, the UUID of the found service
 *     is stored in the device context. This list of services is used for
 *     selecting the best device for a specific service type. If this
 *     constant is defined to 0, service tracking is disabled and the
 *     supporting code is eliminated from the build.
 */
#ifndef DS_NUM_SERVICES
#define DS_NUM_SERVICES         4
#endif

/*---------------------------------------------------------------------------
 * EVM_PROCESS_MAX_ITERATIONS constant
 *
 *     Specifies the maximum number of tasks that the Event Manager will
 *     execute each time it runs.  Increasing this number may increase the
 *     latency when EVM_Process() is called, but may also cause less context
 *     switches.  Decreasing this value will have the opposite effect. The
 *     Event Manager will execute tasks until there is no more work to do or
 *     until this limit is reached.
 *
 *     The default is 5 iterations, and this value must be at least 2.
 */
#ifndef EVM_PROCESS_MAX_ITERATIONS
#define EVM_PROCESS_MAX_ITERATIONS 5
#endif

/*---------------------------------------------------------------------------
 * EVM_MAX_GUARANTEED_ITERATIONS
 *
 *     Specifies the number of EVM_Process() iterations that are dedicated to
 *     tasks that run at the guaranteed (BT_PRIORITY_GUARANTEED) priority.
 *     Tasks of the signal (BT_PRIORITY_SIGNAL) level are always run before
 *     guaranteed tasks, however, guaranteed tasks are guaranteed to run each
 *     time EVM_Process() is called.  This value controls the maximum ratio
 *     of signal level tasks to guaranteed tasks that will run during a
 *     call to EVM_Process().
 *
 *     The default value of 5 for EVM_PROCESS_MAX_ITERATIONS and 1 for
 *     EVM_MAX_GUARANTEED_ITERATIONS yields a ratio of 4:1, meaning up to
 *     four signaling tasks may run before any guaranteed tasks are allowed
 *     to run.
 *
 *     Best Effort tasks are executed only when there are no signaling or
 *     guaranteed tasks scheduled and the scheduler has completed less than
 *     EVM_PROCESS_MAX_ITERATIONS iterations.
 */
#ifndef EVM_MAX_GUARANTEED_ITERATIONS
#define EVM_MAX_GUARANTEED_ITERATIONS 1
#endif

#if EVM_PROCESS_MAX_ITERATIONS < EVM_MAX_GUARANTEED_ITERATIONS
#error EVM_PROCESS_MAX_ITERATIONS must be greater than EVM_MAX_GUARANTEED_ITERATIONS
#endif

#if EVM_MAX_GUARANTEED_ITERATIONS < 1
#error EVM_MAX_GUARANTEED_ITERATIONS must be greater than 0
#endif

/*--------------------------------------------------------------------------- 
 * BT_FEATURE_RETRIES constant
 * 
 *     Specifies the number of times that ME will attempt to retrieve the
 *     remote features and/or extended remote features during a connection
 *     attempt.  If the ME fails to get the remote features after all
 *     retires are exhausted, then the feature masks will be set to 0.
 * 
 *     The default value is 3.
 */
#ifndef BT_FEATURE_RETRIES
#define BT_FEATURE_RETRIES 3
#endif

/****************************************************************************
 *
 * Section: HCI Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HCI_MAX_PROCESS_ITERATIONS constant
 *
 *     Specifies the maximum number of iterations that the HCI tasks will
 *     perform when there is work to do.  Increasing this number may increase
 *     the latency when EVM_Process() is called, but may also cause less
 *     context switches.  Decreasing this value will have the opposite
 *     effect.  The HCI process will run until there is no more work to do or
 *     until this limit is reached.
 *
 *     The default is 5 iterations.
 */
#ifndef HCI_MAX_PROCESS_ITERATIONS
#define HCI_MAX_PROCESS_ITERATIONS 5
#endif

/*---------------------------------------------------------------------------
 * HCI_RESET_TIMEOUT constant
 *
 *     Specifies the amount of time that the HCI will wait for a response
 *     to the RESET command before indicating an error to the Radio Manager.
 *     When the Radio Manager detects the error, it will attempt to reset
 *     the transports and the HCI again.
 *
 *     The default is 10 seconds.
 */
#ifndef HCI_RESET_TIMEOUT
#define HCI_RESET_TIMEOUT 10000
#endif

/*---------------------------------------------------------------------------
 * HCI_NUM_PACKETS constant
 *
 *     Represents the number of HCI packets allocated for sending ACL data to
 *     the Host Controller.  If the HCI layer runs out of packets, no more data
 *     will be sent until the transport layer returns one.
 *
 *     The default number of packets for ACL data is 2 for each potential
 *     connection.  See HCI_NUM_COMMANDS to define the number packets
 *     allocated for commands, and BT_SCO_HCI_NUM_PACKETS to define the number
 *     of packets allocated for SCO data.
 */
#ifndef HCI_NUM_PACKETS
#define HCI_NUM_PACKETS  (2 * NUM_BT_DEVICES)
#endif

#if HCI_NUM_PACKETS < (2 * NUM_BT_DEVICES)
#error HCI_NUM_PACKETS should be greater than or equal to (2 * NUM_BT_DEVICES)
#endif


/*---------------------------------------------------------------------------
 * HCI_NUM_COMMANDS constant
 *
 *     Represents the number of HCI packets allocated for sending commands
 *     to the host controller.
 *
 *     The default number of packets is 1.
 */
#ifndef HCI_NUM_COMMANDS
#define HCI_NUM_COMMANDS 1
#endif


/*---------------------------------------------------------------------------
 * HCI_NUM_EVENTS constant
 *
 *     Represents the number of HCI buffers allocated for handling events
 *     from the host controller.
 *
 *     The default number of packets is 2.
 */
#ifndef HCI_NUM_EVENTS
#define HCI_NUM_EVENTS 2
#endif


/*---------------------------------------------------------------------------
 * HCI_CMD_PARM_LEN constant
 *
 *     Represents the largest size allowed for HCI command parameters.
 *
 *     The this value must be 255.
 */
#ifndef HCI_CMD_PARM_LEN
#define HCI_CMD_PARM_LEN 255
#endif

/* The longest known HCI command is 255 bytes at this time.
 */
#if HCI_CMD_PARM_LEN < 255
#error HCI_CMD_PARM_LEN must be at least 255 to work with all known HCI commands.
#endif


/*---------------------------------------------------------------------------
 * HCI_HOST_FLOW_CONTROL constant
 *
 *     Controls whether HCI applies flow control to data received from the
 *     host controller.
 *
 *     When HCI_HOST_FLOW_CONTROL is enabled, the HCI layer uses
 *     HCC_HOST_NUM_COMPLETED_PACKET commands to periodically tell the
 *     radio hardware how many receive packets it can accept. This is
 *     necessary if the HCI driver's receive buffers could overflow
 *     with incoming data.
 *
 *     When HCI_HOST_FLOW_CONTROL is disabled, the HCI driver assumes
 *     that it can process data faster than the radio hardware can generate
 *     it.
 *
 *     By default, host-side flow control is disabled.
 */
#ifndef HCI_HOST_FLOW_CONTROL
#define HCI_HOST_FLOW_CONTROL XA_DISABLED
#endif

/*-------------------------------------------------------------------------
 * HCI_MAX_COMPLETED_PKTS constant
 *
 *     Defines the maximum number of receive buffers that will be used
 *     before an HCC_HOST_NUM_COMPLETED_PACKET command is sent to the
 *     controller.
 *
 *     By default, the maximum is 8 completed packets.
 */
#ifndef HCI_MAX_COMPLETED_PKTS
#if HCI_HOST_FLOW_CONTROL == XA_ENABLED
#define HCI_MAX_COMPLETED_PKTS 8
#endif
#endif

/*---------------------------------------------------------------------------
 * HCI_SCO_FLOW_CONTROL constant
 *
 *     Controls whether the host controller applies flow control to SCO data
 *     sent by the host.
 *
 *     When HCI_SCO_FLOW_CONTROL is enabled, flow control is applied to SCO
 *     data transmitted to the host controller.
 *
 *     When HCI_SCO_FLOW_CONTROL is disabled, no flow control mechanism is
 *     used when transmitting data to the host controller.
 *
 *     By default, SCO flow control is disabled.
 */
#ifndef HCI_SCO_FLOW_CONTROL
#define HCI_SCO_FLOW_CONTROL XA_DISABLED
#endif

/*-------------------------------------------------------------------------
 * HCI_NUM_ACL_TX_RESERVE constant
 *
 *     Defines the number of TX packets to hold in reserve to prevent
 *     resource depletion by any one HCI channel. A single ACL channel
 *     may use all but HCI_NUM_ACL_TX_RESERVE packets from the TX
 *     packet pool. Use caution when setting this value; as there
 *     must be sufficient transmit packets allocated by HCI_NUM_PACKETS.
 *
 *     By default, 1 ACL TX packet is reserved.
 *     Set this to 0 to disable this option.
 */
#ifndef HCI_NUM_ACL_TX_RESERVE
#define HCI_NUM_ACL_TX_RESERVE  1
#endif

/*-------------------------------------------------------------------------
 * HCI_ACL_BUFF_HEADER_OVERHEAD constant
 *
 *     Defines the number of bytes taken up by HCI and L2CAP headers that 
 *     must be accounted for when defining HCI_ACL_DATA_SIZE and other
 *     constants based on HCI_ACL_DATA_SIZE. 
 */
#if L2CAP_NUM_ENHANCED_CHANNELS > 0
/* This includes HCI header(4) + Basic L2CAP Header(4) +
 * ERTM Extended control(4) + ERTM SAR(2) + FCS(2)
 */
#define HCI_ACL_BUFF_HEADER_OVERHEAD 16
#else
/* This includes HCI header(4) + Basic L2CAP Header (4) +
 * Connectionless PSM (2) 
 */
#define HCI_ACL_BUFF_HEADER_OVERHEAD 10
#endif


/*-------------------------------------------------------------------------
 * HCI_ACL_DATA_SIZE constant
 *
 *     Defines the maximum data size of a received ACL packet in the HCI
 *     layer for BR/EDR and LE. This value affects the size of receive buffers 
 *     allocated by the HCI driver.
 *
 *     This value must be at least 670 + HCI_ACL_BUFF_HEADER_OVERHEAD if 
 *     L2CAP_NUM_EXTENDED_CHANNELS is greater than 1 or 255 otherwise and 
 *     no more than 65531. 
 *     
 *     The default is 1021 bytes (the typical size of a 3-DH5 packet 
 *     payload). This default value was chosen to maximize throughput at the 
 *     baseband. If your priority is to minimize memory use at the host, you 
 *     may want to adjust the HCI_ACL_DATA_SIZE to more closely match the
 *     maximum size indicated in receive buffers from the controller.
 *     Some controllers indicate the full 1021 bytes when available from
 *     a 3-DH5 packet payload; so no adjustment would be necessary. Others
 *     indicate 512 bytes or 343 bytes at a time. If memory is a concern,
 *     determine the max size indicated from your controller and adjust
 *     HCI_ACL_DATA_SIZE as appropriate for your controller.
 *
 */
#ifndef HCI_ACL_DATA_SIZE
#define HCI_ACL_DATA_SIZE 1021
#endif

#ifdef L2CAP_MTU
/* L2CAP_MTU is defined in an override file. In older versions of the stack this was
 * allowed but in this version of the stack L2CAP_MTU is based on HCI_ACL_DATA_SIZE 
 * so we will use it to define HCI_ACL_DATA_SIZE
 */
#if (L2CAP_MTU + HCI_ACL_BUFF_HEADER_OVERHEAD) > HCI_ACL_DATA_SIZE
#undef HCI_ACL_DATA_SIZE
#define HCI_ACL_DATA_SIZE (L2CAP_MTU + HCI_ACL_BUFF_HEADER_OVERHEAD)
#endif
#endif /* #ifdef L2CAP_MTU */

#if HCI_ACL_DATA_SIZE > 65531
#error HCI_ACL_DATA_SIZE cannot be greater than 65531
#endif

#if L2CAP_NUM_ENHANCED_CHANNELS > 0
#if HCI_ACL_DATA_SIZE < (670 + HCI_ACL_BUFF_HEADER_OVERHEAD)
#error HCI_ACL_DATA_SIZE cannot be less than (670 + HCI_ACL_BUFF_HEADER_OVERHEAD)
#endif
#else
#if HCI_ACL_DATA_SIZE < 255
#error HCI_ACL_DATA_SIZE cannot be less than 255
#endif
#endif

/*-------------------------------------------------------------------------
 * MAX_AMP_DATA_SIZE constant
 *
 *     Defines the maximum data size of a received AMP packet at the AMP
 *     transport. The maximum PDU for an 802.11 PAL as defined in the
 *     Bluetooth Specification is 1492 so this is the default.
 */
#ifndef MAX_AMP_DATA_SIZE
#if NUM_AMPS > 0
#define MAX_AMP_DATA_SIZE 1492
#else
#define MAX_AMP_DATA_SIZE 0
#endif
#endif

/*-------------------------------------------------------------------------
 * HCI_SCO_DATA_SIZE constant
 *
 *     Defines the maximum data size of a received SCO packet in the HCI
 *     layer. This value affects the size of receive buffers allocated by
 *     the HCI driver.
 *
 *     The SCO data size must be less than 256 bytes. The default size is
 *     255 bytes.
 */
#ifndef HCI_SCO_DATA_SIZE
#define HCI_SCO_DATA_SIZE 255
#endif

#if HCI_SCO_DATA_SIZE > 255
#error HCI_SCO_DATA_SIZE must be less than 256
#endif

/*-------------------------------------------------------------------------
 * HCI_ACL_ENHANCED_MULTI constant
 *
 *     Defines a multiplier that is used when allocating HCI buffers for
 *     Enhanced L2CAP channels with fragmentation enabled. L2CAP enhanced 
 *     channel PDUs are negotiated to fit within one HCI RX buffer but that 
 *     does not mean that PDUs will be sent up by the controller in one 
 *     HCI ACL buffer. For many reasons a PDU may be sent in fragments 
 *     (multiple HCI buffers). 
 *
 *     HCI_ACL_ENHANCED_MULTI specifies how many HCI RX buffers are allocated
 *     per PDU. L2CAP will begin copying data from one HCI RX buffer to another
 *     when the HCI_ACL_ENHANCED_MULTI number of HCI RX buffers has been
 *     allocated for a PDU. In this way L2CAP will make sure only 
 *     HCI_ACL_ENHANCED_MULTI HCI buffers per PDU is maintained when PDUs
 *     are being held waiting for local busy to clear.
 *
 *     If flowParms.fragmentsEnabled is FALSE then the muliplier is 1 since
 *     the whole SDU must be delivered as one contiguous buffer and since 
 *     it must fit in one HCI RX buffer it will be copied anyway.
 *
 *     It is more efficient for the stack if it can avoid copying data
 *     until it is absolutely necessary so larger values for 
 *     HCI_ACL_ENHANCED_MULTI will allow L2CAP to be more efficient and
 *     reduce copying but with a tradeoff of using more memory (RAM).
 *
 *     The default for HCI_ACL_ENHANCED_MULTI is 3 which is what was
 *     used in earlier versions of the stack.
 */
#ifndef HCI_ACL_ENHANCED_MULTI
#define HCI_ACL_ENHANCED_MULTI 3
#endif

#if HCI_ACL_ENHANCED_MULTI < 1 || HCI_ACL_ENHANCED_MULTI > 255
#error HCI_ACL_ENHANCED_MULTI must be at least 1 and less than 256
#endif

/*-------------------------------------------------------------------------
 * HCI_NUM_ACL_BUFFERS_ENHANCED constant
 *
 *     Defines the number of buffers allocated by the HCI layer
 *     for receiving ACL data destined for L2CAP enhanced channels with 
 *     fragmentation enabled. It is the total number of HCI buffers that 
 *     can be reserved by the L2CAP layer for Enhanced Channels using 
 *     RXBUFF_ReserveBlock().
 *
 *     By default, HCI_NUM_ACL_BUFFERS_ENHANCED is set to 
 *     L2CAP_ERTM_RX_WIN_SIZE * L2CAP_NUM_ENHANCED_CHANNELS * 
 *     HCI_ACL_ENHANCED_MULTI. This default is not necessarily the best 
 *     amount. Not all enhanced channels will need the max window size nor 
 *     will all enhanced channels support fragmentation. 
 *
 *     A more efficient approach is to determine the maximum number 
 *     of enhanced channels that need to run at the same time and the 
 *     necessary window size for each enhanced channel then set 
 *     HCI_NUM_ACL_BUFFERS_ENHANCED to the sum of the window sizes of the 
 *     enhanced channels. A multiplier could be applied (see
 *     HCI_ACL_ENHANCED_MULTI to understand the multiplier). Only Enhanced
 *     channels that support receiving fragments where 
 *     flowParms.fragmentsEnabled = TRUE need to be considered in the
 *     calculation.
 *
 *     L2CAP reserves a block of HCI buffers for each Enhanced channel
 *     with fragments enabled during L2CAP configuration. If the number 
 *     of HCI buffers for the desired window size is not available then 
 *     L2CAP will negotiate a smaller window size. If no buffers can be
 *     reserved then the channel configuration negotiation may fail.
 *
 *     An example of a more efficient approach to setting 
 *     HCI_NUM_ACL_BUFFERS_ENHANCED is as follows:
 *
 *     L2CAP_NUM_ENHANCED channels is 6. Three of those channels have 
 *     flowParms.fragmentsEnabled = TRUE (receiving fragments enabled). 
 *     L2CAP_ERTM_RX_WIN_SIZE is 10 and L2CAP_ERTM_RX_WIN_SIZE_DEFAULT is
 *     5. One of the enhanced channels configured to receive fragments is
 *     configured for the max window size -- flowParms.rxWindow =  
 *     L2CAP_ERTM_RX_WIN_SIZE (10) while the other two channels that support
 *     fragments have the default window size -- flowParms.rxWindow = 
 *     L2CAP_ERTM_RX_WIN_SIZE_DEFAULT (5).
 *
 *     For this setup the HCI_NUM_ACL_BUFFERS_ENHANCED should be set as
 *     follows:
 *
 *     #define HCI_NUM_ACL_BUFFERS_ENHANCED ((L2CAP_ERTM_RX_WIN_SIZE +
 *                                            (2 * L2CAP_ERTM_RX_WIN_SIZE_DEFAULT))
 *                                           * HCI_ACL_ENHANCED_MULTI)
 *
 *     The default setting for  HCI_NUM_ACL_BUFFERS_ENHANCED with the values 
 *     above and HCI_ACL_ENHANCED_MULTI set to 3 is (10 * 6 * 3) = 180. The 
 *     optimized number of buffers is ((10 + (2 * 5)) * 3) = 60. This is a 
 *     big savings in memory without any change in performance.
 */ 
#if L2CAP_NUM_ENHANCED_CHANNELS > 0
#ifndef HCI_NUM_ACL_BUFFERS_ENHANCED
#define HCI_NUM_ACL_BUFFERS_ENHANCED  (L2CAP_ERTM_RX_WIN_SIZE * L2CAP_NUM_ENHANCED_CHANNELS * \
                                       HCI_ACL_ENHANCED_MULTI)
#endif  
#else
#undef HCI_NUM_ACL_BUFFERS_ENHANCED
#define HCI_NUM_ACL_BUFFERS_ENHANCED 0
#endif /* L2CAP_NUM_ENHANCED_CHANNELS > 0 */

/*-------------------------------------------------------------------------
 * HCI_ACL_LE_MULTI constant
 *
 *     Defines a multiplier that is used when allocating HCI buffers for
 *     LE L2CAP dynamic channels. LE L2CAP dynamic channel PDUs will
 *     fit within one HCI buffer but that does not mean the remote device
 *     is able to send a PDU in one baseband packet. For many reasons a PDU
 *     may be sent in multiple baseband band packets and appear to the HCI
 *     in multiple fragments (multiple HCI buffers). 
 *
 *     HCI_ACL_LE_MULTI specifies how many HCI buffers are allocated
 *     per PDU. LE L2CAP will begin copying data from one HCI buffer to another
 *     when the HCI_ACL_LE_MULTI number of HCI buffers has been
 *     allocated for a PDU. In this way LE L2CAP will make sure only 
 *     HCI_ACL_LE_MULTI HCI buffers per PDU is maintained when PDUs
 *     are being held waiting for local busy to clear.
 *
 *     It is more efficient for the stack if it can avoid copying data
 *     until it is absolutely necessary so larger values for 
 *     HCI_ACL_LE_MULTI will allow L2CAP to be more efficient and
 *     reduce copying but with a tradeoff of memory (RAM) size.
 *
 *     The default for HCI_ACL_LE_MULTI is 1. 
 */
#ifndef HCI_ACL_LE_MULTI
#define HCI_ACL_LE_MULTI 1
#endif

#if HCI_ACL_LE_MULTI < 1 || HCI_ACL_LE_MULTI > 255 
#error HCI_ACL_LE_MULTI must be at least 1 and less than 256
#endif

/*-------------------------------------------------------------------------
 * HCI_NUM_ACL_BUFFERS_LE constant
 *
 *     Defines the number of buffers allocated by the HCI driver
 *     for receiving ACL data destined for LE L2CAP dynamic channels where
 *     the LE L2CAP layer handles the credit internally. This
 *     is also the number of HCI buffers that can be reserved by LE L2CAP
 *     for dynamic channels using RXBUFF_ReserveBlock().
 *
 *     By default, LEL2CAP_TOT_NUM_DYNAMIC_CHNLS * HCI_ACL_LE_MULTI buffers 
 *     are allocated but this is not necessarily the best amount. Not all 
 *     LE L2CAP dynamic channels will need max credits. Some may need less 
 *     and some may deal with credits on their own. A more efficient 
 *     approach is to determine the maximum number of LE L2CAP dynamic 
 *     channels that need to run simultaneously and the necessary max 
 *     credits for each channel then set HCI_NUM_ACL_BUFFERS_LE to the 
 *     sum of the max credits of the LE L2CAP dynamic channels. A multiplier 
 *     could be applied (see HCI_ACL_LE_MULTI to understand the multiplier). 
 *
 *     LE L2CAP reserves a block of HCI buffers for each dynamic channel
 *     that requests LE L2CAP to handle credits during LE L2CAP dynamic 
 *     channel creation. If the number of HCI buffers is not 
 *     available for the desired max credits then LE L2CAP will negotiate a 
 *     smaller size or abort fail the connection attempt.
 */
#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
#ifndef HCI_NUM_ACL_BUFFERS_LE
#if 0 /* Since LE currently does not reserve buffers the value must be 0 */
#define HCI_NUM_ACL_BUFFERS_LE  (LEL2CAP_TOT_NUM_DYNAMIC_CHNLS * HCI_ACL_LE_MULTI)
#endif /* #if 0 */
#endif 
/* Temporary until LE is changed to reserve buffers */
#undef HCI_NUM_ACL_BUFFERS_LE
#define HCI_NUM_ACL_BUFFERS_LE 0
#else /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */
#undef HCI_NUM_ACL_BUFFERS_LE
#define HCI_NUM_ACL_BUFFERS_LE 0
#endif /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */

/*-------------------------------------------------------------------------
 * HCI_NUM_ACL_BUFFERS constant
 *
 *     Defines the number of buffers allocated by the HCI driver
 *     for receiving ACL data.
 *
 *     By default, HCI_NUM_ACL_BUFFERS_ENHANCED + NUM_BT_DEVICES + 
 *     NUM_LE_DEVICES buffers are allocated. At least one HCI buffer is
 *     needed per remote device assuming that the remote devices can all
 *     be connected at the same time. The Enhanced channels are in addition
 *     the minimum requirement since ERTM may have to hold receive buffers
 *     if the application above is busy.
 */
#ifndef HCI_NUM_ACL_BUFFERS
#define HCI_NUM_ACL_BUFFERS      (HCI_NUM_ACL_BUFFERS_ENHANCED + HCI_NUM_ACL_BUFFERS_LE + \
                                  ((NUM_BT_DEVICES + NUM_LE_DEVICES) * 2))
#endif

/*-------------------------------------------------------------------------
 * HCI_NUM_SCO_BUFFERS constant
 *
 *     Defines the number of buffers allocated by the HCI driver
 *     for receiving SCO data. If the number of SCO connections allowed
 *     (NUM_SCO_CONNS) is zero, the HCI does not need to
 *     allocate any SCO buffers.
 *
 *     By default, 8 SCO data buffers are allocated.
 */
#ifndef HCI_NUM_SCO_BUFFERS
#define HCI_NUM_SCO_BUFFERS      8
#endif


/*---------------------------------------------------------------------------
 * TWIRE_UART_MULTIPLIER constant
 *
 *     For the HCI Three-Wire Transport Layer code, this value indicates how
 *     much UART speeds are multiplied by the internal UART hardware.
 *     Multiplying the UART clock allows the hardware to achieve higher
 *     speeds while allowing legacy software to use the older unclocked
 *     values.  If the UART hardware being used does not have a clock
 *     multiplier, then this value should be set to the default value of 1.
 *     Otherwise, it must be overridden in the overide.h file to the correct
 *     multiplier value or the calculations of Tmax will not be correct.  If
 *     Tmax is too large, then some devices will time out operations before
 *     they are acknowledged, making the HCI Three-Wire communication unusable.
 */
#ifndef TWIRE_UART_MULTIPLIER
#define TWIRE_UART_MULTIPLIER (1)
#endif


/*---------------------------------------------------------------------------
 * TWIRE_SLIDING_WINDOW constant
 *
 *     For the HCI Three-Wire Transport Layer code, this value indicates how
 *     many sliding window slots are allowed by the host.  The controller
 *     has an independent number of sliding window slots that it supports.
 *     The lower value of the two sliding window slots from the host and
 *     the controller is the actual number of sliding window slots that
 *     will be used.  This define should range from 1 to 7.  Larger
 *     values require more memory for reliable message retransmission
 *     buffers.  If the controller also supports them, larger values
 *     will result in higher payload throughput if the connection to the
 *     controller is reliable and bug free, due to the reduced need for
 *     as many acknowledgements.  Smaller values reduce how many messages
 *     may be outstanding before an acknowledgement is required.
 */
#ifndef TWIRE_SLIDING_WINDOW
#define TWIRE_SLIDING_WINDOW (3)
#endif

/*---------------------------------------------------------------------------
 * TWIRE_CRC constant
 *
 *     For the HCI Three-Wire Transport Layer code, this value indicates
 *     whether CRCs are allowed by the host.  Both the host and the
 *     controller must allow CRCs in order for them to be actually used.
 */
#ifndef TWIRE_CRC
#define TWIRE_CRC XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * TWIRE_OOF constant
 *
 *     For the HCI Three-Wire Transport Layer code, this value indicates
 *     whether out-of-frame software flow control is allowed by the
 *     host.  Both the host and the controller must allow out-of-frame
 *     software flow control in order for it to be actually used.
 *     Currently out-of-frame flow control is not implemented in Three-Wire
 *     code so this must be Disabled.
 */
#ifndef TWIRE_OOF
#define TWIRE_OOF XA_DISABLED
#endif

#if TWIRE_OOF != XA_DISABLED
#error TWIRE_OOF must be set to XA_DISABLED - OOF is not currently implemented
#endif

/*---------------------------------------------------------------------------
 * TWIRE_TX_BUFF_SIZE constant
 *
 *     For the HCI Three-Wire Transport Layer code, this value indicates
 *     the size of the Transmit buffer in which slip frames are created.
 *     The size is not critical but the bigger it is the less number of
 *     writes to the UART physical layer are needed to send a packet.
 *     In some systems it may work better if the size is 1. The default is 1024.
 */
#ifndef TWIRE_TX_BUFF_SIZE
#define TWIRE_TX_BUFF_SIZE (1024)
#endif

/*---------------------------------------------------------------------------
 * TWIRE_RX_BUFF_SIZE constant
 *
 *     For the HCI Three-Wire Transport Layer code, this value indicates
 *     the size of the Receive buffer into which raw data is read from
 *     the UART physical layer (UART_Read). In some cases the physical UART
 *     code is interrupt driven and stores received data into a circular buffer.
 *     UART_Read can be called while data is being read. In this case the
 *     a size of 1 is the best choice because it saves memory. The default is 1.
 */
#ifndef TWIRE_RX_BUFF_SIZE
#define TWIRE_RX_BUFF_SIZE (1)
#endif

/*---------------------------------------------------------------------------
 * TWIRE_RETRANSMIT_TIME constant
 *
 *     For the HCI Three-Wire Transport Layer code, this value indicates
 *     the time for the retransmit timer in milliseconds. The Bluetooth Core
 *     Specification 4.0 recommends that the retransmit timer should be 3 * Tmax
 *     where Tmax is expresses as:
 *
 *     Tmax = Max packet size in bits/ baud rate.
 *
 *     In practice a controller may be able to support a fast baud rate but
 *     not be able to process packets fast enough such that a timer set to
 *     3 * Tmax will result in unnecessary retransmissions. In this case,
 *     better performance can be achieved by using a longer retransmission
 *     timer value. The default is 0 which means use 3 * Tmax.
 */
#ifndef TWIRE_RETRANSMIT_TIME
#define TWIRE_RETRANSMIT_TIME (0)
#endif

/*---------------------------------------------------------------------------
 * TWIRE_ACK_TIME constant
 *
 *     For the HCI Three-Wire Transport Layer code, this value indicates
 *     the time to wait to send an acknowledgement after receiving a reliable
 *     packet. The Bluetooth Core Specification 4.0 recommends that the
 *     maximum wait time before sending an acknowledgement is 2 * Tmax
 *     where Tmax is expresses as:
 *
 *     Tmax = Max packet size in bits/ baud rate.
 *
 *     In practice it may be better to shorten the time or make it 0.
 *     The default is 0. The value 0xFFFF can be used to set the timer to
 *     1 * Tmax. The value 0xFFFE can be used to set the timer to 2 * Tmax.
 */
#ifndef TWIRE_ACK_TIME
#define TWIRE_ACK_TIME (0)
#endif


/*---------------------------------------------------------------------------
 * BT_EXPOSE_BCCMD constant
 *
 *     Exposes an interface to the BCCMD interface supported by CSR chips
 *     using ABCSP connections.
 *
 *     For more information about the BCCMD interface, refer to the
 *     documentation provided by CSR.
 *
 *     By default, the BCCMD interface is disabled because most applications
 *     do not need to use the interface.
 */
#define BT_EXPOSE_BCCMD XA_DISABLED

/****************************************************************************
 *
 * Section: L2CAP Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * L2CAP_NUM_PROTOCOLS constant
 *
 *     Defines the number of protocol services that may be layered above
 *     L2CAP.
 *
 *     Between 1 and 255 L2CAP protocol services are allowed. The BR/EDR 
 *     default is three: one for the SDP client, one for the SDP server,
 *     and one for RFCOMM.  The BR/EDR/LE default is four to account also
 *     for ATT.
 */
#ifndef L2CAP_NUM_PROTOCOLS
#if NUM_LE_DEVICES > 0
#define L2CAP_NUM_PROTOCOLS         4
#else
#define L2CAP_NUM_PROTOCOLS         3
#endif
#endif

#if L2CAP_NUM_PROTOCOLS < 1 || L2CAP_NUM_PROTOCOLS > 255
#error L2CAP_NUM_PROTOCOLS must be between 1 and 255
#endif

/*---------------------------------------------------------------------------
 * L2CAP_EXT_WINDOW_SIZE constant
 *
 *     Defines whether L2CAP supports the Extended Window Size Feature. The
 *     standard maximum ERTM window size is 63. The extended maximum ERTM
 *     window size is 16383.
 *
 *     By default, L2CAP uses the standard maximum ERTM window size to save
 *     memory.
 */
#ifndef L2CAP_EXT_WINDOW_SIZE
#define L2CAP_EXT_WINDOW_SIZE       XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * L2CAP_RTX_TIMEOUT constant
 *
 *     Defines the response timeout value for signaling commands, in
 *     seconds. If the timer elapses without a response from the remote
 *     device, the channel is disconnected.
 *
 *     The timeout value may range from 1 to 60 seconds. The default value
 *     is 30 seconds.
 */
#ifndef L2CAP_RTX_TIMEOUT
#define L2CAP_RTX_TIMEOUT           30
#endif

#if L2CAP_RTX_TIMEOUT < 1 || L2CAP_RTX_TIMEOUT > 60
#error L2CAP_RTX_TIMEOUT must be between 1 and 60 seconds.
#endif


/*---------------------------------------------------------------------------
 * L2CAP_ERTX_TIMEOUT constant
 *
 *     Defines the extended response timeout value for signaling commands,
 *     in seconds. Certain signaling commands (such as a Connect Response
 *     Pending) allow for a longer timeout period than the L2CAP_RTX_TIMEOUT
 *     value. When the ERTX timer elapses without a response, the channel
 *     is disconnected.
 *
 *     The extended timeout value may range from 60 to 300 seconds. The
 *     default value is 150 seconds.
 */
#ifndef L2CAP_ERTX_TIMEOUT
#define L2CAP_ERTX_TIMEOUT          150
#endif

#if L2CAP_ERTX_TIMEOUT < 60 || L2CAP_ERTX_TIMEOUT > 300
#error L2CAP_ERTX_TIMEOUT must be between 60 and 300 seconds.
#endif


/*---------------------------------------------------------------------------
 * L2CAP_DEFAULT_BASIC_MODE constant
 *
 *     Sets the default channel link mode to BASIC Mode.
 *
 *     By default this option is set to XA_DISABLED and L2CAP will configure
 *     channels to use ERTM or Streaming modes. Profiles may always override
 *     the mode settings by setting the flowParms linkMode options in the
 *     L2CAP_RegisterPSM(), L2CAP_ConnectReq() or L2CAP_ConnectRsp() APIs.
 */
#ifndef L2CAP_DEFAULT_BASIC_MODE
#define L2CAP_DEFAULT_BASIC_MODE    XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * L2CAP_NUM_BR_FIXED_CHANNELS constant
 *
 *     Defines the number of L2CAP fixed Channels for BR/EDR links
 */
#ifndef L2CAP_NUM_BR_FIXED_CHANNELS
#if NUM_AMPS > 0
#if NUM_LE_DEVICES > 0
/* Need four channels, Signal, Connectionless, AMP Manager, Security Manager */
#define L2CAP_NUM_BR_FIXED_CHANNELS    4
#else
/* Need three channels, Signal, Connectionless, AMP Manager */
#define L2CAP_NUM_BR_FIXED_CHANNELS    3
#endif
#else
#if NUM_LE_DEVICES > 0
/* Need three channels, Signal, Connectionless, Security Manager */
#define L2CAP_NUM_BR_FIXED_CHANNELS    3
#else
/* Need two channels, Signal, Connectionless */
#define L2CAP_NUM_BR_FIXED_CHANNELS    2
#endif
#endif /* NUM_AMPS > 0 */
#endif /* #ifndef L2CAP_NUM_BR_FIXED_CHANNELS */

#if L2CAP_NUM_BR_FIXED_CHANNELS < 2
#error L2CAP_NUM_BR_FIXED_CHANNELS must be at least 2
#endif

/*---------------------------------------------------------------------------
 * L2CAP_ERTM_TX_WIN_SIZE constant
 *
 *     Specifies the maximum send window size for L2CAP Enhanced mode
 *     channels. This setting controls the amount of memory allocated to
 *     send unacknowledged ERTM I-frame segments. This value is indicated in
 *     the Configuration Response as a hint to the remote device of the number
 *     of frames that can be sent before resources are depleted. The remote
 *     device can use this information to efficiently acknowledge I-frames.
 *     A smaller size will save memory. A larger size will increase
 *     throughput.
 *
 *     Note that the TX window may be adjusted lower than this value
 *     if the remote device specifies an RX window that is smaller than the
 *     local L2CAP_ERTM_TX_WIN_SIZE since we must not overflow the remote
 *     device's RX window.
 *
 *     By default this is set to 5. It should be adjusted as appropriate
 *     for your implementation.
 */
#ifndef L2CAP_ERTM_TX_WIN_SIZE
#define L2CAP_ERTM_TX_WIN_SIZE      5
#endif

#if L2CAP_EXT_WINDOW_SIZE == XA_ENABLED
#if L2CAP_ERTM_TX_WIN_SIZE > 16383 || L2CAP_ERTM_TX_WIN_SIZE < 1
#error L2CAP_ERTM_TX_WIN_SIZE must be between 1 and 16383
#endif
#else 
#if L2CAP_ERTM_TX_WIN_SIZE > 63 || L2CAP_ERTM_TX_WIN_SIZE < 1
#error L2CAP_ERTM_TX_WIN_SIZE must be between 1 and 63
#endif
#endif /* L2CAP_EXT_WINDOW_SIZE == XA_ENABLED */

/*---------------------------------------------------------------------------
 * L2CAP_ERTM_RX_WIN_SIZE constant
 *
 *     Specifies the maximum receive window size for L2CAP Enhanced mode
 *     channels. This setting controls the amount of memory allocated to
 *     manage received ERTM I-frames before indication to the higher layer.
 *     A smaller size will save memory. A larger size will increase
 *     throughput. Certain profiles do not need a large window size since
 *     they expect a response on every frame indicated. The OBEX profile
 *     family can take advantage of a large window size.
 *
 *     By default this is set to 5. It should be adjusted as appropriate
 *     for your implementation.
 */
#ifndef L2CAP_ERTM_RX_WIN_SIZE
#define L2CAP_ERTM_RX_WIN_SIZE      5
#endif

#if L2CAP_EXT_WINDOW_SIZE == XA_ENABLED
#if L2CAP_ERTM_RX_WIN_SIZE > 16383 || L2CAP_ERTM_RX_WIN_SIZE < 1
#error L2CAP_ERTM_RX_WIN_SIZE must be between 1 and 16383
#endif
#else
#if L2CAP_ERTM_RX_WIN_SIZE > 63 || L2CAP_ERTM_RX_WIN_SIZE < 1
#error L2CAP_ERTM_RX_WIN_SIZE must be between 1 and 63 since extended window size is not supported.
#endif
#endif /* L2CAP_EXT_WINDOW_SIZE == XA_ENABLED */

/*---------------------------------------------------------------------------
 * L2CAP_ERTM_TX_WIN_SIZE_DEFAULT constant
 *
 *     Specifies the default send window size for L2CAP Enhanced mode
 *     channels when one is not specified by the L2CAP user. It is also
 *     a good value to be used by applications and protocols that don't
 *     need the maximum value.
 *
 *     Note that the TX window may be adjusted lower than this default value
 *     if the remote device specifies an RX window that is smaller than the
 *     local L2CAP_ERTM_TX_WIN_SIZE_DEFAULT since we must not overflow the 
 *     remote device's RX window. This value must be less than or equal to
 *     L2CAP_ERTM_TX_WIN_SIZE
 *
 *     By default this is set to L2CAP_ERTM_TX_WIN_SIZE so it backwards
 *     compatible with previous versions of the stack. It should be 
 *     adjusted as appropriate for your implementation. It must be less 
 *     than or equal to L2CAP_ERTM_TX_WIN_SIZE.
 */
#ifndef L2CAP_ERTM_TX_WIN_SIZE_DEFAULT
#define L2CAP_ERTM_TX_WIN_SIZE_DEFAULT      L2CAP_ERTM_TX_WIN_SIZE
#endif

#if L2CAP_ERTM_TX_WIN_SIZE_DEFAULT > L2CAP_ERTM_TX_WIN_SIZE
#error L2CAP_ERTM_TX_WIN_SIZE_DEFAULT must be <= L2CAP_ERTM_TX_WIN_SIZE
#endif

/*---------------------------------------------------------------------------
 * L2CAP_ERTM_RX_WIN_SIZE_DEFAULT constant
 *
 *     Specifies the default receive window size for L2CAP Enhanced mode
 *     channels when the L2CAP user does not supply one. 
 *
 *     By default this is set to L2CAP_ERTM_RX_WIN_SIZE to be backwards
 *     compatible with previous versions of the stack. It should be adjusted 
 *     as appropriate for your implementation. It must be less than or equal 
 *     to L2CAP_ERTM_RX_WIN_SIZE.
 */
#ifndef L2CAP_ERTM_RX_WIN_SIZE_DEFAULT
#define L2CAP_ERTM_RX_WIN_SIZE_DEFAULT     L2CAP_ERTM_RX_WIN_SIZE
#endif

#if L2CAP_ERTM_RX_WIN_SIZE_DEFAULT > L2CAP_ERTM_RX_WIN_SIZE
#error L2CAP_ERTM_RX_WIN_SIZE_DEFAULT must be <= L2CAP_ERTM_RX_WIN_SIZE
#endif

/*---------------------------------------------------------------------------
 * L2CAP_ERTM_MAX_TRANSMIT constant
 *
 *     Specifies the default maximum number of transmissions of an I-frame
 *     (including the first transmission) before the link is disconnected.
 *
 *     The default is 10 transmissions.
 */
#ifndef L2CAP_ERTM_MAX_TRANSMIT
#define L2CAP_ERTM_MAX_TRANSMIT     10
#endif

/*---------------------------------------------------------------------------
 * L2CAP_ERTM_RETRANS_TIMEOUT constant
 *
 *     Specifies the default Retransmission timeout value in milliseconds.
 *     If a flush timeout is defined for the link, this value should be the
 *     greater of 3 times the flush timeout value or 1 second.
 *
 *     The default is 2 seconds.
 */
#ifndef L2CAP_ERTM_RETRANS_TIMEOUT
#define L2CAP_ERTM_RETRANS_TIMEOUT  2000
#endif

/*---------------------------------------------------------------------------
 * L2CAP_ERTM_MONITOR_TIMEOUT constant
 *
 *     Specifies the default Monitor timeout value in milliseconds. If a
 *     flush timeout is defined for the link, this value should be the
 *     greater of 6 times the flush timeout value or 12 seconds.
 *
 *     The default is 12 seconds.
 */
#ifndef L2CAP_ERTM_MONITOR_TIMEOUT
#define L2CAP_ERTM_MONITOR_TIMEOUT  12000
#endif

/*---------------------------------------------------------------------------
 * L2CAP_FCS_OPTION constant
 *
 *     Specifies the default FCS option for Enhanced L2CAP channels. Use
 *     one of the options defined by the L2capFcsOption type in l2cap.h.
 *
 *     The default is 16 bit with the option to turn off FCS.
 */
#ifndef L2CAP_FCS_OPTION
#define L2CAP_FCS_OPTION            L2FCS_16BIT
#endif

#if L2CAP_FCS_OPTION != L2FCS_NONE && L2CAP_FCS_OPTION != L2FCS_16BIT
#error L2CAP_FCS_OPTION must be L2FCS_NONE or L2FCS_16BIT.
#endif

/*---------------------------------------------------------------------------
 * L2CAP_ERTM_ACK_TIMEOUT constant
 *
 *     Specifies the default maximum amount of time to wait before a received
 *     I-frame is acknowledged. Normally, I-frames are acknowledged long
 *     before this timer expires on a busy channel. This timer ensures
 *     acknowledgments are sent in a timely manner on a channel with little
 *     traffic.
 *
 *     Note: this timer will not fire if set larger than
 *     L2CAP_ERTM_RETRANS_TIMEOUT.
 *
 *     The default is 1/2 retransmission timeout.
 */
#ifndef L2CAP_ERTM_ACK_TIMEOUT
#define L2CAP_ERTM_ACK_TIMEOUT      (L2CAP_ERTM_RETRANS_TIMEOUT/2)
#endif

/*---------------------------------------------------------------------------
 * L2CAP_ERTM_SREJ_ENABLE constant
 *
 *     Specifies whether Selective Reject capability is enabled for the
 *     enhanced L2CAP channels. Disabling Selective Reject will save memory,
 *     but will likely cause more retransmitted I-frames.
 *
 *     Selective Reject is Enabled by default.
 */
#ifndef L2CAP_ERTM_SREJ_ENABLE
#define L2CAP_ERTM_SREJ_ENABLE      XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * L2CAP_FLOW_CONTROL constant
 *
 *     This option is deprecated. Use L2CAP_NUM_ENHANCED_CHANNELS.
 */
#ifndef L2CAP_FLOW_CONTROL
#define L2CAP_FLOW_CONTROL          XA_DISABLED
#endif

#if L2CAP_FLOW_CONTROL == XA_ENABLED
#error The L2CAP_FLOW_CONTROL option is deprecated.
#endif

/*---------------------------------------------------------------------------
 * L2CAP_PRIORITY constant
 *
 *     Enables support for L2CAP BtPacket priority management.  This define
 *     is always enabled and is provided only for backward compatibility.
 */
#ifndef L2CAP_PRIORITY
#define L2CAP_PRIORITY              XA_ENABLED
#endif

#if L2CAP_PRIORITY != XA_ENABLED
#error L2CAP_PRIORITY must always be defined as XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * L2CAP_MPS constant
 *
 *     Defines the maximum receivable L2CAP PDU size, in bytes. When
 *     L2CAP_NUM_ENHANCED_CHANNELS is 1 or greater, an L2CAP_MTU may be segmented
 *     into multiple L2CAP_MPS sized PDUs. For BASIC mode a whole SDU must be
 *     sent as one unit so a PDU and SDU are basically the same thing.
 *
 *     This value may range from 670 to HCI_ACL_DATA_SIZE less bytes for L2CAP and
 *     HCI headers when NUM_ENHANCED_CHANNELS is greater than 0 otherwise the minimum
 *     of the range is 48. The default value is HCI_ACL_DATA_SIZE less bytes for 
 *     L2CAP and HCI headers.
 *
 */
#ifndef L2CAP_MPS
#if HCI_ACL_DATA_SIZE > MAX_AMP_DATA_SIZE
#define L2CAP_MPS                   (HCI_ACL_DATA_SIZE - HCI_ACL_BUFF_HEADER_OVERHEAD)
#else
#define L2CAP_MPS                   (MAX_AMP_DATA_SIZE - HCI_ACL_BUFF_HEADER_OVERHEAD)
#endif
#endif

#if L2CAP_NUM_ENHANCED_CHANNELS > 0
#if L2CAP_MPS < 670
#error L2CAP_MPS must be at least 670
#endif
#else 
#if L2CAP_MPS < 48
#error L2CAP_MPS must be at least 48
#endif
#endif /* #if L2CAP_NUM_ENHANCED_CHANNELS > 0 */

#if HCI_ACL_DATA_SIZE > MAX_AMP_DATA_SIZE
#if L2CAP_MPS > (HCI_ACL_DATA_SIZE - HCI_ACL_BUFF_HEADER_OVERHEAD)
#error L2CAP_MPS must be <= (HCI_ACL_DATA_SIZE - HCI_ACL_BUFF_HEADER_OVERHEAD)
#endif
#else
#if L2CAP_MPS > (MAX_AMP_DATA_SIZE - HCI_ACL_BUFF_HEADER_OVERHEAD)
#error L2CAP_MPS must be <= (MAX_AMP_DATA_SIZE - HCI_ACL_BUFF_HEADER_OVERHEAD)
#endif
#endif


/*---------------------------------------------------------------------------
 * L2CAP_MTU constant
 *
 *     Defines the maximum receivable L2CAP SDU size, in bytes when the L2CAP
 *     user is unable to receive SDUs in fragments. This limitation applies 
 *     only to packets received from the remote device. L2CAP_MTU must be
 *     less than or equal to L2CAP_MPS and it is recommended that it be set to
 *     L2CAP_MPS since there is no memory savings by setting it smaller.
 *     L2CAP users may set a smaller MTU in the L2capPsm structure (localMtu) 
 *     if a smaller MTU is wanted for performance reasons.
 *
 *     By limiting L2CAP_MTU to L2CAP_MPS this allows a complete SDU to fit
 *     in an HCI ACL buffer so no memory is required in L2CAP for building
 *     a contiguous SDU. L2CAP holds the first HCI ACL buffer and copies all 
 *     fragments into that buffer. When the SDU is complete it is delivered to 
 *     the upper layer and the HCI ACL buffer is freed. 
 *
 *     If this size restriction is a problem for L2CAP users then they can enable
 *     fragmentation which allows them to set the SDU size to any value. It is 
 *     then up to the L2CAP user to parse the fragments or copy them into their
 *     own contiguous SDU buffer. 
 *
 *     This value defines the maximum size of an L2CAP SDU for BASIC mode, 
 *     ERTM, Streaming mode and LE Credit based channels when fragmentation is 
 *     not enabled.
 *
 *     This constant also affects the L2CAP Connectionless MTU. Some profiles
 *     require a minimum Connectionless MTU that is greater than the L2CAP
 *     default of 670 bytes. When implementing such a profile it is important
 *     that the L2CAP_MTU constant be at least as large as the specified
 *     Connectionless MTU size.
 *
 *     This value may range from 670 to L2CAP_MPS when L2CAP_NUM_ENHANCED_CHANNELS
 *     is 1 or greater or from 48 to L2CAP_MPS otherwise. The default is based on the
 *     L2CAP_MPS to maximize throughput at the controller.
 *
 */
#ifndef L2CAP_MTU
#define L2CAP_MTU                   L2CAP_MPS
#endif

#if L2CAP_NUM_ENHANCED_CHANNELS > 0
#if L2CAP_MTU < 670 || L2CAP_MTU > L2CAP_MPS
#error L2CAP_MTU must be between 670 and L2CAP_MPS.
#endif
#else
#if L2CAP_MTU < 48 || L2CAP_MTU > L2CAP_MPS
#error L2CAP_MTU must be between 48 and L2CAP_MPS.
#endif
#endif

/*---------------------------------------------------------------------------
 * L2CAP_MAX_FRAG_MTU constant
 *
 *     Defines the maximum receivable L2CAP SDU size, in bytes when the L2CAP
 *     user can receive SDUs in fragments. L2CAP does not base buffers on
 *     this value so it is limited by the packet structure itself and/or the
 *     buffer space of the L2CAP users. This limitation applies only to packets 
 *     received from the remote device. 
 *
 *     This value is 65529 when L2CAP_NUM_ENHANCED_CHANNELS
 *     is 1 or greater and 65533 otherwise.  
 *
 */
#if L2CAP_NUM_ENHANCED_CHANNELS > 0
#define L2CAP_MAX_FRAG_MTU  65529
#else
#define L2CAP_MAX_FRAG_MTU  65533
#endif

/*---------------------------------------------------------------------------
 * L2CAP_FRAG_MTU constant
 *
 *     Defines the recommended receivable L2CAP SDU size, in bytes when the L2CAP
 *     user can receive SDUs in fragments. L2CAP does not base buffers on
 *     this value so it is limited by the packet structure itself and/or the
 *     buffer space of the L2CAP users. This limitation applies only to packets 
 *     received from the remote device. 
 *
 *     This value may range from 670 to 65529 when L2CAP_NUM_ENHANCED_CHANNELS
 *     is 1 or greater or from 48 to 65533 otherwise. The default is set to the
 *     L2CAP_MPS*L2CAP_ERTM_RX_WIN_SIZE or the largest size allowed. 
 *
 *     
 */
#ifndef L2CAP_FRAG_MTU
#if L2CAP_NUM_ENHANCED_CHANNELS > 0
#if (L2CAP_MPS*L2CAP_ERTM_RX_WIN_SIZE) > 65529
#define L2CAP_FRAG_MTU                   65529
#else
#define L2CAP_FRAG_MTU                   (L2CAP_MPS*L2CAP_ERTM_RX_WIN_SIZE)
#endif
#else /* L2CAP_NUM_ENHANCED_CHANNELS > 0 */
#define L2CAP_FRAG_MTU                   65533  
#endif /* L2CAP_NUM_ENHANCED_CHANNELS > 0 */
#endif

#if L2CAP_NUM_ENHANCED_CHANNELS > 0
#if L2CAP_FRAG_MTU < 670 || L2CAP_FRAG_MTU > 65529
#error L2CAP_FRAG_MTU must be between 670 and 65529.
#endif
#else
#if L2CAP_FRAG_MTU < 48 || L2CAP_FRAG_MTU > 65533
#error L2CAP_FRAG_MTU must be between 48 and 65533.
#endif
#endif /* L2CAP_NUM_ENHANCED_CHANNELS > 0 */

/*---------------------------------------------------------------------------
 * L2CAP_DEREGISTER_FUNC constant
 *
 *     Controls whether the L2CAP_DeregisterPsm function is allowed. In
 *     cases where L2CAP services will be dynamically loaded or unloaded,
 *     this constant must be enabled.
 *
 *     By default, L2CAP service deregistration is disabled. Disabling this
 *     option results in code size savings.
 */
#ifndef L2CAP_DEREGISTER_FUNC
#define L2CAP_DEREGISTER_FUNC       XA_DISABLED
#endif


/*---------------------------------------------------------------------------
 * L2CAP_NUM_SIGNAL_PACKETS constant
 *
 *     Defines the number of signaling packets allocated by L2CAP for BR/EDR.
 *
 *     By default, L2CAP allocates two packets per device plus 8
 *     packets per enhanced channel.
 */
#ifndef L2CAP_NUM_SIGNAL_PACKETS
#if L2CAP_NUM_ENHANCED_CHANNELS > 0
#define L2CAP_NUM_SIGNAL_PACKETS    ((NUM_BT_DEVICES * 2) + (L2CAP_NUM_ENHANCED_CHANNELS * 8) + (L2CAP_NUM_BR_FIXED_CHANNELS * 2))
#else
#define L2CAP_NUM_SIGNAL_PACKETS    (NUM_BT_DEVICES * 2)
#endif
#endif

#if L2CAP_NUM_SIGNAL_PACKETS < 1
#error L2CAP_NUM_SIGNAL_PACKETS must be at least 1
#endif

/*---------------------------------------------------------------------------
 * L2CAP_NUM_TX_PACKETS constant
 *
 *     Defines the number of TX packets allocated by L2CAP for segmenting
 *     upper layer SDUs on Enhanced Channels. Basic Mode channels do not
 *     segment SDUs and do not need TX packets.
 *
 *     This value is based on information obtained from modeling at run time.
 *     A best estimate should be calculated from the max expected TX window
 *     and max number of segments per SDU. If the channel sends only
 *     SDUs smaller than the L2CAP_MPS value, then only one TX packet is
 *     required. If a channel sends large SDUs, then it is likely it will
 *     require enough TX packets to fill the max TX window.
 *
 *     This setting is the total number of TX packets in the entire system.
 *     Remember to allocate enough TX packets for all Enhanced channels.
 *     The default allocates enough TX packets to maximize the TX window on
 *     a single channel plus a single TX packet for each additional Enhanced
 *     channel in the system. The first compile time error checks for this
 *     default as a minimum. If SDUs will not require segmentation, this
 *     check may be disabled. The second compile time error check must not
 *     be disabled. At least one TX packet for each Enhanced channel is
 *     required.
 */
#ifndef L2CAP_NUM_TX_PACKETS
#define L2CAP_NUM_TX_PACKETS        (L2CAP_ERTM_TX_WIN_SIZE + L2CAP_NUM_ENHANCED_CHANNELS)
#endif

/* This check may be disabled if SDUs will not be segmented. */
#if L2CAP_NUM_TX_PACKETS < (L2CAP_ERTM_TX_WIN_SIZE + L2CAP_NUM_ENHANCED_CHANNELS)
#error Insufficient L2CAP_NUM_TX_PACKETS for TX window size
#endif

/* This check must not be disabled. */
#if L2CAP_NUM_TX_PACKETS < L2CAP_NUM_ENHANCED_CHANNELS
#error Insufficient L2CAP_NUM_TX_PACKETS for number of Enhanced channels
#endif

/*---------------------------------------------------------------------------
 * L2CAP_NUM_AGENTS constant
 *
 *     Defines the number of L2CAP Agents. There must be as many agents as
 *     there are channels. This value is used for agents allocated
 *     by meamp. If an implementation defines a new agent then this value
 *     can be reduced. Applications that use the old L2CAP API use agents
 *     as well.
 */
#ifndef L2CAP_NUM_AGENTS
#define L2CAP_NUM_AGENTS            L2CAP_NUM_CHANNELS
#endif

/*---------------------------------------------------------------------------
 * L2CAP_IDLE_DATA_TIMER constant
 *
 *     Specifies how often L2CAP checks the data throughput of each channel
 *     in milliseconds. L2CAP passes this data throughput information to the
 *     L2capAgent and the L2capAgent passes the data throughput information
 *     to the Channel Manager. The Channel Manager uses this information to
 *     manage whether a channel should remain on an AMP. Adjust this setting
 *     with care; as more frequent indications will increase overhead and
 *     may cause a channel to be moved off of an AMP prematurely.
 *
 *     By default, L2CAP checks data throughput every 20 seconds.
 */
#ifndef L2CAP_IDLE_DATA_TIMER
#define L2CAP_IDLE_DATA_TIMER       20000
#endif

/*---------------------------------------------------------------------------
 * L2CAP_EXTENDED_FEATURES constant
 *
 *     Defines the Extended Features supported by this L2CAP implementation.
 *
 *     By default, all available Extended Features are enabled.
 */
#ifndef L2CAP_EXTENDED_FEATURES
#if L2CAP_EXT_WINDOW_SIZE == XA_ENABLED
#define L2CAP_EXTENDED_FEATURES     (L2EXTENDED_FEATURE_FCS |            \
                                     L2EXTENDED_FEATURE_ERTM |           \
                                     L2EXTENDED_FEATURE_STREAMING |      \
                                     L2EXTENDED_FEATURE_FIXED_CHANNELS | \
                                     L2EXTENDED_FEATURE_UNICAST_DATA |   \
                                     L2EXTENDED_FEATURE_EXT_WINDOW_SIZE)
#elif L2CAP_NUM_ENHANCED_CHANNELS > 0
#define L2CAP_EXTENDED_FEATURES     (L2EXTENDED_FEATURE_FCS |            \
                                     L2EXTENDED_FEATURE_ERTM |           \
                                     L2EXTENDED_FEATURE_STREAMING |      \
                                     L2EXTENDED_FEATURE_FIXED_CHANNELS | \
                                     L2EXTENDED_FEATURE_UNICAST_DATA)
#else
#define L2CAP_EXTENDED_FEATURES     (L2EXTENDED_FEATURE_FIXED_CHANNELS | \
                                     L2EXTENDED_FEATURE_UNICAST_DATA)
#endif
#endif


/****************************************************************************
 *
 * Section: SDP Constants
 *
 ****************************************************************************/

/*-------------------------------------------------------------------------
 * SDP_CLIENT_SUPPORT constant
 *
 *     Controls whether the SDP client is included in the SDP protocol.  Set
 *     this constant to XA_ENABLED when it is necessary to have the SDP client
 *     compiled into your system.
 *
 *     By default, the SDP client is enabled.
 */
#ifndef SDP_CLIENT_SUPPORT
#define SDP_CLIENT_SUPPORT XA_ENABLED
#endif

/*-------------------------------------------------------------------------
 * SDP_SERVER_SUPPORT constant
 *
 *     Controls whether the SDP server is included in the SDP protocol.  Set
 *     this constant to XA_ENABLED when it is necessary to have the SDP server
 *     compiled into your system.
 *
 *     By default, the SDP server is enabled.
 */
#ifndef SDP_SERVER_SUPPORT
#define SDP_SERVER_SUPPORT XA_ENABLED
#endif

#if (SDP_SERVER_SUPPORT == XA_DISABLED) && (SDP_CLIENT_SUPPORT == XA_DISABLED)
#error SDP_CLIENT_SUPPORT and/or SDP_SERVER_SUPPORT must be enabled.
#endif

#if (SDP_SERVER_SUPPORT == XA_DISABLED) && (NUM_LE_DEVICES > 0)
#error SDP_SERVER_SUPPORT must be enabled if NUM_LE_DEVICES > 0
#endif

/*-------------------------------------------------------------------------
 * SDP_USE_BASIC_MODE constant
 *
 *     Controls whether the SDP protocol layer is restricted to use
 *     L2CAP BASIC link mode. If this option is disabled, SDP will use
 *     ERTM link mode if it is supported by the remote device. It may be
 *     necessary to enable this option for compatibility with older
 *     devices.
 *
 *     By default, SDP_USE_BASIC_MODE is enabled.
 */
#ifndef SDP_USE_BASIC_MODE
#define SDP_USE_BASIC_MODE XA_ENABLED
#endif

#if SDP_CLIENT_SUPPORT == XA_ENABLED
/*-------------------------------------------------------------------------
 * SDP_CLIENT_LOCAL_MTU constant
 *
 *     Defines the maximum L2CAP MTU used for the local SDP client.
 *
 *     This value may range from 48 to the L2CAP MTU size (L2CAP_MTU).
 *     By default, this value is equal to the L2CAP_MTU size.
 */
#ifndef SDP_CLIENT_LOCAL_MTU
#define SDP_CLIENT_LOCAL_MTU L2CAP_MTU
#endif

#if SDP_CLIENT_LOCAL_MTU < 48 || SDP_CLIENT_LOCAL_MTU > L2CAP_MTU
#error SDP_CLIENT_LOCAL_MTU must be between 48 and L2CAP_MTU.
#endif


/*-------------------------------------------------------------------------
 * SDP_CLIENT_MIN_REMOTE_MTU constant
 *
 *     Defines the minimum remote MTU size acceptable to the local
 *     SDP client. If the remote SDP server cannot support the specified
 *     MTU size, the connection attempt will fail and SDP operations will
 *     not occur.
 *
 *     This value may range from 48 to the L2CAP MTU size (L2CAP_MTU).
 *     By default, this value is 48.
 */
#ifndef SDP_CLIENT_MIN_REMOTE_MTU
#define SDP_CLIENT_MIN_REMOTE_MTU 48
#endif

#if SDP_CLIENT_MIN_REMOTE_MTU < 48 || SDP_CLIENT_MIN_REMOTE_MTU > L2CAP_MTU
#error SDP_CLIENT_MIN_REMOTE_MTU must be between 48 and L2CAP_MTU.
#endif

/*-------------------------------------------------------------------------
 * SDP_PARSING_FUNCS constant
 *
 *     Controls whether the SDP response parsing is included and made
 *     available to the application.  Set this constant to XA_ENABLED
 *     when it is necessary to have SDP response parsing available.
 *
 *     By default, SDP parsing is enabled.
 *
 */
#ifndef SDP_PARSING_FUNCS
#define SDP_PARSING_FUNCS XA_ENABLED
#endif

#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */

#if SDP_SERVER_SUPPORT == XA_ENABLED
/*-------------------------------------------------------------------------
 * SDP_NUM_CLIENTS constant
 *
 *     Defines the maximum number of clients that can connect
 *     simultaneously to the SDP server.
 *
 *     This value may range from 1 to the number of possible devices
 *     (NUM_BT_DEVICES). The default is NUM_BT_DEVICES.
 */
#ifndef SDP_NUM_CLIENTS
#define SDP_NUM_CLIENTS NUM_BT_DEVICES
#endif

#if (SDP_NUM_CLIENTS < 1) || (SDP_NUM_CLIENTS > NUM_BT_DEVICES)
#error SDP_NUM_CLIENTS must be greater than 0 and less than or equal to NUM_BT_DEVICES
#endif

/*-------------------------------------------------------------------------
 * SDP_ACTIVE_CLIENTS constant
 *
 *     Defines the maximum number of active clients that can connect
 *     simultaneously to the SDP server. Active clients are those that
 *     are not in park.
 *
 *     This value may range from 1 to the number of possible active devices
 *     (16). The default value is NUM_BT_DEVICES.
 */
#ifndef SDP_ACTIVE_CLIENTS
#if NUM_BT_DEVICES < 16
#define SDP_ACTIVE_CLIENTS  (NUM_BT_DEVICES)
#else
#define SDP_ACTIVE_CLIENTS  16
#endif
#endif

#if (SDP_ACTIVE_CLIENTS < 1) || (SDP_ACTIVE_CLIENTS > NUM_BT_DEVICES) || (SDP_ACTIVE_CLIENTS > 16)
#error SDP_ACTIVE_CLIENTS must be greater than 0 and less than or equal to NUM_BT_DEVICES and 16.
#endif

/*-------------------------------------------------------------------------
 * SDP_SERVER_SEND_SIZE constant
 *
 *     Defines the maximum number of bytes that the server can send in a
 *     single response.
 *
 *     This value may range from 48 to the L2CAP MTU size (L2CAP_MTU). The
 *     default is 128.
 */
#ifndef SDP_SERVER_SEND_SIZE
#define SDP_SERVER_SEND_SIZE 128
#endif

#if (SDP_SERVER_SEND_SIZE < 48) || (SDP_SERVER_SEND_SIZE > L2CAP_MTU)
#error SDP_SERVER_SEND_SIZE must be between 48 and L2CAP_MTU
#endif


/*-------------------------------------------------------------------------
 * SDP_SERVER_MAX_LEVEL constant
 *
 *     Defines the maximum number of nested data element sequences in
 *     an SDP attribute. SDP uses this value internally when examining
 *     attributes. If the number of nested data elements exceeds
 *     SDP_SERVER_MAX_LEVEL, certain SDP functions will return errors.
 *
 *     The default value is four, which allows for typical attributes.
 */
#ifndef SDP_SERVER_MAX_LEVEL
#define SDP_SERVER_MAX_LEVEL  4
#endif

#if SDP_SERVER_MAX_LEVEL < 4
#error SDP_SERVER_MAX_LEVEL must be at least 4
#endif


/*-------------------------------------------------------------------------
 * SDP_SERVER_LOCAL_MTU constant
 *
 *     Defines the maximum L2CAP MTU used for the local SDP server.
 *
 *     This value may range from 48 to the L2CAP MTU size (L2CAP_MTU).
 *     By default, this value is equal to the L2CAP_MTU size.
 */
#ifndef SDP_SERVER_LOCAL_MTU
#define SDP_SERVER_LOCAL_MTU L2CAP_MTU
#endif

#if SDP_SERVER_LOCAL_MTU < 48 || SDP_SERVER_LOCAL_MTU > L2CAP_MTU
#error SDP_SERVER_LOCAL_MTU must be between 48 and L2CAP_MTU.
#endif


/*-------------------------------------------------------------------------
 * SDP_SERVER_MIN_REMOTE_MTU constant
 *
 *     Defines the minimum remote MTU size acceptable to the local
 *     SDP server. If the remote SDP client cannot support the specified
 *     MTU size, the connection attempt will fail and SDP operations will
 *     not occur.
 *
 *     This value may range from 48 to the L2CAP MTU size (L2CAP_MTU).
 *     By default, this value is 48.
 */
#ifndef SDP_SERVER_MIN_REMOTE_MTU
#define SDP_SERVER_MIN_REMOTE_MTU 48
#endif

#if SDP_SERVER_MIN_REMOTE_MTU < 48 || SDP_SERVER_MIN_REMOTE_MTU > L2CAP_MTU
#error SDP_SERVER_MIN_REMOTE_MTU must be between 48 and L2CAP_MTU.
#endif

#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */

/*-------------------------------------------------------------------------
 * SDP_ALLOW_SECURITY_RECORD constant
 *
 *     Controls whether SEC_AccessRequest() will allow an SDP security record 
 *     to cause an incoming L2CAP connection request to SDP to start the
 *     paring process.
 *
 *     Normally SDP does not require authentication but in order to work with 
 *     a "buggy" phone that does not handle a missing link key correctly a
 *     solution involving SDP has been implemented.
 *
 *     The solution involves starting the pairing process when the "buggy" 
 *     phone attempts to connect to the SDP Server. When the upper layer
 *     detects an ACL connection to the "buggy" phone it will register a 
 *     security record for SDP that will cause pairing to occur when the
 *     "buggy" phone makes request to connect to the SDP server. 
 *
 *     Normally a request to the SDP servier is always allowed but if 
 *     SDP_ALLOW_SECURITY_RECORD is set to XA_ENABLED then SEC_AccessRequest() 
 *     and other security code will check for and use an SDP security record to
 *     start the pairing process. 
 *
 *     By default SDP_ALLOW_SECURITY_RECORD is disabled.
 */
#ifndef SDP_ALLOW_SECURITY_RECORD
#define SDP_ALLOW_SECURITY_RECORD XA_DISABLED
#endif

/*-------------------------------------------------------------------------
 * SDP_ALLOW_SKIP_EXTENDED_INFO constant
 *
 *     Controls whether L2CAP_ConnectRequest() will allow a request for
 *     SDP to proceed even though an outstanding request for L2CAP extended 
 *     info is waiting for a response. Normally, if L2CAP_ConnectRequest() 
 *     is called and there an outstanding L2CAP request for extended info 
 *     then the connectin request is placed on queue and is not sent until
 *     the extended info response is received. If SDP_ALLOW_SKIP_EXTENDED_INFO
 *     is set to XA_ENABLED and SDP_USE_BASIC_MODE is set to XA_ENABLED then 
 *     an L2CAP connect request for SDP will be allowed to proceed.
 *     
 *     By default SDP_ALLOW_SKIP_EXTENDED_INFO is disabled.
 */
#ifndef SDP_ALLOW_SKIP_EXTENDED_INFO
#define SDP_ALLOW_SKIP_EXTENDED_INFO XA_DISABLED
#endif

/****************************************************************************
 *
 * Section: A2ME Constants
 *
 ****************************************************************************/

/*-------------------------------------------------------------------------
 * A2ME_MAX_REMOTE_AMP_CONTROLLERS constant
 *
 *     Defines the maximum number of remote controllers that are stored
 *     in the remote controller list for Remote device.
 *
 *     The default value is 4 for 4 AMPs. The BR/EDR controller is not stored
 *     since it is required. This should be more than enough in most cases
 *     but if the list sent by the remote device is longer the stored list
 *     will be truncated. The maximum number of Remote AMP Controllers is 255.
 */
#ifndef A2ME_MAX_REMOTE_AMP_CONTROLLERS
#define A2ME_MAX_REMOTE_AMP_CONTROLLERS 4
#endif

/*-------------------------------------------------------------------------
 * A2ME_CACHE_REMOTE_AMP_INFO constant
 *
 *     If set to XA_ENABLED then AMP info for remote devices is cached
 *     otherwise it is not cached and every request for the AMP info for
 *     a remote AMP controller will require an A2MP request over the air.
 *
 *     The default is XA_ENABLED
 */
#ifndef A2ME_CACHE_REMOTE_AMP_INFO
#define A2ME_CACHE_REMOTE_AMP_INFO XA_ENABLED
#endif

/****************************************************************************
 *
 * Section: RFCOMM Constants
 *
 ****************************************************************************/


/*-------------------------------------------------------------------------
 * RFCOMM_PROTOCOL constant
 *
 *     Controls whether the RFCOMM protocol layer will be included.  Set
 *     this constant to XA_ENABLED when it is necessary to have RFCOMM
 *     compiled into your system.  This is necessary for all profiles based on
 *     the serial profile.
 *
 *     By default, RFCOMM is enabled.
 */
#ifndef RFCOMM_PROTOCOL
#define RFCOMM_PROTOCOL XA_ENABLED
#endif

#if RFCOMM_PROTOCOL == XA_ENABLED

/*-------------------------------------------------------------------------
 * RF_SECURITY constant
 *
 *     Controls whether RFCOMM protocol layer security is enabled.
 *     This allows an application to enforce different levels of
 *     security from the Bluetooth stack.  Level 2 security includes
 *     the ability to trigger security on an RFCOMM service ID.  If
 *     RF_SECURITY is disabled, then code to perform RFCOMM security
 *     checks is excluded.  RF_SECURITY allows an application to use
 *     stack security, but will not use RFCOMM level security. To do this
 *     the application will disable RF_SECURITY to  minimize code size.
 *
 *     By default, RF_SECURITY is enabled.
 */
#ifndef RF_SECURITY
#define RF_SECURITY XA_ENABLED /* BT_SECURITY */
#endif

/*-------------------------------------------------------------------------
 * RF_SEND_TEST constant
 *
 *     Controls whether the stack offers support for sending an RFCOMM
 *     test. When enabled, the RF_SendTest API can be used to test
 *     the connection with the remote device.
 *
 *     By default, sending RF tests is disabled.
 */
#ifndef RF_SEND_TEST
#define RF_SEND_TEST XA_DISABLED
#endif

/*-------------------------------------------------------------------------
 * RF_SEND_CONTROL constant
 *
 *     Controls whether RFCOMM can be used to send control channel
 *     information. This includes modem status, line status, and negotiation
 *     of remote port settings.
 *
 *     If RF_SEND_CONTROL is enabled, control channel information can
 *     be sent and processed by the application. This includes support for
 *     the following events and APIs.
 *
 *     Events: RFEVENT_MODEM_STATUS_IND, RFEVENT_MODEM_STATUS_CNF,
 *     RFEVENT_LINE_STATUS_IND, RFEVENT_LINE_STATUS_CNF,
 *     RFEVENT_PORT_STATUS_CNF, RFEVENT_PORT_NEG_CNF.
 *
 *     APIs: RF_SetModemStatus, RF_SetLineStatus, RF_RequestPortSettings,
 *     RF_AcceptPortSettings, RF_RequestPortStatus, and RF_SendPortStatus.
 *
 *     If RF_SEND_CONTROL is disabled, RFCOMM will handle control channel
 *     requests received from the remote device, but will not send control
 *     channel requests to the remote device. The APIs and Events listed
 *     above are disabled, resulting in significant code size savings.
 *
 *     By default, sending control channel information is enabled.
 */
#ifndef RF_SEND_CONTROL
#define RF_SEND_CONTROL XA_ENABLED
#endif

/*-------------------------------------------------------------------------
 * NUM_RF_SERVERS constant
 *
 *     Defines the number of RFCOMM services that can be registered with
 *     RF_RegisterServerChannel.
 *
 *     This value may range from 0 to 30. The default is 4.
 */
#ifndef NUM_RF_SERVERS
#define NUM_RF_SERVERS 4
#endif

#if NUM_RF_SERVERS >= 0
#if NUM_RF_SERVERS > 30
#error NUM_RF_SERVERS must not exceed 30
#endif
#else
#error NUM_RF_SERVERS must be 0 or more
#endif


/*-------------------------------------------------------------------------
 * NUM_RF_CHANNELS constant
 *
 *     Defines the maximum number of RFCOMM channels that can be
 *     established to each device. This includes both server and client
 *     channels.
 *
 *     The default number of channels is twice the number of local
 *     servers.
 */
#ifdef NUM_RF_CHANNELS

#if (NUM_RF_CHANNELS < NUM_RF_SERVERS) || (NUM_RF_CHANNELS > 60)
#error NUM_RF_CHANNELS value is invalid
#endif

#else

#if NUM_RF_SERVERS > 0
#define NUM_RF_CHANNELS (NUM_RF_SERVERS * 2)
#else
#define NUM_RF_CHANNELS 1
#endif

#endif

#if !(NUM_RF_CHANNELS > 0)
#error NUM_RF_CHANNELS must be greater than 0
#endif


/*-------------------------------------------------------------------------
 * RF_MAX_FRAME_SIZE constant
 *
 *     Defines the maximum frame size for an RFCOMM channel. All registered
 *     RfChannel structures must have a "maxFrameSize" equal to or less
 *     than this value.
 *
 *     This value may range between 127 bytes and 32767 bytes (or the
 *     L2CAP_MPS size minus 5, whichever is lower). The default maximum
 *     frame size is the L2CAP_MPS size, minus five bytes for RFCOMM
 *     protocol information.
 */
#ifndef RF_MAX_FRAME_SIZE
#if L2CAP_MTU > (32767+5)
#define RF_MAX_FRAME_SIZE 32767
#else
#define RF_MAX_FRAME_SIZE (L2CAP_MTU - 5)
#endif
#endif

#if RF_MAX_FRAME_SIZE > (L2CAP_MTU - 5)
#error RF_MAX_FRAME_SIZE cannot exceed the L2CAP_MTU - 5
#endif

/* The value tested here (23) must be the same as RF_MIN_FRAME_SIZE.  */
#if RF_MAX_FRAME_SIZE < 23
#error RF_MAX_FRAME_SIZE cannot be less than 23
#endif

#if RF_MAX_FRAME_SIZE > 32767
#error RF_MAX_FRAME_SIZE cannot exceed 32767
#endif


/*-------------------------------------------------------------------------
 * RF_CONNECT_TIMEOUT constant
 *
 *     Defines the amount of time to wait for a response to a connect
 *     request for an RFCOMM channel.  Because it is not possible to know
 *     whether authentication is required, this value must allow time for
 *     user interaction.
 *
 *     This value may range between 60000 and 300000 (60 sec and 300 sec).
 *     The default time is 60 seconds.
 */
#ifndef RF_CONNECT_TIMEOUT
#define RF_CONNECT_TIMEOUT 60000
#endif

#if RF_CONNECT_TIMEOUT < 60000
#error RF_CONNECT_TIMEOUT cannot be less than 60000
#endif

#if RF_CONNECT_TIMEOUT > 300000
#error RF_CONNECT_TIMEOUT cannot be greater than 300000
#endif

/*-------------------------------------------------------------------------
 * RF_T1_TIMEOUT constant
 *
 *     Defines the amount of time to wait for a response to an RFCOMM command.
 *     If a response is not received within the allotted time, the RFCOMM
 *     multiplexer session will be torn down.
 *
 *     This value may range between 10000 and 60000 (10 sec and 60 sec).
 *     The default time is 20 seconds.
 */
#ifndef RF_T1_TIMEOUT
#define RF_T1_TIMEOUT 20000
#endif

#if RF_T1_TIMEOUT < 10000
#error RF_T1_TIMEOUT cannot be less than 10000
#endif

#if RF_T1_TIMEOUT > 60000
#error RF_T1_TIMEOUT cannot be greater than 60000
#endif

/*-------------------------------------------------------------------------
 * RF_T2_TIMEOUT constant
 *
 *     Defines the amount of time to wait for a response to an RFCOMM
 *     control channel command.  If a response is not received within the
 *     allotted time, the RFCOMM multiplexer session will be torn down.
 *
 *     This value may range between 1000 and 60000 (1 sec and 60 sec).
 *     The default time is 20 seconds.
 */
#ifndef RF_T2_TIMEOUT
#define RF_T2_TIMEOUT 20000
#endif

#if RF_T2_TIMEOUT < 1000
#error RF_T2_TIMEOUT cannot be less than 1000
#endif

#if RF_T2_TIMEOUT > 60000
#error RF_T2_TIMEOUT cannot be greater than 60000
#endif

#endif /* RFCOMM_PROTOCOL == XA_ENABLED */


/****************************************************************************
 *
 * Section: Internal Flags
 *
 ****************************************************************************/

/* Internal flag for enable/disable of low level tests */
#ifndef JETTEST
#define JETTEST XA_DISABLED
#endif /* JETTEST */

/****************************************************************************
 *
 * Section: UnplugFest Testing
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * UPF_TWEAKS constant
 *
 *     This value controls the inclusion of code which enables certain
 *     behaviors for UnplugFest interoperability testing.  Certain UnplugFest
 *     tests require functionality or behaviors that may not be included in
 *     the Blue SDK.  Set this value to XA_ENABLED to include this code.
 *
 *     This constant should always be set to XA_DISABLED, except when
 *     performing UnplugFest tests.
 */
#ifndef UPF_TWEAKS
#define UPF_TWEAKS XA_DISABLED
#endif /* UPF_TWEAKS */


#endif /* __BTCONFIG_H */

