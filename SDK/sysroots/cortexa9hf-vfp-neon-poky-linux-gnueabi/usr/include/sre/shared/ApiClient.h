/*
***************************************************************************************
***************************************************************************************
***
***     File: ApiClient.h
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

    \Subject    Linux API client class definition.

    \Reference

    \Abstract



    \Version Id:


    "$Id: ApiClient.h 1330 2014-04-22 13:07:36Z STONERIDGE\PSM $"


@{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef APICLASS_H_
#define APICLASS_H_

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/
#include "ApiRequestData.h"

#include "client/sre_api.h"
#include "client/sre_con_base_priv.h"
#include "shared/Notifiable.h"
#include "shared/sre_client_base.h"
#include "shared/tcp_ConnectionHandler.h"

#include <stdint.h>
#include <pthread.h>
#include <map>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
/*
**=====================================================================================
** type definitions
**=====================================================================================
*/


/*
**=====================================================================================
** Forward declarations, we are only using pointers to the below types
**=====================================================================================
*/

class Message;
namespace freja
{
class TlvMessage;
class MessageQueue;
class MessageQueueReadOnly;
class MessageQueueWriteOnly;
}

namespace sre
{
class SubjectBase;
class EventBase;
} // Namespace sre

/**
 *************************************************************************************
 **
 **  @brief Class to implement client base service for api cal:s.
 **
 **************************************************************************************
 **/
class ApiClient : public freja::Notifiable
{
public:
   ApiClient(freja::MessageQueue* in_queue_ptr = NULL, freja::MessageQueue* out_queue_ptr = NULL, uint16_t api_my_address = MAN_UNKNOWN_CLIENT);
   virtual ~ApiClient();


   sre::Status init(uint16_t client_address, const char *host_ptr, const char *port_ptr, boost::asio::io_service *io_service_ptr, void (*threadFunc)());

   bool isInitialised();

   sre::Status connect(sre::SubjectBase *subject_ptr, sre::EventBase *event_ptr, int time_out);

   sre::Status close();

   //! Insert message to be sent here.
   //! transport_tag will not be needed when the new message header is fully implemented.
   sre::Status createApiCall(freja::TlvMessage* message_ptr, int time_out, uint16_t transport_tag,
                             sre::SubjectBase *subject_ptr, sre::EventBase *event_ptr = NULL);

   freja::TlvMessage* getNotifyMessage();

   void setInQueue(freja::MessageQueue* in_queue_ptr);

   void setInQueue(freja::MessageQueueReadOnly *in_queue_ptr);

   void setOutQueue(freja::MessageQueueWriteOnly *out_queue_ptr);

   void setOutQueue(freja::MessageQueue* out_queue_ptr);

   void setAddress(uint16_t api_my_address);

   void handleTimeout(tlv_TagType tag);

   sre::Status serviceMessage(Message* message_ptr);


private:

   sre::Status writeCopyOfMessage(freja::TlvMessage* message_ptr);

   sre::Status writeMessage(freja::TlvMessage* message_ptr);

   bool findTag(tlv_TagType tag);

   sre::Status initFunctionCall(tlv_TagType tag, sre::SubjectBase *subject_ptr, sre::EventBase *event_ptr,
                                int time_out, bool subscription = false);

   ApiRequestData *findRequestData(tlv_TagType tag);

   sre::Status unsubscribeToFunction(tlv_TagType tag);

   virtual void notify(int32_t param_1, int32_t param_2);

   void errorCheck(int status, int line, const char *message);

   std::map<tlv_TagType, ApiRequestData* > m_api_request_map;

   pthread_mutex_t m_api_mutex;

   freja::MessageQueueReadOnly* m_in_queue_ptr;
   freja::MessageQueueWriteOnly* m_out_queue_ptr;

   uint16_t m_api_my_address;

   freja::TlvMessage* m_notify_message_ptr;

   tcp_ConnectionHandler *m_connection_handler_ptr;
   bool m_api_initialized;

   boost::asio::io_service *m_io_service_ptr;
   boost::thread *m_api_thread_ptr;
};


#endif  // APICLASS_H_

/*************************************************************************************/
/** @}
*********************************** End of file **************************************/
