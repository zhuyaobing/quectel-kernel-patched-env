/****************************************************************************
 *
 * File:
 *     prescani.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Prescan interface definition
 * 
 * Copyright 2010-2017 OpenSynergy GmbH.
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

#ifndef __PRESCANI_H
#define __PRESCANI_H

#include "prescan.h"

#if BT_PRESCAN == XA_ENABLED

/* Context memory used by this module */
typedef struct _PrescanContext {
    /* All registered consumers */
    ListEntry consumers;

    /* Mask of all consumers' producer preferences */
    PrescanProducer producerMask;

    /* Mask of all consumer's event masking preferences */
    PrescanEventType eventMask;
} PrescanContext;

#endif /* BT_PRESCAN == XA_ENABLED */

#endif /* __PRESCANI_H */
