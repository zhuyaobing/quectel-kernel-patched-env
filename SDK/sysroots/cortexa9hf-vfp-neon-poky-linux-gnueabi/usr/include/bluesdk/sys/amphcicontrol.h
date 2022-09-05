/****************************************************************************
 *
 * File:
 *     amphcicontrol.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Defines an HCI-based controller for an AMP.
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
#ifndef __AMP_HCI_CONTROL_H
#define __AMP_HCI_CONTROL_H

#include "ampcontrol.h"
#include "ampphylink.h"
#include "ihcitransport.h"
#include "userheap.h"
#include "hcibuffpool.h"

/* Forward structure decl */
typedef struct _AmpHciController AmpHciController;

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Maximum AMP_ASSOC size that the controller may report.
 */
#ifndef AHC_MAX_AMP_ASSOC
#define AHC_MAX_AMP_ASSOC 800
#endif /* AHC_MAX_AMP_ASSOC */

/*---------------------------------------------------------------------------
 * Largest number of parameters any parameter list could have.
 * See Read Local AMP Info Command return params, plus Command
 * Complete event params.
 */
#define AHC_MAX_PARMS 13 

/*---------------------------------------------------------------------------
 * Memory available for internal allocation of AhcProcedure structures
 * (which vary in size according to the procedure being queued).
 *
 * If user requests during normal operation ever return with
 * BT_STATUS_NO_RESOURCES, consider increasing this value.
 */
#ifndef AHC_PROCEDURE_MEM
#define AHC_PROCEDURE_MEM 1024 + (AHC_MAX_AMP_ASSOC * 2)
#endif /* AHC_PROCEDURE_MEM */

/*---------------------------------------------------------------------------
 * Maximum HCI data fragment required for parameters
 */
#define AHC_MAX_CMD_LEN        255

/*---------------------------------------------------------------------------
 * Maximum number of Phy Links supported by any controller 
 */
#ifndef AHC_MAX_PHY_LINKS
#define AHC_MAX_PHY_LINKS      1
#endif /* AHC_MAX_PHY_LINKS */

/*---------------------------------------------------------------------------
 * Number of HCI packets available for queuing to the transport. 
 */
#ifndef AHC_MAX_TX_PACKETS
#define AHC_MAX_TX_PACKETS 4
#endif /* AHC_MAX_TX_PACKETS */

/*---------------------------------------------------------------------------
 * Number of TX packets which may be outstanding in the transport at any
 * given time
 */
#define AHC_MAX_TX_OUTSTANDING AHC_MAX_TX_PACKETS + 1

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/* Internal use only */
typedef enum _InitState {
    AHCI_NONE,           /* Start */
    AHCI_DISABLED,       /* Currently not enabled and not pending enable */
    AHCI_STARTED,        /* Started transport init */
    AHCI_INITED,         /* Transport init complete */
    AHCI_RESETTING,      /* Waiting for completion of reset procedure */
    AHCI_INIT,           /* Init complete, transport+controller active */
    AHCI_DEINIT_STARTED, /* Transport deinit pending */
    AHCI_TRANS_DEINITED, /* Transport deinit is complete */
} InitState;

typedef U8 AhcFlags;

/* Indicates the state in which all tx packets are failing because the
 * transport is about to deinit
 */
#define AHCF_PACKETS_FAILING 0x01

/* End of AhcFlags */

/****************************************************************************
 *
 * Data Structures 
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AhcParameter structure
 *
 *     For internal use. Represents a single HCI command or event parameter.
 */
typedef union _AhcParameter {
    /* Actual storage-width of value is determined by parameter descriptor */
    U32 value;

    /* Buffer data */
    struct {
        U8 *data; 
        U16 len;
    } buffer;
} AhcParameter;


/*---------------------------------------------------------------------------
 * AhcParameters structure
 *
 *     For internal use. A collection of HCI command or event parameters.
 */
typedef struct _AhcParameters {
    /* Number of valid parameters */
    U8 len;
    /* Parameters */
    AhcParameter p[AHC_MAX_PARMS];
} AhcParameters;

/*---------------------------------------------------------------------------
 * AhcPacketTracker structure
 *
 *     Used internally to track outstanding tx packets.
 */
typedef struct _AhcPacketTracker {
    /* List node */
    ListEntry node;

    /* Packet of interest */
    HciPacket *pkt;

    /* A list of associated Rx packets */
    ListEntry rxList;
} AhcPacketTracker;

/*---------------------------------------------------------------------------
 * AhcProcedureEvent
 *
 *     Used internally to signal events to be handled by an ongoing
 *     procedure.
 */
typedef BtEventTypeEx AhcProcedureEvent;

#define APE_COMMAND_AVAIL       (BTE_TYPE_BASE_APP + 0)

