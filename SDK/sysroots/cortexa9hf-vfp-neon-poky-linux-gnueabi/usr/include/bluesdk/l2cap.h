/****************************************************************************
 *
 * File:
 *     l2cap.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     upper layer of the Logical Link Control and Adaptation Protocol.
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

#ifndef __L2CAP_H
#define __L2CAP_H

#include "osapi.h"
#include "bttypes2.h"
#include "me.h"
#include "sys/loglink.h"

/*---------------------------------------------------------------------------
 * Logical Link Control and Adaptation Protocol (L2CAP) layer
 *
 *     The L2CAP layer handles all Bluetooth data communications that
 *     take place over ACL links. It is primarily a protocol multiplexer,
 *     allowing protocols such as RFCOMM, TCS Binary, and SDP to share
 *     a single device-to-device connection. The L2CAP layer also allows
 *     a higher layer protocol to communicate with several other devices
 *     over separate "channels".
 *
 *     L2CAP supports segmentation and reassembly, which is the process
 *     of breaking down transmit packets into an appropriate size for
 *     HCI, and reassembling received packets for higher layers. The
 *     Maximum Transmission Unit (MTU) represents the largest possible
 *     reassembled L2CAP packet.
 *
 *     L2CAP handles channel configuration. This includes settings
 *     for the default L2CAP payload size, timeouts for missing data,
 *     and "quality of service" requirements for data rate, peak bandwidth,
 *     latency, and delay.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * L2CAP_DEFAULT_MTU constant
 *
 *     Defines the MTU size assumed for devices that do not specify an MTU
 *     size explicitly. This value is defined by the Bluetooth
 *     specification as 672 bytes.
 */
#define L2CAP_DEFAULT_MTU           0x02A0


/*---------------------------------------------------------------------------
 * L2CAP_MINIMUM_MTU constant
 *
 *     Defines the minimum allowed MTU size for all L2CAP implementations.
 *     This value is defined by the Bluetooth specification as 48 bytes.
 */
#define L2CAP_MINIMUM_MTU           0x0030


/*---------------------------------------------------------------------------
 * L2CAP_MAXIMUM_MTU constant
 *
 *     Defines the maximum receive SDU size allowed for this L2CAP
 *     implementation when the L2CAP user is not able to receive SDUs in
 *     fragments. This is based on the L2CAP_MTU configuration value.
 *     Note that this value may be less than the L2CAP_DEFAULT_MTU value
 *     to save RAM.
 */
#define L2CAP_MAXIMUM_MTU           L2CAP_MTU


/*---------------------------------------------------------------------------
 * L2CAP_FRAG_MAXIMUM_MTU constant
 *
 *     Defines the maximum receive SDU size allowed for this L2CAP
 *     implementation when the L2CAP user is able to receive SDUs in 
 *     fragments. This is based on the L2CAP_FRAG_MTU configuration value.
 *     Note that this value may be less than the L2CAP_DEFAULT_MTU value
 *     to save RAM.
 */
#define L2CAP_FRAG_MAXIMUM_MTU      L2CAP_FRAG_MTU

/*---------------------------------------------------------------------------
 * L2CAP_SIGNAL_MTU_DEFAULT constant
 *
 *     Defines the minimum MTU for signaling frame when the ACL does not
 *     support the Extended Flow Specification. Since this is a minimum
 *     value it seem reasonable that an implementation can support a larger
 *     signaling MTU.
 */
#define L2CAP_SIGNAL_MTU_DEFAULT  48

/*---------------------------------------------------------------------------
 * L2CAP_SIGNAL_MTU_FLOW_SPEC constant
 *
 *     Defines the minimum MTU for a signaling frame when the ACL supports the 
 *     Extended Flow Specification. Since this is a minimum
 *     value it seem reasonable that an implementation can support a larger
 *     signaling MTU.
 */
#define L2CAP_SIGNAL_MTU_FLOW_SPEC 672

/*---------------------------------------------------------------------------
 * L2FIXED_MASK constant
 *
 *     Defines the Fixed Channels supported by this implementation.
 *     Default: Byte 0 = 0x8E
 *        Signaling (0x02) | Connectionless (0x04) | AMP Manager (0x08) | Security (0x80)
 */
#define L2FIXED_MASK        {0x8E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}


/*---------------------------------------------------------------------------
 * L2LEFIXED_MASK constant
 *
 *     Defines the LE Fixed Channels supported by this implementation.
 *     Default: Byte 0 = 0x72
 *        Signaling (0x02) | LE AT (0x10) |
 *        LE Signaling (0x20) | LE Security (0x40)
 */
#define L2LEFIXED_MASK      {0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}


/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * L2capEventType type
 *
 *     Indicates the type of an L2CAP callback's event during an
 *     L2capCallback function call. These events are delivered in the
 *     L2capCallbackParms structure's "event" field.
 *
 *     Depending on the event, a specific response may be required or
 *     certain fields in the L2capCallbackParms structure may be valid or
 *     invalid. The valid fields and required responses are described
 *     for each event below.
 */
typedef U8 L2capEventType;

/* A remote device has requested a new L2CAP connection (channel). The
 * L2CAP user must call L2CAP_ConnectRsp to confirm or deny the connection
 * request.
 */
#define L2EVENT_CONNECT_IND      1

/* The channel is connected and ready to exchange data. This event occurs
 *  regardless of which device initiated the connection.
 */
#define L2EVENT_CONNECTED        2

/* The remote device requested that its channel be disconnected. L2CAP
 * will respond with a disconnection response, and send the DISCONNECTED
 * event when the disconnection process is complete.
 */
#define L2EVENT_DISCON_IND       3

/* The channel is disconnected. The callback's "status" field contains
 * the reason for the disconnection and is of type L2capDiscReason, not
 * BtStatus. If the L2capDiscReason is L2DISC_LINK_DISCON, then the
 * "errCode" field will contain the ACL disconnect reason of type
 * BtErrorCode. If the L2capDiscReason is L2DISC_SECURITY_BLOCK, then the
 * "errCode" field will contain the ACL security failure reason of type
 * BtErrorCode.
 */
#define L2EVENT_DISCONNECTED     4

/* Channel data has been received. The callback "ptrs.data" field
 * contains the received data, and "dataLen" indicates the data length.
 */
#define L2EVENT_DATA_IND         5

/* A data packet has been sent. The application may re-use or deallocate
 * the packet in the "ptrs.packet" field. The status of the transmit is
 * provided in the "status" field of the callback parameters structure.
 */
#define L2EVENT_PACKET_HANDLED   6

/* A request is complete. Different L2CAP operations may generate a
 * COMPLETE event, depending on configuration options. (See L2CAP_Ping,
 * L2CAP_GetInfo, and L2CAP_ConnectRsp for more information.)
 *
 * During this event, the "status" field contains the results of the
 * request.
 */
#define L2EVENT_COMPLETE         7

/* A Quality-of-Service violation was detected. It is up to the
 * application to take any action based on this event.
 */
#define L2EVENT_QOS_VIOLATION    8

/* The Channel configuration parameters are being negotiated. This event
 * is generated when a connected channel enters renegotiation. No data can
 * be sent while a channel is negotiating its configuration.
 */
#define L2EVENT_CONFIGURING      9

/* If a channel undergoes configuration renegotiation (as indicated
 * by the L2EVENT_CONFIGURING event) this event will be indicated
 * once the configuration negotiation is complete. This event is roughly
 * equivalent to L2EVENT_CONNECTED, and indicates that data transmission
 * can now be resumed.
 */
#define L2EVENT_RECONFIGURED    10

/* The Flush Timeout command was sent to HCI. The "flushTimeout" field
 * indicates the flush timeout value used. The BTEVENT_HCI_COMMAND_SENT
 * event will confirm success or failure of this command.
 */
#define L2EVENT_FLUSH_TIMEOUT   13

/* The Flow Specification command was sent to HCI. The
 * "flowSpec" field indicates the flow specification used. The
 * BTEVENT_QOS_SETUP_COMPLETE event will confirm success or
 * failure of this command.
 */
#define L2EVENT_FLOW_SPEC       14

/* Advises the upper layer of the optimal channel SDU size to achieve
 *  overall system QoS requirements. If the channel is an Enhanced mode
 *  channel, this event notifies the upper layer an automatic MPS size
 *  adjustment has occurred. In this case, the upper layer should adjust
 *  its SDU size to better utilize the current MPS size on the Enhanced
 *  mode channel. If the channel is in Basic Mode, the upper layer must
 *  adjust its SDU size to the size indicated in this event in order to
 *  maintain system QoS. The new SDU/MPS size is indicated in the
 *  "dataLen" field of the event parameters.
 *
 */
