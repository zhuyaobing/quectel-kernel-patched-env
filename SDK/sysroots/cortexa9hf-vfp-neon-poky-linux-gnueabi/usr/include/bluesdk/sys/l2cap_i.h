/***************************************************************************
 *
 * File:
 *     l2cap_i.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file contains the defines, typedefs and function
 *     prototypes for functions internal to the L2CAP protocol.
 *
 * Created:
 *     July 15, 1999
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

#ifndef __L2CAP_I_H
#define __L2CAP_I_H

#include "l2cap.h"
#include "me.h"
#include "sec.h"
#include "sys/evmxp.h"
#include "utils.h"
#include "sys/hci.h"
#include "l2capagent.h"
#include "fixmgr.h"


/*---------------------------------------------------------------------------
 * BtLinkRxState type
 *
 *      Link Receive Buffer State is used to track the L2CAP
 *      receive buffer state.
 */
typedef U8 BtLinkRxState;

#define BRXS_COMPLETE      0x00
#define BRXS_COPYING       0x01
#define BRXS_FORWARDING    0x02
#define BRXS_FLUSH         0x03
#define BRXS_BROADCAST     0x04
#define BRXS_DISCONNECTING 0x05

/* End of BtLinkState */

/* -- L2SIG_DATA_LEN --------------------------------------------------------
 *
 * This size is used to allocate the data area for L2CAP signaling packets.
 * Both the signal packet data area and spare BtPacket header area (8 bytes)
 * are used when building signaling commands and responses. So the effective
 * size is 8 bytes larger than these defines.
 *
 * Maximum: Configure Req(8) + MTU(4) + Flow Control(11) + FCS Option (3) +
 * EFS(18) == 44
 */
#if ((L2CAP_EXTENDED_FEATURES) & L2EXTENDED_FEATURE_EXT_FLOW_SPEC) | (NUM_AMPS > 0)
#define L2SIG_DATA_LEN      668
#else
#define L2SIG_DATA_LEN      44
#endif

/* -- L2C_WORKSPACE_LEN -----------------------------------------------------
 *
 * This size is used to allocate a channel workspace used to store rejected
 * or confirmed configuration options. It is based on the size of MTUsig.
 * A config request contains 8 octets so the configuration parameters are
 * MTUsig - 8.
 *
 */
#if ((L2CAP_EXTENDED_FEATURES) & L2EXTENDED_FEATURE_EXT_FLOW_SPEC) | (NUM_AMPS > 0)
#define L2C_WORKSPACE_LEN   664
#else
#define L2C_WORKSPACE_LEN   40 
#endif

/* Forward Type for L2CAP Channels */
typedef struct _L2CAP_BaseChannel L2CAP_BaseChannel;
typedef struct _L2CAP_Channel L2CAP_Channel;
typedef struct _L2CAP_DynamicChannel L2CAP_DynamicChannel;
typedef struct _L2CAP_FixedChannel L2CAP_FixedChannel;


/* -- General Defines -------------------------------------------------------
 *
 */
#define L2CAP_HEADER_MIN         4  /* Size in Bytes of minimum L2CAP Header */
#define SIGNAL_HEADER_LEN        4  /* Size in Bytes of a Signal Header */
#define L2CAP_CONS_HEADER_LEN    4  /* Size of L2CAP Connection Oriented header. */
#define L2CAP_HEADER_MAX         8  /* Size in Bytes of maximum L2CAP Header */
#define L2CAP_ECHO_MAX           253
#if ((L2CAP_EXTENDED_FEATURES) & L2EXTENDED_FEATURE_EXT_WINDOW_SIZE)
#define L2CAP_EXTENDED_CONTROL_FIELD    XA_ENABLED
#else
#define L2CAP_EXTENDED_CONTROL_FIELD    XA_DISABLED
#endif


#if L2CAP_EXTENDED_CONTROL_FIELD != XA_ENABLED
#define L2E_MAX_WINDOW      0x40
#else /* L2CAP_EXTENDED_CONTROL_FIELD != XA_ENABLED */
#define L2E_MAX_WINDOW      0x4000
#if BT_PACKET_HEADER_LEN < 18
#error BT_PACKET_HEADER_LEN must be 18 or greater for Extended_Window_Size
#endif
#endif /* L2CAP_EXTENDED_CONTROL_FIELD != XA_ENABLED */

/*---------------------------------------------------------------------------
 * L2capAckMode type
 *
 */
typedef U16 L2capAckMode;

/* Ack Disable */
#define L2ACK_DISABLE       0x00

/* Ack Enable */
#define L2ACK_ENABLE        0x01

/* Ack Immediately */
#define L2ACK_IMMEDIATE     0x02

/* Ack Nomally with internal algorithm */
#define L2ACK_NORMAL        0x04

/* End of L2capAckMode */

/*---------------------------------------------------------------------------
 * L2capConfigMode type
 *
 */
typedef U8 L2capConfigMode;

/* Lockstep config */
#define L2CFG_LOCKSTEP      0x00

/* CSA1 config */
#define L2CFG_CSA1          0x01

/* End of L2capConfigMode */

/*---------------------------------------------------------------------------
 * L2capSarState type
 *
 *   ERTM Segmentation and Reasembly state.
 */
typedef U8 L2capSarState;

/* SAR Unsegmented state */
#define L2SAR_UNSEGMENTED   0x00

/* SAR Start state */
#define L2SAR_START         0x01

/* SAR End state */
#define L2SAR_END           0x02

/* SAR Continue state */
#define L2SAR_CONTINUE      0x03

/* End of L2capSarState */

/*---------------------------------------------------------------------------
 * L2capChannelType type
 *
 */
typedef U8 L2capChannelType;

/* Dynamic Channel */
#define L2CCT_DYNAMIC       0x00

 /* Fixed Channel */
#define L2CCT_FIXED         0x01

/* LE dynamic channel */
#define L2CCT_LE_DYNAMIC    0x80

/* LE fixed channel */
#define L2CCT_LE_FIXED      0x81

/* Idenifies if the channel type is LE or not */
#define L2CCT_LE_TYPE       0x80

/* End of L2capChannelType */

typedef void (*L2capErtmFsmFunc)(L2CAP_Channel *Channel, I8 Event);

typedef struct _TxEntry {
    BtPacket   *pkt;
    BtPacket   *sdu;
} TxEntry;

typedef struct _RxEntry {
    ListEntry   node;
    U8          flags;
    U8          sar; 
    U16         segLen;
    U16         sduLen;
    ListEntry   hciBuffList;
} RxEntry;

/* -- L2CAP_Enhanced ----------------------------------------------------------
 *
 * Allocated to the L2CAP_Channel structure when the channel has a link mode
 * of type L2MODE_ERTM or L2MODE_STREAMING.
 */
