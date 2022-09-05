/*
 * mock_bootflags.h
 *
 *  Created on: Jan 24, 2019
 *      Author: lefr
 */

#ifndef MOCK_BOOTFLAGS_H_
#define MOCK_BOOTFLAGS_H_

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ecu/bootflags.h>

class MockBootFlagsClient : public ecu::lapi::platform::BootFlagsClient
{
   public:
      MockBootFlagsClient(
            ecu::lapi::com::ITransportClient_ptr& tclient, int timeout_ms) :
               ecu::lapi::platform::BootFlagsClient(tclient, timeout_ms) {}

      MOCK_METHOD0(current_bank, BootBankResult(void));
      MOCK_METHOD0(selected_bank, BootBankResult(void));
      MOCK_METHOD0(boot_counter, BootCounterResult(void));
      MOCK_METHOD1(boot_bank_valid, BootBankValidResult(BootBank));
      MOCK_METHOD0(bank_switch_cause, BankSwitchCauseResult(void));
      MOCK_METHOD0(boot_mode, BootModeResult(void));
      MOCK_METHOD0(update_timestamp, uint64_t(void));

      MOCK_METHOD2(set_boot_bank, SetResult(BootBank, BankSwitchCause));
      MOCK_METHOD2(set_boot_bank_valid, SetResult(BootBank, bool));
      MOCK_METHOD1(switch_bank,  SetResult(BankSwitchCause));

      MOCK_METHOD1(boot_flags_string, std::string(bool));
};

#endif MOCK_BOOTFLAGS_H_