#define L2EVENT_ADJUST_SDU_SIZE  15

/* Group: The following events occur only on L2CAP Enhanced mode channels
 *  with flowParms.fragmentsEnabled set to TRUE and BASIC mode channels
 *  (including connectionless) with rxFragments set to L2CAP_FRAG_ENABLED 
 *  in the registered L2capPsm structure.
 *
 */

/* Fragmented channel data has been received. The callback
 * "ptrs.data" field contains the received data, "dataLen" field
 * indicates the data length of this fragment, and "sduLen"
 * field indicates total length of all fragments.
 */
#define L2EVENT_FRAGMENT_IND    16

/* The last fragment of fragmented channel data has been received. 
 * The callback "ptrs.data" field contains the received data, "dataLen" 
 * field indicates the data length of this fragment, and "sduLen" field 
 * indicates total length of all fragments. In the case of a BASIC mode 
 * or Streaming mode channel that can handle missing or corrupt packets 
 * if the Event "status" field is not BT_STATUS_SUCCESS then packet has 
 * errors and some frames of the SDU are missing.
 */
#define L2EVENT_LAST_FRAG_IND   17

/* A Move Channel operation has completed. This event occurs regardless
 *  of which device initiated the move. The Event "status" field indicates
 *  the result of the Move Channel operation. If the result is success,
 *  the "dataLen" field indicates the maximum PDU size allowed on the new
 *  Physical Link. This "dataLen" information may be used to adjust the
 *  SDU size (within the limits of the current MTU) to optimize use of
 *  bandwidth. The application must not perform any operation within the
 *  context of this event.
 */
#define L2EVENT_MOVE_CHAN_CNF   18

#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
/* A credit packet has been received on an LE L2CAP Dyanmic Channel.
 *  The callback "ptrs.rcvdTxCredits" field contains the credit value received.
 */
#define L2EVENT_CREDIT_IND      19
#endif /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */

/* End of L2capEventType */

/*---------------------------------------------------------------------------
 * L2capDiscReason type
 *
 *     Indicates the reason for a disconnection. The reason is delivered
 *     during an L2EVENT_DISCONNECTED callback, and can be found in the
 *     L2capCallbackParms "status" field.
 */
typedef U16 L2capDiscReason;

/* Disconnection occurred for an unknown reason. */
#define L2DISC_REASON_UNKNOWN            0x0000

/* The local or remote device requested the disconnection. */
#define L2DISC_USER_REQUEST              0x0001

/* An L2CAP request timed out. */
#define L2DISC_REQUEST_TIMEDOUT          0x0002

/* The underlying ACL Link was disconnected. */
#define L2DISC_LINK_DISCON               0x0003

/* The connection was terminated due to a QOS violation. */
#define L2DISC_QOS_VIOLATION             0x0004

/* The local security manager refused the connection attempt. */
#define L2DISC_SECURITY_BLOCK            0x0005

/* The remote device does not support the requested protocol service (PSM). */
#define L2DISC_CONN_PSM_UNSUPP           0x0042

/* The remote device's security architecture denied the connection. */
#define L2DISC_CONN_SECURITY_BLOCK       0x0043

/* The remote device is out of resources. */
#define L2DISC_CONN_NO_RESOURCES         0x0044

/* The remote device rejected our source CID as invalid. */
#define L2DISC_CONN_SOURCE_CID_INVALID   0x0045

/* The remote device rejected our source CID as being already allocated. */
#define L2DISC_CONN_SOURCE_CID_ALLOCATED 0x0046

/* Configuration failed due to unacceptable parameters. */
#define L2DISC_CONFIG_UNACCEPT           0x0081

/* Configuration was rejected (unknown reason). */
#define L2DISC_CONFIG_REJECT             0x0082

/* Configuration failed due to an unrecognized configuration option. */
#define L2DISC_CONFIG_OPTIONS            0x0083

/* The maximum number of retransmissions has been exceeded (BT1.2 mode only). */
#define L2DISC_MAX_RETRANS               0x0084

/* The connection was terminated due to a protocol violation. */
#define L2DISC_PROTOCOL_ERROR            0x0085

/* The remote device does not support the requested protocol service (PSM). */
#define L2DISC_LE_CONN_PSM_UNSUPP        0x00F2

/* The remote device is out of resources. */
#define L2DISC_LE_CONN_NO_RESOURCES      0x00F4

/* The remote device denied the connection due to insufficient authentication. */
#define L2DISC_LE_CONN_INSUFF_AUTHEN     0x00F5

/* The remote device denied the connection due to insufficient authorization. */
#define L2DISC_LE_CONN_INSUFF_AUTHOR     0x00F6

/* The remote device denied the connection due to insufficient encryption key size. */
#define L2DISC_LE_CONN_INSUFF_KEY_SIZE   0x00F7

/* The remote device denied the connection due to insufficient encryption. */
#define L2DISC_LE_CONN_INSUFF_ENCRYPT    0x00F8

/* The remote device rejected our source CID as invalid. */
#define L2DISC_LE_CONN_SOURCE_CID_INVALID   0x00F9

/* The remote device rejected our source CID as being already allocated. */
#define L2DISC_LE_CONN_SOURCE_CID_ALLOCATED 0x00FA

/* End of L2capDiscReason */


/*---------------------------------------------------------------------------
 * L2capConnStatus type
 *
 *     Indicates the status of a requested connection. When the L2CAP
 *     user receives the L2EVENT_CONNECT_IND event, it must call
 *     L2CAP_ConnectRsp to provide one of the status codes below.
 */
typedef U16 L2capConnStatus;

/* The connection request is accepted. */
#define L2CONN_ACCEPTED                     0x0000

/* The connection request is pending. This might be necessary if some
 * user action is required to approve the connection.
 *
 * After the pending indication is delivered to the remote device, the local
 * protocol will receive an L2EVENT_COMPLETE event. After this event occurs
 * the protocol may re-issue the L2CAP_ConnectRsp call with one of the
 * other connection status values.
 */
#define L2CONN_PENDING                         0x0001

/* Rejects the connection request because no such protocol service exists.
 * Generally, this rejection status should only be originated by L2CAP,
 * not by protocol services.
 *
 * The L2EVENT_DISCONNECTED event will be generated after the rejection
 * has been sent.
 */
#define L2CONN_REJECT_PSM_NOT_SUPPORTED        0x0002

/* Rejects the connection request due to a security failure. The
 * L2EVENT_DISCONNECTED event will be generated after the rejection has
 * been sent.
 */
#define L2CONN_REJECT_SECURITY_BLOCK           0x0003

/* Rejects the connection request due to a resource shortage. The
 * L2EVENT_DISCONNECTED event will be generated after the rejection has
 * been sent.
 */
#define L2CONN_REJECT_NO_RESOURCES             0x0004

/* Rejects the connection request because the controller ID is not
 * supported (e.g. there is no controller with the given ID). The
 * L2EVENT_DISCONNECTED event will be generated after the rejection has
 * been sent.
 */
#define L2CONN_REJECT_CONTROL_ID_NOT_SUPPORTED 0x0005

/* Rejects the connecton request because the source CID is invalid (e.g. a
 * reserved value). 
 */
#define L2CONN_REJECT_INVALID_SOURCE_CID       0x0006

/* Rejects the connection request because the source CID is already
 * allocated 
 */
#define L2CONN_REJECT_SOURCE_CID_ALLOCATED     0x0007

/* End of L2capConnStatus */

/*---------------------------------------------------------------------------
 * L2capPsmValue type
 *
 *     Defines well-known protocol service values. These values identify
 *     protocol services that use L2CAP. When creating an L2capPsm structure
 *     for registration with L2CAP_RegisterPsm, the "psm" field must be
 *     set to one of these values.
 *
 *     Dynamically assigned PSM values are also permitted; however, they
 *     must be odd, within the range of 0x1001 to 0xFFFF, and have
 *     the 9th bit (0x0100) set to zero.
 */
typedef U16 L2capPsmValue;

/* PSMs of this type cannot receive a connection. Only clients establishing
 * outbound L2CAP connections can use it.
 */
#define BT_CLIENT_ONLY_PSM      0x0000

/* Value for a Service Discovery Protocol server */
#define BT_PSM_SDP              0x0001

