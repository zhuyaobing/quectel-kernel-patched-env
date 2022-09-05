/****************************************************************************
 *
 * File:
 *     iampcontrol.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description: 
 *     This file contains the AMP Controller Interfaces defined by
 *     by the AMP HCI. There are two interfaces. The iAmpControl
 *     interface is intended to be used the Agent and AmpME. The
 *     iAmpControlPhy is intended to be used by the AmpPhyLink
 *     class.
 *
 * Copyright 1999-2017 OpenSynergy GmbH.  
 * All rights reserved.  All unpublished rights reserved.  
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

#ifndef __IAMP_CONTROL_H
#define __IAMP_CONTROL_H

#include "bttypes2.h"
#include "phylink.h"
#include "me.h"  /* Needed for BtRemoteDevice which will be moved someday */
#include "sys/hci.h"
#include "extflowspec.h"
#include "loglink.h"
#include "ampphylink.h"

/*---------------------------------------------------------------------------
 * AMP Controller Interface component
 *
 *     The AMP Controller Interface is used to expose AMP controller
 *     functions to the Blue SDK protocol stack. See AMP Controller Class
 *     documentation in ampcontrol.h for more information.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AmpControllerEventType type
 *
 *     The AMP Controller event types are defined below. They
 *     are offsets from BTE_TYPE_BASE_CNTRL define in bttypes2.h.
 */
typedef BtEventTypeEx AmpControllerEventType;

/** Initialization state has changed. The "status" field indicates the
 * new initialization state of the controller. The "p.ampc" field
 * points to the controller.
 * 
 * BT_STATUS_SUCCESS means the controller is active and ready for use. 
 * Unsolicited AMPCRLE_INIT_STATUS events will occur when the init status 
 * changes.
 *
 * BT_STATUS_NOT_READY means the controller has changed to not-active state.
 * Unsolicited AMPCRLE_INIT_STATUS events will occur whenever the init
 * status changes.
 *
 * BT_STATUS_FAILED means the controller is disabled. It will not generate
 * any more events until init is called again.
 *
 * When BT_STATUS_NOT_READY and BT_STATUS_FAILED are sent the AMP Controller
 * shall first notify the completion of any outstanding operations (using
 * other events as appropriate), then deliver AMPCRLE_PHY_DISCONNECT events
 * for any open physical links, and finally shall send this event with the
 * appropriate status.
 */
#define AMPCRLE_INIT_STATUS       (BTE_TYPE_BASE_CNTRL + 0) 

/** The AMP status of a local AMP controller has changed. The "p.ampc" field
 * contains a pointer to the AMP controller with its current status. 
 */
#define AMPCRLE_CONTROLLER_STATUS (BTE_TYPE_BASE_CNTRL + 1)

/** Physical link create or accept is complete. The "status" field indicates if 
 * the physical link accept was successful or not. If successful,
 * the "phyLink" field points to physical link object. If not successful,
 * the "phyLink" field should not be used and "p.phy" is filled instead.
 */
#define AMPCRLE_PHY_CREATE        (BTE_TYPE_BASE_CNTRL + 2) 

/** Physical link is disconnected. "phyLink" points to physical
 * link object. The BtPhysicalLink::close() function is to be called 
 * before this event is issued.
 */
#define AMPCRLE_PHY_DISCONNECT    (BTE_TYPE_BASE_CNTRL + 3)

/** Physical link channel has been selected. "p.ampAssoc" contains 
 * a pointer to the current AMP_ASSOC.
 */
#define AMPCRLE_CHAN_SELECTED     (BTE_TYPE_BASE_CNTRL + 4)

/** Request to get the AMP ASSOC is complete. "p.ampAssoc" contains a pointer 
 * to the current AMP_ASSOC.
 */
#define AMPCRLE_AMP_ASSOC         (BTE_TYPE_BASE_CNTRL + 5)

/** The logical link create or accept is complete The "status" field indicates if 
 * the logical link creation/accept was successful or not. The "p.logLink" field 
 * points to physical link object. This event is only sent to the physical link
 * via the controller callback function in the physical link
 * (AmpPhyLink.crlCallback).
 */
#define AMPCRLE_LOG_CREATE        (BTE_TYPE_BASE_CNTRL + 6) 

/** Logical link disconnect is complete. "p.logLink" points to logical link object. 
 * This event is only sent to the physical link via the controller callback 
 * function in the physical link (AmpPhyLink.crlCallback).
 */