/* Indicates that results have arrived for an earlier command.
 * This occurs as a result of a Command_Complete event or a
 * Command_Status containing an error code.
 *
 * parms->cmd will contain the original command opcode, and parms->error
 * will contain the error code or BEC_NO_ERROR. Additional parameters may
 * be found in parms->parms depending on what the controller supplied.
 */
#define APE_COMMAND_RESULT      (BTE_TYPE_BASE_APP + 1)

/* A command has been received by the controller and is now processing.
 * "cmd" will contain the opcode of the pending command.
 */
#define APE_COMMAND_PENDING       (BTE_TYPE_BASE_APP + 2)

/* An ordinary event has been sent by the controller and needs handling.
 * parms->evt tells you which event, and parms->parms contains its parameters.
 */
#define APE_EVENT         (BTE_TYPE_BASE_APP + 3)

/* An abort condition related to this procedure has been detected.
 * parms->e.status may supply more information about the reason
 * for abort. The procedure is responsible for issuing any failure callbacks
 * and cleaning itself up with AhcProcEnd.
 */
#define APE_ABORT       (BTE_TYPE_BASE_APP + 4)

/* Indicates an event which has been consumed and should be ignored */
#define APE_INVALID        (BTE_TYPE_BASE_APP + 5)

/* End of AhcProcedureEvent */

/*---------------------------------------------------------------------------
 * AhcProcedureParms structure
 *
 *     Complete description of an event that procedures must inspect. All
 *     procedures get all events, so the procedure must determine if the
 *     event is relevant to it, before taking any action.
 *    
 *     When the procedure has determined that it is done processing, it must
 *     call AhcProcEnd. After returning, the procedure will be freed.
 */
typedef struct _AhcProcedureParms {
    /* A description of the event, including an event type (see AhcProcedureEvent)
     * and a pointer to the handler which is really an AhcProcedure pointer.
     *
     * Status is not used.
     */
    BtEventEx e;

    /* Opcode of the original command (valid during APE_COMMAND_RESULT and
     * APE_COMMAND_PENDING)
     */
    HciCommandType cmd;

    /* Opcode of the event being handled (valid during APE_EVENT) */
    HciEventType evt;

    /* Controller-supplied error code (valid during APE_COMMAND_RESULT and
     * APE_ABORT)
     */
    BtErrorCode error;

    /* Additional cmd/event parameters if any (valid during APE_COMMAND_RESULT,
     * APE_EVENT)
     */
    AhcParameters parms;
} AhcProcedureParms;

#define AHCP_STATE_DEAD 0xFF

/*---------------------------------------------------------------------------
 * AhcProcedure structure
 *
 *     An internal control structure for tracking a procedure to completion.
 */
typedef struct _AhcProcedure {
    /* Handler for this procedure. Must be first.
     * "handler->callback" contains function that deals with all procedure
     * events. "handler->context" may point to procedure-defined data.
     */
    BtHandlerEx handler;

    /* Current state of the procedure as defined by it. AHCP_STATE_DEAD
     * (0xFF) is reserved to indicate a "dead" procedure.
     */
    U8 state;

    /* A way to reach back to the original AmpHciController */
    AmpHciController *ahc;

} AhcProcedure;

typedef void (*AhcProcedureCallback)(AhcProcedure *proc, AhcProcedureParms *p);

/* Context structures for various types of procedures */

typedef struct _AhcGetAmpAssocContext {
    AhcProcedure proc; /* Procedure header. Must be first */
    BtHandlerEx *userHandler;
    U16 ampReadPos;
    AmpPhyLink *phy; /* NULL, or associated phy */
    BtEventTypeEx eventCode; /* Event code to send at completion */
    U8 *ampAssoc;
    U16 ampAssocLen;
} AhcGetAmpAssocContext;

typedef struct _AhcCreatePhyLinkContext {
    AhcProcedure proc; /* Procedure header. Must be first */

    /* The PhyLink being created */
    AmpPhyLink *phyLink;

    /* Style of this procedure, either HCC_CREATE_PHY_LINK or 
     * HCC_ACCEPT_PHY_LINK.
     */
    HciCommandType style;

    /* Data necessary to track the writing of the supplied AMP ASSOC */
    U16 ampWritePos;
    U8 *ampAssoc;
    U16 ampAssocLen;

    BtAmpDedKey key;

    /* A channel select request has been indicated by the controller. */
    BOOL channelRequested;

    /* In the event we receive Channel_Selected we need to be ready to
     * re-read the local AMP ASSOC, specific to this phy.
     */
    AhcGetAmpAssocContext *readAmpProc;

} AhcCreatePhyLinkContext;

