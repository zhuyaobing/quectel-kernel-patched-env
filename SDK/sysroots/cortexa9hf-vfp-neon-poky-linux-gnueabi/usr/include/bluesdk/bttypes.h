/****************************************************************************
 *
 * File:
 *     bttypes.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     General types, defines, and prototypes for Blue SDK components.
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

#ifndef __BTTYPES_H
#define __BTTYPES_H

#include "xatypes.h"
#include "btconfig.h"
#include "xastatus.h"
#include "utils.h"


/*---------------------------------------------------------------------------
 * Global Types layer
 *
 *     General types and conversion functions used by all components of
 *     the stack and applications are defined here.
 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BDADDR_NTOA_SIZE constant
 *
 *     Defines the minimum size for the string when using the bdaddr_ntoa
 *     function.
 */
#define BDADDR_NTOA_SIZE        18


/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtStatus type
 *
 *     This type is returned from most stack APIs to indicate the success
 *     or failure of the operation. In many cases, BT_STATUS_PENDING
 *     is returned, meaning that a future callback will indicate the
 *     result of the operation.
 */
typedef XaStatus BtStatus;

#define BT_STATUS_SUCCESS           XA_STATUS_SUCCESS
#define BT_STATUS_FAILED            XA_STATUS_FAILED
#define BT_STATUS_PENDING           XA_STATUS_PENDING
#define BT_STATUS_DISCONNECT        XA_STATUS_DISCONNECT
#define BT_STATUS_NO_LINK           XA_STATUS_NO_LINK
#define BT_STATUS_BUSY              XA_STATUS_BUSY
#define BT_STATUS_NO_RESOURCES      XA_STATUS_NO_RESOURCES
#define BT_STATUS_NOT_FOUND         XA_STATUS_NOT_FOUND
#define BT_STATUS_DEVICE_NOT_FOUND  XA_STATUS_DEVICE_NOT_FOUND
#define BT_STATUS_CONNECTION_FAILED XA_STATUS_CONNECTION_FAILED
#define BT_STATUS_TIMEOUT           XA_STATUS_TIMEOUT
#define BT_STATUS_NO_CONNECTION     XA_STATUS_NO_CONNECTION
#define BT_STATUS_INVALID_PARM      XA_STATUS_INVALID_PARM
#define BT_STATUS_IN_PROGRESS       XA_STATUS_IN_PROGRESS
#define BT_STATUS_RESTRICTED        XA_STATUS_RESTRICTED
#define BT_STATUS_INVALID_TYPE      XA_STATUS_INVALID_TYPE
#define BT_STATUS_HCI_INIT_ERR      XA_STATUS_HCI_INIT_ERR
#define BT_STATUS_NOT_SUPPORTED     XA_STATUS_NOT_SUPPORTED
#define BT_STATUS_IN_USE            XA_STATUS_IN_USE
#define BT_STATUS_SDP_CONT_STATE    XA_STATUS_CONTINUE
#define BT_STATUS_CANCELLED         XA_STATUS_CANCELLED
#define BT_STATUS_QOS_FAILED        XA_STATUS_QOS_FAILED
#define BT_STATUS_NOT_READY         XA_STATUS_NOT_READY
#define BT_STATUS_FAIL_SECURITY     XA_STATUS_FAIL_SECURITY
#define BT_STATUS_PROTOCOL_ERROR    XA_STATUS_PROTOCOL_ERROR
#define BT_STATUS_CACHE_CLEARED     XA_STATUS_CACHE_CLEARED
#define BT_STATUS_FULL              XA_STATUS_FULL
#define BT_STATUS_PRIVACY_ERROR     XA_STATUS_PRIVACY_ERROR
#define BT_STATUS_REPEATED_ATTEMPTS XA_STATUS_REPEATED_ATTEMPTS

/* The last defined status code */
#define BT_STATUS_LAST_CODE         XA_STATUS_LAST_CODE
/* End of BtStatus */

/*---------------------------------------------------------------------------
 * BtErrorCode type
 *
 *     Equivalent to HcStatus in hci.h unless otherwise noted.
 */
