/*
 * mock_connectionobserver.h
 *
 *  Created on: Sep 20, 2016
 *      Author: lefr, Lennart Franzén
 */

#ifndef MOCK_CONNECTIONOBSERVER_H_
#define MOCK_CONNECTIONOBSERVER_H_

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ecu/com/observer.h>

class MockIConnectionObserver: public ecu::lapi::com::IConnectionObserver
{
   public:
      MOCK_METHOD1(on_connect, void(int));
      MOCK_METHOD1(on_disconnect, void(int));
      MOCK_METHOD2(on_log, void(int, const std::string&));
      MOCK_METHOD0(on_error, void ());
};

#endif /* MOCK_CONNECTIONOBSERVER_H_ */
