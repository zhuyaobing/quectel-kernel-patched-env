/*
***************************************************************************************
***************************************************************************************
***
***     File: tcp_Connection.h
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

   \Subject    Linux API tcp connection definition.

   \Reference

   \Abstract



   \Version Id:


   "$Id: tcp_Connection.h 1334 2014-04-23 10:27:05Z STONERIDGE\PSM $"


@{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef TCP_CONNECTION_H_
#define TCP_CONNECTION_H_

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/
#include "shared/sre_client_base.h"
#include "shared/Stack.h"
#include "shared/MessageQueue.h"

#include <boost/asio.hpp>

class tcp_SocketService;
class tcp_ConnectionHandler;


/*
**=====================================================================================
** type definitions
**=====================================================================================
*/


/**
 *************************************************************************************
 **
 ** tcp_Connection
 **
 ** @brief This class encapsulates the socket service and queues needed to make a connection point
 **        between two of the binaries in the API, i.e. the client and dispatcher, manager
 **        and dispatcher etc.
 **
 **        It contains input and output queues for raw, unprocessed data, and tlv_Message in
 **        and out queues.
 **
 **        Messages in the tlv_Message in queue are fetched and processed by the tlv_MessageHandler,
 **        and messages that the handler put in the out queue are transmitted to the other end
 **        of the connection.
 **
 **************************************************************************************
 **/
class tcp_Connection
{
public:

   tcp_Connection(tlv_MessageHandlerAddressType destination,
                  tcp_ConnectionHandler *connection_handler_ptr,
                  tcp_SocketService *socket_service_ptr);

   //! Get reference to socket.
   boost::asio::ip::tcp::socket& socket();

   /**
    *************************************************************************************
    **
    **   @brief Public interface to initiate read, write and connect on socket.
    **
    **************************************************************************************
    **/

   void startRead();

   void writeNext();

   void writeNextFromQueue();

   void startConnect();

   void insertOutMessage(freja::TlvMessage* message_ptr);


   /**
    *************************************************************************************
    **
    **   @brief io service handler callbacks corresponding to read, write and connect.
    **
    **************************************************************************************
    **/
   void handleRead(sre::Status status, std::size_t n);

   void handleWrite(sre::Status status, size_t n);

   void resolvError();

   void handleConnect(sre::Status status);

   bool isDisconnected() const;

   void setAccepted(bool accepted);

   bool isAccepted() const;

   const char* stateAsString() const;

   tlv_MessageHandlerAddressType getDestination() const;

   void setDestination(tlv_MessageHandlerAddressType destination);

   ~tcp_Connection();

   //!    @remark Private constructor.
   //!            Use the create method to create a new connection pointer.
private:

   tcp_SocketService *m_socket_service_ptr;

   boost::array<uint8_t, MAX_SIZE> m_read_buffer;

   tcp_ConnectionHandler *m_connection_handler_ptr;

   Stack m_stack;


   //!    tcp_Connection - in and out queues is public interface.
public:
   freja::MessageQueue m_in_queue;
   freja::MessageQueue m_out_queue;
private:
   tlv_MessageHandlerAddressType m_destination;

   enum
   {
      INIT,
      DISCONNECTED,
      RESOLV,
      CONNECTED
   } m_state;

   bool m_accept;

};


#endif  // TCP_CONNECTION_H_
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
