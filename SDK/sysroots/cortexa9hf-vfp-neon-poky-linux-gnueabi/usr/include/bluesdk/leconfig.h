/****************************************************************************
 *
 * File:
 *     leconfig.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Default constants for configuring the Bluetooth LE protocol stack.
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

#ifndef __LECONFIG_H
#define __LECONFIG_H

#include "btconfig.h"

/*---------------------------------------------------------------------------
 * Low Energy Configuration layer
 *
 *     The Low Energy Configuration layer provides types that configure the
 *     Low Energy layers.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 * LEL2CAP_MTU constant
 *
 *     Defines the recommended receivable LE L2CAP SDU size, in bytes when 
 *     the L2CAP user can receive SDUs in fragments. L2CAP does not base 
 *     buffers on this value so it is limited by the packet structure itself 
 *     and/or the buffer space of the L2CAP users. This limitation applies 
 *     only to packets received from the remote device. 
 *
 *     This value may range from 23 to 65533. The default is set to the
 *     largest size allowed. 
 *
 *     
 */
#ifndef LEL2CAP_MTU
#define LEL2CAP_MTU              65533  
#endif

#if LEL2CAP_MTU < 23 || LEL2CAP_MTU > 65533
#error LEL2CAP_MTU must be between 23 and 65533.
#endif

/*---------------------------------------------------------------------------
 * LEL2CAP_MPS constant
 *
 *     Defines the maximum receivable LE L2CAP PDU size, in bytes. An 
 *     LEL2CAP_MTU may be segmented into multiple LEL2CAP_MPS sized PDUs. 
 *
 *     This value may range from 23 to 65533.  The default value is 
 *     HCI_ACL_DATA_SIZE less bytes for LE L2CAP (4 or 6) and HCI headers (4).
 *
 */
#ifndef LEL2CAP_MPS
#define LEL2CAP_MPS                   (HCI_ACL_DATA_SIZE - 10)
#endif

#if LEL2CAP_MPS < 23
#error LEL2CAP_MPS must be at least 23
#endif

/*---------------------------------------------------------------------------
 * LEL2CAP_NUM_FIXED_CHANNELS constant
 *
 *     Defines the number of L2CAP fixed Channels for LE links
 */
#ifndef LEL2CAP_NUM_FIXED_CHANNELS
#define LEL2CAP_NUM_FIXED_CHANNELS    3
#endif

#if LEL2CAP_NUM_FIXED_CHANNELS < 3
#error LEL2CAP_NUM_FIXED_CHANNELS must be at least 3
#endif

/*---------------------------------------------------------------------------
 * LEL2CAP_TOT_NUM_FIXED_CHNLS constant
 *
 */
#ifndef LEL2CAP_TOT_NUM_FIXED_CHNLS
/* Need fixed channel memory for each device. */
#define LEL2CAP_TOT_NUM_FIXED_CHNLS   (LEL2CAP_NUM_FIXED_CHANNELS * NUM_LE_DEVICES)
#endif

/*---------------------------------------------------------------------------
 * LEL2CAP_NUM_SIGNAL_PACKETS constant
 *
 *     Defines the number of signaling packets allocated by L2CAP for LE.
 *
 *     By default, L2CAP allocates two packets per device plus 2
 *     packets per dynamic and fixed channel.
 */
#ifndef LEL2CAP_NUM_SIGNAL_PACKETS
#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
#define LEL2CAP_NUM_SIGNAL_PACKETS    ((NUM_LE_DEVICES * 2) + (LEL2CAP_TOT_NUM_DYNAMIC_CHNLS * 2) + (LEL2CAP_NUM_FIXED_CHANNELS * 2))
#else
#define LEL2CAP_NUM_SIGNAL_PACKETS    ((NUM_LE_DEVICES * 2) + (LEL2CAP_NUM_FIXED_CHANNELS * 2))
#endif
#endif

#if LEL2CAP_NUM_SIGNAL_PACKETS < 1
#error LEL2CAP_NUM_SIGNAL_PACKETS must be at least 1
#endif

/*---------------------------------------------------------------------------
 * LEL2CAP_NUM_TX_PACKETS constant
 *
 *     Defines the number of TX packets allocated by L2CAP for segmenting
 *     upper layer SDUs on LE Channels.
 *
 *     This value is based on information obtained from modeling at run time.
 *     A best estimate should be calculated from the maximum number
 *     of segments per SDU. If the channel sends only SDUs smaller than the 
 *     MPS value, then only one TX packet is required. If a channel sends 
 *     large SDUs, then it is likely it will require enough TX packets to 
 *     fill the max SDU.
 *
 *     This setting is the total number of TX packets in the entire system
 *     Remember to allocate enough TX packets for all LE channels.
 */
