/*
***************************************************************************************
***************************************************************************************
***
***     File: SocketServerTask.h
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

   "$Id: SocketServerTask.h 1299 2014-04-02 14:01:50Z STONERIDGE\jryd $"

   \{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/
#ifndef SOCKET_SERVER_TASK_H_
#define SOCKET_SERVER_TASK_H_


/*
**=====================================================================================
** Imported definitions, local
**=====================================================================================
*/
#include "shared/EntryHandler.h"

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
class Notifiable;

/**
 **************************************************************************************
 **
 **   @brief Handles a server socket.
 **
 **************************************************************************************
 **/
class SocketServerTask : public EntryHandler
{
public:
   SocketServerTask(Notifiable* owner,
                    uint32_t new_connection_notification_id);
   virtual ~SocketServerTask();
   virtual uint32_t handleEntry(Entry *entry_ptr, void* extra_data_ptr, uint32_t extra_info);

   int createListenSocket(int port_number);
   int closeListenSocket();
   int getFd();
protected:
   // Nothing is protected in this class

private:
   int acceptNewConnection();

   //! File descriptor that is used for the server, opened for read
   int m_fd;

   //! Our owner that we need to inform, when we have produced a new client file descriptor
   Notifiable* m_owner_ptr;

   //! Port on which to listen
   int m_port;

   //! This is the id with which should inform our owner of a new connection
   uint32_t m_new_connection_notification_id;

};

} // namespace freja
#endif // SOCKET_SERVER_TASK_H_

/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
