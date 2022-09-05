/*
 * mock_transportclient.h
 *
 *  Created on: Sep 20, 2016
 *      Author: lefr, Lennart Franzén
 */

#ifndef MOCK_TRANSPORTCLIENT_H_
#define MOCK_TRANSPORTCLIENT_H_

#include <vector>
#include <map>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ecu/com/client.h>
#include <ecu/logging.h>
#include <ecu/com/message.h>

namespace ecu {
namespace lapi {

class MockTransportClient: public com::ITransportClient
{
   public:
      MOCK_METHOD0(connect, bool());
      MOCK_METHOD0(disconnect, void());
      MOCK_METHOD0(isConnected, bool());
      MOCK_METHOD0(is_ready, bool());
      MOCK_METHOD1(wait_ready, bool(const std::chrono::seconds));
      MOCK_METHOD2(publish, int(const std::string&, const com::Message&));
      MOCK_METHOD3(publish_wait_ack, int(const std::string&, const com::Message&, const std::chrono::seconds));
      MOCK_METHOD3(subscribe, void(const std::string&, uint8_t, com::ISubscriptionObserver_ptr));
      MOCK_METHOD2(unsubscribe, void(const std::string&, com::ISubscriptionObserver_ptr));
      MOCK_METHOD1(add_connection_observer, void(com::IConnectionObserver_ptr));
      MOCK_METHOD1(run, void(bool));
      MOCK_METHOD0(start, void());
      MOCK_METHOD0(stop, void());
      MOCK_METHOD0(join, void());
      MOCK_CONST_METHOD0(client_name, const std::string&());
      MOCK_METHOD2(set_will, bool(const std::string&, const com::Message&));
      MOCK_METHOD0(clear_will, bool());
};

class MockSubOnlyTransportClient: public com::ITransportClient
{
   public:
      MOCK_METHOD0(connect, bool());
      MOCK_METHOD0(disconnect, void());
      MOCK_METHOD0(isConnected, bool());
      MOCK_METHOD0(is_ready, bool());
      MOCK_METHOD1(wait_ready, bool(const std::chrono::seconds));
      MOCK_METHOD2(publish, int(const std::string&, const com::Message&));
      MOCK_METHOD3(publish_wait_ack, int(const std::string&, const com::Message&, const std::chrono::seconds));
      MOCK_METHOD2(unsubscribe, void(const std::string&, com::ISubscriptionObserver_ptr));
      MOCK_METHOD1(add_connection_observer, void(com::IConnectionObserver_ptr));
      MOCK_METHOD1(run, void(bool));
      MOCK_METHOD0(start, void());
      MOCK_METHOD0(stop, void());
      MOCK_METHOD0(join, void());
      MOCK_CONST_METHOD0(client_name, const std::string&());
      MOCK_METHOD2(set_will, bool(const std::string&, const com::Message&));
      MOCK_METHOD0(clear_will, bool());

      void subscribe(const std::string& topic, uint8_t /*qos*/,
            com::ISubscriptionObserver_ptr observer)
      {
         LOG(DEBUG) << "MockTransportClient: Add MQTT subscriber for topic '" << topic;
         auto& v = m_subscriptions[topic];
         if ( std::find(v.begin(), v.end(), observer) == v.end() ) {
            LOG(DEBUG) << "Observer will be added for topic: " << topic;
            v.push_back(observer);
         } else {
            LOG(DEBUG) << "Observer already registered for " << topic
               << ", will NOT be added.";
         }
      }

      void message_arrived(const std::string& topic, const com::Message& message)
      {
         LOG(DEBUG) << "MockTransportClient: Message arrived on topic '" << topic;
         auto it = m_subscriptions.find(topic);
         if (it != m_subscriptions.end())
            for (auto& sub : it->second) {
               sub->message(topic, message);
            }
      }

   protected:
      std::map<std::string, std::vector<com::ISubscriptionObserver_ptr>>  m_subscriptions;
};

} /* namespace lapi */
} /* namespace ecu */

#endif /* MOCK_TRANSPORTCLIENT_H_ */
