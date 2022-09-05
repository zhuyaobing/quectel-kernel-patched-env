/****************************************************************************
 *
 * File:
 *     mele.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     upper layer of the LE portion of the Management Entity.
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

#ifndef __MELE_H
#define __MELE_H

#include "osapi.h"
#include "bttypes2.h"
#include "leconfig.h"

/*---------------------------------------------------------------------------
 * Management Entity for LE (MELE) layer
 *
 *     The Management Entity for LE allows applications to inquire for
 *     remote LE devices, establish different types of LE connections, and
 *     control the access to this device by other devices.
 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * LE_MAX_DATA_OCTETS constant
 *
 *     Maximum data length in octets for the Bluetooth controller allowed by 
 *     the LE specification.
 *
 */
#define LE_MAX_DATA_OCTETS     251


/*---------------------------------------------------------------------------
 * LE_MAX_DATA_TIME constant
 *
 *     Maximum time in milleseconds for sending data by the Bluetooth
 *     controller allowed by the LE specification.
 *
 */
#define LE_MAX_DATA_TIME      2120

/*---------------------------------------------------------------------------
 * LE_MIN_DATA_OCTETS constant
 *
 *     Minimum data length in octets for the Bluetooth controller allowed by 
 *     the LE specification.
 *
 */
#define LE_MIN_DATA_OCTETS     27


/*---------------------------------------------------------------------------
 * LE_MIN_DATA_TIME constant
 *
 *     Maximum time in milleseconds for sending data by the Bluetooth sontroller
 *     allowed by the LE specification.
 *
 */
#define LE_MIN_DATA_TIME      328

#if LL_PRIVACY == XA_ENABLED
/*---------------------------------------------------------------------------
 * LL_PRIVACY_MAX_RPA_TIMEOUT constant
 *      Maximum value for the controller's timeout for changing resolvable
 *      private addresses. (in seconds)
 */
#define LL_PRIVACY_MAX_RPA_TIMEOUT 0xA1B8
#endif /* LL_PRIVACY == XA_ENABLED */

#define LE_ADVERT_REPORT_LENGTH        10
#define LE_DIRECT_ADVERT_REPORT_LENGTH 16

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtLeRadioFeature type
 *
 *     Codes for LE radio features as defined in LE LMP.
 */
typedef U8 BtLeRadioFeature;

#define BLRF_ENCRYPTION                     0   /* LE Encryption (4.0) */
#define BLRF_CONNECTION_PARMS_REQ           1   /* Connection Parameters Request Procedure (4.1) */
#define BLRF_EXTENDED_REJECT_IND            2   /* Extended Reject Indication (4.1) */
#define BLRF_SLAVE_INITIATED_FEATURE_EXCH   3   /* Slave-Initiated Features Exchange (4.1) */  
#define BLRF_PING                           4   /* LE Ping (4.1) */
#define BLRF_DATA_LENGTH_EXT                5   /* LE Data Packet Length Extension (4.2) */
#define BLRF_LINK_LAYER_PRIVACY             6   /* LE Link Layer Privacy (4.2) */
#define BLRF_EXT_SCANNER_FILTER_POLICY      7   /* Extended Scanner Filter Policies (4.2) */

/* End of BtLeRadioFeature */

/*---------------------------------------------------------------------------
 * BtLeRadioState type
 *
 *     Codes for LE radio states as defined in LE LMP.
 */
typedef U8 BtLeRadioState;

/* Non-connectable advertising state */
#define BLRS_NON_CONNECTABLE_ADVERT      0

/* Scannable advertising state */
#define BLRS_SCANNABLE_ADVERT            1

/* Connectable advertising state */
#define BLRS_CONNECTABLE_ADVERT          2

/* Directed Advertising state */
#define BLRS_DIRECTED_ADVERT             3

/* Passive Scanning state */
#define BLRS_PASSIVE_SCANNING            4

/* Active Scanning state */
#define BLRS_ACTIVE_SCANNING             5

/* Initiating state, Connection state in master */
#define BLRS_INITIATING                  6

/* Connection state in the slave role */
#define BLRS_CONNECTION_SLAVE            7

/* Non-connectable advertising and passive scanning combined */
#define BLRS_NON_CON_ADVERT_PASS_SCAN    8

/* DiscoverableScannable Advertising State and Passive Scanning State combination */
#define BLRS_SCAN_ADVERT_PASS_SCAN       9

/* Connectable Advertising State and Passive Scanning State combination */
#define BLRS_CON_ADVERT_PASS_SCAN       10

/* Directed Advertising State and Passive Scanning State combination */
#define BLRS_DIRECT_ADVERT_PASS_SCAN    11

/* Non-connectable Advertising State and Active Scanning State */
#define BLRS_NON_CON_ADVERT_ACTIVE_SCAN 12

/* Scannable Advertising State and Active Scanning State combination */
#define BLRS_SCAN_ADVERT_ACTIVE_SCAN    13

/* Connectable Advertising State and Active Scanning State combination */
#define BLRS_CON_ADVERT_ACTIVE_SCAN     14

/* Directed Advertising State and Active Scanning State combination */
#define BLRS_DIRECT_ADVERT_ACTIVE_SCAN  15

/* Non-connectable Advertising State and Initiating State combination */
#define BLRS_NON_CON_ADVERT_INITIATING  16

/* Scannable Advertising State and Initiating State combination supported */
#define BLRS_SCAN_ADVERT_INITIATING     17

/* Non-connectable Advertising State and Master Role combination */
#define BLRS_NON_CON_ADVERT_MASTER      18

/* Scannable Advertising State and Master Role combination */
#define BLRS_SCAN_ADVERT_MASTER         19

/* Non-connectable Advertising State and Slave Role combination */
#define BLRS_NON_CON_ADVERT_SLAVE       20

/* Scannable Advertising State and Slave Role combination */
#define BLRS_SCAN_ADVERT_SLAVE          21

/* Passive Scanning State and Initiating State combination */
#define BLRS_PASSIVE_SCAN_INITIATING    22

/* Active Scanning State and Initiating State combination */
#define BLRS_ACTIVE_SCAN_INITIATING     23

/* Passive Scanning State and Master Role combination */
#define BLRS_PASSIVE_SCAN_MASTER        24

/* Active Scanning State and Master Role combination */
#define BLRS_ACTIVE_SCAN_MASTER         25

/* Passive Scanning state and Slave Role combination */
#define BLRS_PASSIVE_SCAN_SLAVE         26

/* Active Scanning state and Slave Role combination */
#define BLRS_ACTIVE_SCAN_SLAVE          27

/* Initiating State and Master Role combination. Master Role and Master
 * Role combination is also supported
 */
#define BLRS_INITIATING_MASTER          28

/* End of BtLeRadioState */

/*---------------------------------------------------------------------------
 * MeLeEventType type
 *
 *     The LE Management Entity types are passed to all registered handlers
 *     based on the eMask. Handlers are registered using MELE_Register(). The
 *     event types are defined below. They are offsets from BTE_TYPE_BASE_MELE
 *     defined in bttypes2.h
 */
typedef BtEventTypeEx MeLeEventType;

/* Group: Events sent to all handlers registered with the MELE. These can be
 * masked out if  desired.
 */

/* The LE Advertising parameters have been set. "e.status" indicates the
 * success or failure of the operation and "errCode" reports the result of the
 * operation as reported from HCI.
 */
#define MELEE_SET_ADVERT_PARMS_CNF          (BTE_TYPE_BASE_MELE + 0)

/* The LE Advertising data have been set. "e.status" indicates the
 * success or failure of the operation and "errCode" reports the result of
 * the operation as reported from HCI.
 */
#define MELEE_SET_ADVERT_DATA_CNF           (BTE_TYPE_BASE_MELE + 1)

/* The LE Scan response data have been set. "e.status" indicates the success
 * or failure of the operation and "errCode" reports the result of the
 * operation as reported from HCI.
 */
#define MELEE_SET_SCAN_RSP_DATA_CNF         (BTE_TYPE_BASE_MELE + 2)

/* The LE Scan parameters have been set. "e.status" indicates the success or
 * failure of the operation and "errCode" reports the result of the
 * operation as reported from HCI.
 */
#define MELEE_SET_SCAN_PARMS_CNF            (BTE_TYPE_BASE_MELE + 3)

/* Set Advertising Enable operation has completed. "e.status" indicates
 * the success or failure of the operation and "errCode" reports the result
 * of the operation as reported from HCI.
 */
#define MELEE_SET_ADVERT_ENABLE_CNF         (BTE_TYPE_BASE_MELE + 4)

/* Set Scan Enable operation has completed. "e.status" indicates
 * the success or failure of the operation and "errCode" reports the result
 * of the operation as reported from HCI.
 */
#define MELEE_SET_SCAN_ENABLE_CNF           (BTE_TYPE_BASE_MELE + 5)

/* An LE link connection operation has completed. This event is passed to
 * both the initiator and the responder. "e.status" indicates the success
 * or failure of the operation and "errCode" reports the result of the
 * operation as reported from HCI. "bdAddr" and "identityAddr" identifies
 * the remote device involved in the link. "p.link" provides some
 * connection parameters if link create is successful. "p.link.role"
 * indicates who initiated the connection. In LE the master is the
 * initiator and the slave is the responder.
 */
#define MELEE_LINK_CONNECT_CNF              (BTE_TYPE_BASE_MELE + 6)

/* Advertising reports indicate that a Bluetooth device or multiple
 * devices have responded to an active scan or received some information
 * during a passive scan. "bdAddr" identify
 * the remote device and p.advertReport provides additional
 * advertising information.
 */
#define MELEE_ADVERT_REPORT                 (BTE_TYPE_BASE_MELE + 7)

/* An LE link connection was disconnected. "p.discStatus" is valid, and
 * "errCode" may be valid (see documentation for p.discStatus for
 * details. "bdAddr"  identifies the remote device.
 * "e.status" is reflects "errCode" indicating if "errCode" is has an
 * error or not so it may not be very useful.
 */
#define MELEE_LINK_DISCONNECT               (BTE_TYPE_BASE_MELE + 8)

/* An outgoing create LE link has been canceled. "bdAddr" and "identityAddr"
 * identify the remote device. "p.remDev" contains a pointer to the
 * remote device structure. "errCode" indicates success or failure
 * of the operation.
 */
#define MELEE_LINK_CREATE_CANCEL            (BTE_TYPE_BASE_MELE + 9)


/* The LE controller's white list has been cleared. "errCode" and "e.status"
 * indicate success or failure
 */
#define MELEE_CLEAR_WHITE_LIST              (BTE_TYPE_BASE_MELE + 10)

/* An Item has been added to the LE controller's white list. "errCode" and
 * "e.status" indicate success or failure. "bdAddr" indicates the address
 * that was attempted to be added.
 */
#define MELEE_ADD_WHITE_LIST                (BTE_TYPE_BASE_MELE + 11)

/* An Item has been removed from the LE controller's white list. "errCode" and
 * "e.status" indicate success or failure. "bdAddr" indicates
 * the address that was attempted to be removed.
 */
#define MELEE_REMOVE_WHITE_LIST             (BTE_TYPE_BASE_MELE + 12)

/* An LE link's parameters have been updated. "errCode" and
 * "e.status" indicate success or failure. "bdAddr" and "identityAddr" identifies
 * the link that was updated. "p.link" contains the current link values.
 * "p.link.connInterval", "p.link.connLatency", and "p.link.superTimeout" will
 * have new values if the update was successful.
 */
#define MELEE_UPDATE_LINK_CNF               (BTE_TYPE_BASE_MELE + 13)

/* An LE link's channel map has been read. "errCode" and "e.status" indicate
 * success or failure. "bdAddr" and "identityAdr" identifies the link
 * whose channel map was read. "p.channelMap" contains the current channel
 * map if the operation is successful.
 */
#define MELEE_READ_CHANNEL_MAP              (BTE_TYPE_BASE_MELE + 14)

/* A Start Encryption operation has completed. "errCode" and "e.status" indicate
 * success or failure. "bdAddr" and "identityAddr" identify the link on which
 * the Start Encryption operation was performed.
 */
#define MELEE_START_ENCRYPT_COMPLETE        (BTE_TYPE_BASE_MELE + 15)

/* A Set Channel Classification operation has completed. "errCode" and
 * "e.status" indicate success or failure. "p.channelMap" contains the
 * channel map that was sent to the LE controller.
 */
#define MELEE_SET_CHAN_CLASS_COMPLETE       (BTE_TYPE_BASE_MELE + 16)

/* A Set Random Address operation has completed. "errCode" and
 * "e.status" indicate success or failure. "p.randomAddr" contains the
 * random address that was sent to the controller.
 */
#define MELEE_SET_RANDOM_ADDR_COMPLETE      (BTE_TYPE_BASE_MELE + 17)

/* Group: Events sent to the single handler passed to the function
 * whose result is passed via the event.
 */

/* A Rand operation has completed. "errCode" and "e.status" indicate
 * the success or failure. "p.rand" contains the requested random
 * number if the operation was successful.
 */
#define MELEE_RAND_COMPLETE                 (BTE_TYPE_BASE_MELE + 18)

/* An Encrypt operation had completed. "errCode" and "e.status" indicate
 * the success or failure. "p.encryptedData" contains the encrypted data
 * block if the operation was successful).
 */
#define MELEE_ENCRYPT_COMPLETE              (BTE_TYPE_BASE_MELE + 19)

/* Starting an LE receiver test has completed. "errCode" and "e.status" indicate
 * the success or failure.
 */
#define MELEE_RX_TEST_COMPLETE              (BTE_TYPE_BASE_MELE + 20)

/* Starting an LE transmitter test has completed. "errCode" and "e.status" indicate
 * the success or failure.
 */
#define MELEE_TX_TEST_COMPLETE              (BTE_TYPE_BASE_MELE + 21)

/* Ending a test has completed. "errCode" and "e.status" indicate  the success
 * or failure. The p.numTestPacketsRx contains the number of test packets
 * received for a receiver test and 0 for a transmitter test.
 */
#define MELEE_TEST_END_COMPLETE             (BTE_TYPE_BASE_MELE + 22)

/* Encryption settings have changed. "p.encrypt", "errCode" and
 * "e.status" are valid.
 */
#define MELEE_ENCRYPTION_CHANGE             (BTE_TYPE_BASE_MELE + 23)

