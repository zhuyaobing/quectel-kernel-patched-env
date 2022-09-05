/*
***************************************************************************************
***************************************************************************************
***
***     File: EntryHandler.h
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

    \Subject    Linux API Entry handler definition.

    \Reference  TBD

    \Abstract




    \Version Id:


    "$Id: EntryHandler.h 1299 2014-04-02 14:01:50Z STONERIDGE\jryd $"


@{
***************************************************************************************
*/

#ifndef ENTRYHANDLER_H_
#define ENTRYHANDLER_H_

#include <stdint.h>
#include <stdio.h>

namespace freja
{

class Entry;


class EntryHandler
{
public:
   virtual ~EntryHandler(){};

   virtual uint32_t handleEntry(Entry *entry_ptr, void *extra_data_ptr = NULL, uint32_t extra_info = 0) = 0;
};

} /* namespace freja */

#endif /* ENTRYHANDLER_H_ */
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
