/****************************************************************************
 *
 * File:
 *     bsm.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description: Application Interface to Blue System Manager
 *
 * Copyright 2009-2017 OpenSynergy GmbH.
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
#ifndef __BSM_H
#define __BSM_H

#include "bttypes2.h"
#include "osapi.h"
#include "me.h"
#if defined(NUM_LE_DEVICES) && NUM_LE_DEVICES > 0
#include "lesec.h"
#include "mele.h"
#include "conmgrlea.h"
#include "conmgrled.h"
#include "devinfo_service.h"
#endif /* NUM_LE_DEVICES */

/*---------------------------------------------------------------------------
 * Bluetooth System Manager (BSM) layer
 *
 *     The BSM allows management applications and user-interface components
 *     to monitor and interact with a high-level view of available devices,
 *     services, and connections.
 */


/****************************************************************************
 *
 * Config
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BSM_INQ_TIMEOUT constant
 *
 *     Configures a timeout for device inquiry during the discovery process.
 *     If at least one new device has responded, but no new devices respond
 *     in BSM_INQ_TIMEOUT milliseconds, BSM cancels the inquiry process and
 *     begins its name-request/sdp-query procedures for all responding
 *     devices.
 *
 */
#ifndef BSM_INQ_TIMEOUT
#define BSM_INQ_TIMEOUT 4000
#endif

/*---------------------------------------------------------------------------
 * BSM_DISCOVERY_CONN_TIMEOUT constant
 *
 *     Configures a timeout for connections being established during
 *     the discovery process. If a discovery-based connection request does
 *     not complete in the given time, the connection attempt is
 *     cancelled.
 */
#ifndef BSM_DISCOVERY_CONN_TIMEOUT
#define BSM_DISCOVERY_CONN_TIMEOUT 5000
#endif

/*---------------------------------------------------------------------------
 * BSM_PERSISTENCE_FORMAT_VERSION constant
 *
 *     Indicates the current version of the persistence data. If this number
 *     changes, persistence databases from prior versions should be
 *     considered binary-incompatible.
 */
#define BSM_PERSISTENCE_FORMAT_VERSION 13

#define BSM_MAX_PASSCODE_LEN 16
#define BSM_MAX_OOB_DATA_LEN 16

#if NUM_LE_DEVICES > 0
/*---------------------------------------------------------------------------
 * BSM_ADD_GATT_SERVICE_TO_ADVERT constant
 *
 *     If enabled the GATT service UUID is added to the LE adverising packets.
 *     The BT Core Specification indicates that the GATT and GAP service
 *     UUID should not be included in the advertising packets so the default
 *     is XA_DISABLED.
 */
#ifndef BSM_ADD_GATT_SERVICE_TO_ADVERT
#define BSM_ADD_GATT_SERVICE_TO_ADVERT XA_DISABLED
#endif
#endif /* NUM_LE_DEVICES > 0 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BsmServiceClassUuid type
 *
 *     Corresponds to service classes which may be available.
 *     (See also: SdpServiceClassUuid.)
 */
typedef U16 BsmServiceClassUuid;

/* The following service class can be used by agents that need to register with
 * with BSM but don't want to expose a "real" UUID such as a client agent that
 * needs to support incoming connections so registers as a server. Note that
 * the value is currently not assigned by Bluetooth SIG but could be sometime
 * in the future in which case it may need to be changed to a new value.
 */
#define BSM_SERV_CLASS_UUID_UNASSIGNED 0xFFFC

/* End of BsmServiceClassUuid */

/*---------------------------------------------------------------------------
 * BsmDeviceClass type
 *
 *     Corresponds to device bits, describing the type of device
 *     (See also: BtClassOfDevice.)
 */
typedef U32 BsmDeviceClass;

/* End of BsmDeviceClass */

/*---------------------------------------------------------------------------
 * BsmServiceMode type
 *
 *     Provides additional information about a service instance.
 */
typedef U16 BsmServiceMode;

/* Can be used as a client. */
#define BSM_S_CLIENT        0x0001

/* Can be used as a server. */
#define BSM_S_SERVER        0x0002

/* A mask for bits that help identify the service type */
#define BSM_S_TYPE_MASK     (BSM_S_CLIENT | BSM_S_SERVER)

/* This service is actually available for connections. For a remote service this
 * implies that the service was successfully discovered during the last
 * SDP scan. For a local service, this implies that a local service
 * agent is present and listening for incoming connections.
 */
#define BSM_S_AVAILABLE             0x0004

/* The service is currently in-use (connected).
 */
#define BSM_S_CONNECTED             0x0008

/* The user has requested that this service be made available and so it is
 * kept in an enabled state whenever possible. This bit applies to local
 * services only.
 */
#define BSM_S_USER_REQUESTED        0x0010

/* This service is designated as "authorized". When set, connections to or from
 * this service will not cause BSM_URT_AUTHORIZE requests to occur. This bit applies
 * to remote services only.
 *
 * A remote service's authorize level can be modified using BSM_S_AUTHORIZED.
 */
#define BSM_S_AUTHORIZED               0x0020

/* End of BsmServiceMode */


/* Event bases */

/* Events delivered to BSM users */
#define BTE_TYPE_BASE_BSM          BTE_TYPE_BASE_APP

/* Events delivered by the BSM Agent Manager */
#define BTE_TYPE_BASE_BSM_AM       (BTE_TYPE_BASE_BSM + 80)

/* Events delivered by BSM Agents */
/*#define BTE_TYPE_BASE_BSM_SA       0x00100080 */

/* May be used to determine what kind of object is being handled */
#define BSME_LD_BASE        (BTE_TYPE_BASE_BSM + 0x00)
#define BSME_LD_BASE_LAST   (BTE_TYPE_BASE_BSM + 0x0F)
#define BSME_RD_BASE        (BTE_TYPE_BASE_BSM + 0x10)
#define BSME_RD_BASE_LAST   (BTE_TYPE_BASE_BSM + 0x1F)
#define BSME_LS_BASE        (BTE_TYPE_BASE_BSM + 0x20)
#define BSME_LS_BASE_LAST   (BTE_TYPE_BASE_BSM + 0x2F)
#define BSME_RS_BASE        (BTE_TYPE_BASE_BSM + 0x30)
#define BSME_RS_BASE_LAST   (BTE_TYPE_BASE_BSM + 0x3F)
#define BSME_UR_BASE        (BTE_TYPE_BASE_BSM + 0x40)
#define BSME_UR_BASE_LAST   (BTE_TYPE_BASE_BSM + 0x4F)
#define BSME_LA_BASE        (BTE_TYPE_BASE_BSM + 0x50)
#define BSME_LA_BASE_LAST   (BTE_TYPE_BASE_BSM + 0x5F)
#define BSME_RA_BASE        (BTE_TYPE_BASE_BSM + 0x60)
#define BSME_RA_BASE_LAST   (BTE_TYPE_BASE_BSM + 0x6F)

/*---------------------------------------------------------------------------
 * BsmEventType type
 *
 *     Describes the different kinds of events that can be delivered to the
 *     registered BsmSystem handler.
 */
typedef BtEventTypeEx BsmEventType;

/** The enable-state of the local device has changed. An event status of
 * BT_STATUS_SUCCESS means the local device is now available for use.
 * Other values indicate the local device is currently unavailable.
 */
#define BSME_LD_ENABLE_STATE            (BTE_TYPE_BASE_BSM + 0x00)

/** Indicates a change to the "mode" of the local device. The current
 * mode can be retrieved using BSM_LD_GetMode().
 */
#define BSME_LD_MODE_CHANGE             (BTE_TYPE_BASE_BSM + 0x01)

/* Private address of the local device has changed */
#define BSME_LD_PRIVATE_ADDR_CHANGE     (BTE_TYPE_BASE_BSM + 0x02)

/** A new remote device has been found. */
#define BSME_RD_FOUND                   (BTE_TYPE_BASE_BSM + 0x10)

/** A remote device name has been updated */
#define BSME_RD_NAME_CHANGE             (BTE_TYPE_BASE_BSM + 0x11)

/** A remote device's mode bits have changed. Use BSM_RD_GetMode() to
 * get the remote device's current mode bits.
 */
#define BSME_RD_MODE_CHANGE             (BTE_TYPE_BASE_BSM + 0x12)

/** A remote device has been identified as dual-mode and has had its 
 * unique identifier updated 
 */
#define BSME_RD_DM_IDENT_CHANGE         (BTE_TYPE_BASE_BSM + 0x13)

/* Directed RPA address used by the peer device */
#define BSME_RD_DIRECTED_ADDRESS        (BTE_TYPE_BASE_BSM + 0x14)

/** The enable-state of the specified local service has changed
 * (or failed to change). An event status of BT_STATUS_SUCCESS means
 * the service is currently enabled. Any other event status means
 * the service is not enabled. Generally, when a local service is
 * enabled, it is in a state to allow incoming connections.
 */
#define BSME_LS_ENABLE_STATE            (BTE_TYPE_BASE_BSM + 0x20)

/** The name of the specified local service has changed. It can be retrieved
 * using BSM_LS_GetFriendlyName() on the service.
 */
#define BSME_LS_NAME_CHANGE             (BTE_TYPE_BASE_BSM + 0x21)

/** This event indicates that all available local services and their 
 *  respective agents have been created.        
 */
#define BSME_LS_INIT_COMPLETED          (BTE_TYPE_BASE_BSM + 0x22)

/** A remote service instance was discovered.
 */
#define BSME_RS_FOUND                   (BTE_TYPE_BASE_BSM + 0x30)

/** Signals an update to a remote service's active modes. Use
 * BSM_RS_GetMode() to check the current mode bits.
 */
#define BSME_RS_MODE_CHANGE             (BTE_TYPE_BASE_BSM + 0x31)

/** Signals an update to the remote service's name. (This occurs when the
 * name generated for a service found in extended inquiry results is
 * replaced by the actual service name found in a remote device's SDP record.)
 */
#define BSME_RS_NAME_CHANGE             (BTE_TYPE_BASE_BSM + 0x32)

/** The remote service has changed which AMPs it is currently using (or
 * preparing to use) for data exchange. BSM_RS_GetNextRemoteAmp() may be
 * used to iterate through the remote AMPs in use by this service.
 */
#define BSME_RS_AMP_CHANGE              (BTE_TYPE_BASE_BSM + 0x33)

/** A remote service instance was removed.
 */
#define BSME_RS_REMOVED                 (BTE_TYPE_BASE_BSM + 0x34)

/** User intervention is requested according to the enclosed user request
 * object. BSM_UR_GetInfo should be called to get information about the
 * request. Once the user has responded, BSM_UR_Respond must be called
 * to complete the request.
 *
 * If the application does not understand or cannot display a particular
 * request, it should be rejected automatically using BSM_UR_Respond.
 */
#define BSME_UR_REQUESTED               (BTE_TYPE_BASE_BSM + 0x40)

/** The previously requested interaction is considered complete by BSM. The
 * BsmUserRequest handle is considered invalid as of delivery of this event.
 * Any UI indications should be closed or defocussed as a result of this
 * event.
 */
