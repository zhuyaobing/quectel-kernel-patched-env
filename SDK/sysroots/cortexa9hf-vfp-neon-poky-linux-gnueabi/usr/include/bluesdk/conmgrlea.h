/****************************************************************************
 *
 * File:
 *     conmgrlea.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions for the LE Advertising capability
 *     of Connection Manager.
 *
 * Created:
 *     November 21, 2011
 *
 * Copyright 2011-2017 OpenSynergy GmbH.
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

#ifndef __CONMGRADVERT_H_
#define __CONMGRADVERT_H_

#include "conmgr.h"

#if NUM_LE_DEVICES > 0
/*---------------------------------------------------------------------------
 * Connection Manager LE Advertising API layer
 *
 *     The Connection Manager Advertising API provides a set of functions
 *     for managing LE Advertising behavior.  This includes interactions
 *     with the LE Management Entity component (MELE) to setup the
 *     advertising parameters, advertising data, and start/stop advertising
 *     behavior.
 *
 *     See the Blue SDK Connection Manager API layer for core connection
 *     creation and disconnection API.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/* Forward reference */
typedef struct _CmgrLeAdvertEvent CmgrLeAdvertEvent;

/*---------------------------------------------------------------------------
 * CmgrLeAdvertCallback type
 *
 * A function of this type is called to indicate advertising events to the profile.
 */
typedef void (*CmgrLeAdvertCallback)(void *context, CmgrLeAdvertEvent *event);

/* End of CmgrLeAdvertCallback */

/*---------------------------------------------------------------------------
 * CmgrLeAdvertPriority type
 *
 * LE Advertising priority values.  
 * 
 * NOTE: This type has been deprecated and is no longer used within LE 
 *     Advertising.
 */
typedef U8 CmgrLeAdvertPriority;

/* Normal priority - another operation can override the current advertising settings */
#define CMGR_LE_ADVERT_PRIORITY_NORMAL          0

/* High priority - another operation cannot override the current advertising settings */
#define CMGR_LE_ADVERT_PRIORITY_HIGH            1

/* End of CmgrLeAdvertPriority */

/*---------------------------------------------------------------------------
 * CmgrLeAdvertDiscoveryMode type
 *
 * LE Advertising discovery mode values.  These values allow setting general,
 * limited, or non-discoverable modes.  However, each discovery mode has a
 * subset of possible Advertising Types that are allowed.  Additionally, the
 * discovery mode will indicate what advertising data flag capabilities are
 * sent.
 */
typedef U8 CmgrLeAdvertDiscoveryMode;

/* Non-discoverable mode - all advertising types are allowed */
#define CMGR_LE_ADVERT_NOT_DISCOVERABLE        1

/* General Discoverable mode - allowed advertising types are:
 * MLAT_CONNECTABLE_UNDIRECTED, MLAT_DISCOVERABLE_UNDIRECTED, and
 * MLAT_NONCONNECTABLE_UNDIRECTED
 */
#define CMGR_LE_ADVERT_GENERAL_DISCOVERABLE    2

/* Limited Discoverable mode - allowed advertising types are:
 * MLAT_CONNECTABLE_UNDIRECTED, MLAT_DISCOVERABLE_UNDIRECTED, and
 * MLAT_NONCONNECTABLE_UNDIRECTED
 */
#define CMGR_LE_ADVERT_LIMITED_DISCOVERABLE    3

/* End of CmgrLeAdvertDiscoveryMode */

/*---------------------------------------------------------------------------
 * CmgrLeAdvertFlags type
 *
 * LE Advertising flag values.
 */
typedef U16 CmgrLeAdvertFlags;

/* Advertising handler is registered */
#define CMGR_LE_ADVERT_FLAG_REGISTERED          0x01

/* Override the existing advertising parameters and data with new values */
#define CMGR_LE_ADVERT_FLAG_OVERRIDE            0x02

/* Second advertising interval timer is active */
#define CMGR_LE_ADVERT_FLAG_SECONDARY_TIMER     0x04

/* End of CmgrLeAdvertFlags */

/*---------------------------------------------------------------------------
 * CmgrLeAdvertInt type
 *
 * LE Interval values used for advertising. The time = value * 0.625 ms.
 * Below are values expressed in milliseconds commonly used in profiles.
 */
typedef U16 CmgrLeAdvertInt;

#define CMGR_LE_ADVERT_INT_20MS                 32      /* Minimum possible value */
#define CMGR_LE_ADVERT_INT_25MS                 40
#define CMGR_LE_ADVERT_INT_30MS                 48
#define CMGR_LE_ADVERT_INT_35MS                 56
#define CMGR_LE_ADVERT_INT_40MS                 64
#define CMGR_LE_ADVERT_INT_45MS                 72
#define CMGR_LE_ADVERT_INT_50MS                 80
#define CMGR_LE_ADVERT_INT_55MS                 88
#define CMGR_LE_ADVERT_INT_60MS                 96
#define CMGR_LE_ADVERT_INT_100MS               160
#define CMGR_LE_ADVERT_INT_200MS               320
#define CMGR_LE_ADVERT_INT_500MS               800
#define CMGR_LE_ADVERT_INT_1000MS             1600
#define CMGR_LE_ADVERT_INT_1280MS             2048
#define CMGR_LE_ADVERT_INT_1500MS             2400
#define CMGR_LE_ADVERT_INT_2000MS             3200
#define CMGR_LE_ADVERT_INT_2500MS             4000
#define CMGR_LE_ADVERT_INT_2560MS             4096
#define CMGR_LE_ADVERT_INT_5120MS             8192
#define CMGR_LE_ADVERT_INT_10240MS           16384      /* Maximum possible value */

