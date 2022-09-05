/*
***************************************************************************************
***************************************************************************************
***
***     File: TransactionManager.h
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


    "$Id: TransactionManager.h 1361 2014-05-06 14:06:06Z STONERIDGE\dahi $"


@{
***************************************************************************************
*/
#ifndef TRANSACTIONMANAGER_H_
#define TRANSACTIONMANAGER_H_


#include "shared/Notifiable.h"
#include "shared/EntryHandler.h"

#include <map>
#include <deque>
#include <string>

/*
**=====================================================================================
** Forward declarations, we are only using pointers to the below types
**=====================================================================================
*/


namespace freja
{
class TlvMessage;
class TransactionFunctorFactory;
class TransactionFunctor;
class Transaction;
class TaskQueue;
class MessageQueueWriteOnly;

/**
 **************************************************************************************
 **
 **   @brief Transaction Manager.
 **
 **          Before we had Transaction Manager we need to see how information was
 **          fetched from the ModemManager.\n
 **          We use ReadSms as an example.\n
 **          With synchronous ReadSms we are given the Sms number to read.\n
 **          We make 4 different synchronous calls to the DBus ModemManger to read the
 **          sms text, sms phone number, smcs number, sms binary data. We store each
 **          reply on the stack until we have all the information. Then we package the
 **          info into a message, including the destination and and other message header
 **          info and put the message in the outgoing message queue.\n
 **
 **          To make the asynch ReadSms ????\n
 **          We are given the Sms number to read. We make the first asynch call to the
 **          DBus ModemManager to read the sms text. We are given a "sequence number" of
 **          this DBus call so we can somehow match the DBus reply with this DBus call.
 **          We return to do some other work.
 **          We get notified there is a DBus reply with our "sequence number".\n
 **
 **          Now what ??\n
 **
 **          Compared to the synchronous ReadSms we no longer know which
 **          Sms number to read, do not know where to store intermediate replies, do not
 **          know the destination to send the reply message to.
 **          i.e. This info needs to be stored when we start the first asynch call to the
 **          DBus ModemManager.\n
 **
 **          Keep in mind that we do not want to limit how the application developer
 **          designs their application. e.g. They could have done a listSms command and
 **          found that there were 5 Sms's to read. They could then send out 5 different
 **          asynch ReadSms commands and then just wait to get the 5 replies.\n
 **
 **          This is 2014 and we are designing a new platform so we do not want to design
 **          a slow system where everything is serialised. TP4 has suffered enough with
 **          this. e.g. Quick Test can take up to 20 minutes to perform, during which time
 **          TP4 does not reply to anything. i.e. TP4 appears to be dead.\n
 **
 **          WebServers have been around for 20 years and do not have this serialisation
 **          problem.\n
 **
 **          So for every new request we create an object to store the intermediate info
 **          as well as the destination and other message header info. Obviously for
 **          different requests there will be different objects. e.g ReadSms is different
 **          from listSms.\n
 **
 **          Functors are objects that can be treated as though they are a function.
 **          (We are not overloading the () operator.) Instead we are just using the name
 **          to mean that we are converting a function to an object.
 **          e.g. ReadSmsFunctor. This is a class where we can create objects of
 **          ReadSmsFunctor where we can store the intermediate info. E.g. We can store
 **          the m_sms_number to read, m_sms_text, m_sms_phone_number etc.\n
 **
 **          To get some order we can create a new Transaction for every new request.
 **          The Transaction has a m_transaction_number for internal use. It has a
 **          pointer to the incoming message and a pointer to a Functor. e.g. ReadSmsFunctor.\n
 **
 **          So the example of 5 different asynch ReadSms commands would mean we would create
 **          5 transactions each with their own pointer to their incoming Message as well
 **          as a pointer to their own ReadSmsFunctor Object.\n
 **
 **          How do we get the Functor Object ? e.g. ReadSmsFunctor.\n
 **
 **          TransactionManager::handleIncomingMessage() maps the message function tag
 **          and message dest tag to get a functor key. The functor key is used by the
 **          functor factory to create the correct functor.\n
 **
 **          How does it all work ?\n
 **
 **          class TransactionManager : public Notifiable, public EntryHandler
 **          i.e. TransactionManager is derived from both Notifiable and EntryHandler.
 **          The "glue" components Notifiable and EntryHandler as described in the design
 **          overview.\n
 **
 **          TransactionManager::handleIncomingMessage() maps the message function tag
 **          and message dest tag to get a functor key. The functor key is used by the
 **          functor factory to create the correct TransactionFunctor.
 **          A new transaction is created and stored in the transaction_table using the
 **          transaction number as the key.\n
 **          The TransactionFunctor::run() function is called.\n
 **          It takes transaction_number, pointer to Notifiable (in this case "this" the
 **          TransactionManager), pointer to incoming TlvMessage.\n
 **
 **          In ReadSmsFunctor a DBus asynch call is made to DBus ModemManager to get
 **          the sms text. The DBus sequence number and a callbackEntry to the
 **          ReadSmsFunctor are stored in SreDBusManager callbackTable.\n
 **
 **          When the DBus replies with an answer then the SreDBusManager callbackTable
 **          is used to "callback" to the correct ReadSmsFunctor. This process repeats
 **          until all the information is retrieved from the DBus ModemManager.
 **          At this point the ReadSmsFunctor has all the info it needs.\n
 **
 **          The ReadSmsFunctor should not need to know how and where to send the message reply.
 **          Instead it uses the "Notifiable::notify()" function to report that it is "done".
 **          This "Notifiable" is in fact the TransactionManager.
 **          Instead of using this thread to send the message, a taskEntry is made and
 **          put in the taskQueue.
 **          Later when the "working thread" handles the TaskQueue then
 **          TransactionManager::handleEntry() will be called which will call
 **          TransactionManager::handleDone(). This calls the
 **          TransactionFunctor::fillReplyMessageData(). i.e. the message is filled
 **          with the reply data. The message is then put in the outgoing message queue.
 **          The Transaction is then deleted which deletes the TransactionFunctor.\n
 **
 **          Special Features.\n
 **          If an error occurs in the TransactionFunctor then the TransactionFunctor must
 **          call the "Notifiable::notify()" function with its transaction Number and
 **          Status Code.\n
 **
 **          Sometimes an asynch call is not required and a synch call would be easier.
 **          e.g. Say we have a function to read our version number.
 **          In this case a synch call is simulated by calling "Notifiable::notify()" done
 **          immediately in the TransactionFunctor::run() function. The actual work of
 **          getting for example our version number is performed when the
 **          TransactionFunctor::fillReplyMessageData() is called.\n
 **
 **          Sometimes people say that having all these TransactionFunctor objects for
 **          each function is a lot of extra work when a single function could be made
 **          to handle a group of these functions.\n
 **          This can be true except the single function begins to get more complicated
 **          and often has switch statements or else ifs to handle the different cases.
 **          By having separate TransactionFunctor objects for each function means that
 **          a new function can be added WITHOUT modifying any of the existing tested
 **          TransactionFunctor objects. i.e. Unintentional side effects are minimised.
 **
 **************************************************************************************
 **/
class TransactionManager : public Notifiable, public EntryHandler
{
public:
   TransactionManager(TransactionFunctorFactory *functor_factory_ptr,
                      MessageQueueWriteOnly* out_message_queue_ptr,
                      TaskQueue* out_task_queue_ptr);

