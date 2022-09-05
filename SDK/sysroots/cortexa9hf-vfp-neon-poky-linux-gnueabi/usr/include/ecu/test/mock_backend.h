/*
 * mock_backend.h
 *
 *  Created on: Apr 11, 2017
 *      Author: lefr, Lennart Franzén
 */

#ifndef MOCK_BACKEND_H_
#define MOCK_BACKEND_H_

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ecu/com/backend.h>

class MockBackendClient : public ecu::lapi::com::BackendClient
{
   public:
      MockBackendClient(ecu::lapi::com::ITransportClient_ptr& tclient) :
         ecu::lapi::com::BackendClient(tclient) {}

      MOCK_METHOD0(is_connected, bool(void));
      MOCK_METHOD0(queue_status, QueueStatus(void));
      MOCK_METHOD1(add_connection_callback, void(ConnectionStateCallback_t));
      MOCK_METHOD1(add_queue_status_callback, void(QueueStatusCallback_t));
      MOCK_METHOD2(add_content_callback, void(const uint32_t, ContentCallback_t));
      MOCK_METHOD1(clear_content_callbacks, void(const uint32_t));
      MOCK_METHOD3(publish, uint64_t(const ecu::lapi::com::Message&, const uint32_t, const TxOptions&));
};


#endif /* MOCK_BACKEND_H_ */
