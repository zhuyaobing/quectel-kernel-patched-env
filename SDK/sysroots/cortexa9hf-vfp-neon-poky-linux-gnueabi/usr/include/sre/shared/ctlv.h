/*
***************************************************************************************
***************************************************************************************
***
***     File: ctlv.h
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

   \Subject    C Wrapper for the tlv message.

   \Reference

   \Abstract



   \Version Id:


   "$Id: ctlv.h 1243 2014-03-18 07:39:55Z STONERIDGE\PSM $"

\{
***************************************************************************************
*/
#ifndef CTLV_H_
#define CTLV_H_

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
** Forward declarations, opaque pointer implementation
**=====================================================================================
*/
struct CTlv;
typedef struct CTlv* CTlvHndl;

CTlvHndl ctlv_createHandle();

void ctlv_deleteHandle(CTlvHndl* tlv_hndl);

int ctlv_setFromRawMsg(CTlvHndl tlv_hndl, const uint8_t* data_ptr, int size);

uint16_t ctlv_getTlvVersion(const CTlvHndl tlv_hndl);

#ifdef __cplusplus
}
#endif

#endif //CTLV_H_
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