typedef struct _L2CAP_Enhanced {
    BOOL                inUse;

    L2capErtmFsmFunc    xmitState;
    U16                 maxTxMps;       /* Maximum sized Tx-sdu-segment (MPS) for channel */
    U16                 txMps;          /* Current Tx-sdu-segment (MPS) for channel */
    U16                 remRxWindow;    /* Tx Window in Remote device's config req (its RX window) */
    U16                 remTxWindow;    /* Tx Window in Remote device's config rsp (its TX window) */
    U16                 maxTransmit;    /* From configuration option */
    U16                 nextTxSeq;
    U16                 expectAckSeq;
    ListEntry           pendingData;
    U16                 unackedFrames;
    U16                 framesSent;
    TxEntry             unackedList[L2E_MAX_WINDOW];
    ListEntry           movePktList;
    I32                 movePktCount;

    L2capErtmFsmFunc    recvState;
    U16                 rxMps;          /* Maximum sized Rx-sdu-segment (MPS) for channel */
    U16                 rxWindow;       /* Window advertised to sending peer */
    U16                 txWindow;       /* Local devices txWindow which may be smaller than remote device's rxWindow. */
    U8                 *rxHeader;
    U16                 rxSduLen;
    U16                 realRxWindow;
    RxEntry             sFrame;
    RxEntry             iFrame[L2E_MAX_WINDOW];
    RxEntry             errFrame;
    U16                 txSeqRx;
    U16                 reqSeqRx;
    U16                 expectTxSeq;
    U16                 bufferSeq;
    BOOL                fBitSet;
    U16                 more;
    BOOL                streamingException;

    U16                 retransTimeout; /* From configuration option */
    U16                 monitorTimeout; /* From configuration option */
    EvmTimer            retransTimer;
    EvmTimer            monitorTimer;
    U16                 ackTimeout;
    EvmTimer            ackTimer;
    U16                 lastAckSeq;

    BOOL                rnrSent;
    BOOL                remoteBusy;
    BOOL                localBusy;
    U8                  retryCount;
    U16                 retryIframes[L2E_MAX_WINDOW];
    BOOL                rejActioned;
    BOOL                sendRej;
    U16                 fBitActive;
    U8                  fcsOption;

    BOOL                srejActioned;
    U8                  srejSaveReqSeq;
#if L2CAP_ERTM_SREJ_ENABLE == XA_ENABLED
    U8                  srejListCount;
    ListEntry           srejList;
    U8                  bufferSeqSrej;
#endif /* L2CAP_ERTM_SREJ_ENABLE == XA_ENABLED */

    U16                 dropSeq;        /* For testing only */
    U8                  dropCount;      /* For testing only */
    U16                 sduBuffLen;
    HciBuffer          *sduHciBuff;
    U8                  firstOff;
    EvmTask             txTask;
    U32                 totalData;
    
    /* For allocating a block of hci buffers */
    HciBufferBlock      hciBuffBlock;
    U8                  multiplier;  /* The multiplier that was used. */

} L2CAP_Enhanced;


/* -- SignalPacket ----------------------------------------------------------
 *
 * Used by L2CAP for sending signaling packets. A combination of a BtPacket
 * and the workspace necessary for building signaling commands.
 */
typedef struct _SignalPacket {
    BtPacket    packet;
    U8          workspace[L2SIG_DATA_LEN];
} SignalPacket;


/* -- L2CAP_BaseChannel ----------------------------------------------------------
 *
 */
struct _L2CAP_BaseChannel {
    ListEntry           node;
    BtLogicalLink      *lLink;          /* Logical Link */
    BtRemoteDevice     *remDev;
    U32                 flags;
    L2capChannelId      localCid;       /* Local Channel Id (>= 0x0040) */
    L2capChannelId      remoteCid;      /* Remote devices channel id */
    U16                 txMtu;          /* Max Tx SDU size */
    U16                 inUse;          /* Number of signal + data packets outstanding on channel */
    L2capChannelType    type;
    BtPriority          priority;
    U8                  state;          /* L2CAP Channel Connection State */
    U8                  linkMode;       /* Uses LINK_MODE_XXX defines. */
    U8                  localIdent;     /* Identifier sent in signaling request */
    U8                  remoteIdent;    /* Identifier recv'd in signaling req */
};

/* -- L2CAP_Channel ----------------------------------------------------------
 *
 * One per protocol binding between two devices. For example when 2 ACL links are
 * established two channels will be established for each protocol and therefore
 * there will be local 2 CID's. The CID values are dynamically allocated in the
 * range from 0x0040 to 0xFFFF. There is no Channel for the signaling channel(s).
 */
struct _L2CAP_Channel {
    L2CAP_BaseChannel   bc;             /* Inherit */

    EvmTimer            timer;          /* RTX/ERTX */

    ListEntry           lnode;          /* For Lockstep Admission control. */
    ListEntry           hnode;          /* For HCI commands. */
    ListEntry           pendPkts;       /* Packets waiting for info requests to complete. */

    L2capAgent         *agent;

    /* Channel oriented stuff */
    const L2capPsm     *psmInfo;        /* PSM hosting this channel */
    L2capFlowSpec      *flowSpec;
    U32                 flags2;
    U16                 txFlushTimeout;
    U16                 rxFlushTimeout;

    U16                 rxMtu;          /* Max Rx SDU size (MTU) */
    U16                 remotePsm;      /* Remote PSM value */
    U8                  configMode;     /* Indicate config mode. */


#if L2CAP_NUM_ENHANCED_CHANNELS > 0 
    L2CAP_Enhanced     *eCh;
    L2capFlowParms      flowParms;      /* Flow control parms for channel */ 
#endif

};


/* -- L2CAP_DynamicChannel ----------------------------------------------------------
 *
 * One per protocol binding between two devices. For example when 2 ACL links are
 * established two channels will be established for each protocol and therefore
 * there will be local 2 CID's. The CID values are dynamically allocated in the
 * range from 0x0040 to 0xFFFF. There is no Channel for the signaling channel(s).
 */
struct _L2CAP_DynamicChannel {
    L2CAP_Channel       c;
    BtSecurityToken     authorization;  /* Passed to ME for access requests. */
    EvmTask             task;           /* General purpose task structure. */

    void               *disconnectCtx;

    U8                  workspace[L2C_WORKSPACE_LEN]; /* Holder for rejected config options. */
    U16                 wsLen;          /* Length of options in workspace */

    U16                 negotiatedMtu;
    U16                 qosMps;         /* QoS MPS */
    U16                 result;         /* Configure/Connect response "result" */
    U16                 inResult;

    BOOL                incomingAgent;  /* TRUE if agent has been allocated by L2CAP */
    U8                  numConfigs;     /* Number of configuration requests. */
    U8                  remoteLinkMode; /* Link Mode requested by remote. */


#if L2CAP_NUM_ENHANCED_CHANNELS > 0 
    ExtFlowSpecPair     efsPair;
    U8                  lockstepState;
    EvmTimer            admissionTimer;
#endif

#if NUM_AMPS > 0 
    L2CAP_DynamicChannel *clone;
    BOOL                  oldLogDisc;     /* TRUE if old logical link was disconnected during move */
    BtLogicalLink        *oldLlink;       /* Old Logical Link. */
#endif
};

/* -- L2CAP_FixedChannel ----------------------------------------------------------
 */
struct _L2CAP_FixedChannel {
    L2CAP_Channel       c;
    void               *fixCtx;         /* Context of a fixed channel client */
};

/* Misc ERTM bit masks. */
#define L2E_REQSEQ_MASK             0x3f
#define L2E_P_BIT_MASK              0x10
#define L2E_F_BIT_MASK              0x80

