/***************************************************************************
 *
 * File:
 *     xa_abcsp.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     HCI Transport - ABCSP integration definitions for inclusion by
 *     ABCSP library. This file must be added to the ASBCP library
 *     includes list, "src\abcsp_private.h" is the recommended file.
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

#include "abcsp.h"

/*----------------------------------------------------------------------------
 * BcspChannel type
 *
 *     Channel types defined by BCSP specification.
 */
#define BCSP_CHAN_ACK   0 /* for ACKs only */
#define BCSP_CHAN_LE    1 /* (unreliable) is for link establishment */
#if BT_EXPOSE_BCCMD == XA_ENABLED
#define BCSP_CHAN_BCCMD 2 /* (reliable) is for BCCMD commands and events */
#endif
#define BCSP_CHAN_HCI   5 /* (reliable) is for HCI commands and events */
#define BCSP_CHAN_ACL   6 /* (reliable) is for ACL data */
#define BCSP_CHAN_SCO   7 /* (unreliable and reliable) is for SCO data */

extern ABCSP_RXMSG *xa_rxmsg_create(unsigned chan, unsigned len);
extern char *xa_rxmsg_getbuf(ABCSP_RXMSG *m, unsigned *n);
extern void xa_rxmsg_write(ABCSP_RXMSG *m, char *buf, unsigned n);
extern void xa_rxmsg_complete(ABCSP_RXMSG *m);
extern void xa_delivermsg(ABCSP_RXMSG *m, unsigned chan, unsigned rel);
extern void xa_rxmsg_destroy(ABCSP_RXMSG *m);
extern void xa_cancel_bcsp_timer(void);
extern void xa_cancel_tshy_timer(void);
extern void xa_cancel_tconf_timer(void);
extern void xa_start_bcsp_timer(void);
extern void xa_start_tshy_timer(void);
extern void xa_start_tconf_timer(void);
extern void xa_txmsg_init_read(ABCSP_TXMSG *msg);
extern unsigned xa_txmsg_length(ABCSP_TXMSG *msg);
extern const char *xa_txmsg_getbuf(ABCSP_TXMSG *msg, unsigned *buflen);
extern void xa_txmsg_taken(ABCSP_TXMSG *msg, unsigned ntaken);
extern void xa_txmsg_done(ABCSP_TXMSG *msg);
extern char *xa_uart_gettxbuf(unsigned *bufsiz);
extern void xa_uart_sendbytes(char *buf, unsigned n);
extern void xa_panic(unsigned int e);
extern void xa_free(void *m);
extern void *xa_malloc(unsigned n);
extern void *xa_zmalloc(unsigned n);
extern void xa_event(unsigned e);
extern void xa_req_pumptxmsgs(void);

/*
 * From config_txmsg.h
 */
/* Obtain buffer for uart output */
#define  ABCSP_UART_GETTXBUF    xa_uart_gettxbuf

/* Pass a block of bytes to the output uart */
#define  ABCSP_UART_SENDBYTES   xa_uart_sendbytes

/* Initialise reading a bcsp transmit message */
#define  ABCSP_TXMSG_INIT_READ  xa_txmsg_init_read

/* How long is a transmit message */
#define  ABCSP_TXMSG_LENGTH     xa_txmsg_length

/* Access raw message bytes in a message */
#define  ABCSP_TXMSG_GETBUF     xa_txmsg_getbuf

/* Tell message how many bytes have been read */
#define  ABCSP_TXMSG_TAKEN      xa_txmsg_taken

/* Signal that message has been delivered */
#define  ABCSP_TXMSG_DONE       xa_txmsg_done


/*
 * From config_event.h 
 */
/* Report an event from the abcsp code */
#define ABCSP_EVENT             xa_event

/* Request external code call abcsp_pumptxmsgs */
#define ABCSP_REQ_PUMPTXMSGS    xa_req_pumptxmsgs

/*
 * From config_rxmsg.h 
 */
/* Pass abcsp message to higher level code */
#define ABCSP_DELIVERMSG        xa_delivermsg

/* Create a abcsp receive message */
#define ABCSP_RXMSG_CREATE      xa_rxmsg_create

/* Obtain storage to write to a message */
#define ABCSP_RXMSG_GETBUF      xa_rxmsg_getbuf

/* Write to a higher-level message */
#define ABCSP_RXMSG_WRITE       xa_rxmsg_write

/* Finish a higher-level message */
#define ABCSP_RXMSG_COMPLETE    xa_rxmsg_complete

/* Destroy a abcsp receive message */
#define ABCSP_RXMSG_DESTROY     xa_rxmsg_destroy

/*
 * From config_timer.h 
 */
/* Start the bcsp ack timeout timer */
#define ABCSP_START_BCSP_TIMER      xa_start_bcsp_timer

/* Start the bcsp-le tshy timer */
#define ABCSP_START_TSHY_TIMER      xa_start_tshy_timer

/* Start the bcsp-le tconf timer */
#define ABCSP_START_TCONF_TIMER     xa_start_tconf_timer

/* Cancel the bcsp ack timeout timer */
#define ABCSP_CANCEL_BCSP_TIMER     xa_cancel_bcsp_timer

/* Cancel the bcsp-le tshy timer */
#define ABCSP_CANCEL_TSHY_TIMER     xa_cancel_tshy_timer

/* Cancel the bcsp-le tconf timer */
#define ABCSP_CANCEL_TCONF_TIMER    xa_cancel_tconf_timer

/*
 * From config_malloc.h
 */
/* Allocate a block of memory */
#define ABCSP_MALLOC        xa_malloc

/* Allocate a block of zeroed memory */
#define ABCSP_ZMALLOC       xa_zmalloc

/* Liberate a block of memory */
#define ABCSP_FREE          xa_free

/*
 * From config_panic.h
 */
/* Report a panic from the abcsp code */
#define ABCSP_PANIC         xa_panic

