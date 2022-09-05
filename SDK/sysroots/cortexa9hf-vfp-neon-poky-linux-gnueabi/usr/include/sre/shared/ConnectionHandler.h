/*
***************************************************************************************
***************************************************************************************
***
***     File: ConnectionHandler.h
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

    \Subject    Linux API tcp server definition.

    \Reference

    \Abstract



    \Version Id:


    "$Id: ConnectionHandler.h 1299 2014-04-02 14:01:50Z STONERIDGE\jryd $"


@{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef CONNECTION_HANDLER_H
#define CONNECTION_HANDLER_H

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/

#include "shared/MessageQueue.h"
#include "shared/Notifiable.h"
/*
**=====================================================================================
** type definitions
**=====================================================================================
*/

/**
 **************************************************************************************
 **
 **   @brief Base class to handle connections and messages.
 **
 **************************************************************************************
 **/
class ConnectionHandler : public freja::Notifiable
{
public:
   ConnectionHandler(freja::Notifiable* owner_ptr, bool thread_safe);

   virtual void shutDown();

   freja::MessageQueueReadOnly& getIncommingMessageQueue();

   freja::MessageQueueWriteOnly& getOutgoingMessageQueue();

protected:
   //! This queue holds the incomming messages from peers
   freja::MessageQueue m_in_queue;

   //! This queue holds the outgoing messages
   freja::MessageQueue m_out_queue;
};

#endif  // TCP_CONNECTION_HANDLER

/*************************************************************************************/
/** @}
*********************************** End of file **************************************/
