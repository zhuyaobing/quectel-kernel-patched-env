/*
***************************************************************************************
***************************************************************************************
***
***     File: tcp_ConnectionHandler.h
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

    \Subject    Linux API tcp connection handler definition.

    \Reference

    \Abstract



    \Version Id:


    "$Id: tcp_ConnectionHandler.h 1334 2014-04-23 10:27:05Z STONERIDGE\PSM $"


@{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef TCP_CONNECTION_HANDLER_H_
#define TCP_CONNECTION_HANDLER_H_

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/
#include "shared/ConnectionHandler.h"
#include "shared/tcp_Connection.h"
#include "shared/sre_client_base.h"

/*
**=====================================================================================
** type definitions
**=====================================================================================
*/

class Notifiable;

/**
 **************************************************************************************
 **
 **   @brief Class to handle tcp/ip connections and ioservices.
 **          This class implements general io service and connection handling.
 **
 **          It is configured with specific functionality for when it is used in
 **          the dispatcher, iomanager, client etc. using the m_pManager to an
 **          object which is a subclass of tlv_MessageHandler.
 **
 **************************************************************************************
 **/
class tcp_ConnectionHandler : public ConnectionHandler
{
public:

   tcp_ConnectionHandler(const std::string &host,
                         const std::string &port,
                         Notifiable* owner_ptr,
                         boost::asio::io_service& io_service);

   virtual ~tcp_ConnectionHandler();

   void startAccept();

   void startConnect();

   void schedulePeriodic(int time);

   void postService();

   //! TODO: This needs to be reworked, one should not expose one's private parts.
   boost::asio::io_service& getIoService();

   virtual void shutDown();

   //! Implements Notifiable
   virtual void notify(int32_t param_1, int32_t param_2);


private:
   void scheduleIn(int time);

   void executeTimerService();

   void executeService();

   void handleAccept(tcp_Connection *new_connection_ptr,
                     const boost::system::error_code& error);

   void checkForDuplicateConnection(unsigned int index);

   boost::asio::io_service& m_io_service;
   std::vector<tcp_Connection*> m_connections;

   boost::asio::ip::tcp::acceptor *m_acceptor_ptr;

   boost::asio::deadline_timer m_timer;

   std::string m_host;
   std::string m_port;

   //! Strand to ensure the connection's handlers are not called concurrently.
   boost::asio::io_service::strand m_strand;

   int m_timer_period;
   bool m_running_server_mode;
};


#endif  // TCP_CONNECTION_HANDLER_H_

/*************************************************************************************/
/** @}
*********************************** End of file **************************************/
