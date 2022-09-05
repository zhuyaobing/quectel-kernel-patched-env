/***************************************************************************
 *
 * File:
 *     abcsphci.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file contains definitions and structures specific
 *     to the ABCSP HCI transport driver.
 *
 * Created:
 *     December 21, 1999
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

#ifndef __ABCSPHCI_H
#define __ABCSPHCI_H

#include "bttypes.h"
#include "hcitrans.h"
#include "sys/rxbuff.h"
#include "eventmgr.h"
#include "config_txmsg.h"
#if BT_EXPOSE_BCCMD == XA_ENABLED
#include "bccmd.h"
#endif

/****************************************************************************
 *
 * Section: Configuration Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * UART_RX_BUFF_SIZE constant
 *
 *     The UART receive buffer is used to store data read from the
 *     UART for indication to the ABCSP layer. This buffer can be any
 *     size greater than 1.
 */
#ifndef UART_RX_BUFF_SIZE
#define UART_RX_BUFF_SIZE       64
#endif


/*---------------------------------------------------------------------------
 * UART_TX_BUFF_SIZE constant
 *
 *     The UART transmit buffer is used to store data from the ABCSP
 *     layer to be written to the UART. This buffer can be any size 
 *     greater than 1.
 */
#ifndef UART_TX_BUFF_SIZE
#define UART_TX_BUFF_SIZE       64
#endif


/*---------------------------------------------------------------------------
 * TIMER_TCONF_TIMEOUT constant
 *
 *     BCSP Link Establishment Timer: Tconf (default 1 second)
 */
#ifndef TIMER_TCONF_TIMEOUT
#define TIMER_TCONF_TIMEOUT     1000
#endif


/*---------------------------------------------------------------------------
 * TIMER_TSHY_TIMEOUT constant
 *
 *     BCSP Link Establishment Timer: Tshy (default 1 second) 
 */
#ifndef TIMER_TSHY_TIMEOUT
#define TIMER_TSHY_TIMEOUT      1000
#endif


/*---------------------------------------------------------------------------
 * TIMER_BCSP_TIMEOUT constant
 *
 *     BCSP Reliable Message Retransmit Timer: (default 0.25 seconds) 
 */
#ifndef TIMER_BCSP_TIMEOUT
#define TIMER_BCSP_TIMEOUT      250
#endif


/****************************************************************************
 *
 * Section: Types and Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * TXMSG type
 *
 *     We statically allocate TXMSG structures to eliminate the need for
 *     malloc() support. Since txmsg.c defines the TXMSG structure locally
 *     we use this typedef of the same size locally. If the structure size
 *     grows in the future, xa_malloc() will detect this and Assert.
 */
typedef struct txmsg {
    void       *next;
    void       *m;
    unsigned    chan:4;
    unsigned    seq:3;
} TXMSG;    


/*---------------------------------------------------------------------------
 * ABCSP_Context structure
 *
 *     The ABCSP HCI transport driver context contains queues and state
 *     information for the HCI-ABCSP transport driver.
 */
typedef struct _ABCSP_Context 
{
    /* Callback Entry points */
    TranEntry    tranEntry;
    TranCallback callback;

    /* ASBCP initialization state. */
    U8          abcspInit;

    /* True when ABCSP has requested we call abcsp_pumptxmsgs() */
    BOOL        pumpTx;

    /* Receive: UART buffer variables */
    U16         rxUartReadOffset;
    U16         rxUartWriteOffset;
    U8          rxUartBuff[UART_RX_BUFF_SIZE];

    /* Transmit: UART buffer variables */
    U16         txUartReadOffset;
    U16         txUartWriteOffset;
    U8          txUartBuff[UART_TX_BUFF_SIZE];

    /* Receive: HCI buffer variables */
    RxBuffHandle rxBuff;
    U16         rxBuffSize;     /* Anticipated total size of rx buffer */
    U16         rxWriteOffset;  /* Write offset into rx buffer */
    U8          rxState;
    U8          rxChannel;      /* ABCSP channel of current rx buffer */
    U8          rxStage[2];     /* Temp buffer to receive HCI conn handle */

    /* Transmit: HCI buffer variables */
    ListEntry   txQueue;        /* Packets to send */
    U16         txReadOffset;   /* Number of bytes already read */
    U8          txFrag;         /* Index of fragment to process */

    /* TXMSG pool is used to eliminate the need for dynamic memory.
     * It needs to be large enough to cover relq size + 1 for the unrelq.
     */
    U8          txMsgInuse;
    TXMSG       txMsgPool[ABCSP_TXWINSIZE+1];

    /* Timers used by ABCSP as well as a receive retry timer. */
    struct _timers {
        EvmTimer    bcsp;
        EvmTimer    tshy;
        EvmTimer    tconf;
        EvmTimer    pumpRx;
    } timers;

#if XA_DEBUG == XA_ENABLED
    ListEntry   txEnroute;      /* Debug verify packets held by ABCSP */
#endif

#if BT_EXPOSE_BCCMD == XA_ENABLED
    bccmdMsgStruct bccmdTXMsg;
    bccmdBufStruct bccmdTXBuf;
    bccmdMsgStruct bccmdRXMsg;
    bccmdBufStruct bccmdRXBuf;
#endif

} ABCSP_Context;


/* Define the appropriate external reference to the transport context,
 * if necessary.
 */
#if (XA_CONTEXT_PTR == XA_ENABLED)
extern ABCSP_Context *xprt;
#define ABCSP(s) (xprt->s)

#elif (XA_CONTEXT_PTR == XA_DISABLED)
extern ABCSP_Context xprt;
#define ABCSP(s) (xprt.s)
#endif

/* Function Prototypes */
BtStatus ABCSP_Init(TranCallback tranCallback);
BtStatus ABCSP_Shutdown(void);

#endif /* __ABCSPHCI_H */