/* Value for an RFCOMM server */
#define BT_PSM_RFCOMM           0x0003

/* Value for a TCS Binary server */
#define BT_PSM_TCS              0x0005

/* Value for a TCS Binary group */
#define BT_PSM_TCS_CORDLESS     0x0007

/* Value for the BNEP service */
#define BT_PSM_BNEP             0x000F

/* Value for the HID Control Channel */
#define BT_PSM_HID_CTRL         0x0011

/* Value for the HID Interrupt Channel */
#define BT_PSM_HID_INTR         0x0013

/* Value for the UPnP/ESDP service */
#define BT_PSM_UPNP             0x0015

/* Value for the A/V Control Transport Protocol signal channel */
#define BT_PSM_AVCTP            0x0017

/* Value for the A/V Distribution Transport protocol */
#define BT_PSM_AVDTP            0x0019

/* Value for the A/V Control Transport Protocol browsing channel*/
#define BT_PSM_AVCTP_BROWSING   0x001B

/* Value for Unrestricted Digital Information Control Plane protocol */
#define BT_PSM_UDI_C            0x001D

/* Servers that use dynamically allocated PSMs should register using
 * this value. The L2CAP_RegisterPsm function will assign a dynamic
 * PSM value and return it in the "psm" field.
 */
#define BT_DYNAMIC_PSM          0x1101

/* Value to register for A2MP channel commands and responses. */
#define BT_A2ME_PSM             0x1103

/* Value to register for LE ATT channel commands and responses. */
#define BT_ATT_PSM              0x001F

/* End of L2capPsmValue */

/*---------------------------------------------------------------------------
 * L2capInfoType type
 *
 *     Defines the types of information that can be queried from a remote
 *     L2CAP entity with L2CAP_GetInfo.
 */
typedef U16 L2capInfoType;

#define L2INFO_CONNECTIONLESS_MTU   0x0001

#define L2INFO_EXTENDED_FEATURES    0x0002

#define L2INFO_FIXED_CHANNELS       0x0003

/* End of L2capInfoType */


/*---------------------------------------------------------------------------
 * L2capConfigFlags type
 *
 */
typedef U16 L2capConfigFlags;

/* No flags. */
#define L2CFG_FLAG_NONE     0x0000

/* More/Continuation bit in configuration packets. */
#define L2CFG_FLAG_MORE     0x0001

/* End of L2capConfigFlags */

/*---------------------------------------------------------------------------
 * L2capConfigResult type
 *
 *     Describes the result of configuration negotiation. This type is
 *     indicated when sending a L2CAP Configure Response.
 */
typedef U16 L2capConfigResult;

/* Requested parameters are acceptable */
#define L2CFG_RESULT_ACCEPTED           0x0000

/* Requested parameters are unacceptable */
#define L2CFG_RESULT_UNACCEPTABLE       0x0001

/* Request rejected - unspecified reason */
#define L2CFG_RESULT_REJECTED           0x0002

/* Unknown option(s) were present */
#define L2CFG_RESULT_UNKN_OPTIONS       0x0003

/* Request pending */
#define L2CFG_RESULT_PENDING            0x0004

/* Request rejected - Flow Spec unacceptable. */
#define L2CFG_RESULT_FLOW_SPEC_REJECTED 0x0005

/* End of L2capConfigResult */


/*---------------------------------------------------------------------------
 * L2capConfigOption type
 *
 *     Describes the type of a configuration parameter option.
 */
typedef U8 L2capConfigOption;

/* Option type for the remote device's maximum payload size (maximum
 * transmission unit). The length field will indicate 2 bytes, and be
 * followed by the 16-bit MTU size.
 */
#define L2CFG_OPTION_MTU            0x01

/* Option type for the remote device's flush timeout option. This is the
 * amount of time the remote device will attempt to retransmit before
 * giving up and flushing the data. The length field will indicate 2 bytes,
 * and be followed by the flush timeout value.
 */
#define L2CFG_OPTION_FLUSH_TIMEOUT  0x02

/* Option type for the remote device's quality of service parameters.
 * This option corresponds with the guarantees that the remote device
 * will provide. See the Bluetooth L2CAP specification for more information.
 */
#define L2CFG_OPTION_QOS            0x03

/* Option type for the remote device's Enhanced L2CAP modes.
 * This option specifies the link modes supported by L2CAP such
 * as Enhanced Retransmission mode or Streaming mode plus related
 * configuration parameters.
 */
#define L2CFG_OPTION_FLOW_CONTROL   0x04

/* Option type for the remote device's Frame Check Sequence (FCS) type
 * when in Enhanced Retransmission or Streaming modes.
 */
#define L2CFG_OPTION_FCS            0x05

/* Option type Extended Flow Specification
 */
#define L2CFG_OPTION_EXT_FLOW_SPEC  0x06

/* Option type for Extended Window Size
 */
#define L2CFG_OPTION_EXT_WIN_SIZE   0x07

/* End of L2capConfigOption */

/*---------------------------------------------------------------------------
 * L2capQosServiceType type
 *
 *      Indicates the quality of service (QOS) guaranteed by a device.
 *      This type is used when reading a L2CFG_OPTION_QOS configuration
 *      option parameter (see L2capConfigOption).
 */
typedef U8 L2capQosServiceType;

/* No outgoing data will be sent so QOS parameters are unnecessary */
#define L2QOS_OPTION_NO_TRAFFIC       0x00

/* The device cannot guarantee the QOS parameters, but will make its
 * "best effort" to honor them.
 */
#define L2QOS_OPTION_BEST_EFFORT      0x01

/* The device guarantees that it will operate within the QOS parameters
 * provided.
 */
#define L2QOS_OPTION_GUARANTEED       0x02

/* End of L2capQosServiceType */


/*---------------------------------------------------------------------------
 * L2capLinkMode type
 *
 *     Indicates the L2CAP Link modes supported by the channel. This type
 *     is used when reading a L2CFG_OPTION_FLOW_CONTROL configuration option
 *     flags parameter.
 */
typedef U8 L2capLinkMode;

/* Bluetooth v1.1 mode, no Enhanced L2CAP modes supported. */
#define L2MODE_BASIC                0x01

/* This mode is not supported. It has been replaced by L2MODE_ERTM. */
#define L2MODE_RETRANSMISSION       0x02

/* This mode is not supported. It has been replaced by L2MODE_ERTM. */
#define L2MODE_FLOW_CONTROL         0x04

/* Enhanced Retransmission of L2CAP frames is enabled. In
 * enhanced retransmission mode, flow control is utilized on the
 * connection and frames may be selectively retransmitted as
 * part of an error recovery procedure.
 */
#define L2MODE_ERTM                 0x08

/* Streaming of L2CAP frames is enabled. In streaming mode,
 * there is no retransmission and a finite flush timeout is
 * utilized to keep the transmission stream current.
 */
#define L2MODE_STREAMING            0x10

/* End of L2capLinkMode */


/*---------------------------------------------------------------------------
 * L2capChannelId type
 *
 *     Identifies a particular channel. Channel ID values are assigned
 *     by L2CAP to uniquely identify a connection to a remote device,
 *     using a specific higher-layer protocol.
 */
typedef U16 L2capChannelId;

/* Specifies an invalid L2CAP channel ID. */
#define L2CID_INVALID       0x0000

/* The L2CAP Fixed Signaling channel. */
#define L2CID_SIGNAL        0x0001

/* The L2CAP Fixed Connectionless channel. */
#define L2CID_CONNLESS      0x0002

/* The L2CAP Fixed A2MP channel. */
#define L2CID_A2MP          0x0003

/* The LE ATT Fixed channel. */
#define L2CID_LE_ATT        0x0004

/* The LE Signaling Fixed channel. */
#define L2CID_LE_SIGNAL     0x0005

/* The LE Security Manager Fixed channel. */
#define L2CID_LE_SEC        0x0006

/* The BR/EDR Security Manager Fixed channel. */
#define L2CID_BR_SEC        0x0007

/* The last valid L2CAP fixed channel. */
#define L2CID_FIXED_LAST    0x003F

/* End of L2capChannelId */


/*---------------------------------------------------------------------------
 * L2capControllerId type
 *
 *     Identifies the physical link for the channel.
 */
typedef U8 L2capControllerId;

/* End of L2capControllerId */


