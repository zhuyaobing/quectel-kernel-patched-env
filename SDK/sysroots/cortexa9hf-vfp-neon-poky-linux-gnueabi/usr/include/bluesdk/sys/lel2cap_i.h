/***************************************************************************
 *
 * File:
 *     lel2cap_i.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains the defines, typedefs and function
 *     prototypes for functions internal to the LE L2CAP protocol.
 *
 * Created:
 *     January 22, 2014
 *
 * Copyright 1999-2017 OpenSynergy GmbH
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
 * OpenSynergy GmbH which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __LEL2CAP_I_H
#define __LEL2CAP_I_H

#if NUM_LE_DEVICES > 0
#include "lel2cap.h"
#include "sys/l2cap_i.h"
#include "me.h"
#include "sec.h"
#include "sys/evmxp.h"
#include "utils.h"
#include "sys/hci.h"
#include "sys/l2cap_i.h"
#include "sys/fixmgr.h"
#include "lesec.h"


/*---------------------------------------------------------------------------
 * LEL2CAP_SIGNAL_DEPTH constant
 *
 *     Number of non-channel based signaling requests that can be
 *     simultaneously handled. If the remote device exceeds this depth,
 *     non-channel requests may be silently discarded. Non-channel requests
 *     include connect reject responses. This parameter is
 *     per-device with a default of 2.
 */
#ifndef LEL2CAP_SIGNAL_DEPTH
#define LEL2CAP_SIGNAL_DEPTH      2
#endif

/* These defines are used to convert from ChannelId's to LEL2CAP_DynamicChannel 
 * structures.
 */
#define NULL_IDENTIFIER_LE_CID   0x0000
#define BASE_DYNAMIC_LE_CID      0x0040
#define LAST_DYNAMIC_LE_CID      0x007F

/* -- GetNextLeIdent Macro -----------------------------------------------------
 *
 * This macro returns the next available signaling identifier.
 */
#define GetNextLeIdent()   (LEL2C(nextIdent) == 0xFF ? \
    LEL2C(nextIdent)=1,0xFF : LEL2C(nextIdent)++)

/* -- ReturnLeSysPacket Macro  -------------------------------------------------
 *
 * Returns a system packet, allocated via LEL2Cap_GetSysPkt() to the system pool.
 */
void ReturnLeSysPkt(BtPacket *Packet);
#define ReturnLeSysPkt(_PKT) do {_PKT->headerLen = 0;_PKT->dataLen = 0;_PKT->flags = 0;\
                               InsertTailList(&LEL2C(signalPackets), &(_PKT)->node);   \
                               XASTAT_PeakDec(l2cap_num_signal_packets, 1);} while (0)

/* -- LE L2CAP Channel Flags ----------------------------------------------------
 *
 * Flags used in the 'flags' field of the L2CAP_BaseChannel structure.
 */
#define LECHF_CONNECTED           0x00000001ul  /* Channel is connected */
#define LECHF_SEND_REQUEST        0x00000002ul  /* Need to send a request (when a packet is available) */
#define LECHF_SEND_RESPONSE       0x00000004ul  /* Need to send a response (when a packet is available) */
#define LECHF_CONN_RSP_SENT       0x00000008ul  /* Connect Response sent or requested */
#define LECHF_LE_CREDIT_CHANNEL   0x00000010ul  /* This is temporary until channels are refactored */
#define LECHF_RCVD_FIRST_PDU      0x00000020ul  /* Received first PDU in the SDU */
#define LECHF_RTX_ACTIVE          0x00000040ul  /* RTX timer is active */
#define LECHF_SEC_PENDING         0x00000080ul  /* LESEC_AccessRequest() in progress */

/****************************************************************************
 *
 * Internal data structures
 *
 ****************************************************************************/
/* -- LEL2CAP_FixedChannel ----------------------------------------------------------
 *
 */
typedef struct _LEL2CAP_FixedChannel {
    L2CAP_BaseChannel   bc;
    void               *fixCtx;         /* Context of a fixed channel client */
    BtCallbackEx        callback;
} LEL2CAP_FixedChannel;

#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
/* -- LEL2CAP_DynamicChannel ----------------------------------------------------
 *
 * One per protocol binding between two devices. For example when 2 ACL links are
 * established two channels will be established for each protocol and therefore
 * there will be local 2 CID's. The CID values are dynamically allocated in the
 * range from 0x0040 to 0xFFFF. There is no Channel for the signaling channel(s).
 */
typedef struct _LEL2CAP_DynamicChannel {
    L2CAP_BaseChannel   bc;

    /* Channel oriented stuff */
    const LeL2capPsm   *psmInfo;        /* PSM hosting this channel */
    U16                 result;         /* Connect response "result" */
    LeL2capFlowParms    flowParms;
    U16                 txMps;          /* Max Tx PDU size */
    U16                 txCredits;      /* Credits available for transmit packets */
    U16                 rxCredits;      /* Credits available for receive packets */

    U16                 rxOffset;       /* Offset into the current received L2CAP SDU */
    U16                 rxSduSize;      /* Size of the current received L2CAP SDU */

    ListEntry           pendingData;    /* List of data packets waiting to be sent or
                                         * currently in the process of being sent.
                                         */

    U16                 remotePsm;      /* Remote PSM value to issue in the Connect Req */
    U16                 creditsToSend;  /* Pending credits to send when a packet is available */

    EvmTimer            timer;          /* RTX timer */

    LeSecurityToken     auth;           /* Passed to ME for access requests. */
} LEL2CAP_DynamicChannel;

