/****************************************************************************
 *
 * File:
 *     iphylink.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     logical link in the Management Entity.
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

#ifndef __IPHYLINK_H
#define __IPHYLINK_H

#include "extflowspec.h"

/*---------------------------------------------------------------------------
 * BtPhysicalLink type
 *
 *    Forward reference to BtPhysicalLink structure defined in phylink.h
 */
typedef struct _BtPhysicalLink BtPhysicalLink;

/* End of BtPhysicalLink type */

/*---------------------------------------------------------------------------
 * PHYLINK::Init()
 *
 *     Initialize the physical link object and associate with the Remote 
 *     device. 
 *
 * Parameters:
 *     ctx - Pointer to object containing the Physical link interface
 *
 *     remDev - Pointer to the remote device to which the physical link is to
 *         be associated
 *
 * Returns:
 *     BT_STATUS_SUCCESS.  The ctx parameter will be set to a pointer to a 
 *     BtPhysicalLink object or 0 if a phylink does not exist. 
 *
 */
typedef BtStatus (*BtPhLinkInitFunc)(BtPhysicalLink *ctx, void /*BtRemoteDevice*/ *RemDev);

/*---------------------------------------------------------------------------
 * PHYLINK::ReceiveData()
 *
 *     Parses the incoming data buffer and if approprpiate passes it to the
 *     L2CAP layer. This function is called by the physical link's controller 
 *     when a data buffer has been received. 
 *
 * Parameters:
 *     ctx - Pointer to object containing the Link interface
 *
 *     HciBuff - pointer to received data in HciBuffer format.
 *
 *     FirstPacket - set to TRUE if this is the first or only buffer associated
 *     with an L2CAP frame. It is set to FALSE for subsequent buffers of the
 *     the same L2CAP frame. Some controllers fragment L2CAP frames. 
 *
 * Returns:      
 *     void
 *
 */
typedef void (*BtPhLinkReceiveDataFunc)(BtPhysicalLink *ctx, void *HciBuff, BOOL FirstPacket);

/*---------------------------------------------------------------------------
 * PHYLINK::AllocLink()
 *
 *     Allocate a logical link object associated with the physical link. If
 *     isBestEffort is TRUE and a Best Effort logical link is already 
 *     connected then the existing Best Effort logical link is returned
 *     otherwise a new logical link is returned. If no logical link objects
 *     are available then 0 is returned. All best effort channels over the 
 *     same physical link share a single logical link.
 *
 * Parameters:
 *     ctx - Pointer to object containing the Physical link interface
 *
 *     isBestEffort - If TRUE then allocate a logical link to be used for a Best Effort
 *         channel otherwise allocate a logical link to be used for Guaranteed
 *         channel. 
 *
 * Returns:      
 *     Pointer to a BtLogicalLink object or 0 if no logical link objects exist. 
 *
 */
typedef void * (*BtPhLinkAllocLinkFunc)(BtPhysicalLink *ctx, BOOL isBestEffort);

/*---------------------------------------------------------------------------
 * PHYLINK::FreeLink()
 *
 *     Free the memory for the logical link structure. 
 *
 * Parameters:
 *     ctx - Pointer to object containing the Physical link interface
 *
 *     Llink - Pointer to logical link to be freed.
 *
 * Returns:      
 *     TRUE - the logical link has been successfully freed
 *
 *     FALSE - the logical link stucture was not free because
 *         it has L2CAP channels associated with it.
 *
 */
typedef BOOL (*BtPhLinkFreeLinkFunc)(BtPhysicalLink *ctx, void *Llink);

/*---------------------------------------------------------------------------
 * PHYLINK::TxReady()
 *
 *     Return the next packet available on the transmit queue(s) for 
 *     transmission. This function is called by the controller when
 *     transmit resources are available
 *
 * Parameters:
 *     ctx - Pointer to object containing the Physical link interface
 *
 *     Broadcast - if non-0 then pull the packet from the broadcast queue
 *         otherwise use the normal transmit queues.
 *
 * Returns:      
 *     Pointer to BtPacket or 0 if no transmit packets are available.
 *
 */
typedef BtPacket * (*BtPhLinkTxReadyFunc)(BtPhysicalLink *ctx,
                                          BtPriorityToken *PriToken,
                                          HciHandle Broadcast);