/* End of CmgrLeAdvertInt */

/*---------------------------------------------------------------------------
 * CmgrLeAdvertUuidPriority type
 *
 * LE Advertising UUID Priority values.
 */
typedef U8 CmgrLeAdvertUuidPriority;

#define CMGR_LE_ADVERT_UUID_PRIORITY_LOW        1
#define CMGR_LE_ADVERT_UUID_PRIORITY_MEDIUM     2
#define CMGR_LE_ADVERT_UUID_PRIORITY_HIGH       3

/* End of CmgrLeAdvertUuidPriority */

/*---------------------------------------------------------------------------
 * CmgrLeStopAdvertReason type
 *
 *     Indicates the various reason codes why advertising may have been
 *     stopped.
 */
typedef U8 CmgrLeStopAdvertReason;

/* User stopped advertising */
#define SADR_USER_TERMINATED                    0x00

/* Advertising stopped due to interval timeout */
#define SADR_INTERVAL_TIMEOUT                   0x01

/* Connection forced advertising to stop - this occurs if an incoming
 * connection has completed or if an outgoing connection is attempted
 * while advertising indicates that the device is connectable.  The 
 * user can re-enable advertising using CMGR_StartLeAdvertising() if 
 * needed.  All advertising types are supported when in an active 
 * connection with 4.1 controllers, but only the discoverable type
 * is supported on 4.0 controllers.
 */
#define SADR_CONNECTION_TERMINATED              0x02

/* Directed Advertising Timeout */
#define SADR_DIRECTED_ADVERT_TIMEOUT            0x03

/* Temporarily paused for enabling/disabling privacy */
#define SADR_PRIVACY_PAUSED                     0x04

/* Temporarily paused for enabling/disabling link layer privacy */
#define SADR_LINK_PRIVACY_PAUSED                0x05

/* End of CmgrLeStopAdvertReason */

/*---------------------------------------------------------------------------
 * CmgrLeAdType type
 *
 *     Specifies the possible advertising (AD) types that are possible
 *     to include advertising packets.
 */
typedef U8 CmgrLeAdType;

#define CMGR_LE_ADTP_FLAGS                      0x01    /* Flags */
#define CMGR_LE_ADTP_PARTIAL_16BIT_UUIDS        0x02    /* Incomplete list of 16-bit service UUID's */
#define CMGR_LE_ADTP_COMPLETE_16BIT_UUIDS       0x03    /* Complete list of 16-bit service UUID's */
#define CMGR_LE_ADTP_PARTIAL_32BIT_UUIDS        0x04    /* Incomplete list of 32-bit service UUID's */
#define CMGR_LE_ADTP_COMPLETE_32BIT_UUIDS       0x05    /* Complete list of 32-bit service UUID's */
#define CMGR_LE_ADTP_PARTIAL_128BIT_UUIDS       0x06    /* Incomplete list of 128-bit service UUID's */
#define CMGR_LE_ADTP_COMPLETE_128BIT_UUIDS      0x07    /* Complete list of 128-bit service UUID's */
#define CMGR_LE_ADTP_SHORTENED_NAME             0x08    /* Shortened Local Name */
#define CMGR_LE_ADTP_COMPLETE_NAME              0x09    /* Complete Local Name */
#define CMGR_LE_ADTP_TX_POWER_LEVEL             0x0A    /* Transmit Power Level */
#define CMGR_LE_ADTP_COD                        0x0D    /* Class of Device */
#define CMGR_LE_ADTP_PAIRING_HASH               0x0E    /* Simple Pairing Hash (C or C-192) */
#define CMGR_LE_ADTP_PAIRING_RANDOMIZER         0x0F    /* Simple Pairing Randomizer (R or R-192) */
#define CMGR_LE_ADTP_DEVICE_ID                  0x10    /* Device ID */
#define CMGR_LE_ADTP_SECURITY_TK_VALUE          0x10    /* Security Manager TK Value */
#define CMGR_LE_ADTP_SECURITY_OOB_FLAGS         0x11    /* Security Manager Out of Band Flags */
#define CMGR_LE_ADTP_SLAVE_CONN_INTERVAL        0x12    /* Slave Connection Interval Range */
#define CMGR_LE_ADTP_16BIT_SERVICE_UUIDS        0x14    /* List of 16-bit Service Solicitation UUID's */
#define CMGR_LE_ADTP_128BIT_SERVICE_UUIDS       0x15    /* List of 128-bit Service Solicitation UUID's */
#define CMGR_LE_ADTP_16BIT_SERVICE_DATA_UUID    0x16    /* Service Data - 16-bit UUID */
#define CMGR_LE_ADTP_PUBLIC_ADDRESS             0x17    /* Public Target Address */
#define CMGR_LE_ADTP_RANDOM_ADDRESS             0x18    /* Random Target Address */
#define CMGR_LE_ADTP_APPEARANCE                 0x19    /* Appearance */
#define CMGR_LE_ADTP_INTERVAL                   0x1A    /* Advertising Interval */
#define CMGR_LE_ADTP_LE_ADDRESS                 0x1B    /* LE Bluetooth Device Address */
#define CMGR_LE_ADTP_LE_ROLE                    0x1C    /* LE Role */
#define CMGR_LE_ADTP_PAIRING_HASH_256           0x1D    /* Simple Pairing Hash C-256 */
#define CMGR_LE_ADTP_PAIRING_RANDOMIZER_256     0x1E    /* Simple Pairing Randomizer R-256 */
#define CMGR_LE_ADTP_32BIT_SERVICE_UUIDS        0x1F    /* List of 32-bit Service Solicitation UUID's */
#define CMGR_LE_ADTP_32BIT_SERVICE_DATA_UUID    0x20    /* Service Data - 32-bit UUID */
#define CMGR_LE_ADTP_128BIT_SERVICE_DATA_UUID   0x21    /* Service Data - 128-bit UUID */
#define CMGR_LE_ADTP_SECURE_CONN_CONFIRM_VALUE  0x22    /* Secure Connections Confirmation Value */
#define CMGR_LE_ADTP_SECURE_CONN_RANDOM_VALUE   0x23    /* Secure Connections Random Value */
#define CMGR_LE_ADTP_URI                        0x24    /* URI */
#define CMGR_LE_ADTP_INDOOR_POSITIONING         0x25    /* Indoor Positioning Service */
#define CMGR_LE_ADTP_TRANSPORT_DISCOVERY_DATA   0x26    /* Transport Discovery Data */
#define CMGR_LE_ADTP_3D_DATA                    0x3D    /* 3D Information Data */
#define CMGR_LE_ADTP_MANUFACTURER_DATA          0xFF    /* Manufacturer Specific Data */

