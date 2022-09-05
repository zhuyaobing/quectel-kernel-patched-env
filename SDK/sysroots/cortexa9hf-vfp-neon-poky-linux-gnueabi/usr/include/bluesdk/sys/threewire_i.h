/****************************************************************************
 *
 * File:
 *     threewire_i.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file is part of an Three-Wire Transport implementation.  It contains
 *     definitions of types, constants and data structures used to implement
 *     the Three-Wire transport layer.
 *
 * Created:
 *     April 5, 2013
 *
 * Copyright 2006-2017 OpenSynergy GmbH.
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

#ifndef __THREEWIRE_I_H
#define __THREEWIRE_I_H

#include "config.h"
#include "bttypes.h"
#include "hcitrans.h"
#include "sys/threewire.h"

/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/

/* MS to wait for HCI events */
#define UTP_UART_EVENT_WAIT_TIME  10000

/* Receive States */
#define RXS_GET_FRM_START     0
#define RXS_GET_TWIRE_HEAD1   1
#define RXS_GET_TWIRE_HEAD234 2
#define RXS_GET_LINK_PAYLOAD  3
#define RXS_GET_HCI_DATA      4
#define RXS_GET_CRC           5
#define RXS_GET_FRM_END       6

/* Transmit States */
#define TXS_IDLE          1
#define TXS_TYPE          2
#define TXS_HEADER        3
#define TXS_DATA          4

/* Link State */
typedef U8 TWireLinkState;

#define LINK_STATE_UNINIT   0
#define LINK_STATE_INIT     1
#define LINK_STATE_ACTIVE   2

/* Header types */
typedef U8 TWireHeadType;

#define HEAD_TYPE_ACK         0
#define HEAD_TYPE_HCI_CMD     1
#define HEAD_TYPE_HCI_DATA    2
#define HEAD_TYPE_HCI_SCO     3
#define HEAD_TYPE_HCI_EVENT   4
#define HEAD_TYPE_VENDOR      14
#define HEAD_TYPE_LINK        15


/* TX state */
typedef U8 TWireTxState;

#define TWTX_STATE_START    0
#define TWTX_STATE_HEAD     1
#define TWTX_STATE_PAYLOAD  2
#define TWTX_STATE_CRC1     3
#define TWTX_STATE_CRC2     4
#define TWTX_STATE_END      5
#define TWTX_STATE_DONE     6

/* Flags */
typedef U16 TWireFlags;

#define TWIREF_TX_ACTIVE       0x0001
#define TWIREF_SEND_SYNC       0x0002
#define TWIREF_SEND_SYNC_RSP   0x0004
#define TWIREF_SEND_CONFIG     0x0008
#define TWIREF_SEND_CONFIG_RSP 0x0010
#define TWIREF_NEED_SCO_BUFF   0x0020
#define TWIREF_NEED_ACL_BUFF   0x0040
#define TWIREF_NEED_EVENT_BUFF 0x0080
#define TWIREF_RETRANSMIT      0x0100
#define TWIREF_NEED_RETRANS    0x0200
#define TWIREF_NEED_ACK        0x0400
#define TWIREF_RETRANS_PKT     0x0800

#define TWIREF_WAIT_BUFFER     (TWIREF_NEED_SCO_BUFF | TWIREF_NEED_ACL_BUFF | TWIREF_NEED_EVENT_BUFF)

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * THREEWIRE_Context structure
 *
 *     The HCI Three-wire transport driver context contains
 *     information used by the HCI Three-Wire transport driver.
 */