/*---------------------------------------------------------------------------
 * L2capSuperType type
 *
 *    ERTM Supervisory Packet types (also the first 4 bits of the S-frame)
 */
typedef U8 L2capSuperType;

#define L2SUPERVISORY_RR            0x00 /* bin: 0000 - Send a Receiver-Ready S-frame */
#define L2SUPERVISORY_REJ           0x01 /* bin: 0100 - Send a Reject S-frame */
#define L2SUPERVISORY_RNR           0x02 /* bin: 1000 - Send a Receiver-Not-Ready S-frame */
#define L2SUPERVISORY_SREJ          0x03 /* bin: 1100 - Send a S-Reject S-frame */
#define L2SUPERVISORY_ACK           0x0f /* Send an ACK (I-frame or S-frame) */
#define L2SUPERVISORY_MASK          0x0f /* Mask of supervisory bits */

/* End of L2capSuperType */

/*---------------------------------------------------------------------------
 * L2capErtmEvent type
 *
 *    ERTM events
 */
typedef U8 L2capErtmEvent;

#define LLCE_DATA_REQUEST           0x20
#define LLCE_LOCAL_BUSY_DET         0x21
#define LLCE_LOCAL_BUSY_CLR         0x22
#define LLCE_RCV_REQSEQ_AND_FBIT    0x23
#define LLCE_RCV_FBIT               0x24
#define LLCE_RETRANS_EXPIRED        0x25
#define LLCE_MONITOR_EXPIRED        0x26
#define LLCE_RCV_I_FRAME            0x27
#define LLCE_RCV_RR                 0x28
#define LLCE_RCV_RNR                0x29
#define LLCE_RCV_REJ                0x2A
#define LLCE_RCV_SREJ               0x2B
#define LLCE_MANUAL_ACK             0x30 /* For testing */

/* End of L2capErtmEvent */


/* -- L2CAP Channel Flags ----------------------------------------------------
 *
 * Flags used in the 'flags' field of the L2CAP_Channel structure.
 */
#define L2CHF_SEC_PENDING         0x00000001ul  /* SEC_AccessRequest() in progress */
#define L2CHF_CONNECTED           0x00000002ul  /* Channel is connected */
#define L2CHF_RTX_ACTIVE          0x00000004ul
#define L2CHF_ERTX_ACTIVE         0x00000008ul
#define L2CHF_OUT_CONFIG_OK       0x00000010ul  /* Local Config Request was accepted */
#define L2CHF_IN_CONFIG_OK        0x00000020ul  /* Remote Config Request was accepted */
#define L2CHF_CONFIG_REQ_SENT     0x00000040ul  /* Config Req Sent */
#define L2CHF_MORE_CONFIG         0x00000080ul  /* Received Config Req had More bit set */
#define L2CHF_CONFIG_RSP_SENT     0x00000100ul  /* Config Rsp Sent */
#define L2CHF_CONN_RSP_SENT       0x00000200ul  /* Connect Response sent or requested */
#define L2CHF_NOT_VISIBLE         0x00000400ul  /* App does not know of channel yet */
#define L2CHF_AUTO_CONFIG         0x00000800ul  /* Handle Config Internally */
#define L2CHF_SEND_REQUEST        0x00001000ul  /* Need to send a request (when a packet is available) */
#define L2CHF_SEND_RESPONSE       0x00002000ul  /* Need to send a response (when a packet is available) */
#define L2CHF_NON_FLUSH_PBF       0x00004000ul  /* Indicates default state of Non-Flushable Packet Boundry Flag */
#define L2CHF_TIMER_RESET         0x00008000ul  /* Timer has been reset at least once. */
#define L2CHF_SET_TX_MPS          0x00010000ul  /* Change the channel TX MPS. */
#define L2CHF_SEND_FLOW_SPEC      0x00020000ul  /* Send an HCI flow spec command. */
#define L2CHF_USE_NEGOTIATED_MTU  0x00040000ul  /* Use the "fake" negotiated MTU. */
#define L2CHF_SEND_FLUSH_TIMEOUT  0x00080000ul  /* Send a flush timeout. */
#define L2CHF_MOVE_IN_PROG        0x00100000ul  /* Channel Move is in progress. */
#define L2CHF_SEND_PKTS           0x00200000ul  /* Send Queued Packets. */
#define L2CHF_Q_CONN_REQ          0x00300000ul  /* Queued conn request */
#define L2CHF_Q_CONN_RSP          0x00400000ul  /* Queued conn response */
#define L2CHF_Q_CREATE_REQ        0x01000000ul  /* Queued create request */
#define L2CHF_Q_CREATE_RSP        0x02000000ul  /* Queued create response */
#define L2CHF_RETRANSMIT          0x04000000ul  /* Retransmit I-frames after Move completes. */
#define L2CHF_BEGIN_MOVE          0x08000000ul  /* Begin Channel Move. */
#define L2CHF_MOVE_WAIT_ACK       0x10000000ul
#define L2CHF_SEC_PENDING_TX      0x20000000ul  /* SEC_AccessRequest() in progress */

/*
 * Flags used in the 'flags2' field of the L2CAP_Channel structure.
 */
#define L2CHF2_FEATURES_REQ       0x00000001ul  /* Requesting remote features */
#define L2CHF2_FOUND_FC_REQ       0x00000002ul  /* Found Flow Control Option in config req */
#define L2CHF2_FOUND_FC_RSP       0x00000004ul  /* Found Flow Control Option in config rsp */
#define L2CHF2_DISABLE_ACK        0x00000008ul  /* Disable Auto Ack */
#define L2CHF2_SET_F_BIT          0x00000010ul
#define L2CHF2_SET_P_BIT          0x00000020ul
#define L2CHF2_CLIENT_ROLE        0x00000040ul
#define L2CHF2_INDICATING         0x00000080ul
#define L2CHF2_DATA_REQUEST       0x00000100ul
#define L2CHF2_SET_BUSY           0x00000200ul
#define L2CHF2_CLEAR_BUSY         0x00000400ul
#define L2CHF2_FREE_CHANNEL       0x00000800ul
#define L2CHF2_EXTENDED_WINDOW    0x00001000ul
#define L2CHF2_FIXED_CHNLS_REQ    0x00002000ul  /* Requesting remote fixed channels */
#define L2CHF2_INITIATOR_ROLE     0x00004000ul
#define L2CHF2_INITIATOR_DONE     0x00008000ul
#define L2CHF2_RESPONDER_DONE     0x00010000ul
#define L2CHF2_SEND_MV_CNF        0x00020000ul
#define L2CHF2_SEND_MV_RSP        0x00040000ul
#define L2CHF2_WAIT_MV_CNF_RSP_TX 0x00080000ul
#define L2CHF2_UNRELIABLE_DATA    0x00100000ul
#define L2CHF2_LE_CREDIT_CHANNEL  0x00200000ul  /* This is temporary until channels are refactored */

/* -- L2CAP States -----------------------------------------------------------
 *
 * The following states are used by the L2CAP_Channel 'state' field.
 */
/* CLOSED: No channel associated with this CID */
#define L2C_CLOSED          0

/* CLOSING: The channel is releasing all resources before it indicates
 * a disconnect to the upper layer.
 */
#define L2C_CLOSING         1