/* End of CmgrLeAdType */

/*---------------------------------------------------------------------------
 * CmgrLeAdTypeMask type
 *
 *     Specifies which AD types are included in advertising packets and
 *     scan response packets. If set, the AD type is included (if the 
 *     corresponding information is present in the Connection Manager) 
 *     otherwise it is excluded. The flag AD type cannot be enabled/disabled 
 *     and is always sent in advertising packets if non-0 (not scan response 
 *     packets). The values are ORed together. Different settings
 *     are used if privacy is enabled or not. The default when privacy
 *     is enabled is CMGR_LE_ATPM_NONE and the default for when privacy is not
 *     enabled is CMGR_LE_ATPM_ALL.
 */
typedef U16 CmgrLeAdTypeMask;

/* Service UUIDs */
#define CMGR_LE_ATPM_SERV_UUID     0x0001

/* Local Name */
#define CMGR_LE_ATPM_LOCAL_NAME    0x0002

/* Manufacturer Specific Data */
#define CMGR_LE_ATPM_MAN_DATA      0x0004

/* TX Power Level */
#define CMGR_LE_ATPM_TX_POWER      0x0008

/* Slave Connection Interval Range */
#define CMGR_LE_ATPM_SLAVE_CON_INT 0x0010

/* Service Solicitation */
#define CMGR_LE_ATPM_SERV_SOL      0x0020

/* Service Data */
#define CMGR_LE_ATPM_SERV_DATA     0x0040

/* Appearance */
#define CMGR_LE_ATPM_APPEARANCE    0x0080

/* Public Target Address */
#define CMGR_LE_ATPM_PUB_ADDR      0x0100

/* Random Target Address */
#define CMGR_LE_ATPM_RAND_ADDR     0x0200

/* Advertising Interval */
#define CMGR_LE_ATPM_ADV_INT       0x0400

/* LE Address */
#define CMGR_LE_ATPM_LE_ADDR       0x0800

/* LE Role */
#define CMGR_LE_ATPM_LE_ROLE       0x1000

/* URI */
#define CMGR_LE_ATPM_URI           0x2000

/* Custom AD Types */
#define CMGR_LE_ATPM_CUSTOM        0x4000

/* All of them */
#define CMGR_LE_ATPM_ALL           0xFFFF

/* None of them */
#define CMGR_LE_ATPM_NONE          0x0000

/* End of CmgrLeAdTypeMask */

/*---------------------------------------------------------------------------
 * CmgrLeAdRole type
 *
 *     Specifies the LE Role value for the advertising type 
 *     CMGR_LE_ADTP_LE_ROLE.
 */
typedef U16 CmgrLeAdRole;

/* Peripheral Role Only */
#define CMGR_LE_ADR_PERIPHERAL_ONLY         0x00

/* Central Role Only */
#define CMGR_LE_ADR_CENTRAL_ONLY            0x01

/* Peripheral and Central Role supported.  Peripheral role preferred for 
 * connection establishment.
 */
#define CMGR_LE_ADR_PERIPHERAL_PREF_ROLE    0x02

/* Peripheral and Central Role supported.  Central role preferred for 
 * connection establishment.
 */
#define CMGR_LE_ADR_CENTRAL_PREF_ROLE       0x03


/* End of CmgrLeAdRole */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * CmgrLeAdvertUuid structure
 *
 *     CmgrLeAdvertUuid holds a 16-bit or 128-bit UUID
 */
typedef struct _CmgrLeAdvertUuid {
    ListEntry                   node;           /* Internal use only */

    CmgrLeAdvertUuidPriority    priority;       /* Priority of the UUID */
    U8                          len;            /* length of UUID in octets (2, 4 or 16) */

    /* UUID */
    union {
        U8                      uuid128[16];    /* 128-bit UUID */
        U32                     uuid32;         /* 32-bit UUID */
        U16                     uuid16;         /* 16-bit UUID */
    } uuid;
} CmgrLeAdvertUuid;

/*---------------------------------------------------------------------------
 * CmgrLeAdvertTLV structure
 *
 *     CmgrLeAdvertTLV holds the type, length, and value of a new advertising
 *     type to include in the advertising data.
 */
