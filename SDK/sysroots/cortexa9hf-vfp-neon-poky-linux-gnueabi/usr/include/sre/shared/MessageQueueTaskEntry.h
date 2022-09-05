/*
***************************************************************************************
***************************************************************************************
***
***     File: MessageQueueTaskEntry.h
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


    "$Id: MessageQueueTaskEntry.h 1299 2014-04-02 14:01:50Z STONERIDGE\jryd $"


@{
***************************************************************************************
*/

#ifndef MESSAGE_QUEUE_TASK_ENTRY_H_
#define MESSAGE_QUEUE_TASK_ENTRY_H_

/*
**=====================================================================================
** Imported definitions, local
**=====================================================================================
*/
#include "shared/TaskEntry.h"

/*
**=====================================================================================
** Imported definitions, external
**=====================================================================================
*/
#include <stdint.h>


namespace freja
{
/*
**=====================================================================================
** Forward declarations
**=====================================================================================
*/
class MessageQueue;

/**
 *************************************************************************************
 **
 **   @brief Task class to connect message queues
 **
 **
 **************************************************************************************
 **/
class MessageQueueTaskEntry : public TaskEntry
{
public:
   enum {
      HANDLE_MESSAGE_QUEUE_REGISTRATION = 98,
      HANDLE_MESSAGE_QUEUE_DEREGISTRATION
   };
   MessageQueueTaskEntry(EntryHandler *entry_handler_ptr, MessageQueue* owners_in_queue);
   MessageQueueTaskEntry();
   virtual ~MessageQueueTaskEntry();

   void registerOwnersIncommingQueue(MessageQueue* read_queue_ptr);
   void registerOthersIncommingQueue(MessageQueue* read_queue_ptr);
   MessageQueue* getOwnersIncommingQueue();
   MessageQueue* getOthersIncommingQueue();
private:

   //! This is the read queue of the owner of the MessageQueueTaskEntry
   MessageQueue* m_owners_read_queue_ptr;

   //! This is the read queue of the other end of the MessageQueueTaskEntry
   /*!
     I.e the one to which this object is sent to be filled in.
   */
   MessageQueue* m_others_read_queue_ptr;
};

} /* namespace freja */

#endif /* MESSAGE_QUEUE_TASK_ENTRY_H_ */
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
