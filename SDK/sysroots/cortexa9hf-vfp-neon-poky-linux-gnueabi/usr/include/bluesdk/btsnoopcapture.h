/***************************************************************************
 *
 * File:
 *     $Id: btsnoopcapture.h 15730 2016-11-04 14:52:56Z mpa $
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     $Revision: 15730 $
 *
 * Description:
 *     OpenSynergy BTSnoop interface header file.
 *
 * Created:
 *     November 4, 2016
 *
 * Copyright 2002-2017 OpenSynergy GmbH.
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

#include "btconfig.h"
#include "bttypes.h"

/*---------------------------------------------------------------------------
 * BTSnoop Capture API layer
 *
 *     The BTSnoop Capture API layer provides functions to capture raw frames
 *     to disk. The frames can be later used with the FtsSniffer component
 *     to process the captured file.
 */

/****************************************************************************
 *
 * Configuration Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * MAX_FNAME_SIZE constant
 *     Specifies the maximum length of the filename (and path) used to 
 *     capture raw frames.
 */
#define MAX_FNAME_SIZE  260

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BTSNOOP_CaptureInit()
 *
 *      Starts embedded capture of raw frames to disk. Use the FtsSniffer
 *      to process the captured file. If called while capture is already
 *      active, no action is taken.
 *
 * Parameters:
 *      FileName   - Pointer to a character buffer which will receive the
 *                   path and name of the file created to capture frames.
 *                   This may be null if the file name is not required.
 *                   If FileName is not an empty string, then the string
 *                   passed into FileName will be used to create the
 *                   capture file.
 *
 *      Len        - Length of the FileName character buffer in bytes.
 *
 *      ScoEnabled - Bool to enable the capture of SCO Packets.
 *
 * Returns:
 *      None.
 */
void BTSNOOP_CaptureInit(char *FileName, U16 Len, BOOL ScoEnabled);

/*---------------------------------------------------------------------------
 * BTSNOOP_CaptureDeinit()
 *
 *      Stops embedded capture of raw frames to disk. Use the FtsSniffer
 *      to process the captured file. If called while capture is not
 *      active, no action is taken.
 *
 * Parameters:
 *      None.
 *
 * Returns:
 *      None.
 */
void BTSNOOP_CaptureDeinit(void);
