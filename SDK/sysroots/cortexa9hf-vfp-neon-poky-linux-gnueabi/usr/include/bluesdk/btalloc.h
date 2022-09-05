/***************************************************************************
 *
 * File:
 *     btalloc.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Contains the definitions for all the memory allocations
 *     (both ROM and RAM) for the Bluetooth stack.
 *
 * Created:
 *     October 19, 1999
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

#ifndef __BTALLOCH
#define __BTALLOCH

#include "sys/btstats.h"

#include "sys/mei.h"
#include "sys/host.h"
#include "sys/rxbuff.h"
#include "sys/radiomgi.h"
#include "sys/l2cap_i.h"
#if RFCOMM_PROTOCOL == XA_ENABLED
#include "sys/rfc.h"
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
#include "sys/sdpi.h"
#include "sys/prescani.h"

#if NUM_AMP_HCI_CONTROLLERS > 0
#include "sys/amphcicontrol.h"
#endif

#if NUM_LE_DEVICES > 0
#include "sys/gatti.h"
#include "sys/smpi.h"
#include "sys/lel2cap_i.h"
#endif /* NUM_LE_DEVICES > 0 */

#include XA_HEADER_H

#ifndef IPP_CTX
#define IPP_CTX
#endif

/*---------------------------------------------------------------------------
 * Bluetooth Stack Memory Allocation layer
 *
 *     The context structure in this layer contains all the memory used
 *     internally by the Blue SDK protocol stack. This memory can either
 *     be allocated dynamically or statically. When porting to a new
 *     operating system this context structure must be allocated and the
 *     access macros defined appropriately.
 */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtContext structure
 *
 *     Internal memory used by the Blue SDK protocol stack.
 */
typedef struct {
    BtMeContext        me;
    BtHciContext       hci;   
    BtRxbuffContext    rxbf;
    BtRmgContext       rmg;
    BtL2CapContext     l2cap;
#if SDP_SERVER_SUPPORT == XA_ENABLED
    BtSdpServContext   sdps;
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */
#if SDP_CLIENT_SUPPORT == XA_ENABLED
    BtSdpClientContext sdpc;
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */
#if RFCOMM_PROTOCOL == XA_ENABLED
    BtRfcContext       rfc;
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
#if NUM_LE_DEVICES > 0
    GattContext        gatt;
    SmpContext         smp;
    LeL2capContext     lel2cap;
#endif /* NUM_LE_DEVICES > 0 */
    IPP_CTX

    ListEntry          timerList;
    ListEntry          synchronizerList;
    ListEntry          systemTaskList;
    ListEntry          signalTaskList;
    ListEntry          guaranteedTaskList;
    ListEntry          bestEffortTaskList;
    BOOL               taskMgrScheduled;
#if XA_STATISTICS == XA_ENABLED
    BtStatistics       stats;
#endif /* XA_STATISTICS == XA_ENABLED */
#if BT_PRESCAN == XA_ENABLED
    PrescanContext     prescan;
#endif /* BT_PRESCAN == XA_ENABLED */
#if NUM_AMP_HCI_CONTROLLERS > 0
    AmpHciController  hciControls[NUM_AMP_HCI_CONTROLLERS];
#endif

} BtContext;


/****************************************************************************
 *
 * Functions and Macros
 *
 ****************************************************************************/

/* The stack context can be either a global structure or a pointer
 * to a global structure. If XA_CONTEXT_PTR is set to XA_ENABLED then the
 * the context is a pointer otherwise it is a global structure.
 */
#if XA_CONTEXT_PTR == XA_ENABLED
extern BtContext* bt;
#define BTC(s)  (bt->s)
#define MEC(s)  (bt->me.s)
#define HCC(s)  (bt->hci.s)
#define RMG(s)  (bt->rmg.s)
#define L2C(s)  (bt->l2cap.s)
#if SDP_SERVER_SUPPORT == XA_ENABLED
#define SDPS(s) (bt->sdps.s)
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */
#if SDP_CLIENT_SUPPORT == XA_ENABLED
#define SDPC(s) (bt->sdpc.s)
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */
#if RFCOMM_PROTOCOL == XA_ENABLED
#define RFC(s)  (bt->rfc.s)
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
#define RXBUF(s) (bt->rxbf.s)
#if BT_PRESCAN == XA_ENABLED
#define PRESCAN(s) (bt->prescan.s)
#endif /* BT_PRESCAN == XA_ENABLED */
#define GATT(s)  (bt->gatt.s)
#define SMP(s)   (bt->smp.s)
#define LEL2C(s) (bt->lel2cap.s)
#else /* XA_CONTEXT_PTR == XA_ENABLED */

extern BtContext bt;
#define BTC(s)  (bt.s)
#define MEC(s)  (bt.me.s)
#define HCC(s)  (bt.hci.s)
#define RMG(s)  (bt.rmg.s)
#define L2C(s)  (bt.l2cap.s)
#if SDP_SERVER_SUPPORT == XA_ENABLED
#define SDPS(s) (bt.sdps.s)
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */
#if SDP_CLIENT_SUPPORT == XA_ENABLED
#define SDPC(s) (bt.sdpc.s)
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */
#if RFCOMM_PROTOCOL == XA_ENABLED
#define RFC(s)  (bt.rfc.s)
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
#define RXBUF(s) (bt.rxbf.s)
#if BT_PRESCAN == XA_ENABLED
#define PRESCAN(s) (bt.prescan.s)
#endif /* BT_PRESCAN == XA_ENABLED */
#define GATT(s)  (bt.gatt.s)
#define SMP(s)   (bt.smp.s)
#define LEL2C(s) (bt.lel2cap.s)
#endif /* XA_CONTEXT_PTR */

/*---------------------------------------------------------------------------
 * BtInitMemory()
 *
 *     Called by the Event Manager to fill the context memory with 0.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     void
 */
BOOL BtInitMemory(void);

#endif /* __BTALLOCH */