typedef struct _CmgrLeAdvertTLV {
    ListEntry                   node;           /* Internal use only */

    CmgrLeAdType                type;           /* Advertising type */
    U8                          len;            /* Length of value */

    union {
        U8                      oneByte;        /* 1-byte value */
        U16                     twoBytes;       /* 2-byte value */
        BD_ADDR                 address;        /* 6-byte address value (stored in Little Endian) 
                                                 * Valid for CMGR_LE_ADTP_PUBLIC_ADDRESS and 
                                                 * CMGR_LE_ADTP_RANDOM_ADDRESS only 
                                                 */
        U8                      leAddress[7];   /* 7-byte address value (stored in Little Endian 
                                                 * and includes the address type)
                                                 * Valid for CMGR_LE_ADTP_LE_ADDRESS only
                                                 */

        U8                     *custom;         /* Pointer to buffer of "len" size (stored in Little Endian). 
                                                 * Used for all adType lengths > 2-bytes.
                                                 */
    } value;
} CmgrLeAdvertTLV;

/*---------------------------------------------------------------------------
 * CmgrLeAdvertParms structure
 *
 *     Used to hold LE advertising parameters.
 */
typedef struct _CmgrLeAdvertParms {

    /* Advertising Type */
    MeLeAdvertType              type;

    /* Advertising Discovery Mode - this allows setting of the general, limited,
     * or non-discoverable modes.  However, each mode has a subset of possible
     * Advertising Types that are allowed.
     *             (see CmgrLeAdvertDiscoveryMode description)
     */
    CmgrLeAdvertDiscoveryMode   discoveryMode;

    /* Advertising Filter Policy */
    MeLeAdvertFilterPolicy      policy;

    /* Advertising Channel Map */
    MeLeAdvertChanMap           map;

    /* Set 1 advertising interval and time settings. This set is used to trigger
     * a frequent advertising interval, which is useful for "fast connection"
     * scenarios where power savings is not an issue. The advertising interval values
     * correspond to time in milliseconds where time = value * 0.625ms. These values
     * are passed to the LE controller and are in operation for time1 milliseconds. If
     * advertising is not completed after time1 milliseconds then set 2 is started.
     *
     * If time1 is set to CMGR_TIME_INIFINTY then the interval runs until advertising
     * is stopped by calling CMGR_StopLeAdvertising() and set 2 will not be used.
     * If time1 is set to 0, then this set is disabled and set 2 will be used instead.
     */
    CmgrLeAdvertInt             frequentIntervalMin;  /* Default setting is CMGR_LE_ADVERT_INT_20MS */
    CmgrLeAdvertInt             frequentIntervalMax;  /* Default setting is CMGR_LE_ADVERT_INT_30MS */
    TimeT                       time1;                /* Default setting is 30000 (30 seconds) */

    /* Set 2 for advertising interval and time settings. This set is used to
     * trigger a less frequent advertising interval, which is useful for
     * "power saving" scenarios, or a situation where a connection is already active.
     * The advertising interval values correspond to time in milliseconds where
     * time = value * 0.625ms. These values are passed to the LE controller when
     * a connection is already active or after the first interval set expires, and are
     * in operation for time2 milliseconds. If advertising is not stopped before time2
     * then advertising is considered complete and the handler is called with
     * CMEVENT_LE_ADVERT_STOPPED and the stopReason field will be set to SADR_INTERVAL_TIMEOUT.
     *
     * If time2 is set to 0 then this set is disabled and will be skipped. If time2 is
     * set to CMGR_TIME_INIFINTY then the interval runs until advertising is stopped
     * by calling CMGR_StopLeAdvertising() is called.
     */
    CmgrLeAdvertInt             powerSaveIntervalMin;  /* Default setting is CMGR_LE_ADVERT_INT_1000MS */
    CmgrLeAdvertInt             powerSaveIntervalMax;  /* Default setting is CMGR_LE_ADVERT_INT_2500MS */
    TimeT                       time2;                 /* Default setting is 60000 (60 seconds) */

    /* If directed advertising is performed 
     * (type == MLAT_CONNECTABLE_DIRECTED or MLAT_LOW_DUTY_DIRECTED) 
     * then peerAddrType and peerAddr must be provided, 
     * otherwise they are ignored and not used.
     */
    MeLeAddressType             peerAddrType;

    /* Device address used for the remote device when type ==
     * MLAT_CONNECTABLE_DIRECTED or MLAT_LOW_DUTY_DIRECTED
     */
    BD_ADDR                     peerAddr;

} CmgrLeAdvertParms;

/*---------------------------------------------------------------------------
 * CmgrLeAdvertVendorData structure
 *
 *     Used to specify vendor specific data used in the advertising data.
 */
typedef struct _CmgrLeAdvertVendorData {
    U16          companyId;  /* Company identifier */
    U8           data[27];   /* Vendor specific data (maximum of 27 bytes are allowed) */
    U8           len;        /* Vendor specific data length (only a portion may be sent) */
    BOOL         priority;   /* Indicates whether vendor data is prioritized ahead of local name and services */
} CmgrLeAdvertVendorData;

/*---------------------------------------------------------------------------
 * CmgrLeAdvertServiceData structure
 *
 *     Used to specify service specific data used in the advertising data.
 */
typedef struct _CmgrLeAdvertServiceData {
    U8           data[27];   /* Service specific data (maximum of 27 bytes are allowed) */
    U8           len;        /* Service specific data length (only a portion may be sent) */
    BOOL         priority;   /* Indicates whether service data is prioritized ahead of local name and services */
    U8           uuidLen;    /* length of UUID in octets (2, 4 or 16) */

    union {
        U16          uuid16;       /* 16-bit service UUID */
        U32          uuid32;       /* 32-bit service UUID */
        U8           uuid128[16];  /* 128-bit service UUID */
    } uuid;
} CmgrLeAdvertServiceData;