/*---------------------------------------------------------------------------
 * PHYLINK::TxDone()
 *
 *     Indicates that transmission of the packet is complete.
 *
 *     Callers to this function must ensure that packet completion is
 *     indicated before indicating any other rx data that arrived as a
 *     result of the transmission of this packet.
 *
 * Parameters:
 *     ctx - Pointer to object containing the Physical link interface
 *
 *     pkt - A BtPacket pointer (previously returned by ::TxReady)
 *
 *     status - BT_STATUS_SUCCESS if the packet was delivered,\
 *         BT_STATUS_FAILED if not.
 *
 */
typedef void (*BtPhLinkTxDoneFunc)(BtPhysicalLink *ctx, BtPacket *pkt, BtStatus status);

/*---------------------------------------------------------------------------
 * PHYLINK::CreateLogLink()
 *
 *     Create a logical connection to the remote AMP controller over the
 *     physical link. If the Logical link is type Best Effort and already
 *     exists then the Extended Flow Specification will be aggregated
 *     and a Modify Flow Spec issued to the controller.
 *
 *
 * Parameters:
 *     ctx - Pointer to object containing the Physical link interface
 *
 *     Llink - Pointer to the logical link
 *
 *     efsPair - Pointer to the Extended Flow Specification pair. Before
 *         calling this function the id should be set by calling
 *         PHYLINK:SetEfsId on the outgoing extended flow spec in the pair.
 *
 * Returns:      
 *     BT_STATUS_PENDING - operation started successfully. The link callback
 *         registered with the ME will be called with the 
 *         LINK_EVENT_LOGLINK_CREATE event.
 *
 *     BT_STATUS_SUCCESS - operation completed succesfully. The callback will not
 *         be called.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because the physical link is not
 *         connected.
 *
 *     BT_STATUS_QOS_FAILED - operation failed because the guaranteed Extended Flow
 *         specification failed admission control.
 *
 *     BT_STATUS_INVALID_TYPE - operation failed because the efsPair is an invalid
 *         type - not the same type as what exists already for the logical link or
 *         the type and the parameters do not make sense.
 *
 *     BT_STATUS_FAILED - operation failed because the efsPair is type Best Effort
 *         and a best effort logical link already exists but the logical link 
 *         object passed in is the not the same.
 */
typedef BtStatus (*BtPhLinkCreateLogLinkFunc)(BtPhysicalLink *ctx, void *Llink, ExtFlowSpecPair *efsPair);

/*---------------------------------------------------------------------------
 * PHYLINK::AcceptLogLink()
 *
 *     Accept a logical connection initiated by the remote AMP controller over the
 *     physical link. If the Logical link is type Best Effort and already
 *     exists then the Extended Flow Specification will be aggregated
 *     and a Modify Flow Spec issued to the controller.
 *
 * Parameters:
 *     ctx - Pointer to object containing the Physical link interface
 *
 *     Llink - Pointer to the logical link
 *
 *     efsPair - Pointer to the Extended Flow Specification pair. Before
 *         calling this function the id should be set by calling
 *         PHYLINK:SetEfsId on the outgoing extended flow spec in the pair.
 *
 * Returns:      
 *     BT_STATUS_PENDING - operation started successfully. The link callback
 *         registered with the ME will be called with the 
 *         LINK_EVENT_LOGLINK_CREATE event.
 *
 *     BT_STATUS_SUCCESS - operation completed succesfully. The callback will not
 *         be called.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because the physical link is not
 *         connected.
 *
 *     BT_STATUS_QOS_FAILED - operation failed because the guaranteed Extended Flow
 *         specification failed admission control.
 *
 *     BT_STATUS_INVALID_TYPE - operation failed because the efsPair is an invalid
 *         type - not the same type as what exists already for the logical link or
 *         the type and the parameters do not make sense.
 *
 *     BT_STATUS_FAILED - operation failed because the efsPair is type Best Effort
 *         and a best effort logical link already exists but the logical link 
 *         object passed in is the not the same.
 *
 */
typedef BtStatus (*BtPhLinkAcceptLogLinkFunc)(BtPhysicalLink *ctx, void *Llink, ExtFlowSpecPair *efsPair);

