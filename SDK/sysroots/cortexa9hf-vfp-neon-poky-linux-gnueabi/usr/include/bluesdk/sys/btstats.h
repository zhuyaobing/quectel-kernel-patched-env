/***************************************************************************
 *
 * File:
 *     btstats.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file contains declarations for stack instrumentation.
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

#ifndef __BTSTATS_H
#define __BTSTATS_H

#include "bttypes.h"
#include "eventmgr.h"

/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/

#if XA_STATISTICS == XA_ENABLED
typedef struct _XaTimeStat {
    BOOL          started;
    TimeT         peak_latency;
    TimeT         total_time;
    U32           total_bytes;
    U32           samples;
} XaTimeStat;

#if BT_STACK == XA_ENABLED

/*---------------------------------------------------------------------------
 * BtStatistics structure
 *
 * Structure used in instrumentation to record statistics.
 */
typedef struct _BtStatistics {
    /* HCI statistics */
    U16     hci_num_acl_buffers;            /* Peak number of ACL buffers allocated */  
    U16     hci_num_no_acl_rxbuf;           /* Count of no buf available for ACL rx */
    U16     hci_num_no_sco_rxbuf;           /* Count of no buf available for SCO rx */
    U16     hci_acl_buff_size;              /* Peak ACL buffer allocation */
    U16     hci_num_acl_bt_packets;         /* Peak number of ACL BtPackets queued */
    U16     hci_num_sco_buffers;            /* Peak number of SCO buffers allocated */
    U16     hci_sco_buff_size;              /* Peak SCO buffer allocation */
    U16     hci_num_sco_bt_packets;         /* Peak number of SCO BtPackets queued */
    U16     hci_num_event_buffers;          /* Peak number of Event buffers allocated */
    U16     hci_num_hci_packets;            /* Peak number of ACL HciPackets queued */
    U16     hci_num_sco_hci_packets;        /* Peak number of SCO HciPackets queued */
    U16     hci_num_no_hci_packets;         /* Count of no HciPackets available for tx*/
    U16     hci_num_no_sco_hci_packets;     /* Count of no SCO HciPackets available for tx*/
    U16     hci_max_tx_packet_size;         /* Peak size of HCI ACL Tx packets */
    XaTimeStat  hci_tx_timer;
    XaTimeStat  hci_rx_timer;

    /* HCI AMP statistics */
    U16     hpool_num_buffers;              /* Peak number of HPOOL buffers allocated */ 
    U16     hpool_num_no_rxbuf;             /* Count of no buf available for HPOOL rx */
    U16     hpool_buff_size;                /* Peak HPOOL buffer allocation */
    U16     hciamp_num_tx_buffers;          /* Peak number of tx buffers used */
    U16     hciamp_num_no_txbuf;            /* Count of tx buff alloc failures */

    XaTimeStat   hpool_rx_timer;
    XaTimeStat   hpool_tx_timer;

    /* L2CAP statistics */
    U16     l2cap_num_protocols;        /* Peak number of registered protocols */
    BOOL    l2cap_ping_support;         /* TRUE - Ping service was used. */
    U16     l2cap_rtx_timeout;          /* Number of times RTX timer expired. */
    U16     l2cap_ertx_timeout;         /* Number of times E-RTX timer expired. */
    U16     l2cap_retrans_timeout;      /* Number of times retransmit timer expired (BT1.2). */
    U16     l2cap_monitor_timeout;      /* Number of times monitor timer expired (BT1.2). */
    U16     l2cap_mtu;                  /* Maximum received PDU size. */
    U16     l2cap_num_channels;         /* Peak number of channels allocated. */
    BOOL    l2cap_deregister_func;      /* TRUE - Deregister function was used. */
    U16     l2cap_num_signal_packets;   /* Peak number of signaling packets used. */
    U16     l2cap_num_segment_packets;  /* Peak number of segment packets used. */

    XaTimeStat  l2cap_tx_timer;

    /* RFCOMM statistics */

    XaTimeStat  rfcomm_tx_timer;

    /*
     * This structure is used to keep track of the current value of a variable
     * whose peak value is what the statistics is really trying to capture. For
     * a variety of reasons, structure may not be accurate at deinitialization time.
     */
    struct _BtStatCurrent {
        /* HCI statistics */
        U16     hci_num_acl_buffers;        /* Peak number of ACL buffers allocated */
        U16     hci_num_acl_bt_packets;     /* Peak number of ACL packets queued */
        U16     hci_num_acl_hci_packets;    /* Peak number of ACL HciPackets queued */
        U16     hci_num_sco_buffers;        /* Peak number of SCO buffers allocated */
        U16     hci_num_sco_bt_packets;     /* Peak number of SCO packets queued */
        U16     hci_num_sco_hci_packets;    /* Peak number of SCO HciPackets queued */
        U16     hci_num_event_buffers;      /* Peak number of Event buffers allocated */
        U16     hci_num_hci_packets;        /* Peak number of ACL HciPackets queued */
        U16     hci_max_tx_packet_size;     /* Peak size of HCI ACL Tx packets */

        /* HCI AMP statistics */
        U16     hpool_num_buffers;          /* Peak number of HPOOL buffers allocated */ 
        U16     hciamp_num_tx_buffers;      /* Peak number of tx buffers used */

        /* L2CAP statistics */
        U16     l2cap_num_protocols;        /* Current number of registered protocols */
        U16     l2cap_num_channels;         /* Current number of channels allocated. */
        U16     l2cap_num_signal_packets;   /* Current number of signaling packets used. */
        U16     l2cap_num_segment_packets;  /* Current number of segment packets used. */


    } curr;

} BtStatistics;
/* End of BtStatistics */
#endif /* BT_STACK == XA_ENABLED */

