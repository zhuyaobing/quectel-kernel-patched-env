/*
***************************************************************************************
***************************************************************************************
***
***     File: MessageQueue.h
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

    \Subject    Linux API message queue definition.

    \Reference  TBD

    \Abstract



    \Version Id:


    "$Id: MessageQueue.h 1299 2014-04-02 14:01:50Z STONERIDGE\jryd $"


@{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef MESSAGE_QUEUE_H_
#define MESSAGE_QUEUE_H_

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/
#include "MessageQueueReadOnly.h"
#include "MessageQueueWriteOnly.h"
#include "NotificationQueue.h"
#include "NotificationQueueReadOnly.h"
#include "NotificationQueueWriteOnly.h"
#include <stdint.h>

/*
**=====================================================================================
** type definitions
**=====================================================================================
*/

class Message;

namespace freja
{
class Notifiable;

/**
 *************************************************************************************
 **
 **   @brief This class holds a queue that mat have multiple writers, but only one reader.
 **          It may optionally be constructed with tread safe enqueue and dequeue.
 **
 **************************************************************************************
 **/
class MessageQueue : public NotificationQueue<Message*>
{
public:
   MessageQueue(bool use_locking) :
      NotificationQueue<Message*>(use_locking)
   {}

   MessageQueueReadOnly& getNotificatitionQueueReadOnly()
   {
      NotificationQueueReadOnly<Message*>& ref = NotificationQueue<Message*>::getNotificatitionQueueReadOnly();
      return static_cast<MessageQueueReadOnly&>(ref);
   }


   MessageQueueWriteOnly& getNotificatitionQueueWriteOnly()
   {
      NotificationQueueWriteOnly<Message*>& ref = NotificationQueue<Message*>::getNotificatitionQueueWriteOnly();
      return static_cast<MessageQueueWriteOnly&>(ref);
   }


};

} /* namespace freja */
#endif  // MESSAGE_QUEUE_H_
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
