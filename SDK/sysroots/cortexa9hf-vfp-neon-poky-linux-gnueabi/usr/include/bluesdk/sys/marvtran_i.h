/****************************************************************************
 *
 * File:
 *     marvtran_i.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Internal definitions for Marvell 802.11 HCI transport.
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

#ifndef __MARVTRAN_I_H
#define __MARVTRAN_I_H

#include "bttypes.h"
#include "ihcitransport.h"

#include <initguid.h>
#include "windows.h"
#include "mrvsdio_if.h"

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/
#define MRV_HCI_RX_SIZE  2048
#define MRV_HCI_NUM_RX   5

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * MarvHciPacket type
 *
 *     Represents a defragmented HciPacket.
 */

typedef struct _MarvHciPacket
{
   ListEntry node;
   
   /*Original HciPacket structure*/
   HciPacket* pstPacket;

   /*Original HciPacket flags*/
   HciBufFlags flags;

   /*Buffer containing the defragmented HciPacket data.*/
   U8* buffer;

   /*Lenght of the data in buffer.*/
   U32 buffLen;
} MarvHciPacket;

/* End of MarvHciPacket */

/*---------------------------------------------------------------------------
 * MarvOpType type
 *
 *     Enumeration of available operations.
 */
typedef enum _MarvOpType
{
   MARV_OP_INVALID = 0,
   MARV_OP_WRITE = IOCTL_MRVSDIO_TX,
   MARV_OP_READ = IOCTL_MRVSDIO_RX
} MarvOpType;

/* End of MarvOpType */

typedef struct _MarvOp MarvOp;
typedef struct _MarvPipe MarvPipe;
typedef struct _MarvHciTransport MarvHciTransport;

typedef void (*MarvOpCompleteFunc)(XaStatus OpStatus, MarvOp* Op, MarvPipe* Pipe);

/*---------------------------------------------------------------------------
 * MarvOp type
 *
 *     Structure which defines an asynchronous operation.
 */

struct _MarvOp
{
   ListEntry node;

   /*Asyncronous operation ID.*/
   U8 id;

   /*Operation to perform.*/
   MarvOpType type;

   /*Tracking member for async operations.*/
   OVERLAPPED overlap;

   union
   {
      struct
      {
         MarvHciPacket* hciPkt;
      } tx;

      struct
      {
          U8* buffer;
          U16 bufLen;
      } rx;
   } p;

   /*
   Operation completion function.
   Called after device driver finishes a 
   transaction.
   */
   MarvOpCompleteFunc complete;
};

/* End of MarvOp */

/*---------------------------------------------------------------------------
 * MarvPipe type
 *
 *     Structure which represents a packet datapath.
 *
 */

struct _MarvPipe
{
   /*Thread which processes the pipe.*/
   HANDLE hThread;
   volatile LONG bThreadStop;

   /*Handle to send op's to.*/
   MarvHciTransport* parent;

   /*Event to signal a new addition to waitingOps.*/
   HANDLE hNewOpEvent;

   /*Operations waiting to be given to device.*/
   ListEntry waitingOps;

   /*Operations given to device.*/
   U32 maxPending;
   ListEntry pendingOps;

   /*Operations that have completed but not given to the stack.*/
   ListEntry completedOps;

   /*TRUE if initialized*/
   BOOL bInitialized;

   /*
   Boolean.  If set to >0 stops the flow of OP's.
   OP's can still be queue'd but will not be sent
   to the device.
   */
   volatile LONG bDisabled;
};

/* End of MarvPipe */

struct _MarvHciTransport
{
   /*Must be first!*/
   IHciTransport  trans;

#if BT_SINGLE_HW_INTERRUPT == XA_DISABLED
   CRITICAL_SECTION critical;
#endif

   /*Callback handler*/
   BtHandlerEx *handler;

   /*Pipe used for all write operations*/
   MarvPipe pipeWrite;

   /*Pipe used for all write operations*/
   MarvPipe pipeRead;

   /*Global flag for tracking transport errors.*/
   BOOL bError;

   /*Handle to send op's to.*/
   HANDLE hFile;

   /*Marvell additions*/
   S32 port;
   HANDLE hBuffAvail;
};

/*IHciTransport entry points*/
BtStatus MRV_HcitranInit(IHciTransport *ctx, BtHandlerEx *handler);
BtStatus MRV_HcitranDeinit(IHciTransport *ctx);
BtStatus MRV_HcitranBufferAvailable(IHciTransport *ctx, HciBufFlags type);
BtStatus MRV_HcitranSendData(IHciTransport *ctx, HciPacket *packet);
void MRV_HcitranPause(IHciTransport *ctx);
void MRV_HcitranResume(IHciTransport *ctx);

/*Controller callback helpers*/
HciBuffer* marvGetBuffer(MarvHciTransport* Marv, HciBufFlags Type, U16 Len);
void marvRxBufferInd(MarvHciTransport* Marv, HciBuffer* Buffer, BtStatus Status);
void marvPacketSent(MarvHciTransport* Marv, HciPacket *Packet);

#endif /* __MARVTRAN_I_H */
