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


    "$Id: MessageQueueWriteOnly.h 1299 2014-04-02 14:01:50Z STONERIDGE\jryd $"


@{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef MESSAGE_QUEUE_WRITE_ONLY_H_
#define MESSAGE_QUEUE_WRITE_ONLY_H_

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/
#include "NotificationQueue.h"
#include "shared/Message.h"

/*
**=====================================================================================
** type definitions
**=====================================================================================
*/
namespace freja
{
/**
 *************************************************************************************
 **
 **   @brief This class is an interface, that defines the read end of the MessageQueue.
 **
 **************************************************************************************
 **/
class MessageQueueWriteOnly : public NotificationQueueWriteOnly<Message*>
{
public:
   MessageQueueWriteOnly(NotificationQueue<Message*>& write_queue) :
      NotificationQueueWriteOnly<Message*>(write_queue)
   {}
};

} /* namespace freja */
#endif  // MESSAGE_QUEUE_WRITE_ONLY_H_
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