/****************************************************************************
 *
 * Function Section
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * XASTAT_PeakInc 
 *      Increments a peak or maximum value. This function is to capture
 *      the high-water mark of a variable.
 *     
 * Requires:
 *      XA_STATISTICS must be enabled.
 *      The variable name 'member' must exist in BtStatistics and 
 *      BtStatistics.curr.
 *     
 * Parameters:
 *      member - a variable in the statistics structure. Must be a member
 *              of both BtStatistics and BtStatistics.curr
 *
 *      val - amount to increment 'member'
 */
void XASTAT_PeakInc(U32 member, U16 val);

#define XASTAT_PeakInc(f, i) do { \
        if ((U32)(BTC(stats).curr.f + i) < (U32)(0xFFFFFFFF >> (8*(4-sizeof(BTC(stats).curr.f))))) BTC(stats).curr.f += i; else BTC(stats).curr.f = (U16)~0; \
        if (BTC(stats).curr.f > BTC(stats).f) BTC(stats).f = BTC(stats).curr.f; } while(0)

/*---------------------------------------------------------------------------
 * XASTAT_PeakDec 
 *      Decrements a peak or maximum value. This function is for those
 *      variables whose high-water marks are being captured.
 *     
 * Requires:
 *      XA_STATISTICS must be enabled.
 *      The variable name 'member' must exist in BtStatistics and 
 *      BtStatistics.curr.hpool_buff_size
 *     
 * Parameters:
 *      member - a variable in the statistics structure. Must be a member
 *              of both BtStatistics and BtStatistics.curr
 *
 *      val - amount to decrement 'member'
 */
void XASTAT_PeakDec(U32 member, U16 val);

#define XASTAT_PeakDec(f, d) \
    do { if (BTC(stats).curr.f >= d) BTC(stats).curr.f -= d; } while(0)

/*---------------------------------------------------------------------------
 * XASTAT_PeakSet 
 *      Sets a peak or maximum value. This function is for those
 *      variables whose high-water marks are being captured.
 *     
 * Requires:
 *      XA_STATISTICS must be enabled.
 *      The variable name 'member' must exist in BtStatistics.
 *     
 * Parameters:
 *      member - a variable in the statistics structure. Must be a member
 *              of BtStatistics
 *
 *      val - value to which 'member' is set; must be greater that the
 *            current value of 'member'
 */
void XASTAT_PeakSet(U32 member, U16 val);

#define XASTAT_PeakSet(f, i) \
        do { if (i > (BTC(stats).f)) BTC(stats).f = i; } while(0)

/*---------------------------------------------------------------------------
 * XASTAT_CounterInc 
 *      Increments a statistics counter.
 *     
 * Requires:
 *      XA_STATISTICS must be enabled.
 *      The variable name 'member' must exist in BtStatistics.
 *     
 * Parameters:
 *      member - a variable in the statistics structure. Must be a member
 *              of both BtStatistics
 *
 *      val - value to which 'member' is set; must be greater that the
 *            current value of 'member'
 */
void XASTAT_CounterInc(U32 member, U16 val);

#define XASTAT_CounterInc(f, i) \
        do { if ((U32)(BTC(stats).f + i) < (U32)(0xFFFFFFFF >> (8*(4-sizeof(BTC(stats).f))))) BTC(stats).f += i; else BTC(stats).f = (U16)~0; } while (0)

/*---------------------------------------------------------------------------
 * XASTAT_Set 
 *      Sets a statistics variable.
 *     
 * Requires:
 *      XA_STATISTICS must be enabled.
 *      The variable name 'member' must exist in BtStatistics.
 *     
 * Parameters:
 *      member - a variable in the BtStatistics structure.
 *
 *      val - value to which 'member' is set
 */
void XASTAT_Set(U32 member, U16 val);
#define XASTAT_Set(f, v) \
        BTC(stats).f = v

