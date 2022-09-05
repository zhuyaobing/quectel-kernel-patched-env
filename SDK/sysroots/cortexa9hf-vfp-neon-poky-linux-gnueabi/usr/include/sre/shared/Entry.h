/*
***************************************************************************************
***************************************************************************************
***
***     File: Entry.h
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


    "$Id: Entry.h 1348 2014-04-30 10:16:41Z STONERIDGE\dahi $"


@{
***************************************************************************************
*/

#ifndef ENTRY_H_
#define ENTRY_H_

#include <stdint.h>
namespace freja
{

/**
 *************************************************************************************
 **
 **   @brief Entry is used as the base for LookupEntry, TaskEntry, QueueEntry,
 **          CallbackEntry, LookupCallbackEntry.
 **
 **          It is the basis of the "Command Pattern".
 **          e.g. A working thread can be looking after a Queue containing
 **          "QueueEntries" or it may lookup in a LookupTable for a "LookupEntry".
 **          Anyway the thread gets an "Entry". It could be a pointer to a subclass of
 **          Entry or an actual derived object from Entry.
 **          (Note a pointer to a subclass of Entry is also a pointer to Entry.)
 **
 **          All the working thread has to do is call the handleEntry() function
 **          inside the Entry. Nothing more, nothing less.
 **          (Similar idea as a callback in c)
 **
 **          This is very nice.
 **          It means that the working thread ONLY needs to know about its queue or
 **          its lookupTable and the class Entry. Nothing more, nothing less.
 **
 **          The "Entry" is a self contained little packet.
 **
 **          How does it work ?
 **          Polymorphism, the derived classes (sub classes) of Entry override the
 **          handleEntry() function. This means that the derived class's handleEntry()
 **          gets called instead of Entry's handleEntry().
 **          The difference to a 'c' callback is that we are calling back to a
 **          function in an object. i.e. The object has its own data that we can use.
 **
 **
 **          All very good but now you think that there is going to be an explosion
 **          of subclasses of Entry in order to get your project running.
 **
 **          Luckily there are some tricks:-
 **
 **          The main idea is to have a small entity class derived from "Entry" which
 **          contains a pointer to another class. This small entity class overrides
 **          handleEntry() to call a function in the pointed to class.
 **          Its like a two stage rocket.
 **          The small entity class can also contain some extra data that it sends to
 **          the pointed class. In fact the easiest way is to send a pointer of
 **          itself ("this") in the call to the function in the pointed to class.
 **
 **          This is done in the following two classes.
 **          "TaskEntry" and "EntryHandler".
 **               "TaskEntry" is the small entity class derived from "Entry".
 **               "EntryHandler" is the pointed to class. It has a function called
 **               handleEntry()
 **
 **         e.g. The code looks like
 **         uint32_t EntryHandler::handleEntry(Entry *entry_ptr,
 **                             void *extra_data_ptr = NULL, uint32_t extra_info = 0);
 **
 **         uint32_t TaskEntry::handleEntry(void* extra_data_ptr, uint32_t extra_info)
 **         {
 **            uint32_t ret = 0;
 **            if (m_entry_handler_ptr != NULL){
 **               ret = m_entry_handler_ptr->handleEntry(this, extra_data_ptr, extra_info);
 **            }
 **            return ret;
 **         }
 **
 **         Note the "this" in the call.
 **         TaskEntry has
 **            uint32_t m_user_data1;
 **            uint32_t m_user_data2;
 **         which means that the EntryHandler gets access to them.
 **         e.g. In TransactionManager::handleEntry()
 **            sre::TaskEntry *task_ptr = dynamic_cast< sre::TaskEntry *>(entry_ptr);
 **            trans_number = task_ptr->getUserData1();
 **
 **
 **
 **          The "TaskEntry" and "EntryHandler" classes gives an idea of what
 **          can be done. Its like a two stage rocket.
 **          The TaskEntry class takes a pointer to EntryHandler in its
 **          constructor.
 **          The TaskEntry is derived from Entry and overrides handleEntry().
 **          The TaskEntry::handleEntry() calls EntryHandler::handleEntry().
 **          i.e. uint32_t EntryHandler::handleEntry(Entry *entry_ptr,
 **                             void *extra_data_ptr = NULL, uint32_t extra_info = 0);
 **
 **          Another usage is callbacks to member functions from a lookupTable.
 **          This is used in the ReadSmsFunctor class with callbacks to the
 **          ReadSmsFunctor.
 **
 **************************************************************************************
 **/
class Entry
{
public:
   virtual ~Entry();

   virtual uint32_t handleEntry(void *extra_data_ptr, uint32_t extra_info) = 0;
   // If the Entry is being used in a lookupTable then the extra_info can be
   // the lookup key.
   // The lookup key can be used for debugging to see where the call originated from.
   // The lookup_key should not be part of LookupEntry since the same lookupEntry could
   // be used with several keys.


   // These helper functions below are mainly for use with queues.
   // They can be completely ignored in your own application if you do not want to use
   // them.

   // The queues we use are pointers to Entry objects.
   // Normally when an entry is put in a lookup table or an array then the same piece of
   // code is used to remove it from the lookup table or array.
   // However for queues often we have several different pieces of code put entries
   // into the queue and we have one generic piece of code to read from the queue and
   // execute the virtual function handleEntry(). The generic piece of code then deletes
   // the entry.
   // For the task queue for example there are the same entries passing through the queue
   // time and time again.
   // e.g. Timer Poll, handle incoming message.
   // This means there is a lot of creating and deleting identical entries all the time.
   // Instead a single instance could be used repeatedly in the queue if it wasn't deleted.
   // This can be accomplished by calling setToBeDeleted(false) and having the generic
   // piece of code that deletes the entry check isToBeDeleted() before deleting the Entry;

   // This means that the some entries get deleted after they come out from the queue and
   // other entries don't get deleted.

   // An added feature is that the Entries that DO NOT get deleted can have their function
   // setInUse(false) called. This can be used by a memory pool handler to see which objects
   // are free for reuse. A function isInUse(); is provided for this.

   virtual bool isToBeDeleted() const;
   // If isToBeDeleted() == false then the queue manager should not
   // try to delete the entry.

   virtual bool isInUse() const;
   // This can be used by a memory pool handler to see which objects are free for reuse.

   void setToBeDeleted(bool to_be_deleted);
   void setInUse(bool in_use);

protected:
   Entry();

private:
   bool m_to_be_deleted;
   bool m_in_use;
};

} /* namespace freja */

#endif /* ENTRY_H_ */
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
