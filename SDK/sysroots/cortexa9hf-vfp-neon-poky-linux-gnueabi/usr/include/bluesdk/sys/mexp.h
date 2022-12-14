/***************************************************************************
 *
 * File:
 *     mexp.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file contains functions exported to the Event Manager by the
 *     Management Entity.
 *
 * Created:
 *     July 20, 1999
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

#ifndef __MEXP_H
#define __MEXP_H

#include "bttypes.h"

/*---------------------------------------------------------------------------
 * Prototype:     BtStatus ME_Init(void)
 *
 * Descriptions:  Initialize the Management Entity. 
 *
 * Parameters:    void
 *
 * Returns:       BT_STATUS_SUCCESS - ME initialized successfully.
 *
 *                BT_STATUS_FAILED - ME failed to initialize.
 */
BOOL ME_Init(void);

/*---------------------------------------------------------------------------
 * Prototype:     void ME_Deinit(void)
 *---------------------------------------------------------------------------
 *
 * Description:   Deinitialize the Management Entity. 
 *
 * Parameters:    void
 *
 * Returns:       void
 */
void ME_Deinit(void);

#endif /* __MEXP_H */