#ifndef LEL2CAP_NUM_TX_PACKETS
#define LEL2CAP_NUM_TX_PACKETS        (LEL2CAP_TOT_NUM_DYNAMIC_CHNLS * 7)
#endif

/*---------------------------------------------------------------------------
 * MIN_ADVERT_NAME_LEN constant
 *
 *     Defines the minimum number of bytes to be reserved for the device
 *     name when issuing Advertising or Scanning Response Data. If non-0,
 *     the local device name may be truncated to this size to allow sufficient
 *     room for other advertising/scanning data elements to be written. If 0,
 *     no space is reserved for the local device name, and other elements may
 *     crowd it out.
 */
#ifndef MIN_ADVERT_NAME_LEN
#define MIN_ADVERT_NAME_LEN       8
#endif

#if MIN_ADVERT_NAME_LEN > 29
#error MIN_ADVER_NAME_LEN cannot be greater than 29
#endif

/*---------------------------------------------------------------------------
 * LE_SECURITY_TIMEOUT constant
 *
 *     Specifies the number of seconds to elapse before a service-specific
 *     access request (see LESEC_AccessRequest) is automatically cancelled.
 *     The stack's security manager will cancel the request regardless of
 *     what is causing the delay. Cancelling the request will cause the
 *     security request to fail, which in most cases will prevent the
 *     requesting service from connecting.
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
#ifndef LE_SECURITY_TIMEOUT
#define LE_SECURITY_TIMEOUT 80
#endif /* LE_SECURITY_TIMEOUT */

 /*---------------------------------------------------------------------------
 * LE_REPEATED_ATTEMPTS_MIN_TIME constant
 *
 *     Specifies the minimum number of milliseconds to elapse before LE Security 
 *     will initiate a new Pairing Request command or Security Request command to 
 *     a remote device in which pairing failed, or before it will respond to a 
 *     Pairing Request command or Security Request command initiated by the failed
 *     remote device. This constant is part of LE security repeated attempts
 *     procedure.
 *
 *     When a pairing operation fails a timer is started and while the timer
 *     is active pairing attempts to the same remote device will fail. The
 *     timer value starts with LE_REPEATED_ATTEMPTS_MIN_TIME and is exponentially
 *     increased after each failed attempt until LE_REPEATED_ATTEMPTS_MAX_TIME
 *     is reached. Upon success the time is exponentially decreased until
 *     LE_REPEATED_ATTEMPTS_MIN_TIME is reached. Only one address is saved so
 *     pairing attempts to other devices will clear repeated attempts information.
 *
 *     To disable Repeated attempts set LE_REPEATED_ATTEMPTS_MIN_TIME to 0. This
 *     only recommended for testing purposes.
 *
 *     By default, this value is set to 2000 milliseconds (2 seconds). 
 */
#ifndef LE_REPEATED_ATTEMPTS_MIN_TIME
#define LE_REPEATED_ATTEMPTS_MIN_TIME 2000
#endif /* LE_REPEATED_ATTEMPTS_MIN_TIME */

 /*---------------------------------------------------------------------------
 * LE_REPEATED_ATTEMPTS_MAX_TIME constant
 *
 *     Specifies the maximum number of milliseconds to elapse before LE Security
 *     will initiate a new Pairing Request command or Security Request command to
 *     a remote device in which pairing failed, or before it will respond to a
 *     Pairing Request command or Security Request command initiated by the failed
 *     remote device. This constant is part of LE security repeated attempts
 *     procedure.
 *
 *     See LE_REPEATED_ATTEMPTS_MIN_TIME for a description of how this
 *     constant is used.  
 *
 *     By default, this value is set to 32000 milliseconds (32 seconds).
 */
#ifndef LE_REPEATED_ATTEMPTS_MAX_TIME
#define LE_REPEATED_ATTEMPTS_MAX_TIME 32000
#endif /* LE_REPEATED_ATTEMPTS_MAX_TIME */

