/***************************************************************************
 *
 * File:
 *     btrom.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file contains external references to global constant data
 *     for the stack.
 *
 * Created:
 *     March 6, 2000
 *
 * Copyright 2000-2017 OpenSynergy GmbH.
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

#include "bttypes.h"
#include "sdp.h"

/* CRC table for calculating FCS for RFCOMM frames */
extern const U8 crctable[256];

#define SDP_ZERO_SERV_CLASS_ID_SIZE 5
extern const U8 ZeroServClassIdVal[SDP_ZERO_SERV_CLASS_ID_SIZE];

#define SDP_ZERO_VER_NUMBER_SIZE    8
extern const U8 ZeroVersionNumberList[SDP_ZERO_VER_NUMBER_SIZE];

/* Bluetooth base UUID for SDP */
extern const U8 BT_BASE_UUID[16];

/* Table for new version of CRC-CCITT generator used by L2CAP.*/
extern const U16 L2CrcTab[16];

extern const U16 L2Crc16Tab[256];