/*---------------------------------------------------------------------------
 * L2ExtendedFeatures type
 *
 *     Defines the L2CAP Extended Feature Mask values found in an L2CAP
 *     Information Response. The response value is sent in Little Endian and
 *     may need conversion before being compared with the mask values. If a
 *     bit is set in the response, the corresponding feature is present.
 */
typedef U32 L2ExtendedFeatures;

/* L2CAP Flow Control is supported */
#define L2EXTENDED_FEATURE_FLOW_CONTROL         0x00000001

/* L2CAP Retransmission is supported */
#define L2EXTENDED_FEATURE_RETRANSMISSIONS      0x00000002

/* L2CAP bi-directional QoS is supported */
#define L2EXTENDED_FEATURE_BIDIRECTIONAL_QOS    0x00000004

/* L2CAP Enhanced Retransmission is supported */
#define L2EXTENDED_FEATURE_ERTM                 0x00000008

/* L2CAP Streaming is supported */
#define L2EXTENDED_FEATURE_STREAMING            0x00000010

/* L2CAP FCS Options are supported */
#define L2EXTENDED_FEATURE_FCS                  0x00000020

/* L2CAP Extended Flow Specification for BR/EDR is supported */
#define L2EXTENDED_FEATURE_EXT_FLOW_SPEC        0x00000040

/* L2CAP Fixed Channels is supported */
#define L2EXTENDED_FEATURE_FIXED_CHANNELS       0x00000080

/* L2CAP Extended Window Size is supported */
#define L2EXTENDED_FEATURE_EXT_WINDOW_SIZE      0x00000100

/* L2CAP Unicast Connectionless Data Reception is supported */
#define L2EXTENDED_FEATURE_UNICAST_DATA         0x00000200

/* End of L2ExtendedFeatures */


/* Check the L2CAP_EXTENDED_FEATURES mask now that L2ExtendedFeatures
 * is defined.
 */

#if (NUM_AMPS > 0) && (0 == (L2CAP_EXTENDED_FEATURES & L2EXTENDED_FEATURE_ERTM))
#error NUM_AMPS > 0 requires L2EXTENDED_FEATURE_ERTM
#endif

#if (0 == (L2CAP_EXTENDED_FEATURES & L2EXTENDED_FEATURE_FIXED_CHANNELS))
#error L2CAP requires L2EXTENDED_FEATURE_FIXED_CHANNELS
#endif

#if (L2CAP_EXT_WINDOW_SIZE == XA_ENABLED) && (0 == (L2CAP_EXTENDED_FEATURES & L2EXTENDED_FEATURE_EXT_WINDOW_SIZE))
#error L2CAP_EXT_WINDOW_SIZE requires L2EXTENDED_FEATURE_EXT_WINDOW_SIZE
#endif

#if (L2CAP_EXT_WINDOW_SIZE == XA_DISABLED) && (L2CAP_EXTENDED_FEATURES & L2EXTENDED_FEATURE_EXT_WINDOW_SIZE)
#error L2EXTENDED_FEATURE_EXT_WINDOW_SIZE requires L2CAP_EXT_WINDOW_SIZE to be XA_ENABLED
#endif

#if (L2CAP_NUM_ENHANCED_CHANNELS == 0) && (L2CAP_EXTENDED_FEATURES & (L2EXTENDED_FEATURE_FCS |           \
                                                                      L2EXTENDED_FEATURE_ERTM |          \
                                                                      L2EXTENDED_FEATURE_STREAMING |     \
                                                                      L2EXTENDED_FEATURE_EXT_FLOW_SPEC | \
                                                                      L2EXTENDED_FEATURE_EXT_WINDOW_SIZE))
#error FCS, ERTM, STREAMING, EXT_FLOW_SPEC and EXT_WINDOW_SIZE require L2CAP_NUM_ENHANCED_CHANNELS > 0
#endif


/*---------------------------------------------------------------------------
 * L2capFcsOption type
 *
 */
typedef U16 L2capFcsOption;

/* No FCS */
#define L2FCS_NONE          0x01

/* 16-bit FCS (default) */
#define L2FCS_16BIT         0x02

/* Require 16-bit FCS */
#define L2FCS_16BIT_ONLY    0x04

/* End of L2capFcsOption */

/*---------------------------------------------------------------------------
 * L2capRxFrag type
 *
 *     Defines the setting for enabling/disabling fragmented data indications 
 *     on a BASIC mode PSM or channel. 
 */
typedef U8 L2capRxFrag;

/* Fragmentation for BASIC mode is disabled */
#define L2CAP_FRAG_DISABLED 0

/* Fragmentation for BASIC mode is enabled */
#define L2CAP_FRAG_ENABLED 1

/* End of L2capRxFrag */

/* Forward reference for the L2capCallback type */
typedef struct _L2capCallbackParms L2capCallbackParms;

/*---------------------------------------------------------------------------
 * L2capCallback type
 *
 *     A function of this type is called to indicate L2CAP events to
 *     a protocol service. When L2CAP calls this function, it provides
 *     the channel ID that received the event and additional information
 *     in the "info" parameter.
 *
 *     Protocol services provide the callback function's address to
 *     L2CAP by calling the L2CAP_RegisterPsm function.
 */
typedef void (*L2capCallback)(L2capChannelId cid, L2capCallbackParms *Info);

/* End of L2capCallback */


/*---------------------------------------------------------------------------
 * L2CAP Security Id
 *
 * Security ID for L2CAP access.
 */
#define SEC_L2CAP_ID    (BtProtocolId)L2Cap_SecAccessRsp

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * L2capFlowSpec structure
 *
 *     Specifies the L2CAP Flow Specification parameters for the Channel.
 */
typedef struct _L2capFlowSpec
{
    /* A boolean indicating the direction of this flow specification.
     * 0x00 indicates outgoing, 0x01 indicates incoming, 0x02 indicates both.
     */
    U8      flowDirection;

    /* The average data rate required by the channel. Specified in octets
     * per second. (Equivalent to the L2CAP QoS Token Rate.)
     */
    U32     avgDataRate;

    /* The peak data rate required by the channel. Specified in octets per
     * second. (Equivalent to the L2CAP QoS Peak Bandwidth.)
     */
    U32     peakDataRate;

    /* The maximum size SDU sent by the channel that must be sent
     * contiguously. Specified in octets.
     * (Equivalent to the L2CAP QoS Token Bucket Size.)
     */
    U32     maxSduSize;

    /* The maximum acceptable delay between the instant a data fragment is
     * given to the Host Controller and the instant the data fragment is
     * transmitted over the air. Specified in microseconds.
     * (Equivalent to the L2CAP QoS Access Latency.)
     */
    U32     accessLatency;

} L2capFlowSpec;


/*---------------------------------------------------------------------------
 * L2capCallbackParms structure
 *
 *     Provides information to the L2CAP user about an L2capCallback
 *     event. Different fields are valid depending on the type of
 *     the event, indicated by the "event" field. See L2capEventType for
 *     more information about specific callback events.
 *
 *     The L2CAP user should not modify elements in this structure.
 *     Unless otherwise noted, the callback structure and any data pointers
 *     it provides should be considered invalid after the callback returns.
 */
struct _L2capCallbackParms
{
    /* L2CAP upper layer event. Always valid. */
    L2capEventType      event;

    /* Completion status, based on the event type. Possible types are
     * L2capDiscReason for L2EVENT_DISCONNECTED events or BtStatus for
     * most others. */
    U16                 status;

    /* Device that originated the event or data. Always valid. */
    BtRemoteDevice     *aclLink;

    /* Logical Link that originated the event or data. Always valid. */
    BtLogicalLink      *lLink;

    union {
        /* Pointer to the PSM structure passed in to L2CAP_RegisterPsm */
        const struct _L2capPsm      *psm;

        struct {
            /* L2capPsmValue registered with L2CAP_RegisterConnectionless */
            L2capPsmValue                psm; /* PSM value registered via L2CAP_RegisterPsm */

            /* Psm value received in connectionless packet. This will be the same as psm
             * expect in the case where regPsm is 0
             */
            L2capPsmValue                rxPsm; 
        } cl;
                                              
#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
        /* Pointer to the PSM structure passed in to LEL2CAP_RegisterPsm */
        const struct _LeL2capPsm    *lePsm;
#endif /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */
    } owner;

    /* Length of data in "ptrs.data", during L2EVENT_DATA_IND,
     * L2EVENT_FRAGMENT_IND and L2EVENT_LAST_FRAG_IND events. Maximum
     * Physical Link PDU size during L2EVENT_MOVE_CHAN_CNF and
     * L2EVENT_ADJUST_SDU_SIZE events.
     */
    U16                 dataLen;