#define BSME_UR_COMPLETE                (BTE_TYPE_BASE_BSM + 0x41)

/** A BsmLocalAmp ("p.localAmp") is now available.
 *
 * Note that an available AMP is not necessarily enabled. Use
 * BSM_LA_GetInfo to test for this.
 */
#define BSME_LA_FOUND                   (BTE_TYPE_BASE_BSM + 0x50)

/** Something about the status of BsmLocalAmp has changed. Use
 * BSM_LA_GetInfo on "p.localAmp" to request the AMP's current
 * information and/or BSM_LA_GetNextRemoteAmp to review the
 * list of connected remote AMPs.
 */
#define BSME_LA_INFO_CHANGE             (BTE_TYPE_BASE_BSM + 0x51)

/** The availability state of a BsmLocalAmp is changing. If status is
 * BT_STATUS_SUCCESS, the "p.remoteAmp" pointer refers to a newly available
 * AMP. If "status" is not BT_STATUS_SUCCESS, the "p.remoteAmp" pointer
 * refers to an AMP that is no longer available, and the pointer should
 * no longer be used.
 *
 * Note that remote AMP's become unavailable whenever the associated
 * BsmRemoteDevice is disconnected.
 */
#define BSME_RA_FOUND                   (BTE_TYPE_BASE_BSM + 0x60)

/** Something about a BsmRemoteAmp has changed. Use BSM_RA_GetInfo on
 * "p.remoteAmp" to request the AMP's current information.
 */
#define BSME_RA_INFO_CHANGE             (BTE_TYPE_BASE_BSM + 0x61)

/* End of BsmEventType */

/*---------------------------------------------------------------------------
 * BsmLocalDeviceMode type
 *
 *     Bits that are OR'ed together to present a full picture of local
 *     device activity states. Retrieved using BSM_LD_GetMode().
 *
 *     Certain modes can be set using BSM_LD_SetMode().
 */
typedef U16 BsmLocalDeviceMode;

/** (read-only) One or more connections are being established. */
#define BSM_LD_CONNECTING           0x0001

/** (read-only) One or more connections are in progress. */
#define BSM_LD_CONNECTED            0x0002

/** (read-only) Actively discovering for remote devices. */
#define BSM_LD_DISCOVERING          0x0004

/** (read/write) Remote devices can connect to this device. */
#define BSM_LD_CONNECTABLE          0x0008

/** (read/write) Remote devices can discover this device. Note that only
 * one of BSM_LD_DISCOVERABLE and BSM_LD_LIMITED_DISCOVERABLE can be set.
 */
#define BSM_LD_DISCOVERABLE         0x0010

/** (read/write) Remote devices performing a "limited discovery" can discover
 * this device. Note that only one of BSM_LD_DISCOVERABLE and
 * BSM_LD_LIMITED_DISCOVERABLE can be set.
 */
#define BSM_LD_LIMITED_DISCOVERABLE 0x0020

/** (read-only) Actively advertising via LE.
 */
#define BSM_LD_LE_ADVERTISING       0x0040

/** (read/write) Remote devices can bond with this device
 */
#define BSM_LD_BONDABLE             0x0080

/** (read/write) Auto authorize all authorization requests
 */
#define BSM_LD_AUTO_AUTHORIZE       0x0100

/** (read/write) Remote devices can discover and connect to this device
 */
#define BSM_LD_GENERAL_ACCESSIBLE   (BSM_LD_CONNECTABLE | BSM_LD_DISCOVERABLE)

/** (read/write) Remote devices can limited-discover and connect to this device
 */
#define BSM_LD_LIMITED_ACCESSIBLE   (BSM_LD_CONNECTABLE | BSM_LD_LIMITED_DISCOVERABLE)

/** A mask covering all possible accessibility modes.
 */
#define BSM_LD_ACCESSIBLE_MASK      (BSM_LD_CONNECTABLE | BSM_LD_DISCOVERABLE | BSM_LD_LIMITED_DISCOVERABLE)

/* End of BsmLocalDeviceMode */

/*---------------------------------------------------------------------------
 * BsmRemoteDeviceMode type
 *
 *     Bits that are OR'ed together to describe the status of a remote device
 *     as perceived by the local device. Retrieve these bits using
 *     BSM_RD_GetMode.
 */
typedef U16 BsmRemoteDeviceMode;

/* This device is believed to be in-range, either because it was found during
 * the most recent inquiry, is currently connected, or was connected since
 * the most recent inquiry.
 */
#define BSM_RD_IN_RANGE             0x0001

/* An attempt is being made to connect to this device. */
#define BSM_RD_CONNECTING           0x0002

/* A connection exists with this device */
#define BSM_RD_CONNECTED            0x0004

/* An attempt is being made to pair with this device. */
#define BSM_RD_PAIRING              0x0008

/* A paired relationship exists with this device */
#define BSM_RD_PAIRED               0x0010

/* An attempt is being made to encrypt with this device. */
#define BSM_RD_ENCRYPTING           0x0020

/* An encryption relationship exists with this device */
#define BSM_RD_ENCRYPTED            0x0040

/* This device is completely authorized */
#define BSM_RD_AUTHORIZED           0x0080

/* This device is blocked */
#define BSM_RD_BLOCKED              0x0100

/* An attempt is being made to update the connection
 * parameters with this device.
 */
#define BSM_RD_UPDATE_PARMS         0x0200

/* This device is private */
#define BSM_RD_PRIVACY              0x0400

/* This device is pairing while already connected */
#define BSM_RD_PAIRING_CONNECTED    0x0800

/* The link keys have been stored for this device */
#define BSM_RD_BONDED               0x1000

/* End of BsmRemoteDeviceMode */

/*---------------------------------------------------------------------------
 * BsmUserRequestType type
 *
 *     Indicates the type of a user request. Requests should be formatted
 *     and delivered to the user for consideration. When the user indicates
 *     a response, BSM_UR_Respond must be called. The meaning of the
 *     additional arguments to BSM_UR_Respond is described with each type.
 */
typedef U8 BsmUserRequestType;

/* The remote device specified is being authenticated and requires an
 * alphanumeric passcode (formerly known as a PIN code).
 *
 * The "passCode" member of the associated BsmUserRequestInfo structure is
 * valid for this user request.
 *
 * When accepting this request, BSM_UR_Respond's "info" parameter must be
 * valid and all info.passCode fields must be filled in.
 */
#define BSM_URT_PASSCODE          1

/* A remote device with a display is being authenticated. The supplied number
 * (the "Passkey") must be displayed and confirmed on both the local device
 * and the remote device.
 *
 * For bonding to complete, the user of the local device must indicate
 * whether the displayed number matches on both sides. The user's response
 * must be provided using BSM_UR_Respond's "accept" parameter. If the local
 * user accepts the number, the local device should continue to display the
 * request until a corresponding BSME_UR_COMPLETE event arrives.
 *
 * If the remote device has an input mechanism, bonding will not complete
 * until its user has also confirmed or rejected the number. If the remote
 * device does not have an input mechanism, it should display the number
 * for the duration of the bonding operation.
 *
 * The "number" member of the associated BsmUserRequestInfo structure is
 * valid for this user request. When responding to this request,
 * BSM_UR_Respond's "data" parameter is ignored.
 */
#define BSM_URT_CONFIRM_NUMBER    2

/* A remote device with a keyboard, but no display, is being authenticated.
 * The local device's user must be asked to enter the provided number
 * (the "Passkey") on the remote device to complete the process.
 *
 * It is not necessary to respond to this request using BSM_UR_Respond.
 * However, the local user may decide to cancel the request, in which case
 * the remote device is forcibly disconnected.
 *
 * The local device should continue to display the request until a
 * corresponding BSME_UR_COMPLETE event indicates the request is complete.
 */
#define BSM_URT_ENTER_NUMBER_REMOTELY    3

/* A service connection with a remote device is being established, and
 * requires user approval.
 *
 * The "authorize" member of the associated BsmUserRequestInfo structure is
 * valid for this user request.
 *
 * The user must be queried to determine whether this service is allowed
 * access. If desired, the BSM application may also offer the user options
 * for permanently authorizing the service (BSM_RS_Authorize) or device
 * (BSM_RD_Authorize).
 */
#define BSM_URT_AUTHORIZE                    4

/* A bonding attempt has been reported and requires user approval.
 *
 * BSM_UR_Respond must be called to tell BSM whether the user accepts
 * the bonding attempt.
 */
#define BSM_URT_BOND                     5

/* A bonded device has reported that it lost its key. It is possible that
 * a remote attacker is attempting to "hijack" an existing bonded
 * relationship. Therefore, the user must explicitly approve of the
 * re-establishment of the bond relationship.
 *
 * BSM_UR_Respond must be called to tell BSM whether the user accepts
 * the re-bonding attempt.
 */
#define BSM_URT_REBOND                   6

/* A LE L2CAP Connection Oriented channel or a GATT/ATT attribute has been accessed 
 * that requires authorization by the user.
 *
 * The "authorize" member of the associated BsmUserRequestInfo structure is
 * valid for this user request.
 *
 * The user must be queried to determine whether access is allowed.
 * If desired, the BSM application may also offer the user options
 * for permanently authorizing the device (BSM_RD_Authorize).
 */
#define BSM_URT_LESEC_AUTHORIZE          7

/* A remote device with IO Capabilities of NO IO is attempt to pair/bond. We want
 * MITM but the resulting key will be a Just work keys so we need to ask the user
 * if this is acceptable
 *
 * BSM_UR_Respond must be called to tell BSM whether the user accepts
 * the request.
 *
 */
#define BSM_URT_REM_NO_IO                8

 /* End of BsmUserRequestType */

/*---------------------------------------------------------------------------
 * BsmLinkBonding type
 *
 *     Describes if bonding is enabled by a service.
 */
typedef U8 BsmLinkBonding;

/** Represents the system default bond settings (i.e., whatever was set
 * by BSM_SetDefaultSecurity).
 */
#define BSM_LINK_BOND_DEFAULT       0

/** Bonding is enabled.
 * 
 * The link keys will be stored on both devices for use in future pairing
 */
#define BSM_LINK_BOND_ENABLED       1

/** Bonding is disabled.
 * 
 * The link keys will NOT be stored 
 */
#define BSM_LINK_BOND_DISABLED      2

/* End of BsmLinkBonding type*/

/*---------------------------------------------------------------------------
 * BsmLinkAuth type
 *
 *     Describes if link authentication is required by a service.
 */
typedef U8 BsmLinkAuth;

/** Represents the system default authentication settings (i.e., whatever was
 * set by BSM_SetDefaultSecurity).
 */
#define BSM_LINK_AUTH_DEFAULT           0

/** Link authentication is required.
 * 
 * User interaction is required to authenticate the link key. This setting
 * protects against "man-in-the-middle" attacks.
 */
#define BSM_LINK_AUTH_REQUIRED          1

/** Link authentication is not required.
 * 
 * User interaction is not required when exchanging link keys. This setting
 * leaves the pairing process open to "man-in-the-middle" attacks.
 */
