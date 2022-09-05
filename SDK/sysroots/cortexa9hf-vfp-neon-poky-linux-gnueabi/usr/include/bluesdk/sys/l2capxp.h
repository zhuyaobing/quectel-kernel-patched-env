/***************************************************************************
 *
 * File:
 *     l2capxp.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Funtion exports for lower layer (ME).
 *
 * Created:
 *     July 15, 1999
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

#ifndef __L2CAPXP_H
#define __L2CAPXP_H

#include "bttypes2.h"

/*---------------------------------------------------------------------------
 * Prototype:     BtStatus L2CAP_Init()
 *
 * Descriptions:  Initialize the L2CAP Protocol.
 *
 * Parameters:    void
 *
 * Returns:       BT_STATUS_SUCCESS - L2CAP is initialized.
 *
 *                BT_STATUS_FAILED - L2CAP initialization failed.
 */
BtStatus L2CAP_Init(void);


/*---------------------------------------------------------------------------
 * Prototype:     void L2CAP_Deinit()
 *
 * Description:   Denitialize the L2CAP Protocol.
 *
 * Parameters:    void
 *
 * Returns:       void
 */
void L2CAP_Deinit(void);

#endif /* __L2CAPXP_H */

