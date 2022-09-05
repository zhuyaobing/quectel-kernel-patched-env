/*
***************************************************************************************
***************************************************************************************
***
***     File: TaskEntry.h
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

    \Subject    Linux API daemon implementation.

    \Reference  TBD

    \Abstract




    \Version Id:


    "$Id: TaskEntry.h 1318 2014-04-04 13:12:29Z STONERIDGE\dahi $"


@{
***************************************************************************************
*/

#ifndef TASK_ENTRY_H_
#define TASK_ENTRY_H_

#include "shared/Entry.h"

#include <stdint.h>
#include <stdio.h>


namespace freja
{

class EntryHandler;
class TaskQueue;

/**
 *************************************************************************************
 **
 **   @brief TaskEntry contains a pointer to an EntryHandler.\n
 **   The EntryHandler means we have a handler that handles Entries.\n
 **   See Entry.h for details.
 **
 **   If the Entry is being used in a lookupTable then the extra_info can be
 **   the lookup key.
 **   The lookup key can be used for debugging to see where the call originated from.
 **   The lookup_key should not be part of LookupEntry since the same lookupEntry could
 **   be used with several keys.
 **
 **   The default extra_info = 0xFFFFFFFF and not 0 is that often extra_info is used as an enum
 **   value which normally starts at 0.
 **
 **   Note handleEntry() actually calls
 **   m_entry_handler_ptr->handleEntry(this, extra_data_ptr, extra_info);
 **   i.e. A pointer to itself (this) is sent in the call.
 **   This means the m_entry_handler_ptr->handleEntry() gets access to
 **   m_user_data1 and m_user_data2.
 **
 **************************************************************************************
 **/
class TaskEntry : public Entry
{
public:
   TaskEntry(EntryHandler *entry_handler_ptr, uint32_t user_data1 = 0, uint32_t user_data2 = 0);
   TaskEntry();
   virtual ~TaskEntry();

   virtual uint32_t handleEntry(void *extra_data_ptr = NULL, uint32_t extra_info = 0xFFFFFFFF);

   uint32_t getUserData1() const;
   void setUserData1(uint32_t user_data1);
   uint32_t getUserData2() const;
   void setUserData2(uint32_t user_data2);
   void setEntryHandlerPtr(EntryHandler * entry_handler_ptr);


   TaskQueue* getTaskQueuePtr();
   void setTaskQueuePtr(TaskQueue* taskQueuePtr);

private:
   //! Secondary handler of the entry
   EntryHandler *m_entry_handler_ptr;

   //! User data1 which is passed on the secondary handler, through a "this" pointer.
   uint32_t m_user_data1;

   //! User data2 which is passed on the secondary handler, through a "this" pointer.
   uint32_t m_user_data2;

   // Note void *m_user_data_ptr; has not been added.
   // This is because it can be misused by pointing to a temp object.
   // e.g. the Entry gets put into a queue and then the temp object disappears.
   // alternatively is that the temp object never gets deleted.

   TaskQueue *m_task_queue_ptr;
   // By having the m_task_queue_ptr with the TaskEntry allows a new Task to be put
   // into the same TaskQueue. This is useful when different TaskQueues can have different
   // priority threads. It allows the different TaskQueues to run the same "task" but put
   // the next "task" into the correct TaskQueue.
};

} /* namespace freja */

#endif /* TASK_ENTRY_H_ */
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
