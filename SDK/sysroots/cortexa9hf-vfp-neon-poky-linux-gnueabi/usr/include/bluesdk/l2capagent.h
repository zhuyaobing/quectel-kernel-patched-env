/****************************************************************************
 *
 * File:
 *     l2capagent.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     L2CAP agent.
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

#ifndef __L2CAPAGENT_H
#define __L2CAPAGENT_H

#include "sys/l2capagent_i.h"

/*---------------------------------------------------------------------------
 * L2capAgent Class layer
 *
 *     The L2capAgent class (structure) is the base class for all L2CAP
 *     Agents. This L2CAP Agent provides a basic set of services. Applications
 *     can create more elaborate L2CAP Agents by inheriting from this base
 *     class. To inherit from this class simply have L2capAgent be the
 *     first field of the new class as shown:
 *
 *     struct NewL2capAgent {
 *         L2capAgent la;
 *         *** New fields };
 *
 *     There are situations where applications will need to allocate an
 *     L2capAgent. An L2capAgent can be allocated using the
 *     L2CAPAGENT_Alloc() factory, dynamically allocated using malloc()
 *     or by being statically allocated. If the L2capAgent is dynamically
 *     allocated or statically allocated its constructor, L2CAPAGENT_Create(),
 *     must be called on it before it can be used. After it is constructed the
 *     interface function (L2A_xxx defined in il2capagent.h) can be used.
 *     An L2capAgent structure allocated using L2CAPAGENT_Alloc() is already
 *     constructed so only init() must be called in order to use it.
 *
 *     L2capAgents allocated by the stack for incoming connections will be
 *     freed by the stack. L2capAgents allocated by the application for outgoing
 *     connections must be freed by the application by calling the L2capAgent's
 *     free function. The default free function does nothing so L2capAgents allocated
 *     dynamically using malloc or statically will work properly. L2capAgents
 *     allocated using L2CAPAGENT_Alloc() have the free function set
 *     appropriately.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 * L2capChannelState type
 *
 *     Describes the state of an L2CAP Channel.
 */
typedef U8 L2capChannelState;

/* Channel in state considered not active or not relevant. */
#define L2ACHANSTATE_NOT_ACTIVE     0

/* Channel that is in the process of being connected. */
#define L2ACHANSTATE_CONNECTING     1

/* Channel that is in the process of being configured. */
#define L2ACHANSTATE_CONFIGURING    2

/* Channel that is open - connected, configured and enabled for data flow */
#define L2ACHANSTATE_OPEN           3

/* Channel that is in the process of being moved */
#define L2ACHANSTATE_MOVING         4

/* Channel that is in the process of being disconnected or closed */
#define L2ACHANSTATE_DISCONNECTING  5

/* End of L2capChannelState */

/*---------------------------------------------------------------------------
 * L2capChannelCriteria type
 *
 *     The L2CAP Channel criteria is used to specify the type of channels
 *     that are to be returned using the L2A_GetActiveChannels() function
 *     based on L2capChannelState. The criteria is created by ORing together
 *     the values for the desired types of L2CAP channels. To retrieve all
 *     active channels use L2ACHANCRIT_ALL.
 */
typedef U16 L2capChannelCriteria;

#define L2ACHANCRIT_CONNECTING      0x0001
#define L2ACHANCRIT_CONFIGURING     0x0002
#define L2ACHANCRIT_OPEN            0x0004
#define L2ACHANCRIT_MOVING          0x0008
#define L2ACHANCRIT_DISCONNECTING   0x0010
#define L2ACHANCRIT_ALL             0xFFFF

/* End of L2capChannelCriteria */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * L2capAgent structure
 *
 *     Manages L2CAP channels.
 */
struct _L2capAgent
{
    /* Group: All fields are used internally by the stack **/

    ListEntry             node; /* Used internally by the stack. Must be first */

    L2capChannelId        cid;
    L2capAgentState       state;
    L2capConnStatus       responseCode;

    L2capPsmValue         psm;
    L2capPsm             *protocol;
    L2capConnSettings     settings;
    BtRemoteDevice       *remDev;

    /* Function to pass events to the Agent */
    L2capAgentCallback    callback;

    /* Physical link to use for creating channel */
    BtPhysicalLink       *pLink;

#if NUM_AMPS > 0 
    IL2capAgent           i;        /* Agent interface */
    U8                    remCrlId; /* ID of the remote controller */
    BtServiceId           serviceId;

    /* Following are used to interact with the Channel Manager */
    L2capAgentCMState     cmState;
    BtStatus              cmStatus;
    void                 *cmCtx;

    ControllerPreference  pref;
    AmpController        *ampc;

    /* Fields used in moving channels */
    ControllerPreference  movePref;
    U8                    moveRemCrlId;   /* Remote Controller ID in which to move */
    BtPhysicalLink       *movePlink;
    AmpController        *moveAmpc;
    BtLinkKeyType         keyType;        /* Save key type */
    EvmTimer              timer;          /* Timer used to retry create */
#endif /* NUM_AMPS > 0  */

    BtSecurityToken       authorization;  /* Passed to ME for access requests. */
    BOOL                  remDevRefCount; /* Set to TRUE if remDev refCount has been increased by agent */
};

/*---------------------------------------------------------------------------
 * L2capChannelDesc structure
 *
 *     Manages L2CAP channels.
 */