/*---------------------------------------------------------------------------
 * PHYLINK::DiscLogLink()
 *
 *     Disconnect the Logical link. This function should be called each time
 *     an L2CAP channel associated with the logical link is disconnected. 
 *     It is assumed that the channel intended to be removed from the logical 
 *     link has already been removed by using LOGLINK_DelChannel(). If the 
 *     logical link type is Best Effort then the EfsPair of the logical link 
 *     is regenerated by aggregating the EfsPairs of the remaining channels.
 *     If the new EfsPair aggregate has changed then a Modify Flow Spec is 
 *     issued to the controller. If all the channels have been removed from the
 *     the logical link a Disconnect Logical Link is issued to the controller. 
 *
 * Parameters:
 *     ctx - Pointer to object containing the Physical link interface
 *
 *     Llink - pointer to BtLogicalLink object to disconnect
 *
 *     efsPair - Pointer to the Extended Flow Specification pair of the
 *         channel being disconnected. 
 *
 * Returns:      
 *     BT_STATUS_PENDING - operation started successfully. The link callback
 *         registered with the ME will be called with the 
 *         LINK_EVENT_LOGLINK_DISC event.
 *
 *     BT_STATUS_IN_USE - there are still channels associated with the 
 *         logical link. A new EfsPair aggregate was calculated for the
 *         logical link from the remaining channels and if the EfsPair
 *         aggregate changed then the controller has been informed by 
 *         issuing a Modify Flow Spec. No events will be issued.
 *
 *     BT_STATUS_SUCCESS - the operation was successful. No events with be issued.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because the physical link is
 *         not connected or the logical link is not associated with the
 *         physical link
 */
typedef BtStatus (*BtPhLinkDiscLogLinkFunc)(BtPhysicalLink *ctx, void *Llink, ExtFlowSpecPair *efsPair);

/*---------------------------------------------------------------------------
 * PHYLINK::ModFlowSpec()
 *
 *     Modify the logical link's Extended Flow specification with the given
 *     Extended Flow Specification Pair.
 *
 * Parameters:
 *     ctx - Pointer to object containing the Physical link interface
 *
 *     Llink - Pointer to the BtLogicalLink object whose flow spec pair is
 *         being modified.
 *
 *     efsPair - Pointer to the Extended Flow Spec Pair
 *
 * Returns:      
 *     BT_STATUS_PENDING - operation started successfully. The link callback
 *         registered with the ME will be called with the 
 *         LINK_EVENT_LOGLINK_MOD_FLOW event.
 *
 *     BT_STATUS_NO_CONNECTION - operation failed because the physical link is
 *         not connected or the logical link is not associated with the
 *         physical link
 *
 *     BT_STATUS_INVALID_TYPE - operation failed because the efsPair is an invalid
 *         type - not the same type as what exists already for the logical link or
 *         the type and the parameters do not make sense.
 *
 */
typedef BtStatus (*BtPhLinkModFlowSpecFunc)(BtPhysicalLink *ctx, void *Llink, ExtFlowSpecPair *efsPair);

#if L2CAP_NUM_ENHANCED_CHANNELS > 0 
/*---------------------------------------------------------------------------
 * PHYLINK::SetEfsId()
 *
 *     Set the ID field of the Extended Flow Specification so that it is
 *     unique from the perspective of the Physical Link. This function
 *     only guarantees that it is unique based on the logical links
 *     currently associated with the physical link via createLogLink or
 *     acceptLogLink calls.
 *
 * Parameters:
 *     ctx - Pointer to object containing the Physical link interface
 *
 *     efs - pointer to Extended Flow Spec whose ID will be set. The
 *           other values of the Extended Flow Spec must be set since
 *           the ID is based on the service type of the Extended Flow Spec.
 *
 *     bestEffort - TRUE means flow spec is service type Best effort 
 *           while FALSE means Guaranteed. This needed for the case
 *           where the servType is set to EFS_TYPE_NO_TRAFFIC.
 *
 * Returns:      
 *     void 
 *
 */
typedef void (*BtPhLinkSetEfsIdFunc)(BtPhysicalLink *ctx, ExtFlowSpec *efs, BOOL bestEffort);

/*---------------------------------------------------------------------------
 * PHYLINK::UseLockstep()
 *
 *     Requests the Lockstep capability of the physical link. 
 *
 * Parameters:
 *     ctx - Pointer to object containing the Physical link interface
 *
 * Returns:      
 *     TRUE - Use the Lockstep configuration process for channels
 *           on this physical link.
 *
 *     FALSE - Use the Standard configuration process for channels
 *           on this physical link.
 *
 */
