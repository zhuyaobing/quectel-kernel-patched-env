/*
***************************************************************************************
***************************************************************************************
***
***     File: SocketClientTask.h
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

   \Subject      Linux API Socket server definition.

   \Reference

   \Abstract

   \Version

   "$Id: SocketClientTask.h 1299 2014-04-02 14:01:50Z STONERIDGE\jryd $"

   \{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/
#ifndef SOCKET_CLIENT_TASK_H_
#define SOCKET_CLIENT_TASK_H_


/*
**=====================================================================================
** Imported definitions, local
**=====================================================================================
*/
#include "shared/EntryHandler.h"
#include "shared/BytestuffingFilter.h"
#include "shared/MessageQueueTaskEntry.h"
#include "shared/MessageQueue.h"

/*
**=====================================================================================
** Imported definitions, external
**=====================================================================================
*/
#include <stdint.h>

/*
**=====================================================================================
** Forward declarations
**=====================================================================================
*/
class Notifiable;


namespace freja
{
/*
**=====================================================================================
** Forward declarations
**=====================================================================================
*/
class TaskEntry;
/**
 **************************************************************************************
 **
 **   @brief Encapsulates a socket.
 **
 **          Its interface is the message queue on one side and the selector client
 **          task on the other side.
 **
 **************************************************************************************
 **/
class SocketClientTask : public EntryHandler, public Notifiable
{
public:
   SocketClientTask(Notifiable* owner, int fd, bool thread_safe_message_queue,
                    uint32_t read_entry_id, int32_t write_request_id);
   virtual ~SocketClientTask();
   virtual uint32_t handleEntry(Entry *entry_ptr, void* extra_data_ptr, uint32_t extra_info);
   void notify(int32_t param_1, int32_t param_2 = -1);

   void  registerMessageQueueHandler(EntryHandler* handler);

   int getFd();

   void handleMessageQueue();

   bool isScheduledForRemoval();

   TaskEntry* getTaskEntry();
protected:
   // Nothing is protected in this class

private:
   //! File descriptor that is used for the server, opened for read
   int m_fd;

   //! Our owner that we need to inform, when we have produced a new client file descriptor
   Notifiable* m_owner_ptr;

   //! This is the bytestuffer used to parse messages
   BytestuffingFilter m_byte_stuffer;

   //! This is the interface towards the selector, to pass file descriptors etc
   TaskEntry m_task_entry;

   //! This is the interface towards the message queue, e.g. dispatcher
   freja::MessageQueueTaskEntry m_message_queue_entry;

   //! The read end of the message queue, i.e. the TlvMessages that we should send.
   MessageQueue m_read_message_queue;

   //! Flag telling that this connection should be removed
   bool m_scheduled_for_removal;

   //! The id with which the entry handler is called, when read is requested
   uint32_t m_read_entry_id;

   //! The id with which the entry handler is called, when write is requested
   int32_t m_write_request_id;

   //! The other end of the message queue.
   EntryHandler* m_message_queue_handler_ptr;
};

} // namespace freja
#endif // SOCKET_CLIENT_TASK_H_

/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