    /* Total length of fragments in fragmented data indications.
     * Valid only during L2EVENT_FRAGMENT_IND and
     * L2EVENT_LAST_FRAG_IND events.
     */
    U16                 sduLen;

#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
    /* Number of credits remaining after data has been received during
     * L2EVENT_FRAGMENT_IND, L2EVENT_LAST_FRAG_IND, and L2EVENT_DATA_IND 
     * events for an LEL2CAP dynamic channel. If the value is 0 then no 
     * more data can be sent by the remote device for this channel until 
     * more credit is advanced using LEL2CAP_AdvanceCredits().
     */
    U16                remainingCredits;

    /* Initial transmit credits issued by the remote device for the
     * LEL2CAP dynamic channel connection. Valid only during the
     * L2EVENT_CONNECTED event.
     */
    U16                initialTxCredits;

    /* Contains the maximum SDU size the remote device can 
     * receive. Valid only during the L2EVENT_CONNECTED event.
     */
    U16                maxRemoteSdu;

#endif /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */
    union {
        /* Packet data, valid only during L2EVENT_PACKET_HANDLED callback
         * events. */
        BtPacket       *packet;

        /* During a L2EVENT_DATA_IND, L2EVENT_FRAGMENT_IND and L2EVENT_LAST_FRAG_IND
         * callback event this points to the data received by the channel. During a 
         * L2EVENT_COMPLETE callback event for an L2CAP_GetInfo() call, this points 
         * to the requested information. During L2EVENT_COMPLETE events for an 
         * L2CAP_Ping() call, this points to any data provided in the echo response.
         */
        U8             *data;

        /* The Flush Timeout value sent to HCI indicated in an
         * L2EVENT_FLUSH_TIMEOUT event.
         */
        U16             flushTimeout;

        /* The Flow Specification sent to HCI indicated in an
         * L2EVENT_FLOW_SPEC event.
         */
        L2capFlowSpec  *flowSpec;

        /* ACL disconnect reason or Auth failure reason. For more information,
         * see L2EVENT_DISCONNECTED event.
         */
        BtErrorCode     errCode;

#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
        /* During an L2EVENTEX_CREDIT_IND event for an LEL2CAP dynamic
         * channel connection this contains the number of received 
         * credits pertaining to additional transmit PDU's.
         */
        U16             rcvdTxCredits;
#endif /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */
    } ptrs;
};

#if L2CAP_NUM_ENHANCED_CHANNELS > 0
/*---------------------------------------------------------------------------
 * L2capFlowParms structure
 *
 *     Specifies the Enhanced L2CAP mode parameters for the PSM or Channel.
 */
typedef struct _L2capFlowParms
{
    /* Requested operating modes for channels opened for this PSM. For
     * maximum compatiblity, this should include L2MODE_BASIC. If basic is
     * not included, connections to Bluetooth v1.1 devices will fail. An
     * L2CAP Information request may be issued to retrieve the extended
     * features mask to ascertain the remote devices capabilities. If the
     * PSM does not permit Enhanced L2CAP modes added in the Bluetooth v2.1
     * addendum, set this parameter to L2MODE_BASIC.
     *
     * The inbound and outbound link modes must be symmetric. For example:
     * if inbound packets can use Enhanced Retranmission mode, outbound data
     * must use Enhanced Retransmission mode also. If the link is configured
     * to use basic mode in one direction, it must use basic mode in the
     * other direction as well.
     */

    /* Specify the acceptable modes for the link. Both incoming and outgoing
     * links have the same mode since supported link modes are symmetric.
     */
    L2capLinkMode   linkMode;

    /* Specify the preferred link mode for the link. L2CAP will begin
     * negotiating the channel configuration with this mode if more than
     * one mode is specified in "linkMode".
     */
    L2capLinkMode   prefLinkMode;

    /* The size of the send window for retransmission modes. */
    U16             txWindow;

    /* The size of the receive window for retransmission modes. */
    U16             rxWindow;

    /* The maximum number of times the remote L2CAP can transmit a packet
     * before considering the channel lost. The minimum value of 1 indicates
     * no retransmissions shall be sent. Note that if the retransmit timer
     * expires and the maxTransmit value has been reached, the channel will
     * be disconnected. This value is not used by basic mode connections.
     */
    U8              maxTransmit;

    /* The retransmission timeout to be used by the remote device when
     * retransmission mode is enabled. This timeout should be set fairly
     * high in order to avoid unnecessary retransmissions due to delayed
     * acknowledgements. Suitable values, specified in milliseconds, could
     * be 100s of milliseconds or higher. This value is not used by basic
     * mode connections.
     */
    U16             retransTimeout;

    /* The monitor timeout to be used by the remote device when no activity
     * is present on the channel. It serves to ensure that lost acknowledgements
     * are retransmitted. Suitable values, specified in milliseconds, could
     * be 100s of milliseconds or higher. This value is not used by basic
     * mode connections.
     */
    U16             monitorTimeout;

    /* The FCS Option allows configuration of a channel with a preference
     * for no Frame Check Sequence (FCS) in the L2CAP packet. If this option
     * is L2FCS_NONE, the configuration will specify no FCS. Note that if either
     * side of the link requests FCS in a configuration request, then
     * FCS will be used for the link.
     */
    L2capFcsOption  fcsOption;

    /* The txMps defines the maximum PDU size the local channel can send.
     * Each channel specifies this value separately. Note that L2CAP will
     * adjust this to be less than or equal to the remote device's rxMps.
     */
    U16             txMps;

    /* The rxMps defines the maximum PDU size the local channel can accept.
     * Each channel specifies this value separately.
     */
    U16             rxMps;

    /* Enables or disables fragmented data indications on an Enhanced PSM or
     * channel. Normally L2CAP collects received data fragments in a buffer
     * until an entire SDU is received. With this option enabled, L2CAP will
     * not buffer data and will indicate fragments to the upper layer just as
     * received from the lower controller.
     */
    BOOL            fragmentsEnabled;

    /* The flowSpec defines the outgoing data flow requirements for this
     * connection.
     */
    ExtFlowSpec     efs;

} L2capFlowParms;
#endif /* #if L2CAP_NUM_ENHANCED_CHANNELS > 0 */


/*---------------------------------------------------------------------------
 * L2capPsm structure
 *
 *     Represents a protocol service that uses L2CAP. Any higher-layer
 *     protocol or application that will use L2CAP's connection-oriented
 *     services must allocate and completely fill out this structure.
 *     The structure is then provided to L2CAP by calling L2CAP_RegisterPsm.
 *     It is highly recommended that the structure be filled with 0 based
 *     on the size of structure so in the future when the structure is
 *     expanded old code will initialize the new value correctly.
 *
 *     Example: OS_MemSet((U8 *) l2capPsm, 0, sizeof(L2capPsm));
 *
 *     For an L2CAP user who will only call L2CAP_Ping and L2CAP_GetInfo,
 *     only the "callback" field is required to be filled out. Connection
 *     oriented services may use L2CAP_Ping and L2CAP_GetInfo as well.
 */
