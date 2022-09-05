#ifndef MOCK_UDSCLIENT_H
#define MOCK_UDSCLIENT_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <../src/uds_impl.h>

namespace ecu {
namespace lapi {
namespace diag {

class MockUdsClient : public UdsClientImpl
{
public:
   MockUdsClient(com::ITransportClient_ptr& tclient) : UdsClientImpl(tclient) {}

   MOCK_METHOD2(allocate_interface, UdsInterfaceState(
      const UdsInterfaceAllocationConfig&, uint32_t));
   MOCK_METHOD1(allocate_interface_async, bool(
      const UdsInterfaceAllocationConfig&));
   MOCK_METHOD1(release_interface, bool(uint32_t));
   MOCK_CONST_METHOD0(release_interface_async, bool(void));
   MOCK_METHOD1(withdraw_interface, bool(uint32_t));
   MOCK_CONST_METHOD0(withdraw_interface_async, bool(void));
   MOCK_METHOD4(send_request, UdsClient::RequestResult(
      const UdsMessage&, uint32_t, uint8_t, uint32_t));
   MOCK_METHOD4(send_request_async, UdsRequestStatus(
      const UdsMessage&, uint32_t, uint8_t, uint32_t));
   MOCK_METHOD4(send_flowctrl_async, UdsRequestStatus(
      uint8_t, uint8_t, uint16_t, FlowStatus));

private:
   /* data */
};

} /* namespace diag */
} /* namespace lapi */
} /* namespace ecu */

#endif /* MOCK_UDSCLIENT_H */
