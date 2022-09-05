/***************************************************************************
 *
 * File:
 *     radiomgr.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file contains definitions for the radio manager.
 *
 * Created:
 *     March 26, 2003
 *
 * Copyright 2003-2017 OpenSynergy GmbH.
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

#ifndef __RADIOMGR_H
#define __RADIOMGR_H

#include "bttypes.h"

/*---------------------------------------------------------------------------
 * Radio Manager API layer
 *
 *     The Radio Manager controls the initialization, deinitialization, and 
 *     shutdown of the HCI, HCI transport, and radio module.  
 *
 *     Initialization and shutdown of the transport is handled by the transport
 *     driver (see transprt.h) and is implementation specific.  It may include
 *     initialization of memory and queues, installing and uninstalling 
 *     interrupt routines, port probing, downloading of firmware, or any other
 *     necessary procedures.  Initialization and shutdown of transports are 
 *     asynchronous procedures.
 *
 *     Initializing and shutting down the radio module is handled by special 
 *     radio handling code and is implementation specific.  This initialization 
 *     is typically handled by vendor specific HCI commands, but any HCI command 
 *     may be used.  Initializing and shutting down the radio module are 
 *     asynchronous procedures.
 *
 *     Initializing and deinitializing the HCI is handled internally by the
 *     HCI layer.
 *
 *     Radio Manager controls how these three components are called.  During
 *     initialization, the transports are initialized, then the HCI is 
 *     initialized, and finally the radio module is initialized.  If there are
 *     any failures during this initialization, Radio Manager will retry 
 *     initialization BT_HCI_NUM_INIT_RETRIES times.  If initialization does
 *     not complete after all retries have been made, then Radio Manager will
 *     give up.  During shutdown, the radio module is shut down, then the
 *     transport is shut down, and finally the HCI is deinitialized.  If an
 *     error occurs during shutdown, shutdown of the radio module and transport
 *     is aborted and HCI is deinitialized.
 *
 *     Most Radio Manager functions operate asynchronously, but there are no
 *     callbacks made to the services calling these functions.  Instead, all
 *     notification of the completion of these procedures is made through
 *     the Management Entity (ME).  The ME will generate global events to 
 *     indicate the status of HCI.  See me.h for a description of the following
 *     events:
 *
 *         BTEVENT_HCI_INIT_ERROR
 *         BTEVENT_HCI_FATAL_ERROR
 *         BTEVENT_HCI_INITIALIZED
 *         BTEVENT_HCI_DEINITIALIZED
 *         BTEVENT_HCI_FAILED
 */
 
/****************************************************************************
 *
 * Section: Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * RMGR_RadioInit()
 *
 *     Called to initialize the radio module, including the HCI 
 *     transport drivers, the HCI, and the radio itself if special 
 *     initialization is required.  This function is called by the 
 *     event manager in EVM_Init() when the stack initialization sequence 
 *     begins, but can be called by services and applications afterwards if
 *     necessary (RMGR_RadioShutdown() has been called).
 *
 * Parameters:  void
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Initialization was successful.  ME will
 *         be notified during the call that initialization is complete.
 *     BT_STATUS_PENDING - Initialization was started successfully.  ME will
 *         be notified when initialization is complete.
 *     BT_STATUS_IN_PROGRESS - Initialization is already in progress.
 *     BT_STATUS_FAILED - Initialization failed to start.
 */
BtStatus RMGR_RadioInit(void);

/*---------------------------------------------------------------------------
 * RMGR_RadioDeinit()
 *
 *     Called to deinitialize the radio module.  No attempt is make to 
 *     gracefully close the transport or send HCI commands to the radio module.
 *     EVM_Deinit() calls this function during stack deinitialization.
 *     This function should only be called when unloading the stack from
 *     memory.
 *
 * Parameters:  void
 *
 * Returns:   void
 */
void RMGR_RadioDeinit(void);

/*---------------------------------------------------------------------------
 * RMGR_RadioShutdown()
 *
 *     Called to shut down the radio module, including the HCI 
 *     transport drivers, the HCI, and the radio itself if special 
 *     clean up is required through sending HCI commands.
 *     Applications can also call ME_RadioShutdown instead of 
 *     RMGR_RadioShutdown() to gracefully shut the radio down. The
 *     radio may be reinitialized by calling RMGR_RadioInit().
 *
 * Parameters:  void
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The radio is down.  ME will be notified during
 *         the call that the shutdown was complete.
 *     BT_STATUS_PENDING - The radio will be shut down.  ME will be notified
 *        when shutdown is complete.
 *     BT_STATUS_IN_PROGRESS - A shutdown is already in progress.
 *     BT_STATUS_BUSY - Reset could not be initiated, because the radio 
 *         manager is resetting.
 *     BT_STATUS_FAILED - Shutdown failed.
 */
BtStatus RMGR_RadioShutdown(void);

/*---------------------------------------------------------------------------
 * RMGR_HciReset()
 *
 *     Called by the HCI when an HCI transport error occurs.  The
 *     HCI transport drivers and the HCI are deinitialized and then the
 *     HCI is reset to reestablish synchronization with the radio module.
 *     Transports should call HCI_TransportError() instead of calling this
 *     function directly from an interrupt routine or transport driver
 *     thread.
 *
 * Parameters:  void
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Reset was successful.  ME will be notified during 
 *         the call that initialization is complete.
 *     BT_STATUS_PENDING - Reset had begun.  ME will be notified when reset
 *         is complete.
 *     BT_STATUS_IN_PROGRESS - A reset is already in progress.
 *     BT_STATUS_BUSY - Reset could not be initiated, because the radio 
 *         manager is shutting down.
 *     BT_STATUS_FAILED - Initialization failed.
 */
BtStatus RMGR_HciReset(void);

#endif /* __RADIOMGR_H */


