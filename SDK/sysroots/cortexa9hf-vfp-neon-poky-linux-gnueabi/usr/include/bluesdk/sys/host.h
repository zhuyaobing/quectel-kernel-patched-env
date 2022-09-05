/***************************************************************************
 *
 * File:
 *     host.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file contains internal definitions for the Host
 *     Controller Interface.
 *
 * Created:
 *     September 20, 1999
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

#ifndef __HOST_H
#define __HOST_H

#include "eventmgr.h"
#include "sys/hci.h"
#include "prescan.h"

/*--------------------------------------------------------------------------*
 * Typedefs and Data structures used internally by HCI                      *
 *--------------------------------------------------------------------------*/

/* SCO Support in HCI */
#define HCI_SCO_ENABLED ((NUM_SCO_CONNS != 0) && (BT_SCO_HCI_DATA == XA_ENABLED))

/* HCI States */
#define HCI_STATE_SHUTDOWN        0
#define HCI_STATE_INIT            1
#define HCI_STATE_DEINIT          2
#define HCI_STATE_READY           4

/* HCI Flags */
#define HCI_FLAG_PACKETS_COMPLETE 0x02
#define HCI_FLAG_BALANCE_PKTS     0x10

/* HCI Transmit states */
#define HCI_TX_STATE_SEND_HEADER  0
#define HCI_TX_STATE_SEND_DATA    1
#define HCI_TX_STATE_SEND_TAIL    2
#define HCI_TX_STATE_SEND_FCS     3

/* HCI Connection Flags */
typedef U8 HciConFlags;

#define HCI_ACTIVE_CON         0x01
#define HCI_DATA_TYPE_ACL      0x04
#define HCI_DATA_TYPE_SCO      0x08
#define HCI_DATA_TYPE_LE       0x10 /* Must not be 0, 1 or 2 */

/* Notification Flags */
#define HCI_NOTIFY_FLAG_TX_COMMAND  0x04
#define HCI_NOTIFY_FLAG_TRAN_ERR    0x10
#define HCI_NOTIFY_FLAG_RX_ERR      0x20

/* Completed Return States */
#define HCI_CR_DISABLED        0x00
#define HCI_CR_ENABLED         0x01
#define HCI_CR_DISABLING       0x02

/* Completed Return flags */
#define HCI_CR_TRACKED         0x01
#define HCI_CR_PKT_COMPLETE    0x02

/*---------------------------------------------------------------------------
 * HciPktTracker structure
 *
 *     Used internally to track outstanding tx packets.
 */
typedef struct _HciPktTracker {
    /* List node */
    ListEntry node;

    /* Associated HCI handle */
    HciHandle hciHandle;

    /* Packet of interest */
    BtPacket *pkt;

    /* A list of associated RX hciBuffers */
    ListEntry rxList;
} HciPktTracker;

/* HCI Connection Table */
typedef struct _HciConnection {
    HciHandle     hciHandle;
    HciConFlags   conFlags;

    /* Transmit state variables */
    ListEntry     btPacketList;
    BtPacket     *btPacket;
    U16           remaining;
    U16           fragRemain;
    const U8     *buffPtr;
    U8            txState;

    /* Event Manager task for receiving data */
    EvmTask           hciRxDataTask;
    BtPriorityToken   priToken;
    BtPriority        newPriority;

    /* Priority Tokens for transmit */
    BtPriorityToken  *txSignalPriToken;
    BtPriorityToken  *txGuaranteedPriToken;
    BtPriorityToken  *txBestEffortPriToken;
    BtPriorityToken  *txLePriToken;

#if HCI_SCO_ENABLED
    BtPriorityToken  *txScoPriToken;
#endif

    /* Flow control variables */
    U16           packetsSent;

#if HCI_HOST_FLOW_CONTROL == XA_ENABLED
    U16           packetsComplete;
    U8            disconnecting;
#endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */

    U8            completedReturn;
} HciConnection;

