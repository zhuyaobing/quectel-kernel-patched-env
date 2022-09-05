/*
***************************************************************************************
***************************************************************************************
***
***     File: Router.h
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


    "$Id: Router.h 1361 2014-05-06 14:06:06Z STONERIDGE\dahi $"


@{
***************************************************************************************
*/

#ifndef ROUTER_H_
#define ROUTER_H_

#include "shared/MessageQueuePair.h"
#include "shared/MessageQueue.h"
#include "shared/TransactionManager.h"


class ApiClient;
namespace freja
{
class TlvMessage;
class SubscriptionManager;

/**
 **************************************************************************************
 **
 **   @brief Router. The Router is the "anchor point" of the manager.
 **
 **          As described in the overview, we are transferring messages from the client
 **          to dispatcher and then to the manager. e.g. request ReadSms to the
 **          ModemManager. The manager processes the message and send the reply back
 **          to the dispatcher and then to the client.\n
 **          The freja design has incoming and outgoing message queues to handle this.\n
 **
 **          The Router is where these message queues are attached. i.e. in the future
 **          we can easily have several different connections of different priority
 **          joining to the dispatcher, each with their own message queues.\n
 **          Also there can be cases where we have several different dispatchers. e.g.
 **          having an application which is network connected to 3 different physical
 **          devices (e.g. 3 freja boards) each having their own dispatcher.\n
 **          It is the Router that has lookup tables so that it knows where to route
 **          the external messages.\n
 **          Similarly an executable manager program may actually contain more than
 **          one manager. E.g. A vehicle lights program may contain 4 "small light
 **          managers" as follows : Front_Left_Light_Manager, Front_Right_Light_Manager,
 **          Rear_Left_Light_Lanager, Rear_Right_Light_Manager.\n
 **          The Freja api design needs to be able to support possibilities like this
 **          without needing a major redesign.\n
 **          It is the Router that has lookup tables so that it knows where to route
 **          the internal messages.\n
 **
 **          The lookup tables, internal and external routing of messages as well as
 **          dynamic function name to function tag lookup is done by the Router.
 **          Currently there is simple one to one mapping of the routing since there is
 **          only one connection or one manager. However this mapping goes through
 **          functions so that it can easily be implemented when needed.\n
 **          Also the src, dest, and func tags as well as the RouterInfo which contains
 **          the m_dispatcher_prefix of which connection the TlvMessage comes from are
 **          contained with the internal TlvMessage. This will allow lookup tables to
 **          resolve future routing problems.\n
 **          As stated, the lookup tables and routing have not been implemented, however
 **          all the data needed is sent to the "dummy" routing functions to route the
 **          messages. e.g.\n
 **          - preprocessIncomingExternalMessage()
 **          - routeIncomingMessage()
 **          - postprocessOutgoingMessage()
 **          - routeOutgoingMessage()\n
 **
 **          The TlvMessageQueues are thread aware, reentrant and light weight. i.e.
 **          Only pointers to the TlvMessage are stored in the queue.\n
 **          The TlvMessageQueues are the place where thread switching is easiest to
 **          implement.\n
 **
 **          The TransactionManager is 80% ready for dynamic name resolution.\n
 **
 **
 **************************************************************************************
 **/
class Router : public EntryHandler, public Notifiable
{
public:
   static Router* getInstance();
   virtual ~Router();

   void registerConnectionQueuePairs(MessageQueuePair &message_queue_pair);
   // There is normally just one queue pair.
   // i.e. The queue pair to the ConnectionHandler that has got the connection to the
   // dispatcher.
   // If there are two dispatchers (e.g. New and old dispatcher versions) then for
   // incoming messages it is no problem but for outgoing messages which "out queue"
   // should be used to send the reply ? Can be a problem if the same source uses
   // both dispatchers. Maybe TlvMessage has some internal routing info attached.
   //
   // If we have priority connections then there needs to be a way to know this.
   // i.e. So we can have low priority threads to handle low priority connections.
   // Also have problem of matching low priority messages to be replied back to which
   // queue / connection?

   void registerTransactionManagers(TransactionManager *trans_manager_ptr);


   void registerApiClient(ApiClient *apiClient_ptr);
   void registerSubscriptionManagers(SubscriptionManager *subscription_manager_ptr);
   ApiClient * getApiClient();


   uint32_t handleEntry(Entry *entry_ptr, void* extra_data_ptr, uint32_t extra_info);
   // To handle Router TaskEntries.

   void setRootPath(const std::string& pathName);  // See m_root_path
   const std::string& getRootPath() const;

   void setAddress(uint16_t address);  // e.g. MAN_MODEM;
   uint16_t getAddress() const;

   virtual void notify(int32_t param_1 = -1, int32_t param_2 = -1);
   void setWriteTaskQueuePtr(TaskQueue *writeTaskQueuePtr);


   /*
   **=====================================================================================
   ** Naming !!  read from right to left. Repeat read from right to left.
   ** e.g. m_outgoing_transaction_message_queue.
   ** It is a "queue". It is a "message" queue. It is a "transaction message" queue.
   ** It is an "outgoing transaction message" queue.
   **=====================================================================================
   */