typedef struct _AhcDiscPhyLinkContext {
    AhcProcedure proc; /* Procedure header. Must be first */

    /* The PhyLink being disconnected */
    AmpPhyLink *phyLink;

    /* Error code to send along as the reason for disconnect */
    BtErrorCode reason;

    /* Create/Accept procedure being cancelled, if any */
    AhcCreatePhyLinkContext *cancel;

} AhcDiscPhyLinkContext;

typedef struct _AhcCreateLogLinkContext {
    AhcProcedure proc; /* Procedure header. Must be first */

    /* Physical link under the new logical link */
    AmpPhyLink *phyLink;

    /* Handle for the new logical link */
    BtLogicalLink *logLink;

    /* Type of command (Create or Accept) */
    HciCommandType cmdType;

    /* The Create/AcceptLogLink commands have a large number of parameters. */
    AhcParameter tempParms[6 * 2 + 1];
    
} AhcCreateLogLinkContext;

typedef struct _AhcDiscLogLinkContext {
    /* Control, must be first */
    AhcProcedure proc;  

    /* One of the two members below is valid, depending
     * on whether this struct is being used as a disc or
     * a cancel.
     */
    union {
        /* Logical link we are disconnecting */
        BtLogicalLink *logLink; 

        /* The original create proc being cancelled */
        AhcCreateLogLinkContext *create;
    } u;
} AhcDiscLogLinkContext;

/*---------------------------------------------------------------------------
 * AmpHciController structure
 *
 *     Represents an AMP controller that is made accessible via an HCI 
 *     interface.
 */
struct _AmpHciController {
    AmpController   crl;          /* Superclass */
    IHciTransport  *trans;        /* Pointer to underlying HCI transport */
    BtHandlerEx     transHandler; /* Handler for transport-generated events */
    InitState       initState;    /* Internal init/deinit state */
    AhcFlags        flags;        /* Flag fields */

    /* Number of commands the controller currently allows us to send */
    U8              cmdsAllowed; 

    /* Controller information */
    struct {
        U16  maxACLDataPacketLen;
        BOOL blockMode;
        U16  curPacketsAllowed;
        U16  dataBlockLen;
        U16  totalNumDataBlocks;
        U16  curBlocksAllowed;
    } flow;

    /* Rx packet trackers */
    ListEntry       freeTrackers;
    ListEntry       inUseTrackers;
    ListEntry       rxToProcess;

    /* Procedures */
    U8              procedureMem[AHC_PROCEDURE_MEM];
    ListEntry       inUseProcs;

    /* HciPacket for command transmission */
    HciPacket       cmdPacket;
    BOOL            cmdPacketInUse;

    /* HciPackets for data delivery */
    ListEntry       freeDataPackets;

    /* BtPackets being transmitted */
    ListEntry       inTransit;

    /* AmpPhyLink objects */
    ListEntry       freePhyLinks;
    ListEntry       inUsePhyLinks;

    /* Reserve data for procedure contexts used to close phy links */
    ListEntry       phyClosers;

    /* List of BtLogicalLink->cNodes associated with this controller. */
    ListEntry       logLinks; 

    /* Reserve data for procedure contexts used to close log links */
    ListEntry       logClosers;

    /* Data from transport to be processed during stack context goes here.
     * Notice that this data is freely touched by HciTransport's thread,
     * so access to these members must be protected with Pause/Resume().
     */
    struct {
        ListEntry       sentPackets; 
        ListEntry       receivedBuffers;
        HciTransError   error; 
        BOOL            rxBufferRequested;
    } tran;

    /* Memory touched only once at init time */
    struct {
        AmpPhyLink       phyLinkArray[AHC_MAX_PHY_LINKS];
        U8               cmdPacketData[AHC_MAX_CMD_LEN];
        AhcPacketTracker trackerArray[AHC_MAX_TX_OUTSTANDING];
        HciPacket        packetArray[AHC_MAX_TX_PACKETS];
    } alloc;

    /* HCI Buffer pools for ACL (Data) and events and Hci Buffer free handler*/
    HciBufferPool        aclHbPool;
    HciBufferPool        eventHbPool;
    BtHandlerEx          hbHandler;

    /* Event Manager Scheduler Tasks */
    EvmTask              evmAhcProcessTask;
    EvmTask              evmTxSignalTask;
    EvmTask              evmTxGuaranteedTask;
    EvmTask              evmTxBestEffortTask;
    EvmTask              evmRxSignalTask;
    EvmTask              evmRxGuaranteedTask;
    EvmTask              evmRxBestEffortTask;
};


/*---------------------------------------------------------------------------
 * AHC_Create()
 *
 *     Initializes an AmpHciController instance.
 *
 * Parameters:
 *     ctx - An uninitialized AmpHciController structure.
 *
 *     hciTransport - The underlying transport to use for data exchange.
 */