#endif /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */

/* -- LE L2CAP Context Structure ------------------------------------------------
 *
 * This structure provides the context and state information for the entire
 * LE L2CAP module. The structure is global to all of the files in LE L2CAP.
 */
typedef struct _LeL2capContext {
    ListEntry                signalPackets;          /* Queue of packets used for signaling */
    /* Packet pool RAM */
    SignalPacket            _sigPacket[LEL2CAP_NUM_SIGNAL_PACKETS + LEL2CAP_SIGNAL_DEPTH];
    U8                       nextIdent;              /* Used for Signal Command Identification */

#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
    ListEntry                psmList;

    LEL2CAP_DynamicChannel   dynamicChannels[LEL2CAP_TOT_NUM_DYNAMIC_CHNLS];

    U16                      currRxDataLen;          /* Length of data pointed to by L2C(currRxData) */
    U8                      *currRxData;             /* Pointer to current location in Link RxPacket */

    ListEntry                txPktList;
    BtPacket                 txPackets[LEL2CAP_NUM_TX_PACKETS];
    U32                      needSysPkt;             /* Waiting for a free system packet */
#endif /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */
    
    LEL2CAP_FixedChannel     fixedChannels[LEL2CAP_TOT_NUM_FIXED_CHNLS];

    /* Fixed manager context */
    FixMgrLeContext          fixMgrLeCtx;

    /* LE ATT */
    ListEntry           leAttHandlers;
    L2capPsm            leAttSvrPsm;

} LeL2capContext;

/****************************************************************************
 *
 * Internal function prototypes
 *
 ****************************************************************************/
BtStatus LEL2CAP_Init(void);
void LEL2CAP_Deinit(void);
LEL2CAP_FixedChannel *LeL2Cap_AllocFixedChannel(BtLogicalLink *Llink, L2capChannelId FixedId);
void LeL2capInitLe(BtLogicalLink *Llink);
#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
LEL2CAP_DynamicChannel *LEL2Cap_AllocPsmChannel(BtRemoteDevice *RemDev, U16 Psm, U16 *Reason);
LEL2CAP_DynamicChannel *LeL2capAllocChannel(const LeL2capPsm *protocol);
#endif /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */

/* -- From lel2cap_if.c ----------------------------------------------------- */
void LEL2CAP_LinkEvent(LinkEvent *Event);
void LEL2CAP_HciTxDone(BtLogicalLink *Llink, BtPacket *Packet, BtStatus Status);
BOOL LEL2Cap_HandleRxError(BtLogicalLink *Llink, U16 Cid);
BOOL LeL2capIsPktOnTxQueue(BtPacket *Packet);

#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
void LeL2capReturnTxPkt(LEL2CAP_DynamicChannel *DChannel, BtPacket *Packet);
BtStatus LeL2capSendSegments(LEL2CAP_DynamicChannel *DChannel);
#endif /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */

/* -- From l2cap_sm.c ----------------------------------------------------- */
#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
void LEL2Cap_StateMachine(LEL2CAP_DynamicChannel *DChannel, I8 Event);
void LEL2Cap_ProcessReq(LEL2CAP_FixedChannel *fChannel, BtLogicalLink *Llink, U8 *RxData, U16 RxDataLen);
void LEL2Cap_ProcessRsp(LEL2CAP_FixedChannel *fChannel, U8 *RxData, U16 RxDataLen);
#endif /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */

/* -- From lel2cap_utl.c ---------------------------------------------------- */
L2CAP_BaseChannel *LEL2Cap_GetChannel(BtRemoteDevice *RemDev, L2capChannelId Cid);
BOOL LEL2Cap_ValidateCommand(const U8 *RxData, U16 RxLen);
void LEL2Cap_NotifyPsm(L2CAP_BaseChannel *Channel, L2capEventType Event, U16 Status);

#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
BOOL LEL2Cap_ValidateSourceCID(BtRemoteDevice *remDev, U16 cid, U16 *reason);
LEL2CAP_DynamicChannel *LEL2Cap_GetValidChannel(L2capChannelId Cid);
LEL2CAP_DynamicChannel *LEL2Cap_GetValidChannelByRemoteCid(L2capChannelId Cid);
LEL2CAP_DynamicChannel *LEL2Cap_FindChannelByLocalIdent(U8 Ident);
void LeL2CapStartRtx(LEL2CAP_DynamicChannel *DChannel);
void LeL2CapStopRtx(LEL2CAP_DynamicChannel *DChannel);
#endif /* LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0 */

L2CAP_BaseChannel *LEL2Cap_CheckForDuplicateRequest(U8 Ident, BtRemoteDevice *RemDev);
void LEL2Cap_RemoveChannel(L2CAP_BaseChannel *Channel, BtLogicalLink *Llink);
void LEL2Cap_FreeChannel(L2CAP_BaseChannel *Channel);
void LEL2Cap_NotifyUserAndCloseChannel(L2CAP_BaseChannel *Channel, U16 Status);
void LEL2Cap_FlushChannelTransmits(L2CAP_BaseChannel *Channel);
BtPacket *LEL2Cap_GetSysPkt(BOOL Priority);
#endif /* NUM_LE_DEVICES > 0 */

#endif /* __LEL2CAP_I_H */