/* W4_ACL_INIT: The ACL is up. Requesting features and fixed channel mask if available. */
#define L2C_W4_ACL_INIT     2

/* W4_CONN_CNF: An L2CAP_ConnectReq has been sent, waiting for a L2CAP_ConnectRsp. */
#define L2C_W4_CONN_CNF     3

/* W4_CONN_RSP: An L2CA_ConnectInd has been indicated to the upper layer.
 *              Waiting for the upper layer to send the response.
 */
#define L2C_W4_CONN_RSP     4

/** W4_CREATE_CNF: a channel creation request has been received, but only
 *  a response with indication "pending" can be sent. This state is
 *  similar to WAIT_CONNECT.
 */
#define L2C_W4_CREATE_CNF   5

/** W4_CREATE_RSP: a channel creation request has been sent, pending a
 *  channel creation response. This state is similar to WAIT_CONNECT_RSP.
 */
#define L2C_W4_CREATE_RSP   6

/* CONFIG: Both sides are negotiating the channel configuration parameters. */
#define L2C_CONFIG          7

/* CONFIG: Both sides are negotiating the channel configuration parameters. */
#define L2C_LOCKSTEP_CONFIG 8

/* OPEN: The connection is established and configured, and data may flow. */
#define L2C_OPEN            9

/* W4_DISC_CNF: An L2CAP_DisconnectReq has been sent, waiting for a L2CAP_DisconnectRsp. */
#define L2C_W4_DISC_CNF     10

/* W4_DISC_RSP: An L2CA_DisconnectInd has been indicated to the upper layer.
 *              waiting for the upper layer to send the response.
 */
#define L2C_W4_DISC_RSP     11

/** W4_MOVE_REQ: a request to move the current channel to another
 *  Controller has been received, but only a response with indication
 *  "pending" can be sent.
 */
#define L2C_W4_MOVE_REQ     12

/** W4_MOVE_RSP: a request to move a channel to another Controller has
 *  been sent, pending a move response.
 */
#define L2C_W4_MOVE_RSP     13

/** W4_MOVE_CNF: a response to the move channel request has been sent,
 *  waiting for a confirmation of the move operation by the initiator
 *  side.
 */
#define L2C_W4_MOVE_CNF     14

/** W4_CONFIRM_RSP: a move channel confirm has been sent, waiting for a
 *  move channel confirm response.
 */
#define L2C_W4_MOVE_CNF_RSP 15

/* W4_ECHO_CNF: Special state for echo client, waiting for L2CAP_EchoRsp. */
#define L2C_W4_ECHO_CNF     16

/* W4_INFO_CNF: Special state for info client, waiting for L2CAP_InfoRsp. */
#define L2C_W4_INFO_CNF     17

/* -- L2CAP Command-Reject Reasons -----------------------------------------
 *
 */
typedef U16 L2capCommandRejectReason;

#define L2C_CMD_REJ_REASON_COMMAND  0x0000 /* Command not understood */
#define L2C_CMD_REJ_REASON_MTU      0x0001 /* Signaling MTU exceeded */
#define L2C_CMD_REJ_REASON_CID      0x0002 /* Invalid CID */

/* End of L2capCommandRejectReason */

/* -- L2CAP Info-Response Results -----------------------------------------
 *
 */
typedef U16 L2capInfoResponseResult;
#define L2C_INFO_RSP_RESULT_SUCCESS       0x0000
#define L2C_INFO_RSP_RESULT_NOT_SUPPORTED 0x0001

/* End of L2capInfoResponseResult */

/* -- L2CAP Pending Connection Status -------------------------------------
 *
 */
typedef U16 L2capConnPendingStatus;
#define L2C_CONN_PEND_STATUS_UNKNOWN         0x0000
#define L2C_CONN_PEND_STATUS_AUTHENTICATION  0x0001
#define L2C_CONN_PEND_STATUS_AUTHORIZATION   0x0002

/* End of L2capConnPendingStatus */

/* -- L2CAP Lockstep States -------------------------------------------------
 *
 * The following states are used by the L2CAP_Channel 'lockstepState' field.
 */
typedef U8 L2capLockStepState;

#define L2C_LOCKSTEP_REQ_SENT   0x01
#define L2C_LOCKSTEP_REQ_RECV   0x02
#define L2C_LOCKSTEP_PEND_RECV  0x04
#define L2C_LOCKSTEP_PENDING    0x08
#define L2C_LOCKSTEP_ADMIT      0x10
#define L2C_LOCKSTEP_STATUS     0x20
#define L2C_LOCKSTEP_COMPLETE   0x40

/* End of L2capLockStepState */

/* -- L2CAP Move Response Result codes --------------------------------------
 *
 */
typedef U16 L2capMoveRspStatus;

#define L2C_MOVE_SUCCESS        0x0000
#define L2C_MOVE_PENDING        0x0001
#define L2C_MOVE_REFUSED_ID     0x0002
#define L2C_MOVE_REFUSED_SAME   0x0003
#define L2C_MOVE_REFUSED_CONF   0x0004
#define L2C_MOVE_REFUSED_COLL   0x0005
#define L2C_MOVE_REFUSED_NA     0x0006

/* End of L2capMoveRspStatus */

/* -- L2CAP Move Confirm Result codes --------------------------------------
 *
 */
typedef U16 L2capMoveCnfStatus;

#define L2C_MOVE_CNF_SUCCESS    0x0000
#define L2C_MOVE_CNF_FAILURE    0x0001

/* End of L2capMoveCnfStatus */

/*---------------------------------------------------------------------------
 * L2CAP_DEFAULT_FLUSH_TIMEOUT constant
 *
 *     Defines the maximum time (in milliseconds) that the Bluetooth
 *     hardware will attempt to send L2CAP data before giving up. Data
 *     packets that cannot be transmitted within this time period are
 *     "flushed". The default value of 0xFFFF indicates that the packets
 *     should be retransmitted indefinitely and never be flushed.
 */
#define L2CAP_DEFAULT_FLUSH_TIMEOUT 0xFFFFFFFF

/*---------------------------------------------------------------------------
 * L2CAP_SIGNAL_DEPTH constant
 *
 *     Number of non-channel based signaling requests that can be
 *     simultaneously handled. If the remote device exceeds this depth,
 *     non-channel requests may be silently discarded. Non-channel requests
 *     include echo, info and connect reject responses. This parameter is
 *     per-device with a default of 2.
 */
#ifndef L2CAP_SIGNAL_DEPTH
#define L2CAP_SIGNAL_DEPTH      2
#endif

/*---------------------------------------------------------------------------
 * L2CAP_TOT_NUM_E_CHNLS constant
 *
 */
#ifndef L2CAP_TOT_NUM_E_CHNLS
#if NUM_AMPS > 0
/* Add one Enhanced Channel for the A2MP fixed channel. */
#define L2CAP_TOT_NUM_E_CHNLS   (L2CAP_NUM_ENHANCED_CHANNELS + 1)
#else /* NUM_AMPS > 0 */
#define L2CAP_TOT_NUM_E_CHNLS   (L2CAP_NUM_ENHANCED_CHANNELS)
#endif /* NUM_AMPS > 0 */
#endif

/*---------------------------------------------------------------------------
 * L2CAP_TOT_NUM_FIXED_CHNLS constant
 *
 */