#if LE_REPEATED_ATTEMPTS_MAX_TIME <= LE_REPEATED_ATTEMPTS_MIN_TIME
#error LE_REPEATED_ATTEMPTS_MAX_TIME must be greater than LE_REPEATED_ATTEMPTS_MIN_TIME
#endif

 /*---------------------------------------------------------------------------
 * LE_REPEATED_ATTEMPTS_NO_FAIL_TIME constant
 *
 *     Specifies the number of milliseconds to elapse of no pairing failures
 *     for LE Security to reduce the interval between attempts to a remote
 *     device in which pairing fails. The interval is decreased exponentially
 *     until it reaches LE_REPEATED_ATTEMPTS_MIN_TIME.
 *
 *     By default, this value is set to 20000 milliseconds (20 seconds).
 */
#ifndef LE_REPEATED_ATTEMPTS_NO_FAIL_TIME 
#define LE_REPEATED_ATTEMPTS_NO_FAIL_TIME  20000
#endif /* LE_REPEATED_ATTEMPTS_NO_FAIL_TIME */

#if LE_REPEATED_ATTEMPTS_NO_FAIL_TIME == 0
#error LE_REPEATED_ATTEMPTS_NO_FAIL_TIME must be greater than 0
#endif

 /*---------------------------------------------------------------------------
 * LE_REPEATED_ATTEMPTS_NUM_RECORDS constant
 *
 *     Specifies the number of remote devices that can be tracked for
 *     repeated attempts simultaneously. 
 *
 *     By default, this value is set to NUM_LE_DEVICES.
 */
#ifndef LE_REPEATED_ATTEMPTS_NUM_RECORDS 
#define LE_REPEATED_ATTEMPTS_NUM_RECORDS  NUM_LE_DEVICES
#endif /* LE_REPEATED_ATTEMPTS_NUM_RECORDS */

#if LE_REPEATED_ATTEMPTS_NUM_RECORDS < NUM_LE_DEVICES
#error LE_REPEATED_ATTEMPTS_NUM_RECORDS must be at least equal to NUM_LE_DEVICES
#endif

 /*---------------------------------------------------------------------------
 * LE_MAX_ID_ADDRESSES constant
 *
 *     Specifies the maximum number of LE ID addresses that can be generated.
 *     LE ID addresses are identity addresses of type BTADDR_LE_ID. These addresses
 *     are used for the identity address of remote device when the remote sends
 *     an identity address of 0 during LE pairing. This value is not used for
 *     memory allocation. It is just used to exist out of loop in case of an error.
 *
 *     By default, this value is set to 10000.
 */
#ifndef LE_MAX_ID_ADDRESSES 
#define LE_MAX_ID_ADDRESSES  10000
#endif /* LE_MAX_ID_ADDRESSES */

#if LE_MAX_ID_ADDRESSES < 50
#error LE_MAX_ID_ADDRESSES must be at least equal to 50
#endif

/*-------------------------------------------------------------------------
 * ATT_SERV_MAX_ATTRIB_VALUE_SIZE constant
 *
 *     Controls the amount of space allocated for attribute values in
 *     ATT and GATT server structures. The maximum attribute size allowed by
 *     the Bluetooth specification is 512 and is the default. If the GATT
 *     services on a device will never have attributes larger than a
 *     certain size then this constant can be made smaller to save RAM.
 *
 *     Default is 512 the maximum allowed by the Bluetooth specification.
 */
#ifndef ATT_SERV_MAX_ATTRIB_VALUE_SIZE
#define ATT_SERV_MAX_ATTRIB_VALUE_SIZE 512
#endif

#if ATT_SERV_MAX_ATTRIB_VALUE_SIZE > 512
#error ATT_MAX_ATTRIB_VALUE_SIZE cannot be greater than 512
#endif

#if ATT_SERV_MAX_ATTRIB_VALUE_SIZE < 1
#error ATT_MAX_ATTRIB_VALUE_SIZE cannot be less than 1
#endif

/*-------------------------------------------------------------------------
 * GATT_MAX_MTU constant
 *
 *     Controls the maximum size MTU that GATT will support for both LE 
 *     and BR/EDR links and is used when allocating buffers and
 *     storage space in structures such as GattClientToken used in
 *     both LE and BR/EDR.
 *
 *     Note: the MTU is negotiated and if the remote device proposes
 *     a smaller size it will be used during actual operation.
 *
 *     The default is set to 515 (512 + 3).
 *     512 is the maximum size of an attribute value and the MTU needs
 *     to be bigger so it can hold the attribute value plus overhead.
 *     For a write operation the overhead is 3 octets. For a read the
 *     overhead it is 1 octet so 3 is added to 512.
 */
#ifndef GATT_MAX_MTU
#define GATT_MAX_MTU 515
#endif

