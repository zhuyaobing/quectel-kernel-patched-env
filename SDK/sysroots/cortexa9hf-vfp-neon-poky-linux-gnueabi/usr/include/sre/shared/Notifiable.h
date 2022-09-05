/*
***************************************************************************************
***************************************************************************************
***
***     File: Notifiable.h
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

    \Subject    Linux API tcp server definition.

    \Reference  TBD

    \Abstract



    \Version Id:


    "$Id: Notifiable.h 1299 2014-04-02 14:01:50Z STONERIDGE\jryd $"


@{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef NOTIFIABLE_H_
#define NOTIFIABLE_H_

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

namespace freja
{
/**
 *************************************************************************************
 **
 **  @brief Base class to handle connections and messages.
 **
 **************************************************************************************
 **/
class Notifiable
{
public:
   virtual ~Notifiable(){};

   //! Defaults to -1, to be able to use enums starting at zero as events
   virtual void notify(int32_t param_1 = -1, int32_t param_2 = -1) = 0;
};

} /* naespace freja */
#endif  // NOTIFIABLE_H_

/*************************************************************************************/
/** @}
*********************************** End of file **************************************/
