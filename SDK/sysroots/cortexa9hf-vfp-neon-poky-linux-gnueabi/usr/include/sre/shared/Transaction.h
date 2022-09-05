/*
***************************************************************************************
***************************************************************************************
***
***     File: Transaction.h
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


    "$Id: Transaction.h 1348 2014-04-30 10:16:41Z STONERIDGE\dahi $"


@{
***************************************************************************************
*/
#ifndef TRANSACTION_H_
#define TRANSACTION_H_


#include <stdint.h>

namespace freja
{
class TlvMessage;
class TransactionFunctor;

class Transaction
{
public:
   Transaction();
   Transaction(uint32_t transaction_number, uint32_t timestamp,
               TransactionFunctor *functor_ptr, TlvMessage *incoming_message_ptr,
               uint32_t function_status = 0);
   virtual ~Transaction();

   TransactionFunctor* getFunctorPtr();
   void setFunctorPtr(TransactionFunctor* functorPtr);
   TlvMessage* getIncomingMessagePtr();
   void setIncomingMessagePtr(TlvMessage* incomingMessagePtr);
   uint32_t getTimestamp() const;
   void setTimestamp(uint32_t timestamp);
   uint32_t getTransactionNumber() const;
   void setTransactionNumber(uint32_t transactionNumber);

   uint32_t getFunctionStatus() const;
   void setFunctionStatus(uint32_t functionStatus);
   uint32_t& getFunctionStatusRef();

private:
   uint32_t m_transaction_number;
   uint32_t m_timestamp;
   TransactionFunctor *m_functor_ptr;
   TlvMessage *m_incoming_message_ptr;

   uint32_t m_function_status;
   // Used as a holder of the function status (e.g. error code). The TransactionFunctor
   // sends it to the TransactionManager when calling notify "done".
   // This is necessary when we have functors that do not contain state. Otherwise all
   // functors would need to contain m_function_status as a state.
   // ????????????????????????????????? or as notify ?
};

} /* namespace freja */

#endif /* TRANSACTION_H_ */
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