/* Indicates that an authentication attempt has completed. "p.remDev",
 * "errCode" and "e.status" are valid.
 */
#define MELEE_AUTHENTICATED                 (BTE_TYPE_BASE_MELE + 24)

/* An LE remote features operation has completed. This event is passed to
 * the initiator of the operation. "e.status" indicates the success or
 * failure of the operation and "errCode" reports the result of the
 * operation as reported from HCI. "p.features" contains the requested
 * remote features and the remote device, if the operation was successful.
 * In LE the master is the initiator and the slave is the responder.
 */
#define MELEE_REMOTE_FEATURES               (BTE_TYPE_BASE_MELE + 25)

/* Information relevant to the Advertising Data Response (and the
 * associated Scan Data Response) has changed, and the Advertising Data
 * may need to be updated.  This includes changes to the advertised LE
 * services, the local device name, and vendor specific information.
 * The Advertising Data may be updated by calling MELE_SetAdvertData().
 */
#define MELEE_UPDATE_ADVERT_DATA_REQ        (BTE_TYPE_BASE_MELE + 26)

/* Information relevant to the GAP Service Query has changed and
 * may need to be updated.  This includes changes to the Device                                      .
 * Name, Appearance, Peripheral Privacy Flag, Reconnection Address,
 * and Peripheral Preferred Connection Parameters characteristics.
 */
#define MELEE_UPDATE_GAP_SERVICE_REQ        (BTE_TYPE_BASE_MELE + 27)

/* A remote device has requested that the LE link's parameters be updated. 
 * This event is only sent to the handler registered via the
 * MELE_RegisterUpdateLinkHandler() function. The stack will wait to send
 * a response until MELE_AcceptLinkUpdateReq() is called. The p.updateLinkReq
 * field contains the requested link parameters from the slave and the bdAddr
 * and identityAddr fields contains the device addresses of the remote slave. 
 */
#define MELEE_UPDATE_LINK_REQ               (BTE_TYPE_BASE_MELE + 28)

/* The setting of the maximum TX packet size and maximum TX transmission time 
 * for an LE link has completed. The "e.status" indicates the success
 * or failure of the operation and "errCode" reports the result of the
 * operation as reported from HCI. "bdAddr" and "identityAddr"  identifies
 * the remote device involved in the link. The "p.remDev" field
 * provides a pointer to the remote device structure.
 */
#define MELEE_DATA_LENGTH_COMPLETE          (BTE_TYPE_BASE_MELE + 29)

/* Reading the local P-256 Public Key has completed. "errCode" and 
 * "e.status" indicate the success or failure. The "p.P256PublicKey"
 * contains the public key.
 */
#define MELEE_P256_KEY_COMPLETE             (BTE_TYPE_BASE_MELE + 30)

/* Generating a Diffie Hellman Key has completed. "errCode" and 
 * "e.status" indicate the success or failure. The "p.DHKey"
 * contains the key.
 */
#define MELEE_GEN_DHKEY_COMPLETE            (BTE_TYPE_BASE_MELE + 31)

/* An authenticated payload timeout event has occurred.  This signifies
 * that the defined maximum amount of time between receiving a packet with 
 * a valid Message Integrity Check (MIC) has been exceeded. A Host may 
 * choose to disconnect the link when this occurs. "p.remDev" is valid.
 */
#define MELEE_AUTH_PAYLOAD_TIMEOUT          (BTE_TYPE_BASE_MELE + 32)

/* The maximum payload lengths and/or maximum transmission times of Data
 * channel PDUs in either direction have changed for the given link.
 * "bdAddr" and "identityAddr"  identifies the remote device involved 
 * in the link. The "p.lenChange" field provides the maximum values
 * that will actually be used on the link following the change.
 */
#define MELEE_DATA_LENGTH_CHANGE            (BTE_TYPE_BASE_MELE + 33)

/* The setting of the default maximum TX packet size and maximum TX 
 * transmission time used by the controller for new connections has
 * has completed. The "e.status" indicates the success or failure of 
 * the operation and "errCode" reports the result of the operation 
 * as reported from HCI. 
 */
#define MELEE_DEFAULT_DATA_LENGTH_COMPLETE  (BTE_TYPE_BASE_MELE + 34)

/* An Item has been added to the LE controller's resolve list. "errCode" and
 * "e.status" indicate success or failure. "bdAddr" and "addrType" indicate
 * the address that was attempted to be added. "p.irkPair.peerIrk" and
 * "p.irkPair.localIrk" are valid.
 */
#define MELEE_ADD_RESOLVING_LIST_CNF        (BTE_TYPE_BASE_MELE + 35)

/* An Item has been removed from the LE controller's resolve list. "errCode" and
 * "e.status" indicate success or failure. "bdAddr" and "addrType" indicate
 * the address that was attempted to be removed.
 */
#define MELEE_REMOVE_RESOLVING_LIST_CNF     (BTE_TYPE_BASE_MELE + 36)

/* Clear resolve list command was processed by the LE controller. "errCode" and
 * "e.status" indicate success or failure.
 */
#define MELEE_CLEAR_RESOLVING_LIST_CNF      (BTE_TYPE_BASE_MELE + 37)

/* Read peer resolvable private address command process by LE controller.
 * "errCode" and "e.status" indicate success or failure. "identityAddr" 
 * is valid.
 */
#define MELEE_READ_RESOLVE_ADDR_COMPLETE    (BTE_TYPE_BASE_MELE + 38)

/* Address resolution was enabled/disabled on the LE controller. "errCode" 
 * and "e.status" indicate success or failure.
 */
#define MELEE_ENABLE_ADDR_RESOLUTION_CNF    (BTE_TYPE_BASE_MELE + 39)

/* Set resolvable private address timeout handled by the LE controller. 
 * "errCode" and "e.status" indicate success or failure.
 */
#define MELEE_SET_RESOLVE_ADDR_TIMEOUT_CNF  (BTE_TYPE_BASE_MELE + 40)

/* Direct advertising reports indicate that a Bluetooth device or multiple
 * devices have sent ADV_DIRECT_IND PDUs and the following conditions are met:
 * 1. Advertiser is using a resolvable private address
 * 2. Scanning Filter Policy is set to 0x02 or 0x03
 * 3. The address could not be resolved from the entries in the controller's
 *    resolving list
 * "bdAddr" and "addrType" identify the remote device and p.randomAddr 
 * provides the address to which the advertising message is directed.
 */
#define MELEE_DIRECT_ADVERT_REPORT          (BTE_TYPE_BASE_MELE + 41)

/* End of MeLeEventType */

/*---------------------------------------------------------------------------
 * MeLeEventMask type
 *
 *     The LE Management Entity event mask is defined below. It is used to
 *     mask out undesired events. To receive all events set the event mask to
 *     MELEE_MASK_ALL. To ignore an event AND out the event from MELEE_MASK_ALL
 *     as shown in the example below.
 *
 *     mask = MELEE_MASK_ALL & ~MELEE_MASK_SET_ADVERT_PARMS_CNF;
 *
 *     Create a mask with only desired events by ORing together the values for the
 *     desired events.
 */

typedef U32 MeLeEventMask;

#define MELEE_MASK_SET_ADVERT_PARMS_CNF          0x00000001
#define MELEE_MASK_SET_ADVERT_DATA_CNF           0x00000002
#define MELEE_MASK_SET_SCAN_RSP_DATA_CNF         0x00000004
#define MELEE_MASK_SET_SCAN_PARMS_CNF            0x00000008
#define MELEE_MASK_SET_ADVERT_ENABLE_CNF         0x00000010
#define MELEE_MASK_SET_SCAN_ENABLE_CNF           0x00000020
#define MELEE_MASK_LINK_CONNECT_CNF              0x00000040
#define MELEE_MASK_ADVERT_REPORT                 0x00000080
#define MELEE_MASK_LINK_DISCONNECT               0x00000100
#define MELEE_MASK_LINK_CREATE_CANCEL            0x00000200
#define MELEE_MASK_CLEAR_WHITE_LIST              0x00000400
#define MELEE_MASK_ADD_WHITE_LIST                0x00000800
#define MELEE_MASK_REMOVE_WHITE_LIST             0x00001000
#define MELEE_MASK_UPDATE_LINK_CNF               0x00002000
#define MELEE_MASK_READ_CHANNEL_MAP              0x00004000
#define MELEE_MASK_START_ENCRYPT_COMPLETE        0x00008000
#define MELEE_MASK_SET_CHAN_CLASS_COMPLETE       0x00010000
#define MELEE_MASK_SET_RANDOM_ADDR_COMPLETE      0x00020000
#define MELEE_MASK_ENCRYPTION_CHANGE             0x00040000
#define MELEE_MASK_AUTHENTICATED                 0x00080000
#define MELEE_MASK_REMOTE_FEATURES               0x00100000
#define MELEE_MASK_UPDATE_ADVERT_DATA            0x00200000
#define MELEE_MASK_UPDATE_GAP_SERVICE            0x00400000
#define MELEE_MASK_UPDATE_LINK_REQ               0x00800000
#define MELEE_MASK_DATA_LENGTH_COMPLETE          0x01000000
#define MELEE_MASK_AUTH_PAYLOAD_TIMEOUT          0x02000000
#define MELEE_MASK_DATA_LENGTH_CHANGE            0x04000000
#define MELEE_MASK_DEFAULT_DATA_LENGTH_COMPLETE  0x08000000
#define MELEE_MASK_RESOLVING_LIST_CNF            0x10000000
#define MELEE_MASK_SET_ADDR_RES_ENABLE           0x20000000
#define MELEE_MASK_SET_RESOLVE_ADDR_TIMEOUT      0x40000000
#define MELEE_MASK_ALL                           0xFFFFFFFF

/* End of MeLeEventMask */

/*---------------------------------------------------------------------------
 * MeLeAdvertType type
 *
 *     The Advertizing Type is used to determine the packet type that is used
 *     for LE advertising when advertising is enabled.
 */
typedef U8 MeLeAdvertType;

#define MLAT_CONNECTABLE_UNDIRECTED    0x00  /* ADV_IND (default) */
#define MLAT_CONNECTABLE_DIRECTED      0x01  /* ADV_DIRECT_IND (high-duty) */
#define MLAT_DISCOVERABLE_UNDIRECTED   0x02  /* ADV_DISCOVER_IND */
#define MLAT_NONCONNECTABLE_UNDIRECTED 0x03  /* ADV_NONCONN_IND */
#define MLAT_CONNECTABLE_LD_DIRECTED   0x04  /* ADV_DIRECT_IND (low-duty) */

/* End of MeLeAdvertType */

/*---------------------------------------------------------------------------
 * MeLeAddressType type
 *
 *     The Address Type specifies the type of address used in the advertising
 *     packets.
 */
typedef U8 MeLeAddressType;

#define MLADDR_PUBLIC       0x00  /* Public device address (default) */
#define MLADDR_RANDOM       0x01  /* Random device address */
#define MLADDR_PUBLIC_ID    0x02  /* Public identity address (corresponds to RPA) */
#define MLADDR_RANDOM_ID    0x03  /* Random identity address (corresponds to RPA) */
#define MLADDR_INVALID      0xFF  /* Invalid Address */

/* The upper layers do not need to know details of where address came from
 * This operation will effectively mask out the 2nd bit.
 * 0x02 --> 0x00 (MLADDR_PUBLIC_ID --> BTADDR_PUBLIC)
 * 0x03 --> 0x01 (MLADDR_RANDOM_ID --> BTADDR_RANDOM)
 */
#define MELEADDRTYPE_TO_BTADDRTYPE(a)       (a & BTADDR_RANDOM)  /* */

/* End of MeLeAddressType */

/*---------------------------------------------------------------------------
 * MeLeAdvertChanMap type
 *
 *     The Advertising Channel Map is a bit field that indicates the
 *     advertising channels that shall be used when transmitting advertising
 *     packets. At least one channel bit shall be set in the Advertising
 *     Channel Map parameter. The Channel Map is created by ORing together
 *     the desired channel values.
 */
typedef U8 MeLeAdvertChanMap;

#define MLACM_CHAN_37  0x01
#define MLACM_CHAN_38  0x02
#define MLACM_CHAN_39  0x04
#define MLACM_ALL     (MLACM_CHAN_37 | MLACM_CHAN_38 | MLACM_CHAN_39) /* Default */

/* End of MeLeAdvertChanMap */

/*---------------------------------------------------------------------------
 * MeLeAdvertFilterPolicy type
 *
 *     The Advertising Filter Policy type specifies the scan request and
 *     connect request packets that are accepted when advertising is enabled.
 *     It is ignored when directed advertising (MLAT_CONNECTABLE_DIRECTED) is
 *     enabled.
 */
typedef U8 MeLeAdvertFilterPolicy;

/* Allow Scan Request from Any, Allow Connect Request from Any (default). */
#define MLAFP_SCAN_ANY_CONN_ANY  0x00

/* Allow Scan Request from White List Only, Allow Connect Request from Any. */
#define MLAFP_SCAN_WL_CONN_ANY   0x01

/* Allow Scan Request from Any, Allow Connect Request from White List Only. */
#define MLAFP_SCAN_ANY_CONN_WL   0x02

/* Allow Scan Request from White List Only, Allow Connect Request from
 * White List Only.
 */
#define MLAFP_SCAN_WL_CONN_WL    0x03

/* End of MeLeAdvertFilterPolicy */

/*---------------------------------------------------------------------------
 * MeLeScanType type
 *
 *     The Scan Type is used to determine the type of scan to perform when
 *     scanning is enabled.
 */
typedef U8 MeLeScanType;

#define MLST_PASSIVE_SCANNING          0x00  /* No Scan Req packets will be sent (default) */
#define MLST_ACTIVE_SCANNING           0x01  /* Scan Req packets will be sent */

/* End of MeLeScanType */

/*---------------------------------------------------------------------------
 * MeLeScanFilterPolicy type
 *
 *     The Scan Filter policy Type is used to determine the type of packets
 *     to receive while scanning is enabled.
 */
typedef U8 MeLeScanFilterPolicy;

/* Accept all advertisement packets (default). Directed advertising packets
 * which are not addressed for this device are ignored
 */
#define MLSFP_ACCEPT_ALL                0x00

/* Accept only advertisement packets from devices in the White List.
 * Directed advertising packets which are not addressed for this device
 * are ignored.
 */
#define MLSFP_ACCEPT_WL_ONLY            0x01