#define AMPCRLE_LOG_DISCONNECT    (BTE_TYPE_BASE_CNTRL + 7)

/** Logical link modify flow spec is complete. "p.logLink" points to logical  
 * link object. This event is only sent to the physical link via the controller  
 * callback function in the physical link (AmpPhyLink.crlCallback).
 */
#define AMPCRLE_MOD_FLOW_SPEC     (BTE_TYPE_BASE_CNTRL + 8)

/** A previously requested command, "p.hci.cmd", has been acknowledged by
 * the controller object.
 *
 * If the controller completed the command (either with
 * HCE_COMMAND_COMPLETE or HCE_COMMAND_STATUS) "e.status" contains
 * BT_STATUS_SUCCESS. The controller's status code is in "errCode".
 * If a command completed with HCE_COMMAND_COMPLETE and no error,
 * "p.hci.evt" contains the command-specific "Return Parameters"
 * supplied by the controller, starting with the 1-byte status code.
 * Otherwise, p.hci.evt contains no parameters.
 *
 * If the controller transitioned to a deinitialized state while
 * the command was outstanding, "e.status" contains a failure error
 * code.
 */
#define AMPCRLE_COMMAND_CNF    (BTE_TYPE_BASE_CNTRL + 9)

/** An event arrived that was not handled internally by the controller.
 * It is up to each controller to determine which events will be consumed 
 * internally and which will be delivered to higher layers for inspection.
 *
 * "p.hci.ampc" indicates the controller delivering the event. 
 * "p.hci.evt" contains the event and its parameters, which may be 
 * parsed using HCE_Read...() functions. "p.hci.cmd" is not valid.
 */
#define AMPCRLE_EVENT_IND         (BTE_TYPE_BASE_CNTRL + 10)

/* End of AmpControllerEventType type */

/* Forward reference for AmpController type */
typedef struct _AmpController AmpController;


/*---------------------------------------------------------------------------
 * AmpControllerEvent structure
 *
 *     Describes an event generated by the AMP Controller. These events
 *     are passed to the registered callback function supplied by
 *     AmpController::RegisterCallback() or the "crlCallback" member of the
 *     AmpPhyLink.
 */
typedef struct _AmpControllerEvent {
    /* Contains the event type, based on AmpControllerEventType. */
    BtEventEx       e;
    HcStatus        errCode;
    AmpPhyLink     *phyLink;

    /* Parameters */
    union {
        BtLogicalLink  *logLink;

        /* Information about the AMP Assoc data for during the
         * AMPCRLE_AMP_ASSOC event.
         */
        struct {
            U16  len;
            U8  *ptr;
        } ampAssoc;

        /* Pointer to AMP Controller for AMPCRLE_INIT_STATUS and 
         * AMPCRLE_CONTROLLER_STATUS events 
         */
        AmpController *ampc;

        /* Info for AMPCRLE_PHY_CREATE that is used when it fails and for 
         * AMPCRLE_PHY_DISCONNECT when sent to the AMPMGR from MEAMP.
         */
        struct {
            AmpController  *ampc;
            BtRemoteDevice *remDev;
            U8              remCrlId;
        } phy;

        struct {
            AmpController *ampc;
            HciCommand    *cmd;
            HciEvent      *evt;
        } hci;
    } p;
} AmpControllerEvent;

/****************************************************************************
 *
 * Section: AmpController Main Interface Functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AmpController::Deinit()
 *
 *     Deinit the Controller.
 *
 * Parameters:
 *    crl - Pointer to object containing Controller interface (e.g. Controller)
 *
 * Returns:      
 *     void 
 */
typedef void (*IAmpCrlDeinitFunc)(AmpController *crl);

/*---------------------------------------------------------------------------
 * AmpController::Init()
 *
 *     Initialize the Controller including the transport 
 *
 * Parameters:
 *    crl - Pointer to object containing Controller interface
 *
 * Returns:
 *    BT_STATUS_SUCCESS - Initialization completed successfully controller is
 *        active and ready to use. AMPCRLE_INIT_STATUS events will be returned
 *        via the registered callback when the init status of the controller
 *        changes.
 *
 *    BT_STATUS_NOT_READY - The controller is not yet active. AMPCRLE_INIT_STATUS 
 *        events will be returned through the registered callback if the
 *        init status of the controller changes. 
 *
 *    BT_STATUS_FAILED - Initialization failed. The Controller will not send
 *        any events.
 */