typedef U8 BtErrorCode;

#define BEC_NO_ERROR                  0x00 /* No error */
#define BEC_UNKNOWN_HCI_CMD           0x01 /* Unknown HCI Command */
#define BEC_NO_CONNECTION             0x02 /* No connection */
#define BEC_HARDWARE_FAILURE          0x03 /* Hardware Failure */
#define BEC_PAGE_TIMEOUT              0x04 /* Page timeout */
#define BEC_AUTHENTICATE_FAILURE      0x05 /* Authentication failure */
#define BEC_MISSING_KEY               0x06 /* Missing key */
#define BEC_MEMORY_FULL               0x07 /* Memory full */
#define BEC_CONNECTION_TIMEOUT        0x08 /* Connection timeout */
#define BEC_MAX_CONNECTIONS           0x09 /* Max number of connections */
#define BEC_MAX_SCO_CONNECTIONS       0x0a /* Max number of SCO connections to a device */
#define BEC_ACL_ALREADY_EXISTS        0x0b /* The ACL connection already exists. */
#define BEC_COMMAND_DISALLOWED        0x0c /* Command disallowed */
#define BEC_LIMITED_RESOURCE          0x0d /* Host rejected due to limited resources */
#define BEC_SECURITY_ERROR            0x0e /* Host rejected due to security reasons */
#define BEC_PERSONAL_DEVICE           0x0f /* Host rejected (remote is personal device) */
#define BEC_HOST_TIMEOUT              0x10 /* Host timeout */
#define BEC_UNSUPPORTED_FEATURE       0x11 /* Unsupported feature or parameter value */
#define BEC_INVALID_HCI_PARM          0x12 /* Invalid HCI command parameters */
#define BEC_USER_TERMINATED           0x13 /* Other end terminated (user) */
#define BEC_LOW_RESOURCES             0x14 /* Other end terminated (low resources) */
#define BEC_POWER_OFF                 0x15 /* Other end terminated (about to power off) */
#define BEC_LOCAL_TERMINATED          0x16 /* Terminated by local host */
#define BEC_REPEATED_ATTEMPTS         0x17 /* Repeated attempts */
#define BEC_PAIRING_NOT_ALLOWED       0x18 /* Pairing not allowed */
#define BEC_UNKNOWN_LMP_PDU           0x19 /* Unknown LMP PDU */
#define BEC_UNSUPPORTED_REMOTE        0x1a /* Unsupported Remote Feature */
#define BEC_SCO_OFFSET_REJECT         0x1b /* SCO Offset Rejected */
#define BEC_SCO_INTERVAL_REJECT       0x1c /* SCO Interval Rejected */
#define BEC_SCO_AIR_MODE_REJECT       0x1d /* SCO Air Mode Rejected */
#define BEC_INVALID_LMP_PARM          0x1e /* Invalid LMP Parameters */
#define BEC_UNSPECIFIED_ERR           0x1f /* Unspecified Error */
#define BEC_UNSUPPORTED_LMP_PARM      0x20 /* Unsupported LMP Parameter Value */
#define BEC_ROLE_CHG_NOT_ALLOWED      0x21 /* Role Change Not Allowed */
#define BEC_LMP_RESPONSE_TIMEOUT      0x22 /* LMP Response Timeout */
#define BEC_LMP_TRANS_COLLISION       0x23 /* LMP Error Transaction Collision */
#define BEC_LMP_PDU_NOT_ALLOWED       0x24 /* LMP PDU Not Allowed */
#define BEC_ENCRYP_MODE_NOT_ACC       0x25 /* Encryption Mode Not Acceptable */
#define BEC_UNIT_KEY_USED             0x26 /* Unit Key Used */
#define BEC_QOS_NOT_SUPPORTED         0x27 /* QoS is Not Supported */
#define BEC_INSTANT_PASSED            0x28 /* Instant Passed */
#define BEC_PAIR_UNITKEY_NO_SUPP      0x29 /* Pairing with Unit Key Not Supported */
#define BEC_DIFF_TRANS_COLLISION      0x2A /* Different transaction collision */
#define BEC_INSUFF_RES_SCATTER_MODE   0x2B /* Insufficient resources for scatter mode */
#define BEC_QOS_UNACCEPTABLE_PARM     0x2C /* QOS unacceptable parameter */
#define BEC_QOS_REJECTED              0x2D /* QOS rejected */
#define BEC_CHAN_CLASS_NOT_SUPPORTED  0x2E /* Channel classification not supported */
#define BEC_INSUFFICIENT_SECURITY     0x2F /* Insufficient security */
#define BEC_PARM_OUT_OF_MAND_RANGE    0x30 /* Parameter out of mandatory range */
#define BEC_SCAT_MODE_NOT_REQ         0x31 /* Scatter mode no longer required */
#define BEC_ROLE_SWITCH_PENDING       0x32 /* Role switch pending */
#define BEC_SCAT_MODE_PARM_CHNG_PEND  0x33 /* Scatter mode parameter change pending */
#define BEC_RESERVED_SLOT_VIOLATION   0x34 /* Reserved slot violation */
#define BEC_SWITCH_FAILED             0x35 /* Role switch failed */
#define BEC_EXT_INQ_RESP_TOO_LARGE    0x36 /* Extended Inquiry Response too large */
#define BEC_SSP_NOT_SUPPORTED         0x37 /* Secure Simple Pairing not supported */
#define BEC_HOST_BUSY_PAIRING         0x38 /* Host is busy pairing */
#define BEC_CHAN_REJECT_NONE_SUITABLE 0x39 /* Connection rejected due to no sutiable channel found*/
#define BEC_CONTROLLER_BUSY           0x3A /* Controller busy */
#define BEC_UNACCEPT_CONN_INTERVAL    0x3B /* Unacceptable Connection interval */
#define BEC_DIRECTED_ADVERT_TIMEOUT   0x3C /* Directed Advertising timeout */
#define BEC_CONNECTION_TERM_MIC_FAIL  0x3D /* Connection terminated due to MIC failure */
#define BEC_CONNECTION_FAIL_ESTABLISH 0x3E /* Connection failed to be established */
#define BEC_MAC_CONNECTION_FAILED     0x3F /* MAC Connection failed */
#define BEC_COURSE_CLOCK_ADJ_REJECTED 0x40 /* Course Clock Adjustement Rejected but will adjust with Clock Dragging */

