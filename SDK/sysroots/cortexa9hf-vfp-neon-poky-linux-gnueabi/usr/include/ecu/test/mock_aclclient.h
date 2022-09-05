/*
 *******************************************************************************
 *******************************************************************************
 ***
 ***     File: mock_aclclient.h
 ***
 ***     Project: Alexandra
 ***
 ***     Copyright (C) Stoneridge Electronics AB 1990 - 2021
 ***
 ***
 *******************************************************************************
 *******************************************************************************
 */

#ifndef MOCK_ACLCLIENT_H
#define MOCK_ACLCLIENT_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ecu/com/acl.h>

namespace ecu {
namespace lapi {
namespace com {

class MockAclClient : public AclClient
{
public:
   MockAclClient(com::ITransportClient_ptr& tclient) : AclClient(tclient) {}

   MOCK_METHOD2(add_mqtt_user, AclResult(const std::string&, const std::string&));

   MOCK_METHOD4(add_mqtt_acl, AclResult(const std::string& ,
      const std::vector<std::string>&, const std::vector<std::string>&,
      const LapiFeatures&));

   MOCK_METHOD1(rm_mqtt_user, AclResult(const std::string&));

   MOCK_METHOD3(add_websocket_user, AclResult(const std::string&,
      const std::string&, uint32_t));

   MOCK_METHOD1(rm_websocket_user, AclResult(const std::string&));

   MOCK_METHOD2(add_rest_user, AclResult(const std::string&,
      const std::string&));

   MOCK_METHOD1(rm_rest_user, AclResult(const std::string&));

   MOCK_METHOD3(add_rest_acl, AclResult(const std::string&,
      const std::vector<std::string>&, const std::vector<std::string>&));

private:
   /* data */
};

} /* namespace com */
} /* namespace lapi */
} /* namespace ecu */

#endif /* MOCK_ACLCLIENT_H */