/* Accept all undirected advertisement packets. Directed advertising packets
 * which are not addressed for this device and aren't using an RPA are ignored.
 */
#define MLSFP_ACCEPT_ALL_RPA            0x02

/* Accept all advertisement packets from devices in the White List. Directed 
 * advertising packets which are not addressed for this device and aren't using 
 * an RPA are ignored.
 */
#define MLSFP_ACCEPT_ALL_RPA_AND_WL     0x03

/* End of MeLeScanFilterPolicy */

/*---------------------------------------------------------------------------
 * MeLeEnable type
 *
 *     MeLeEnable is used to enable/disable advertising and scanning.
 */
typedef U8 MeLeEnable;

#define MELE_DISABLE          0x00  /* Advertising or scanning is disabled */
#define MELE_ENABLE           0x01  /* Advertising or scanning is enabled */

/* End of MeLeEnable */

/*---------------------------------------------------------------------------
 * MeLeAdvertEventType type
 *
 *     The Advertizing Event Type specifies the type of advertising packet
 *     received.
 */
typedef U8 MeLeAdvertEventType;

#define MLAET_CONNECTABLE_UNDIRECTED    0x00  /* ADV_IND (default) */
#define MLAET_CONNECTABLE_DIRECTED      0x01  /* ADV_DIRECT_IND */
#define MLAET_DISCOVERABLE_UNDIRECTED   0x02  /* ADV_DISCOVER_IND */
#define MLAET_NONCONNECTABLE_UNDIRECTED 0x03  /* ADV_NONCONN_IND */
#define MLAET_SCAN_RESPONSE             0x04  /* SCAN_RSP */

/* End of MeLeAdvertEventType */

/*---------------------------------------------------------------------------
 * MeLeInitFilterPolicy type
 *
 *     The Initiator Filter Policy determines if the white list is used during
 *     an LE Create Connection procedure.
 */
typedef U8 MeLeInitFilterPolicy;

/* White list is not used to determine which advertiser to connect to.
 * The peerAddrType and peerAddr parameters are used.
 */
#define MLIFP_WL_NOT_USED           0x00

/* White list is used to determine which advertiser to connect to.
 * The peerAddrType and peerAddr parameters are ignored.
 */
#define MLIFP_WL_USED               0x01

/* End of MeLeInitFilterPolicy */

/*---------------------------------------------------------------------------
 * MeLeMasterClockAccuracy type
 *
 *     The Master Clock Accuracy specifies the accuracy of the master's clock.
 */
typedef U8 MeLeMasterClockAccuracy;

#define MLMCA_500_PPM     0x00
#define MLMCA_250_PPM     0x01
#define MLMCA_150_PPM     0x02
#define MLMCA_100_PPM     0x03
#define MLMCA_75_PPM      0x04
#define MLMCA_50_PPM      0x05
#define MLMCA_30_PPM      0x06
#define MLMCA_20_PPM      0x07

/* End of MeLeMasterClockAccuracy */

/*---------------------------------------------------------------------------
 * MeLeTxTestPacketPayload type
 *
 *     Packet payloads for LE Transmitter test.
 */
typedef U8 MeLeTxTestPacketPayload;

#define MLTTPP_PSEUDO_RANDOM_9   0x00 /* Pseudo-Random bit sequence 9 */
#define MLTTPP_ALTER_11110000    0x01 /* Pattern of alternating bits ‘11110000' */
#define MLTTPP_ALTER_10101010    0x02 /* Pattern of alternating bits ‘10101010' */
#define MLTTPP_PSEUDO_RANDOM_15  0x03 /* Pseudo-Random bit sequence 15 */
#define MLTTPP_ALL_1             0x04 /* Pattern of All ‘1’ bits */
#define MLTTPP_ALL_0             0x05 /* Pattern of All ‘0’ bits */
#define MLTTPP_ALTER_00001111    0x06 /* Pattern of alternating bits ‘00001111 */
#define MLTTPP_ALTER_0101        0x07 /* Pattern of alternating bits ‘0101’ */

/* End of MeLeTxTestPacketPayload */

/*---------------------------------------------------------------------------
 * MeLeEncryptState type
 *
 *     Encryption State of a Remote Device connected over LE
 */
typedef U8 MeLeEncryptState;

#define MLES_NOT_ENCRYPTED      0x00
#define MLES_START_ENCRYPT      0x01
#define MLES_ENCRYPTED          0x02

/* End of MeLeEncryptState */

/*---------------------------------------------------------------------------
 * MeLeKeyType type
 *
 *     LE security link key types
 */
typedef U8 MeLeKeyType;

#define NO_KEY                   0x00       /* No key exists */
#define UNAUTHENTICATED_KEY      0x01       /* Just Works */
#define AUTHENTICATED_KEY        0x02       /* MITM protection - using passkey or OOB data entry */
#define INVALID_KEY              0xFF

/* End of MeLeKeyType */

/*---------------------------------------------------------------------------
 * MeLeRandomAddr type
 *
 *     Type of private random address to generate
 */
typedef U8 MeLeRandomAddr;

#define MLRA_RESOLVABLE         0x00
#define MLRA_NON_RESOLVABLE     0x01
#define MLRA_STATIC             0x02

/* End of MeLeRandomAddr */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 * MeLeAdvertReport structure
 *
 *     Used to hold an advertising report.
 */
typedef struct _MeLeAdvertReport {
    MeLeAdvertEventType  eventType;
#if LL_PRIVACY == XA_ENABLED
    /* Address Type in the Advertising Report */
    MeLeAddressType      advertAddrType;
#endif 

    /* Address that directed advertisements are being directed to.
     * Valid for MELEE_DIRECT_ADVERT_REPORT events only.
     */
    BtDevAddr            directAddr;

    /* Advertising data in the Advertising Report */
    U8                   dataLen;
    U8                   data[31];

    /* Range -127 <= N <= +20; Units dBm; 127 means RSSI is not available */
    S8                   rssi;

} MeLeAdvertReport;


/*---------------------------------------------------------------------------
 * MeLeHandler structure
 *
 *     An LE Management Entity handler is registered with the LE Management
 *     Entity to receive events. This structure inherits from BtHandlerEx. The
 *     eMask field allows the application to receive only the desired while
 *     "blocking" unwanted events.
 */
typedef struct _MeLeHandler {
    BtHandlerEx  h;
    MeLeEventMask eMask;

} MeLeHandler;

/*---------------------------------------------------------------------------
 * MeLeLinkHandler structure
 *
 *     An LE Management Entity link handler is used when creating an LE
 *     connection. This structure inherits from BtHandlerEx.
 */
typedef struct _MeLeLinkHandler {
    BtHandlerEx  h;

    /* Group: Used internally by the stack */

    /* Pointer or reference to LE remote device */
    void        *remDev;

} MeLeLinkHandler;

/*---------------------------------------------------------------------------
 * MeLeOpHandler structure
 *
 *     An LE Management Entity Operation handler is used for receiving
 *     events when invoking certain commands such as MELE_Rand(),
 *     MELE_Encrypt(), MELE_ReadResolvableAddress(), 
 *     MELE_ReadResolvableAddress and MELE test functions.
 */
typedef struct _MeLeOpHandler {
    BtOperation  op;
    BtHandlerEx  h;

    union {
        /* Parameters used for MELE_Encrypt() */
        struct {
            U8    *key;        /* Pointer to 16 octet encryption key */
            U8    *plainText;  /* Pointer to 16 octet data block */
        } encrypt;

        /* Parameter used for MELE_ReceiverTest() */
        /* Value = (F-2402)/2; Range:0x00-0x27.
         * Frequency Range: 2402 MHz to 2480 MHz
         */
        U8         rxFrequency;

        /* Parameters used for MELE_TransmitterTest() */
        struct {
            /* Value = (F – 2402) / 2; Range: 0x00 – 0x27
             * Frequency Range : 2402 MHz to 2480 MHz
             */
            U8                      txFrequency;

            /* Length in bytes of payload data in each packet.
             * Range: 0x00 - 0x25
             */
            U8                      testDataLen;

            /* Packet payload */
            MeLeTxTestPacketPayload packetPayload;
        } txTest;

        /* Parameter used for MELE_GenerateDHKey() */
        /* Pointer to 64 octet P-256 public key. */
        U8            *P256PublicKey;

        /* Parameter used for MELE_SetDefaultDataLength() */
        struct {
            /* Suggested value for controller's maximum transmited number
             * of payload octets for new connections.
             */
            U16  sugMaxTxOctets;

            /* Suggested value for the controller's maximum packet 
             * transmission time for new connections
             */
            U16  sugMaxTxTime;
        } defaultLen;

        /* Parameter used for MELE_ReadResolvableAddress */
        BtDevAddr      identityAddr;
    }p;

} MeLeOpHandler;

/*---------------------------------------------------------------------------
 * MeLeAdvertParms structure
 *
 *     An LE Management Entity advertising parameter structure is used
 *     to pass LE advertizing parameters to the LE controller.
 *
 *     The advertIntervalMin field shall be less than or equal to the
 *     advertIntervalMax. The advertIntervalMax and advertIntervalMax
 *     should not be the same value to enable the LE Controller to
 *     determine the best advertising interval given other activities.
 *
 *     For directed advertising, when advertType is
 *     MLAT_CONNECTABLE_DIRECTED, advertIntervalMin and advertIntervalMax are
 *     not used and will be ignored.
 *
 *     The advertIntervalMin and advertIntervalMax shall not be set to
 *     less than 0x00A0 (100 ms) if advertType is the Advertising_Type is
 *     MLAT_DISCOVERABLE_UNDIRECTED or MLAT_NONCONNECTABLE_UNDIRECTED.
 */
typedef struct _MeLeAdvertParms {
    /* Minimum advertising interval for non-directed advertising.
     * Range: 0x0020 to 0x4000; Default: N = 0x0800 (1.28 second)
     * Time = N * 0.625 msec; Time Range: 20 ms to 10.24 sec
     */
    U16                   advertIntervalMin;

    /* Maximum advertising interval for non-directed advertising.
     * Range: 0x0020 to 0x4000; Default: N = 0x0800 (1.28 seconds)
     * Time = N * 0.625 msec; Time Range: 20 ms to 10.24 sec
     */
    U16                   advertIntervalMax;

    /* Advertising Type specifies the type of the advertising packets
     * used when advertising is enabled
     */
    MeLeAdvertType         advertType;

    /* ownAddrType determines if the advertising packets are identified
     * with the Public Device Address of the device, or a Random Device
     * Address as written using the MELE_SetRandomAddress() function.
     */
    MeLeAddressType        ownAddrType;

    /* If directed advertising is performed (advertType == 
     * MLAT_CONNECTABLE_DIRECTED or MLAT_LOW_DUTY_DIRECTED) 
     * then peerAddrType and peerAddr are valid, otherwise 
     * they are ignored and not used.
     */
    MeLeAddressType        peerAddrType;

    /* Device address used for the remote (peer) device when 
     * advertType == MLAT_CONNECTABLE_DIRECTED or 
     * MLAT_LOW_DUTY_DIRECTED 
     */
    BD_ADDR                peerAddr;

    /* Advertising Channel Map specifies which advertising channels
     * are used when advertising is enabled
     */
    MeLeAdvertChanMap      channelMap;

    /* Filter policy used for Scan Request and Connect Request. */
    MeLeAdvertFilterPolicy filterPolicy;

} MeLeAdvertParms;

/*---------------------------------------------------------------------------
 * MeLeScanParms structure
 *
 *     An LE Management Entity scan parameter structure is used
 *     to pass LE scan parameters to the LE controller.
 *
 *     The scanInterval and scanWindow parameters are recommendations from
 *     the Host on how long (scanWindow) and how frequently (scanInterval)
 *     the Controller should scan. The scanWindow parameter must always be
 *     set to a value smaller equal to the value set for the scanInterval
 *     parameter. If they are set to the same value scanning will run
 *     continuously.
 *
 */
typedef struct _MeLeScanParms {
    /* scanType specifies the type of scanning that will be used */
    MeLeScanType           scanType;

    /* This is defined as the time interval from when the Controller
     * started its last LE scan until it begins the subsequent LE scan.
     * Range: 0x0004 to 0x4000; Default: 0x0010 (10 ms)
     * Time = N * 0.625 msec; Time Range: 2.5 msec to 10.24 seconds
     */
    U16                    scanInterval;

    /* The duration of the LE scan. scanWindow shall be less than
     * or equal to scanInterval.
     * Range: 0x0004 to 0x4000; Default: 0x0010 (10 ms)
     * Time = N * 0.625 msec; Time Range: 2.5 msec to 10240 msec
     */
    U16                    scanWindow;

    /* ownAddrtype determines if the scanning packets are identified
     * with the Public Device Address of the device, or a Random Device
     * Address as written using the MELE_SetRandomAddress() function.
     */
    MeLeAddressType        ownAddrType;

    MeLeScanFilterPolicy   filterPolicy;

} MeLeScanParms;

/*---------------------------------------------------------------------------
 * MeLeCreateLinkParms structure
 *
 *     An LE Management Entity Create Link parameter structure is used
 *     to pass LE connection parameters to the LE controller as part
 *     of an MELE_CreateLink() operation.
 *
 *     The scanInterval and scanWindow parameters are recommendations from the
 *     Host on how long (scanWindow) and how frequently (scanInterval) the
 *     LE Controller should scan. The scanWindow parameter must be set to a
 *     value smaller or equal to the value set for the scanInterval parameter.
 *     If both are set to the same value, scanning will run continuously.
 *
 *     The minCeLength and maxCeLength parameters are informative
 *     parameters providing the Controller with the expected minimum and
 *     maximum length of the connection events. The minCeLength parameter
 *     must be less than or equal to the maxCeLength parameter.
 */
