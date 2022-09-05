/****************************************************************************
 *
 * File:
 *     bccmd.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file contains interface definitions for the code that sends and 
 *     receive data from the CSR BCSP chip interface.  
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

#include "config.h"

#if BT_EXPOSE_BCCMD == XA_ENABLED
#ifndef BCCMD_H
#define BCCMD_H

/*---------------------------------------------------------------------------
 * BCCMD layer
 *
 * This API allows BCCMD messages to be sent to and received from a CSR chip 
 * using the ABCSP interface.
 *
 * This API links to the HCI transport layer in the bluetooth protocol 
 * stack.  As such, it can be thought of as a side-band interface, since it 
 * bypasses most of the higher stack layers.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * MAX_BCCMD constant
 *     The maximum buffer size for BCCMD messages.  
 */
#define MAX_BCCMD 0x400

/*---------------------------------------------------------------------------
 * UNRELIABLE_CHANNEL constant
 *     Used to specify an unreliable channel.  
 */
#define UNRELIABLE_CHANNEL 0

/*---------------------------------------------------------------------------
 * UNRELIABLE_CHANNEL constant
 *     Used to specify a reliable channel, such as that used by the BCCMD 
 *     messages.  
 */
#define RELIABLE_CHANNEL   1

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BCCMD_RESULT type
 *
 *     Reports the return status of calls to send or receive BCCMD data.
 */
typedef U8 BCCMD_RESULT;

/** Indicates that a BCCMD message was successfully sent or received.
 */
#define BCCMD_OK               0

/** Indicates that the transmission buffer was too large to be handled by 
 *  this software.
 */
#define BCCMD_BUFF_TOO_LARGE   1

/** Indicates that the receive buffer was too small to hold all of the 
 *  received data.
 */
#define BCCMD_BUFF_TOO_SMALL   2

/** Indicates that A previous command string is still in the process of being 
 *  sent, so another cannot be started.  
 */
#define BCCMD_PREV_UNFINISHED  3

/** Indicates that the abcsp transmission software currently unable to send 
 *  data.  
 */
#define BCCMD_NOT_SENDING      4

/** Indicates that no data has been received from the CSR chip.
 */
#define BCCMD_NO_DATA_RECEIVED 5

/** Indicates that only the first part of an entire message has been 
 *  received from the CSR chip.
 */
#define BCCMD_RXMSG_FILLING    6

/** Indicates that although data was received from the CSR chip, previously 
 *  received data was overwritten and lost.
 */
#define BCCMD_DATA_LOST        7

/* End of BCCMD_RESULT */

/*---------------------------------------------------------------------------
 * BCCMD_USE_ENUM type
 *
 *     Indicates the status of a BCCMD message transfer.
 */
typedef U8 BCCMD_USE_ENUM;

/** Indicates that a BCCMD message is empty.
 */
#define BU_EMPTY   0

/** Indicates that a BCCMD message is being filled.
 */
#define BU_FILLING 1

/** Indicates that a BCCMD message is full.
 */
#define BU_FULL    2

/* End of BCCMD_USE_ENUM */

/*---------------------------------------------------------------------------
 * ITERATION_ENUM type
 *
 *     Indicates the status of an iteration loop.
 */
typedef U8 ITERATION_ENUM;

/** Indicates that the iteration process is complete.
 */
#define IE_DONE      0

/** Indicates that the iteration process is ongoing.
 */
#define IE_ITERATING 1

/* End of ITERATION_ENUM */

/*---------------------------------------------------------------------------
 * BCCMD_LOSS_ENUM type
 *
 *     Indicates the loss status of a transaction.
 */
typedef U8 BCCMD_LOSS_ENUM;

/** Indicates that no loss has occurred.
 */
#define BL_NO_LOSS   0

/** Indicates that a data loss has occurred.
 */
#define BL_DATA_LOST 1

/* End of BCCMD_LOSS_ENUM */

/*---------------------------------------------------------------------------
 * MSG_PUMP_ENUM type
 *
 *     Indicates whether further data pump calls are needed.
 */
typedef U8 MSG_PUMP_ENUM;

/** Indicates that no further data pump calls are required at this time.
 */
#define MP_NO_MORE 0

/** Indicates that additional data pump calls are still needed.
 */
#define MP_MORE    1

/* End of MSG_PUMP_ENUM */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * bccmdMsgStruct structure
 *
 *     Defines the BCCMD message structure required by the ABCSP interface.
 */
typedef struct {
    BCCMD_USE_ENUM  use;
    BCCMD_LOSS_ENUM lossState;
    S32             rdNdx;
    S32             wrNdx;
} bccmdMsgStruct;

/*---------------------------------------------------------------------------
 * bccmdBufStruct structure
 *
 *     Defines the BCCMD buffer structure required by the ABCSP interface.
 */
typedef struct {
    char data[MAX_BCCMD];
} bccmdBufStruct;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * bccmdSend()
 *
 *     This function sends a BCCMD command string to a CSR chip using the 
 *     ABCSP interface.
 *
 * Parameters:
 *     buf - Must point to a buffer of values to be sent to the CSR chip.
 *
 *     size - Must contain the number of characters to be sent from the 
 *            buffer pointed at by buf.
 * 
 * Returns:
 *
 *     BCCMD_BUFF_TOO_LARGE - The buffer passed in for transmission was too 
 *                            large to be handled by this software.
 *
 *     BCCMD_OK - The command was successfully sent to the CSR chip.
 *
 *     BCCMD_NOT_SENDING - The abcsp software currently unable to send data.  
 *                         You may wish to try again at a later time when it 
 *                         is not busy.
 *
 *     BCCMD_PREV_UNFINISHED - A previous command string is still in the 
 *                             process of being sent, so another cannot be 
 *                             started.  You may wish to try again at a later 
 *                             time after the software finishes sending the 
 *                             previous command string.
 */
BCCMD_RESULT bccmdSend(char *buf, S32 size);

/*---------------------------------------------------------------------------
 * bccmdGet()
 *
 *     This function receives a BCCMD reply string from a CSR chip using the 
 *     ABCSP interface.
 *
 * Parameters:
 *     buf - Must point to a buffer where the reply data will be copied.
 *
 *     size - Must contain the number of characters that can be copied into 
 *            the buffer pointed at by buf.
 * 
 *     bytesReadP - Must contain a pointer to a location where the actual 
 *                  number of received bytes can be recorded.
 * 
 * Returns:
 *
 *     BCCMD_NO_DATA_RECEIVED - No data was received from the CSR chip.
 *
 *     BCCMD_BUFF_TOO_SMALL - The buffer pointed at by buf was too small to 
 *                            hold all of the received data.
 *
 *     BCCMD_OK - Data was successfully received from the CSR chip.
 *
 *     BCCMD_DATA_LOST - Although data was received from the CSR chip, 
 *                       previously received data was overwritten and lost.
 *
 *     BCCMD_RXMSG_FILLING - Only the first part of an entire message has 
 *                           been received from the CSR chip.  The rest of 
 *                           the message is expected to arrive soon.
 */
BCCMD_RESULT bccmdGet(char *buf, S32 size, S32 *bytesReadP);

#endif /* BCCMD_H */ 

#endif /* BT_EXPOSE_BCCMD == XA_ENABLED */ 
