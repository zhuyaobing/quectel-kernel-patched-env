/*
***************************************************************************************
***************************************************************************************
***
***     File: TransactionFunctor.h
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


    "$Id: TransactionFunctor.h 1360 2014-05-06 13:09:14Z STONERIDGE\dahi $"


@{
***************************************************************************************
*/

#ifndef TRANSACTIONFUNCTOR_H_
#define TRANSACTIONFUNCTOR_H_

#include <stdint.h>
#include <string>

/*
**=====================================================================================
** Forward declarations, we are only using pointers to the below types
**=====================================================================================
*/
namespace freja
{
class TlvMessage;
class TlvMessageData;
class Notifiable;

/**
 **************************************************************************************
 **
 **   @brief Transaction functor
 **
 **************************************************************************************
 **/
class TransactionFunctor
{
public:
   TransactionFunctor();  // e.g. ModemManager
   virtual ~TransactionFunctor();


/**
 **************************************************************************************
 **
 **   @brief Determine if TransactionFunctor should be deleted.
 **
 **   If isToBeDeleted() == false then the transaction manager should not
 **   try to delete the TransactionFunctor when the transaction is finished.\n
 **   When a TransactionFunctor contains no state information that is specific
 **   for a particular transaction then there is no need to make duplicate
 **   TransactionFunctor objects for each new transaction that calls it.\n
 **   In this case the TransactionFunctor::create() function does not create a new
 **   TransactionFunctor object instead it just returns a pointer to itself.\n
 **   Due to this there is no m_transaction_number stored in the base
 **   TransactionFunctor. For the functors that create a new functor object
 **   for each transaction then they can have an m_transaction_number.
 **
 **   The functors that do not make duplicate TransactionFunctor objects for
 **   each new transaction can just call the notifyable_handler_ptr in their run()
 **   function that they are "done".\n The TransactionManager::handleDone()
 **   calls the TransactionFunctor::fillReplyMessageData() which in this case does
 **   the actual work of getting the data.\n
 **   e.g. getVersionNumberFunctor. The fillReplyMessageData() does the fetching
 **   of the version number.
 **
 **   @retval true if the TransactionFunctor should be deleted.
 **
 **************************************************************************************
 **/
   virtual bool isToBeDeleted();


/**
 **************************************************************************************
 **
 **   @brief creates a new TransactionFunctor.
 **
 **   See also isToBeDeleted().
 **   The create() implementation can use the normal constructor or the
 **   copy constructor.
 **   The copy constructor is useful when the functor contains pointers to other objects
 **   that you also need in the new functor object. However a warning is that
 **   you must fix the "this" pointer in the callback functions objects contained
 **   in the TransactionFunctor. Otherwise the callbacks will not return to the
 **   new TransactionFunctor object but instead return to the TransactionFunctor
 **   object that is in the TransactionFunctoryFactory. The correct member function
 **   is called but the member data is from the object in the TransactionFunctoryFactory.
 **
 **   e.g. in ReadSmsFunctor there is a member data called m_sms_number. It is set to
 **   0 in the constructor. When the TransactionFunctor::run() is called then
 **   m_sms_number is set to the requested sms number to read.
 **   If the copy constructor was used to make the new ReadSmsFunctor then on the
 **   callback from the DBusManager after reading the sms_phone_number it was noticed
 **   that the m_sms_number was 0. i.e. It was the m_sms_number of the TransactionFunctor
 **   in the TransactionFunctoryFactory.
 **   To fix this problem, either write your own copy constructor or write the "this"
 **   pointer into the functorCallback.
 **   E.g. in ReadSmsFunctor there is a functorCallback called "dbus_read_sms_phone_number_callback"
 **   Just do dbus_read_sms_phone_number_callback.m_this_ReadSmsFunctor_ptr = this;  before
 **   you add dbus_read_sms_phone_number_callback to the callback table.
 **
 **************************************************************************************
 **/
   virtual TransactionFunctor * create() = 0;


/**
 **************************************************************************************
 **
 **   @brief run performs the work of the TransactionFunctor.
 **   e.g. in ReadSmsFunctor run() sends off the request to the DBus ModemManager.
 **
 **   This is the asynchronous call to the functor.\n
 **   If a synchronous call is wanted then the run() function must immediately call
 **   notify "Done" using the transaction number.\n
 **   i.e. notifyable_handler_ptr->notify(transaction_number, OK);\n
 **   The transactionManager will later call this TransactionFunctor using
 **   fillReplyMessageData().\n It is in this call that the synchronous call is
 **   made.
 **
 **   @param transaction_number
 **   @param notifyable_handler_ptr Pointer to TransactionManager so we can notify
 **   when "Done". See TransactionManager::notify.
 **   @param in_msg_ptr pointer to TlvMessage. Should be changed to pointer to
 **          TlvData but TlvMessage was previously not broken up into two parts
 **          i.e. Header and Data.
 **          A TransactionFunctor SHOULD NOT need to know anything about dest, src,
 **          func_tag etc since this info could be changed in the future. Also it
 **          is Router that has the knowledge of dest, src, func_tag etc.
 **
 **   @retval true means you want to reschedule the same call in another say 200 millisec.
 **   Useful when a resource is busy or blocked.
 **
 **************************************************************************************
 **/
   virtual bool run(uint32_t transaction_number,
                    freja::Notifiable *notifyable_handler_ptr, TlvMessage *in_msg_ptr) = 0;
/*
 **   TlvMessage *msg_ptr is used instead of const TlvMessage *msg_ptr
 **   this allows run() to take ownership of the MessageData if it wants to.
 **   Usually it doesn't.
 **   run() should not take ownership of the TlvMessage since then the
 **   TransactionManager will not know where to send the reply message when
 **   TransactionManager::handleDone() is called.
*/


/**
 **************************************************************************************
 **
 **   @brief Fills the reply message.
 **
 **   @param in_msg_ptr is sent here as well as in the run() function.\n This allows
 **   the subclassed TransactionFunctor the choice of handling the message in the run()
 **   or in the fillReplyMessageData().\n i.e. run() is for asynch whereas fillReplyMessageData()
 **   is for synchronous().
 **
 **   @param &function_status is used by the TransactionManager to be put in the reply
 **   message status.\n
 **   The status arrives in this call as the value that was sent in the notify "done"
 **   message sent to the TransactionManager.\n As previously mentioned if the functor
 **   does not contain state info then this is the method for the function status to be
 **   saved in the Transaction class.\n
 **   A reference is used to function_status so that the cases where the actual work is
 **   performed in this function then an error code (function_status) could be returned.
 **
 **   @param out_message_data pointer to tlvData to put the out data.
 **
 **   @retval Return true means you want to reschedule the same call in another say
 **   200 millisec. Useful when a resource is busy or blocked.
 **
 **************************************************************************************
 **/
   virtual bool fillReplyMessageData(TlvMessageData &out_message_data, uint32_t &function_status,
                                     TlvMessage *in_msg_ptr) = 0;


/**
 **************************************************************************************
 **
 **   @brief cancels an ongoing transaction.
 **   Called from TransactionManager timeout functionality. (Not yet implemented)
 **
 **   Main purpose is to allow the Functor to clean up any callbacks before it is
 **   deleted.\n
 **   With multithreading a delay between calling cancel and actually deleting the
 **   functor can stop problems of callbacks calling a deleted object.
 **
 **   @note m_transaction_cancelled should be set.\n
 **   This way callback returns can check this to see if the transaction has not
 **   been cancelled.
 **
 **   @param transaction_number
 **
 **************************************************************************************
 **/
   virtual void cancel(uint32_t transaction_number);


/**
 **************************************************************************************
 **
 **   @brief gets the function name. NOTE not Functor name.
 **
 **   @retval function_name. NOTE not Functor name.\n
 **   e.g. "ReadSms" not "ReadSmsFunctor"
 **
 **************************************************************************************
 **/
   virtual const std::string getFunctionName() = 0;


/**
 **************************************************************************************
 **
 **   @brief gets the function version.
 **
 **   @retval function_version
 **
 **************************************************************************************
 **/
   virtual const std::string getFunctionVersion() = 0;
   virtual const std::string getInterfaceName();
private:
   std::string m_interface_name;
   bool m_transaction_cancelled; // Callback returns can check this to see if the transaction has not
   // been cancelled
};

} /* namespace freja */

#endif /* TRANSACTIONFUNCTOR_H_ */
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
