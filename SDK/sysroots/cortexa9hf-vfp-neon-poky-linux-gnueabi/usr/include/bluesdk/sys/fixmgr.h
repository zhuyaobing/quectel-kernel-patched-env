/****************************************************************************
 *
 * File:
 *     fixmgr.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     Fixed channel Manager.
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

#ifndef __FIXMGR_H
#define __FIXMGR_H

#include "osapi.h"
#include "bttypes.h"
#include "sys/l2signal.h"
#include "a2me.h"
#include "l2cap.h"
#include "l2capagent.h"
#include "me.h"
#include "sys/l2conless.h"
#if NUM_LE_DEVICES > 0
#include "sys/le_signal_i.h"
#include "sys/le_att_i.h"
#include "sys/le_sec_i.h"
#include "lel2cap.h"
#endif /* NUM_LE_DEVICES > 0 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/
#define MAX_FIXED_CHANNEL_ID 0x3F


/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Data structures
 *
 ****************************************************************************/

typedef void *(*FixMgrAllocClientInstance)(U8 remDevIndex, U8 clientIndex, void *channel);

/*---------------------------------------------------------------------------
 * Fixed Channel Client Static
 *
 *     Holds the static class data of a Fixed Channel Client
 */
typedef struct _FixChannelClientStatic
{
    U16          cid;      /* Fixed channel CID */
    L2capPsm     psm;
    ListEntry    handlers; /* List of handlers */
    BtHandlerEx  handler;  /* Handler for psm */

    /* Function for allocating client instance */
    FixMgrAllocClientInstance   allocClientInstance;
    BtCallbackEx                deallocClientInstance;


} FixChannelClientStatic;


#if NUM_LE_DEVICES > 0
/*---------------------------------------------------------------------------
 * Fixed Channel Client Static
 *
 *     Holds the static class data of a Fixed Channel Client
 */
typedef struct _FixChannelLeClientStatic
{
    U16           cid;      /* Fixed channel CID */
    ListEntry     handlers; /* List of handlers */
    BtHandlerEx   handler;   /* Handler for psm */

    /* Function for allocating client instance */
    FixMgrAllocClientInstance   allocClientInstance;
    BtCallbackEx                deallocClientInstance;


} FixChannelLeClientStatic;
#endif /* NUM_LE_DEVICES > 0 */

/*---------------------------------------------------------------------------
 * Fixed Channel Manager Context for BR/EDR links
 *
 */
typedef struct _FixMgrBrContext
{
    /******Start of Fixed Channel Client objects *****************************/
    /* Each Fixed Channel Client needs to have an array of context structures 
     * of size NUM_BT_DEVICES. This is the dynamic portion of Fixed Channel Client
     */
    /* Signaling fixed channel*/
    BtL2SignalContext       signalCtx[NUM_BT_DEVICES];

#if NUM_AMPS > 0
    /* A2ME fixed channels */
    BtA2meContext           a2meCtx[NUM_BT_DEVICES];
#endif /* NUM_AMPS > 0 */

    /* L2CAP Connectionless fixed channels */
    BtL2ConlessContext      conlessCtx[NUM_BT_DEVICES];

#if NUM_LE_DEVICES > 0
    /* BR/EDR Security Manager Channel */
    BtBrSecContext          brSecCtx[NUM_BT_DEVICES];
#endif /* NUM_LE_DEVICES > 0 */

    /* Add the next one here */


    /******End of Fixed Channel Client objects  ******************************/


    /* Array of Fixed Channel Clients */
    FixChannelClientStatic  clients[L2CAP_NUM_BR_FIXED_CHANNELS];

    /* Arrary of ATT contexts */
#if NUM_LE_DEVICES > 0
    BtBrAttContext          leAttCtx[NUM_BT_DEVICES];
#endif


} FixMgrBrContext;


#if NUM_LE_DEVICES > 0
/*---------------------------------------------------------------------------
 * Fixed Channel Manager Context for LE links
 *
 */
typedef struct _FixMgrLeContext
{
    /******Start of Fixed Channel Client objects *****************************/
    /* Each Fixed Channel Client needs to have an array of context structures 
     * of size NUM_LE_DEVICES. This is the dynamic portion of Fixed Channel Client
     */
    /* LE ATT fixed channels */
    BtLeAttContext          leAttCtx[NUM_LE_DEVICES];

    /* LE Signaling  fixed channels */
    BtLeSignalContext       leSignalCtx[NUM_LE_DEVICES];

    /* LE Security Manager fixed channels */
    BtLeSecContext          leSecCtx[NUM_LE_DEVICES];

    /* Add the next one here */


    /******End of Fixed Channel Client objects  ******************************/


    /* Array of Fixed Channel Clients */
    FixChannelLeClientStatic  clients[LEL2CAP_NUM_FIXED_CHANNELS];

} FixMgrLeContext;
    
#endif /* NUM_LE_DEVICES > 0 */