#define BSM_LINK_AUTH_NOT_REQUIRED      2

/* End of BsmLinkAuthenticate type*/

/*---------------------------------------------------------------------------
 * BsmAuthorizeRequirement type
 *
 *     Describes the level of authorization required by the device
 *     or service.
 */
typedef U8 BsmAuthorizeRequirement;

/** Represents the system default authorize level (i.e., whatever
 *  was set by BSM_SetDefaultSecurity).
 */
#define BSM_AUTHORIZE_DEFAULT           0

/** User authorization is not required to establish a connection 
 *  to this device or to access this service 
 */
#define BSM_AUTHORIZE_NOT_REQUIRED      1

/** User authorization is required to establish a connection to
 *  this device or to access this service 
 */
#define BSM_AUTHORIZE_REQUIRED          2

/* End of BsmAuthorizeRequirement */

/*---------------------------------------------------------------------------
 * BsmAutoAuthorize type
 *
 *     Used to automatically respond with acceptance to any authorization
 *     requests (BTEVENT_AUTHORIZATION_REQ). This setting has not effect
 *     if neither the local device nor any of the local services require
 *     user authorization.
 *      
 *     This is a BSM only setting and has no corresponding setting in the
 *     stack. However, when auto accept is on and the device is bonded, the
 *     remote device security record will be updated to grant access for
 *     all future connections.
 */
typedef U8 BsmAutoAuthorize;

/** Auto authorize requests will not be automatically accepted and user will
 *  be prompted prior to granting access
 */
#define BSM_AUTO_AUTHORIZE_OFF      0

/** Auto authorize requests will be automatically accepted and no user
 *  request will be generated
 */
#define BSM_AUTO_AUTHORIZE_ON       1

/* End of BsmAutoAuthorize */

/*---------------------------------------------------------------------------
 * BsmAmpMode type
 *
 *     Indicates the current AMP mode for a local or remote AMP.
 */
typedef U8 BsmAmpMode;

/** The AMP is currently enabled. If not present, the AMP is powered down */
#define BSM_AM_ENABLED   0x01

/** A link is active for this AMP */
#define BSM_AM_CONNECTED 0x02

/* End of BsmAmpMode */

/*---------------------------------------------------------------------------
 * BsmDiscoverFlag type
 *
 *     Indicates the the type of discovery to perform.
 */
typedef U8 BsmDiscoverFlag;

/** Perform device inquiry using BR/EDR */
#define BSM_DF_BR   0x01

/** Perform device scan using Low Energy */
#define BSM_DF_LE   0x02

/** Perform service discovery using SDP */
#define BSM_DF_SDP  0x04

/** Perform service discovery using GATT over BR/EDR */
#define BSM_DF_GATT 0x08

/** Perform device name discovery */
#define BSM_DF_NAME 0x10

/** Perform Device ID discovery */
#define BSM_DF_DEVID 0x20

/** Perform service discovery using GATT over LE */
#define BSM_DF_LEGATT 0x40

/** Perform device name discovery for LE */
#define BSM_DF_LENAME 0x80

#if NUM_LE_DEVICES > 0
#define BSM_DF_MASK       (BSM_DF_BR | BSM_DF_LE | BSM_DF_SDP | BSM_DF_GATT | BSM_DF_NAME |BSM_DF_DEVID | BSM_DF_LEGATT | BSM_DF_LENAME )
#define BSM_DF_BREDR_MASK (BSM_DF_SDP | BSM_DF_GATT | BSM_DF_NAME |BSM_DF_DEVID)
#define BSM_DF_LE_MASK    (BSM_DF_LENAME | BSM_DF_LEGATT)
#else
#define BSM_DF_MASK       (BSM_DF_BR | BSM_DF_SDP | BSM_DF_NAME | BSM_DF_DEVID)
#define BSM_DF_BREDR_MASK (BSM_DF_SDP | BSM_DF_NAME |BSM_DF_DEVID)
#define BSM_DF_LE_MASK    0
#endif /* NUM_LE_DEVICES > 0 */

/* End of BsmDiscoverFlag */

/*---------------------------------------------------------------------------
 * BsmRemoteDevice type
 *
 *     A handle to a known remote Bluetooth device.
 */
typedef struct _BsmRemoteDevice BsmRemoteDevice;
/* End of BsmRemoteDevice type */

/*---------------------------------------------------------------------------
 * BsmLocalDevice type
 *
 *     Represents the local Bluetooth device.
 */
typedef struct _BsmLocalDevice BsmLocalDevice;
/* End of BsmLocalDevice type */

/*---------------------------------------------------------------------------
 * BsmRemoteService type
 *
 *     A handle to a service instance available on, or connected from, a
 *     remote Bluetooth device.
 */
typedef struct _BsmRemoteService BsmRemoteService;
/* End of BsmRemoteService type */

/*---------------------------------------------------------------------------
 * BsmLocalService type
 *
 *     A handle to a service instance accessible through the local Bluetooth
 *     device.
 */
typedef struct _BsmLocalService BsmLocalService;
/* End of BsmLocalService type */

/*---------------------------------------------------------------------------
 * BsmUserRequest type
 *
 *     A handle to an outstanding user request.
 */
typedef struct _BsmUserRequest BsmUserRequest;
/* End of BsmUserRequest type */

/*---------------------------------------------------------------------------
 * BsmLocalAmp type
 *
 *     A handle to a local Alternate MAC/PHY (AMP) controller.
 */
typedef struct _BsmLocalAmp BsmLocalAmp;
/* End of BsmLocalAmp type */

/*---------------------------------------------------------------------------
 * BsmRemoteAmp type
 *
 *     A handle to a remote Alternate MAC/PHY (AMP) controller.
 */
typedef struct _BsmRemoteAmp BsmRemoteAmp;
/* End of BsmRemoteAmp type */

/*---------------------------------------------------------------------------
 * BsmUserRequestInfo struct
 *
 *     Provides information about an outstanding user request.
 */
typedef struct _BsmUserRequestInfo {
    /* Type of the request */
    BsmUserRequestType  type;

    union {
        /* BSM_URT_PASSCODE fields */
        struct {
            BsmRemoteDevice *remoteDevice;
            U16 minLen;
            U16 maxLen;     /* 16 for pin code, 6 for passkey */
        } passCode;

        /* BSM_URT_CONFIRM_NUMBER and BSM_URT_ENTER_NUMBER_REMOTELY fields */
        struct {
            BsmRemoteDevice *remoteDevice;

            /* An up to six-digit number (from 0 to 999999) to display or
             * request remote entry
             */
            U32 passkey;
        } number;

        /* BSM_URT_AUTHORIZE fields */
        struct {
            /* The remote device requesting authorized access. */
            BsmRemoteDevice *remoteDevice;

            /* The remote service requesting authorized access, or 0 if not known */
            BsmRemoteService *remoteService;
        } authorize;

        /* BSM_URT_REBOND fields */
        struct {
            /* The remote device requesting re-bond. */
            BsmRemoteDevice *remoteDevice;
        } rebond;

        /* BSM_URT_BOND fields */
        struct {
            /* The remote device requesting bond. */
            BsmRemoteDevice *remoteDevice;
        } bond;

        /* BSM_URT_REM_NO_IO fields */
        struct {
            /* The remote device requesting bond. */
            BsmRemoteDevice *remoteDevice;
        } noIo;
    } u;
} BsmUserRequestInfo;

/*---------------------------------------------------------------------------
 * BsmUserResponseInfo struct
 *
 *     Provides additional information needed to answer a user request.
 */
typedef struct _BsmUserResponseInfo {
    union {
        /* Supplies a successful response to a BSM_URT_PASSCODE request */
        struct {
            /* User-supplied alphanumeric pass code (Also known as PIN code) */
            U8 code[BSM_MAX_PASSCODE_LEN];

            /* Length of pass code provided in "code" */
            U8 codeLen;
        } passCode;
    } u;
} BsmUserResponseInfo;

/*---------------------------------------------------------------------------
 * BsmSecuritySettings struct
 *
 *     Describes the security settings required by the local device or a 
 *     local service.
 */
typedef struct _BsmSecuritySettings {

    /* Bond enable setting */
    BsmLinkBonding bond;

    /* Authenticate requirement */
    BsmLinkAuth authenticate;

    /* Authorization requirement */
    BsmAuthorizeRequirement authorize;

    /* Automatic authorization setting */
    BsmAutoAuthorize autoAuthorize;

} BsmSecuritySettings;

#if NUM_LE_DEVICES > 0
/*---------------------------------------------------------------------------
 * BsmLeKeyDistribution struct
 *
 *     Describes the LE key distribution settings required for security.
 */
typedef struct _BsmLeKeyDistribution {

    /* Local Key Distribution */
    U8      localKeys;

    /* Remote Key Distribution */
    U8      remoteKeys;

} BsmLeKeyDistribution;
#endif /* NUM_LE_DEVICES > 0 */

/*---------------------------------------------------------------------------
 * BsmpAmpInfo struct
 *
 *     Provides information about an AMP's status and capabilities.
 */
typedef struct _BsmAmpInfo {

    /* The type of this AMP. */
    BtControllerType type;

    /* In KBit/sec, expresses an estimate of current bandwidth available
     * for transmitted and received data. This is an "upper" estimate, the
     * effective throughput rate will generally be less than this.
     *
     * For local AMPs, this value may fluctuate over time, especially if
     * the AMP has other non-Bluetooth users.
     *
     * For a local AMP that is disabled or has no remaining bandwidth,
     * this value is 0.
     */
    U32 bandwidth;

    /* Current mode bits for this AMP. */
    BsmAmpMode mode;

    /* Remote device where this remote AMP was found. For local AMPs
     * this field will always be null.
     */
    BsmRemoteDevice *rdev;

    /* Local AMP to which this remote AMP is currently connected, if any.
     * For local AMPs this field will always be null.
     */
    BsmLocalAmp *localAmp;

} BsmAmpInfo;


/*---------------------------------------------------------------------------
 * BsmAmpPair structure
 *
 *     Identifies both a local and remote AMP. Normally used to describe
 *     an connection between the two AMPs.
 */
typedef struct _BsmAmpPair {
    BsmLocalAmp *localAmp;
    BsmRemoteAmp *remoteAmp;
} BsmAmpPair;

/*---------------------------------------------------------------------------
 * BsmDeviceId structure
 *
 *     The device id information to be registered in the DevId SDP entry and
 *     to be provided in the EIR info.
 */
typedef struct _BsmDeviceId {
    U16 vendorIdSrc;
    U16 vendorId;
    U16 productId;
    U16 version;
    BOOL primaryRecord;
} BsmDeviceId;


/*---------------------------------------------------------------------------
 * BsmEvent structure
 *
 *     Describes an event as delivered to the global BsmSystem handler.
 */