typedef BtStatus (*IAmpCrlInitFunc)(AmpController *crl);

/*---------------------------------------------------------------------------
 * AmpController::RegisterCallback()
 *
 *     Register a callback with the HCI for command and data events. This
 *     function can be called before init().
 *
 * Parameters:
 *    crl - Pointer to object containing Amp Controller interface (e.g. Controller)
 *
 *    handler - Pointer to handler containing callback function and context
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - Request was successful. 
 *
 *     BT_STATUS_FAILED - Request failed.
 *
 */
typedef BtStatus (*IAmpCrlRegisterFunc)(AmpController *crl, BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * AmpController::AllocatePhyLink()
 *
 *     Allocate a physical link object. The physical link object is freed by
 *     calling its free() function.
 *
 *     Note that the BtPhysicalLink object returned will persist beyond
 *     transport errors or calls to init/deinit(). In other words,
 *     BtPhysicalLink objects are never freed automatically.
 *
 * Parameters:
 *    crl - Pointer to object containing Amp Controller interface (e.g. Controller)
 *
 *    remDev - Pointer to the remote device object to which a phys link is to 
 *             be created
 *
 *    remCrl - Id of the remote controller to which physical link is to be created
 *
 * Returns:      
 *     Pointer to a BtPhysicalLink object or 0 if no objects are available or
 *     the described BtPhysicalLink object already exists.
 *
 */
typedef BtPhysicalLink *(*IAmpCrlAllocatePhyLink)(AmpController *crl,
            BtRemoteDevice *remDev, U8 remoteControllerId);

/*---------------------------------------------------------------------------
 * AmpController::CreatePhyLink()
 *
 *     Create an Amp Physical link between the local device AMP Controller 
 *     and a remote device AMP controller as specified via the
 *     BtPhysicalLink object.
 *
 * Parameters:
 *    crl - Pointer to object containing Amp Controller interface (e.g. Controller)
 *
 *    phyLink - Pointer to the physical link object which identifies the
 *              remote device and the remote device's AMP controller. 
 *
 *    dedKey - Pointer to the AMP Dedicated Key to be used for security.
 *
 *    ampAssocLen - Length of AMP_ASSOC
 *
 *    ampAssoc - Pointer to the AMP_ASSOC structure obtained from the remote
 *        device. This function makes an internal copy of the AMP ASSOC data.
 *
 * Returns:      
 *     BT_STATUS_PENDING - Request was successfully started. Completion of the
 *         request is indicated by a call to registered callback with
 *         the AMPCRLE_PHY_CREATE event type. During the creation operation the
 *         AMPCRLE_CHAN_SELECTED event may be set via the registered callback
 *         with an AMP_ASSOC containing the selected channel. This event will occur
 *         before the AMPCRLE_PHY_CREATE event.
 *
 *     BT_STATUS_IN_PROGRESS - physical link is in the process of being created
 *
 *     BT_STATUS_IN_USE - the physical link already exists.
 *
 *     BT_STATUS_FAILED - Request failed because the physical link is not associated
 *         with the AMP controller (i.e. not allocated from the controller (crl))
 *
 *     BT_STATUS_HCI_INIT_ERR - The lower level transport is not in an initialized
 *         state and therefore the operation cannot proceed or the controller is 
 *         disabled.
 */

typedef BtStatus (*IAmpCrlCreatePhyLink)(AmpController *crl, BtPhysicalLink *phyLink, BtAmpDedKey *dedKey,
                                         U16 ampAssocLen, U8 *ampAssoc);


/*---------------------------------------------------------------------------
 * AmpController::AcceptPhyLink()
 *
 *     Accept the creation of an Amp Physical link between the local device 
 *     AMP Controller and a remote device AMP controller as specified via the 
 *     BtPhysicalLink object.
 *
 * Parameters:
 *    crl - Pointer to object containing Amp Controller interface
 *          (e.g. Controller)
 *
 *    phyLink - Pointer to the physical link object which identifies the
 *              remote device and the remove device's AMP controller. 
 *
 *    dedKey - Pointer to the AMP Dedicated Key to be used for security.
 *
 *    ampAssocLen - Length of AMP_ASSOC
 *
 *    ampAssoc - Pointer to the AMP_ASSOC structure obtained from the remote
 *        device. This function makes an internal copy of the AMP ASSOC data.
 *
 * Returns:      
 *     See AmpController::CreatePhyLink.
 */

typedef BtStatus (*IAmpCrlAcceptPhyLink)(AmpController *crl, BtPhysicalLink *phyLink, BtAmpDedKey *dedKey,
                                         U16 ampAssocLen, U8 *ampAssoc);

/*---------------------------------------------------------------------------
 * AmpController::DisconnectPhyLink()
 *
 *     Disconnect the Amp Physical link, or cancel an outstanding 
 *     AmpController  CreatePhyLink or AmpController  AcceptPhyLink request.
 *
 * Parameters:
 *    crl - Pointer to object containing Amp Controller interface (e.g. Controller)
 *
 *    phyLink - Pointer to the physical link object which identifies the
 *              remote device and the remove device's AMP controller. 
 *
 *    reason - Reason for the disconnection. Valid reasons are 0x05,
 *             0x13 - 0x16, 0x1A. 
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - An outstanding Create/Accept request was cancelled
 *         successfully (causing an appropriate AMPCRLE_PHY_CREATE or
 *         AMPCRLE_PHY_ACCEPT event with failure status code). No further events
 *         will be generated.
 * 
 *     BT_STATUS_PENDING - Request was successfully started or in progress. 
 *         Completion of the request is indicated by a call to registered callback 
 *         with the AMPCRLE_PHY_DISCONNECT, AMPCRLE_PHY_CREATE, or 
 *         AMPCRLE_PHY_ACCEPT event type, depending on the state of the physical link 
 *         when this function was requested.
 *
 *     BT_STATUS_NO_CONNECTION - Request failed because there is no connection
 *
 *     BT_STATUS_FAILED - Request failed because the physical link is not associated
 *         with the AMP controller (i.e. not allocated from the controller (crl)); 
 *
 *     BT_STATUS_HCI_INIT_ERR - The lower level transport is not in an initialized
 *         state and therefore the operation cannot proceed or the controller is 
 *         disabled.
 */

typedef BtStatus (*IAmpCrlDiscPhyLink)(AmpController *crl, BtPhysicalLink *phyLink, HcStatus reason); 


/*---------------------------------------------------------------------------
 * AmpController::GetAmpAssoc()
 *
 *     Retrieve the most up date AMP Assoc from the controller. The AMP assoc
 *     will be returned via the AMPCRLE_AMP_ASSOC event to the handler.
 *
 * Parameters:
 *    crl - Pointer to object containing Amp Controller interface (e.g. Controller)
 *
 *    handler - pointer to handler to receive the AMP assoc
 *
 * Returns:      
 *     BT_STATUS_PENDING - Request was successfully started. Completion of the
 *         request is indicated by a call to the handler with the
 *         AMPCRLE_PHY_DISCONNECT event type.  
 *
 *     BT_STATUS_FAILED - Request failed. 
 *
 *     BT_STATUS_HCI_INIT_ERR - The lower level transport is not in an initialized
 *         state and therefore the operation cannot proceed or the controller is 
 *         disabled.
 */

typedef BtStatus (*IAmpCrlGetAmpAssoc)(AmpController *crl, BtHandlerEx *handler);

/*---------------------------------------------------------------------------
 * AmpController::GetPhyLink()
 *
 *     Return a pointer to the phylink object associated with the remDev
 *     and remote controller ID. Return 0 if a phylink does not exist. 
 *
 * Parameters:
 *    crl - Pointer to object containing Amp Controller interface (e.g. Controller)
 *
 *    remDev - Pointer to the remote device object to which a phys link is connected
 *
 *    remCrl - Id of the remote controller to which phy link is connected
 *
 * Returns:      
 *     Pointer to a BtPhysicalLink object or 0 if a physical link does not exist. 
 *
 */
typedef BtPhysicalLink *(*IAmpCrlGetPhyLink)(AmpController *crl, BtRemoteDevice *remDev,
                                             U8 remoteControllerId);

/*---------------------------------------------------------------------------
 * AmpController::Enable()
 *
 *     Set the enable/disable state of the controller.
 *
 *     If "enable" is set to TRUE, the request is to enable a disabled
 *     controller. An enabled controller is usable by the stack.
 *
 *     If the "enable" flag is set FALSE the request is to disable an enabled
 *     controller. A disabled controller is not useable by the stack. In the
 *     disabled state the controller shall return errors for all requests
 *     and reject most requests from remote controllers.
 *
 * Parameters:
 *    crl - Pointer to object containing Amp Controller interface (e.g. Controller)
 *
 *    enable - TRUE to enable the controller, FALSE to disable the controller.
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - operation was successful and the state was changed
 *
 *     BT_STATUS_PENDING - the controller started the process of becoming enabled.
 *         Completion of the operation is indicated by a call to the registered
 *         handler with the AMPCRLE_INIT_STATUS event type and status indicating
 *         the result.
 *
 *     BT_STATUS_RESTRICTED - controller did not change state because it is already
 *         in the state requested by "enabled".
 *
 *     BT_STATUS_FAILED - operation failed. In the case of being disabled the 
 *         operation failed because the controller was active. A controller is 
 *         active if it has at least one physical link allocated or is in the middle 
 *         of performing an operation (e.g. the controller has issued a command 
 *         to the radio and is waiting for a response that will result in an 
 *         event to the caller. 
 *
 */
typedef BtStatus (*IAmpCrlEnable)(AmpController *crl, BOOL enable);

/*---------------------------------------------------------------------------
 * AmpController::SendCommand()
 *
 *     Sends a custom HCI command to the AMP controller.
 *
 * Parameters:
 *    crl - Pointer to object containing AMP Controller interface (e.g.
 *        Controller).
 *
 *    cmd - Command to send. The command should be initialized with
 *        HCC_InitCommand() and may be filled with HCC_Write...() functions.
 *
 * Returns:      
 *     BT_STATUS_PENDING - The command will be delivered to the controller
 *         and must not be modified. The AMPCRLE_COMMAND_CNF event will
 *         arrive when the controller is finished with the command.
 *
 *     error code - The command could not be sent to the controller.
 */
typedef BtStatus (*IAmpCrlSendCommand)(AmpController *crl, HciCommand *cmd);

/*---------------------------------------------------------------------------
 * IAmpControl structure
 *
 *     Defines the interface which must be implemented by any AmpController
 *     object. When creating the AmpController, each field below is filled
 *     in with an appropriate function pointer.
 */
typedef struct _IAmpControl {
    IAmpCrlInitFunc        init;
    IAmpCrlDeinitFunc      deinit;
    IAmpCrlRegisterFunc    registerHandler;
    IAmpCrlAllocatePhyLink allocatePhyLink;
    IAmpCrlCreatePhyLink   createPhyLink;
    IAmpCrlAcceptPhyLink   acceptPhyLink;
    IAmpCrlDiscPhyLink     discPhyLink;
    IAmpCrlGetAmpAssoc     getAmpAssoc;
    IAmpCrlGetPhyLink      getPhyLink;
    IAmpCrlEnable          enable;
    IAmpCrlSendCommand     sendCommand;
} IAmpControl;


/****************************************************************************
 *
 * Section: AmpController Phy Interface Functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AmpController::FreePhyLink()
 *
 *     Free the physical link object. 
 *
 * Parameters:
 *    crl - Pointer to object containing Amp Controller interface (e.g. Controller)
 *
 *    phyLink - Pointer to the physical link object to be freed. 
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - phylink was freed. 
 *
 *     BT_STATUS_IN_USE  - Request failed because the phylink is still active
 *
 *     BT_STATUS_FAILED - Request failed because the phylink is not associated
 *         with this controller or has not been allocated.
 */

typedef BtStatus (*IAmpCrlFreePhyLink)(AmpController *crl, AmpPhyLink *phyLink);

/*---------------------------------------------------------------------------
 * AmpController::CreateLogLink()
 *
 *     Create a Logical link over an existing Physical link. The logical
 *     link object to use should be allocated from the physical link and have
 *     a valid efsPair field. 
 *
 * Parameters:
 *    crl - Pointer to object containing Amp Controller interface (e.g. Controller)
 *
 *    phyLink - Pointer to the physical link object which identifies the
 *        remote device and the remove device's AMP controller. 
 *
 *    llink - Pointer to the logical link object to create. This logical link
 *        object must contain a valid ExtFlowSpecPair object defining the 
 *        desired QoS. 
 *
 * Returns:      
 *     BT_STATUS_PENDING - Request was successfully started. Completion of the
 *         request is indicated by a call to registered callback with
 *         the AMPCRLE_LOG_CREATE event.  
 *
 *     BT_STATUS_NO_CONNECTION - Physical link is not in the BDS_CONNECTED state
 *
 *     BT_STATUS_FAILED - Request failed. 
 */

typedef BtStatus (*IAmpCrlCreateLogLink)(AmpController *crl, AmpPhyLink *phyLink, 
                                         BtLogicalLink *llink);


/*---------------------------------------------------------------------------
 * AmpController::AcceptLogLink()
 *
 *     Accept Logical link creation over an existing Physical link. The logical
 *     link object to use should be allocated from the physical link and have
 *     a valid efsPair field. 
 *
 * Parameters:
 *    crl - Pointer to object containing Amp Controller interface (e.g. Controller)
 *
 *    phyLink - Pointer to the physical link object which identifies the
 *        remote device and the remove device's AMP controller. 
 *
 *    llink - Pointer to the logical link object to create. This logical link
 *        object must contain a valid ExtFlowSpecPair object defining the
 *        desired QoS. 
 *
 * Returns:      
 *     BT_STATUS_PENDING - Request was successfully started. Completion of the
 *         request is indicated by a call to registered callback with
 *         the AMPCRLE_LOG_CREATE event.  
 *
 *     BT_STATUS_FAILED - Request failed. 
 */

typedef BtStatus (*IAmpCrlAcceptLogLink)(AmpController *crl, AmpPhyLink *phyLink, 
                                         BtLogicalLink *llink);


/*---------------------------------------------------------------------------
 * AmpController::DiscLogLink()
 *
 *     Disconnect a connected logical link, or cancel an existing connection
 *     attempt.
 *
 * Parameters:
 *    crl - Pointer to object containing Amp Controller interface (e.g. Controller)
 *
 *    llink - Pointer to the logical link object to disconnect.  
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - A request to connect was cancelled successfully.
 * 
 *     BT_STATUS_PENDING - Request was successfully started or already in
 *         progress. Completion of the request is indicated by a call to 
 *         registered callback with the AMPCRLE_LOG_DISCONNECT event.  
 *
 *     BT_STATUS_NO_CONNECTION - Logical link is not connected.
 *
 *     BT_STATUS_FAILED - Request failed. 
 */

typedef BtStatus (*IAmpCrlDiscLogLink)(AmpController *crl, BtLogicalLink *llink); 


/*---------------------------------------------------------------------------
 * AmpController::ModFlowSpec()
 *
 *     Modify the flow spec of the logical link.
 *
 * Parameters:
 *    crl - Pointer to object containing Amp Controller interface (e.g. Controller)
 *
 *    llink - Pointer to the logical link object to disconnect.  
 *
 * Returns:      
 *     BT_STATUS_PENDING - Request was successfully started. Completion of the
 *         request is indicated by a call to registered callback with
 *         the AMPCRLE_MOD_FLOW_SPEC event.  
 *
 *     BT_STATUS_FAILED - Request failed. 
 */
typedef BtStatus (*IAmpCrlModFlowSpec)(AmpController *crl, BtLogicalLink *llink); 

/*---------------------------------------------------------------------------
 * AmpController::ReqToSend()
 *
 *     Tell the controller that the given physical link has data to send. 
 *     The controller should use the AmpPhyLink::TxReady routine to
 *     retrieve BtPackets for transmission.
 *
 * Parameters:
 *    crl - Pointer to object containing Amp Controller interface (e.g. Controller)
 *
 *    plink - Pointer to the physical link object with data to send.  
 *
 * Returns:      
 *     void 
 */
typedef void (*IAmpCrlReqToSend)(AmpController *crl, AmpPhyLink *plink, BtPriorityToken *priToken); 

/*---------------------------------------------------------------------------
 * IAmpControlPhy structure
 *
 *     Defines the phy-link-related interface which must be implemented by any
 *     AmpController object. When creating the AmpController, each field below
 *     is filled in with an appropriate function pointer.
 *
 */
typedef struct _IAmpControlPhy {
    IAmpCrlFreePhyLink     freePhyLink;
    IAmpCrlCreateLogLink   createLogLink;
    IAmpCrlAcceptLogLink   acceptLogLink;
    IAmpCrlDiscLogLink     discLogLink;
    IAmpCrlModFlowSpec     modFlowSpec;
    IAmpCrlReqToSend       reqToSend;
} IAmpControlPhy;

/* Type init */
void AmpController_Init(void);

#endif /* __IAMP_CONTROL_H */