typedef struct _L2capPsm
{
    /* Callback to invoke whenever an event occurs. */
    L2capCallback   callback;

    /* Identifier for the protocol service. */
    L2capPsmValue   psm;

    /* Group: The following parameters specify the connection settings used
     * to configure channels opened on this PSM. L2CAP will automatically
     * negotiate the connection configuration with the remote peer.
     */

    /* Maximum receive MTU for this service. Should be equal to or less
     * than L2CAP_MAXIMUM_MTU. This is the MTU; which will be equivalent
     * to the higher layer SDU size only in Basic L2CAP mode.
     */
    U16             localMtu;

    /* Minimum acceptable value for remote device's receive MTU size. Aka,
     * local transmit MTU. Should be no less than L2CAP_MINIMUM_MTU.
     */
    U16             minRemoteMtu;

    /* If this PSM supports a Fixed Channel, this is the L2capChannelId of the
     * Fixed Channel. This must be zero otherwise.
     */
    U16             fixedChannelCid;

#if L2CAP_NUM_ENHANCED_CHANNELS > 0
    /* Default Enhanced L2CAP mode parameters for PSM. These can be
     * overridden when a channel is established (see L2capConnSettings.)
     */
    L2capFlowParms  flowParms;

#endif /* #if L2CAP_NUM_ENHANCED_CHANNELS > 0 */

    /* Handler installed by the application to receive L2CAP events. For
     * applications that understand L2capAgents this handler is called
     * instead of the callback and must be initialized. If handler is set
     * to 0, L2CAP will consider the caller to be a legacy application and
     * will assign it a "legacy" handler.
     */
    BtHandlerEx    *handler;

    /* L2cap calls this function to allocate an L2capAgent object for
     * incoming L2CAP channels. Applications that understand L2capAgents
     * may set this to 0 to indicate they want the system default factory.
     * The returned L2capAgent object type is defined in il2capagent.h.
     */
    BtFactory       agentFactory;

    /* Enables or disables fragmented data indications (L2EVENT_FRAGMENT_IND 
     * and L2EVENT_LAST_FRAG_IND) on a BASIC Mode PSM or channel. Normally 
     * L2CAP collects received data fragments in a buffer until an entire 
     * SDU is received. With this option enabled, L2CAP will not buffer data 
     * and will indicate fragments to the upper layer just as received from 
     * the controller.
     */
    L2capRxFrag    rxFragments;

#if NUM_AMPS > 0 
   /* The service ID is used by modules such as the L2capAgent Preference
     * Manager for determining the preferences to use when an L2CAP
     * channel is created using an L2capAgent.
     */
    BtServiceId     serviceId;
#endif

} L2capPsm;


/*---------------------------------------------------------------------------
 * L2capConnSettings structure
 *
 *     Provides connection settings to L2CAP during connection establishment.
 */
typedef struct _L2capConnSettings {

   /* The channel-specific flush timeout sent to the remote device in a
    * configuration response. A non-zero value specifies the flush timeout to
    * be sent. Otherwise, the default infinite flush timeout is assumed and
    * no flush timeout will be sent in the response. A non-zero flush timeout
    * should be calculated from the processing time and buffer capacity of
    * the local receiver. It is sent in a configuration response so the
    * remote device will know what flush timeout to use based on local
    * receiving capacity.
    */
    U16             flushTimeout;

    /* The default channel-specific priority.
     */
    BtPriority      priority;

#if L2CAP_NUM_ENHANCED_CHANNELS > 0 
    /* Optional Enhanced L2CAP mode parameters for Channel. The channel will
     * use the parameters set in the PSM if not specified here.
     */
    L2capFlowParms *flowParms;
#endif

    /* Optional pointer to initialized L2capAgent structure to be used with
     * L2CAP_ConnectReq() function call.
     */
    void           *agent;

#if NUM_AMPS > 0 
    /* Optional service ID to be applied to the agent. If supplied, this
     * service ID will overwrite any prior service ID associated with the
     * PSM. If 0, no action is taken.
     */
    BtServiceId     serviceId;
#endif

} L2capConnSettings;


/*---------------------------------------------------------------------------
 * L2capConnectionlessHandler structure
 *
 *     Handles both client and server connectionless data events.
 */