#ifndef L2CAP_TOT_NUM_FIXED_CHNLS
/* Need fixed channel memory for each device. */
#define L2CAP_TOT_NUM_FIXED_CHNLS   (L2CAP_NUM_BR_FIXED_CHANNELS * NUM_BT_DEVICES)
#endif



/* -- GetNextIdent Macro -----------------------------------------------------
 *
 * This macro returns the next available signaling identifier.
 */
#define GetNextIdent()   (L2C(nextIdent) == 0xFF ? \
    L2C(nextIdent)=1,0xFF : L2C(nextIdent)++)

/* -- L2CAP Channel Id Macros ------------------------------------------------
 *
 * These macros and defines are used to convert from ChannelId's to
 * L2CAP_Channel structures.
 */
#define NULL_IDENTIFIER_CID      0x0000
#define BASE_DYNAMIC_CID         0x0040
#define LAST_DYNAMIC_CID         (BASE_DYNAMIC_CID + L2CAP_NUM_CHANNELS - 1)


#if L2CAP_NUM_ENHANCED_CHANNELS > 0

struct _TxBufferBlock
{    
    U16             size;      /* Number of BtPackets in the block */    
    /*L2CAP_Channel   *channel;  */  /* the channel that has reserved this block, 0 if not reserved by any */
    L2CAP_Enhanced  *eCh;    /* The pointer that is used as a channel identifier for reservations */
    
};

typedef struct _TxBufferBlock TxBufferBlock;

#endif



/* -- L2CAP Context Structure ------------------------------------------------
 *
 * This structure provides the context and state information for the entire
 * L2CAP module. The structure is global to all of the files in L2CAP.
 */
typedef struct _BtL2CapContext {
    L2capPsm            *protocols[L2CAP_NUM_PROTOCOLS+L2CAP_NUM_BR_FIXED_CHANNELS];
    L2CAP_DynamicChannel dynamicChannels[L2CAP_NUM_CHANNELS];
    L2CAP_FixedChannel   fixedChannels[L2CAP_TOT_NUM_FIXED_CHNLS];

#if L2CAP_NUM_ENHANCED_CHANNELS > 0
    L2CAP_Enhanced      eChannels[L2CAP_TOT_NUM_E_CHNLS];
#endif /* L2CAP_NUM_ENHANCED_CHANNELS > 0 */
    ListEntry           signalPackets;          /* Queue of packets used for signaling */
    U32                 flags;                  /* L2CAP Module flags */
    U8                  nextIdent;              /* Used for Signal Command Identification */
    U8                  lastRemoteIdent;        /* Used to age remote idents */
    U32                 extendedFeaturesBase;   /* The base extended features set to L2CAP_EXTENDED_FEATURES */
    U32                 extendedFeatures;       /* Working Extended Features for Info Response */
    U16                 currRxDataLen;          /* length of data pointed to by L2C(currRxData) */
    U8                 *currRxData;             /* pointer to current location in Link RxPacket */
    L2capCallbackParms  callbackParms;          /* Callback information */
    BtHandlerEx         legacyHandler;          /* Compatible API */
    EvmTimer            dataTimer;
    L2CAP_Channel      *denied;

    HciCommand          hciCmd;
    ListEntry           pendHciCmd;

    /* Packet pool RAM */
    SignalPacket        _sigPacket[L2CAP_NUM_SIGNAL_PACKETS + L2CAP_SIGNAL_DEPTH];

#if L2CAP_NUM_ENHANCED_CHANNELS > 0
    ListEntry           txPktList;
    BtPacket            txPackets[L2CAP_NUM_TX_PACKETS];
    /* Each enhanced channel is guaranteed to get 1 buffer. They come from the numChannelsReserved reserve and   
     * the maximum number of blocks in this reserve is L2CAP_NUM_ENHANCED_CHANNELS 
     */
    U32                 numChannelsReserved;  
    /* additional buffers that enhanced L2CAP channels can reserve. The number of such buffers
     * is L2CAP_NUM_TX_PACKETS - L2CAP_NUM_ENHANCED_CHANNELS  
     */
    U32                 numAuxBlocksUsed;  
    /* block reservation table for TxPkts */
    TxBufferBlock       blockTable[L2CAP_NUM_ENHANCED_CHANNELS]; 

#endif /* L2CAP_NUM_ENHANCED_CHANNELS > 0 */

    BtPacket           *infoPkt;
    BtHandlerEx         linkHandler;
#if XA_DEBUG == XA_ENABLED
    BOOL                manualMode;
#endif
    ListEntry           pendQueue;              /* Channels with pending packets. */

    /* Fixed manager context */
    FixMgrBrContext     fixMgrBrCtx;

    /* Connectionless */
    ListEntry           clHandlers;

} BtL2CapContext;


/* -- L2CAP Context Flags ----------------------------------------------------
 *
 * Flags global to the L2CAP or RemDev Context.
 *
 */
#define L2CCF_BROADCASTS_ENABLED  0x00000001ul
#define L2CCF_NEED_SYSPACKET      0x00000002ul
#define L2CCF_RESCHEDULE          0x00000004ul
#define L2CCF_SEND_FLUSH_TIMEOUT  0x00000008ul
#define L2CCF_FEATURES_REQ        0x00000010ul
#define L2CCF_FIXED_CHNLS_REQ     0x00000020ul
#define L2CCF_INFO_COMPLETE       0x00000040ul
#define L2CCF_CHECK_BR_SEC        0x00000080ul

/* -- L2CAP Signal Codes -----------------------------------------------------
 *
 * The following defines are used as events to the L2CAP state machine. The
 * codes 0x01 through 0x0B match the opcodes of the respective L2CAP signaling
 * commands. The remainder of the defines are for events used internally.
 */
typedef U8 L2capSignalCode;
#define LLC_COMMAND_REJ     0x01
#define LLC_CONN_REQ        0x02
#define LLC_CONN_RSP        0x03
#define LLC_CONFIG_REQ      0x04
#define LLC_CONFIG_RSP      0x05
#define LLC_DISC_REQ        0x06
#define LLC_DISC_RSP        0x07
#define LLC_ECHO_REQ        0x08
#define LLC_ECHO_RSP        0x09
#define LLC_INFO_REQ        0x0A
#define LLC_INFO_RSP        0x0B
#define LLC_CREATE_REQ      0x0C
#define LLC_CREATE_RSP      0x0D
#define LLC_MOVE_REQ        0x0E
#define LLC_MOVE_RSP        0x0F
#define LLC_MOVE_CNF        0x10
#define LLC_MOVE_CNF_RSP    0x11

/* Last command/response that can run over BR/EDR
 * L2CAP signaling channel. This must be kept
 * up to date when new command/responses are added.
 */
#define LLC_LAST_BR_CMD     0x11