   bool handleIncomingExternalMessage(uint32_t queue_Id);
   TlvMessage * preprocessIncomingExternalMessage(TlvMessage *in_msg_ptr);  // Returns preprocessed msg
   bool routeIncomingMessage(TlvMessage *in_msg_ptr);


   void handleIncomingFunctionCallMessage(TlvMessage *in_msg_ptr);
   void handleIncomingFunctionReturnMessage(TlvMessage *in_msg_ptr);
   void handleIncomingConnectTagMessage(TlvMessage *in_msg_ptr);
   void handleAllIncomingSubscriptionMessages(TlvMessage *in_msg_ptr);


   bool handleOutgoingTransactionMessage();
   bool handleOutgoingInternalMessage();
   bool handleOutgoingSubscriptionMessage();

   bool postprocessOutgoingMessage(TlvMessage *out_msg_ptr);
   bool routeOutgoingMessage(TlvMessage *out_msg_ptr);


   bool writeToOutgoingInternalMessage(TlvMessage *out_msg_ptr);

   uint16_t getNextSequenceNumber();
   uint16_t getOrigin();
   MessageQueueWriteOnly *getOutgoingInternalMessageQueuePtr();

private:
   enum {
      INCOMING_EXTERNAL_MESSAGE_QUEUE_0_ID = 0,
      INCOMING_EXTERNAL_MESSAGE_QUEUE_LAST_ID = INCOMING_EXTERNAL_MESSAGE_QUEUE_0_ID,

      OUTGOING_TRANSACTION_MESSAGE_QUEUE_ID = 1000,
      OUTGOING_INTERNAL_MESSAGE_QUEUE_ID = 1001,
      OUTGOING_SUBSCRIPTION_MESSAGE_QUEUE_ID = 1002
   };
   // used in param_1 in notify() and TaskEntry

   enum {
      HANDLE_INCOMING_EXTERNAL_MESSAGE = 1,
      HANDLE_OUTGOING_TRANSACTION_MESSAGE = 2,
      HANDLE_OUTGOING_INTERNAL_MESSAGE = 3,
      HANDLE_OUTGOING_SUBSCRIPTION_MESSAGE = 4
   };
   // used in param_2 in notify() and TaskEntry

   Router();
   static Router* m_instance;
   std::string m_root_path;
   // This is the name that is registered with the dispatcher. i.e. The dispatcher
   // knows which manager to call when a client asks for a function that has an
   // object_path that starts with m_root_path. e.g. similar to ".com" or ".se"
   // The m_root_path must be unique. e.g. "ModemManagerXYZ" or "/ModemManagerXYZ"  ???
   // see TransactionFunctorFactory::createObjectPathInterfaceFuncName() partly from
   // Dbus spec ???
   uint16_t m_origin;   // e.g. the address MAN_MODEM;
   volatile uint16_t m_last_sequence_number;

   MessageQueuePair m_external_message_queue_pair;  // external_message queue pair is the
                                                    // connections to the dispatchers.
                                                    // Just one for now.


   TaskQueue* m_write_task_queue_ptr;            // For writing Router TaskEntries.

   TransactionManager *m_trans_manager_ptr;     // Just one for now.
                                                // Different object paths will
                                                // mean different TransactionManagers.
                                                // The TransactionManager holds the
                                                // "object path" name.


   /*
   **=====================================================================================
   ** The three queues below have been created instead of just having just one
   ** "outgoing message" queue.
   ** The reason being that the different queues can be given different priorities.
   ** e.g. m_outgoing_subscription_message_queue might have message that need to be sent
   ** say every 5 millisec. In which case you do not want these messages queued behind
   ** a slow transaction reply sending say 10 Meg of data.
   **=====================================================================================
   */

   MessageQueue m_outgoing_transaction_message_queue;
   // This is owned by the router.
   // The ReadOnly interface is given as m_read_reply_message_queue_ptr.
   // The WriteOnly interface is given to the TransactionManager when they register with
   // the router.

   MessageQueueReadOnly* m_read_outgoing_transaction_message_queue_ptr;
   // This is normally the queue that the TransactionManager puts its return message into.


   MessageQueue m_outgoing_internal_message_queue;
   // This is owned by the router.
   // The ReadOnly interface is given as m_read_outgoing_internal_message_queue_ptr.

   MessageQueueReadOnly* m_read_outgoing_internal_message_queue_ptr;
   // This is normally the queue that the "connect" message puts its reply in.
   MessageQueueWriteOnly* m_write_outgoing_internal_message_queue_ptr;
   // This is used by writeToOutgoingInternalMessage() so that
   // the "connect" message can puts its reply in.
   // Also if calling from the API.


   MessageQueue m_outgoing_subscription_message_queue;
   // This is owned by the router.
   // The ReadOnly interface is given as m_read_outgoing_subscription_message_queue_ptr.

   MessageQueueReadOnly* m_read_outgoing_subscription_message_queue_ptr;
   // This is normally the queue that the subscription Manager puts its message in.


   ApiClient *m_apiClient_ptr;
   SubscriptionManager *m_subscription_manager_ptr;

   pthread_mutex_t m_mutex;
};

} /* namespace freja */

#endif /* ROUTER_H_ */
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
