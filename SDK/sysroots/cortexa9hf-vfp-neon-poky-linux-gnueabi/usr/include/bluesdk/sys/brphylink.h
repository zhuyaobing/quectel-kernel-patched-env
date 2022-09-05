/****************************************************************************
 *
 * File:
 *     brphylink.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     BR/EDR physical link.
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

#ifndef __BRPHYLINK_H
#define __BRPHYLINK_H

#include "phylink.h"

/*---------------------------------------------------------------------------
 * BR/EDR Physical Link structure
 *
 *     Manages Physical Link.
 */
typedef struct _BrPhylLink
{
    BtPhysicalLink  pl;
 
    HciCommand      hciCmd;

    /* There is one Logical link per BR/EDR Physical link */

    /* Broadcast queue. */
    ListEntry       txBcQueue;

    /* SCO Priority token. */
    BtPriorityToken priSco;
    BtPriorityToken priScoIncoming;

} BrPhyLink;


void BRPHYLINK_Create(BrPhyLink *plink);

#endif /* __BRPHYLINK_H */