/* HCI Context */
typedef struct _BtHciContext {

    /* HCI Queues */
    ListEntry       rxBuffListSignal;      /* Received HCI buffers         */
    ListEntry       rxBuffListGuaranteed;  /* Received HCI buffers         */
    ListEntry       rxBuffListBestEffort;  /* Received HCI buffers         */
    ListEntry       rxBuffListSco;         /* Received HCI buffers         */
    ListEntry       rxBuffListLe;          /* Received HCI buffers         */
    ListEntry       rxBuffListEvents;      /* Received HCI buffers         */
    ListEntry       rxBadBuffList;         /* Received HCI buffers w/error */
    ListEntry       sentPacketList;        /* Handled HCI TX packets       */
    ListEntry       commandList;           /* Commands queued for transmit */
    ListEntry       deferredList;          /* Commands queued during init  */
    ListEntry       sentCommandList;       /* Commands sent to the radio   */
    ListEntry       hciPacketPool;         /* Available HCI TX packets     */
    ListEntry       hciScoPacketPool;      /* Available HCI TX packets     */
    ListEntry       hciCmdPacketPool;      /* Available HCI TX packets     */

    /* HCI Transmit and flow control variables */
    HciPacket       hciPackets[HCI_NUM_PACKETS];

#if HCI_SCO_ENABLED
    HciPacket       hciScoPackets[BT_SCO_HCI_NUM_PACKETS];
#endif /* HCI_SCO_ENABLED */

    HciPacket       hciCmdPackets[HCI_NUM_COMMANDS];
    U16             hciPacketsLeft;

#if HCI_SCO_ENABLED
    U16             hciScoPacketsLeft;
#endif

#if NUM_SCO_CONNS != 0
    U16             activeScoCons;
#endif

    U16             aclMaxTxPackets;
    U16             aclTxPacketLen;
    U16             aclTxPacketsLeft;
    U8              scoTxPacketLen;
    U16             scoTxPacketsLeft;
    U16             leTxPacketLen;
    U16             leTxPacketsLeft;
    U16             leMaxTxPackets;
#if HCI_HOST_FLOW_CONTROL == XA_ENABLED
    U16             aclRxPacketsLeft;

#if HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED)
    U16             scoRxPacketsLeft;
#endif /* HCI_SCO_ENABLED */

    EvmTimer        pktsCompleteTimer;
#endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */

    /* Internal command packet */
    HciCommand      cmd;

    /* Command flow control variable */
    U8              maxCommands;

    /* HCI connection table and state */
    HciConnection   con[HCI_NUM_HANDLES];
    U16             activeAclCons;
    U16             txCon;

    /* Other HCI state variables */
    U8              state;
    U8              flags;
    U8              conReqs;
    U8              transportError;
    U8              notifyFlags;

    /* Dynamic broadcast handles */
    HciHandle       piconetBcHandle;
    HciHandle       activeBcHandle;

    /* Reset delay and retry timers */
    EvmTimer        resetTimer;
    EvmTimer        retryTimer;

    /* Transport driver entry points */
    TranEntry       tranEntry;

    /* Radio handler entry point */
    RadioHandlerCallback radioEntry;

    /* Event Manager Tasks for HCI */
    EvmTask         hciProcessTask;
    EvmTask         hciTxSignalTask;
    EvmTask         hciTxGuaranteedTask;
    EvmTask         hciTxBestEffortTask;
    EvmTask         hciTxScoTask;
    EvmTask         hciScoRxTask;
    EvmTask         hciUnconnectedRxTask;
    EvmTask         hciEventRxTask;
    EvmTask         hciTxLeTask;
    EvmTask         hciLeRxTask;
    EvmTask         hciTxDoneTask;
    ListEntry       rescheduleTaskList;

    /* Rx packet trackers */
    HciPktTracker   trackerArray[HCI_NUM_PACKETS];
    ListEntry       freeTrackers;
    ListEntry       inUseTrackers;
    ListEntry       rxToProcess;

} BtHciContext;

/*--------------------------------------------------------------------------*
 * Functions used internally by the driver                                  *
 *--------------------------------------------------------------------------*/

/* Internal references to HCI entry points */
void HciTransportError(HciTransError);
void HciBufferInd(HciBuffer *buffer, BtStatus status);
void HciPacketSent(HciPacket *packet);
/*BtStatus HciSendCommand(U16 opCode, U8 parmLen, HciCommand *cmd, BOOL internal); */
void HciPktsCompleteTimerHandler(EvmTimer *timer);

/* HCI utility functions */
U16 HciGetTableEntry(HciHandle hciHandle);
HciHandle HciAddConnection(HciHandle hciHandle, U8 linkType);
BtStatus HciDeleteConnection(HciHandle hciHandle);
void HciSetNewBcHandle(HciHandle *handle);
U8 HCI_GetState(void);
#define HCI_GetState() (HCC(state))
void HciRescheduleTxTasks(void);
HciPktTracker *HciFindTracker(HciHandle hciHandle, BOOL First);

/* HCI event processing functions */
void HciProcessTask(EvmTask *Task);
void HciHandleTxDataTask(EvmTask *Task);
void HciProcessEvent(HciBuffer *hciBuffer);
void HciHandleRxDataTask(EvmTask *Task);
void HciHandleSentPacketsTask(EvmTask *Task);

/* Prescan initialization  */
void HciInitPrescan(void);

#endif /* __HOST_H */
