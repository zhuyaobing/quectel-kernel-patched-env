/****************************************************************************
 *
 * File:
 *     le_signal_i.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Public types, defines, and prototypes for LE Signaling.
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

#ifndef __LE_SIGNAL_I_H
#define __LE_SIGNAL_I_H

#include "bttypes2.h"

/* -- LE Signal MTU -------------------------------------------------------
 *
 * The following defines the LE signaling max MTU size as defined by the Core
 * specification. 
 */
#define LE_SIG_MAX_MTU 23

/* -- LE L2CAP Connection Parameter Update States ---------------------------
 *
 * The following states are used by the BtLeSignalContext 'state' field.
 */
#define L2CPU_IDLE                          0
#define L2CPU_SLAVE                         1
#define L2CPU_MASTER                        2
#define L2CPU_TX_DATA                       3
#define L2CPU_WAIT_REM_FEATURES_SEND_REQ    4
#define L2CPU_WAIT_REM_FEATURES_RCV_REQ     5
#define L2CPU_WAIT_ENCRYPT_SEND_REQ         6
#define L2CPU_WAIT_ENCRYPT_RCV_REQ          7

/* -- LE Signal Codes -------------------------------------------------------
 *
 * The following defines are used as events to the LE state machine. They 
 * match the opcodes of the respective LE signaling commands.
 */
typedef U8 LeSignalCode;

#define LE_CONN_PARM_UPDATE_REQ  0x12
#define LE_CONN_PARM_UPDATE_RSP  0x13
#define LE_CREDIT_BASED_CON_REQ  0x14
#define LE_CREDIT_BASED_CON_RSP  0x15
#define LE_FLOW_CONTROL_CREDIT   0x16

/* The following events are only used internally */
#define LE_TX_CON_PARM_UPDATE_REQ       0x20
#define LE_RX_CON_PARM_UPDATE_REQ       0x21
#define LE_RX_CONN_PARM_UPDATE_RSP      0x22
#define LE_CONN_PARM_UPDATE_CNF         0x23
#define LE_TX_DATA                      0x24
#define LE_TX_DONE                      0x25
#define LE_RX_DONE                      0x26
#define LE_REJECT_PKT                   0x27
#define LE_REM_FEATURES_BUSY_RCV_REQ    0x28
#define LE_REM_FEATURES_BUSY_SEND_REQ   0x29
#define LE_ENCRYPT_BUSY_SEND_REQ        0x2A
#define LE_ENCRYPT_BUSY_RCV_REQ         0x2B
#define LE_REM_FEATURES_COMPLETE        0x2C
#define LE_ENCRYPT_COMPLETE             0x2D

/* End Of LeSignalCode */

/* -- LE Connection Parameters Result ---------------------------------------
 *
 * The result returned in a connection parameters update response.
 */
typedef U16 LeConnParmResult;

#define LE_CONN_PARM_ACCEPTED   0x0000
#define LE_CONN_PARM_REJECTED   0x0001

/* End Of LeConnParmResult */

/*---------------------------------------------------------------------------
 * NUM_LE_TX_PKTS constant
 * 
 *     Defines the number of TX Packets allocated for LE SIGNALING.
 *
 *     The default is 1.
 */
#define NUM_LE_TX_PKTS          1

/* -- LE Signaling Context Structure -----------------------------------------
 * 
 * This structure provides the context and state information for the LE
 * signaling channel.
 */
typedef struct _BtLeSignalContext {
    void               *channel;
    U8                  iClient;
    BtHandlerEx         signalHandler;
    BtRemoteDevice     *remDev;
    U8                 *data;
    U16                 dataLen;
    BOOL                waitUpl;
    LeConnParmResult    updateRspResult;
    BtPacket           *pkt;          /* Master packet */
    MeLeHandler         meleHandler;
    MeLeEvent          *meleEvent;
    MeLeUpdateLinkParms connParms;
    EvmTimer            rtxTimer;
    U8                  state;
} BtLeSignalContext;

void LESIGNAL_Create(BtLeSignalContext *ctx);
void LESIGNAL_InitFixedChannelClient(void *Client);

#endif /* __LE_SIGNAL_I_H */
