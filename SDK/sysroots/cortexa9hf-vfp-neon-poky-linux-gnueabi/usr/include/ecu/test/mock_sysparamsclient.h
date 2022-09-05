#ifndef MOCK_SYSPARAMCLIENT_H
#define MOCK_SYSPARAMCLIENT_H

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ecu/sysparams.h>

#include "mock_transportclient.h"

using ::testing::NiceMock;

namespace ecu {
namespace lapi {
namespace var {

class MockSysParamsClient : public SysParamsClient
{
   public:
      MockSysParamsClient() :
         SysParamsClient(com::ITransportClient_ptr(
            new NiceMock<MockTransportClient>())) { }

      MOCK_METHOD0(fetch, void());
      MOCK_METHOD0(write, WriteResult());

      MOCK_METHOD1(get_all, ParamMap(bool force_fetch));
      MOCK_METHOD1(set_all, void(const ParamMap& parameter_map));
      MOCK_METHOD1(merge, void(const ParamMap& parameter_map));
      MOCK_METHOD1(set_receive_timeout, void(int timeout));

      MOCK_METHOD2(get, GetResult(ParamKey key, bool force_fetch));

};

}}} // ecu::lapi::var namespace

#endif /* MOCK_SYSPARAMCLIENT_H */