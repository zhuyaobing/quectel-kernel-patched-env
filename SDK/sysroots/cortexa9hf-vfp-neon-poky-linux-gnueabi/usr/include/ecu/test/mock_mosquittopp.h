/*
 * mock_mosquittopp.h
 *
 *  Created on: Sep 20, 2016
 *      Author: lefr, Lennart Franzén
 */
#ifndef MOCK_MOSQUITTOPP_H_
#define MOCK_MOSQUITTOPP_H_

#include <map>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <mosquittopp.h>

class MockMosquittopp
{
   public:
      MockMosquittopp(const char *id)
      {
         m_id = id;
      }
      ~MockMosquittopp()
      {
         m_instances.erase(m_id);
      }

      // ----------------------------------------------------------------------
      MOCK_METHOD2(reinitialise, int(const char *, bool));
      MOCK_METHOD0(socket, int());
      MOCK_METHOD5(will_set, int(const char *, int, const void *, int, bool));
      MOCK_METHOD0(will_clear, int());
      MOCK_METHOD2(username_pw_set, int(const char *, const char *));
      MOCK_METHOD3(connect, int(const char *, int, int));
      MOCK_METHOD3(connect_async, int(const char*, int, int));
      MOCK_METHOD4(connect, int(const char *, int, int, const char *));
      MOCK_METHOD4(connect_async, int(const char *, int, int, const char *));
      MOCK_METHOD0(reconnect, int());
      MOCK_METHOD0(reconnect_async, int());
      MOCK_METHOD0(disconnect, int());
      MOCK_METHOD6(publish, int(int *, const char *, int , const void *, int , bool));
      MOCK_METHOD3(subscribe, int(int *, const char *, int ));
      MOCK_METHOD2(unsubscribe, int(int *, const char *));
      MOCK_METHOD3(reconnect_delay_set, void(unsigned int, unsigned int, bool));
      MOCK_METHOD1(max_inflight_messages_set, int(unsigned int));
      MOCK_METHOD1(message_retry_set, void(unsigned int));
      MOCK_METHOD1(user_data_set, void(void *));
      MOCK_METHOD5(tls_set, int(const char *, const char *, const char *, const char *, int (*)(char *, int , int , void *)));
      MOCK_METHOD3(tls_opts_set, int(int, const char *, const char *));
      MOCK_METHOD1(tls_insecure_set, int(bool));
      MOCK_METHOD3(tls_psk_set, int(const char *, const char *, const char *));
      MOCK_METHOD2(opts_set, int(enum mosq_opt_t, void *));
      MOCK_METHOD2(loop, int(int, int));
      MOCK_METHOD0(loop_misc, int());
      MOCK_METHOD1(loop_read, int(int));
      MOCK_METHOD1(loop_write, int(int));
      MOCK_METHOD2(loop_forever, int(int, int));
      MOCK_METHOD0(loop_start, int());
      MOCK_METHOD1(loop_stop, int(bool));
      MOCK_METHOD0(want_write, bool());
      MOCK_METHOD1(threaded_set, int(bool));
      MOCK_METHOD4(socks5_set, int(const char *, int, const char *, const char *));

      static MockMosquittopp* instance(const char *id)
      {
         if(m_instances.find(id) == m_instances.end()) {
            m_instances[id] = new MockMosquittopp(id);
         }
         return m_instances[id];
      }

   private:
      std::string m_id;
      static std::map<std::string, MockMosquittopp*> m_instances;
};

#endif /* MOCK_MOSQUITTOPP_H_ */