typedef struct _BsmEvent {
    BtEventEx e;

    union {
        /* Set for BSME_LS_* events */
        BsmLocalService *localService;

        /* Set for BSME_RS_* events */
        BsmRemoteService *remoteService;

        /* Valid for all BSME_LD_* events */
        BsmLocalDevice *localDevice;

        /* Valid for all BSME_RD_* events */
        BsmRemoteDevice *remoteDevice;

        /* Valid for all BSM_UR_* events */
        BsmUserRequest *userRequest;

        /* Valid for all BSM_LA_* events */
        BsmLocalAmp *localAmp;

        /* Valid for all BSM_RA_* events */
        BsmRemoteAmp *remoteAmp;
    } p;
} BsmEvent;


/* Other forward refs */
typedef struct _BsmAgentManager BsmAgentManager;
typedef struct _BsmPersistenceManager BsmPersistenceManager;

/****************************************************************************
 *
 * Section: General APIs
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BSM_Init()
 *
 *     Initializes BSM internal memory.
 */
BOOL BSM_Init(void);

/*---------------------------------------------------------------------------
 * BSM_Deinit()
 *
 *     Deinitializes BSM internal memory, freeing all internal allocations.
 */
void BSM_Deinit(void);

/*---------------------------------------------------------------------------
 * BSM_Start()
 *
 *     Starts up the BSM system with a user-supplied BSM Agent Manager and
 *     Persistence Manager. Must be called only once, before any other BSM
 *     APIs.
 *
 *     Note that during execution of BSM_Start(), a large number of events
 *     may be delivered to the handler. These events inform the handler of
 *     previously encountered devices, services, and so forth as the persistence
 *     database is scanned.
 *
 * Parameters:
 *     handler - Event handler for all BSM events.
 *     agentMgr - Agent manager for BSM to use.
 *     persMgr - Persistence manager for BSM to use.
 */
void BSM_Start(BtHandlerEx *handler, BsmAgentManager *agentMgr,
               BsmPersistenceManager *persMgr);

/*---------------------------------------------------------------------------
 * BSM_FinishStart()
 *
 *     Starts up the BSM system with a user-supplied BSM Agent Manager and
 *     Persistence Manager. Must be called only once, before any other BSM
 *     APIs.
 *
 *     Note that during execution of BSM_Start(), a large number of events
 *     may be delivered to the handler. These events inform the handler of
 *     previously encountered devices, services, and so forth as the persistence
 *     database is scanned.
 *
 * Parameters:
 *     handler - Event handler for all BSM events.
 *     agentMgr - Agent manager for BSM to use.
 *     persMgr - Persistence manager for BSM to use.
 */
void BSM_FinishStart(void);

/*---------------------------------------------------------------------------
 * BSM_FinishStart2()
 *
 *     Starts up the BSM system with a user-supplied BSM Agent Manager and
 *     Persistence Manager. Must be called only once, before any other BSM
 *     APIs.
 *
 *     Note that during execution of BSM_Start(), a large number of events
 *     may be delivered to the handler. These events inform the handler of
 *     previously encountered devices, services, and so forth as the persistence
 *     database is scanned.
 *
 * Parameters:
 *     handler - Event handler for all BSM events.
 *     agentMgr - Agent manager for BSM to use.
 *     persMgr - Persistence manager for BSM to use.
 */
void BSM_FinishStart2(void);

/*---------------------------------------------------------------------------
 * BSM_Lock()
 *
 *     Enters a lock state in BSM. When this method returns, the BSM is
 *     prevented from changing its internal state. Must be followed by a
 *     call to BSM_Unlock() to allow BSM to continue processing.
 */
void BSM_Lock(void);

/*---------------------------------------------------------------------------
 * BSM_Unlock()
 *
 *     Leaves the locked BSM state. See BSM_Lock().
 *
 */
void BSM_Unlock(void);

/*---------------------------------------------------------------------------
 * BSM_GetLocalDevice()
 *
 *     Returns a pointer to the local device being used by BSM. At this time,
 *     only one local device is available at any one time.
 *
 * Returns:
 *     BsmLocalDevice pointer
 */
BsmLocalDevice *BSM_GetLocalDevice(void);

#if NUM_LE_DEVICES > 0
/*---------------------------------------------------------------------------
 * BSM_SetLeKeyDistribution()
 *
 *     Sets the key distribution for LE security operations.  These values
 *     are negotiated with the remote device to determine which keys are
 *     actually transmitted and received.
 *
 * Parameters:
 *     localKeys - local keys allowed for distribution.
 *
 *     remoteKeys - remote keys allows for distribution.
 *
 * Returns:
 *     TRUE if settings were applied.
 *     FALSE if the "keys" parameter was invalid.
 */
BOOL BSM_SetLeKeyDistribution(U8 localKeys, U8 remoteKeys);

/*---------------------------------------------------------------------------
 * BSM_SetLeEncryptKeySize()
 *
 *     Sets the encryption key size for LE security operations.  This value
 *     is negotiated with the remote device, but cannot exceed the size
 *     presented to this function.  Sizes ranging from 7-16 bytes are the
 *     only values that will be accepted.
 *
 * Parameters:
 *     keySize - IO Capability to setup for BR/EDR and LE security operations.
 *
 * Returns:
 *     TRUE if settings were applied.
 *     FALSE if the "keySize" parameter was invalid.
 */
BOOL BSM_SetLeEncryptKeySize(U8 keySize);

/*---------------------------------------------------------------------------
 * BSM_SetLeAdvertisingType()
 *
 *     Sets the advertising type for local LE advertising behavior.
 *
 * Parameters:
 *     type - Advertising type to setup for LE advertising behavior.
 *
 * Returns:
 *     TRUE if settings were applied.
 *     FALSE if the "type" parameter was invalid.
 */
BOOL BSM_SetLeAdvertisingType(MeLeAdvertType type);

/*---------------------------------------------------------------------------
 * BSM_SetLeDiscoveryMode()
 *
 *     Sets the discovery mode for local LE advertising behavior.
 *
 * Parameters:
 *     mode - Discovery mode to setup for LE advertising behavior.
 *
 * Returns:
 *     TRUE if settings were applied.
 *     FALSE if the "mode" parameter was invalid.
 */
BOOL BSM_SetLeDiscoveryMode(CmgrLeAdvertDiscoveryMode mode);

/*---------------------------------------------------------------------------
 * BSM_SetLeOobData()
 *
 *     Sets the OOB data for the local device.  This only works for Legacy
 *     LE pairing.  
 *
 * Parameters:
 *     rDev - Pointer to remote device
 *
 *     data - Pointer to OOB data or 0 to clear the data
 *
 * Returns:
 *     TRUE if OOB was set successfully
 *     FALSE if the operation failed.
 */
BOOL BSM_SetLeOobData(BsmRemoteDevice *rDev, U8 *data);

#endif /* NUM_LE_DEVICES > 0 */

/*---------------------------------------------------------------------------
 * BSM_SetBrIoCapability()
 *
 *     Sets the IO Capability for the local device.  This is advertised when
 *     performing BR/EDR security operations.
 *
 * Parameters:
 *     brCap - IO Capability to setup for BR/EDR security operations.
 *
 * Returns:
 *     TRUE if settings were applied.
 *     FALSE if the "capability" parameter was invalid.
 */
BOOL BSM_SetBrIoCapability(U8 brCap);

/*---------------------------------------------------------------------------
 * BSM_EnablePairingBonding()
 *
 *     Allows enabling or disabling of the pairing/bonding mode.  If
 *     pairing/bonding is disabled, all attempts will be rejected until this
 *     call is invoked to enable it again.
 *
 * Parameters:
 *     enable - Enable or disable bonding mode.
 */
void BSM_EnablePairingBonding(BOOL enable);

/*---------------------------------------------------------------------------
 * BSM_SetDefaultSecurity()
 *
 *     Sets security settings that apply to all services (unless those
 *     services explicitly indicate a non-default level parameter). New
 *     security settings will only affect future connection attempts.  It
 *     is also possible to disable the bonding mode.
 *
 * Parameters:
 *     settings - Default settings. Each parameter of settings must be filled in
 *         with a non-default setting.
 *
 * Returns:
 *     TRUE if settings were applied.
 *     FALSE if the setting parameter was invalid.
 */
BOOL BSM_SetDefaultSecurity(const BsmSecuritySettings *settings);

/*---------------------------------------------------------------------------
 * BSM_GetDefaultSecurity()
 *
 *     Returns the current default security settings.
 *
 * Parameters:
 *     settings - Structure to be filled with current security settings.
 */
void BSM_GetDefaultSecurity(BsmSecuritySettings *settings);

/*---------------------------------------------------------------------------
 * BSM_GetBrIoCapability()
 *
 *     Returns the current IO capability security setting for BR/EDR.
 *
 * Parameters:
 *     brCap - Pointer to a BtIoCapabilities value to be filled with the
 *         current BR/EDR IO capability security setting.
 *
 *     leCap - Pointer to a BtIoCapabilities value to be filled with the
 *         current LE IO capability security setting.
 *
 */
void BSM_GetBrIoCapability(BtIoCapabilities *brCap);

#if NUM_LE_DEVICES > 0
/*---------------------------------------------------------------------------
 * BSM_GetLeEncryptKeySize()
 *
 *     Returns the current LE encryption key size setting.
 *
 * Parameters:
 *     keySize - Pointer to the current LE encryption key size value.
 */
void BSM_GetLeEncryptKeySize(U8 *keySize);

/*---------------------------------------------------------------------------
 * BSM_GetLeKeyDistribution()
 *
 *     Returns the current LE key distribution settings.
 *
 * Parameters:
 *     keys - Structure to be filled with the LE key distributon values for
 *         both the local and remote devices.
 */
void BSM_GetLeKeyDistribution(BsmLeKeyDistribution *keys);

/*---------------------------------------------------------------------------
 * BSM_GetLeAdvertisingType()
 *
 *     Returns the current LE advertising type settings.
 *
 * Parameters:
 *     type - Pointer to be the current LE advertising type value.
 */
void BSM_GetLeAdvertisingType(MeLeAdvertType *type);

/*---------------------------------------------------------------------------
 * BSM_GetLeDiscoveryMode()
 *
 *     Returns the current LE discovery mode settings.
 *
 * Parameters:
 *     type - Pointer to be the current LE discovery mode value.
 */
void BSM_GetLeDiscoveryMode(CmgrLeAdvertDiscoveryMode *mode);

#endif /* NUM_LE_DEVICES > 0 */

/*---------------------------------------------------------------------------
 * BSM_GetNextLocalService()
 *
 *     Returns the next local service currently being tracked or 0 if no
 *     more are available.
 *
 * Parameters:
 *     previous - The previously found local service, or 0 to get the first.
 *
 * Returns:
 *     The next BsmLocalService entity known to BSM, or 0 if there are no
 *     more.
 */
BsmLocalService *BSM_GetNextLocalService(BsmLocalService *previous);

/*---------------------------------------------------------------------------
 * BSM_GetNextRemoteDevice()
 *
 *     Returns the next remote device currently being tracked or 0 if no
 *     more are available.
 *
 * Parameters:
 *     previous - The previously found remote device, or 0 to get the first.
 *
 * Returns:
 *     The next BsmRemoteDevice entity known to BSM, or 0 if there are no
 *     more.
 */
