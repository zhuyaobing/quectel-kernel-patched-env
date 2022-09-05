/*
***************************************************************************************
***************************************************************************************
***
***     File: TlvFunctionClass.h
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

   \Subject    Linux API function class definition.

   \Reference  TBD

   \Abstract



   \Version Id:


   "$Id: TlvFunctionClass.h 1334 2014-04-23 10:27:05Z STONERIDGE\PSM $"


@{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef TLV_FUNCTION_CLASS_H
#define TLV_FUNCTION_CLASS_H

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/

#include "shared/sre_client_base.h"

/**
 **************************************************************************************
 **
 **   @brief Base class to implement manager functionality functions.
 **
 **************************************************************************************
 **/
class TlvFunctionClass
{
public:

   TlvFunctionClass()
   {}

   virtual ~TlvFunctionClass() {}

   virtual void doFunction(freja::TlvMessage &in_message, freja::TlvMessage &reply_message) = 0;

   virtual const std::string functionName() = 0;

   bool findSubscriber(uint16_t tag);

   void addSubscriber(uint16_t tag) { m_subscribers.push_back(tag); }

   void removeSubscriber(uint16_t tag);

private:
   std::vector<uint16_t> m_subscribers;
};

#endif  // TLV_FUNCTION_CLASS_H
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
