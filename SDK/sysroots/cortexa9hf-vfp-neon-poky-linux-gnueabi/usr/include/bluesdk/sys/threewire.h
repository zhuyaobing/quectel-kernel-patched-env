/****************************************************************************
 *
 * File:
 *     threewire.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file is part of an Three-Wire Transport implementation.  It provides 
 *     header information used by the code that provides the transport layer 
 *     interface between the upper HCI layer and the lower Three-wire 
 *     layer.  
 *
 * Created:
 *     April 5, 2013
 *
 * Copyright 2006-2017 OpenSynergy GmbH.
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

#ifndef __THREEWIRE_H
#define __THREEWIRE_H

#include "config.h"
#include "bttypes.h"
#include "hcitrans.h"


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * THREEWIRE_Init()
 *     This function is invoked by the upper HCI layer whenever the 
 *     transport layer needs to initialize the HCI Three-Wire transport and UART 
 *     hardware drivers.  
 *
 * Parameters:
 *     tranCallback - This is the routine that should be invoked to signal 
 *     the upper HCI layers when a successful initialization or shutdown 
 *     event has occurred.  
 *
 * Returns:
 *     This function returns a Bluetooth Status enumeration indicating 
 *     things such as whether the operation succeeded, whether the operation 
 *     is currently pending, or whether the operation encountered some type 
 *     of error.  
 */
BtStatus THREEWIRE_Init(TranCallback tranCallback);

/*---------------------------------------------------------------------------
 * THREEWIRE_Shutdown()
 *     This function is invoked by the upper HCI layer whenever the 
 *     transport layer needs to be shut down.  
 *
 * Returns:
 *     This function returns a Bluetooth Status enumeration indicating 
 *     things such as whether the operation succeeded, whether the operation 
 *     is currently pending, or whether the operation encountered some type 
 *     of error.  
 */
BtStatus THREEWIRE_Shutdown(void);

#endif /* __THREEWIRE_H */ 