typedef struct _L2capConnectionlessHandler {
    BtHandlerEx     handler;
    L2capPsmValue   psm;
    BtPhysicalLink *pLink;
} L2capConnectionlessHandler;


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * L2CAP_RegisterPsm()
 *
 *     Registers a PSM identifier with L2CAP. Once registered, the protocol
 *     can initiate connections and data transfer as well as receive
 *     connection requests and data.
 *
 * Parameters:
 *     protocol - Pointer to the L2capPsm structure to be registered.
 *         This structure is owned by L2CAP until it is successfully
 *         deregistered. All fields in the structure must be initialized
 *         before this function is called. It is highly recommended that
 *         structure be filled with 0's before setting specific values
 *         so existing code will properly initialize fields that are
 *         added later.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - PSM was successfully registered with L2CAP.
 *
 *     BT_STATUS_FAILED - Invalid parameter value (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_RESOURCES - Maximum number of registered PSMs reached.
 *
 *     BT_STATUS_IN_USE - The protocol is already registered (XA_ERROR_CHECK only).
 */
BtStatus L2CAP_RegisterPsm(L2capPsm *protocol);


#if L2CAP_DEREGISTER_FUNC == XA_ENABLED
/*---------------------------------------------------------------------------
 * L2CAP_DeregisterPsm()
 *
 *     Deregisters a registered protocol from L2CAP.
 *
 * Requires:
 *     L2CAP_DEREGISTER_FUNC enabled.
 *
 * Parameters:
 *     protocol - Pointer to L2capPsm structure previously registered with
 *         the L2CAP_RegisterPsm function call.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The protocol has been deregistered.
 *
 *     BT_STATUS_FAILED - The protocol parameter was invalid (XA_ERROR_CHECK
 *         only).
 *
 *     BT_STATUS_NOT_FOUND - The protocol was not previously registered.
 *
 *     BT_STATUS_BUSY - The protocol has one or more active channels and
 *         cannot be deregistered.
 */
BtStatus L2CAP_DeregisterPsm(const L2capPsm *protocol);

#endif /* L2CAP_DEREGISTER_FUNC == XA_ENABLED */

/*---------------------------------------------------------------------------
 * L2CAP_ConnectReq()
 *
 *     Requests the establishment of an L2CAP channel to the specified
 *     remote protocol service and device. If this function returns
 *     BT_STATUS_PENDING, the request was successfully initiated.
 *     Connection status events will be sent to the initiating
 *     L2capPsm's callback function.
 *
 * Parameters:
 *     protocol - Pointer to a registered L2capPsm structure.
 *
 *     psm - PSM value of protocol to connect to on the remote device.
 *
 *     remDevice - The remote device to connect with.
 *
 *     settings - Pointer to the channel-specific connection settings. This
 *         parameter may be zero if no settings are relevant. If provided,
 *         all settings must be initialized.
 *
 *     localCid - Pointer to a channel ID. If this function returns
 *         BT_STATUS_PENDING, this parameter is filled with a channel
 *         ID assigned to this connection.
 *
 * Returns:
 *     BT_STATUS_PENDING - The connection process has been successfully
 *         initialized. When the connection process is complete, the
 *         protocol service's callback will receive either the
 *         L2EVENT_CONNECTED or L2EVENT_DISCONNECTED L2capEventType.
 *
 *     BT_STATUS_INVALID_PARM - Protocol specified is not registered
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - Protocol linkModes mismatch.
 *
 *     BT_STATUS_NO_RESOURCES - L2CAP could not allocate a channel
 *         structure or an Enhanced channel structure to initiate the connection.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link to the specified device
 *         is not active.
 *
 *     BT_STATUS_RESTRICTED - The PSM is not authorized to establish
 *         outbound connections.
 */
BtStatus L2CAP_ConnectReq(L2capPsm *protocol, L2capPsmValue psm,
                          BtRemoteDevice *RemDev, const L2capConnSettings *settings,
                          L2capChannelId *localCid);


/*---------------------------------------------------------------------------
 * L2CAP_ConnectRsp()
 *
 *     Indicates the response to a connection request (L2EVENT_CONNECT_IND
 *     L2capEventType) from a remote device.
 *
 * Parameters:
 *     cid - Channel ID assigned by L2CAP to this connection. This
 *         ID must match the one provided by the L2EVENT_CONNECT_IND event
 *         callback.
 *
 *     status - Response code used in the response to the remote device.
 *         This value may indicate success, pending, or rejection.
 *
 *     settings - Pointer to the channel-specific connection settings. This
 *         parameter may be zero if no settings are relevant.  If provided,
 *         all settings must be initialized.
 *
 * Returns:
 *     BT_STATUS_PENDING - The connection response packet has been initiated
 *         successfully. One of the following events will arrive to
 *         indicate completion:
 *
 *         L2EVENT_CONNECTED, if the connection was established successfully.
 *
 *         L2EVENT_DISCONNECTED, if the connection failed. Check the callback
 *         info's "status" parameter for the exact reason.
 *
 *         L2EVENT_COMPLETE, when the remote device has been informed of the
 *         L2CONN_PENDING connection status. L2CAP_ConnectRsp must
 *         be called a second time.
 *
 *     BT_STATUS_FAILED - Invalid channel ID (XA_ERROR_CHECK only), or
 *         L2CAP_ConnectRsp was called at the wrong time.
 *
 *     BT_STATUS_IN_PROGRESS - A response is already in progress. Wait for
 *         L2EVENT_COMPLETE before calling L2CAP_ConnectRsp.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this
 *         channel is no longer active.
 *
 *     BT_STATUS_NO_RESOURCES - L2CAP could not allocate an Enhanced channel 
 *         structure to initiate the connection. The caller must call
 *         L2CAP_ConnectRsp() to reject the request or change the "settings"
 *         to allow BASIC mode and accept the request.
 */
BtStatus L2CAP_ConnectRsp(L2capChannelId cid, L2capConnStatus status,
                          const L2capConnSettings *settings);


/*---------------------------------------------------------------------------
 * L2CAP_DisconnectReq()
 *
 *     Requests the disconnection of the specified L2CAP channel.
 *
 * Parameters:
 *     cid - The channel to disconnect.
 *
 * Returns:
 *     BT_STATUS_PENDING - The disconnect request has been initiated
 *         successfully. When the disconnect is complete, the
 *         L2EVENT_DISCONNECTED event is provided to the L2CAP user.
 *
 *     BT_STATUS_IN_PROGRESS - The channel is already disconnecting
 *         (XA_ERROR_CHECK Only).
 *
 *     BT_STATUS_FAILED - The channel is invalid or could not be
 *         disconnected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this
 *         channel is no longer active.
 */
BtStatus L2CAP_DisconnectReq(L2capChannelId cid);


/*---------------------------------------------------------------------------
 * L2CAP_SendData()
 *
 *      Sends data on the specified channel.
 *
 * Parameters:
 *      cid - Channel to send the data over. The channel must be connected.
 *
 *      packet - Data packet describing data to transmit. The caller
 *          must initialize all necessary BtPacket fields, including
 *          "data", "dataLen", "flags", "ulpContext", "tail", "tailLen",
 *          "header" and "headerLen" as described below.
 *
 *          The "data" pointer and "dataLen" length indicate the payload
 *          data to be transmitted by L2CAP.
 *
 *          The "header" array is filled with higher-layer protocol
 *          header data. This array is filled back-to-front so that
 *          protocols may fill in data as the packet is handed down
 *          the protocol stack. "headerLen" indicates the number of
 *          valid header bytes at the end of the "header" array.
 *
 *          The "ulpContext" field may be filled with a protocol-specific
 *          value. L2CAP and lower layers ignore this field.
 *
 *          The "flags" field may be set as described by the BtPacketFlags
 *          type.
 *
 *          The "tail" pointer may point to a data buffer that is appended
 *          to the end of the packet (after "data"). The field "tailLen"
 *          is set to the number of bytes pointed to by "tail". If this field
 *          is used, the packet flag BTP_FLAG_TAIL must be set.
 *
 *          The packet's total size ("headerLen" plus "dataLen" plus
 *          "tailLen", if present) must be less than or equal to
 *          the remote device's L2CAP MTU.
 *
 * Returns:
 *     BT_STATUS_PENDING - Packet was successfully queued for transmission on
 *         the channel. The completion of the send request is indicated by an
 *         L2EVENT_PACKET_HANDLED event.
 *
 *     BT_STATUS_FAILED - The channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this
 *         channel is no longer active.
 */
BtStatus L2CAP_SendData(L2capChannelId cid, BtPacket *packet);


/*---------------------------------------------------------------------------
 * L2CAP_SetFlushTimeout()
 *
 *      Sets or resets the flush timeout on the specified channel.
 *
 * Parameters:
 *      ChannelId - Channel for which to set the flush timeout. The channel
 *          must be connected.
 *
 *      FlushTimeout - Value of new flush timeout.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Flush timeout command was sent.
 *
 *     BT_STATUS_PENDING - Flush timeout command was queued.
 *
 *     BT_STATUS_FAILED - The channel is invalid or is not connected.
 */
BtStatus L2CAP_SetFlushTimeout(L2capChannelId ChannelId, U16 FlushTimeout);


/*---------------------------------------------------------------------------
 * L2CAP_GetConnectionMode()
 *
 *     Retrieves the link mode of the connected channel.
 *
 * Parameters:
 *     Cid - Channel ID to query.
 *
 * Returns:
 *     Link Mode - The L2capLinkMode selected during channel configuration.
 *
 *     0 - If the channel does not exist or is not connected (XA_ERROR_CHECK only).
 */
L2capLinkMode L2CAP_GetConnectionMode(L2capChannelId Cid);

/*---------------------------------------------------------------------------
 * L2CAP_GetTxMtu()
 *
 *     Retrieves the transmit MTU of the connected channel. Do not use this
 *     API for fixed channels.
 *
 * Parameters:
 *     cid - Channel ID to query.
 *
 * Returns:
 *     Size - The Maximum Transmission Unit (MTU) size in bytes.
 *
 *     0 - If the channel does not exist or is not connected (XA_ERROR_CHECK only).
 */
U16 L2CAP_GetTxMtu(L2capChannelId cid);

/*---------------------------------------------------------------------------
 * L2CAP_RequestFlowSpec()
 *
 *      Sets or resets the flow specification on the specified channel.
 *      The "FlowSpec" structure is owned by L2CAP until the application
 *      receives an L2EVENT_FLOW_SPEC event indicating the command has been
 *      sent and the structure has been released. The application will get
 *      a BTEVENT_QOS_COMPLETE event when the radio confirms or denies the
 *      flow specification.
 *
 * Parameters:
 *      ChannelId - Channel for which to set the flow specification. The
 *          channel must be connected.
 *
 *      FlowSpec - pointer to an L2capFlowSpec structure initialized with
 *          valid flow specification parameters.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Flow Specification command was queued.
 *
 *     BT_STATUS_FAILED - The channel is invalid or is not connected.
 */
BtStatus L2CAP_RequestFlowSpec(L2capChannelId ChannelId,
                               L2capFlowSpec *FlowSpec);

#if L2CAP_NUM_ENHANCED_CHANNELS > 0 
/*---------------------------------------------------------------------------
 * L2CAP_SetLocalBusy()
 *
 *     Sets the busy mode of the connected ERTM channel.
 *
 * Requires:
 *     L2CAP_NUM_ENHANCED_CHANNELS > 0.
 *
 * Parameters:
 *     Cid - Channel ID to set.
 *
 *     Set - When true, set busy. When false, clear busy.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Retransmissions have been disabled.
 *
 *     BT_STATUS_FAILED - The channel is not configured properly
 *                        or is in wrong state for this operation.
 */
BtStatus L2CAP_SetLocalBusy(L2capChannelId Cid, BOOL Set);

/*---------------------------------------------------------------------------
 * L2CAP_GetTxMps()
 *
 *     Retrieves the transmit MPS of the connected channel. 
 *
 * Requires:
 *     L2CAP_NUM_ENHANCED_CHANNELS > 0.
 *
 * Parameters:
 *     cid - Channel ID to query.
 *
 * Returns:
 *     Size - The Maximum PDU Size (MPS) in bytes.
 *
 *     0 - If the channel does not exist or is not connected (XA_ERROR_CHECK only).
 */
U16 L2CAP_GetTxMps(L2capChannelId cid);


/*---------------------------------------------------------------------------
 * L2CAP_SetTxMps()
 *
 *     Set the TX MPS on the specified dynamic channel.
 *
 * Requires:
 *     L2CAP_NUM_ENHANCED_CHANNELS > 0.
 *
 * Parameters:
 *     cid - Channel to send the data over. The channel must be connected.
 *
 *     mps - MPS size to set on the channel.
 *
 * Returns:
 *     BT_STATUS_PENDING - The TX MPS will be adjusted on the channel. The
 *         completion of the send request is indicated by an
 *         L2EVENT_ADJUST_SDU_SIZE event.
 *
 *     BT_STATUS_FAILED - The channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this
 *         channel is no longer active.
 */
BtStatus L2CAP_SetTxMps(L2capChannelId ChannelId, U16 Mps);

#endif /* #if L2CAP_NUM_ENHANCED_CHANNELS > 0 */ 

/*---------------------------------------------------------------------------
 * L2CAP_AllowUnreliableData()
 *
 *     Allows flushed or unreliable data on Basic Mode channels. Normally,
 *     Basic Mode channels cannot continue if data is flushed and L2CAP will
 *     close the ACL when this condition is detected. This API tells L2CAP
 *     the upper layer profile can handle missing data in the data stream
 *     and L2CAP will not close the ACL when this condition is detected.
 *
 * Parameters:
 *     Cid - Channel ID to set.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Unreliable data is allowed.
 *
 *     BT_STATUS_FAILED - The channel is not configured properly
 *                        or is in wrong state for this operation.
 */
BtStatus L2CAP_AllowUnreliableData(L2capChannelId ChannelId);

#if L2CAP_PING_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * L2CAP_Ping()
 *
 *     Sends an Echo Request to the L2CAP protocol on the specified remote
 *     device. The data length should not exceed the default L2CAP
 *     signaling MTU (44 bytes).
 *
 * Requires:
 *     L2CAP_PING_SUPPORT enabled.
 *
 * Parameters:
 *     pingClient - Client registration structure used by L2CAP to track
 *         the Ping request. The caller must set the callback field.
 *
 *     remAddr - Bluetooth address of device to ping.
 *
 *     data - Data to send to remote device.
 *
 *     dataLen - Length of "data" buffer in bytes. Must be less than 44.
 *
 * Returns:
 *     BT_STATUS_PENDING - Ping request was successfully queued for
 *         transmit. The result is indicated by an L2EVENT_COMPLETE event,
 *         sent to the "pingClient" callback function, when a response is
 *         received or the request times out. Check the L2capCallbackParms
 *         "status" parameter for the completion status. Any data echoed
 *         back can be found in the callback info's "data" and "dataLen"
 *         fields.
 *
 *     BT_STATUS_NO_RESOURCES - L2CAP could not allocate a signaling
 *         channel or packet.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link to the specified device
 *         is not active.
 */
BtStatus L2CAP_Ping(const L2capPsm *PingClient, BtRemoteDevice *RemDev,
                    const U8 *Data, U16 DataLen);

#endif /* L2CAP_PING_SUPPORT */


#if L2CAP_GET_INFO_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * L2CAP_GetInfo()
 *
 *     Sends an Info Request to the L2CAP protocol on the specified remote
 *     device. Info requests are used to exchange implementation-specific
 *     information regarding L2CAP's capabilities.
 *
 * Requires:
 *     L2CAP_GET_INFO_SUPPORT enabled.
 *
 * Parameters:
 *     infoClient - Client registration structure used by L2CAP to track
 *         the Info request. The caller must set the callback field.
 *
 *     remAddr - Bluetooth address of the device to query.
 *
 *     type - Type of information to request.
 *
 * Returns:
 *     BT_STATUS_PENDING - Sending of an Information Request has been
 *         successfully initiated. The result is indicated by an
 *         L2EVENT_COMPLETE event when a response is received or the request
 *         times out. The result is reported in the L2capCallbackParms
 *         "status" parameter. The result value & length is conveyed in the
 *         callback "data" and "dataLen" fields.
 *
 *     BT_STATUS_NO_RESOURCES - L2CAP could not allocate a signaling
 *         channel or packet.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link to the specified device
 *         is not active.
 */
BtStatus L2CAP_GetInfo(const L2capPsm *InfoClient,
                       BtRemoteDevice *RemDev, L2capInfoType Type);

#endif /* L2CAP_GET_INFO_SUPPORT */


/*---------------------------------------------------------------------------
 * L2CAP_DisableCLT()
 *
 *     Disables incoming connectionless traffic. By default, incoming
 *     connectionless traffic is enabled.
 *
 * Parameters:
 *     psm - Not used at this time.
 *
 * Returns:
 *    BT_STATUS_SUCCESS - Call was successful.
 */
BtStatus L2CAP_DisableCLT(L2capPsmValue Psm);


/*---------------------------------------------------------------------------
 * L2CAP_EnableCLT()
 *
 *     Enables incoming connectionless traffic. By default, incoming
 *     connectionless traffic is enabled.
 *
 * Parameters:
 *     psm - Not used at this time.
 *
 * Returns:
 *    BT_STATUS_SUCCESS - Call was successful.
 */
BtStatus L2CAP_EnableCLT(L2capPsmValue Psm);


/*---------------------------------------------------------------------------
 * L2CAP_RegisterConnectionless()
 *
 *     Registers a connectionless handler with L2CAP. Once registered,
 *     the handler will receive connectionless data indications that
 *     match the PSM specified. If the PSM value is null, all connectionless
 *     data will be indicated to the handler.
 *
 * Parameters:
 *     Handler - Handler initialized with callback that will receive
 *          connectionless data indications.
 *
 *     Psm - L2capPsmValue of desired connectionless data. Set this to
 *          null to receive all connectionless data.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Handler was successfully registered with L2CAP.
 *
 *     BT_STATUS_FAILED - Invalid parameter value (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_USE - The Handler is already registered (XA_ERROR_CHECK only).
 */
BtStatus L2CAP_RegisterConnectionless(L2capConnectionlessHandler *Handler,
                                      L2capPsmValue Psm);


/*---------------------------------------------------------------------------
 * L2CAP_DeregisterConnectionless()
 *
 *     Deregisters a connectionless handler from L2CAP.
 *
 * Parameters:
 *     Handler - Handler initialized with callback that will receive
 *          connectionless data indications.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Handler was successfully removed from L2CAP.
 *
 *     BT_STATUS_FAILED - Invalid parameter value (XA_ERROR_CHECK only).
 */
BtStatus L2CAP_DeregisterConnectionless(L2capConnectionlessHandler *ClHandler);


/*---------------------------------------------------------------------------
 * L2CAP_SendConnectionless()
 *
 *     Sends data on the connectionless channel.
 *
 * Parameters:
 *      Handler - Pointer to handler that will receive the
 *          L2EVENT_PACKET_HANDLED event. This handler must have been
 *          registered with L2CAP_RegisterConnectionless().
 *
 *     Psm - L2capPsmValue to which the data will be sent. This must
 *          be a valid PSM value.
 *
 *     Packet - Data packet describing data to transmit. The caller
 *          must initialize all necessary BtPacket fields, including
 *          "data", "dataLen", "flags", "ulpContext", "tail", "tailLen",
 *          "header" and "headerLen" as described below.
 *
 *          The "data" pointer and "dataLen" length indicate the payload
 *          data to be transmitted by L2CAP.
 *
 *          The "header" array is filled with higher-layer protocol
 *          header data. This array is filled back-to-front so that
 *          protocols may fill in data as the packet is handed down
 *          the protocol stack. "headerLen" indicates the number of
 *          valid header bytes at the end of the "header" array.
 *
 *          The "ulpContext" field may be filled with a protocol-specific
 *          value. L2CAP and lower layers ignore this field.
 *
 *          The "flags" field may be set as described by the BtPacketFlags
 *          type. If the "flags" field includes the BTP_FLAG_BROADCAST flag
 *          the packet will be broadcast, otherwise, the packet will be
 *          unicast to the "Link" device. Note the local device must be
 *          in the Master role to send broadcast packets.
 *
 *          The "tail" pointer may point to a data buffer that is appended
 *          to the end of the packet (after "data"). The field "tailLen"
 *          is set to the number of bytes pointed to by "tail". If this field
 *          is used, the packet flag BTP_FLAG_TAIL must be set.
 *
 *          The packet's total size ("headerLen" plus "dataLen" plus
 *          "tailLen", if present) must be less than or equal to
 *          the remote device's L2CAP MTU.
 *
 *     Link - An associated remote device.
 *
 * Returns:
 *     BT_STATUS_PENDING - Packet was successfully queued for transmission on
 *          the connectionless channel. The completion of the send request is
 *          indicated by an L2EVENT_PACKET_HANDLED event.
 *
 *     BT_STATUS_FAILED - The BdAddr is non-null and the remote device does
 *          not support unicast data.
 *
 *     BT_STATUS_INVALID_TYPE - The Role is not correct for a broadcast.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this
 *          connectionless channel is no longer active.
 */
BtStatus L2CAP_SendConnectionless(L2capConnectionlessHandler *Handler,
                                  L2capPsmValue Psm, BtPacket *Packet,
                                  BtRemoteDevice *Link);



/*---------------------------------------------------------------------------
 * L2CAP_GetAgentFromCid()
 *
 *     Retrieves the agent of the connected channel.
 *
 * Parameters:
 *     cid - Channel ID to query.
 *
 * Returns:
 *     Agent - Pointer to the L2capAgent structure associated with the
 *          connected channel.
 *
 *     0 - If the channel does not exist or is not connected.
 */
void *L2CAP_GetAgentFromCid(L2capChannelId Cid);



#define BT_INVALID_PSM      BT_CLIENT_ONLY_PSM

void L2Cap_SecAccessRsp(const BtEvent *Event);

#endif /* __L2CAP_H */
