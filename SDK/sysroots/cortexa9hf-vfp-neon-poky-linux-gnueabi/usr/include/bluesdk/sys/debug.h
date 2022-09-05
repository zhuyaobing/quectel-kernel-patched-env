/***************************************************************************
 *
 * File:
 *     debug.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file contains prototypes for debug message functions.
 *
 * Created:
 *     August 13, 1999
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

#ifndef __DEBUG_H
#define __DEBUG_H

#include "me.h"

/*****************************************************************************
 *
 * Debug functions for converting protocol and status types to strings.
 *
 * NOTE: Do not include this file in a header file. It should only be included
 *       in source code files and should be the last file in the include list.
 *
 *****************************************************************************/

/*---------------------------------------------------------------------------
 * SniffProtocolId type
 *
 *     This type is used in ChannelInfo to identify the valid protocol id's.
 */
typedef U32 SniffProtocolId;

#define SPI_LM          0x00010000  /* LMP Connection Handle */
#define SPI_L2CAP       0x00020000  /* L2CAP Protocol */
#define SPI_SDP         0x00040000  /* SDP Protocol */
#define SPI_RFCOMM      0x00080000  /* RFCOMM Channel protocol */
#define SPI_OBEX        0x00100000  /* OBEX Protocol */
#define SPI_TCS         0x00200000  /* TCS Binary protocol */
#define SPI_DATA        0x00400000  /* Application data */
#define SPI_L2CAP_E     0x00800000  /* L2CAP Protocol in Enhanced mode */
#define SPI_SIM         0x01000000  /* SIM Access protocol */
#define SPI_AV          0x02000000  /* AV protocols */
#define SPI_AVDTP_SIG   0x02000001  /* AV protocols */
#define SPI_AVDTP_MED   0x02000002  /* AV protocols */
#define SPI_AVCTP       0x02000004  /* AV protocols */
#define SPI_AVCTP_BRWS  0x02000008  /* AV protocols */
#define SPI_BNEP        0x04000000  /* BNEP protocol */
#define SPI_A2MP        0x08000000  /* A2MP protocol */
#define SPI_L2CAP_EX    0x10000000  /* L2CAP Protocol in Extended mode */
#define SPI_ATT         0x20000000  /* ATT Protocol */
#define SPI_SMP         0x40000000  /* SMP Protocol */
#define SPI_GATT        0x80000000  /* GATT Protocol */
/* End of SniffProtocolId */

#if (XA_SNIFFER == XA_ENABLED) && (BT_PRESCAN == XA_ENABLED)

/*---------------------------------------------------------------------------
 * SnifferRegisterEndpoint()
 *
 *     This function is called by protocol implementations to register a
 *     new connection in the sniffer connection table. The registering
 *     protocol provides information about the local endpoint, remote
 *     device and the protocol that is running over that endpoint.
 *
 * Requires:
 *     XA_DEBUG enabled.
 *
 * Parameters:
 *     EndpointType - Protocol type of 'Endpoint'. Must be one of SPI_L2CAP
 *         or SPI_RFCOMM.
 *
 *     Endpoint - Identifier for the endpoint. Must be a pointer to an
 *         L2CAP L2capChannelId, or RFCOMM RfChannel->dlci.
 *
 *     RemDev - BtRemoteDevice pointer.
 *
 *     Protocol - Protocol layered above 'Endpoint'.
 */
void SnifferRegisterEndpoint(SniffProtocolId EndpointType, void *Endpoint, 
                             BtRemoteDevice *RemDev, SniffProtocolId Protocol);
#else
#define SnifferRegisterEndpoint(A,B,C,D)   (void)0
#endif /* (XA_SNIFFER == XA_ENABLED) && (BT_PRESCAN == XA_ENABLED) */


#if XA_DEBUG_PRINT == XA_ENABLED

/*
 *---------------------------------------------------------------------------
 *            TCS Debug functions
 *---------------------------------------------------------------------------
 */
#if defined(__TCS_H)
const char *pTCS_Event(TcsEvent tcs_event);
const char *pTCS_CallState(TcsCallState State);
#endif

#endif /* XA_DEBUG_PRINT */

#if defined(__TCS_H)
const char *pTCS_Message(TcsMsgType msg);
#endif

#endif /* __DEBUG_H */