typedef BOOL (*BtPhLinkUseLockstepFunc)(BtPhysicalLink *ctx);

#endif /* #if L2CAP_NUM_ENHANCED_CHANNELS > 0 */ 

/*---------------------------------------------------------------------------
 * PHYLINK::ForceDisconnect()
 *
 *     Force the phylink to be disconnected. 
 *
 * Parameters:
 *     ctx - Pointer to object containing the Physical link interface
 *
 * Returns:      
 *     BT_STATUS_PENDING - the operation was started successfully.
 *
 *     BT_STATUS_NO_CONNECTION - Request failed because there is no connection
 *
 *     BT_STATUS_HCI_INIT_ERR - The lower level transport is not in an initialized
 *         state and therefore the operation cannot proceed or the controller is 
 *         disabled.
 */
typedef BtStatus (*BtPhLinkForceDisconnectFunc)(BtPhysicalLink *ctx);

/*---------------------------------------------------------------------------
 * PHYLINK::GetIdAndType()
 *
 *     Return the Local Controller ID and Local Controller type for the
 *     phylink 
 *
 * Parameters:
 *     ctx - Pointer to object containing the Physical link interface
 *
 *     pId - pointer to memory to receive the Controller ID. If the pointer
 *         is 0 then the Controller ID is not returned.
 *
 *     pType - pointer to memory to receive the Controller Type If the pointer
 *         is 0 then the Controller type is not returned.
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - operation completed successfully
 *
 *     BT_STATUS_FAILED - operation failed because the phylink is not
 *         currently connected or is invalid.
 *
 */
typedef BtStatus (*BtPhLinkGetIdAndTypeFunc)(BtPhysicalLink *ctx, U8 *pId, 
                                             BtControllerType *pType);

/*---------------------------------------------------------------------------
 * PHYLINK::GetMaxPduSize()
 *
 *     Return the Maximim PDU size that can be transmitted on this
 *     physical link
 *
 * Parameters:
 *     ctx - Pointer to object containing the Physical link interface
 *
 * Returns:      
 *     >0 - Maximim PDU size that can be transmitted on this physical link.
 *
 *      0 - operation failed because the phylink is not
 *          currently connected or is invalid.
 *
 */
typedef U32 (*BtPhLinkGetMaxPduSizeFunc)(BtPhysicalLink *ctx);

/*---------------------------------------------------------------------------
 * PHYLINK::IsKeyChanged()
 *
 *     Return TRUE if the phylink is disconnecting because of a link key 
 *         change. 
 *
 * Parameters:
 *     ctx - Pointer to object containing the Physical link interface
 *
 */
typedef BOOL (*BtPhLinkIsKeyChanged)(BtPhysicalLink *ctx);

/*---------------------------------------------------------------------------
 * BtPhysicalLink Interface structure
 *
 *     Physical Link Interface.
 */
typedef struct _iPhysicalLink
{
    BtPhLinkInitFunc            init;
    BtPhLinkReceiveDataFunc     receiveData;
    BtPhLinkAllocLinkFunc       allocLink;
    BtPhLinkFreeLinkFunc        freeLink;
    BtPhLinkTxReadyFunc         txReady;
    BtPhLinkTxDoneFunc          txDone;
    BtPhLinkCreateLogLinkFunc   createLogLink;
    BtPhLinkAcceptLogLinkFunc   acceptLogLink;
    BtPhLinkDiscLogLinkFunc     discLogLink;
    BtPhLinkModFlowSpecFunc     modFlowSpec;

#if L2CAP_NUM_ENHANCED_CHANNELS > 0 
    BtPhLinkSetEfsIdFunc        setEfsId;
    BtPhLinkUseLockstepFunc     useLockstep;
#endif

    BtPhLinkForceDisconnectFunc forceDisconnect;
    BtPhLinkGetIdAndTypeFunc    getIdAndType;
    BtPhLinkGetMaxPduSizeFunc   getMaxPduSize;
    BtPhLinkIsKeyChanged        isKeyChanged;
} iPhysicalLink;

#endif /* __IPHYLINK_H */