/*---------------------------------------------------------------------------
 * CmgrLeAdvertHandler structure
 *
 *     Used to identify the handler for Connection Manager events.
 */
typedef struct _CmgrLeAdvertHandler {
    /* Inherit from BtHandlerEx */
    BtHandlerEx             h;

    /* === Internal use only === */
    CmgrLeAdvertFlags       flags;      /* Advertising flags */
    EvmTimer                timer;
} CmgrLeAdvertHandler;

/*---------------------------------------------------------------------------
 * CmgrLeAdvertEvent structure
 *
 *     Used to pass LE advertising information from the CMGR layer to CMGR
 *     client applications.
 */
struct _CmgrLeAdvertEvent {
    /* Inherit from BtEventEx. Note that "e.handler" contains a pointer
     * to the handler which is the CmgrLeAdvertHandler
     */
    BtEventEx                   e;

    /* Error code valid during all callback events */
    BtErrorCode                 errCode;

    union {
        /* Stopped Advertising reason code - valid for CMEVENTEX_LE_ADVERT_STOPPED event */
        CmgrLeStopAdvertReason  stopReason;

        /* Current advertising parameters - valid for CMEVENTEX_LE_ADVERT_CHANGE event */
        CmgrLeAdvertParms      *parms;
    } info;
};

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * CMGR_InitLeAdvertHandler()
 *
 *     Initializes a connection manager's LE advertising handler. All
 *     advertising handlers must be initialized before they can be passed to
 *     CMGR advertising functions. This function can be used or filling the
 *     contents of the handler with 0 will also suffice. A handler should
 *     only need to be initialized once.
 *
 * Parameters:
 *     handler - Pointer to an advertising handler structure.  This memory
 *         allocated by the application.
 */
void CMGR_InitLeAdvertHandler(CmgrLeAdvertHandler *handler);
#define CMGR_InitLeAdvertHandler(h) OS_MemSet((U8 *)h, 0, sizeof(CmgrLeAdvertHandler))

/*---------------------------------------------------------------------------
 * CMGR_RegisterLeAdvertHandler()
 *
 *     Registers a connection manager LE advertising handler.  A function 
 *     callback must be specified, which will received all advertising events.
 *     Multiple handlers can register to receive advertising events, although
 *     only one handler can start/stop advertising.
 *
 *     Note that this API is deprecated.  Use CMGR_RegisterLeAdvertHandlerEx 
 *     instead.
 *
 * Parameters:
 *     handler - Pointer to the advertising handler structure to use.
 *
 *     callback - Function that is called to receive advertising events from
 *         the connection manager.
 *
 *     priority - Priority to apply for advertising operations on this
 *         handler.  Note that this priority field is no longer supported
 *         in LE advertising.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Initialization was successful.
 *
 *     BT_STATUS_IN_USE - Could not register Handler, because it is
 *         already in use.
 *
 *     BT_STATUS_INVALID_PARM - Callback function or handler are not valid.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 */
BtStatus CMGR_RegisterLeAdvertHandler(CmgrLeAdvertHandler *handler,
                                      CmgrLeAdvertCallback callback,
                                      CmgrLeAdvertPriority priority);

/*---------------------------------------------------------------------------
 * CMGR_RegisterLeAdvertHandlerEx()
 *
 *     Registers a connection manager LE advertising handler.  A function 
 *     callback must be specified, which will received all advertising events.
 *     Multiple handlers can register to receive advertising events, although
 *     only one handler can start/stop advertising.
 *
 * Parameters:
 *     handler - Pointer to the advertising handler structure to use.
 *
 *     callback - Function that is called to receive advertising events from
 *         the connection manager.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Initialization was successful.
 *
 *     BT_STATUS_IN_USE - Could not register Handler, because it is
 *         already in use.
 *
 *     BT_STATUS_INVALID_PARM - Callback function or handler are not valid.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 */
BtStatus CMGR_RegisterLeAdvertHandlerEx(CmgrLeAdvertHandler *handler,
                                        CmgrLeAdvertCallback callback);

/*---------------------------------------------------------------------------
 * CMGR_DeregisterLeAdvertHandler()
 *
 *     Deregisters a connection manager LE advertising handler.
 *
 * Parameters:
 *     handler - Pointer to the advertising handler structure to deregister.
 *         This handler must have been previously registered and not currently
 *         associated with an active advertising operation.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Deinitialization was successful.
 *
 *     BT_STATUS_NOT_FOUND - Could not deregister Handler, because it is
 *         not registered.
 *
 *     BT_STATUS_IN_USE - Handler is currently associated with an advertising
 *         attempt.
 *
 *     BT_STATUS_INVALID_PARM - Handler is not valid.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 */
BtStatus CMGR_DeregisterLeAdvertHandler(CmgrLeAdvertHandler *handler);