/* This value used when establishing an L2CAP connection over BR/EDR
 * and cannot be less than 48
 */
#if GATT_MAX_MTU < 48
#error GATT_MAX_MTU must be 48 or greater
#endif

/*-------------------------------------------------------------------------
 * GATT_MAX_MTU_LE constant
 *
 *     Controls the MTU that GATT will propose when negotiating MTU
 *     over an LE link.
 *
 *     Note: buffer and storage space sizes are controlled by GATT_MAX_MTU.
 *
 *     The default is set to GATT_MAX_MTU
 */
#ifndef GATT_MAX_MTU_LE
#define GATT_MAX_MTU_LE  GATT_MAX_MTU 
#endif

/* This value used when negotiating MTU over an LE link and it cannot
 * be less than 23
 */
#if GATT_MAX_MTU_LE < 23
#error GATT_MAX_MTU_LE must be 23 or greater
#endif

/*-------------------------------------------------------------------------
 * GATT_CACHE_INCLUDE_DEPTH constant
 *
 *     Controls the maximum depth of nested include declarations in a
 *     service. If the depth is exceeded then the an error is reported
 *     to the upper layer. Note that the code that checks for nested
 *     include declarations is recursive so the memory affected is the
 *     call stack.
 *
 *     The default is set to 10
 */
#ifndef GATT_CACHE_INCLUDE_DEPTH
#define GATT_CACHE_INCLUDE_DEPTH 10
#endif

/* Depth must be at least 2 */
#if GATT_CACHE_INCLUDE_DEPTH < 2
#error GATT_CACHE_INCLUDE_DEPTH must be at least 2
#endif

/*-------------------------------------------------------------------------
 * GATT_SERVER_MAX_SIGNED_WRITE constant
 *
 *     Specifies the maximum number of octets allowed for a signed write
 *     value. This excludes the 12 octet signature. This constant is used
 *     to allocate space to store the signed write value when a signed
 *     write request is received and the signature is being calculated.
 *     Signed write requests with values larger than this constant are
 *     ignored. It is up to the impementer to make sure that attributes that
 *     accept signed writes are not larger than this constant.
 *
 *     The default is set to GATT_MAX_MTU - 12
 */
#ifndef GATT_SERVER_MAX_SIGNED_WRITE
#define GATT_SERVER_MAX_SIGNED_WRITE (GATT_MAX_MTU - 12)
#endif

/*-------------------------------------------------------------------------
 * GATT_SERVER_INITIATOR_TIMEOUT constant
 *
 *     Specifies the number of milliseconds the GATT server will wait after
 *     sending a response to a remote GATT client before determining the
 *     the GATT connection should be disconnected. This is the default value 
 *     used when the local device is the initiator of the connection. The
 *     value can be changed by calling GATTS_SetIdleTimerValues()
 *
 *     This timeout is only used in the situation where both the local GATT
 *     server and client are involved in communications with their remote
 *     counterparts and it is difficult to determine when the GATT
 *     connection should be disconnected.
 *
 *     The default is set to 10000 (10 seconds)
 */
#ifndef GATT_SERVER_INITIATOR_TIMEOUT
#define GATT_SERVER_INITIATOR_TIMEOUT  10000
#endif

/*-------------------------------------------------------------------------
 * GATT_SERVER_RESPONDER_TIMEOUT constant
 *
 *     Specifies the number of milliseconds the GATT server will wait after
 *     sending a response to a remote GATT client before determining the
 *     the GATT connection should be disconnected. This is the default value 
 *     used when the remote device is the initiator of the connection. The
 *     value can be changed by calling GATTS_SetIdleTimerValues().
 *
 *     This timeout is only used in the situation where both the local GATT
 *     server and client are involved in communications with their remote
 *     counterparts and it is difficult to determine when the GATT
 *     connection should be disconnected.
 *
 *     The default is set to 30000 (30 seconds)
 */
#ifndef GATT_SERVER_RESPONDER_TIMEOUT
#define GATT_SERVER_RESPONDER_TIMEOUT  30000
#endif

/*-------------------------------------------------------------------------
 * GATT_SERVER_NUM_PREPARED_WRITE constant
 *
 *     Specifies the number of GattServerPrepWrite structures allocated
 *     per remote device. These structures are used to hold Prepare
 *     Write requests.
 *
 *     The default is set 30. This allows a maximum size attribute value to
 *     be written when the MTU size is the minimum size of 23 octets.
 *
 */