typedef struct _MeLeCreateLinkParms {
    /* This is defined as the time interval from when the Controller started
     * its last LE scan until it begins the subsequent LE scan.
     * Range: 0x0004 to 0x4000; Time = N * 0.625 msec;
     * Time Range: 2.5 msec to 10240 msec
     */
    U16                  scanInterval;

    /* Amount of time for the duration of the LE scan. scanWindow must be
     * less than or equal to scanInterval
     * Range: 0x0004 to 0x4000; Time = N * 0.625 msec;
     * Time Range: 2.5 msec to 10.24 seconds
     */
    U16                  scanWindow;

    /* The filterPolicy is used to determine whether the White List is used.
     * If the White List is not used, the peerAddrType and the peerAddr
     * parameters specify the address type and address of the advertising device to
     * connect to.
     */
    MeLeInitFilterPolicy filterPolicy;

    /* Public Device Address or Random Device Address of the device to be
     * connected. Used if filterPolicy is set to MLIFP_WL_NOT_USED or 
     * MLIFP_RPA_WL_NO_USED otherwise it is ignored.
     */
    BD_ADDR             peerAddr;

    /* Type of the peerAddr. Used if filterPolicy is set to MLIFP_WL_NOT_USED or 
     * MLIFP_RPA_WL_NO_USED otherwise it is ignored.
     */
    MeLeAddressType     peerAddrType;

    /* Type of address of the local device to be used when establishing the
     * link. The Public address is the BD_ADDR of the BR/EDR/LE controller. The
     * random address is set in the controller using MELE_SetRandomAddress().
     */
    MeLeAddressType      ownAddrType;

    /* Minimum value for the connection event interval. This must be less
     * than or equal to connIntervalMax.
     * Range: 0x0006 to 0x0C80; Time = N * 1.25 msec
     * Time Range: 7.5 msec to 4 seconds.
     */
    U16                  connIntervalMin;

    /* Maximum value for the connection event interval. This must be
     * greater than or equal to Conn_Interval_Min.
     * Range: 0x0006 to 0x0C80; Time = N * 1.25 msec
     * Time Range: 7.5 msec to 4 seconds.
     */
    U16                  connIntervalMax;

    /* Slave latency for the connection in number of connection events.
     * Slave latency allows a slave to use a reduced number of connection
     * events. The connLatency parameter defines the number of consecutive
     * connection events that the slave device is not required to listen for
     * the master. The value of connSlaveLatency should not cause a
     * Supervision Timeout.
     * Range: 0x0000 to 0x01F3
     */
    U16                 connLatency;

    /* Supervision timeout for the LE Link. The Supervision_Timeout in
     * milliseconds must be larger than connIntervalMax in milliseconds.
     * Range: 0x000A to 0x0C80; Time = N * 10 msec
     * Time Range: 100 msec to 32 seconds
     */
    U16                 superTimeout;

    /* Information parameter about the minimum length of connection
     * needed for this LE connection.
     * Range: 0x0000 – 0xFFFF; Time = N * 0.625 msec.
     */
    U16                 minCeLength;

    /* Information parameter about the maximum length of connection needed
     * for this LE connection.
     * Range: 0x0000 – 0xFFFF; Time = N * 0.625 msec.
     */
    U16                 maxCeLength;

} MeLeCreateLinkParms;

/*---------------------------------------------------------------------------
 * MeLeUpdateLinkParms structure
 *
 *     An LE Management Entity Update Link parameter structure is used
 *     to pass LE connection parameters to the LE controller as part
 *     of an MELE_UpdateLink() operation.
 *
 *     The minCeLength and maxCeLength parameters are informative
 *     parameters providing the Controller with the expected minimum and
 *     maximum length of the connection events. The minCeLength parameter
 *     must be less than or equal to the maxCeLength parameter.
 */
typedef struct _MeLeUpdateLinkParms {
    /* Minimum value for the connection event interval. This must be less
     * than or equal to connIntervalMax.
     * Range: 0x0006 to 0x0C80; Time = N * 1.25 msec
     * Time Range: 7.5 msec to 4 seconds.
     */
    U16                  connIntervalMin;

    /* Maximum value for the connection event interval. This must be
     * greater than or equal to Conn_Interval_Min.
     * Range: 0x0006 to 0x0C80; Time = N * 1.25 msec
     * Time Range: 7.5 msec to 4 seconds.
     */
    U16                  connIntervalMax;

    /* Slave latency for the connection in number of connection events.
     * Slave latency allows a slave to use a reduced number of connection
     * events. The connLatency parameter defines the number of consecutive
     * connection events that the slave device is not required to listen for
     * the master. The value of connSlaveLatency should not cause a
     * Supervision Timeout.
     * Range: 0x0000 to 0x01F3
     */
    U16                 connLatency;

    /* Supervision timeout for the LE Link. The Supervision_Timeout in
     * milliseconds must be larger than connIntervalMax in milliseconds.
     * Range: 0x000A to 0x0C80; Time = N * 10 msec
     * Time Range: 100 msec to 32 seconds
     */
    U16                 superTimeout;

    /* Information parameter about the minimum length of connection
     * needed for this LE connection.
     * Range: 0x0000 – 0xFFFF; Time = N * 0.625 msec.
     */
    U16                 minCeLength;

    /* Information parameter about the maximum length of connection needed
     * for this LE connection.
     * Range: 0x0000 – 0xFFFF; Time = N * 0.625 msec.
     */
    U16                 maxCeLength;

} MeLeUpdateLinkParms;

/*---------------------------------------------------------------------------
 * MeLeLinkParms structure
 *
 *     An LE Management Entity Connection parameter structure holds
 *     the connection parameters for an active LE connection (link).
 */
typedef struct _MeLeLinkParms {
    /* The local device's role in this connection */
    BtConnectionRole        role;

    /* Connection interval used on this connection. Range: 0x0006 to 0x0C80
     * Time = N * 1.25 msec; Time Range: 7.5 msec to 4000 msec.
     */
    U16                     connInterval;

    /* Connection latency for this connection. Range: 0x0006 to 0x0C80
     * Time = N * 1.25 msec; Time Range: 7.5 msec to 4000 msec.
     */
    U16                     connLatency;

    /* Connection supervision timeout. Range: 0x000A to 0x0C80
     * Time = N * 10 msec; Time Range: 100 msec to 32 seconds
     */
    U16                     superTimeout;

    /* Accuracy of the master's clock. The masterClockAccuracy parameter is
     * only valid for a slave. On a master, this parameter is set to 0x00.
     */
    MeLeMasterClockAccuracy masterClockAccuracy;

} MeLeLinkParms;

/*---------------------------------------------------------------------------
 * MeLeEvent structure
 *
 *     Used to pass information from the LE Management Entity to applications.
 *     Not all fields are valid at any given time. The type of event determines
 *     which fields are valid. This structure inherits from BtEventEx.
 */
typedef struct _MeLeEvent {
    BtEventEx       e;
    BtErrorCode     errCode;

    /* Fields used to identify a remote device used by
     * a number of events including MELEE_LINK_CONNECT_CNF,
     * MELEE_ADVERT_REPORT, MELEE_LINK_DISCONNECT, MELEE_MASK_LINK_CREATE_CANCEL,
     * MELEE_UPDATE_LINK_CNF, MELEE_REMOTE_FEATURES, MELEE_UPDATE_LINK_REQ,
     * MELEE_DATA_LENGTH_COMPLETE, MELEE_DATA_LENGTH_CHANGE, MELEE_ADD_WHITE_LIST,
     * MELEE_REMOVE_WHITE_LIST, MELEE_READ_CHANNEL_MAP, MELEE_START_ENCRYPT_COMPLETE
     */
    BtDevAddr       bdAddr;

    /* Identity address used to index into DDB. This is the "real" address. Valid
     * for MELEE_LINK_CONNECT_CNF, MELEE_UPDATE_LINK_CNF, MELEE_LINK_DISCONNECT,
     * MELEE_DATA_LENGTH_COMPLETE, MELEE_DATA_LENGTH_CHANGE, 
     * MELEE_ADD_RESOLVING_LIST_CNF, MELEE_REMOVE_RESOLVING_LIST_CNF, 
     * MELEE_READ_RESOLVE_ADDR_COMPLETE,  MELEE_READ_CHANNEL_MAP, MELEE_START_ENCRYPT_COMPLETE
     */
    BtDevAddr       identityAddr;

    union {
        /* Valid for MELEE_ADVERT_REPORT and MELEE_DIRECT_ADVERT_REPORT events */
        MeLeAdvertReport    advertReport;

        /* Valid for MELEE_AUTHENTICATED, MELEE_MASK_LINK_CREATE_CANCEL,
         * MELEE_START_ENCRYPT_COMPLETE, MELEE_DATA_LENGTH_COMPLETE and 
         * MELEE_AUTH_PAYLOAD_TIMEOUT events
         */
        BtRemoteDevice     *remDev;

        /* Valid for MELEE_ENCRYPTION_CHANGE event */
        struct {
            BtRemoteDevice *remDev;
            U8              mode; /* New encryption mode (uses the
                                   * BtEncryptMode type) */
        } encrypt;

        struct {
            BtRemoteDevice *remDev;

            /* Valid for MELEE_LINK_CONNECT_CNF and MELEE_UPDATE_LINK_CNF events
             * Note: for MELEE_UPDATE_LINK_CNF only link.connInterval, link.connLatency
             * and link.superTimeout parameters will have changed
             */
            MeLeLinkParms   link;
        } connect;

        struct {
            BtRemoteDevice *remDev;

            /* Valid for MELEE_LINK_DISCONNECT event.
             * If disconnection was successful, contains BEC_NO_ERROR.
             * errCode will contain the disconnect reason.
             *
             * Unsuccessful disconnections will contain an error code
             * as generated by the radio. In this case, errCode can
             * be ignored.
             */
            BtErrorCode     discStatus;
        } disconnect;

        /* Valid for MELEE_LINK_CHANNEL_MAP and MELEE_SET_CHAN_CLASS_COMPLETE
         * event. There are 37 1-bit fields. The nth such field (in the range
         * 0 to 36) contains the value for the link layer channel index n.
         * For MELEE_LINK_CHANNEL_MAP event channel n is unused = 0 and
         * channel n is used = 1. For MELEE_SET_CHAN_CLASS_COMPLETE
         * channel n is bad = 0 and channel n is unknown = 1.
         * The most significant bits are reserved and are set to 0.
         */
        U8                  channelMap[5];

        /* Valid for MELEE_RAND_COMPLETE */
        U8                  rand[8];

        /* Valid for MELEE_ENCRYPT_COMPLETE */
        U8                  encryptedData[16];

        /* Valid for MELEE_TEST_END_COMPLETE */
        U16                 numTestPacketsRx; /* Number of test packets received */

        /* Valid for MELEE_SET_RANDOM_ADDR_COMPLETE */
        BD_ADDR             randomAddr;

        /* Valid for MELEE_SET_SCAN_ENABLE_CNF and MELEE_SET_ADVERT_ENABLE_CNF and
         * MELEE_SET_ADDR_RESOLUTION_ENABLE_CNF */
        MeLeEnable          enable;

        /* Valid for MELEE_REMOTE_FEATURES */
        struct {
            BtRemoteDevice     *remDev;
            U8                  remoteFeatures[8];  /* Remote features supported */
        } features;

        /* Valid for MELEE_UPDATE_LINK_REQ */
        struct {
            BtRemoteDevice      *remDev;

            /* The connIntervalMin, connIntervalMax, connLatency and superTimeout fields contain
             * the values being requested by the remote slave. The minCeLength and maxCeLength
             * fields contain the values currently set in the remDev structure that were used
             * when the connection was created or last updated (createMinCeLength and createMaxCeLength)
             */
            MeLeUpdateLinkParms  parms;
        } updateLinkReq;

        /* Valid for MELEE_P256_KEY_COMPLETE */
        U8                 *P256PublicKey;

        /* Valid for MELEE_GEN_DHKEY_COMPLETE */
        U8                 *DHKey;

        /* Valid for MELEE_DATA_LENGTH_CHANGE */
        struct {
            /* Pointer to remote device identifying the connection */
            BtRemoteDevice     *remDev;

            /* The maximum number of payload octets in link layer data channel PDU 
             * that a controller will send on this connections
             */
            U16  maxTxOctets; 
            
            /* The maximum time that the local controller will take to send a link layer
             * data channel PDU on this connection
             */
            U16  maxTxTime;
            
            /* The maximum number of payload octets in link layer data channel PDU 
             * that the local controller expects to receive on this connection.
            */
            U16  maxRxOctets;
            
            /* The maximum time that the local Controller expects to take to receive a
             * link layer data channel PDU on this connection. 
            */
            U16  maxRxTime;
        } lenChange;

#if LL_PRIVACY == XA_ENABLED
        /* Valid for MELEE_READ_RESOLVE_ADDR_COMPLETE */
        struct {
            BD_ADDR     bdAddr;
            BOOL        local;
        } resolvableAddr;

        /* Valid for MELEE_ADD_RESOLVING_LIST_CNF */
        struct {
            U8          peerIrk[16];
            U8          localIrk[16];
        } irkPair;

        U16 resolvableAddrTimeout;
#endif /* LL_PRIVACY == XA_ENABLED */
    } p;

} MeLeEvent;


/*---------------------------------------------------------------------------
 * MeLeDataLengthParms structure
 *
 *     Holds data length parameters. 
 */