/*---------------------------------------------------------------------------
 * XASTAT_Max 
 *      Sets a member to a new value only if the new value is greater than  
 *      the current value.
 *     
 * Requires:
 *      XA_STATISTICS must be enabled.
 *      The variable name 'member' must exist in BtStatistics.
 *     
 * Parameters:
 *      member - a variable in the statistics structure. Must be a member
 *              of both BtStatistics
 *
 *      val - value to which 'member' is set
 */
void XASTAT_Max(U32 member, U16 val);

#define XASTAT_Max(f, v) \
        BTC(stats).f = max(BTC(stats).f, v)

/*---------------------------------------------------------------------------
 * XASTAT_TimerStart 
 *      Start a statistics timer. The actual timer is not updated by this
 *      function. The current time (returned by OS_GetSystemTimeUs) is place
 *      in 'start'. The actual timer structure is updated during 
 *      XASTAT_TimerStop.
 *     
 * Requires:
 *      XA_STATISTICS must be enabled.
 *      The variable passed into 'member' must exist in BtStatistics.
 *     
 * Parameters:
 *      member - name of an XaTimeStat structure in the statistics structure.
 *
 *      start - pointer to variable in which to put start time. 'start'
 *              is set to the current time. 
 */
void XASTAT_TimerStart(XaTimeStat member, TimeT start);

#define XASTAT_TimerStart(f, ctxt) \
        do { \
            ctxt = OS_GetSystemTimeUs(); \
            BTC(stats.f).started = TRUE; \
        } while (0)

/*---------------------------------------------------------------------------
 * XASTAT_TimerStop 
 *      Stop a statistics timer that was previously started with 
 *      XASTATS_StartTimer. The elapsed time is current time less start
 *      time. 
 *
 *      If elapsed > the peak latency, timer.peak_latency is set
 *      to elapsed 
 *
 *      timer.peak_total is incremented by elapsed. 
 *
 *      timer.total_bytes is incremented by bytes
 *
 *      timer.samples is incremented  
 *     
 * Requires:
 *      XA_STATISTICS must be enabled.
 *      The variable name 'timer' must exist in BtStatistics.
 *     
 * Parameters:
 *      bytes - number of bytes sent or received
 * 
 *      timer - a timer variable in the statistics structure.
 *
 *      start - system time as set by XASTAT_TimerStart when the interval being
 *              measured was started. This is typically the second arg for
 *              XASTAT_TimerStart.
 */
void XASTAT_TimerStop(U16 bytes, XaTimeStat timer, TimeT start);

#define XASTAT_TimerStop(bytes, f, ctxt) if (ctxt) \
        do { \
            TimeT sysTime; \
            sysTime = OS_GetSystemTimeUs(); \
            if (BTC(stats.f).started == TRUE) { \
                if (ctxt > sysTime) \
                    ctxt = 0xFFFFFFFF - ctxt + sysTime; \
                else \
                    ctxt = sysTime - ctxt; \
                if (ctxt > BTC(stats.f).peak_latency) \
                    BTC(stats.f).peak_latency = ctxt; \
                if (((BTC(stats.f).total_time + ctxt) < BTC(stats.f).total_time) || \
                    ((BTC(stats.f).total_bytes + bytes) < BTC(stats.f).total_bytes)) { \
                    BTC(stats.f).total_time = 0; \
                    BTC(stats.f).total_bytes = 0; \
                    BTC(stats.f).samples = 0; \
                }  \
                BTC(stats.f).total_time += ctxt; \
                BTC(stats.f).total_bytes += bytes; \
                BTC(stats.f).samples++; \
                ctxt = 0; \
                BTC(stats.f).started = FALSE; \
            } \
        } while (0)

#if BT_STACK == XA_ENABLED
/*---------------------------------------------------------------------------
 * BTSTAT_Report()
 *      Output statistics to debug output device. This function is located in
 *      stack/debug.c.
 *      
 * Requires:
 *      XA_STATISTICS and XA_DEBUG are enabled
 *     
 * Parameters:
 *      Stats - pointer to statics structure in stack context structure.
 *
 *      Print - pointer to a printf() style function that displays lines
 *         of text. If this parameter is null, OS_Report() will be called.
 */
void BTSTAT_Report(const BtStatistics *Stats, void (*Print)(const char *,...));
#endif /* BT_STACK == XA_ENABLED */

#else /* XA_STATISTICS == XA_ENABLED */

#define XASTAT_PeakInc(f, i)         (void)0
#define XASTAT_PeakDec(f, d)         (void)0
#define XASTAT_PeakSet(f, i)         (void)0
#define XASTAT_CounterInc(f, i)      (void)0
#define XASTAT_Set(f, v)             (void)0
#define XASTAT_Max(f, v)             (void)0
#define XASTAT_TimerStart(f, ctxt)   (void)0
#define XASTAT_TimerStop(bytes, f, ctxt)    (void)0

#endif /* XA_STATISTICS == XA_ENABLED */

#endif /* __BTSTATS_H */