#ifndef GATT_SERVER_NUM_PREPARED_WRITE
#define GATT_SERVER_NUM_PREPARED_WRITE  30
#endif

/*-------------------------------------------------------------------------
 * GATT_SERVER_PREP_WRITE_BUFF_SIZE constant
 *
 *     Specifies the size of the buffer used to hold Prepare Write values
 *     per remote device.
 *
 *     The default is set to ATT_SERV_MAX_ATTRIB_VALUE_SIZE. This allows a
 *     maximum size attribute value to be written.
 *
 */
#ifndef GATT_SERVER_PREP_WRITE_BUFF_SIZE
#define GATT_SERVER_PREP_WRITE_BUFF_SIZE  ATT_SERV_MAX_ATTRIB_VALUE_SIZE
#endif

/*-------------------------------------------------------------------------
 * GATT_CLIENT_DELAY constant
 *
 *     Specifies the number of milliseconds the GATT client will wait after
 *     receiving the GATT connection open event before issuing it to the
 *     upper layer when the remote GATT service is cached and a cache
 *     change indication may occur.
 *
 *     The default is set to 1000 (1 second)
 */
#ifndef GATT_CLIENT_DELAY
#define GATT_CLIENT_DELAY   1000
#endif

/*-------------------------------------------------------------------------
 * LE_PRIVACY constant
 *
 *     Specifies whether code for the LE privacy feature is included or not.
 *     By default LE privacy is not included.
 *
 *     The default is XA_DISABLED
 */
#ifndef LE_PRIVACY
#define LE_PRIVACY         XA_DISABLED
#endif

#if (LE_PRIVACY == XA_ENABLED) && (NUM_LE_DEVICES == 0)
#error LE_PRIVACY cannot be enabled unless NUM_LE_DEVICES is > 0
#endif

/*-------------------------------------------------------------------------
 * LL_PRIVACY constant
 *
 *     Specifies whether code for the Link Layer privacy feature is 
 *     included or not. By default LL privacy is not included.
 *
 *     The default is XA_DISABLED
 */
#ifndef LL_PRIVACY
#define LL_PRIVACY         XA_DISABLED
#endif

#if (LE_PRIVACY == XA_DISABLED) && (LL_PRIVACY == XA_ENABLED)
#error LL_PRIVACY cannot be enabled unless LE_PRIVACY is enabled
#endif

#if (LL_PRIVACY == XA_ENABLED) && (NUM_LE_DEVICES == 0)
#error LL_PRIVACY cannot be enabled unless NUM_LE_DEVICES is > 0
#endif

/*-------------------------------------------------------------------------
 * LE_GATT_SUPPORT_SDP constant
 *
 *     Specifies whether code that adds SDP records for the GATT and GAP
 *     services are included or not. 
 *
 *     The default is XA_ENABLED
 */
#ifndef LE_GATT_SUPPORT_SDP
#define LE_GATT_SUPPORT_SDP         XA_ENABLED
#endif

/*-------------------------------------------------------------------------
 * LE_PROXIMITY_SUPPORT_SDP constant
 *
 *     Specifies whether code for adding SDP records for the LE Proximity 
 *     Profile is included or not. Including SDP code is for testing 
 *     Proximity/FindMe over BR/EDR links.
 *
 *     The default is XA_DISABLED
 */
#ifndef LE_PROXIMITY_SUPPORT_SDP
#define LE_PROXIMITY_SUPPORT_SDP         XA_DISABLED
#endif

/*-------------------------------------------------------------------------
 * LE_HEALTH_THERMOMETER_SUPPORT_SDP constant
 *
 *     Specifies whether code for adding SDP records for the LE Health 
 *     Thermometer Profile is included or not. Including SDP code is for 
 *     testing Health Thermometer over BR/EDR links.
 *
 *     The default is XA_DISABLED
 */
#ifndef LE_HEALTH_THERMOMETER_SUPPORT_SDP
#define LE_HEALTH_THERMOMETER_SUPPORT_SDP         XA_DISABLED
#endif

/*-------------------------------------------------------------------------
 * LE_HEART_RATE_SUPPORT_SDP constant
 *
 *     Specifies whether code for adding SDP records for the LE Heart Rate 
 *     Profile is included or not. Including SDP code is for testing 
 *     Heart Rate over BR/EDR links.
 *
 *     The default is XA_DISABLED
 */
#ifndef LE_HEART_RATE_SUPPORT_SDP
#define LE_HEART_RATE_SUPPORT_SDP         XA_DISABLED
#endif


#endif /* __LECONFIG_H */
