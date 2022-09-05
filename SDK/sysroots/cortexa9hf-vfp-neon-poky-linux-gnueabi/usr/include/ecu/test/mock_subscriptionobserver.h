/*
 * mock_connectionobserver.h
 *
 *  Created on: Sep 20, 2016
 *      Author: lefr, Lennart Franzén
 */

#ifndef MOCK_SUBSCRIPTIONOBSERVER_H_
#define MOCK_SUBSCRIPTIONOBSERVER_H_

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ecu/com/observer.h>
#include <ecu/com/message.h>

namespace ecu {
namespace lapi {

class MockISubscriptionObserver: public com::ISubscriptionObserver
{
   public:
      MOCK_METHOD2(message, void(const std::string&, const com::Message&));
};

} /* namespace lapi */
} /* namespace ecu */

#endif /* MOCK_SUBSCRIPTIONOBSERVER_H_ */
