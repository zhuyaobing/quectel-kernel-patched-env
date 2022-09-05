/*
 * mock_maintenacerequestclient.h
 *
 *  Created on: Apr 18, 2017
 *      Author: matu
 */

#ifndef ALEX_CSLC_LAPI_MOCKS_MOCK_MAINTENANCEREQUESTCLIENT_H_
#define ALEX_CSLC_LAPI_MOCKS_MOCK_MAINTENANCEREQUESTCLIENT_H_

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ecu/diag/main_req.h>

namespace ecu {
namespace lapi {
namespace diag {

class MockMaintenanceRequestClient : public MaintenanceRequestClient
{
public:
	MockMaintenanceRequestClient(com::ITransportClient_ptr& tclient) : MaintenanceRequestClient(tclient) {}
	MOCK_METHOD2(request, MaintenanceRequestClient::RequestResult(const Request&, unsigned));

};

} /* namespace diag */
} /* namespace lapi */
} /* namespace ecu */

#endif /* ALEX_CSLC_LAPI_MOCKS_MOCK_MAINTENANCEREQUESTCLIENT_H_ */