BsmRemoteDevice *BSM_GetNextRemoteDevice(BsmRemoteDevice *previous);

/*---------------------------------------------------------------------------
 * BSM_GetNextLocalAmp()
 *
 *     Returns the next locally available AMP or 0 if there are no more.
 *
 * Parameters:
 *     previous - 0 to get the first local amp, or the most recently
 *          returned AMP to get the next one.
 *
 * Returns:
 *     Next local AMP, or 0 if no more
 */
BsmLocalAmp *BSM_GetNextLocalAmp(BsmLocalAmp *previous);


/*---------------------------------------------------------------------------
 * BSM_RegisterDeviceId()
 *
 *     Registers the Device ID record.
 *
 * Parameters:
 *     devId - The device id information for the device (e.g. VID, PID, etc.).
 */
BOOL BSM_RegisterDeviceId(BsmDeviceId *devId);

/*---------------------------------------------------------------------------
 * BSM_UnRegisterDeviceId()
 *
 *     Unregisters the Device ID record.
 *
 * Parameters:
 */
void BSM_UnRegisterDeviceId(void);

/*---------------------------------------------------------------------------
 * BSM_GetOrCreateRemoteService()
 *
 *     Returns a remote service associated with the current remote
 *     device and class. The service will be created if it does not
 *     yet exist.
 *
 * Parameters:
 *     rdev - the remote device.
 *
 *     lservice - A local service.
 *
 * Returns:
 *     BsmRemoteService associated with the local service and remote
 *     device or 0 if not found. The service will be persisted
 *     if successful.
 */
BsmRemoteService *BSM_GetOrCreateRemoteService(BsmRemoteDevice *rdev,
                                               BsmLocalService *lservice);


/****************************************************************************
 *
 * Section: Local Device APIs
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * BSM_LD_GetDeviceAddress()
 *
 *     Retrieves the device address corresponding to this device.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *     addr - (Output) Destination for address.
 *
 */
void BSM_LD_GetDeviceAddress(BsmLocalDevice *ldev, BtDevAddr *addr);

/*---------------------------------------------------------------------------
 * BSM_LD_GetPrivateAddress()
 *
 *     Return the private address currently being used. If privacy is
 *     is disabled then the 'type' field of the address we be set to
 *     BTADDR_INVALID. 
 *
 * Parameters:
 *     rpa - pointer to stucture to receive the resolvable private address
 *
 * Returns:
 *     void
 */
void BSM_LD_GetPrivateAddress(BtDevAddr *rpa);

#if LL_PRIVACY == XA_ENABLED
/*---------------------------------------------------------------------------
 * BSM_LD_GetLocalRpa()
 *
 *     Return the local resovlable private address (RPA) currently being 
 *     used by the LE controller. If Link Layer Privacy is disabled then 
 *     the 'type' field of the address we be set to BTADDR_INVALID. 
 *
 * Parameters:
 *     localRpa - pointer to stucture to receive the resolvable private address
 *
 * Returns:
 *     void
 */
void BSM_LD_GetLocalRpa(BtDevAddr *localRpa);
#endif /* LL_PRIVACY == XA_ENABLED */

/*---------------------------------------------------------------------------
 * BSM_LD_GetEnabled()
 *
 *     Indicates whether this local device is enabled (i.e. active and ready
 *     to receive commands.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *
 * Returns:
 *     TRUE if the device is ready, FALSE if not.
 */
BOOL BSM_LD_GetEnabled(BsmLocalDevice *ldev);

/*---------------------------------------------------------------------------
 * BSM_LD_GetMode()
 *
 *     Returns the local device's current mode bits
 *
 * Parameters:
 *     ldev - The local device of interest.
 *
 * Returns:
 *     Mode bits describing the state of the local device.
 */
BsmLocalDeviceMode BSM_LD_GetMode(BsmLocalDevice *ldev);

/*---------------------------------------------------------------------------
 * BSM_LD_SetIac()
 *
 *     Sets the local device inquiry access code (General/Limited);
 *
 * Parameters:
 *     iac - Specifies the BtIac code to indicate which devices should respond to
 *           inquiries.
 *
 * Returns:
 *     void
 */
void BSM_LD_SetIac(BtIac iac);

/*---------------------------------------------------------------------------
 * BSM_LD_SetGattUseSdpForBrEdr()
 *
 *     Set the flag specifying if GATT should use SDP or GATT for service
 *     search when running over BR/EDR
 *
 * Parameters:
 *     val - TRUE means use SDP FALSE means do not use SDP.
 *
 * Returns:
 *     void
 */
void BSM_LD_SetGattUseSdpForBrEdr(BOOL val);

/*---------------------------------------------------------------------------
 * BSM_LD_SetFriendlyName()
 *
 *     Sets the name to be advertised by the local Bluetooth device.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *     name - NULL-terminated, UTF-8 name to be copied.
 */
void BSM_LD_SetFriendlyName(BsmLocalDevice *ldev, const U8 *name);

/*---------------------------------------------------------------------------
 * BSM_LD_GetFriendlyName()
 *
 *     Returns the friendly name advertised by the local Bluetooth device.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *     name - A buffer containing at least "len" bytes of storage. If the
 *          length is not long enough it is truncated and null-terminated.
 *     len - The size of the "name" buffer.
 *
 * Returns:
 *     The full length of the name including null-termination.
 */
U16 BSM_LD_GetFriendlyName(BsmLocalDevice *ldev, U8 *name, U16 len);

/*---------------------------------------------------------------------------
 * BSM_LD_Discover()
 *
 *     Asks the local device to discover information about devices in range.
 *     Any device found to be in range is scanned for updates to its
 *     friendly name and available services.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *     rdev - The remote device to discover or 0 to look for all devices in
 *         range.
 *
 * Returns:
 *     TRUE if the local device was successfully transitioned to discovery
 *     mode. BSM_LD_GetMode will return BSM_LD_DISCOVERING until discovery
 *     is complete.
 *
 *     FALSE if it was not possible to begin the discovery process.
 */
BOOL BSM_LD_Discover(BsmLocalDevice *ldev, BsmRemoteDevice *rdev);

/*---------------------------------------------------------------------------
 * BSM_LD_SetDiscoverMode()
 *
 *     Sets the default flags used by BSM_LD_Discover.
 *
 * Parameters:
 *     flags - Default device information flags.
 *
 * Returns:
 */
void BSM_LD_SetDiscoverMode(BsmLocalDevice *ldev, BsmDiscoverFlag flags);

/*---------------------------------------------------------------------------
 * BSM_LD_AddRemoteDeviceAddress()
 *
 *     Allows a remote device to be manually added to the list of known devices.
 *
 * Parameters:
 *     devAddr - A pointer to a BtDevAddr structure.
 *
 * Returns:
 */
void BSM_LD_AddRemoteDeviceAddress(BtDevAddr *devAddr);

/*---------------------------------------------------------------------------
 * BSM_LD_StopDiscovery()
 *
 *     Asks the local device to cancel any discovery processes currently
 *     in progress.
 *
 *     At the successful cancellation of discovery, the local device's mode
 *     will change, resulting in BSME_LD_MODE_CHANGE.
 *
 * Parameters:
 *     ldev - The local device of interest.
 */
void BSM_LD_StopDiscovery(BsmLocalDevice *ldev);

/*---------------------------------------------------------------------------
 * BSM_LD_SetMode()
 *
 *     Asks the local device to change the accessibility mode. The
 *     accessibility mode can be used to control discoverability and remote
 *     connectability.  The current mode can be retrieved by using getMode().
 *
 *     When the accessibility mode change completes, the local device's
 *     mode will change accordingly, resulting in BSME_LD_MODE_CHANGE.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *
 *     mode - The new device mode to change the local device to immediately.
 */
void BSM_LD_SetMode(BsmLocalDevice *ldev, BsmLocalDeviceMode mode);

/*---------------------------------------------------------------------------
 * BSM_LD_SetTempMode()
 *
 *     Asks the local device to temporarily change the accessibility mode.
 *     After the time specified the accessibility mode will return to the
 *     mode set via BSM_LD_SetMode().
 *
 *     When the accessibility mode change completes, the local device's mode
 *     will change accordingly, resulting in BSME_LD_MODE_CHANGE.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *
 *     mode - The new device mode to change the local device to immediately.
 *
 *     time - Time in seconds to wait before reverting to the original
 *         accessibility mode.
 */
void BSM_LD_SetTempMode(BsmLocalDevice *ldev, BsmLocalDeviceMode mode, U16 time);

#if defined(NUM_LE_DEVICES) && NUM_LE_DEVICES > 0
/*---------------------------------------------------------------------------
 * BSM_LD_IsAdvertising()
 *
 *     Determine if the LE device is currently advertising.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *
 * Returns:
 *     TRUE if the local device is currently advertising.
 *
 *     FALSE if the local device is not currently advertising.
 *
 */
BOOL BSM_LD_IsAdvertising(BsmLocalDevice *ldev);

/*---------------------------------------------------------------------------
 * BSM_LD_IsAdvertLeOnly()
 *
 *     Determine if the LE device advertises support for LE-only mode.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *
 * Returns:
 *     TRUE if the local device is currently advertising support for LE only.
 *
 *     FALSE if the local device is currently advertising support for both
 *         LE and BR/EDR.
 *
 */
BOOL BSM_LD_IsAdvertLeOnly(BsmLocalDevice *ldev);

/*---------------------------------------------------------------------------
 * BSM_LD_IsAdvertRestart()
 *
 *     Determine if the LE device will automatically restart advertising
 *     when the link is disconnected.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *
 * Returns:
 *     TRUE if the local device is currently setup to restart advertising.
 *
 *     FALSE if the local device is not currently setup to restart
 *         advertising.
 *
 */
BOOL BSM_LD_IsAdvertRestart(BsmLocalDevice *ldev);

/*---------------------------------------------------------------------------
 * BSM_LD_IsAllPrivacyAdMask()
 *
 *     Determine if the LE device will include all advertising data (AD)
 *     types when privacy is enabled.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *
 * Returns:
 *     TRUE if the local device is currently setup to include all AD types.
 *
 *     FALSE if the local device is currently setup to include no AD types.
 *
 */
BOOL BSM_LD_IsAllPrivacyAdMask(BsmLocalDevice *ldev);

/*---------------------------------------------------------------------------
 * BSM_LD_SetAdvertLeOnly()
 *
 *     Allow the LE device to advertise support for LE-only or
 *     LE and BR/EDR modes.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *
 *     enable - TRUE or FALSE.
 *
 */
void BSM_LD_SetAdvertLeOnly(BsmLocalDevice *ldev, BOOL enable);

/*---------------------------------------------------------------------------
 * BSM_LD_AutoRestartAdvert()
 *
 *     Request that the LE device automatically restarts advertising
 *     upon a link disconnect.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *
 *     enable - TRUE or FALSE.
 *
 */
void BSM_LD_AutoRestartAdvert(BsmLocalDevice *ldev, BOOL enable);

