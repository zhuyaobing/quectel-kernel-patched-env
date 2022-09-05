/*
***************************************************************************************
***************************************************************************************
***
***     File: MessageQueueReadOnly.h
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


    "$Id: MessageQueueReadOnly.h 1299 2014-04-02 14:01:50Z STONERIDGE\jryd $"


    @{
    ***************************************************************************************
    */


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef MESSAGE_QUEUE_READONLY_H_
#define MESSAGE_QUEUE_READONLY_H_

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/
#include "NotificationQueue.h"

#include <stdint.h>

/*
**=====================================================================================
** Forward declarations
**=====================================================================================
*/
class Message;

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
class MessageQueueReadOnly : public NotificationQueueReadOnly<Message*>
{
public:
   MessageQueueReadOnly(NotificationQueue<Message*>& read_queue) :
      NotificationQueueReadOnly<Message*>(read_queue)
   {}
};

} /* namespace freja */
#endif  // MESSAGE_QUEUE_READONLY_H_
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
