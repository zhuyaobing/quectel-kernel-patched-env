/*
 * mock_systemd.h
 *
 *  Created on: Nov 30, 2017
 *      Author: anw, Anders Wildros
 *
 *  systemd mock used for unit-testing purposes
 *
 *  Use by linking against libcslcipcmock instead of libcslcipc
 *  (in the CMake file)
 */
#ifndef MOCK_SYSTEMD_H_
#define MOCK_SYSTEMD_H_
//Include testing
#include <gtest/gtest.h>
#include <gmock/gmock.h>
//Include real symbols
#include <ecu/systemd.h>
//Include standard library
#include <vector>
#include <string>
//Define mock functions for systemd's entire public interface
class MockSystemd: public ecu::lapi::ipc::Systemd{
    public:
        MockSystemd(){}
        virtual ~MockSystemd(){}

        // ----------------------------------------------------------------------
        MOCK_METHOD1(start_isolate_target,          bool(const std::string&));
        MOCK_METHOD1(change_target,                 bool(const std::string&));
        MOCK_METHOD1(stop_target_services,          void(const std::string&));
        MOCK_METHOD1(start_target_services,         void(const std::string&));
        MOCK_METHOD1(is_target_active,              bool(const std::string&));
        MOCK_METHOD2(list_unit_dependencies,        std::vector<std::string>(const std::string&,const std::string&));
        MOCK_METHOD1(list_target_services,          std::vector<std::string>(const std::string&));
        MOCK_METHOD1(are_target_services_stopped,   bool(const std::string&));
        MOCK_METHOD1(get_unit_activestate,          ecu::lapi::ipc::Systemd::ActiveState(const std::string&));
        MOCK_METHOD1(get_unit_installstate,         ecu::lapi::ipc::Systemd::InstallState(const std::string&));
        MOCK_METHOD1(start_unit,                    bool(const std::string&));
        MOCK_METHOD1(stop_unit,                     bool(const std::string&));
        MOCK_METHOD1(restart_unit,                  bool(const std::string&));
        MOCK_METHOD1(reload_unit,                   bool(const std::string&));
        MOCK_METHOD3(start_stop_unit,               bool(const std::string&,const ecu::lapi::ipc::Systemd::UnitAction,const std::string&));
        MOCK_METHOD4(get_object_instance,           std::string(const std::string&,const std::string&,const std::string&,const std::string&));
        MOCK_METHOD4(get_property_value,            std::string(const std::string&,const std::string&,const std::string&,const std::string&));
        MOCK_METHOD4(get_property_value_array,      std::vector<std::string>(const std::string&,const std::string&,const std::string&,const std::string&));
        // ----------------------------------------------------------------------
};
#endif /* MOCK_SYSTEMD_H_ */