typedef struct _THREEWIRE_Context
{
    /* Transmit queues */
    ListEntry      txQueue;
    ListEntry      unackedList;

    /* Callback Entry points */
    TranEntry      tranEntry;
    TranCallback   callback;

    TWireLinkState linkState;
    TWireFlags     flags;

    U32            tMax;
    U16            maxBuffSize;

    /* Current config settings */
    U8             numUnacked;
    U8             seq;
    U8             ack;
    U8             windowSize;
    BOOL           useOof;
    BOOL           integrity;

    /* RX State variables */
    U16            rxOff;
    U16            rxPayLen;
    U8             rxCh;
    U8             buffType;
    U8            *rxPtr;
    U8             rxTwireHead[4];
    U8             linkPayload[3];
    U8             rxSeq;
    U8             rxAck;
    U8             rxHeadType;
    BOOL           rxReliable;
    BOOL           rxIntegrity;
    U8             rxHeader[4];
    U8             rxHeaderLen;
    U8             rxCrc[2];
    RxBuffHandle   rxBuffHandle;
    U8             rxState;
    BOOL           rxEscape;
#if (TWIRE_RX_BUFF_SIZE > 1)
    U8             rxBuff[TWIRE_RX_BUFF_SIZE];
    U16            rxi;
    U16            readLen;
#endif

    /* TX variables */
    HciPacket     *txPacket;
    U8            *txPayPtr;           /* pointer to pay load buffer */
    U16            txPayLen;           /* num bytes in payload to write */
    U16            txPayOff;           /* Offset into payload buffer */
    U16            txOff;
    U8             txHeadSize;         /* Size of HciPacket header */
    U8             txFrag;             /* index of fragment to process */
    TWireTxState   txState;
    BOOL           escape;
    BOOL           txScoFlowControl;   /* If TRUE SCO packets are sent reliably other unreliably */
    BOOL           txReliable;
    BOOL           txIntegrity;
    U8             escapeC;
    U16            txCrc;
    U8             txHeader[4];
    U8             txConfigBuff[3];
    U16            txTransSize;        /* Amount of data to transmit at a time */
    U8             txBuff[TWIRE_TX_BUFF_SIZE];

    TimeT          txTimeVal;
    TimeT          retransmitTime;
    TimeT          ackTime;
    EvmTimer       txTimer;
    EvmTimer       ackTimer;
} THREEWIRE_Context;

/* Define the appropriate external reference
 * to the transport context, if necessary.
 */
#if (XA_CONTEXT_PTR == XA_ENABLED)

extern THREEWIRE_Context *twireCtx;
#define THREEWIRE(s) (twireCtx->s)

#elif (XA_CONTEXT_PTR == XA_DISABLED)

extern THREEWIRE_Context twireCtx;
#define TWIRE(s) (twireCtx.s)

#endif /* XA_CONTEXT_PTR */

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

U32 TwireConvertUARTEncodingToBPS(U32 speed);
void TwireCalcTmax(U32 speed);

BtStatus TwireSendSync(void);
BtStatus TwireSendSyncRsp(void);
BtStatus TwireSendConfig(void);
BtStatus TwireSendConfigRsp(void);
BtStatus TwireSendAck(void);

void TwireTxTimerHandler(EvmTimer *timer);
void TwireAckTimerHandler(EvmTimer *timer);
BtStatus TwireUartWrite(void);
BtStatus TwireSendHciPacket(void);
BtStatus TwireRetransmitHciPacket(void);
void TwireSetupTxPacket(U8 seq);
BOOL TwireProcessRxChar(void);

BOOL TwireRxCheckEscape(U8 rxCh, U8 *ch);
BOOL TwireProcessRxHeader(void);
void TwireReadData(void);
BOOL TwireProcessRxLinkPacket(void);
void TwireSetupTxLink(U16 payLen);
BOOL TwireSetupHciBuffRead(void);
U16 Newpppfcs(U16 fcs, U8 *cp, U16 len);
U16 TwireReverseCrc(U16 crc);
void TwireSetupSlipHeader(U8 seq, U8 ack, TWireHeadType type, BOOL integrity, BOOL reliable, U16 payLen);

extern U8 *TwireSync;
extern U8 *TwireSyncRsp;
extern U8 *TwireConfig;
extern U8 *TwireConfigRsp;

#endif /* __THREEWIRE_I_H */