/*---------------------------------------------------------------------------
 * CMGR_StartLeAdvertising()
 *
 *     Associate a handler with an active LE advertising operation. If the LE 
 *     controller is not currently advertising and a connection request is 
 *     not in progress then advertising is started using the parameters 
 *     currently set in the Connection Manager. CMGR_SetLeAdvertParms() can 
 *     be used to set the advertising parameters.
 *
 *     If the advertising parameters are using a policy that uses the controller's
 *     white list (i.e. "policy" is set to MLAFP_SCAN_WL_CONN_ANY, MLAFP_SCAN_ANY_CONN_WL 
 *     or MLAFP_SCAN_WL_CONN_WL) then addresses must be added the white list using
 *     CMGR_AddLeWhitelist() before calling CMGR_StartLeAdvertising. If there are no
 *     addresses in the white list and the policy set to use then white list
 *     then BT_STATUS_NOT_READY is returned.
 *
 *     If the LE controller is already advertising, then advertising will be
 *     changed to use to the new advertising parameter values. If the
 *     advertising parameters have not changed, then the existing advertising
 *     will remain in place.
 *
 *     If the LE controller is scanning or connected, advertising can still
 *     be performed, but will adhere to the power savings interval unless
 *     this interval set is intentionally disabled by the caller.  This
 *     behavior should be used sparingly to avoid excessive power consumption.
 *
 *     Once associated the handler must be disassociated with 
 *     CMGR_StopLeAdvertising() before it can be deregistered.
 *
 * Parameters:
 *     handler - Pointer to the advertising handler structure used to trigger
 *         advertising. This handler must have been previously registered and
 *         not currently associated with LE advertising.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Advertising was started successfully. No event
 *         will be generated in this case.
 *
 *     BT_STATUS_PENDING - Operation to start/change advertising was started
 *         successfully. The CMEVENT_LE_ADVERT_STARTED event will be indicated
 *         to the handler callback when advertising has started. Alternatively,
 *         if advertising is already started but merely was changed, then the
 *         CMEVENT_LE_ADVERT_CHANGED event will be indicated instead.
 *
 *     BT_STATUS_NOT_FOUND - Could not start advertising because the handler
 *         is not registered.
 *
 *     BT_STATUS_IN_USE - Handler is currently associated with an advertising
 *         operation, or advertising is currently processing an ongoing
 *         operation.
 *
 *     BT_STATUS_NOT_SUPPORTED - Advertising cannot start since the current
 *         advertising type supports connectable behavior, which is disallowed
 *         when a LE connection is in place or in the process of completing.
 *         The advertising type should be changed to be discoverable using
 *         CMGR_SetLeAdvertParms() after which CMGR_StartLeAdvertising() can
 *         be called again.
 *
 *     BT_STATUS_NOT_READY - advertising parameters field "policy" is set to
 *         MLAFP_SCAN_WL_CONN_ANY, MLAFP_SCAN_ANY_CONN_WL  or 
 *         MLAFP_SCAN_WL_CONN_WL but no addresses have been added to the white
 *         list.
 *
 *     BT_STATUS_INVALID_PARM - Handler fields are not valid.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 */
BtStatus CMGR_StartLeAdvertising(CmgrLeAdvertHandler *handler);

/*---------------------------------------------------------------------------
 * CMGR_StopLeAdvertising()
 *
 *     Disassociates the handler with LE advertising and initiates stopping
 *     of LE advertising behavior. However, this handler will not be 
 *     disassociated fully until the disable advertising operation has 
 *     completed.  If the handler must be removed immediately (as in the 
 *     case of shutdown), then CMGR_EnableLeAdvertising should be used 
 *     instead.
 *
 * Parameters:
 *     handler - Pointer to the advertising handler structure to disassociate
 *         with advertising. This handler must have been previously registered
 *         and currently associated with an active advertising operation.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Advertising was disassociated successfully. No event
 *         will be generated in this case.
 *
 *     BT_STATUS_PENDING - Operation to stop advertising was started
 *         successfully on the associated advertising handler. The 
 *         CMEVENT_LE_ADVERT_STOPPED event will be indicated to all 
 *         registered handlers when advertising has stopped.
 *
 *     BT_STATUS_NOT_FOUND - Could not disassociate the handler as it is
 *         not registered.
 *
 *     BT_STATUS_BUSY - Handler is currently associated with an advertising
 *         operation, or advertising is currently processing an ongoing
 *         operation.
 *
 *     BT_STATUS_PRIVACY_ERROR - A privacy operation is currently underway that
 *         is in the process of stopping and restarting advertising. Wait until
 *         the privacy operation has completed.
 *
 *     BT_STATUS_INVALID_PARM - Handler fields are not valid.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 */
BtStatus CMGR_StopLeAdvertising(CmgrLeAdvertHandler *handler);

/*---------------------------------------------------------------------------
 * CMGR_EnableLeAdvertising()
 *
 *     Allows enabling/disabling of LE advertising on the local device.  If
 *     disabled, all active advertising operations will be terminated, all
 *     handlers will be disassociated and all future calls to 
 *     CMGR_StartLeAdvertising() will fail until advertising is re-enabled.
 *
 * Parameters:
 *     enable - Determines whether to enable or disable advertising on the
 *         local device.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Advertising was enabled/disabled successfully.
 *
 *     BT_STATUS_PENDING - Operation to disable advertising was started
 *         successfully.  The CMEVENT_LE_ADVERT_DISABLED event will be indicated
 *         to the handler callback when advertising has been disabled.
 *
 *     BT_STATUS_FAILED - Failed to start the operation to disable active advertising.
 *
 *     BT_STATUS_BUSY - Advertising is currently processing an ongoing operation.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 */
BtStatus CMGR_EnableLeAdvertising(BOOL enable);

