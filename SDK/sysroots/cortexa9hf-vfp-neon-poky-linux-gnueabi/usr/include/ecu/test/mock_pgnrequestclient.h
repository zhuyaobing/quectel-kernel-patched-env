/*
 * mock_pgnrequestclient.h
 *
 */

#ifndef MOCK_PGNREQUESTCLIENT_H_
#define MOCK_PGNREQUESTCLIENT_H_

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ecu/diag/pgn.h>
#include <ecu/com/client.h>

#include <chrono>
#include <string>

namespace ecu {
namespace lapi {
namespace diag {

class MockPgnRequestClient: public diag::PgnRequestClient
{
public:
   MOCK_METHOD2(get_request_token,
         diag::PgnRequestClient::TokenResult(
            const std::string&, std::chrono::seconds));
   MOCK_METHOD1(release_token, void(const std::string&));
};

} /* namespace diag */
} /* namespace lapi */
} /* namespace ecu */

#endif /* MOCK_PGNREQUESTCLIENT_H_ */
