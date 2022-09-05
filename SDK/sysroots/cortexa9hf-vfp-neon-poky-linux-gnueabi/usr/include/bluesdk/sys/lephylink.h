/****************************************************************************
 *
 * File:
 *     lephylink.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     LE physical link.
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

#ifndef __LEPHYLINK_H
#define __LEPHYLINK_H

#include "sys/phylink.h"

/*---------------------------------------------------------------------------
 * LE Physical Link structure
 *
 *     Manages Physical Link.
 */
typedef struct _LePhylLink
{
    BtPhysicalLink  pl;

} LePhyLink;


void LEPHYLINK_Create(LePhyLink *plink);

#endif /* __LEPHYLINK_H */

