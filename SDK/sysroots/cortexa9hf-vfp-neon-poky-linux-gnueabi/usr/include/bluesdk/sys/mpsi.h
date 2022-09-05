/****************************************************************************
 *
 * File:
 *     mpsi.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file contains internal definitions for the Multi-Profile 
 *     Specification (MPS)
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

#ifndef __MPSI_H_
#define __MPSI_H_

#include "sdp.h"
#include "mps.h"

/****************************************************************************
 *
 * Internal Data Types
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * MpsSdpFlags type
 *
 *      Internal MPS flags used to tracking the status of the SDP
 *      parsing information.
 */
typedef U8 MpsSdpFlags;

#define MPS_SDP_FLAGS_MPSD_FLAGS        0x01   /* Multi-Profile - Single Device flags */
#define MPS_SDP_FLAGS_MPMD_FLAGS        0x02   /* Multi-Profile - Multiple Device flags */
#define MPS_SDP_FLAGS_PROF_DEPENDENCIES 0x04   /* Multi-Profile - Profile dependencies */
#define MPS_SDP_FLAGS_SERV_DESC         0x08   /* Device ID - Service Description */
#define MPS_SDP_FLAGS_SPEC_VER          0x10   /* Device ID - Specification Version */

/* End of MpsSdpFlags */

/****************************************************************************
 *
 * Internal Data Structures
 *
 ****************************************************************************/

#if SDP_SERVER_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * MultiProfile structure
 *
 * Used internally for each Multi-Profile record registered
 */
typedef struct _MultiProfile{

    ListEntry                   node; 

    /* Multi-Profile Specification Version */
    U16                         specVer;

    /* This record is already in use. */
    BOOL                        inUse;

    /* SDP Info */
    SdpRecord                   mpsRecord;
#if MPS_SDP_REGISTER_SERVICE_DESCRIPTION == XA_ENABLED
    SdpAttribute                mpsAttribs[8];
#else
    SdpAttribute                mpsAttribs[8];
#endif /* MPS_SDP_REGISTER_SERVICE_DESCRIPTION == XA_ENABLED */
    U8                          sdpMpSdFeatureFlags[9];
    U8                          sdpMpMdFeatureFlags[9];
    U8                          sdpProtocolDependencyFlags[3];

} MultiProfile;
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 * MpsContext structure
 *
 *     Multi-Profile context structure.
 */
typedef struct _BtMpsContext {
    BOOL                initialized;
    BOOL                queryInProgress;
    BOOL                registered;

#if SDP_SERVER_SUPPORT == XA_ENABLED
    /* Multi-Profile Information stored locally about the device, including SDP
       records, etc. */
    MultiProfile        localMultiProfile[MPS_MAX_LOCAL_DEVICE_RECORDS];
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */

    /* Used for making a local copy of remote Device ID info*/
    MultiProfileInfo    remoteMultiProfile[MPS_MAX_REMOTE_DEVICE_RECORDS];

    /* Callback into application to relay data from remote Device */
    MultiProfileCallback callback;

    /* pointer to the BtRemoteDevice of the device being queried */
    BtRemoteDevice      *remDev;

    /* Used internally for querying for Multi-Profile records */
    SdpQueryToken        sdpQueryToken;
    MpsSdpFlags          sdpFlags;
    U8                   sdpRecNum;
    /* Used when parsing partial results */
    U8                   attribPos;
} BtMpsContext;

/*---------------------------------------------------------------------------
 * Context Pointer declarations
 */
#if XA_CONTEXT_PTR == XA_ENABLED
extern BtMpsContext *Mps;
#define MPS(f) (Mps->f)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern BtMpsContext Mps;
#define MPS(f) (Mps.f)
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

/* Function Prototypes */
#if SDP_SERVER_SUPPORT == XA_ENABLED
BtStatus MultiProfileAddServiceRecords(const MultiProfileRegisterParms *Parms);
BtStatus MultiProfileRemoveServiceRecords(void);
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */

BtStatus MultiProfileStartServiceQuery(SdpQueryMode mode,
                                   BtRemoteDevice *remDev);
#endif /* __MPSI_H_ */

