/****************************************************************************
 *
 * File:
 *     ampphylink.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     logical link in the Management Entity.
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

#ifndef __AMPPHYLINK_H
#define __AMPPHYLINK_H

#include "me.h"
#include "phylink.h"
#include "bttypes2.h"
#include "loglink.h"

/*---------------------------------------------------------------------------
 * AmpOutConState type
 *
 *      AMPs have a substates while in BDS_OUT_CON and BDS_OUT_DISC.
 */
typedef U8 AmpOutConState;

#define AOCS_INVALID         0x00
#define AOCS_WAIT_CHAN_SEL   0x01
#define AOCS_WAIT_COMPLETE   0x02
#define AOCS_CON_FAILED      0x03
#define AOCS_CON_COMPLETE    0x04
#define AOCS_DISC_WAIT       0x05
#define AOCS_DISC_COMPLETE   0x06

/* End of AmpOutConState */


/*---------------------------------------------------------------------------
 * BtPhysicalLink structure
 *
 *     Manages Physical Link.
 */
typedef struct _AmpPhyLink
{
    BtPhysicalLink  pl;
    ListEntry       node; /* Used internally by meamp */

    /* Pointer to AMP Controller */
    void *crl;

    /* In an AMP controller the Best Effort Logical link is put in  a 
       variable
     */
    BtLogicalLink  *beLogLink;

    /* Number of allocations of beLogLink */
    U16             beLogLinkRefCount;

    /* List of AMPMGR clients */
    ListEntry       clients;

    /* List of allocated but not connected loglinks */
    ListEntry       allocLlinks;

    /* State is accessed directly by MEAMP */
    BtLinkState     state;
    AmpOutConState  outConState;
    U8              remCrlID;    /* ID of the Remote Controller */

    /* If ReqToSend is TRUE it means that L2CAP has requested the controller
     * to send data. The Controller calls txReady to get packets from the
     * phy link. The Controller can use this flag to determine when to call
     * txReady
     */
    BOOL            reqToSend;   /* Set to TRUE when there is data to send */

    /* TRUE if local device initiated creation of phylink. FALSE if the
     * remote device initiated it.
     */
    BOOL            initiator;

    /* TRUE if the phylink is being disconnected because of link key change */
    BOOL            isKeyChanged;

    /* TRUE if the phylink has incremented the remDev->refCount */
    BOOL            remDevRefCount;

    /* The AMP controller calls this callback function when it has
     * events for the physical link. It passes it AmpControllerEvent
     * type events.
     */
    BtCallbackEx    crlCallback;

    /* Used by HCI-based controllers to uniquely identify this PhyLink */ 
    U8              hciHandle;

    /* Packet (if any) received from BtPhysicalLink but not yet delivered */
    BtPacket       *hciTxReady;

} AmpPhyLink;


/****************************************************************************
 *
 * Function specific to AMP Physical Links
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AMPPHYLINK_Create()
 *
 *     Constructor for AMP physical link. 
 *
 * Parameters:
 *     Plink - Pointer to physical link object to be constructed
 *
 * Returns:      
 *     void
 */

void AMPPHYLINK_Create(AmpPhyLink *Plink);

/*---------------------------------------------------------------------------
 * AMPPHYLINK_AddClient()
 *
 *     Add client to the client list of the physical link 
 *
 * Parameters:
 *     ctx - Pointer to physical link object
 *
 *     client - pointer to ListEntry of client.
 *
 * Returns:      
 *     void
 */
void AMPPHYLINK_AddClient(AmpPhyLink *ctx, ListEntry *client);

/*---------------------------------------------------------------------------
 * AMPPHYLINK_RemoveClient()
 *
 *     Remove a client from the physical link's client list. If this is the
 *     last client and there are no logical links associated with the
 *     the physical link then the physical link will be disconnected. 
 *
 * Parameters:
 *     ctx - Pointer to physical link object
 *
 *     client - pointer to ListEntry of client.
 *
 * Returns:      
 *     BT_STATUS_PENDING - The client has been successfully removed and a 
 *         disconnect request has been initiated successfully. 
 * 
 *     BT_STATUS_SUCCESS - The client has been successfully removed but the
 *         physical link is not being disconnected because it has other
 *         clients or logical links. 
 *
 *     BT_STATUS_INVALID_PARM - The client is not associated with this channel.
 *
 *     BT_STATUS_FAILED - The client has been removed but the phy link 
 *         disconnect failed.
 *
 *     BT_STATUS_NO_CONNECTION - The client has been removed but the phy link
 *         is no longer active so no event about disconnect will be given.
 */
BtStatus AMPPHYLINK_RemoveClient(AmpPhyLink *ctx, ListEntry *client);

/*---------------------------------------------------------------------------
 * AMPPHYLINK_RemoveAllClients()
 *
 *     Remove all the clients associated with the physical link. The
 *     associated logical links are not affected. 
 *
 * Parameters:
 *     ctx - Pointer to physical link object
 *
 * Returns:
 *     void
 */
void AMPPHYLINK_RemoveAllClients(AmpPhyLink *ctx);

/*---------------------------------------------------------------------------
 * AMPPHYLINK_DoPhyLinkClientsExist()
 *
 *     Return TRUE if there are clients or logical links assocaited with 
 *     the AMP physical link otherwise return FALSE. 
 *
 * Parameters:
 *     ctx - Pointer to physical link object
 *
 * Returns:
 *     TRUE - clients and/or logical links exist
 *
 *     FALSE - no clients or logical links exist
 */
BOOL AMPPHYLINK_DoPhyLinkClientsExist(AmpPhyLink *ctx);


void AMPPHYLINK_Close(AmpPhyLink *Plink, BtErrorCode discReason);

#endif /* __AMPPHYLINK_H */
