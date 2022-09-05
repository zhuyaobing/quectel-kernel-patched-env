/*
***************************************************************************************
***************************************************************************************
***
***     File: filename.ext
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

   "$Id: LookupCallbackTable.h 1299 2014-04-02 14:01:50Z STONERIDGE\jryd $"

   \{
***************************************************************************************
*/
#ifndef LOOKUPCALLBACKTABLE_H_
#define LOOKUPCALLBACKTABLE_H_


#include <map>
#include <stdint.h>

namespace freja
{

class Entry;

class LookupCallbackTable
{
public:
   LookupCallbackTable();
   virtual ~LookupCallbackTable();

   Entry * find(uint32_t key);
   void add(uint32_t key, Entry *lookup_entry_ptr);
   void remove(uint32_t key);

   std::map<uint32_t, Entry * > m_lookup_table;
};

} /* namespace freja */

#endif /* LOOKUPCALLBACKTABLE_H_ */
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