/*---------------------------------------------------------------------------
 * BSM_LD_SetAllPrivacyAdMask()
 *
 *     Request that the LE device sets up the advertising data (AD) types
 *     to be included during privacy.  The option is either all AD types or
 *     no AD types.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *
 *     enable - TRUE or FALSE.
 *
 */
void BSM_LD_SetAllPrivacyAdMask(BsmLocalDevice *ldev, BOOL enable);

/*---------------------------------------------------------------------------
 * BSM_LD_UpdateAdvertData()
 *
 *     Request that LE Controller to update its advertising data due to a 
 *     change locally.
 *
 */
void BSM_LD_UpdateAdvertData(void);

/*---------------------------------------------------------------------------
 * BSM_LD_SetAdvertMode()
 *
 *     Request the LE Controller to start or stop advertising. The Controller
 *     manages the timing of advertisements based on the advertising parameters
 *     set using parms. If no parameters are provided, the default advertising
 *     parameters in Connection Manager will be used.
 *
 *     The Controller will continue advertising until CMGR_StopLeAdvertising() is
 *     called, which is triggered by setting "enable" to MELE_DISABLE, or
 *     until advertising encounters a timeout.  A timeout will occur when
 *     both advertising timer intervals expire or when a direct advertising
 *     timeout is indicated by the controller.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *
 *     enable - MELE_ENABLE or MELE_DISABLE.
 *
 *     parms - Optional advertisement parameters to use.  Set to 0 to use default
 *             parameters.
 */
void BSM_LD_SetAdvertMode(BsmLocalDevice *ldev, MeLeEnable enable,
                          CmgrLeAdvertParms* parms);

/*---------------------------------------------------------------------------
 * BSM_LD_EnableAdvertising()
 *
 *     Request the LE Controller enable or disable advertising. If advertising
 *     is disabled then BSM_LD_SetAdvertMode will not be able to start
 *     advetising.
 *
 * Parameters:
 *     enable - MELE_ENABLE or MELE_DISABLE.
 */
void BSM_LD_EnableAdvertising(BOOL enable);

/*---------------------------------------------------------------------------
 * BSM_LD_GetAdvertParms()
 *
 *     Get the LE advertising parameters.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *
 *     parms - Pointer to memory to receive the parameters.
 */
void BSM_LD_GetAdvertParms(BsmLocalDevice *ldev, CmgrLeAdvertParms* parms);

/*---------------------------------------------------------------------------
 * BSM_LD_SetLeScanParms()
 *
 *  Set the Le Scan Parameters
 *
 * Parameters:
 *     ldev - The local device of interest.
 *
 *     parms - Scan parameters to use.
 *
 *     discAdvType - Discovory Type (limited/general)
 */
void BSM_LD_SetLeScanParms(BsmLocalDevice *ldev, CmgrLeScanParms *parms, CmgrLeAdvertType discAdvType);

/*---------------------------------------------------------------------------
 * BSM_LD_SetConnParms()
 *
 *     Setup the LE connection parameters.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *
 *     parms - Connection parameters to use.
 */
void BSM_LD_SetConnParms(BsmLocalDevice *ldev, CmgrLeGlobalConParms* parms);

/*---------------------------------------------------------------------------
 * BSM_LD_GetConnParms()
 *
 *     Get the LE connection parameters.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *
 *     parms - Pointer to memory to receive the parameters.
 */
void BSM_LD_GetConnParms(BsmLocalDevice *ldev, CmgrLeGlobalConParms* parms);

/*---------------------------------------------------------------------------
 * BSM_RD_LookupLeGapService()
 *
 *     Discovers GAP service values on a remote device.
 *
 * Parameters:
 *     rdev - The remote device
 */
void BSM_RD_LookupLeGapService(BsmRemoteDevice *rdev);

/*---------------------------------------------------------------------------
 * BSM_RD_UpdateLeConnectionParms()
 *
 *     Changes the Link Layer connection parameters of an existing link.
 *     This function can only be used when the local device's role is master.
 *
 * Parameters:
 *     rdev - The remote device
 *
 *     parms - LE link parameters
 */
void BSM_RD_UpdateLeConnectionParms(BsmRemoteDevice *rdev, MeLeUpdateLinkParms *parms);

/*---------------------------------------------------------------------------
 * BSM_RD_SetLeDataLen()
 *
 *     Suggest the maximum transmission packet size and maximum packet 
 *     transmission time to be used for the connection with the remote device.
 *
 * Parameters:
 *     rdev - The remote device
 *
 *     length - the maximum data length.
 *
 *     time - the maximum transmission time in milliseconds.
 */
void BSM_RD_SetLeDataLen(BsmRemoteDevice *rdev, U16 length, U16 time);

/*---------------------------------------------------------------------------
 * BSM_RD_GetIdentityAddress()
 *
 *     Return a pointer to the BtDevAddr structure containing the identity
 *     address to be used with the device with given bdAddr.
 *
 * Parameters:
 *     addr - BtDevAddr pointer of device to lookup. This will be filled
 *            with the directed address on return.
 */
void BSM_RD_GetIdentityAddress(BtDevAddr *addr);

/*---------------------------------------------------------------------------
 * BSM_LD_EnablePrivacy()
 *
 *     Request the LE device supports privacy mode.
 *
 * Parameters:
 *     enable - TRUE or FALSE.
 *
 */
void BSM_LD_EnablePrivacy(BOOL enable);

/*---------------------------------------------------------------------------
 * BSM_LD_ChangePrivacyAddress()
 *
 *     Request the LE device change its privacy address.
 *
 * Parameters:
 *     ldev - The local device of interest.
 *
 */
void BSM_LD_ChangePrivacyAddress(BsmLocalDevice *ldev);

/*---------------------------------------------------------------------------
 * BSM_LD_SetZeroIdentityAddr()
 *
 *     Request the LE device distribute a zero Identity Address.
 *
 * Parameters:
 *     enable - TRUE or FALSE.
 *
 */
void BSM_LD_SetZeroIdentityAddr(BOOL useZero);

/*---------------------------------------------------------------------------
 * BSM_LD_GetZeroIdentityAddr()
 *
 *     Get current setting for how LE device distributes a zero Identity Address.
 *
 * Parameters:
 *     none
 *
 */
BOOL BSM_LD_GetZeroIdentityAddr(void);

/*---------------------------------------------------------------------------
 * BSM_LD_GATT_GetMtu()
 *
 *     Request the LE GATT MTU value.
 *
 * Parameters:
 *     none.
 *
 */
int BSM_LD_GATT_GetMtu(void);

/*---------------------------------------------------------------------------
 * BSM_LD_GATT_SetMtu()
 *
 *     Set the LE GATT MTU value.
 *
 * Parameters:
 *     none.
 *
 */
void BSM_LD_GATT_SetMtu(U16 gattMtu);

void BSM_LD_GATT_AllowDeviceNameWrite(BOOL allow);
void BSM_LD_GATT_AllowAppearanceWrite(BOOL allow);
void BSM_LD_GATT_SetVendorAdvertData(BOOL set);
void BSM_LD_GATT_SetAdvertServiceData(BOOL set);
void BSM_LD_GATT_SetAdvertType(BOOL set, U8 index);
void BSM_LD_GATT_SetDevInfo(GattDevInfo *devInfo);
void BSM_LD_GATT_RemoveDevInfo(BOOL isShutdown);
void BSM_LD_GATT_GetServerTimeValues(TimeT *initTime, TimeT *respTime);
void BSM_LD_GATT_SetServerTimeValues(TimeT initTime, TimeT respTime);
void BSM_LD_SetLeDefaultDataLen(U16 length, U16 time);

#endif /* NUM_LE_DEVICES */


/****************************************************************************
 *
 * Section: Remote Device APIs
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BSM_RD_Discover()
 *
 *     Asks the local device to discover information about devices in range.
 *     Any device found to be in range is scanned for updates to its
 *     friendly name and available services.
 *
 * Parameters:
 *     rdev - The remote device to discover.
 *
 *     flags - Type of discovery to perform.  Valid flags: BSM_DF_SDP,
 *             BSM_DF_GATT, and BSM_DF_NAME.
 * Returns:
 *     TRUE if the local device was successfully transitioned to discovery
 *     mode. BSM_LD_GetMode will return BSM_LD_DISCOVERING until discovery
 *     is complete.
 *
 *     FALSE if it was not possible to begin the discovery process.
 */
BOOL BSM_RD_Discover(BsmRemoteDevice *rdev, BsmDiscoverFlag flags);

/*---------------------------------------------------------------------------
 * BSM_RD_GetNextService()
 *
 *     Returns the next remote service known to have been advertised on the
 *     specified device, or 0 if no more.
 *
 * Parameters:
 *     rdev - The remote device of interest.
 *
 *     previous - 0 to get the first remote service, or the most recently
 *     returned service to get the next one.
 *
 * Returns:
 *     Next remote service, or 0 if no more
 */
BsmRemoteService *BSM_RD_GetNextService(BsmRemoteDevice *rdev, BsmRemoteService *previous);

/*---------------------------------------------------------------------------
 * BSM_RD_GetMode()
 *
 *     Returns the remote device's current mode bits.
 *
 * Parameters:
 *     rdev - The remote device of interest.
 *
 * Returns:
 *     Mode bits describing the state of the remote device.
 */
BsmRemoteDeviceMode BSM_RD_GetMode(BsmRemoteDevice *rdev);

/*---------------------------------------------------------------------------
 * BSM_RD_Pair()
 *
 *     Creates or breaks pairing with the device.
 *
 * Parameters:
 *     rdev - The remote device to pair or unpair.
 *     bonded - TRUE to pair. If pairing already exists, returns 
 *         immediately. If a pair does not exist, this call will attempt
 *         to initiate pairing. If "paired" is FALSE, any existing pairing
 *         is broken.
 *
 * Returns:
 *     TRUE if the request was initiated or completed.
 *
 *     FALSE if the request could not be initiated.
 */
BOOL BSM_RD_Pair(BsmRemoteDevice *rdev, BOOL paired);

/*---------------------------------------------------------------------------
 * BSM_RD_Encrypt()
 *
 *     Initiates encryption with a bonded device.  This is only available
 *     for Low Energy devices, as encryption is started upon ACL establishment
 *     for BR/EDR devices.
 *
 * Parameters:
 *     rdev - The remote device to encrypt with.
 *     encrypt - TRUE to start encryption. If encryption exists, returns
 *         immediately. If encryption does not exist, this call will attempt
 *         to initiate encryption. If "encrypt" is FALSE, encryption will be
 *         disabled by removing the ACl connection.
 *
 * Returns:
 *     TRUE if the request was initiated or completed.
 *
 *     FALSE if the request could not be initiated.
 */
BOOL BSM_RD_Encrypt(BsmRemoteDevice *rdev, BOOL encrypt);

/*---------------------------------------------------------------------------
 * BSM_RD_Disconnect()
 *
 *     Forcibly disconnects all connections with this device. If and when
 *     the disconnection attempt completes, a BSME_RD_CONNECT_STATE event
 *     is delivered.
 *
 * Parameters:
 *     rdev - The remote device of interest.
 */