   virtual ~TransactionManager();


   //
   //=====================================================================================
   // Two partial blocking / rescheduling functions
   //=====================================================================================
   //
   bool handleIncomingMessage(TlvMessage* in_msg_ptr);  // calls functor::run()
   bool handleDone(uint32_t transaction_number, uint32_t status); // calls functor::fillReplyMessageData
   // If the TransactionFunctor that gets called by these two functions
   // is trying to access a resource that is already in use then it
   // can return true to indicate to the TransactionManager to try again in
   // say 200 millisec.
   // The TransactionManager sets the status to "runReschedule" or "doneReschedule"
   // in the Transaction.
   // The TransactionManager already has code to check for transaction timeout.
   // maybe return uint32_t as microsecs or nanosecs to delay. 0 means it is OK.
   // Sometimes want delay 0.2 millisec

   uint32_t getFunctorKey(uint16_t source_tag, uint16_t dest_tag, uint16_t func_tag);
   TransactionFunctor * getFunctor(uint32_t functor_key);

   uint32_t getNextTransactionNumber();


   void fixTempTranslationTable();  // Temp translation table between hard coded enum to function name.

   //
   //=====================================================================================
   // Callback functions
   //=====================================================================================
   //
   virtual void notify(int32_t param_1 = -1, int32_t param_2 = -1);
   // The notify() is called from a TransactionFunctor when it is "done".
   // i.e. Finished, either with a good answer or an error code.
   // For TransactionManager::notify() param_1 is the TransactionNumber
   // and param_2 is the function_status.
   // The TransactionFunctor e.g. ReadSmsFunctor could call notify() from
   // somewhere deep in its code. Now TransactionManager

   uint32_t handleEntry(Entry *entry_ptr, void* extra_data_ptr, uint32_t extra_info);
   // The handleEntry()
   // Used to call handleDone


   const std::string getObjectPath();
   void setOutMessageQueuePtr(MessageQueueWriteOnly *outMessageQueuePtr);

private:
   TransactionFunctorFactory *m_functor_factory;
   MessageQueueWriteOnly* m_out_message_queue_ptr;
   TaskQueue * m_out_task_queue_ptr;
   volatile uint32_t m_last_transaction_number;

   std::map<uint32_t, Transaction *> m_transaction_table;

   std::map<uint32_t, uint32_t> m_func_tag_to_functor_key_table;

   std::map<uint32_t, uint32_t> m_temp_translation_table;

   std::deque<uint32_t> m_done_queue;

   pthread_mutex_t m_mutex;
};

} // namespace freja

#endif /* TRANSACTIONMANAGER_H_ */
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
