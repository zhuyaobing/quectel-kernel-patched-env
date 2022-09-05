/*
***************************************************************************************
***************************************************************************************
***
***     File: MessageData.h
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

   \Subject    Linux API message data definition base class.

   \Reference  TBD

   \Abstract



   \Version Id:


   "$Id: MessageData.h 1277 2014-03-19 16:41:58Z STONERIDGE\PSM $"


@{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef MESSAGE_DATA_H_
#define MESSAGE_DATA_H_

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/

#include <stdint.h>

/*
**=====================================================================================
** type definitions
**=====================================================================================
*/

/**
 *************************************************************************************
 **
 **  @brief MessageData - Base class for message data.
 **
 **************************************************************************************
 **/
class MessageData
{
public:
   MessageData()
   {}

   virtual ~MessageData()
   {}

};

#endif  // MESSAGE_DATA_H_
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/