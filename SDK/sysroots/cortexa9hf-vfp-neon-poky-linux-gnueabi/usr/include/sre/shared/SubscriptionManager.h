/*
***************************************************************************************
***************************************************************************************
***
***     File: SubscriptionManager.h
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


    "$Id: SubscriptionManager.h 1318 2014-04-04 13:12:29Z STONERIDGE\dahi $"


@{
***************************************************************************************
*/

/*
**=====================================================================================
**  This program uses C++ STL and Boost libraries
**
**  Boost is used under the the Boost software license, see
**  http://www.boost.org/LICENSE_1_0.txt
**  This file is heavily based on
**  http://www.boost.org/doc/libs/1_51_0/doc/html/boost_asio/example/fork/daemon.cpp
**  Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
**=====================================================================================
*/

#ifndef SUBSCRIPTIONMANAGER_H_
#define SUBSCRIPTIONMANAGER_H_

#include <stdint.h>


namespace freja
{
class TlvMessage;

class SubscriptionManager {
public:
   virtual ~SubscriptionManager();

   SubscriptionManager();

   void handleAllIncomingSubscriptionMessages(TlvMessage* in_msg_ptr);

};

} /* namespace freja */

#endif /* SUBSCRIPTIONMANAGER_H_ */
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
