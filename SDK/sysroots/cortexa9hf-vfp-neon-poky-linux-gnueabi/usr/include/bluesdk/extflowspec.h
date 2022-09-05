/****************************************************************************
 *
 * File:
 *     extflowspec.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     Extended Flow Specification.
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

#ifndef __EXTFLOWSPEC_H
#define __EXTFLOWSPEC_H

#include "bttypes.h"

/*---------------------------------------------------------------------------
 * Extended Flow Specification (EXTFLOW) layer
 *
 *     An Extended Flow Specification is used by an application to specify
 *     its outgoing data flow. By passing an Extended Flow Specification
 *     to L2CAP via an L2capAgent the application is making a Quality of
 *     Service (QoS) request. 
 *
 *     L2CAP exchanges Extended Flow Specifications during channel configuration
 *     (if both devices support them). A complete QoS request involves both
 *     the local and remote Extended Flow Specifications. An Extended Flow 
 *     Specification Pair contains both Extended Flow Specifications. 
 *
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * EfsServType type
 *
 * EfsServType indicates the type of a flow spec. The two types are Best Effort
 * and Guaranteed. Within a Pair of Flow specs it is not possible to have
 * one be Guaranteed while the other is Best Effort or both to be "No Traffic" 
 * but one can be of type "No Traffic". In a pair, if one is type "No Traffic" 
 * then the type of the pair is the type of the flow spec that is not 
 * "No Traffic". 
 *
 */
typedef U8 EfsServType;

#define EFS_TYPE_NO_TRAFFIC  0x00
#define EFS_TYPE_BEST_EFFORT 0x01
#define EFS_TYPE_GUARANTEED  0x02

/* End of EfsServType */


/*---------------------------------------------------------------------------
 * ExtFlowSpec structure
 * 
 *     Defines the outgoing (tx) data flow from the layer (profile or application)
 *     sitting above L2CAP to its peer on the remote device. A pair of 
 *     ExtFlowSpec structures is passed to AMP controllers when creating a 
 *     logical link. 
 */
typedef struct _ExtFlowSpec
{
    U8          id;               /* Identifier created by stack */
    EfsServType servType;         /* Service Type */
    U16         maxSduSize;       /* Maximum SDU size octets */
    U32         sduIntArrTime;    /* SDU Inter-arrival Time microseconds */
    U32         accLatency;       /* Access Latency microseconds */
    U32         flushtime;        /* Flush Timeout microseconds */

} ExtFlowSpec;

/*---------------------------------------------------------------------------
 * ExtFlowSpecPair structure
 * 
 *     A logical link is defined by a pair of flow specs, an outgoing (tx) and
 *     incoming (rx) flow specs. The outgoing flow spec is provided by the local
 *     application/profile while the incoming flow spec is provided by the
 *     remote device. Flow specs are exchanged using the Lockstep configuration
 *     process when creating an L2CAP channel or reconfiguring one.
 */
typedef struct _ExtFlowSpecPair
{
    EfsServType type;             /* Type agreed by both flow specs (Best effort or Guaranteed) */
    U16         count;            /* Number of flow specs aggregated */
    ExtFlowSpec out;              /* Out going (tx) flow spec (from local device) */
    ExtFlowSpec in;               /* Incoming (rx) flow spec (from remote device) */

} ExtFlowSpecPair;


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * EXTFLOW_InitDefault()
 *
 *     Initialize an Extended Flow Specification object with default
 *     parameters which is Best Effort with all 0xFF values (except
 *     the ID field).
 *
 * Parameters:
 *     efs - Pointer to an Extended Flow Specification.
 *
 * Returns:      
 *     void 
 *
 */
void EXTFLOW_InitDefault(ExtFlowSpec *efs);


#endif /* __EXTFLOWSPEC_H */