void BSM_RD_Disconnect(BsmRemoteDevice *rdev);

/*---------------------------------------------------------------------------
 * BSM_RD_GetDeviceAddress()
 *
 *     Retrieves the device address corresponding to this device.
 *
 * Parameters:
 *     rdev - The remote device of interest.
 *     addr - (Output) Destination for address.
 */
void BSM_RD_GetDeviceAddress(BsmRemoteDevice *rdev, BtDevAddr *addr);

/*---------------------------------------------------------------------------
 * BSM_RD_GetIdentityAddressRdev()
 *
 *     Retrieves the identity address corresponding to this device. This
 *     will be the same as the device address for BR/EDR.
 *
 * Parameters:
 *     rdev - The remote device of interest.
 *     addr - (Output) Destination for address.
 */
void BSM_RD_GetIdentityAddressRdev(BsmRemoteDevice *rdev, BtDevAddr *addr);

/*---------------------------------------------------------------------------
 * BSM_RD_GetDirectedRpaAddressRdev()
 *
 *     Retrieves the peer's directed RPA address received in the advertising
 *     reports but that may correspond to an older address for our device.
 *
 * Parameters:
 *     rdev - The remote device of interest.
 *     addr - (Output) Destination for address.
 */
void BSM_RD_GetDirectedRpaAddressRdev(BsmRemoteDevice *rdev, BtDevAddr *addr);

/*---------------------------------------------------------------------------
 * BSM_RD_GetDualModeId()
 *
 *     Retrieves the unique dual-mode identifier corresponding to this 
 *     device. This identifier will be used in pairs to link LE and BR/EDR
 *     devices.
 *
 * Parameters:
 *     rdev - The remote device of interest.
 *
 * Returns:
 *     Dual-mode identifier value.
 */
U16 BSM_RD_GetDualModeId(BsmRemoteDevice *rdev);

/*---------------------------------------------------------------------------
 * BSM_RD_AreGattServicesCached()
 *
 *     Determines if Gatt services of the remote device are cached.
 *
 * Parameters:
 *     rdev - The remote device of interest.
 *
 * Returns:
 *     TRUE Gatt services are cached otherwise FALSE.
 */
BOOL BSM_RD_AreGattServicesCached(BsmRemoteDevice *rdev);

/*---------------------------------------------------------------------------
 * BSM_RD_ClearGattCache()
 *
 *     Clear the GATT cache for the remote device
 *
 * Parameters:
 *     rdev - The remote device of interest.
 *
 *     clearAll - Triggers whether all GATT service information is cleared
 *         for this remote device.
 *
 */
void BSM_RD_ClearGattCache(BsmRemoteDevice *rdev, BOOL clearAll);

/*---------------------------------------------------------------------------
 * BSM_RD_RemoveRemoteDevice()
 *
 *     Remove a remote device, delete all its resources and update the
 *     the persistent storage.
 *
 * Parameters:
 *     rdev - The remote device of interest.
 *
 */
void BSM_RD_RemoveRemoteDevice(BsmRemoteDevice *rdev);

/*---------------------------------------------------------------------------
 * BSM_RD_RemoveDeviceServices()
 *
 *     Remove the services from a remote device, deleting all the resources
 *     used by the services and removing them from the persistent storage.
 *
 * Parameters:
 *     rdev - The remote device of interest.
 *
 */
void BSM_RD_RemoveDeviceServices(BsmRemoteDevice *rdev);

/*---------------------------------------------------------------------------
 * BSM_RD_GetDeviceClass()
 *
 *     Returns the class of device by which this item is known.
 *
 * Parameters:
 *     rdev - The remote device of interest.
 *
 * Returns:
 *    The last known device class bits observed for this device.
 */
BsmDeviceClass BSM_RD_GetDeviceClass(BsmRemoteDevice *rdev);

/*---------------------------------------------------------------------------
 * BSM_RD_GetFriendlyName()
 *     Returns the friendly name last retrieved from the remote device.
 *
 * Parameters:
 *     rdev - The remote device of interest.
 *     name - A buffer containing at least "len" bytes of storage. If the
 *          length is not long enough it is truncated and null-terminated.
 *     len - The size of the "name" buffer.
 *
 * Returns:
 *     The full length of the name including null-termination.
 */
U16 BSM_RD_GetFriendlyName(BsmRemoteDevice *rdev, U8 *name, U16 len);

/*---------------------------------------------------------------------------
 * BSM_RD_Authorize()
 *
 *     Indicates whether services on the remote device are automatically
 *     accepted whenever future service authorization requests are made.
 *
 *     This setting is only important if a local service requires authorize
 *     when connecting. Changing this setting will not affect existing
 *     connections or authorize requests already in-progress.
 *
 *     The setting can be saved to the device security record when the device
 *     is bonded and the alwaysAuthorize parameter is set to TRUE. This will
 *     store the automatic authorization for all future connections unless
 *     the bonding information is deleted.
 *
 *     The current level of authorize can be read using BSM_RD_GetMode and
 *     inspecting the BSM_RD_AUTHORIZED bit.
 *
 * Parameters:
 *     rdev - The remote device of interest.
 *     authorize - FALSE to indicate the device should be authorized. FALSE to
 *         indicate the device should not be authorized.
 *     alwaysAuthorize - FALSE to indicate the authorization applies to
 *         this connection only. TRUE to indicate the authorization should
 *         be saved to the device security record for future connections.
 *
 * Returns:
 *     TRUE  - Authorize has been requested or applied to the device.
 *     FALSE - Authorize could not be updated (user attempted to mark an
 *         unbonded and unconnected device as authorized).
 */
BOOL BSM_RD_Authorize(BsmRemoteDevice *rdev, BOOL authorize, BOOL alwaysAuthorize);

/*---------------------------------------------------------------------------
 * BSM_RD_Block()
 *
 *     Blocks or unblocks a remote device.
 *
 *     A blocked device is disconnected, if connected. No further
 *     connections to the blocked device are permitted (regardless of
 *     who initiates them).
 *
 * Parameters:
 *     rdev - The remote device to block or unblock
 *     block - TRUE to block the device, FALSE to unblock it.
 */
void BSM_RD_Block(BsmRemoteDevice *rdev, BOOL block);

/*---------------------------------------------------------------------------
 * BSM_RD_GetNextAmp()
 *
 *     Returns the next AMP known to exist on the remote device, or 0 if
 *     there are no more.
 *
 * Parameters:
 *     rdev - The remote device of interest.
 *
 *     previous - 0 to get the first remote amp, or the most recently
 *          returned AMP to get the next one.
 *
 * Returns:
 *     Next remote AMP, or 0 if no more
 */
BsmRemoteAmp *BSM_RD_GetNextAmp(BsmRemoteDevice *rdev, BsmRemoteAmp *previous);

/*---------------------------------------------------------------------------
 * BSM_RD_ConnectRemoteDevice()
 *
 *     Connects a remote device.
 *
 * Parameters:
 *     rdev - The remote device
 */
void BSM_RD_ConnectRemoteDevice(BsmRemoteDevice *rdev, BtDevAddr *devAddr);

/*---------------------------------------------------------------------------
 * BSM_RD_DisconnectRemoteDevice()
 *
 *     Disconnects a remote device.  This also handles situations where
 *     cancellation of an outstanding connection is needed.
 *
 * Parameters:
 *     rdev - The remote device
 */
void BSM_RD_DisconnectRemoteDevice(BsmRemoteDevice *rdev);

/*---------------------------------------------------------------------------
 * BSM_RD_SetAuthPayloadTimeoutValue()
 *
 *     Setup the Authentication Payload Timeout value for the current 
 *     connection.
 *
 * Parameters:
 *     rdev - The remote device
 *
 *     timeout - Authentication payload timeout to use.
 */
void BSM_RD_SetAuthPayloadTimeoutValue(BsmRemoteDevice *rdev, U16 timeout);

/****************************************************************************
 *
 * Section: User Request APIs
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BSM_UR_GetInfo()
 *     Retrieves information about an outstanding user request.
 *
 * Parameters:
 *     ureq - The user request of interest.
 *     info - A pointer to a BsmUserRequestInfo structure which is
 *         filled with information about the user request.
 *
 * Returns:
 *     TRUE if the user request info was successfully retrieved.
 *     FALSE if the user request handle is invalid (this occurs if the
 *         user request was already responded to or was cancelled).
 */
BOOL BSM_UR_GetInfo(BsmUserRequest *ureq, BsmUserRequestInfo *info);

/*---------------------------------------------------------------------------
 * BSM_UR_Respond()
 *     Responds to a user request previously indicated via BSME_UR_REQUESTED.
 *     BSME_UR_COMPLETE is generated when the response is finalized.
 *     Depending on the event type, this may occur during BSM_UR_Respond or
 *     at a later point.
 *
 * Parameters:
 *     ureq - The user request receiving a response. If this handle is
 *         invalid this function call is ignored.
 *     accept - TRUE or FALSE, indicating whether the request was accepted.
 *     info - Additional info as required by some requests.
 *
 *
 * Returns:
 *     TRUE if the response was accepted.
 *     FALSE if the response was not accepted. This can happen if the
 *         request handle is invalid, has already heard a response, because
 *         the response is malformed, or because the request does not require
 *         a response.
 */
BOOL BSM_UR_Respond(BsmUserRequest *ureq, BOOL accept,
                    const BsmUserResponseInfo *info);


/****************************************************************************
 *
 * Section: Local Service APIs
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * BSM_LS_SetEnabled()
 *
 *     Requests this service be transitioned to enabled or disabled state.
 *     If this request results in a change to the service's enable state,
 *     BSME_LS_ENABLE_STATE event will arrive.
 *
 *     Note that if the service is in the process of becoming enabled or
 *     disabled, any request to change it is ignored.
 *
 * Parameters:
 *     service - The local service of interest.
 *
 *     enabled - TRUE to enable, FALSE to disable
 */
void BSM_LS_SetEnabled(BsmLocalService *service, BOOL enabled);

/*---------------------------------------------------------------------------
 * BSM_LS_GetEnabled()
 *
 *     Returns the current enable-state of this local service.
 *
 * Parameters:
 *     service - The local service of interest.
 *
 * Returns:
 *     TRUE or FALSE to indicate the service's enable state.
 */
BOOL BSM_LS_GetEnabled(BsmLocalService *service);

/*---------------------------------------------------------------------------
 * BSM_LS_SetSecurity()
 *
 *     Sets the security requirements for this service.
 *
 * Parameters:
 *     service - The local service of interest.
 *
 *     settings - Describes the requested security level
 *
 * Returns:
 *     TRUE if the security parameters were successfully applied.
 *     FALSE if an error occurred.
 */
BOOL BSM_LS_SetSecurity(BsmLocalService *service, const BsmSecuritySettings *settings);

/*---------------------------------------------------------------------------
 * BSM_LS_GetSecurity()
 *
 *     Retrieves the current security requirements for this service.
 *
 * Parameters:
 *     service - The local service of interest.
 *
 *     settings - On output, filled with the current security level for
 *         the service.
 *
 * Returns:
 *     TRUE if the parameters were successfully retrieved.
 *     FALSE if an error occurred.
 */
