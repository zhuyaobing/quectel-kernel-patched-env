/*
***************************************************************************************
***************************************************************************************
***
***     File: Selector.h
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


    "$Id: Selector.h 1299 2014-04-02 14:01:50Z STONERIDGE\jryd $"


@{
***************************************************************************************
*/

/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/
#ifndef SELECTOR_H_
#define SELECTOR_H_

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
#include <vector>
#include <map>
#include <sys/select.h>


namespace freja
{
class EntryHandler;
class Entry;
class Runnable;

/**
 **************************************************************************************
 **
 **   @brief Encapsulaties the posix select.
 **
 **
 **
 ** @startuml{SelectorSocketManagerDispatcherIncomingMessage.png}
 ** title Incoming message
 **
 ** participant Selector
 ** participant SelectorClientTask
 ** participant SocketManager
 ** participant TaskEntry
 ** participant MessageQueue
 **
 ** activate Selector
 ** activate SocketManager
 **
 ** -> Selector : run
 ** Selector -> Selector : pollClientUpdates()
 ** note right : This is to make the\n clients update what\n file descriptors to\n listen to.
 **
 ** activate Selector
 ** Selector -> SelectorClientTask
 ** activate SelectorClientTask
 **
 ** Selector -> SelectorClientTask: setUserData2(HANDLE_FD_REGISTRATION)
 ** Selector -> SocketManager : handleEntry(SelectorClientTask* )
 ** activate SocketManager
 ** SocketManager -> TaskEntry
 ** activate TaskEntry
 ** SocketManager -> SelectorClientTask : addToReadFd(fd, TaskEntry*)
 ** deactivate SocketManager
 ** deactivate Selector
 **
 ** Selector -> Selector : runTasks()
 ** activate Selector
 ** Selector -> SocketManager : run()
 ** activate SocketManager
 ** deactivate SocketManager
 ** deactivate Selector
 **
 ** Selector -> Selector : select()
 ** note right TaskEntry : handleEntry is called\n for the fd:s previously\n registered in addToReadFd
 ** Selector -> TaskEntry : handleEntry(HANDLE_READ)
 **
 ** TaskEntry -> TaskEntry :read(fd)
 ** alt A message was read
 ** TaskEntry -> MessageQueue : enqueue
 ** else Zero bytes were read
 ** TaskEntry -> TaskEntry : Schedule for destruction
 **
 ** deactivate Selector
 ** deactivate SelectorClientTask
 ** deactivate SocketManager
 ** deactivate TaskEntry
 ** end
 ** @enduml
 **
 **************************************************************************************
 **/
class Selector
{
public:
   Selector();
   virtual ~Selector();

   void registerRunnable(Runnable* runnable_ptr);
   void registerSelectClient(EntryHandler* select_client_ptr);
   void run();

private:
   void runTasks();
   void pollClientUpdates();

   //! The file descriptor set to watch for read
   fd_set m_read_fds;

   //! The file descriptor set to watch for write
   fd_set m_write_fds;

   //! The highest current file descriptor
   int m_high_fd;

   //! Pipe that is used to signal from clients, that they want to be run e.g. pending write operations
   int m_signal_pipe;

   //! This is a lookup table with file descriptors as key and enrties as value.
   std::map<int, Entry * > m_lookup_table;

   //! Registered runnables, which will be run when something happens on any of the file descriptors.
   std::vector<Runnable*> m_runnables;

   //! List of clients that are registered to receive events for their file descriptors.
   std::vector<EntryHandler*> m_select_clients;
};

} /* namespace freja */

#endif /* SELECTOR_H_ */
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