/* Group: The following are additional error codes not covered in HcStatus */
#define BEC_NOT_FOUND                 0xf1 /* Item not found */
#define BEC_REQUEST_CANCELLED         0xf2 /* Pending request cancelled */


/* Group: The following error codes are used when the
 * SDEVENT_QUERY_FAILED event is sent.
 */
#define BEC_INVALID_SDP_PDU           0xd1 /* SDP response PDU is invalid */
#define BEC_SDP_DISCONNECT            0xd2 /* The SDP L2CAP channel or link disconnected */
#define BEC_SDP_NO_RESOURCES          0xd3 /* Not enough L2CAP resources */
#define BEC_SDP_INTERNAL_ERR          0xd4 /* Some type of internal stack error */

/* Group: The following error code is used when the
 * BTEVENT_PAIRING_COMPLETE event is sent.
 */
#define BEC_STORE_LINK_KEY_ERR        0xe0

/* End of BtErrorCode */

/*---------------------------------------------------------------------------
 * BtPacketFlags type
 *
 *     This type is used by L2CAP and protocols that use directly L2CAP
 *     to manage the status of a particular BtPacket.
 */
typedef U16 BtPacketFlags;

#define BTP_FLAG_NONE           0x0000  /* No current flags */
#define BTP_FLAG_INUSE          0x0001  /* Used only by packet owner */
#define BTP_FLAG_LSYSTEM        0x0002  /* Used only by L2CAP */
#define BTP_FLAG_TAIL           0x0004  /* Used only by L2CAP Applications */
#define BTP_FLAG_RDEV           0x0008  /* Used only by L2CAP */
#define BTP_FLAG_FCS            0x0010  /* FCS field is valid, set only by L2CAP */
#define BTP_FLAG_NON_FLUSH      0x0020  /* Used by L2CAP, HCI or packet owner */
#define BTP_FLAG_ENHANCED       0x0040  /* Used only by L2CAP */
#define BTP_FLAG_RESEGMENT      0x0080  /* Used only by L2CAP */
#define BTP_FLAG_TXDONE         0x0100  /* Used only by L2CAP */
#define BTP_FLAG_BROADCAST      0x0200  /* Used by L2CAP or packet owner */
#define BTP_FLAG_JETTEST        0x8000  /* Used only by L2CAP */

