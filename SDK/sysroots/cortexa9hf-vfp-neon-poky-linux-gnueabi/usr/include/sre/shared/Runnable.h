/*
***************************************************************************************
***************************************************************************************
***
***     File: Runnable.h
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

   \ingroup      SRE Linux API

   \Subject      Linux API "Change this!" definition.

   \Reference

   \Abstract

   \Version

   "$Id: Runnable.h 1294 2014-04-02 10:32:17Z STONERIDGE\jryd $"

   \{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/
#ifndef RUNNABLE_H_
#define RUNNABLE_H_


/*
**=====================================================================================
** Imported definitions, local
**=====================================================================================
*/


/*
**=====================================================================================
** Imported definitions, external
**=====================================================================================
*/


/*
**=====================================================================================
** Forward declarations
**=====================================================================================
*/

namespace freja
{
/**
 **************************************************************************************
 **
 **   @brief Defines a runnable interface.
 **
 **          The idea is that another class may take a pointer to a Runnable
 **          and schedule runtime for it.
 **
 **************************************************************************************
 **/
class Runnable
{
public:
   virtual ~Runnable() {};

   //! This class should be overriden by subclasses. It is intended to be called after
   //! e.g. notify when we know that we have something to process from e.g. a queue
   virtual void run() = 0;
};

} // namespace freja
#endif // RUNNABLE_H_

/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