/* The following events are only used internally */
#define LLC_RTX_EXPIRED     0x20    /* RTX timer expired */
#define LLC_ERTX_EXPIRED    0x21    /* Extended RTX timer expired */
#define LLC_TX_DONE         0x22    /* Transmit done */
#define LLC_ACCESS_RSP      0x23    /* + response to SEC_AccessRequest() */
#define LLC_ACCESS_REJ      0x24    /* - response to SEC_AccessRequest() */
#define LLC_DISCONNECT_IND  0x25    /* ACL Link disconnected */
#define LLC_QOS_VIOL        0x26    /* QoS Violation occurred */
#define LLC_SEND_PACKET     0x27    /* Indicates that a transmit packet is available. */
#define LLC_RETRANS_EXPIRED 0x28    /* Segment retransmit timer expired */
#define LLC_MONITOR_EXPIRED 0x29    /* Monitor timer expired */
#define LLC_LAST_VALUE      0x29

/* End of L2capSignalCode */

#define CONFIG_OPTION_HINT      0x80

/* -- L2CAP Link Modes -------------------------------------------------------
 *
 * The link mode settings used during configuration negotiation. Note that
 * these values must correlate with the L2capLinkMode values used by the API
 * as follows: L2MODE_XXX = (1 << LINK_MODE_XXX)
 */
#define LINK_MODE_BASIC             0
#define LINK_MODE_RETRANSMISSION    1
#define LINK_MODE_FLOW_CONTROL      2
#define LINK_MODE_ERTM              3
#define LINK_MODE_STREAMING         4


/* -- L2CAP Channel Rx Error Types -------------------------------------------
 *
 */
#define L2ERR_INVALID_PDU       1   /* Segment type and framing errors */
#define L2ERR_INVALID_MTU       2   /* Length errors, causes channel closure. */
#define L2ERR_INVALID_MPS       3   /* Length errors, causes channel closure. */
#define L2ERR_INVALID_SAR       4   /* SAR field inappropriate */

/* -- L2MODE_UNKNOWN ---------------------------------------------------------
 *
 * This define is used with BtRemoteDevice->linkModes to describe the condition
 * when an Info request has not been sent to retrieve the available link modes.
 * Typically, an Info Request is not sent until a PSM that uses BT 1.2 extended
 * features (Flow Control/Retransmissions) attempts to establish a connection.
 */
#define L2MODE_UNKNOWN              0x00

/* -- Minimum timeouts -------------------------------------------------------
 *
 * Minimum timeouts are used to protect the local device from having its
 * processor consumed by L2CAP. This is prudent because the timeout values
 * are specified by the remote device.
 */
#ifndef MINIMUM_MONITOR_TIMEOUT
#define MINIMUM_MONITOR_TIMEOUT     200     /* 200 milliseconds */
#endif

#ifndef MINIMUM_RETRANS_TIMEOUT
#define MINIMUM_RETRANS_TIMEOUT     100     /* 100 milliseconds */
#endif

/* -- WriteSignalHeader Macro  -----------------------------------------------
 *
 * Initializes the Signal header workspace and writes the 4 byte command
 * header. It assumes that the command length is less than 255 bytes. (It
 * only writes the LSB of the length). It can only be used in the  L2Cap_
 * ProcessReq() function because it accesses variables which are local to
 * that function (ws & wsLen).
 */
void WriteSignalHeader(U8 Command, U8 Identifier, U16 Length);

#define WriteSignalHeader(_CMD, _ID, _LEN)                          \
      do { Assert((_LEN) <= 0xFF); OS_MemSet(ws, 0, 16); \
           ws[0] = (_CMD); ws[1] = (_ID); ws[2] = (U8)(_LEN);       \
           wsLen = 4+(_LEN); } while(0)

#define WriteEchoData(_DATA, _LEN) \
      do { OS_MemCopy(ws+4, _DATA+4, wsLen-4); \
           if (_LEN == L2CAP_ECHO_MAX) \
           { OS_MemCopy(ws+40, _DATA+(L2CAP_ECHO_MAX - 4), 8);} \
      } while(0)

/* -- WriteConfigOption16 Macro  ---------------------------------------------
 *
 * Writes a two byte format confguration option into the specified buffer.
 */
#define WriteConfigOption16(_BUFF, _OPTN, _VAL16) \
           (_BUFF)[0] = (_OPTN);(_BUFF)[1] = 2;StoreLE16((_BUFF)+2, (_VAL16))


/* -- ReturnSysPacket Macro  -------------------------------------------------
 *
 * Returns a system packet, allocated via L2Cap_GetSysPkt() to the system pool.
 */
void ReturnSysPkt(BtPacket *Packet);
#define ReturnSysPkt(_PKT) do {_PKT->headerLen = 0;_PKT->dataLen = 0;_PKT->flags = 0;\
                               InsertTailList(&L2C(signalPackets), &(_PKT)->node);   \
                               XASTAT_PeakDec(l2cap_num_signal_packets, 1);} while (0)

/*---------------------------------------------------------------------------
 *            L2E_UseExtWin()
 *---------------------------------------------------------------------------
 *
 * Synopsis: Checks Channel Extended Window flag
 *
 * Return:
 *
 */
BOOL L2E_UseExtWin(L2CAP_Channel *Channel);
#define L2E_UseExtWin(c)            ((c)->flags2 & L2CHF2_EXTENDED_WINDOW ? TRUE : FALSE)

/*---------------------------------------------------------------------------
 *            L2E_FcsSize()
 *---------------------------------------------------------------------------
 *
 * Synopsis:
 *
 * Return:
 *
 */
U8 L2E_FcsSize(L2CAP_Channel *Channel);
#define L2E_FcsSize(c)              ((c)->eCh->fcsOption == (L2FCS_NONE >> 1) ? 0 : 2)

/*---------------------------------------------------------------------------
 *            L2E_SHeaderLen()
 *---------------------------------------------------------------------------
 *
 * Synopsis: Get S-frame header length
 *
 * Return:
 *
 */
U8 L2E_SHeaderLen(L2CAP_Channel *Channel);
#define L2E_SHeaderLen(c)           ((L2E_UseExtWin(c) ? 8 : 6) + L2E_FcsSize(c))

/*---------------------------------------------------------------------------
 *            L2E_IHeaderLen()
 *---------------------------------------------------------------------------
 *
 * Synopsis: Get I-frame header length
 *
 * Return:
 *
 */
U8 L2E_IHeaderLen(L2CAP_Channel *Channel);
#define L2E_IHeaderLen(c)           (L2E_UseExtWin(c) ? 8 : 6)

/*---------------------------------------------------------------------------
 *            L2E_GetSAR()
 *---------------------------------------------------------------------------
 *
 * Synopsis:
 *
 *
 * Return:
 *
 */
L2capSarState L2E_GetSAR(U8 *ControlBuffer, BOOL Extended);
#define L2E_GetSAR(c, e)            ((e) ? (((U8)(c)[6]) & 0x03) : (((U8)(c)[5]) >> 6))

/*---------------------------------------------------------------------------
 *            L2E_ControlLen()
 *---------------------------------------------------------------------------
 *
 * Synopsis: Get control header length
 *
 * Return:
 *
 */
U8 L2E_ControlLen(L2CAP_Channel *Channel, U8 *Buffer);
#define L2E_ControlLen(c, b)        ((((b)[4]) & 0x01) ? L2E_SHeaderLen(c) :    \
                                     (L2E_IHeaderLen(c) +                       \
                                     ((L2E_GetSAR(b, L2E_UseExtWin(c)) == L2SAR_START) ? 2 : 0) + \
                                     L2E_FcsSize(c)))

