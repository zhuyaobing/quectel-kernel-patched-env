/*
***************************************************************************************
***************************************************************************************
***
***     File: crc.h
***
***     Project: SRE Linux API
***
***     Copyright (C) Stoneridge Electronics AB, 1990 - $Date:: 2014#$
***
***     This file is a part of the Stoneridge Common Software Linux API
***     and may only be used in accordance with the terms and conditions stipulated
***     in the agreement contract under which it has been supplied.
***
***************************************************************************************
**************************************************************************************/
/** \file

   \ingroup    SRE Linux API

   \Subject    C interface to calculate crc.

   \Reference  TBD

   \Abstract



   \Version Id:


   "$Id: crc.h 1144 2014-02-13 15:15:59Z STONERIDGE\jryd $"

\{
***************************************************************************************
*/
#ifndef CRC_H
#define CRC_H

#ifdef __cplusplus
extern "C" {
#endif

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/
#include <stdint.h>

/*
**=====================================================================================
** Macro definitions
**=====================================================================================
*/

uint16_t crc_calculateCrc16Slow(const uint8_t* data, int data_size, int start, int bytes);

#ifdef __cplusplus
}
#endif

#endif //CPROXY_H
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