BOOL BSM_LS_GetSecurity(BsmLocalService *service, BsmSecuritySettings *settings);

/*---------------------------------------------------------------------------
 * BSM_LS_Show()
 *
 *     Requests this service launch its configuration UI, if any.
 *
 * Parameters:
 *     service - The local service of interest. Must be "enabled".
 *
 *     show - TRUE to show, FALSE to hide.
 *
 * Returns:
 *     FALSE - The service is not enabled, or does not have a configuration
 *         UI.
 *     TRUE - The request to show or hide the UI has been initiated.
 */
BOOL BSM_LS_Show(BsmLocalService *service, BOOL show);

/*---------------------------------------------------------------------------
 * BSM_LS_SetFriendlyName()
 *
 *     Sets the friendly name for this service. For server-capable services,
 *     this may update the advertised friendly name, visible by remote
 *     devices. If and when the name is successfully changed,
 *     BSME_LS_NAME_CHANGE event is issued by BSM.
 *
 * Parameters:
 *     service - The local service of interest.
 *     name - A null-terminated, UTF-8 name.
 */
void BSM_LS_SetFriendlyName(BsmLocalService *service, const U8 *name);

/*---------------------------------------------------------------------------
 * BSM_LS_GetFriendlyName()
 *
 *     Retrieves this service's null-terminated UTF8 friendly name, writing
 *     it into the provided buffer.
 *
 * Parameters:
 *     service - The local service of interest.
 *     name - A buffer containing at least "len" bytes of storage. If the
 *         length is not long enough it is truncated and null-terminated.
 *     len - The size of the "name" buffer.
 *
 * Returns:
 *    The full length of the name including null-termination.
 */
U16 BSM_LS_GetFriendlyName(BsmLocalService *service, U8 *name, U16 len);

/*---------------------------------------------------------------------------
 * BSM_LS_GetMode()
 *
 *     Returns mode bits that describe this service instance
 *
 * Parameters:
 *     service - The local service of interest.
 *
 * Returns:
 *     Flags describing the service.
 */
BsmServiceMode BSM_LS_GetMode(BsmLocalService *service);

/*---------------------------------------------------------------------------
 * BSM_LS_GetServiceClass()
 *
 *     Returns the service class associated with this service.
 *
 * Parameters:
 *     service - The local service of interest.
 *
 * Returns:
 *    The service class associated with this service instance.
 */
BsmServiceClassUuid BSM_LS_GetServiceClass(BsmLocalService *service);

/*---------------------------------------------------------------------------
 * BSM_LS_GetContext()
 *
 *     Returns a platform-specific context pointer.
 *
 * Parameters:
 *     service - The local service of interest.
 *
 * Returns:
 *     A service-specific context pointer, as supplied by the corresponding
 *     service agent. Returns 0 if the local service is not enabled or does
 *     not supply a context.
 */
void *BSM_LS_GetContext(BsmLocalService *service);

/*---------------------------------------------------------------------------
 * BSM_LS_GetNextRemoteService()
 *
 *     Enumerates the services associated with this local service.
 *
 * Parameters:
 *     lservice - The local service of interest.
 *
 *     rservice - The current remote service, or zero to return the first.
 *
 * Returns:
 *     The next known remote service after rservice, or 0 if there are
 *     no more.
 */
BsmRemoteService *BSM_LS_GetNextRemoteService(BsmLocalService *lservice,
                                              BsmRemoteService *rservice);

/****************************************************************************
 *
 * Section: Remote Service APIs
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * BSM_RS_GetFriendlyName()
 *
 *     Retrieves this service's null-terminated UTF8 friendly name, writing
 *     it into the provided buffer.
 *
 * Parameters:
 *     service - The remote service of interest.
 *     name - A buffer containing at least "len" bytes of storage. If the
 *         length is not long enough it is truncated and null-terminated.
 *     len - The size of the "name" buffer.
 *
 * Returns:
 *    The full length of the name *including* null-termination.
 */
U16 BSM_RS_GetFriendlyName(BsmRemoteService *service, U8 *name, U16 len);

/*---------------------------------------------------------------------------
 * BSM_RS_GetServiceClass()
 *
 *     Returns the service class associated with this service.
 *
 * Parameters:
 *     service - The remote service of interest.
 *
 * Returns:
 *    The service class associated with this remote service instance.
 */
BsmServiceClassUuid BSM_RS_GetServiceClass(BsmRemoteService *service);

/*---------------------------------------------------------------------------
 * BSM_RS_GetMode()
 *
 *     Returns current mode bits for this remote service.
 *
 * Parameters:
 *     service - The remote service of interest.
 *
 * Returns:
 *    The current mode bits.
 */
BsmServiceMode BSM_RS_GetMode(BsmRemoteService *service);

/*---------------------------------------------------------------------------
 * BSM_RS_Authorize()
 *
 *     Indicates whether the remote service is automatically accepted
 *     whenever future service authorization requests are made.
 *
 *     This setting is only important if the corresponding local service
 *     requires authorize. Changing this setting will not affect existing
 *     service connections or authorize requests already in progress.
 *
 *     The current level of authorize can be read using BSM_RS_GetMode and
 *     inspecting the BSM_S_AUTHORIZED bit.
 *
 * Parameters:
 *     service - The remote service of interest.
 *
 *     authorized - TRUE to indicate that authorize requests for this service
 *         should be approved automatically.
 */
void BSM_RS_Authorize(BsmRemoteService *service, BOOL authorized);

/*---------------------------------------------------------------------------
 * BSM_RS_GetDevice()
 *
 *     Returns a pointer to the remote device where this service was found.
 *
 * Parameters:
 *     service - The remote service of interest.
 *
 * Returns:
 *     BsmRemoteDevice pointer or 0 if service was invalid.
 */
BsmRemoteDevice *BSM_RS_GetDevice(BsmRemoteService *service);

/*---------------------------------------------------------------------------
 * BSM_RS_Start()
 *
 *     Initiates activity (often a connection) with the remote service.
 *
 *     Actual "start" behavior triggered by this call is determined by the
 *     platform's underlying implementation of the service. For example,
 *     the service may immediately initiate a connection, open a
 *     configuration dialog requesting more information, provide the user
 *     with a menu of actions that could be performed through the remote
 *     service, etc.
 *
 *     When called for a service that is already connected, this function
 *     may be interpreted as a request to display or re-open the user
 *     interface for the remote service instance.
 *
 * Parameters:
 *     service - The remote service of interest.
 *
 * Returns:
 *     TRUE - the activity was successfully launched, resulting in UI
 *         updates or future event notifications.
 *
 *     FALSE - no activity could be launched for this service at this time.
 */
BOOL BSM_RS_Start(BsmRemoteService *service);

/*---------------------------------------------------------------------------
 * BSM_RS_Stop()
 *
 *     Requests closure of any activity started with BSM_RS_Start() and/or
 *     disconnection of currently active connections.
 *
 * Parameters:
 *     service - The remote service of interest.
 */
void BSM_RS_Stop(BsmRemoteService *service);

/*---------------------------------------------------------------------------
 * BSM_RS_IsIncoming()
 *
 *     Indicates whether this remote service entity corresponds only to
 *     a remote client talking to a local server.
 *
 * Parameters:
 *     service - The remote service of interest.
 *
 * Returns:
 *     A service connection object or 0 if the connection attempt
 *     could not be initiated.
 */
BOOL BSM_RS_IsIncomingOnly(BsmRemoteService *service);

/*---------------------------------------------------------------------------
 * BSM_RS_GetContext()
 *
 *     Returns a platform-specific context pointer.
 *
 * Parameters:
 *     service - The remote service of interest.
 *
 * Returns:
 *     A service-specific context pointer, as supplied by the corresponding
 *     service agent in connection with this service's device. Returns 0
 *     if the remote service has no corresponding enabled local service
 *     entity.
 */
void *BSM_RS_GetContext(BsmRemoteService *service);

/*---------------------------------------------------------------------------
 * BSM_RS_GetLocalService()
 *
 *     Returns the local service associated with the current remote service's
 *     connection.
 *
 * Parameters:
 *     service - A remote service in non-disconnected state.
 *
 * Returns:
 *     BsmLocalService involved with the connection, or 0 if this remote
 *     service is not connected.
 */
BsmLocalService *BSM_RS_GetLocalService(BsmRemoteService *rservice);


/*---------------------------------------------------------------------------
 * BSM_RS_GetNextAmpPair()
 *
 *     Returns the next AMP pair currently in use by a remote service.
 *
 * Parameters:
 *     pair - On input, contains a valid AMP pair, or points to an
 *         empty structure to request the first AMP pair. On output,
 *         contains the next AMP pair. If there are no further AMP pairs,
 *         will be empty upon return.
 */
void BSM_RS_GetNextAmpPair(BsmRemoteService *rservice, BsmAmpPair *pair);


/****************************************************************************
 *
 * Section: Local AMP APIs
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * BSM_LA_GetInfo()
 *     Retrieves information about a local AMP.
 *
 * Parameters:
 *     lamp - The local AMP of interest.
 *     info - A pointer to a BsmAmpInfo structure which is filled with
 *         information about the AMP.
 *
 * Returns:
 *     TRUE if successful in filling the info structure.
 *     FALSE if the AMP is not recognized.
 */
BOOL BSM_LA_GetInfo(BsmLocalAmp *lamp, BsmAmpInfo *info);

/*---------------------------------------------------------------------------
 * BSM_LA_GetNextRemoteAmp()
 *     Returns the next remote AMP with a connection to this local AMP
 *     or 0 if no more.
 *
 * Parameters:
 *     lamp - The local AMP of interest.
 *     previous - The previous connected remote AMP, or 0 to query for the
 *         first.
 *
 * Returns:
 *     The next connected remote AMP, or 0 if no more.
 */
BsmRemoteAmp *BSM_LA_GetNextRemoteAmp(BsmLocalAmp *lamp, BsmRemoteAmp *previous);


/****************************************************************************
 *
 * Section: Remote AMP APIs
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * BSM_RA_GetInfo()
 *     Retrieves information about a remote AMP.
 *
 * Parameters:
 *     ramp - The remote AMP of interest.
 *     info - A pointer to a BsmAmpInfo structure which is filled with
 *         information about the AMP.
 *
 * Returns:
 *     TRUE if successful in filling the info structure.
 *     FALSE if the AMP is not recognized.
 */
BOOL BSM_RA_GetInfo(BsmRemoteAmp *ramp, BsmAmpInfo *info);

/*---------------------------------------------------------------------------
 * BSM_RA_GetRemoteDevice()
 *     Returns the remote device upon which this remote AMP was found.
 *
 * Parameters:
 *     ramp - The remote AMP of interest.
 *
 * Returns:
 *     The remote device hosting this remote AMP, or 0 if not found.
 */
BsmRemoteDevice *BSM_RA_GetRemoteDevice(BsmRemoteAmp *ramp);

void bsmCheckCreatable(void);

#endif /* __BSM_H */
