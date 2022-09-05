/***************************************************************************
 *
 * File:
 *     rfcexp.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Function exports to lower layer (ME).
 *
 * Created:
 *     September 13, 1999
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

#ifndef __RFCEXP_H
#define __RFCEXP_H

#include "bttypes.h"

/*---------------------------------------------------------------------------
 * Prototype:     BtStatus RF_Init(void)
 *
 * Description:   Initialize RFCOMM.  Register with L2CAP, initialize state and
 *                other variables.
 *            
 * Parameters:    void
 *
 * Returns:       BT_STATUS_SUCCESS - initialization successful.
 *
 *                BT_STATUS_FAILED - initialization failed.   
 */
BtStatus RF_Init(void);

/*---------------------------------------------------------------------------
 * Prototype:     RF_Deinit()
 *
 * Description:   Deinitialize RFCOMM.  Deregister with L2CAP.
 *            
 * Parameters:    void
 *
 * Returns:       void   
 */
void RF_Deinit(void);

#endif /* __RFCEXP_H */

