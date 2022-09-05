#ifndef MOCK_VARIABLESCLIENT_H
#define MOCK_VARIABLESCLIENT_H

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ecu/variables.h>

#include "mock_transportclient.h"

using ::testing::NiceMock;

namespace ecu {
namespace lapi {
namespace var {

class MockVariablesClient : public VariablesClient
{
   public:
      MockVariablesClient() :
         VariablesClient(com::ITransportClient_ptr(
            new NiceMock<MockTransportClient>())) { }

      MOCK_METHOD3(get, const std::string&(
         const std::string& variable,
         const std::string& default_value, int timeout));

      MOCK_METHOD4(get_extended, const std::string&(uint64_t& timestamp_ms,
         const std::string& variable,
         const std::string& default_value,
         int timeout));

};

}}} // ecu::lapi::var namespace

#endif //VARIABLESCLIENT_H