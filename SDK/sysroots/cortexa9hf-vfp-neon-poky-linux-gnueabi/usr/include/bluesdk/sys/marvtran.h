/****************************************************************************
 *
 * File:
 *     marvtran.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Defines an Marvell 802.11 HCI transport.
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

#ifndef __MARVTRAN_H
#define __MARVTRAN_H

#include "marvtran_i.h"

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * MRV_Create()
 *
 *     Primary access point for the Marvell transport layer.  This call
 *     initializes the Marvell transport instance.
 *
 * Parameters:
 *     Marv - Memory representing the transport instance to initialize.
 *
 * Returns:
 *     <ptr> - Operation completed successfully and the transport is ready 
 *             to use.
 *
 *     0 - The operation failed.
 */
IHciTransport* MRV_Create(MarvHciTransport* Marv);

#endif /* __MARVTRAN_H */