/*---------------------------------------------------------------------------
 * CMGR_SetLeAdvertParms()
 *
 *     Sets the global LE advertising parameters used in Connection Manager.
 *     If advertising is disabled, these values will be used the next time
 *     advertising is enabled.  However, if advertising is already enabled,
 *     these values will not take effect until advertising is disabled and
 *     then enabled again.
 *
 * Parameters:
 *     parms - Advertising parameters that apply all advertising operations.
 *         Any existing parameters will be overridden with these provided
 *         values.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Advertising parameters were set successful.
 *
 *     BT_STATUS_FAILED - The provided Advertising Parameters could not be 
 *         set.  This will happen if the current LE controller doesn't 
 *         support the advertising type selected (e.g. low-duty advertising).
 *
 *     BT_STATUS_INVALID_PARM - One (or more) advertising parameters are
 *         invalid.  Specifically, it should be noted that the advertising
 *         interval values shall not be less than CMGR_LE_ADVERT_INT_100MS
 *         if the advertising type is MLAT_DISCOVERABLE_UNDIRECTED or
 *         MLAT_NONCONNECTABLE_UNDIRECTED.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 *
 *     BT_STATUS_INVALID_TYPE - The operation failed because the request was for 
 *         directed advertising to an RPA where the remote device does not
 *         support Central Address Resolution.
  */
BtStatus CMGR_SetLeAdvertParms(CmgrLeAdvertParms *parms);

/*---------------------------------------------------------------------------
 * CMGR_GetLeAdvertParms()
 *
 *     Get the current LE advertising parameters that are setup for
 *     advertising operations in the Connection Manager.  If
 *     CMGR_SetLeAdvertParms has not been called, then the default
 *     advertising parameters will be returned.
 *
 * Parameters:
 *     parms - Pointer to a CmgrLeAdvertParms structure to receive the
 *         LE advertising parameters.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation was performed successfully.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the advertising
 *         parameters is an invalid pointer.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 */
BtStatus CMGR_GetLeAdvertParms(CmgrLeAdvertParms *parms);

/*---------------------------------------------------------------------------
 * CMGR_AddLeAdvertService()
 *
 *     Adds a LE advertising service to be used in the advertising data in
 *     Connection Manager.  Multiple services can be added but the service
 *     UUID memory must exist until the service is removed via
 *     CMGR_RemoveLeAdvertService.
 *
 * Parameters:
 *     uuid - Service UUID to be added into the Connection Manager
 *         advertising data.  This includes the ability to prioritize this
 *         UUID in the list of supported services.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Advertising service UUID was added successfully.
 *
 *     BT_STATUS_INVALID_PARM - The advertising service UUID is invalid.
 *
 *     BT_STATUS_IN_USE - This particular service UUID instance has already
 *         been added to the Connection Manager advertising service list.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 */
BtStatus CMGR_AddLeAdvertService(CmgrLeAdvertUuid *uuid);

/*---------------------------------------------------------------------------
 * CMGR_RemoveLeAdvertService()
 *
 *     Removes a LE advertising service from the advertising data used
 *     within Connection Manager.
 *
 * Parameters:
 *     uuid - Service UUID to be removed from the Connection Manager
 *         advertising data.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Advertising service UUID was removed successfully.
 *
 *     BT_STATUS_INVALID_PARM - Advertising service UUID is invalid.
 *
 *     BT_STATUS_NOT_FOUND - Service UUID does not exist in the
 *         advertising service list.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 */
BtStatus CMGR_RemoveLeAdvertService(CmgrLeAdvertUuid *uuid);

/*---------------------------------------------------------------------------
 * CMGR_AddLeAdvertTLV()
 *
 *     Adds a LE advertising TLV (type/length/value triplet) to be used in 
 *     the advertising data in Connection Manager.  Multiple TLV's can be 
 *     added but they must offer unique AD types.  This function currently 
 *     allows the following AD types to be added: 
 *
 *                CMGR_LE_ADTP_PUBLIC_ADDRESS (6-bytes),
 *                CMGR_LE_ADTP_RANDOM_ADDRESS (6-bytes), 
 *                CMGR_LE_ADTP_APPEARANCE (2-bytes), 
 *                CMGR_LE_ADTP_INTERVAL (2-bytes), 
 *                CMGR_LE_ADTP_LE_ADDRESS (6-bytes), 
 *                CMGR_LE_ADTP_LE_ROLE (1-byte),
 *                CMGR_LE_ADTP_URI (variable),
 *                and other custom profile specific AD types
 *
 * Parameters:
 *     tlv - advertising TLV to be added into the Connection Manager
 *         advertising data.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Advertising TLV was added successfully.
 *
 *     BT_STATUS_INVALID_PARM - The advertising TLV is invalid.
 *
 *     BT_STATUS_IN_USE - This particular advertising TLV instance has already
 *         been added to the Connection Manager advertising data types list.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 */
BtStatus CMGR_AddLeAdvertTLV(CmgrLeAdvertTLV *tlv);

/*---------------------------------------------------------------------------
 * CMGR_RemoveLeAdvertTLV()
 *
 *     Removes a LE advertising TLV (type/length/value triplet) from the 
 *     advertising data used within Connection Manager. 
 *
 * Parameters:
 *     tlv - advertising TLV to remove from the Connection Manager
 *         advertising data.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Advertising TLV was removed successfully.
 *
 *     BT_STATUS_INVALID_PARM - Advertising TLV is invalid.
 *
 *     BT_STATUS_NOT_FOUND - Advertising TLV does not exist in the
 *         advertising data types list.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 */
BtStatus CMGR_RemoveLeAdvertTLV(CmgrLeAdvertTLV *tlv);

