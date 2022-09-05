/****************************************************************************
 *
 * File:
 *     didi.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file contains internal definitions for the Health Device Profile
 *     (MAP).
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

#ifndef __DIDI_H_
#define __DIDI_H_

#include "sdp.h"
#include "did.h"

/****************************************************************************
 *
 * Internal Data Types
 *
 ***************************************************************************/

/* Constants */
#define DEVID_MANDATORY_ATTRIB_CNT   8

#if DEVID_SDP_REGISTER_SERVICE_DESCRIPTION == XA_ENABLED
#define DEVID_SERV_DESC_ATTRIB_CNT   2
#else
#define DEVID_SERV_DESC_ATTRIB_CNT   0
#endif

#if DEVID_SDP_REGISTER_DOCUMENTATION_URL == XA_ENABLED
#define DEVID_DOC_URL_ATTRIB_CNT     1
#else
#define DEVID_DOC_URL_ATTRIB_CNT     0
#endif

#if DEVID_SDP_REGISTER_CLIENT_EXEC_URL == XA_ENABLED
#define DEVID_EXEC_URL_ATTRIB_CNT    1
#else
#define DEVID_EXEC_URL_ATTRIB_CNT    0
#endif

#define DEVID_ATTRIB_CNT (DEVID_MANDATORY_ATTRIB_CNT + \
                          DEVID_SERV_DESC_ATTRIB_CNT + \
                          DEVID_DOC_URL_ATTRIB_CNT + \
                          DEVID_EXEC_URL_ATTRIB_CNT)


/*---------------------------------------------------------------------------
 * DidSdpFlags type
 *
 *      Internal DID flags used to tracking the status of the SDP
 *      parsing information.
 */
typedef U8 DidSdpFlags;

#define DID_SDP_FLAGS_DEVID_VENDOR            0x01   /* Device ID - Vendor */
#define DID_SDP_FLAGS_DEVID_PRODUCT_ID        0x02   /* Device ID - Product ID */
#define DID_SDP_FLAGS_DEVID_VERSION           0x04   /* Device ID - Version */
#define DID_SDP_FLAGS_DEVID_PRIMARY_RECORD    0x08   /* Device ID - Primary Record */
#define DID_SDP_FLAGS_DEVID_VENDOR_SOURCE     0x10   /* Device ID - Vendor Source ID */
#define DID_SDP_FLAGS_DEVID_SERV_DESC         0x20   /* Device ID - Service Description */
#define DID_SDP_FLAGS_DEVID_SPEC_VER          0x40   /* Device ID - Specification Version */
#define DID_SDP_FLAGS_DEVID_DOC_URL           0x80   /* Device ID - Documentation URL*/

/* End of DidSdpFlags */

/*---------------------------------------------------------------------------
 * DidSdpFlags2 type
 *
 *      Internal DID flags used to tracking the status of the SDP
 *      parsing information.
 */
typedef U8 DidSdpFlags2;

#define DID_SDP_FLAGS_DEVID_CLIENT_EXEC_URL 0x01  /* Device ID - Client executable URL */

/* End of DidSdpFlags2 */

/****************************************************************************
 *
 * Internal Data Structures
 *
 ****************************************************************************/

#if SDP_SERVER_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * DeviceId structure
 *
 * Used internally for each Device Id record registered
 */
typedef struct _DeviceId{

    /* Device ID Specification Version */
    U16                         specVer;

    /* Source used to assign the vendor ID. */
    VendorIdSource              vendorIdSrc;

    /* Vendor ID of this device's manufacturer, as assigned by the
     * entity specified in device.vendorIdSrc.
     */
    U16                         vendorId;

    /* Product ID for the device (assigned by vendor) */
    U16                         productId;

    /* Version code for the device. This value is vendor-supplied in 0xJJMN
     * format where JJ is the major version number, M is the minor version
     * number, and N is the sub-minor version number. The version number
     * increases with subsequent versions of the device specified in
     * device.productId.
     */
    U16                         version;

    /* Flag to indicate that the record is the primary Device ID record */
    BOOL                        primaryRecord;

    /* This record is already in use. */
    BOOL                        inUse;

    /* SDP Info */
    SdpRecord                   devIdRecord;
    SdpAttribute                devIdAttribs[DEVID_ATTRIB_CNT];
    U8                          sdpDevIdLangBaseIdList[11];
    U8                          sdpDevIdServiceDesc[2 + DEVID_MAX_SERV_DESC_LEN];
    U8                          sdpDevIdDocumentationUrl[2 + DEVID_MAX_URL_LEN];
    U8                          sdpDevIdClientExecUrl[2 + DEVID_MAX_URL_LEN];
    U8                          sdpDevIdSpecification[3];
    U8                          sdpDevIdVendor[3];
    U8                          sdpDevIdProduct[3];
    U8                          sdpDevIdVersion[3];
    U8                          sdpDevIdVendorIdSource[3];
    U8                          sdpDevIdPrimaryRecord[2];

} DeviceId;
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 * DidContext structure
 *
 *     Device ID Profile context structure.
 */
typedef struct _BtDidContext {
    BOOL                        initialized;
    BOOL                        queryInProgress;
    BOOL                        registered;

#if SDP_SERVER_SUPPORT == XA_ENABLED
    /* Dev ID Information stored locally about the device, including SDP
       records, etc. */
    DeviceId                    localDevId[DEVID_MAX_LOCAL_DEVICE_RECORDS];
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */

    /* Used for making a local copy of remote Device ID info*/
    DevIdInfo                   remoteDevInfo[DEVID_MAX_REMOTE_DEVICE_RECORDS];

    /* Callback into application to relay data from remote Device */
    DevIdCallback              callback;

    /* pointer to the BtRemoteDevice of the device being queried */
    BtRemoteDevice             *remDev;

    /* Used internally for querying for Device ID's*/
    SdpQueryToken              sdpQueryToken;
    DidSdpFlags                sdpFlags;
    DidSdpFlags2               sdpFlags2;
    U8                         sdpRecNum;

    U8                         devIdEIRResponse[10];

} BtDidContext;

/*---------------------------------------------------------------------------
 * Context Pointer declarations
 */
#if XA_CONTEXT_PTR == XA_ENABLED
extern BtDidContext *Did;
#define DID(f) (Did->f)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern BtDidContext Did;
#define DID(f) (Did.f)
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

/* Function Prototypes */
#if SDP_SERVER_SUPPORT == XA_ENABLED
BtStatus DevIdAddServiceRecords(const DevIdRegisterParms *Parms);
BtStatus DevIdRemoveServiceRecords(void);
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */

BtStatus DevIdStartServiceQuery(SdpQueryMode mode,
                                   BtRemoteDevice *remDev);
#endif /* __DIDI_H_ */