/*---------------------------------------------------------------------------
 *            L2E_LocalBusyIsTrue()
 *---------------------------------------------------------------------------
 *
 * Synopsis: LocalBusy = TRUE or FALSE - TRUE indicates the local L2CAP
 *           entity is in a busy condition and FALSE indicates the local
 *           L2CAP entity is not busy.
 *
 * Return:
 *
 */
BOOL L2E_LocalBusyIsTrue(L2CAP_Channel *Channel);
#define L2E_LocalBusyIsTrue(c)          ((c)->eCh->localBusy == TRUE)


/****************************************************************************
 *
 * Internal Testing Interfaces
 *
 ****************************************************************************/

#define L2EVENT_ACTION_REQUIRED 99

/****************************************************************************
 *
 * Internal function prototypes
 *
 ****************************************************************************/
/* -- From l2cap_utl.c ----------------------------------------------------- */
L2CAP_Channel *L2Cap_GetChannel(BtRemoteDevice *RemDev, L2capChannelId Cid);
L2CAP_DynamicChannel *L2Cap_GetValidChannel(L2capChannelId Cid);
L2CAP_Channel *L2Cap_FindChannelByLocalIdent(U8 Ident, BtRemoteDevice *RemDev);
L2CAP_Channel *L2Cap_FindChannelByRemoteCid(BtRemoteDevice *RemDev, L2capChannelId Cid);
L2CAP_Channel *L2Cap_CheckForDuplicateRequest(U8 Ident, BtRemoteDevice *RemDev);
BOOL           L2Cap_FindFlushable(BtLogicalLink *Llink, L2CAP_Channel *Channel);
void           L2Cap_DisconnectAllReliableChannels(BtPhysicalLink *Plink);
L2capChannelId L2Cap_AllocAgentChannel(L2capAgent *Agent, BtStatus *Error);
L2CAP_DynamicChannel *L2Cap_GetPlinkChannel(BtPhysicalLink *pLink, L2capChannelId cid, BtStatus *Error);
L2CAP_DynamicChannel *L2Cap_AllocLinkChannel(BtPhysicalLink *Plink, const L2capPsm *Psm, BtStatus *Error);
L2CAP_DynamicChannel *L2Cap_AllocPsmChannel(BtRemoteDevice *RemDev, U8 CrlId, U16 Psm, U16 *Reason);
L2CAP_FixedChannel *L2Cap_AllocFixedChannel(BtLogicalLink *Llink, L2capChannelId FixedId, BtStatus *Error);
L2CAP_DynamicChannel *L2Cap_CloneChannel(L2CAP_DynamicChannel *Channel);
void           L2Cap_RemoveChannel(L2CAP_BaseChannel *Channel, BtLogicalLink *Llink);
BtPacket      *L2Cap_GetSysPkt(BOOL Priority);
void     L2Cap_FreeChannel(L2CAP_Channel *);
void     L2Cap_StartRtx(L2CAP_Channel *Channel);
void     L2Cap_StartErtx(L2CAP_Channel *Channel);
void     L2Cap_StopTimer(L2CAP_Channel *Channel);
U16      L2Cap_GetMtu(U8 *Options, U16 OptionLen);
#if NUM_AMPS > 0 
U8       L2CapGetConfigMode(L2capPsmValue Psm, BtPhysicalLink *Plink, U8 RemoteCrlId);
#else
U8       L2CapGetConfigMode(L2capPsmValue Psm, BtPhysicalLink *Plink);
#endif

BtStatus L2cap_RequestFlowSpec(L2CAP_Channel *Channel);

#if L2CAP_NUM_ENHANCED_CHANNELS > 0 
void     L2capValidateExtFlowSpec(void);
#endif

BOOL     L2Cap_ValidateCommand(const U8 *RxData, U16 RxLen);
BOOL L2Cap_ValidateSourceCID(BtRemoteDevice *remDev, U16 cid, U16 *reason);

void     L2Cap_NotifyPsm(L2CAP_Channel *Channel, L2capEventType Event, U16 Status);
#if (XA_ERROR_CHECK == XA_ENABLED) || (XA_DEBUG == XA_ENABLED)
BOOL     L2Cap_IsValidProtocol(const L2capPsm *Protocol);
#endif
L2capChannelId L2Cap_GetCidFromUlpChannel(void *UlpChannel);
void     L2Cap_FlushChannelTransmits(L2CAP_BaseChannel *Channel);
void     L2Cap_NotifyUserAndCloseChannel(L2CAP_DynamicChannel *Channel, U16 Status);
BtStatus L2capGetPhyLink(L2capAgent *Agent, BtRemoteDevice *RemDev, U8 CrlId);
BtStatus L2capCheckEfsPair(L2CAP_DynamicChannel *Channel);
void     L2Cap_StartAdmissionTimer(L2CAP_DynamicChannel *Channel);
void     L2Cap_StopAdmissionTimer(L2CAP_DynamicChannel *Channel);
L2capChannelState L2Cap_ConvertState(L2capChannelState l2aState);
BOOL     L2Cap_GetChannelByAgent(L2capAgent *Agent, L2capChannelDesc *Desc);
BOOL     L2Cap_GetChannelByPsm(BtRemoteDevice *RemDev, L2capPsmValue Psm,
                               L2capChannelDesc *Desc, BOOL IgnoreIncoming);
BtStatus L2cap_SetFlushTimeout(L2CAP_Channel *Channel);
void     L2cap_ProcessHciCommands(void);
void     L2Cap_TaskHandler(EvmTask *Task);
BOOL     L2Cap_IsExtendedWindow(L2CAP_Channel *Channel);
void     L2Cap_InitTypes(void);

/* -- From l2cap.c -------------------------------------------------------- */
BtStatus L2Cap_Send(L2CAP_BaseChannel *Channel, BtPacket *Packet, U16 RemoteCid);
BtStatus L2Cap_SendFixedChannelWithIdent(L2CAP_BaseChannel *channel, BtPacket *Packet, U8 remoteIdent);
void     L2Cap_QueueTxPacket(BtLogicalLink *Llink, BtPacket *Packet);
BOOL     L2Cap_SendSignal(BtLogicalLink *Llink, U8* ws, U16 wsLen);
BOOL     L2Cap_SendRejectSignalMtu(BtLogicalLink *Llink, U8 ident, U16 mtu);
BOOL     L2Cap_IsTxQueue(BtLogicalLink *Llink);
BOOL     L2Cap_IsPktOnTxQueue(BtLogicalLink *Llink, BtPacket *Packet);
BtPacket *L2Cap_DequeueChannelTxPacket(BtLogicalLink *Llink, L2capChannelId ChannelId);
BtPacket *L2Cap_DequeueTxPacket(BtLogicalLink *Llink, BtPacket *Packet);
BtStatus L2Cap_SendConnRspInternal(L2CAP_DynamicChannel *Channel);
BtStatus L2Cap_SendConnReqInternal(L2CAP_DynamicChannel *Channel);
BtStatus L2Cap_SendDiscReqInternal(L2CAP_DynamicChannel *Channel);
BtStatus L2Cap_SendDiscRspInternal(L2CAP_DynamicChannel *Channel);
BtStatus L2Cap_SendConfigReqInternal(L2CAP_DynamicChannel *Channel, BtPacket *Req,
                                     L2capConfigFlags Flags);