/*---------------------------------------------------------------------------
 * CMGR_SetLeVendorAdvertData()
 *
 *     Sets the vendor specific data included in the LE advertising data
 *     used within Connection Manager.
 *
 * Parameters:
 *     vendor - Vendor specific data to include in the advertising data. This
 *         includes a two-byte value company identifier and any other vendor
 *         specific data bytes (up to a maximum of 27 additional bytes). Since
 *         the overall advertising data is limited to 31 bytes, it is
 *         recommended that this data is very small.  If former vendor
 *         specific data is intended to be cleared and not sent, then the
 *         no vendor data should be provided.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Advertising vendor data was set successfully.
 *
 *     BT_STATUS_INVALID_PARM - Advertising vendor data is invalid.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 */
BtStatus CMGR_SetLeVendorAdvertData(CmgrLeAdvertVendorData *vendor);

/*---------------------------------------------------------------------------
 * CMGR_SetLeAdvertServiceData()
 *
 *     Sets the service specific data included in the LE advertising data
 *     used within Connection Manager.
 *
 * Parameters:
 *     service - Service specific data to include in the advertising data. This
 *         includes a two, four, or sixteen byte service UUID value and any other 
 *         service specific data bytes (up to a maximum of 27 additional bytes). 
 *         Since the overall advertising data is limited to 31 bytes, it is
 *         recommended that this data is very small.  If former service
 *         specific data is intended to be cleared and not sent, then no
 *         service should be provided.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Advertising service data was set successfully.
 *
 *     BT_STATUS_INVALID_PARM - Advertising service data is invalid.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 */
BtStatus CMGR_SetLeAdvertServiceData(CmgrLeAdvertServiceData *service);

/*---------------------------------------------------------------------------
 * CMGR_UpdateLeAdvertData()
 *
 *     Allows the user to specifically update the LE controller with new
 *     advertising data information.  This will occur automatically when
 *     advertising is started, but for changes that occur while advertising
 *     is active, this function should be used.  This may occur after a
 *     local name change is indicated, services are added/removed, or
 *     vendor specific data is added.  Note: this function has the side
 *     effect of updating the scan response data as well.
 *
 * Parameters:
 *     handler - Pointer to the advertising handler structure used to update
 *         the advertising data. This handler must have been previously
 *         registered.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Advertising data has not changed, so it is
 *         already setup successfully.
 *
 *     BT_STATUS_PENDING - Advertising data is being updated and will
 *         be signaled with the CMEVENTEX_LE_ADVERT_DATA_UPDATE event upon
 *         completion.
 *
 *     BT_STATUS_BUSY - Advertising is currently processing an ongoing operation.
 *
 *     BT_STATUS_NOT_FOUND - Could not update the advertising data because
 *         the handler is not registered.
 *
 *     BT_STATUS_INVALID_PARM - Handler fields are not valid.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 */
BtStatus CMGR_UpdateLeAdvertData(CmgrLeAdvertHandler *handler);

/*---------------------------------------------------------------------------
 * CMGR_EnableLeOnlyAdvertMode()
 *
 *     Triggers the device to advertise support for LE-only mode.  In this
 *     mode the advertising data sent by the local device will indicate that
 *     BR/EDR support is disabled, regardless of the features supported on
 *     the LE controller.  When this mode is disabled the LE controller
 *     support will dictate the advertising support that is claimed.
 *
 *     This behavior is intended for situations where LE-only mode needs to
 *     be forced.
 *
 * Parameters:
 *     enable - Used to enable/disable a forced LE-only mode, as indicated
 *         in the advertising data.
 */
void CMGR_EnableLeOnlyAdvertMode(BOOL enable);

/*---------------------------------------------------------------------------
 * CMGR_SetAdvertAdMask()
 *
 *     Specify which AD types are present in advertising data and active scan
 *     response data when privacy is not enabled. This function can be called 
 *     any time. If the device is advertising it will not take effect until the 
 *     next time advertising is started.
 *
 * Parameters:
 *     mask - If a value is set it will be included in the advertising and 
 *         scanning response data.
 *
 * Returns:
 *     void
 */
void CMGR_SetAdvertAdMask(CmgrLeAdTypeMask mask);

/*---------------------------------------------------------------------------
 * CMGR_GetAdvertAdMask()
 *
 *     Get the current setting of the Ad type mask. 
 *
 * Parameters:
 *     none 
 *
 * Returns:
 *     current mask
 */
CmgrLeAdTypeMask CMGR_GetAdvertAdMask(void);

#if LE_PRIVACY == XA_ENABLED
/*---------------------------------------------------------------------------
 * CMGR_SetAdvertAdMaskForPrivacy()
 *
 *     Specify which AD types are present in advertising data and active scan
 *     response data while private. Some AD types such as device name should 
 *     not be present in the advertising or scan response data when the device 
 *     is private. This could defeat privacy.
 *
 *     This function can be called any time. If privacy is not enabled it will
 *     not take effect until privacy is enabled. If privacy is enabled and
 *     the device is advertising it will not take effect until the next time 
 *     advertising is started.
 *
 * Parameters:
 *     mask - If a value is set it will be included in the advertising and 
 *         scanning data.
 *
 * Returns:
 *     void
 */
void CMGR_SetAdvertAdMaskForPrivacy(CmgrLeAdTypeMask mask);

/*---------------------------------------------------------------------------
 * CMGR_GetAdvertAdMaskForPrivacy()
 *
 *     Get the current setting of the Ad type privacy mask. 
 *
 * Parameters:
 *     none 
 *
 * Returns:
 *     current mask
 */
CmgrLeAdTypeMask CMGR_GetAdvertAdMaskForPrivacy(void);

#endif /* LE_PRIVACY == XA_ENABLED */
#endif /* NUM_LE_DEVICES > 0 */

#endif /* __CONMGRADVERT_H_ */

