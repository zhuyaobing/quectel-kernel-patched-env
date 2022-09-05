/****************************************************************************
 *
 * File:
 *     iloglink.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
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

#ifndef __ILOGLINK_H
#define __ILOGLINK_H

#include "sys/ilink.h"
#include "sys/phylink.h"

#if 0
/*---------------------------------------------------------------------------
 * LOGLINK::Init()
 *
 *     Initialize the logical link. 
 *
 * Parameters:
 *     ctx - Pointer to object containing the Logical Link interface
 *
 *     plink - Pointer to physical link to which the logical link runs over.
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - operation completed successfully
 *
 */
typedef BtStatus (*BtLgLinkInitFunc)(void *ctx, BtPhysicalLink *plink);


/*---------------------------------------------------------------------------
 * LOGLINK::Init()
 *
 *     Return TRUE if the both logical links run over the same physical link
 *     otherwise return FALSE.
 *
 * Parameters:
 *     ctx - Pointer to object containing the Logical Link interface
 *
 *     ctx2 - Pointer to second object containing the Logical link interface.
 *
 * Returns:      
 *     TRUE - both logical links run over the same physical link
 *
 *     FALSE - the two logical links do not run over the same physical link.
 *
 */
typedef BOOL (*BtLgLinkIsLinkFunc)(void *ctx, void *ctx2);

/*---------------------------------------------------------------------------
 * LOGLINK::MapCidToChanel()
 *
 *     Return a pointer to the L2CAP channel identified by ChannelId that 
 *     operates over the logical link. If the channel is not found return 0.
 *
 * Parameters:
 *     ctx - Pointer to object containing the Logical Link interface
 *
 *     ChannelId - L2CAP channel ID. 
 *
 * Returns:      
 *     Pointer to L2CAP channel or 0.
 *
 */
typedef void * (*BtLgLinkMapCidToChannelFunc)(void *Llink, U16 ChannelId);

#endif /* 0 */
/*---------------------------------------------------------------------------
 * BtLogicalLink Interface structure
 *
 *     Logical Link Interface.
 */
#if 0 /* DWS size */
typedef struct _iLogicalLink
{
    BtLgLinkInitFunc            init;
    /*BtLgLinkIsLinkFunc          isLink; DWS size */
    /*BtLgLinkMapCidToChannelFunc mapCidToChannel; DWS size */

} iLogicalLink;

#endif

#endif /* __ILOGLINK_H */