BtStatus L2Cap_SendConfigRspInternal(L2CAP_DynamicChannel *Channel, BtPacket *Rsp,
                                     U16 Result, L2capConfigFlags Flags);

/* -- From l2cap_sm.c ----------------------------------------------------- */
void L2Cap_StateMachine(L2CAP_Channel *Channel, I8 Event);
void L2Cap_ProcessReq(BtLogicalLink *Llink, U8 *RxData, U16 RxDataLen);
void L2Cap_ProcessRsp(BtLogicalLink *Llink, U8 *RxData, U16 RxDataLen);
void L2capAdmissionControl(BtLogicalLink *Llink, BtStatus Status);
void L2CapCheckPendQueue(void);

/* -- From l2cap_if.c ----------------------------------------------------- */
void L2CAP_LinkEvent(void *ctx, LinkEvent *Event);
void L2CAP_HciTxDone(BtLogicalLink *Llink, BtPacket *Packet, BtStatus Status);
void L2capGetAclInfo(BtLogicalLink *Llink);
void L2capInitLe(BtLogicalLink *Llink);
void L2CAP_CheckFeaturesReqAtClose(BtLogicalLink *lLink);
BOOL L2Cap_HandleRxError(BtLogicalLink *Llink, U16 Cid);
BOOL L2CAP_CheckChannel(L2CAP_BaseChannel *channel);

/* -- From l2cap tester --------------------------------------------------- */
#if JETTEST == XA_ENABLED
#define BDS_UDP_CONNECT 0xAA   /* Simulate connection oriented behavior in UDP Fake PAL */
extern BOOL (*Tester_HciReceive)(BtPhysicalLink *Plink, HciBuffer *HciBuff, BOOL firstPacket);
extern void (*Tester_HciTxDone)(BtPhysicalLink *Plink, BtPacket *Packet, BtStatus Status);
extern BOOL (*Tester_LinkDisconnect)(BtPhysicalLink *Plink, L2CAP_BaseChannel *Channel, BtErrorCode ErrCode);
#endif /* JETTEST == XA_ENABLED */

/* -- From ERTM ---- */
void L2E_SetXmitState(L2CAP_Channel *Channel, L2capErtmFsmFunc State);
#define L2E_SetXmitState(c, s)           ((c)->eCh->xmitState = s)
void L2E_SetRecvState(L2CAP_Channel *Channel, L2capErtmFsmFunc State);
#define L2E_SetRecvState(c, s)           ((c)->eCh->recvState = s)
void L2E_HciReceive(L2CAP_Channel *Channel);
void L2E_ReturnEnhancedPkt(L2CAP_Channel *Channel, BtPacket *Packet,
                           BOOL Acknowledged, BtStatus Status);
void L2E_XmitState(L2CAP_Channel *Channel, I8 Event);
void L2E_RecvState(L2CAP_Channel *Channel, I8 Event);
L2CAP_Enhanced *L2E_AllocEnhancedChannel(void);
BOOL L2E_InitEnhancedSettings(L2CAP_Channel *Channel);
void L2E_SetEnhancedOptions(L2CAP_Channel *Channel, U8 *OptionsIn, U8 *OptionsOut, BOOL Request);
BOOL L2E_ReserveHciBlock(L2CAP_Channel *Channel, U8 linkMode);
BtStatus L2E_SetBusyState(L2CAP_Channel *Channel, BOOL Busy);
void L2E_SendAck(L2CAP_Channel *Channel, L2capAckMode Mode);
void L2E_FreeChannel(L2CAP_Channel *Channel);
BtStatus L2E_SendSupervisory(L2CAP_Channel *Channel, U8 Type, U16 Sequence, U8 BitMask);
void L2E_SetDropSequence(L2capChannelId cid, U16 DropSequence, U16 DropCount);
BtStatus L2Cap_SendFeaturesReq(L2CAP_Channel *Channel, BtPacket *Request);
BtStatus L2Cap_SendFixedChannelsReq(L2CAP_Channel *Channel, BtPacket *Request);
void L2E_StopTransmission(L2CAP_Channel *Channel);
void L2E_StartTransmission(L2CAP_Channel *Channel);
BOOL L2E_IsTxPkt(L2CAP_Channel *Channel);
BOOL L2E_IsPktOnTxQueue(L2CAP_Channel *Channel, BtPacket *Packet);
BtPacket *L2E_GetTxPkt(L2CAP_Channel *Channel);
void L2E_ReturnTxPkt(L2CAP_Channel *Channel, BtPacket *Packet, BOOL Schedule);
void L2E_TxTask(EvmTask *Task);
void L2E_AdjustMps(L2CAP_Channel *Channel, U16 NewTxMps);

#if NUM_AMPS > 0 
void L2E_CleanClone(L2CAP_DynamicChannel *Channel);
#endif

#if L2CAP_NUM_ENHANCED_CHANNELS > 0 
void L2E_FlushPendingTransmits(L2CAP_Channel *Channel);
void L2E_Schedule(L2CAP_Channel *Channel);


BtStatus TXBUFF_Init(void);

void TXBUFF_Deinit(void);

BtStatus TXBUFF_ReserveBlock(L2CAP_Channel *channel, U16 *size);

U16 TXBUFF_BlockSizeAvail(void);

BtStatus TXBUFF_FreeBlock(L2CAP_Channel *channel);


#endif

U32  L2E_Reschedule(L2CAP_Channel *Channel, BtPriority Priority, L2CAP_Channel *Chans[]);

/* -- From l2cap.c ---- */
BtStatus L2_ConnectReq(L2capAgent *Agent);
BtStatus L2_ConnectRsp(L2capAgent *Agent);
BtStatus L2_AllocConnectReq(L2capAgent *Agent);
BtStatus L2_DeallocConnectReq(L2capAgent *Agent);
BtStatus _L2CAP_ConnectReq(L2capAgent *Agent);
BtStatus _L2CAP_ConnectRsp(L2capAgent *Agent);
BtStatus _L2CAP_DisconnectReq(L2capAgent *Agent);
BtStatus _L2CAP_Ping(L2capAgent *Agent, const U8 *Data, U16 DataLen);
BtStatus _L2CAP_GetInfo(L2capAgent *Agent, L2capInfoType InfoType);
BtStatus _L2CAP_SetManualMode(BOOL ManualMode);

#if NUM_AMPS > 0 
/* -- From L2A ---- */
BtStatus L2SendCreateRspInternal(L2CAP_DynamicChannel *Channel);
BtStatus L2SendCreateReqInternal(L2CAP_DynamicChannel *Channel);

BtStatus L2SendMoveReqInternal(L2CAP_DynamicChannel *Channel);
BtStatus L2SendMoveRspInternal(L2CAP_DynamicChannel *Channel);
BtStatus L2SendMoveCnfInternal(L2CAP_DynamicChannel *Channel);
BtStatus L2SendMoveCnfRspInternal(L2CAP_DynamicChannel *Channel);
BtStatus _L2CAP_MoveReq(L2capAgent *Agent);
BtStatus _L2CAP_MoveRsp(L2capAgent *Agent);
BtStatus _L2CAP_MoveCnf(L2capAgent *Agent);
#endif

#endif /* __L2CAP_I_H */