typedef struct _L2capChannelDesc
{
    L2capChannelState   state;

    /* The mode in which the channel is operating. The mode is only valid
     * if the channel is in the L2ACHANTYPE_OPEN or
     * L2ACHANGETYPE_DISCONNECTING states.
     */
    L2capLinkMode       mode;

    /* Pointer to channel's L2capAgent */
    L2capAgent         *agent;

    /* The efs pair is only valid if the channel is in the L2ACHANTYPE_OPEN or
     * L2ACHANGETYPE_DISCONNECTING states.
     */
    ExtFlowSpecPair     efsPair;

    /* Channel CID */
    L2capChannelId      cid;
} L2capChannelDesc;

/****************************************************************************
 *
 *  Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * L2CAPAGENT_Create()
 *
 *     Constructor for the L2capAgent structure. This function initializes
 *     the interface and must be called before the interface can be used.
 *
 * Parameters:
 *     agent - Pointer to object containing the L2CAP Agent interface
 *
 * Returns:
 *     void
 */
void L2CAPAGENT_Create(L2capAgent *agent);

/*---------------------------------------------------------------------------
 * L2CAPAGENT_Alloc()
 *
 *     Factory for allocating L2capAgent structures. The number of system
 *     L2capAgent structures is controlled by L2CAP_NUM_AGENTS in btconfig.h.
 *     If the application does not provide its own L2CAP Agent factory in
 *     L2capPsm structure then L2capAgents for incoming channels are allocated
 *     from the system. The free function of the L2capAgent is set appropriately.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     A constructed L2capAgent structure or 0 if out of resources.
 */
L2capAgent *L2CAPAGENT_Alloc(void);

/*---------------------------------------------------------------------------
 * L2A_RegisterPsm()
 *
 *     Registers a PSM identifier with L2CAP. Once registered, the protocol
 *     can initiate connections and data transfer as well as receive
 *     connection requests and data.
 *
 * Parameters:
 *     protocol - Pointer to the L2capPsm structure to be registered.
 *         This structure is owned by L2CAP until it is successfully
 *         deregistered. All fields in the structure must be initialized
 *         before this function is called.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - PSM was successfully registered with L2CAP.
 *
 *     BT_STATUS_FAILED - Invalid parameter value (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_RESOURCES - Maximum number of registered PSMs reached.
 *
 *     BT_STATUS_IN_USE - The protocol is already registered (XA_ERROR_CHECK only).
 */
BtStatus L2A_RegisterPsm(L2capPsm *protocol);


#if L2CAP_DEREGISTER_FUNC == XA_ENABLED
/*---------------------------------------------------------------------------
 * L2A_DeregisterPsm()
 *
 *     Deregisters a registered protocol from L2CAP.
 *
 * Requires:
 *     L2CAP_DEREGISTER_FUNC enabled.
 *
 * Parameters:
 *     protocol - Pointer to L2capPsm structure previously registered with
 *         the L2CAP_RegisterPsm function call.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The protocol has been deregistered.
 *
 *     BT_STATUS_FAILED - The protocol parameter was invalid (XA_ERROR_CHECK
 *         only).
 *
 *     BT_STATUS_NOT_FOUND - The protocol was not previously registered.
 *
 *     BT_STATUS_BUSY - The protocol has one or more active channels and
 *         cannot be deregistered.
 */
BtStatus L2A_DeregisterPsm(const L2capPsm *protocol);

#endif /* L2CAP_DEREGISTER_FUNC == XA_ENABLED */

/*---------------------------------------------------------------------------
 * L2A_GetActiveChannels()
 *
 *     Retrieve an array of channel descriptors identifying active channels
 *     that meet the criteria specified by the "criteria" field. If the desc
 *     array pointer is 0 then no descriptors are returned. The total
 *     number of channels that meet the criteria is always returned so this
 *     function can be used to determine the total number that could be
 *     returned. The total number of channel descriptors actually returned
 *     in the desc array is copied to the memory pointed to by "total".
 *     If the "total" pointer is 0 then the number copied is not returned.
 *
 * Parameters:
 *     size - number of items in the array pointed to by desc. If the size
 *         is 0 then no L2CAP channel descriptors are returned.
 *
 *     desc - array of BtL2capChanneldesc structures to receive L2CAP channel
 *         descriptors. If the pointer is 0 no descriptors are returned.
 *
 *     criteria - defines the type of L2CAP channels that should be returned.
 *
 *     total - pointer to memory to receive the total number of L2CAP
 *         channels descriptors returned in "desc". If the pointer is 0 then
 *         total is not returned.
 *
 * Returns:
 *     Total number of L2CAP Channels that meet the criteria in the "criteria" field.
 */
U16 L2A_GetActiveChannels(U16 size, L2capChannelDesc desc[], L2capChannelCriteria criteria, U16 *total);

/*---------------------------------------------------------------------------
 * L2A_BeginMove()
 *
 *     Prepare a channel for Move to a different controller.
 *
 * Parameters:
 *     ChannelId - Channel ID assigned by L2CAP to this connection.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The move preparation has started.
 *
 *     BT_STATUS_FAILED - The ChannelId parameter was invalid (XA_ERROR_CHECK
 *         only).
 */
BtStatus L2A_BeginMove(L2capChannelId ChannelId);

/*---------------------------------------------------------------------------
 * L2A_EndMove()
 *
 *     End move preparation. This function may be called independent of
 *     L2A_BeginMove().
 *
 * Parameters:
 *     ChannelId - Channel ID assigned by L2CAP to this connection.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The move preparation has completed.
 *
 *     BT_STATUS_FAILED - The ChannelId parameter was invalid (XA_ERROR_CHECK
 *         only).
 */
BtStatus L2A_EndMove(L2capChannelId ChannelId);

#endif /* __L2CAPAGENT_H */
