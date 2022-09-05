/****************************************************************************
 *
 * File:
 *     extflowspec_i.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     System types, defines, and prototypes for accessing the
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

#ifndef __EXTFLOWSPEC_I_H
#define __EXTFLOWSPEC_I_H

#include "extflowspec.h"

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * EXTFLOW_Init()
 *
 *     Initialize an Extended Flow Specification Pair object. This function
 *     should be done before using an extended flow specification pair object. 
 *
 * Parameters:
 *     efsPair - Pointer to the Extended Flow Specification pair.
 *
 * Returns:      
 *     void 
 *
 */
void EXTFLOW_Init(ExtFlowSpecPair *EfsPair);


/*---------------------------------------------------------------------------
 * EXTFLOW_SetOut()
 *
 *     Set the outgoing Extended Flow Specification. The id field of the flow
 *     spec will set by the stack when a logical link is created or accepted.
 *
 *     Note: values for sduIntArrTime should be greater than 50 for Best
 *     Effort Extended Flow Specifications because of the significant effect
 *     of rounding errors durin aggregation. Since Best Effort uses sduIntArrTime
 *     only as part of speed calculations the value for MaxSduSize can be 
 *     increased so that the correct speed is achieved with a larger sduIntArrTime.
 *     speed (in BPS) = (maxSduSize * 8000000)/sduIntArrTime
 *     
 *
 * Parameters:
 *     efsPair - Pointer to the Extended Flow Specification pair.
 *
 *     outEfs - Pointer to the outgoing Extended Flow Specification
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - operation successful
 *
 *     BT_STATUS_INVALID_TYPE - operation failed because the type of the
 *         incoming Extended Flow Specification does not match the
 *         the outgoing.
 *
 */
BtStatus EXTFLOW_SetOut(ExtFlowSpecPair *efsPair, ExtFlowSpec *outEfs);

/*---------------------------------------------------------------------------
 * EXTFLOW_SetIn()
 *
 *     Set the incoming Extended Flow Specification. The id field of the flow
 *     spec will set by the stack when a logical link is created or accepted.
 *     For an Extended Flow Specification of type Best Effort the maxSduSize
 *     and sduIntArrTime will be adjusted so that sduIntArrTime is greater
 *     than 50.
 *
 * Parameters:
 *     efsPair - Pointer to the Extended Flow Specification pair.
 *
 *     inEfs - Pointer to the incoming Extended Flow Specification (from
 *             the remote device) as it is found in an L2CAP configuration
 *             request packet.
 *
 * Returns:      
 *     BT_STATUS_SUCCESS - operation successful
 *
 *     BT_STATUS_INVALID_TYPE - operation failed because the type of the
 *         outgoing Extended Flow Specification already present in the
 *         Extended Flow Specification pair does not match the
 *         the incoming.
 *
 */
BtStatus EXTFLOW_SetIn(ExtFlowSpecPair *efsPair, U8 *inEfs);

/*---------------------------------------------------------------------------
 * EXTFLOW_Aggregate()
 *
 *     Aggregate the two Best Effort flow specs putting the result in 
 *     destEfsPair. The count in destEfsPair is incremented. It is assumed
 *     the aggregate and efsPair are valid Best Effort flow specs. This
 *     function return TRUE if the aggregation mas modified and FALSE otherwise. 
 *     If it returns TRUE then a modify flow spec operation is required.
 *
 *
 * Parameters:
 *     destPair - pointer to existing aggregate to which efsPair
 *         will be aggregated. 
 *
 *     efsPair - Pointer to the Extended Flow Specification pair.
 *
 * Returns:      
 *     TRUE the aggregate was modified. FALSE the aggregate was not changed.
 *
 */
BOOL EXTFLOW_Aggregate(ExtFlowSpecPair *destEfsPair, ExtFlowSpecPair *efsPair);

/*---------------------------------------------------------------------------
 * EXTFLOW_IsValidPair()
 *
 *     Return TRUE is if Extended Flow Specification pair valid otherwise 
 *     return FALSE.
 *
 *
 * Parameters:
 *     efsPair - Pointer to the Extended Flow Specification Pair.
 *
 * Returns:      
 *     TRUE - if valid otherwise FALSE. 
 *
 */
BOOL EXTFLOW_IsValidPair(ExtFlowSpecPair *efsPair);

/*---------------------------------------------------------------------------
 * EXTFLOW_IsValidBestEffort()
 *
 *     Return TRUE is Extended Flow Specification pair is of type Best 
 *     Effort and the fields are valid. Otherwise return FALSE.
 *
 *
 * Parameters:
 *     efs - Pointer to the Extended Flow Specification.
 *
 * Returns:      
 *     TRUE - if valid otherwise FALSE. 
 *
 */
BOOL EXTFLOW_IsValidBestEffort(ExtFlowSpec *efs);

/*---------------------------------------------------------------------------
 * EXTFLOW_IsPairValidBestEffort()
 *
 *     Return TRUE if the Extended Flow Spec Pair is of type Best Effort
 *     and all the parameters are valid. 
 *
 *
 * Parameters:
 *     efsPair - Pointer to the Extended Flow Specification pair.
 *
 * Returns:      
 *     TRUE - if valid otherwise FALSE. 
 *
 */
BOOL EXTFLOW_IsPairValidBestEffort(ExtFlowSpecPair *efsPair);

/*---------------------------------------------------------------------------
 * EXTFLOW_SetEfsPairType()
 *
 *     Set the type and return TRUE is if Extended Flow Specification pair
 *     valid otherwise return FALSE.
 *
 *
 * Parameters:
 *     efsPair - Pointer to the Extended Flow Specification Pair.
 *
 * Returns:      
 *     TRUE - if valid otherwise FALSE. 
 *
 */
BOOL EXTFLOW_SetEfsPairType(ExtFlowSpecPair *efsPair);

#endif /* __EXTFLOWSPEC_I_H */

