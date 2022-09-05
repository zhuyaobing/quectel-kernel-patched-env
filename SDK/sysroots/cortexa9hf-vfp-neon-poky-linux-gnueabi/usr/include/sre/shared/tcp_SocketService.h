/*
***************************************************************************************
***************************************************************************************
***
***     File: tcp_SocketService.h
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

   \Subject    Linux API Encapsulates tcp socket service.

   \Reference  TBD

   \Abstract



   \Version Id:


   "$Id: tcp_SocketService.h 1277 2014-03-19 16:41:58Z STONERIDGE\PSM $"


@{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef TCP_SOCKET_SERVICE_H_
#define TCP_SOCKET_SERVICE_H_

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/
#include "shared/sre_client_base.h"
#include <boost/asio.hpp>


/*
**=====================================================================================
** type definitions
**=====================================================================================
*/
class tcp_ConnectionHandler;
class tcp_Connection;

/**
 *************************************************************************************
 **
 **   tcp_SocketService
 **   @brief Encapsulates the boost::asio socket handling.
 **
 **************************************************************************************
 **/
class tcp_SocketService
{
public:
   tcp_SocketService(boost::asio::io_service& io_service, const std::string &host, const std::string &port);

   virtual ~tcp_SocketService();

   virtual void setConnection(tcp_Connection *connection_ptr);

   virtual void asyncReadSome(boost::array<uint8_t, MAX_SIZE> &read_buffer);
   virtual void handleRead(const boost::system::error_code& err, std::size_t n);

   virtual void asyncWrite(std::vector<uint8_t> &write_data);
   virtual void handleWrite(const boost::system::error_code& err, size_t n);
   virtual void writeNext(tcp_ConnectionHandler *connection_handler_ptr);

   virtual void asyncResolve();
   virtual void handleResolv(const boost::system::error_code &err, boost::asio::ip::tcp::resolver::iterator ix);
   virtual void handleConnect(const boost::system::error_code &err);

   boost::asio::ip::tcp::socket& getSocket();

   const std::string& getHost() const;
   const std::string& getPort() const;
private:
   boost::asio::ip::tcp::socket m_socket;

   std::string m_host;
   std::string m_port;

   boost::asio::ip::tcp::resolver m_resolver;

   boost::asio::io_service::strand m_strand;

   tcp_Connection *m_connection_ptr;
};

#endif  // TCP_SOCKET_SERVICE_H_
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
