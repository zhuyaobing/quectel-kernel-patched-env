/****************************************************************************
 *
 * File:
 *     overide.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:  This file is a private include file for stacklib that 
 *     points back to stacklib_override.  This allows projects including 
 *     stacklib to have their own overide.h file.  Any competing overide.h 
 *     file may contain application-specific or profile-specific overrides 
 *     but MUST also include stacklib_override.h and not conflict with any 
 *     core stack overrides.
 *
 * Copyright 2000-2017 OpenSynergy GmbH
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
 * OpenSynergy GmbH which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifdef LICENSE_CHECK
#define XA_HEADER_H "ippro.h"
#define IPP_CTX ipp_context ipp;
#define BD_ADDR_HOOK(a) if (!ipp_classify(&a)) { \
BtStatus status = RMGR_RadioShutdown(); \
if ((status != BT_STATUS_SUCCESS) && \
    (status != BT_STATUS_PENDING)) { \
        RMGR_RadioDeinit(); \
} \
Xlogv(TAG, "Verify BD_ADDR."); \
return; \
}

#define CONNECTION_HOOK \
    if (!ipp_classify_family(&MEC(bdaddr).bda)) { \
        BtStatus status = RMGR_HciReset(); \
        if (status == BT_STATUS_FAILED) { \
            RMGR_RadioDeinit(); \
        } \
        return; \
    }
#endif /* LICENSE_CHECK */

#define USE_SHA XA_ENABLED

#include "stacklib_override.h"

/* End of file */
