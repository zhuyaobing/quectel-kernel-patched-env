/***************************************************************************
 *
 * File:
 *     radiomgi.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file contains internal definitions for the Radio Manager.
 *
 * Created:
 *     April 11, 2003
 *
 * Copyright 2003-2017 OpenSynergy GmbH.
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

#ifndef __RADIOMGI_H
#define __RADIOMGI_H

#include "bttypes.h"

/*--------------------------------------------------------------------------*
 * Typedefs and Data structures used internally by Radio Manager            *
 *--------------------------------------------------------------------------*/
 
/* Radio Manager States */
#define RMGR_STATE_SHUTDOWN             0
#define RMGR_STATE_SHUTTING_DOWN        1
#define RMGR_STATE_RESETTING            2
#define RMGR_STATE_INITIALIZING         3
#define RMGR_STATE_READY                4

#define RMGR_TRAN_STATE_DOWN            0
#define RMGR_TRAN_STATE_INIT            1
#define RMGR_TRAN_STATE_DEINIT          2
#define RMGR_TRAN_STATE_READY           3

#define RMGR_HCI_STATE_DOWN             0
#define RMGR_HCI_STATE_READY            1

#define RMGR_MOD_STATE_DOWN             0
#define RMGR_MOD_STATE_INIT             1
#define RMGR_MOD_STATE_DEINIT           2
#define RMGR_MOD_STATE_READY            3

/* Parms for delayed processing of transport events */
typedef struct _RmgTranParms {
    TranEvent   event;                /* Event received from the transport */
    BtStatus    status;               /* Status of the event from the transport */
} RmgTranParms;

/* Radio Manager context */
typedef struct _BtRmgContext {
    U8              retries;          /* Number of retries during init */
    U8              rmState;          /* Radio manager state           */
    U8              tranState;        /* Transport state               */
    U8              hciState;         /* HCI state                     */
    U8              modState;         /* Radio Module state            */
    EvmTimer        timer;
    EvmTimer        retryTimer;
    RmgTranParms    tranParms;
} BtRmgContext;

#endif /* __RADIOMGI_H */

