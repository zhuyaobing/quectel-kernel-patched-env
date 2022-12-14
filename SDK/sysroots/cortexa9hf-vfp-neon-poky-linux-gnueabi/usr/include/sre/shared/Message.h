/*
***************************************************************************************
***************************************************************************************
***
***     File: Message.h
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

   \Subject    Linux API message definition.

   \Reference  TBD

   \Abstract



   \Version Id:


   "$Id: Message.h 1261 2014-03-19 07:17:38Z STONERIDGE\jryd $"


@{
***************************************************************************************
*/

/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef MESSAGE_H_
#define MESSAGE_H_

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/


/*
**=====================================================================================
** Macro definitions
**=====================================================================================
*/


/*
**=====================================================================================
** type definitions
**=====================================================================================
*/


class Message
{
public:
   Message(){};
   virtual ~Message(){};
};

#endif  // MESSAGE_H_
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