/*---------------------------------------------------------------------------
 * FIXMGR_Init()
 *
 *     Initialize the Fixed Channel Manager. 
 *
 * Parameters:
 *     void
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - initialized successfully.
 *
 *     BT_STATUS_FAILED - failed to initialize.
 */
BtStatus FIXMGR_Init(void);

/*---------------------------------------------------------------------------
 * FIXMGR_Deinit()
 *
 *     Deinitialize the Fixed Channel Manager. 
 *
 * Parameters:
 *     void
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - initialized successfully.
 *
 *     BT_STATUS_FAILED - failed to initialize.
 */
void FIXMGR_Deinit(void);


/*---------------------------------------------------------------------------
 * FIXMGR_CreateBrFixedClients()
 *
 *     Create Instances of Fixed channel clients for BR/EDR links
 *
 * Parameters:
 *     llink - pointer to BtLogicalLink structure representing the logical link
 *
 *     remDevIndex - index of BtRemoteDevice
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - initialized successfully.
 *
 *     BT_STATUS_FAILED - failed to initialize.
 */
BtStatus FIXMGR_CreateBrFixedClients(BtLogicalLink *llink, I8 remDevIndex);

/*---------------------------------------------------------------------------
 * FIXMGR_DeleteBrFixedClients()
 *
 *     Delete Instances of Fixed channel clients for BR/EDR
 *
 * Parameters:
 *     llink - pointer to BtLogicalLink structure representing the logical link
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - initialized successfully.
 *
 *     BT_STATUS_FAILED - failed to initialize.
 */
BtStatus FIXMGR_DeleteBrFixedClients(BtLogicalLink *llink);

#if NUM_LE_DEVICES > 0
/*---------------------------------------------------------------------------
 * FIXMGR_CreateLeFixedClients()
 *
 *     Create Instances of Fixed channel clients for BR/EDR links
 *
 * Parameters:
 *     llink - pointer to BtLogicalLink structure representing the logical link
 *
 *     remDevIndex - index of BtRemoteDevice
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - initialized successfully.
 *
 *     BT_STATUS_FAILED - failed to initialize.
 */
BtStatus FIXMGR_CreateLeFixedClients(BtLogicalLink *llink, I8 remDevIndex);

/*---------------------------------------------------------------------------
 * FIXMGR_DeleteLeFixedClients()
 *
 *     Delete Instances of Fixed channel clients for LE
 *
 * Parameters:
 *     llink - pointer to BtLogicalLink structure representing the logical link
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - initialized successfully.
 *
 *     BT_STATUS_FAILED - failed to initialize.
 */
BtStatus FIXMGR_DeleteLeFixedClients(BtLogicalLink *llink);

#endif /* NUM_LE_DEVICES > 0 */


/*---------------------------------------------------------------------------
 * FIXMGR_IsRemoteFixedChannel()
 *
 *     Return TRUE if the fixed CID is supported by the remote device otherwise 
 *     return FALSE. 
 *
 * Parameters:
 *     void
 *
 * Returns:      
 *     TRUE - fixed CID is supported.
 *
 *     FALSE - fixed CID is not supported.
 */
BOOL FIXMGR_IsRemoteFixedChannel(BtRemoteDevice *remDev, U8 FixedId);


/*---------------------------------------------------------------------------
 * FIXMGR_FindClientIndexEx()
 *
 *     Return the index in the remDev->fixedClients array for the Fixed
 *     Channel client with the given fixed ID (fixed channel CID). If the fixed
 *     id is not supported then 0xff is returned;
 *
 * Parameters:
 *     fixedId - CID of the fixed channel.
 *
 *     le - if TRUE search LE otherwise search BR/EDR
 *
 * Returns:      
 *     Index into the fixedClients array.
 */
U8 FIXMGR_FindClientIndexEx(U16 fixedId, BOOL le);

#define FIXMGR_FindClientIndex(fi) FIXMGR_FindClientIndexEx(fi, FALSE)

/*---------------------------------------------------------------------------
 * FIXMGR_RegisterHandler()
 *
 *     Register a handler for the Fixed Channel Client identified by clientIndex. 
 *
 * Parameters:
 *     clientIndex - Index for the Fixed Channel Client.
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - handler registered
 *
 *     BT_STATUS_FAILED - clientIndex not valid.
 */
BtStatus FIXMGR_RegisterHandler(U8 clientIndex, BtHandlerEx *handler);


/*---------------------------------------------------------------------------
 * FIXMGR_NotifyHandlers()
 *
 *     Call all the handlers for the Fixed Channel Client identified by 
 *     clientIndex with the given event e. 
 *
 * Parameters:
 *     clientIndex - Index for the Fixed Channel Client.
 *
 *     e - pointer to event to be passed to each handler.
 *
 * Returns: 
 *     void
 */
void FIXMGR_NotifyHandlers(U8 clientIndex, BtEventEx *e);

#endif /* __FIXMGR_H */
