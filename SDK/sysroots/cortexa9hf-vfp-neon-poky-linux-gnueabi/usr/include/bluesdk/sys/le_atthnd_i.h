/****************************************************************************
 *
 * File:
 *     le_atthnd_i.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Internal types, defines, and prototypes for implementing
 *     ATT operations.
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

#ifndef __LE_ATTHND_I_H
#define __LE_ATTHND_I_H

#include "bttypes2.h"

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * LeAttRole type
 *
 *     Defines the LE Attrubute Protocol Role.
 */
typedef U8 LeAttRole;

#define LEATT_CLIENT                    0x01
#define LEATT_SERVER                    0x02

/* End of LeAttRole */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * LeAttHandler structure
 *
 *     An LeAttHandler is registered with LEATT_Register() to receive events
 *     from the ATT channel.
 */
struct _LeAttHandler {
    BtHandlerEx     h;
    LeAttRole       role;
};

#endif /* __LE_ATTHND_I_H */