/* End of BtPacketFlags */


/*---------------------------------------------------------------------------
 * BtPriority type
 *
 *     Specifies the priority of an L2CAP channel or BtPacket.
 */
typedef U8 BtPriority;

/* Used internally by the stack. */
#define BT_PRIORITY_SYSTEM      0x01

/* L2CAP 1st priority. */
#define BT_PRIORITY_SIGNAL      0x02

/* L2CAP 2nd priority. */
#define BT_PRIORITY_GUARANTEED  0x03

/* L2CAP 3rd priority. */
#define BT_PRIORITY_BEST_EFFORT 0x04

/* End of BtPriority */


/*---------------------------------------------------------------------------
 * HciTransError type
 *
 *     Describes the type of transport error encountered.
 */
typedef U8 HciTransError;

/* No transport error has occurred. */
#define HCI_TRANSERROR_NONE                0x00

/* An unknown transport error occurred */
#define HCI_TRANSERROR_UNKNOWN             0x01

/* Poorly formed data was received from the radio */
#define HCI_TRANSERROR_ILLEGAL_DATA        0x02

/* The Radio itself indicated a hardware failure */
#define HCI_TRANSERROR_HARDWARE            0x03

/* A command required by the stack failed during initialization */
#define HCI_TRANSERROR_REQUIRED_CMD_FAILED 0x04

/* A command was issued but no response was ever heard. See
 * HCI_RESET_TIMEOUT. */
#define HCI_TRANSERROR_CMD_TIMEOUT         0x05

/* An error occurred while writing data to the transport */
#define HCI_TRANSERROR_WRITE_ERROR         0x06

/* An error occurred while reading data from the transport */
#define HCI_TRANSERROR_READ_ERROR          0x07

/* A buffer could not be allocated */
#define HCI_TRANSERROR_BUFFER_ERROR        0x08

/* The transport was externally shut down or no longer responds */
#define HCI_TRANSERROR_SHUTDOWN            0x09

/* End of HciTransError */

/*---------------------------------------------------------------------------
 * BtServiceId type
 *
 *    System-specific identifier used to identity a service/profile. The
 *    intent of BtServiceId is that an actual ID be defined for each
 *    system to match the desired capabilities and features needed by the
 *    system. For example the Service ID could be a pointer to a data
 *    structure containing a UUID and other profile specific data or it could
 *    be taken from a simple set of #define values.
 */
typedef void *BtServiceId;

/* End of BtServiceId type */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtPacket structure
 *
 *     Represents a packet of data. These packets may be used by applications
 *     to provide data to the stack. Certain stack events may also provide
 *     data to the application enclosed in a BtPacket structure.
 */
