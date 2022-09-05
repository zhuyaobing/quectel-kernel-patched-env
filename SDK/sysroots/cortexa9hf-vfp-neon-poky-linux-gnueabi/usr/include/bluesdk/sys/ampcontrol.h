/****************************************************************************
 *
 * File:
 *     ampcontrol.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file contains the definition of a Bluetooth AMP
 *     Controller class.
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

#ifndef __AMP_CONTROLLER_H
#define __AMP_CONTROLLER_H

#include "iampcontrol.h"
#include "ampphylink.h"
#include "bttypes2.h"
#include "eventmgr.h"

/*---------------------------------------------------------------------------
 * AMP Controller Class component
 *
 *     The AMP Controller Class defines the full interface required by the
 *     Blue SDK to monitor and control an AMP Controller. Instances of this
 *     class use the AmpController structure defined here to manage state
 *     information and provide a set of function endpoints. The Blue SDK
 *     makes calls to the AMP Controller Interface functions as necessary to
 *     control physical links with remote devices and logical data links
 *     established over those physical links.
 *
 *     To expose a new AMP Controller to the Blue SDK system, a subclass
 *     of the AmpController class must be created. All function pointers
 *     defined in the IAmpControl and IAmpControlPhy interfaces must be
 *     supplied. After an instance of the AmpController subclass is
 *     initialized it is given to the stack by calling
 *     MEAMP_AddController().
 */


/*---------------------------------------------------------------------------
 * AmpController::Process()
 *
 *     A function of this type is used to provide operating context to an
 *     AmpController instance.
 *
 * Parameters:
 *     crl - Controller to process.
 *
 */
typedef void (*AmpControlProcessFunc)(EvmTask *Task);

/*---------------------------------------------------------------------------
 * AmpController structure
 *
 *     Defines the class that represents an AMP Controller. All AMP
 *     Controller instances must use this structure to expose their
 *     functionality and state to the rest of the stack.
 */
struct _AmpController {
    ListEntry         node;      /* Used internally by MEAMP. */

    /* Controller interface for generic functions */
    IAmpControl       icrl;

    /* Controller interface for physical link management */
    IAmpControlPhy    icp;

    /* Assigned and used internally by MEAMP to identify the controller */
    U8                id;

    /* Assigned by the AmpController to identify its controller type */
    BtControllerType  type;

    /* Updated by the AmpController to reflect its current status */
    BtAmpStatus       status;

    /* Assigned and used internally by MEAMP to identify disabled controllers */
    U8                disableId;

    /* May be used by AmpController as the main handler for AmpController events. */
    BtHandlerEx      *handler;

    /* May be used by AmpController as handler for AMPCRLE_AMP_ASSOC events */
    BtHandlerEx      *ampAssocHandler;

    /* Must be used by AmpController to store current information about the AMP */
    BtAmpInfo        ampInfo;

    /* AMP Controller process function. This function is called when ever the Event
     * Manager runs and the AMP Controller is active (on MEAMP controller or pending lists).
     */
    AmpControlProcessFunc process;
};



#endif /* __AMP_CONTROLLER_H */
