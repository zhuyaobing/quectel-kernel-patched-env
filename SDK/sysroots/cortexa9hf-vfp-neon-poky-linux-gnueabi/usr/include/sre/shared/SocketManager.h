/*
***************************************************************************************
***************************************************************************************
***
***     File: SocketManager.h
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


    "$Id: SocketManager.h 1321 2014-04-16 08:36:28Z STONERIDGE\jryd $"


@{
***************************************************************************************
*/

/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/
#ifndef SOCKETMANAGER_H_
#define SOCKETMANAGER_H_

/*
**=====================================================================================
** Imported definitions, local
**=====================================================================================
*/
#include "shared/EntryHandler.h"
#include "shared/TaskEntry.h"
#include "shared/Notifiable.h"
#include "shared/BytestuffingFilter.h"
#include "shared/TaskQueue.h"
#include "shared/Runnable.h"

/*
**=====================================================================================
** Imported definitions, external
**=====================================================================================
*/


namespace freja
{
/*
**=====================================================================================
** Forward declarations
**=====================================================================================
*/
class SocketManagerImpl;
class SocketServerTask;

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
class SocketManager : public EntryHandler, public Notifiable, public Runnable
{
public:
   enum
   {
      HANDLE_NEW_CONNECTION = 58,
      HANDLE_WRITE_REQUEST
   };
   enum {
      INCOMING_SELECTOR_QUEUE_ID = 422,
   };
   SocketManager();
   virtual ~SocketManager();

   virtual uint32_t handleEntry(Entry *entry_ptr, void* extra_data_ptr, uint32_t extra_info);
   virtual void notify(int32_t param_1 = -1, int32_t param_2 = -1);
   virtual void run();

   bool initServer(int port_number);
   void registerMessageQueueHandler(EntryHandler* handler);

private:
   void handleMessageQueue();

   //! Implements the server
   SocketServerTask* m_server_ptr;

   //! Pipe that is used to signal that we want to run, i.e. pending writes
   int m_signal_pipe;

   //! Implementation
   SocketManagerImpl* priv;

   //! Connection to the client of the Socket manager, e.g. Dispatcher
   EntryHandler* m_message_queue_handler;

   //! Flag indicating if something has happened, we need to be run.
   bool m_do_run;
};

} // namespace freja

#endif /* SOCKETMANAGER_H_ */
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