typedef struct _BtPacket
{
    ListEntry    node;    /* Used internally by the stack. */

    U8          *data;    /* Points to a buffer of user data.  */
    U16          dataLen; /* Indicates the length of "data" in bytes. */

    BtPacketFlags flags;  /* Must be initialized to BTP_FLAG_NONE by
                           * applications running on top of L2CAP. Other
                           * higher layer protocols must never modify this
                           * value.
                           */

    BtPriority   priority; /* May be used to override the L2CAP channel
                            * BtPriority. If set to 0, packet will use the
                            * default priority set for the L2CAP channel.
                            */

    /* Group: The following fields are for internal use only by the stack. */
    void        *ulpContext;
    U8          *tail;
    U16          tailLen;

#if XA_STATISTICS == XA_ENABLED
    U32          rfc_timer;
    U32          hci_timer;
    U32          l2cap_timer;
    U32          l2cap_sdu;
#endif /* XA_STATISTICS == XA_ENABLED */

    U16          llpContext;
    void        *ulpChannel;
    void        *llpChannel;
    U16          remoteCid;
    void        *lLink;

    U16          sduHdrLen;
    U16          sduDataLen;
    U16          sduTailLen;
    U16          segStart;
    U16          segNum;
    U16          segCount;
    U16          dataOffset;
    U16          segCountOffset;
    U8           fcs[2];

    U8           hciPackets;
    U8           completedPackets;
    U8           cpFlags;
    U8           headerLen;
    U8           header[BT_PACKET_HEADER_LEN];
} BtPacket;


/* Indicates size of BD_ADDR array */
#define BD_ADDR_SIZE    6

/*---------------------------------------------------------------------------
 * BD_ADDR structure
 *
 *     Represents a 48-bit Bluetooth device address.
 */
typedef struct _BD_ADDR {
    U8    addr[BD_ADDR_SIZE];
} BD_ADDR;


/* Internal ROM constant defined in btrom.c to represent an invalid address */
extern const BD_ADDR INVALID_BDADDR;


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * bdaddr_aton()
 *
 *     Converts an ASCII-formatted Bluetooth device address into a BD_ADDR
 *     structure.
 *
 * Parameters:
 *     addr - Address in the format "xx:xx:xx:xx:xx", where x is a
 *         hexadecimal digit. The most significant byte appears first.
 *
 * Returns:
 *     BD_ADDR - A structure that contains the binary version of the
 *         address.
 */
BD_ADDR  bdaddr_aton(const char *addr);


/*---------------------------------------------------------------------------
 * bdaddr_ntoa()
 *
 *     Converts a BD_ADDR structure into an ASCII-formatted device address.
 *
 * Parameters:
 *     addr - Binary device address to convert.
 *
 *     addrString - Destination string space for formatted device address.
 *         The string must contain at least BDADDR_NTOA_SIZE bytes of space.
 *
 * Returns:
 *     char * - A copy of the pointer passed in as "addrString".
 */
char    *bdaddr_ntoa(const BD_ADDR *addr, char *addrString);

/*---------------------------------------------------------------------------
 * bdaddr_swap()
 *
 *     Reverses a BD_ADDR.
 *
 * Parameters:
 *     addr - Binary device address to convert.
 *
 * Returns:
 *     BD_ADDR - A structure that contains the reversed binary version of
 *               the address.
 */
BD_ADDR bdaddr_swap(const BD_ADDR *Addr);

/*---------------------------------------------------------------------------
 * AreBdAddrsEqual()
 *
 *     Compares two Bluetooth device addresses for equality.
 *
 * Parameters:
 *     addrA - First address to compare.
 *
 *     addrB - Second address to compare.
 *
 * Returns:
 *     TRUE when the addresses are the same, FALSE when they are different
 */
BOOL    AreBdAddrsEqual(const BD_ADDR *addrA, const BD_ADDR *addrB);
#define AreBdAddrsEqual(_ADDRA, _ADDRB)   OS_MemCmp((_ADDRA)->addr, 6, (_ADDRB)->addr, 6)


/*---------------------------------------------------------------------------
 * IsValidBdAddr()
 *
 *     Determines whether a BD_ADDR contains a valid Bluetooth device
 *     address.
 *
 * Parameters:
 *     addr - Address to examine.
 *
 * Returns:
 *     TRUE when the address is valid, FALSE if it is invalid.
 */
BOOL    IsValidBdAddr(const BD_ADDR *Addr);
#define IsValidBdAddr(_BDA) (OS_MemCmp((_BDA)->addr, 6, INVALID_BDADDR.addr, 6) == FALSE)

#endif /* __BTTYPES_H */