typedef struct _MeLeDataLengthParms {
    /* The maximum number of payload octets in a Link Layer Data Channel PDU
     * sent by the local controller
     */
    U16                     maxTxOctets;
    
    /* The maximum time in milliseconds that the local Controller will take to 
     * send a Link Layer Data Channel PDU
     */
    
    U16                     maxTxTime;       
    /* The maximum number of payload octets in a Link Layer Data Channel PDU
     * that the local controller expects to receive
     */
    U16                     maxRxOctets;
    
    /* The maximum time in milliseconds that the local Controller expects to
     * take to receive a Link Layer Data Channel PDU
     */
    U16                     maxRxTime;        
} MeLeDataLengthParms;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * MELE_Register()
 *
 *     Register a handler with the LE Management Entity. The handler will be
 *     called back on LE ME events. Only events that are set in handler.eMask
 *     will be received. MeLeEventMask types can be ORed together. To receive
 *     all events set handler.eMask to MELEE_MASK_ALL.
 *
 * Parameters:
 *     handler - Pointer to handler with fields initialized (especially eMask and
 *         h.callback).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_INVALID_PARM - h.callback in handler is 0 (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - request failed because the handler is already
 *         registered or LE is not supported by the controller.
 *
 */
BtStatus MELE_Register(MeLeHandler *handler);

/*---------------------------------------------------------------------------
 * MELE_Unregister()
 *
 *     Unregisters a handler from the LE ME. The handler will receive
 *     no further events.
 *
 * Parameters:
 *     handler - Pointer to a registered handler.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed because the handler was not
 *         registered or LE is not supported by the controller.
 *
 */
BtStatus MELE_Unregister(MeLeHandler *handler);

/*---------------------------------------------------------------------------
 * MELE_SetRandomAddress()
 *
 *     Set the Random address in the LE Controller.
 *
 *     Note: This API shall not be used if the BlueSDK Connection Manager
 *     is enabled. The Connection manager uses this API, acts on events
 *     generated by this API and assumes that it is the sole user of it.
 *
 * Parameters:
 *     bdAddr - pointer to random address.
 *
 * Returns:
 *     BT_STATUS_PENDING - The random address has been sent to the LE
 *         controller.  When complete, a MELEE_SET_RANDOM_ADDR_COMPLETE event
 *         will be sent to all registered handlers.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress.  Only one operation can be in progress at a time. Keep
 *         track of the ME LE events to see when the operation is complete.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameter
 *         was not properly initialized (XA_ERROR_CHECK only).
 */
BtStatus MELE_SetRandomAddress(BD_ADDR *bdAddr);

/*---------------------------------------------------------------------------
 * MELE_SetAdvertParms()
 *
 *     Set the LE advertising parameters that will be used by the LE
 *     controller when advertising is enabled.
 *
 *     Note: This API shall not be used if the BlueSDK Connection Manager
 *     is enabled. The Connection manager uses this API, acts on events
 *     generated by this API and assumes that it is the sole user of it.
 *
 * Parameters:
 *     parms - Pointer to advertising parameters.
 *
 * Returns:
 *     BT_STATUS_PENDING - The Advertising Parameters are being sent to the
 *         LE controller.  When complete, a MELEE_SET_ADVERT_PARMS_CNF event
 *         will be sent to all registered handlers.
 *
 *     BT_STATUS_FAILED - The provided Advertising Parameters could not be 
 *         sent to the LE controller.  This could happen if the current LE
 *         controller doesn't support the advertising type selected 
 *         (e.g. low-duty advertising).
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress.  Only one operation can be in progress at a time. Keep
 *         track of the ME LE events to see when the operation is complete.
 *
 *     BT_STATUS_INVALID_TYPE - The operation failed because the request was for 
 *         directed advertising to an RPA where the remote device does not
 *         support Central Address Resolution.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameter
 *         was not properly initialized (XA_ERROR_CHECK only).
 */
BtStatus MELE_SetAdvertParms(MeLeAdvertParms *parms);

/*---------------------------------------------------------------------------
 * MELE_SetAdvertData()
 *
 *     Set the LE advertising data that is used in advertising packets that
 *     have a data field. The maximum size of the advertising data is 31
 *     octets. Only the significant part of advertising data is in transmitted
 *     in advertising packets. The significant part contains a sequence of
 *     AD structures.
 *
 *     Each AD structure has a Length field of one octet, which contains the
 *     Length value, and a Data field of Length octets. The first octet of
 *     the Data field contains the AD type field. The content of the remaining
 *     Length – 1 octets in the Data field depends on the value of the AD type
 *     field and is called the AD data. The non-significant part extends the
 *     Advertising and Scan Response data to 31 octets and contains all-zero
 *     octets.
 *
 *     Note: This API shall not be used if the BlueSDK Connection Manager
 *     is enabled. The Connection manager uses this API, acts on events
 *     generated by this API and assumes that it is the sole user of it.
 *
 * Parameters:
 *     length - Number of octets for the significant part of the
 *         advertising data. Only "length" number of octets will be sent
 *         to the controller with the remaining octets up to 31 being set
 *         to 0.
 *
 *     data - pointer to the advertising data.
 *
 * Returns:
 *     BT_STATUS_PENDING - The Advertising Data are being sent to the
 *         LE controller.  When complete, a MELEE_SET_ADVERT_DATA_CNF event
 *         will be sent to all registered handlers.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress.  Only one operation can be in progress at a time. Keep
 *         track of the ME LE events to see when the operation is complete.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         were invalid (XA_ERROR_CHECK only).
 */
BtStatus MELE_SetAdvertData(U8 length, U8 *data);

/*---------------------------------------------------------------------------
 * MELE_SetScanParms()
 *
 *     Set the LE scan parameters that will be used by the LE controller when
 *     scanning is enabled.
 *
 *     Note: This API shall not be used if the BlueSDK Connection Manager
 *     is enabled. The Connection manager uses this API, acts on events
 *     generated by this API and assumes that it is the sole user of it.
 *
 * Parameters:
 *     parms - Pointer to scan parameters.
 *
 * Returns:
 *     BT_STATUS_PENDING - The Scan Parameters are being sent to the
 *         LE controller.  When complete, a MELEE_SET_SCAN_PARMS_CNF event
 *         will be sent to all registered handlers.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress.  Only one operation can be in progress at a time. Keep
 *         track of the ME LE events to see when the operation is complete.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameter
 *         was not properly initialized (XA_ERROR_CHECK only).
 */
BtStatus MELE_SetScanParms(MeLeScanParms *parms);

/*---------------------------------------------------------------------------
 * MELE_SetScanRspData()
 *
 *     Set the LE scan response data that is used in scanning packets that
 *     have a data field. The maximum size of the scan data is 31 octets.
 *     Only the significant part of scan data is in transmitted in scan
 *     packets. The significant part contains a sequence of AD structures.
 *
 *     Each AD structure has a Length field of one octet, which contains the
 *     Length value, and a Data field of Length octets. The first octet of
 *     the Data field contains the AD type field. The content of the remaining
 *     Length – 1 octets in the Data field depends on the value of the AD type
 *     field and is called the AD data. The non-significant part extends the
 *     Advertising and Scan Response data to 31 octets and contains all-zero
 *     octets.
 *
 *     Note: This API shall not be used if the BlueSDK Connection Manager
 *     is enabled. The Connection manager uses this API, acts on events
 *     generated by this API and assumes that it is the sole user of it.
 *
 * Parameters:
 *     length - Number of octets for the significant part of the
 *         scan data. Only "length" number of octets will be sent
 *         to the controller with the remaining octets up to 31 being set
 *         to 0.
 *
 *     data - pointer to the advertising data.
 *
 * Returns:
 *     BT_STATUS_PENDING - The Scan Data are being sent to the
 *         LE controller.  When complete, a MELEE_SET_SCAN_RSP_DATA_CNF event
 *         will be sent to all registered handlers.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress.  Only one operation can be in progress at a time. Keep
 *         track of the ME LE events to see when the operation is complete.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         were invalid (XA_ERROR_CHECK only).
 */
BtStatus MELE_SetScanRspData(U8 length, U8 *data);

/*---------------------------------------------------------------------------
 * MELE_SetAdvertEnable()
 *
 *     Request the LE Controller to start or stop advertising. The Controller
 *     manages the timing of advertisements based on the advertising parameters
 *     set using MELE_SetAdvertParms().
 *
 *     The Controller will continue advertising until MELE_SetAdvertEnable() is
 *     called with "enable" set to MELE_DISABLE, until a connection is created
 *     or until advertising is timed out due to direct advertising. In these
 *     cases, advertising is then disabled.
 *
 *     If the adverType of the MeLeAdvertParms is MLAT_CONNECTABLE_DIRECTED and
 *     the advertising fails to create a connection, an MELEE_CONNECT_COMPLETE
 *     event with failure will be sent to all registered handlers.
 *
 *     If the adverType of the MeLeAdvertParms is MLAT_CONNECTABLE_UNDIRECTED or
 *     MLAT_CONNECTABLE_DIRECTED and a connection is established an
 *     MELEE_CONNECT_COMPLETE will be sent to all registered handlers.
 *
 *     Note that there is possible race condition if advertising is disabled (i.e.
 *     "enable" equals MELE_DISABLE) and the advertType of the MeLeAdverParms is
 *     MLAT_CONNECTABLE_UNDIRECTED or MLAT_CONNECTABLE_DIRECTED. The advertisements
 *     might not be stopped before a connection is created and therefore an
 *     MELEE_CONNECT_COMPLETE and an MELEE_SET_ADVERT_ENABLE_CNF event may
 *     be sent. This can also occur when direct advertising is timed out
 *     and this function disables advertising.
 *
 *     Note: This API shall not be used if the BlueSDK Connection Manager
 *     is enabled. The Connection manager uses this API, acts on events
 *     generated by this API and assumes that it is the sole user of it.
 *
 * Parameters:
 *     enable - Specifies if advertising should be enabled or disabled.
 *
 * Returns:
 *     BT_STATUS_PENDING - The Set Advertising enable command is being sent to
 *         the LE controller.  When complete, a MELEE_SET_ADVERT_ENABLE_CNF
 *         event will be sent to all registered handlers. (See comments above
 *         about other possible events).
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress.  Only one operation can be in progress at a time. Keep
 *         track of the ME LE events to see when the operation is complete.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         were invalid (XA_ERROR_CHECK only).
 */
BtStatus MELE_SetAdvertEnable(MeLeEnable enable);

/*---------------------------------------------------------------------------
 * MELE_SetScanEnable()
 *
 *     Request the LE Controller to start or stop scanning. Scanning is used
 *     to discover advertising devices nearby. The Controller manages the
 *     scanning based on the scan parameters set using MELE_SetScanParms().
 *
 *     Once scanning is enabled zero or more MELEE_ADVERT_REPORT events are
 *     generated based on advertising packets received and the duplicate
 *     filtering.
 *
 *     Note: This API shall not be used if the BlueSDK Connection Manager
 *     is enabled. The Connection manager uses this API, acts on events
 *     generated by this API and assumes that it is the sole user of it.
 *
 * Parameters:
 *     enable - Indicates if scanning should be enabled or disabled.
 *
 *     filterDuplicates - Specifies if duplicate filtering is enabled or disabled.
 *
 * Returns:
 *     BT_STATUS_PENDING - The Set Scan enable command is being sent to the
 *         LE controller.  When complete, a MELEE_SET_SCAN_ENABLE_CNF event
 *         will be sent to all registered handlers. (See comments above
 *         about other possible events).
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress.  Only one operation can be in progress at a time. Keep
 *         track of the ME LE events to see when the operation is complete.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         were invalid (XA_ERROR_CHECK only).
 */
BtStatus MELE_SetScanEnable(MeLeEnable enable, MeLeEnable filterDuplicates);

/*---------------------------------------------------------------------------
 * MELE_CreateLink()
 *
 *     Create an LE Link Layer connection to a connectable advertiser. Only
 *     one LE Link Layer connection can be pending in an LE controller. If
 *     this function returns BT_STATUS_PENDING the results will be sent to
 *     the handler. When the operation is complete all registered LE
 *     handlers will receive MELEE_CONNECT_COMPLETE as will all handlers
 *     passed to this function. The "errCode" field of the MeLeEvent will
 *     indicate the HCI result of the operation. The "e.status" field will
 *     indicate success or failure of the operation.
 *
 *     If a connection already exists to the device indicated by
 *     parms->peerAddr, this function will register the handler with the
 *     LE link and return BT_STATUS_SUCCESS immediately. In this way, many
 *     local handlers can be registered to the same connection. Be aware that
 *     MELE_DisconnectLink will not take effect until it is called for
 *     ALL registered handlers of the LE link. To determine whether a
 *     connection already exists without registering a handler, use
 *     MELE_FindRemoteDevice instead.
 *
 *     A slave device does not need to use this API unless it needs to
 *     register a local handler or initiate a disconnection of the link (see
 *     MELE_DisconnectLink).
 *
 *     Note: This API shall not be used if the BlueSDK Connection Manager
 *     is enabled. The Connection manager uses this API, acts on events
 *     generated by this API and assumes that it is the sole user of it.
 *
 * Parameters:
 *     handler - pointer to an unregistered handler to receive events. This
 *         handler cannot be the same handler used in MELE_Register() though
 *         they both can point to the same callback. The handler must be
 *         initialized using MeLeInitLinkHandler. The handler is in use until
 *         the link is disconnected and should not be modified while in use.
 *         Note that if a user has both a handler registered with MELE_Register
 *         and one registered with the LE link then both will receive events
 *         for the link.
 *
 *     parms - Pointer to create connection parameters.
 *
 * Returns:
 *     BT_STATUS_PENDING - A connection has been attempted. When complete,
 *         a MELEE_LINK_CONNECT_CNF event will be sent to all registered
 *         handlers. Note that handler fields used internally by the stack
 *         will be modified.
 *
 *     BT_STATUS_SUCCESS - A connection to the given remote device already
 *         exists and the handler is registered with the LE Link. Note that
 *         handler fields used internally by the stack will be modified.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress.  Only one operation can be in progress at a time. Keep
 *         track of the ME LE events to see when the operation is complete.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameter
 *         was not properly initialized (XA_ERROR_CHECK only).
 */
BtStatus MELE_CreateLink(MeLeLinkHandler *handler, MeLeCreateLinkParms *parms);

/*---------------------------------------------------------------------------
 * MELE_ForceDisconnectLinkWithReason()
 *
 *     Force disconnection of the link regardless of registered handlers.
 *     See MELE_DisconnectLink for additional information.
 *
 * Parameters:
 *     handler - pointer to registered handler (can be 0).
 *
 *     bdAddr - pointer to BtDevAddr of remote device to be disconnected (is
 *         ignored and can be 0 if handler is non-0).
 *
 *     reason - disconnect reason. Must be one of: BEC_AUTHENTICATE_FAILURE,
 *          BEC_USER_TERMINATED, BEC_LOW_RESOURCES, BEC_POWER_OFF,
 *          BEC_UNSUPPORTED_REMOTE, or BEC_PAIR_UNITKEY_NO_SUPP.
 *
 *     forceDisconnect - If TRUE, will attempt to disconnect regardless of
 *         whether other handlers are still registered. If FALSE, behaves as
 *         MELE_DisconnectLinkWithReason.
 *
 * Returns:
 *     BT_STATUS_INVALID_PARM - the operation failed because the "reason"
 *         parameter is invalid for a disconnect link operation.
 *
 *     See MELE_DisconnectLink for additional returns.
 */
BtStatus MELE_ForceDisconnectLinkWithReason(MeLeLinkHandler *handler,
                                            BtDevAddr *bdAddr,
                                            BtErrorCode reason,
                                            BOOL forceDisconnect);

/*---------------------------------------------------------------------------
 * MELE_ForceDisconnectLinkWithReasonEx()
 *
 *     Force disconnection of the link regardless of registered handlers.
 *     The link will not be closed if the remote device initiated the 
 *     connection unless the forceDisconnect flag is used.
 *
 *     See MELE_DisconnectLink for additional information.
 *
 *     However,the connection can always be forced down by using the other 
 *     disconnect API's. 
 * 
 * Parameters:
 *     handler - pointer to registered handler (can be 0).
 *
 *     bdAddr - pointer to BtDevAddr of remote device to be disconnected (is
 *         ignored and can be 0 if handler is non-0).
 *
 *     reason - disconnect reason. Must be one of: BEC_AUTHENTICATE_FAILURE,
 *          BEC_USER_TERMINATED, BEC_LOW_RESOURCES, BEC_POWER_OFF,
 *          BEC_UNSUPPORTED_REMOTE, or BEC_PAIR_UNITKEY_NO_SUPP.
 *
 *     forceDisconnect - If TRUE, will attempt to disconnect regardless of
 *         whether other handlers are still registered. If FALSE, behaves as
 *         MELE_DisconnectLinkWithReasonEx.
 *
 * Returns:
 *     BT_STATUS_INVALID_PARM - the operation failed because the "reason"
 *         parameter is invalid for a disconnect link operation.
 *
 *     See MELE_DisconnectLink for additional returns.
 */
BtStatus MELE_ForceDisconnectLinkWithReasonEx(MeLeLinkHandler *handler,
                                              BtDevAddr *bdAddr,
                                              BtErrorCode reason,
                                              BOOL forceDisconnect);

/*---------------------------------------------------------------------------
 * MELE_DisconnectLinkWithReason()
 *
 *     Request disconnection of the link with a non-default reason code.
 *     See MELE_DisconnectLink for additional information.
 *
 * Parameters:
 *     handler - pointer to registered handler.
 *
 *     reason - disconnect reason. If multiple handlers call this
 *         function or MELE_DisconnectLink, the last handler's reason code
 *         will be used. If reason is BEC_POWER_OFF, disconnect is
 *         more forceful in that all the handlers need not have already
 *         been freed.
 *
 * Returns:
 *     BT_STATUS_INVALID_PARM - the operation failed because the "reason"
 *         parameter is invalid for a disconnect link operation.
 *
 *     See MELE_DisconnectLink for additional returns.
 */
BtStatus MELE_DisconnectLinkWithReason(MeLeLinkHandler* handler,
                                       BtErrorCode reason);
#define MELE_DisconnectLinkWithReason(ha, rs) \
    (MELE_ForceDisconnectLinkWithReason(ha, 0, rs, FALSE))

/*---------------------------------------------------------------------------
 * MELE_DisconnectLinkWithReasonEx()
 *
 *     Request disconnection of the link with a non-default reason code.
 *     The link will not be closed if the remote device initiated the 
 *     connection.  However, the connection can always be forced down by 
 *     using the other disconnect API's. 
 * 
 *     See MELE_DisconnectLink for additional information.
 *
 * Parameters:
 *     handler - pointer to registered handler.
 *
 *     reason - disconnect reason. If multiple handlers call this
 *         function or MELE_DisconnectLink, the last handler's reason code
 *         will be used. If reason is BEC_POWER_OFF, disconnect is
 *         more forceful in that all the handlers need not have already
 *         been freed.
 *
 * Returns:
 *     BT_STATUS_INVALID_PARM - the operation failed because the "reason"
 *         parameter is invalid for a disconnect link operation.
 *
 *     See MELE_DisconnectLink for additional returns.
 */
BtStatus MELE_DisconnectLinkWithReasonEx(MeLeLinkHandler* handler,
                                         BtErrorCode reason);
#define MELE_DisconnectLinkWithReasonEx(ha, rs) \
    (MELE_ForceDisconnectLinkWithReasonEx(ha, 0, rs, FALSE))

/*---------------------------------------------------------------------------
 * MELE_DisconnectLink()
 *
 *     Request disconnection of the link, using the default reason code
 *     (BEC_USER_TERMINATED). This function also deregisters the
 *     specified handler, which was previously registered with
 *     MELE_CreateLink.
 *
 *     The link is actually closed only when all registered handlers have
 *     requested disconnection. If a device needs to know exactly when the
 *     disconnection occurs, it should use an LE handler to monitor connection
 *     state. The handler specified in this API is freed by this function, so it
 *     will not be notified of any disconnection events.
 *
 *     If the remote device initiated the connection, the local
 *     device may have no registered handler for the connection. In this
 *     case, the local device must call MELE_CreateLink or to register a handler,
 *     then call this function to request disconnection.
 *
 * Parameters:
 *     handler - pointer to registered handler.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - the operation has completed successfully.
 *         The caller's handler is unbound from the link. When the link
 *         is actually disconnected the MELEE_LINK_DISCONNECT event will
 *         be sent to all registered handlers (registered with
 *         MELE_Register and MELE_CreateLink), unless the link had
 *         not actually come up yet, in which case no event will be
 *         generated. The handler is free upon return from this function.
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because the
 *         link is the process of coming up or being canceled. If the link
 *         is coming up the handler is not free and caller should wait
 *         until the MELEE_LINK_CONNECT_CNF event has been received. If
 *         the link is being canceled the handler is already free.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         were not properly initialized or invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - the operation failed. The link is already
 *         disconnected, disconnect has already been issued for this handler,
 *         the handler is not bound to the link or the handler does not identify
 *         an existing link.
 */
BtStatus MELE_DisconnectLink(MeLeLinkHandler* handler);
#define MELE_DisconnectLink(ha) \
    (MELE_ForceDisconnectLinkWithReason(ha, 0, BEC_USER_TERMINATED, FALSE))

/*---------------------------------------------------------------------------
 * MELE_DisconnectLinkEx()
 *
 *     Request disconnection of the link, using the default reason code
 *     (BEC_USER_TERMINATED). This function behaves the same as 
 *     MELE_DisconnectLink, except that the link will not be closed if the 
 *     remote device initiated the connection.  However, the connection can 
 *     always be forced down by using the other disconnect API's. 
 *
 *     See MELE_DisconnectLink for additional information.
 *
 * Parameters:
 *     handler - pointer to registered handler.
 *
 * Returns:
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         were not properly initialized or invalid (XA_ERROR_CHECK only).
 *
 *     See MELE_DisconnectLink for additional returns.
 */
BtStatus MELE_DisconnectLinkEx(MeLeLinkHandler* handler);
#define MELE_DisconnectLinkEx(ha) \
    (MELE_ForceDisconnectLinkWithReasonEx(ha, 0, BEC_USER_TERMINATED, FALSE))

/*---------------------------------------------------------------------------
 * MELE_CancelCreateLink()
 *
 *     Request cancellation of the pending MELE_CreateLink. This function
 *     also deregisters the specified handler, which was previously
 *     registered when the ME_CreateLink was initiated. If other handlers
 *     are bound with the link being created the MELE_CreateLink will
 *     not be canceled.
 *
 *     Note: This API shall not be used if the BlueSDK Connection Manager
 *     is enabled. The Connection manager uses this API, acts on events
 *     generated by this API and assumes that it is the sole user of it.
 *
 * Parameters:
 *     handler - pointer to registered handler.
 *
 *     bdAddr - pointer to BtDevAddr of remote device to be disconnected.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - the operation has completed successfully.
 *         The caller's handler is unbound from the link. If more than one
 *         handler is bound to the link, no further events will occur.
 *         Otherwise, the MELEE_LINK_CREATE_CANCEL event will be sent to
 *         all registered LE handlers when the cancel completes. The
 *         LE handler must still check the MeLeEvent "errCode" field
 *         in the event structure, as the cancel may fail and the link may
 *         require further maintenance. The handler is free upon return
 *         from this function.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         were not properly initialized or invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - the operation failed. The link is already
 *         connected or disconnected, or the handler is not bound to
 *         the link.
 */
BtStatus MELE_CancelCreateLink(MeLeLinkHandler* handler, BtDevAddr *bdAddr);

/*---------------------------------------------------------------------------
 * MELE_GetWhiteListSize()
 *
 *     Get the total number of white list entries that can be stored in
 *     the LE Controller.
 *
 * Parameters:
 *     result - pointer to one octet of memory to receive the result
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation is successful the result has been copied
 *         to "result".
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameter
 *         was invalid (XA_ERROR_CHECK only).
 *
 */
BtStatus MELE_GetWhiteListSize(U8 *result);

/*---------------------------------------------------------------------------
 * MELE_ClearWhiteList()
 *
 *     Request that the LE controller's white list be cleared.
 *
 *     Note: This API shall not be used if the BlueSDK Connection Manager
 *     is enabled. The Connection manager uses this API, acts on events
 *     generated by this API and assumes that it is the sole user of it.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     BT_STATUS_PENDING - Clear white list operation has been started. When
 *     complete MELEE_CLEAR_WHITE_LIST event will be sent to all registered
 *     LE handlers
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - A white list operation is already in
 *         progress (clear, add or remove).  Only one white list operation
 *         can be in progress at a time. Keep track of the ME LE events to
 *         see when the white list operation is complete.
 */
BtStatus MELE_ClearWhiteList(void);

/*---------------------------------------------------------------------------
 * MELE_AddWhiteList()
 *
 *     Add a device address to the LE controller's white list.
 *
 *     Note: This API shall not be used if the BlueSDK Connection Manager
 *     is enabled. The Connection manager uses this API, acts on events
 *     generated by this API and assumes that it is the sole user of it.
 *
 * Parameters:
 *     bdAddr - device address to add
 *
 * Returns:
 *     BT_STATUS_PENDING - an add white list operation has been started. When
 *         complete MELEE_ADD_WHITE_LIST event will be sent to all registered
 *         LE handlers
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - A white list operation is already in
 *         progress (clear, add or remove).  Only one white list operation
 *         can be in progress at a time. Keep track of the ME LE events to
 *         see when the white list operation is complete.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         were not properly initialized or invalid (XA_ERROR_CHECK only).
 */
BtStatus MELE_AddWhiteList(BtDevAddr *bdAddr);

/*---------------------------------------------------------------------------
 * MELE_RemoveWhiteList()
 *
 *     Remove a device address from the LE controller's white list.
 *
 *     Note: This API shall not be used if the BlueSDK Connection Manager
 *     is enabled. The Connection manager uses this API, acts on events
 *     generated by this API and assumes that it is the sole user of it.
 *
 * Parameters:
 *     bdAddr - device address to remove
 *
 * Returns:
 *     BT_STATUS_PENDING - a remove white list operation has been started. When
 *         complete MELEE_REMOVE_WHITE_LIST event will be sent to all registered
 *         LE handlers.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - A white list operation is already in
 *         progress (clear, add or remove).  Only one white list operation
 *         can be in progress at a time. Keep track of the ME LE events to
 *         see when the white list operation is complete.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         were not properly initialized or invalid (XA_ERROR_CHECK only).
 */
BtStatus MELE_RemoveWhiteList(BtDevAddr *bdAddr);

/*---------------------------------------------------------------------------
 * MELE_SetChannelClassification()
 *
 *     Specify a channel classification for data channels based on local
 *     information. The channel classification persists until overwritten
 *     via this function. The host should call this function within 10
 *     seconds of knowing that the channel classification has changed.
 *     The interval between two successive commands to set the channel 
 *     classification shall be at least one second. 
 *
 * Parameters:
 *     channelMap - pointer to 5 octet channel Map. Channel map is 37 1-bit
 *         fields. The nth such field (in the range 0 to 36) contains the value
 *         for the link layer channel index n. Channel n is bad = 0. Channel n is
 *         unknown = 1. The most significant bits are reserved and shall be set
 *         to 0. At least one channel shall be marked as unknown.
 *
 * Returns:
 *     BT_STATUS_PENDING - a Set Channel Classification operation has been
 *         started. When complete a MELEE_SET_CHAN_CLASS_COMPLETE event will
 *         be sent to all registered LE handlers.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress.  Only one operation can be in progress at a time. Keep
 *         track of the ME LE events to see when the operation is complete.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         were not properly initialized or invalid (XA_ERROR_CHECK only).
 */
BtStatus MELE_SetChannelClassification(U8 *channelMap);

/*---------------------------------------------------------------------------
 * MELE_RegisterUpdateLinkHandler()
 *
 *     Register a handler with MELE to deal with Connection Parameter Update 
 *     requests from remote slaves received on the LE signaling channel. If 
 *     no handler is registered all requests will automatically be accepted. 
 *     The registered handler will receive MELEE_UPDATE_LINK_REQ events when a 
 *     remote slave requests a connection parameter update. The handler must 
 *     call MELE_AcceptLinkUpdateReq() to accept or reject the slave's request.
 *
 *     To deregister set handler to 0. If no handler is registered requests
 *     will automatically be accepted.
 *
 * Parameters:
 *     handler - Pointer to handler with fields initialized (h.callback). Only
 *         one handler can be registered at a time. This handler is not placed on
 *         a list so it can be any handler including a registered MeLeHandler or
 *         any handler that is derived from BtHandlerEx. The handler can be set to 0.
 *
 * Returns:
 *     The previous handler or 0 if no handler was registered.
 */
BtHandlerEx *MELE_RegisterUpdateLinkHandler(BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * MELE_AcceptLinkUpdateReq()
 *
 *     Accept or reject the request from a Bluetooth 4.0 remote device to 
 *     update the link parameters. This function is called in response to 
 *     receiving the MELEE_UPDATE_LINK_REQ event.  If the request is accepted 
 *     then the the HCI LE Connection Update command with the slave's requested 
 *     parameters will be sent to the controller.
 *
 *     If a Bluetooth 4.1 controller is being used then 
 *     MELE_AcceptLinkUpdateReqEx() should be used as it can be used for 4.0 
 *     (or later) devices.
 *
 * Parameters:
 *     devAddr - device address of remote device identifying the link. The best
 *         address to use is the identity address for efficiency.
 *
 *     accept - If TRUE then the request is accepted otherwise it is rejected.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed because bdAddr does not identify a link
 *         in the proper state (waiting to respond to slave's request). 
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because bdAddr is 0
 *         (XA_ERROR_CHECK only).
 */
BtStatus MELE_AcceptLinkUpdateReq(BtDevAddr *devAddr, BOOL accept);

/*---------------------------------------------------------------------------
 * MELE_AcceptLinkUpdateReqEx()
 *
 *     Accept or reject the request from a Bluetooth 4.0 (or later) remote 
 *     device to update the link parameters. This function is called in 
 *     response to receiving the MELEE_UPDATE_LINK_REQ event.  If both devices
 *     are Bluetooth 4.1 and support the Connection Parameters Request
 *     Procedure and the request is accepted, then the HCI LE Remote 
 *     Connection Parameter Request Reply command with the requested 
 *     parameters will be sent to the controller. Otherwise the HCI LE 
 *     Remote Connection Parameter Request Negative Reply command will be 
 *     sent to the controller to reject the request to change connection 
 *     parameters.  However, it is recommended that alternate connection 
 *     parameters are provided rather than merely rejecting the request 
 *     to change connection parameters, if at all possible.
 *
 *     If either device doesn't support the Bluetooth 4.1 Connection 
 *     Parameters Request Procedure then the Bluetooth 4.0 behavior 
 *     to respond through the LE signaling channel will be used.
 *
 * Parameters:
 *     devAddr - device address of remote device identifying the link. The best
 *         address to use is the identity address for efficiency.
 *
 *     parms - pointer to new connection (link) parameters.  If the default 
 *         parameters received in the MELEE_UPDATE_LINK_REQ event are acceptable,
 *         then these default values can be used by sending parms of 0.
 *
 *     accept - If TRUE then the request is accepted otherwise it is rejected.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - request succeeded.
 *
 *     BT_STATUS_FAILED - request failed because bdAddr does not identify a link
 *         in the proper state (waiting to respond to slave's request). 
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because devAddr is 0
 *         (XA_ERROR_CHECK only).
 */
BtStatus MELE_AcceptLinkUpdateReqEx(BtDevAddr *devAddr, 
                                    MeLeUpdateLinkParms *parms, 
                                    BOOL accept);

/*---------------------------------------------------------------------------
 * MELE_UpdateLink()
 *
 *     Change the Link Layer connection parameters of an existing link.
 *     If the local device's role is master, then the LE Connection Update
 *     HCI command will be issued.  Otherwise, the L2CAP signaling channel
 *     will be used to transmit this information.
 *
 * Parameters:
 *     devAddr - device address of remote device identifying the link. The best
 *         address to use is the identity address for efficiency.
 *
 *     parms - pointer to new connection (link) parameters
 *
 * Returns:
 *     BT_STATUS_PENDING - an Update Link operation has been started.
 *         When complete MELEE_UPDATE_LINK_CNF event will be sent to all
 *         registered LE handlers and handlers bound to the link via a
 *         call to MELE_CreateLink().
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress.  Only one operation can be in progress at a time. Keep
 *         track of the ME LE events to see when the operation is complete.
 *
 *     BT_STATUS_FAILED - The operation failed because the devAddr
 *         does not correspond to an active link. The link may not exist or it
 *         may be in a state of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         were not properly initialized or invalid (XA_ERROR_CHECK only).
 */
BtStatus MELE_UpdateLink(BtDevAddr *devAddr, MeLeUpdateLinkParms *parms);

/*---------------------------------------------------------------------------
 * MELE_ReadChannelMap()
 *
 *     Read the current channel map for the LE link identified by the
 *     "bdAddr" and "addrType" of the remote device.
 *
 * Parameters:
 *     bdAddr - device address of remote device identifying the link
 *
 *     parms - pointer to new connection (link) parameters
 *
 * Returns:
 *     BT_STATUS_PENDING - an Read Channel Map operation has been started.
 *         When complete a MELEE_READ_CHANNEL_MAP event will be sent to all
 *         registered LE handlers and handlers bound to the link via a call
 *         to MELE_CreateLink().
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress.  Only one operation can be in progress at a time. Keep
 *         track of the ME LE events to see when the operation is complete.
 *
 *     BT_STATUS_FAILED - The operation failed because the bdAddr and addrType
 *         do not correspond to an active link. The link may not exist, or it
 *         may be in a state of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         were not properly initialized or invalid (XA_ERROR_CHECK only).
 */
BtStatus MELE_ReadChannelMap(BtDevAddr *bdAddr);

/*---------------------------------------------------------------------------
 * MELE_StartEncryption()
 *
 *     Authenticate the given long term key, "LTKey" associated with the
 *     remote device specified by "bdAddr" and "addrType". Once authenticated
 *     the link will be encrypted.
 *
 * Parameters:
 *     bdAddr - device address of remote device identifying the link
 *
 *     rand - pointer to 64-bit (8 octet) random number.
 *
 *     encryptDiv - 16 bit encrypted diversifier
 *
 *     LTKey - pointer to 128-bit (16 octet) long term key.
 *
 * Returns:
 *     BT_STATUS_PENDING - a Start Encryption operation has been started.
 *         When complete a MELEE_START_ENCRYPT_COMPLETE event will be sent to
 *         all registered LE handlers and handlers bound to the link via a call
 *         to MELE_CreateLink().
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress.  Only one operation can be in progress at a time. Keep
 *         track of the ME LE events to see when the operation is complete.
 *
 *     BT_STATUS_FAILED - The operation failed because the bdAddr and addrType
 *         do not correspond to an active link. The link may not exist, or it
 *         may be in a state of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         were not properly initialized or invalid (XA_ERROR_CHECK only).
 */
BtStatus MELE_StartEncryption(BtDevAddr *bdAddr, U8 *rand,
                              U16 encryptDiv,  U8 *LTKey);

/*---------------------------------------------------------------------------
 * MELE_InitOpHandler()
 *
 *     Initialize the operation handler (MeLeRandHandler or MeLeEncryptHandler.
 *
 * Parameters:
 *     handler - handler to initialize
 *
 * Returns:
 *     void
 */
void MELE_InitOpHandler(MeLeOpHandler *handler);
#define MELE_InitOpHandler(h) (h)->op.opType = BOP_NOP

/*---------------------------------------------------------------------------
 * MELE_InitLinkHandler()
 *
 *     Initialize the LE link handler.
 *
 * Parameters:
 *     handler - handler to initialize
 *
 * Returns:
 *     void
 *
 */
void MELE_InitLinkHandler(MeLeLinkHandler *handler);
#define MELE_InitLinkHandler(lh) (InitializeListEntry(&((lh)->h.node)),(lh)->remDev = 0)

/*---------------------------------------------------------------------------
 * MELE_Rand()
 *
 *     Request an 8 octet rand number form the LE controller.
 *
 * Parameters:
 *     handler - pointer to a MeLeOpHandler structure that has been initialized
 *         with MELE_InitOpHandler() (first time only). This handler can point
 *         to the same callback function as a registered handler. It will
 *         be the only handler to receive the result. The handler is "owned"
 *         by the stack and should not be modified until the MELEE_RAND_COMPLETE
 *         is received via the handler.
 *
 * Returns:
 *     BT_STATUS_PENDING - a Rand operation has been started. When complete a
 *         MELEE_RAND_COMPLETE event will be sent to the handler.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_FAILED - The operation failed because the handler appears to
 *         be in use  or not initialized.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the handler is 0
 *         (XA_ERROR_CHECK only).
 */
BtStatus MELE_Rand(MeLeOpHandler *handler);

/*---------------------------------------------------------------------------
 * MELE_Encrypt()
 *
 *     Request that a 16 octet of plain text data be encrypted using the
 *     specified key.
 *
 * Parameters:
 *     handler - pointer to a MeLeOpHandler structure that has been initialized
 *         with MELE_InitOpHandler() (first time only) and pointers to the key
 *         plain text data. This handler can point to the same callback
 *         function  as a registered handler. It will be the only handler to
 *         receive the completion event. The handler is "owned" by the stack and
 *         should not be modified until the MELEE_ENCRYPT_COMPLETE event is
 *         received via the handler. Handler must be initialized with desired
 *         key and plain text data before calling this function. The key and plain
 *         text data will not be modified by the stack.
 *
 *         Note that the key and plain text data might not be copied immediately so
 *         they MUST persist for as long as the handler is owned by the stack.
 *
 * Returns:
 *     BT_STATUS_PENDING - an Encrypt operation has been started. When complete
 *         a MELEE_ENCRYPT_COMPLETE event will be sent to the handler.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_FAILED - The operation failed because the handler appears to
 *         be in used or not initialized.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the handler is 0
 *         or p.encrypt parameters are 0 (XA_ERROR_CHECK only).
 */
BtStatus MELE_Encrypt(MeLeOpHandler *handler);

/*---------------------------------------------------------------------------
 * MELE_CheckRadioFeature()
 *
 *     Indicates whether the local LE radio supports the specified feature.
 *
 * Parameters:
 *     feature - Feature code
 *
 * Returns:
 *     TRUE if LE radio indicates its support for the feature. FALSE if not.
 *
 */
BOOL MELE_CheckRadioFeature(BtLeRadioFeature feature);

/*---------------------------------------------------------------------------
 * MELE_CheckRemoteFeature()
 *
 *     Indicates whether the remote LE device supports the specified feature.
 *     A link to the remote device must exist in order to check the feature.
 *
 * Parameters:
 *     bdAddr - device address of remote device
 *
 *     feature - Feature code
 *
 *     result - pointer to memory to receive the result
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation is successful the result has been copied
 *         to "result". Result is TRUE if LE radio indicates its support for
 *         the feature. FALSE if not.
 *
 *     BT_STATUS_FAILED - The operation failed because the bdAddr and addrType
 *         do not correspond to an active link. The link may not exist, or it
 *         may be in a state of coming up or being disconnected.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         were not properly initialized or invalid (XA_ERROR_CHECK only).
 *
 */
BtStatus MELE_CheckRemoteFeature(BtDevAddr *bdAddr,
                                 BtLeRadioFeature feature, BOOL *result);

/*---------------------------------------------------------------------------
 * MELE_GetAdvertChannelTxPower()
 *
 *     Get the transmit power level used for LE advertising channel packets.
 *     Range is -20 <= N <= 10 in dBm. The accuracy is + or - 4 dBm.
 *
 * Parameters:
 *     result - pointer to one octet of memory to receive the result
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation is successful the result has been copied
 *         to "result".
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameter
 *         was invalid (XA_ERROR_CHECK only).
 *
 */
BtStatus MELE_GetAdvertChannelTxPower(U8 *result);

/*---------------------------------------------------------------------------
 * MELE_CheckRadioState()
 *
 *     Indicates whether the local LE radio supports the specified state.
 *
 * Parameters:
 *     state - State code
 *
 * Returns:
 *     TRUE if LE radio indicates its support for the state. FALSE if not.
 *
 */
BOOL MELE_CheckRadioState(BtLeRadioState state);

/*---------------------------------------------------------------------------
 * MELE_ReceiverTest()
 *
 *     Start a test where the DUT receives test reference packets at a fixed
 *     interval. The tester generates the test reference packets.
 *
 * Parameters:
 *     handler - pointer to a MeLeOpHandler structure that has been initialized
 *         with MELE_InitOpHandler() (first time only). This handler can point
 *         to the same callback function as a registered handler. It will
 *         be the only handler to receive the result. The handler is "owned"
 *         by the stack and should not be modified until the MELEE_RX_TEST_COMPLETE
 *         is received via the handler. Handler must be initialized with desired
 *         p.rxFrequency before calling this function.
 *
 * Returns:
 *     BT_STATUS_PENDING - a Receiver Test operation has been started. When
 *         complete a MELEE_RX_TEST_COMPLETE event will be sent to the handler.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_FAILED - The operation failed because the handler appears to
 *         be in used or not initialized.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the handler is 0,
 *         handler.h.callback is 0 or p.rxFrequency is invalid (XA_ERROR_CHECK only).
 */
BtStatus MELE_ReceiverTest(MeLeOpHandler *handler);

/*---------------------------------------------------------------------------
 * MELE_TransmitterTest()
 *
 *     Start a test where the DUT generates test reference packets at a fixed
 *     interval. The Controller shall transmit at maximum power.
 *
 * Parameters:
 *     handler - pointer to a MeLeOpHandler structure that has been initialized
 *         with MELE_InitOpHandler() (first time only). This handler can point
 *         to the same callback function as a registered handler. It will
 *         be the only handler to receive the result. The handler is "owned"
 *         by the stack and should not be modified until the MELEE_TX_TEST_COMPLETE
 *         is received via the handler. Handler must be initialized with desired
 *         p.txTest.txFrequency, p.txTest.testDataLen and p.txTest.packetPayload
 *         before calling this function.
 *
 * Returns:
 *     BT_STATUS_PENDING - a Transmitter Test operation has been started. When
 *         complete a MELEE_TX_TEST_COMPLETE event will be sent to the handler.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_FAILED - The operation failed because the handler appears to
 *         be in used or not initialized.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the handler is 0,
 *         handler.h.callback is 0, or txTest parms are invalid (XA_ERROR_CHECK only).
 */
BtStatus MELE_TransmitterTest(MeLeOpHandler *handler);

/*---------------------------------------------------------------------------
 * MELE_TestEnd()
 *
 *     Used to stop any test which is in progress. The number of packet for
 *     a receiver test is returned. The number of packets for a transmitter
 *     test is reported as 0.
 *
 * Parameters:
 *     handler - pointer to a MeLeOpHandler structure that has been initialized
 *         with MELE_InitOpHandler() (first time only). This handler can point
 *         to the same callback function as a registered handler. It will
 *         be the only handler to receive the result. The handler is "owned"
 *         by the stack and should not be modified until the MELEE_TEST_END_COMPLETE
 *         is received via the handler.
 *
 * Returns:
 *     BT_STATUS_PENDING - a Test end operation has been started. When complete
 *         a MELEE_TEST_END_COMPLETE event will be sent to the handler.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_FAILED - The operation failed because the handler appears to
 *         be in used or not initialized.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the handler is 0,
 *         or handler.h.callback is 0 (XA_ERROR_CHECK only).
 */
BtStatus MELE_TestEnd(MeLeOpHandler *handler);

/*---------------------------------------------------------------------------
 * MELE_GetRemDevEncryptState()
 *
 *     Get the encryption state of the remote device. This function
 *     does not check for a valid remDev pointer.
 *
 * Parameters:
 *     remDev - pointer to remDev.
 *
 * Returns:
 *     Zero for no encryption, non-zero for encryption.
 */
MeLeEncryptState MELE_GetRemDevEncryptState(BtRemoteDevice *remDev);
#define MELE_GetRemDevEncryptState(r) (((MeLeRemoteDevice *)r)->encryptState)

/*---------------------------------------------------------------------------
 * MELE_FindRemoteDevice()
 *
 *     Return a pointer to the BtRemoteDevice structure corresponding to
 *     the device with given BD_ADDR. 0 is returned if a match is not found.
 *
 *     When this function returns successfully, the BtRemoteDevice structure
 *     returned always refers to a connected or connecting device. Use
 *     caution, as it will also return successfully if the device is in the
 *     process of disconnecting, but has not yet completed. This typically
 *     occurs if called during the notification of a link failure.
 *
 * Parameters:
 *     bdAddr - pointer to 48-bit address and address type of the device
 *
 * Returns:
 *     Pointer to BtRemoteDevice structure if found. 0 is returned if the
 *     device is not found.
 */
BtRemoteDevice *MELE_FindRemoteDevice(BtDevAddr *bdAddr);

/* Prototype for actual MELE_FindRemoteDevice implementation */
#define MELE_FindRemoteDevice(bd) (BtRemoteDevice *)(MeLeFindRemoteDevice(bd))

/*---------------------------------------------------------------------------
 * MELE_RequestAdvertDataUpdate()
 *
 *  Schedule a task to indicate the necessity of an Advertising Data update
 *  to the global handler. This is typically invoked internally in the stack
 *  components when Advertising Data associated values have been changed.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     void
 */
void MELE_RequestAdvertDataUpdate(void);

/*---------------------------------------------------------------------------
 * MELE_RequestGapServiceUpdate()
 *
 *  Schedule a task to indicate the necessity of a GAP Service update
 *  to the global handler. This is typically invoked internally in the stack
 *  components when GAP Service associated values have been changed.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     void
 */
void MELE_RequestGapServiceUpdate(void);

/*---------------------------------------------------------------------------
 * MELE_GetIdentityAddr()
 *
 *     Get the 48-bit identity address and type of the remote device. The
 *     value is meaningless if a connection does not exist to the remote
 *     device.
 *
 * Parameters:
 *     rm - pointer to remote device.
 *
 *     idAddr - pointer to a BtDevAddr structure to receive the result.
 */
void MELE_GetIdentityAddr(BtRemoteDevice *rm, BtDevAddr *idAddr);

/*---------------------------------------------------------------------------
 * MELE_ReadLocalP256Key()
 *
 *     Get the local P-256 public key from the controller. The controller will
 *     generate a new P-256 public/private key pair upon. 
 *
 * Parameters:
 *     handler - pointer to a MeLeOpHandler structure that has been initialized
 *         with MELE_InitOpHandler() (first time only). This handler can point 
 *         to the same callback function as a registered handler. It will be the 
 *         only handler to receive the completion event. The handler is "owned" 
 *         by the stack and should not be modified until the 
 *         MELEE_P256_KEY_COMPLETE event is received via the handler. 
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation has been started. When complete
 *         a MELEE_P256_KEY_COMPLETE event will be sent to the handler.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_FAILED - The operation failed because there is already an operation
 *         in progress or the handler appears to be in used or not is initialized.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the handler is 0
 *         or p.P256PublicKey is 0 (XA_ERROR_CHECK only).
 */
BtStatus MELE_ReadLocalP256Key(MeLeOpHandler *handler);

/*---------------------------------------------------------------------------
 * MELE_GenerateDHKey()
 *
 *     Initiate generation of a Diffie Hellman key for use over the LE 
 *     transport. It take the remote P-256 public key as input. 
 *
 * Parameters:
 *     handler - pointer to a MeLeOpHandler structure that has been initialized
 *         with MELE_InitOpHandler() (first time only) and pointers to a P-256
 *         Public key, "p.P256PublicKey". This handler can point to the same 
 *         callback function  as a registered handler. It will be the only 
 *         handler to receive the completion event. The handler is "owned" by 
 *         the stack and should not be modified until the 
 *         MELEE_GEN_DHKEY_COMPLETE event is received via the handler. The
 *         handler must be initialized with the desired key before calling 
 *         this function. The key will not be modified by the stack.
 *
 *         Note that the public key MUST persist for as long as the 
 *         handler is owned by the stack.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation has been started. When complete
 *         a MELEE_GEN_DHKEY_COMPLETE event will be sent to the handler.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_FAILED - The operation failed because the handler appears to
 *         be in used or not initialized.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the handler is 0
 *         or p.genDHKey parameters are 0 (XA_ERROR_CHECK only).
 */
BtStatus MELE_GenerateDHKey(MeLeOpHandler *handler);


/*---------------------------------------------------------------------------
 * MELE_SetDataLength()
 *
 *     Suggest the maximum transmission packet size and maximum packet 
 *     transmission time to be used for the connection with the remote device
 *     with address "devAddr". The Controller may use smaller or larger
 *     values based on local information. 
 *
 * Parameters:
 *     devAddr - pointer to the device address of the remote device which
 *         identifies the connection. 
 *
 *     maxTxOctets - the maximum number of octets. The valid range is 
 *         LE_MIN_DATA_OCTETS through LE_MAX_DATA_OCTETS.
 *
 *     maxTxTime - the maximum time in milliseconds. The valid range is 
 *         LE_MIN_DATA_TIME through LE_MAX_DATA_TIME.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation has been started successfully. When
 *         complete MELEE_DATA_LENGTH_COMPLETE event will be sent to all
 *         registered LE handlers and handlers bound to the link via a
 *         call to MELE_CreateLink().
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress. Keep track of the ME LE events to see when the
 *         operation is complete.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because there is no connection
 *         or the connection is in the process of being disconnected.
 *
 *     BT_STATUS_NOT_SUPPORTED - Setting the data length is not supported
 *         by the controller.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         were not properly initialized or invalid (XA_ERROR_CHECK only).
 */
BtStatus MELE_SetDataLength(BtDevAddr *devAddr, U16 maxTxOctets, U16 maxTxTime);

/*---------------------------------------------------------------------------
 * MELE_SetDefaultDataLength()
 *
 *     Set the host preferred values for the Controller's maximum transmission
 *     number of payload octets and maximum packet transmission time to be used
 *     for new connections. The controller may use smaller or larger values based
 *     on local information.
 *
 * Parameters:
 *     maxTxOctets - the suggested maximum number of octets. The valid range is
 *         LE_MIN_DATA_OCTETS through LE_MAX_DATA_OCTETS.
 *
 *     maxTxTime - the suggested maximum time in milliseconds. The valid range is
 *         LE_MIN_DATA_TIME through LE_MAX_DATA_TIME.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation has been started successfully. When
 *         complete the MELEE_DEFAULT_DATA_LENGTH_COMPLETE event will be sent 
 *         to all registered LE handlers.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_NOT_SUPPORTED - Setting the default data length is not 
 *         supported by the controller.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress.  Only one operation can be in progress at a time. Keep
 *         track of the ME LE events to see when the operation is complete.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         are not invalid (XA_ERROR_CHECK only).
 */
BtStatus MELE_SetDefaultDataLength(U16 maxTxOctets, U16 maxTxTime);

/*---------------------------------------------------------------------------
 * MELE_GetDefaultDataLength()
 *
 *     Get the host preferred values for the Controller's maximum transmission
 *     number of payload octets and maximum packet transmission time to be used
 *     for new connections. The controller may use smaller or larger values based
 *     on local information.
 *
 * Parameters:
 *     maxTxOctets - pointer to memory to receive the current settings for the 
 *         suggested maximum number of octets. Can be 0 if this value is not
 *         desired. 
 *
 *     maxTxTime - pointer to memory to receive the current settings for the
 *         suggested maximum time in milliseconds. Can be 0 if this value is
 *         not desired
 *
 * Returns:
 *     void
 */
void MELE_GetDefaultDataLength(U16 *maxTxOctets, U16 *maxTxTime);

/*---------------------------------------------------------------------------
 * MELE_GetDefaultDataLength()
 *
 *     Get the controller's maximum supported payload octets and packet duration
 *     times for transmission and reception. 
 *
 * Parameters:
 *     parms - pointer to memory to receive the values. 
 *
 * Returns:
 *     void
 */
void MELE_GetMaximumDataLength(MeLeDataLengthParms *parms);

#if LL_PRIVACY == XA_ENABLED
/*---------------------------------------------------------------------------
 * MELE_AddResolvingList()
 *
 *     Add a device address to the LE controller's resolving list.
 *
 *     Note: This API shall not be used if the BlueSDK Connection Manager
 *     is enabled. The Connection manager uses this API, acts on events
 *     generated by this API and assumes that it is the sole user of it.
 *
 * Parameters:
 *     identityAddr - identity address of the remote device to add
 *
 *     peerIrk - 16-byte identity resolving key (IRK) of the peer device
 *
 *     localIrk - 16-byte identity resolving key (IRK) of the local device
 *
 * Returns:
 *     BT_STATUS_PENDING - an add resolving list operation has been started. 
 *         When complete MELEE_ADD_RESOLVING_LIST event will be sent to all 
 *         registered LE handlers
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - A resolving list operation is already in
 *         progress (clear, add or remove).  Only one resolving list operation
 *         can be in progress at a time. Keep track of the ME LE events to
 *         see when the resolving list operation is complete.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         were not properly initialized or invalid (XA_ERROR_CHECK only).
 */
BtStatus MELE_AddResolvingList(BtDevAddr *identityAddr, U8 peerIrk[16], 
                               U8 localIrk[16]);

/*---------------------------------------------------------------------------
 * MELE_RemoveResolvingList()
 *
 *     Remove an identity device address from the LE controller's resolving 
 *     list.
 *
 *     Note: This API shall not be used if the BlueSDK Connection Manager
 *     is enabled. The Connection manager uses this API, acts on events
 *     generated by this API and assumes that it is the sole user of it.
 *
 * Parameters:
 *     identityAddr - identity address of the remote device to remove
 *
 * Returns:
 *     BT_STATUS_PENDING - a remove resolving list operation has been started. When
 *         complete MELEE_REMOVE_RESOLVING_LIST event will be sent to all registered
 *         LE handlers.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - A resolving list operation is already in
 *         progress (clear, add or remove).  Only one resolving list operation
 *         can be in progress at a time. Keep track of the ME LE events to
 *         see when the resolving list operation is complete.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         were not properly initialized or invalid (XA_ERROR_CHECK only).
 */
BtStatus MELE_RemoveResolvingList(BtDevAddr *identityAddr);

/*---------------------------------------------------------------------------
 * MELE_ClearResolvingList()
 *
 *     Request that the LE controller's resolving list be cleared.
 *
 *     Note: This API shall not be used if the BlueSDK Connection Manager
 *     is enabled. The Connection manager uses this API, acts on events
 *     generated by this API and assumes that it is the sole user of it.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     BT_STATUS_PENDING - Clear resolving list operation has been started. When
 *         complete MELEE_CLEAR_RESOLVING_LIST event will be sent to all registered
 *         LE handlers
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - A resolving list operation is already in
 *         progress (clear, add or remove).  Only one resolving list operation
 *         can be in progress at a time. Keep track of the ME LE events to
 *         see when the resolving list operation is complete.
 */
BtStatus MELE_ClearResolvingList(void);

/*---------------------------------------------------------------------------
 * MELE_GetResolvingListSize()
 *
 *     Get the total number of resolving list entries that can be stored in
 *     the LE Controller.
 *
 * Parameters:
 *     result - pointer to one octet of memory to receive the result
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation is successful the result has been copied
 *         to "result".
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameter
 *         was invalid (XA_ERROR_CHECK only).
 *
 */
BtStatus MELE_GetResolvingListSize(U8 *result);

/*---------------------------------------------------------------------------
 * MELE_ReadResolvableAddress()
 *
 *      Get the resolvable private address the controller is currently
 *      using for the given peer identity address.
 *
 * Parameters:
 *      handler - operation handler pointer
 *
 * Returns:
 *     BT_STATUS_PENDING - Read peer RPA operation has started. When
 *         complete MELEE_READ_RESOLVABLE_ADDR_COMPLETE event will be sent to all 
 *         registered LE handlers
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress.  Only one operation can be in progress at a time. Keep
 *         track of the ME LE events to see when the operation is complete.
 */
BtStatus MELE_ReadResolvableAddress(MeLeOpHandler *handler, BOOL local);

/*---------------------------------------------------------------------------
 * MELE_SetAddressResolutionEnable()
 *
 *     Request the LE Controller to start or stop using resolvable addresses
 *     for advertising, scanning, and initiating.
 *
 *     Once address resolution is enabled, the controller will generate
 *     resolvable private addresses for the local device if a local IRK is
 *     stored in the resolving list. Also, the controller will attempt to 
 *     resolve all incoming peer addresses using the entries in the
 *     resolving list.
 *
 *     Note: This API shall not be used if the BlueSDK Connection Manager
 *     is enabled. The Connection manager uses this API, acts on events
 *     generated by this API and assumes that it is the sole user of it.
 *
 * Parameters:
 *     enable - Indicates if address resolution should be enabled or disabled.
 *
 * Returns:
 *     BT_STATUS_PENDING - The Address Resolution enable command is being 
 *         sent to the LE controller.  When complete, an 
 *         MELEE_ENABLE_ADDR_RESOLUTION_CNF event will be sent to all 
 *         registered handlers.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 *
 *     BT_STATUS_IN_PROGRESS - An operation of this type is already in
 *         progress.  Only one operation can be in progress at a time. Keep
 *         track of the ME LE events to see when the operation is complete.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the parameters
 *         were invalid (XA_ERROR_CHECK only).
 */
BtStatus MELE_SetAddressResolutionEnable(MeLeEnable enable);

/*---------------------------------------------------------------------------
 * MELE_SetResolvableAddressTimeout
 *
 *      Sets the length of time the controller uses a resolvable private
 *      address before a new address is generated and starts being used.
 * 
 * Parameters:
 *      timeout - time in seconds, valid range 0x0001 - 0xA1B8
 *
 * Returns:
 *      BT_STATUS_PENDING - The set resolvable address timeout command is
 *          being sent to the LE controller. When complete, an 
 */
BtStatus MELE_SetResolvableAddressTimeout(U16 timeout);

#endif /* LL_PRIVACY == XA_ENABLED */

#endif /* __MELE_H */