void AHC_Create(AmpHciController *ctx, IHciTransport *hciTransport,
                HciBufferAllocation *aclHbAlloc, HciBufferAllocation *eventHbAlloc);


/* Internal prototypes */

/* Parser/Builder (amphcicontrol_parms) */
BOOL AhcParseEventParms(HciEvent *hciEvent, AhcParameters *parms);
BOOL AhcParseCompleteParms(HciEvent *hciEvent, AhcParameters *hciParms);
HciPacket *AhcBuildCommand(AmpHciController *ctx, HciCommandType cmd, AhcParameter *parameters);
BOOL AhcParseParms(HciEvent *hciEvent, AhcParameters *parms, const U8 *desc);
BOOL AhcIsKnownCommandType(HciCommandType cmdType);

/* Procedure handlers/initiators (amphcicontrol_procs) */
void AhcResetProc(AhcProcedure *proc, AhcProcedureParms *p);
BtStatus AhcStartReadLocalAmpAssocProc(AmpHciController *ctx, BtHandlerEx *userHandler);
BtStatus AhcStartCreatePhyLinkProc(AmpHciController *ctx, AmpPhyLink *phyLink,
                                   HciCommandType style, BtAmpDedKey *dedKey, U8 *ampAssoc, U16 ampAssocLen);
BtStatus AhcStartDiscPhyLinkProc(AmpHciController *ctx, AmpPhyLink *phyLink, HcStatus reason);
BtStatus AhcCancelPhyLink(AmpHciController *ctx, AmpPhyLink *phyLinK, HcStatus reason);
BtStatus AhcStartCreateLogLinkProc(AmpHciController *ctx, AmpPhyLink *phyLink,
                                   BtLogicalLink *logLink, BOOL create);
BtStatus AhcCancelLogLink(AmpHciController *ctx, BtLogicalLink *logLink);
BtStatus AhcDisconLogLink(AmpHciController *ctx, BtLogicalLink *logLink);
BtStatus AhcModifyFlowSpec(AmpHciController *ctx, BtLogicalLink *logLink);
AhcDiscPhyLinkContext *AhcAllocDiscPhyLinkProc(AmpHciController *ctx, AmpPhyLink *phyLink, HcStatus reason);
AhcDiscLogLinkContext *AhcAllocDiscLogLinkProc(AmpHciController *ctx, BtLogicalLink *logLink);
AhcCreateLogLinkContext *AhcFindCreateLogLinkProc(AmpHciController *ctx, BtLogicalLink *logLink);
BtStatus AhcStartCustomCommandProc(AmpHciController *ctx, HciCommand *cmd);

/* Core routines (amphcicontrol_core) */
void AhcProcessTask(EvmTask *Task);
void AhcHandleOutstandingSendsTask(EvmTask *Task);
void AhcHandleOutstandingRxTask(EvmTask *Task);
void AhcSchedule(AmpHciController *ctx, EvmTask *Task);
BtStatus AhcVerifyLogLink(AmpHciController *ctx, BtLogicalLink *logLink);
BtStatus AhcVerifyPhyLink(AmpHciController *ctx, BtPhysicalLink *phyLink);

HciPacket *AhcAllocCmdPacket(AmpHciController *ctx, U16 paramLen);
BtStatus AhcSendCommand(AmpHciController *ctx, HciCommandType cmd, AhcParameter *parameters);
BtStatus AhcSendCustomCommand(AmpHciController *ctx, HciCommand *cmd);
void AhcTransportEventHandler(AmpHciController *ctx, HciTransportEvent *p);
void AhcInitComplete(AmpHciController *ctx, BtStatus status, HciTransError err);

BtStatus AhcProcStart(AmpHciController *ctx, AhcProcedureCallback callback, AhcProcedure *proc);
void AhcProcEnd(AhcProcedure *proc);
void AhcProcAbortAll(AmpHciController *ctx);
void AhcProcAbort(AhcProcedure *proc);
void AhcBeginReset(AmpHciController *ctx);

void AHC_StopHardware(AmpHciController *ctx);
void AHC_ResumeHardware(AmpHciController *ctx);

/* A utility struct sometimes necessary when using IterateList */
typedef struct _NodeNode {
    ListEntry node;
} NodeNode;

/* Values for constructing a descriptor of cmd/event parameters */
#define HCIP_NONE              0
#define HCIP_8_BIT             "\x01"
#define HCIP_16_BIT            "\x02"
#define HCIP_24_BIT            "\x03"
#define HCIP_32_BIT            "\x04"
#define HCIP_6_BYTE_BUFFER     "\x86"
#define HCIP_ALL_BUFFER        "\xFF"


#endif /* __HCI_CONTROL_H */
