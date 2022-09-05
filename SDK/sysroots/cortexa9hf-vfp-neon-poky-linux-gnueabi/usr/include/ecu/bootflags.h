/*
 *******************************************************************************
 *******************************************************************************
 ***
 ***     File: bootflags.h
 ***
 ***     Project: Alexandra
 ***
 ***     Copyright (C) Stoneridge Electronics AB 1990 - 2019
 ***
 *******************************************************************************
 *******************************************************************************
 ***
 ***
 ***   Subject:
 ***
 ***   Document reference:
 ***
 ***   Description of this header file:
 ***
 ***  Version:
 ***
 ***
 *******************************************************************************
 */

#ifndef BOOT_FLAGS_H_
#define BOOT_FLAGS_H_

#include <map>
#include <memory>
#include <condition_variable>

#include <ecu/com/client.h>
#include <ecu/com/message.h>
#include <ecu/com/observer.h>

#include <ecu/pb/bootflags.pb.h>
#include <ecu/common.h>
#include <ecu/partition.h>

namespace ecu {
namespace lapi {
namespace platform {

/**
 ****************************************************************************
 *
 *  @brief Boot flags Client API for getting and setting boot flags, and boot
 *         bank switch cause.
 *
 *  @details The boot flags are normally controlled by u-boot and CSP but a
 *           subset can also be set via this LAPI. If a malformed request is
 *           received, CSP may silently reject it resulting in a timeout.
 *
 *
 * Example usage:
 * @code
 *   // ITransportClient_ptr
 *   auto tclient = ecu::lapi::com::create_client(config)
 *
 *   // BootFlagsClient_ptr
 *   auto boot_flags_client = ecu::lapi::platform::create_boot_flags_client(tclient);
 *
 *   // Connect and start the internal Transport client
 *   tclient->connect();
 *   tclient->start();
 *
 *   // Get the selected boot bank (blocks until received or timeout)
 *   auto bank_res = boot_flags_client->selected_bank();
 *   if (bank_res.ok()) {
 *      LOG(INFO) << "Selected boot bank is: " <<
 *         BootFlagsClient::to_str(bank_res.val());
 *   }
 *
 *   // Set bank 1 as active and bank-switch-cause to preset LAPI2
 *   auto set_res = boot_flags_client( 1, BankSwitchCause::LAP2 );
 *   if (set_res != SetResult::SUCCESS) {
 *      LOG(ERROR) << "Failed setting boot flags, reason: " <<
 *         BootFlagsClient::to_str(set_res);
 *   }
 *
 * @endcode
 *
 *
 ***************************************************************************
 */
class BootFlagsClient
{
   public:
      /**
       ****************************************************************************
       * @brief Convenience typedef to a shared pointer type of this class
       ****************************************************************************
       */
      typedef std::shared_ptr<BootFlagsClient> ptr_t;

#ifndef DOXYGEN_IGNORE
      /**
       * Internal topic definitions, not to be used by application
       */
      static constexpr const char* TOPIC_PUB = "system/bootflags";
      static constexpr const char* TOPIC_SET = "system/bootflags/set";
#endif /* ifndef DOXYGEN_IGNORE */

      /**
       ****************************************************************************
       *
       *  @brief Constructor for the BootFlags Client
       *
       *  @details Needs to be initialized with an existing ITransportClient,
       *  therefore the default constructor is hidden
       *
       *  @param tclient An initialized internal transport client
       *  @param timeout_ms Timeout (ms) to use when waiting for response from base SW
       *
       ***************************************************************************
       */
      BootFlagsClient(com::ITransportClient_ptr tclient, int timeout_ms = 60000);
      virtual ~BootFlagsClient();

      /**
       ****************************************************************************
       *  @brief Definition of the Boot  bank type
       ***************************************************************************
       */
      enum struct BootBank
      {
         BANK_0           = 0,      ///< Boot bank 0
         BANK_1           = 1,      ///< Boot bank 1
         UNINITIALIZED    = 0xFF    ///< Bank not yet received
      };

      /**
       ****************************************************************************
       *  @brief Definition of the Boot mode type
       ***************************************************************************
       */
      enum struct BootMode
      {
         NORMAL           = 0,      ///< Normal application mode
         PRODUCTION_TEST  = 1,      ///< Production test mode
         UNINITIALIZED    = 0xFF    ///< Mode not yet received
      };

      /**
       ****************************************************************************
       *  @brief Definition of the Bank switch cause type
       ***************************************************************************
       */
      enum struct BankSwitchCause
      {
         NEVER_SWITCHED = 0,   ///< No boot bank switch has been carried out
         UBOOT_UNKNOWN,        ///< U-boot bank switch cause could not be determined
         UBOOT_POR,            ///< U-boot switched bank after a Power on reset
         UBOOT_WD,             ///< U-boot switched bank after a Watch dog reset
         UBOOT_WARM,           ///< U-boot switched bank after a Warm restart
         CSP_SW_UPDATE,        ///< CSP switched boot bank after a Software update
         LAPI,                 ///< LAPI user defined generic cause
         LAPI_0,               ///< LAPI user defined cause 0
         LAPI_1,               ///< LAPI user defined cause 1
         LAPI_2,               ///< LAPI user defined cause 2
         LAPI_3,               ///< LAPI user defined cause 3
         LAPI_4,               ///< LAPI user defined cause 4
         LAPI_5,               ///< LAPI user defined cause 5
         LAPI_6,               ///< LAPI user defined cause 6
         LAPI_7,               ///< LAPI user defined cause 7
         LAPI_8,               ///< LAPI user defined cause 8
         LAPI_9,               ///< LAPI user defined cause 9
         UNKNOWN,              ///< Bank switch cause is Unknown
         UNINITIALIZED  = 0xFF ///< Not yet received
      };

      /**
       ****************************************************************************
       *  @brief Definition of the Results type from set-functions
       ***************************************************************************
       */
      enum struct SetResult
      {
         SUCCESS = 0,               ///< OK: Boot flags set successfully
         INVALID_BANK,              ///< ERROR: Invalid bank specified
         INVALID_BANKSWITCH_CAUSE,  ///< ERROR: Invalid bank switch cause specified
         SERIALIZE_FAIL,            ///< ERROR: Failed to serialize MQTT message
         PUBLISH_FAIL,              ///< ERROR: Failed to publish MQTT message
         TIMEOUT_FAIL,              ///< ERROR: Timeout while waiting for response from CSP
         VERIFY_FAIL,               ///< ERROR: Requested parameters does not match actual parameters
         INVALID_PARTITION          ///< ERROR: Invalid partition permission to set boot_flags
      };

      /**
       ****************************************************************************
       *  @brief Boot bank result type
       ****************************************************************************
       */
      using BootBankResult        = Result<BootBank>;

      /**
       ****************************************************************************
       *  @brief Is boot bank valid result type
       ****************************************************************************
       */
      using BootBankValidResult   = Result<bool>;

      /**
       ****************************************************************************
       *  @brief Boot counter result type
       ****************************************************************************
       */
      using BootCounterResult     = Result<uint32_t>;

      /**
       ****************************************************************************
       *  @brief Bank switch cause result type
       ****************************************************************************
       */
      using BankSwitchCauseResult = Result<BankSwitchCause>;

      /**
       ****************************************************************************
       *  @brief Boot mode result type
       ****************************************************************************
       */
      using BootModeResult        = Result<BootMode>;

      /**
       ****************************************************************************
       *  @brief To string helper for displaying BootBank
       *
       *  @param bb BootBank to parse
       *
       *  @return const char pointer to string representation of the BootBank
       ****************************************************************************
       */
      static const char* to_str(const BootBank bb);

      /**
       ****************************************************************************
       *  @brief To string helper for displaying BootMode
       *
       *  @param bm BootMode to parse
       *
       *  @return const char pointer to string representation of the BootMode
       ****************************************************************************
       */
      static const char* to_str(const BootMode bm);

      /**
       ****************************************************************************
       *  @brief To string helper for displaying BankSwitchCause
       *
       *  @param bsc BankSwitchCause to parse
       *
       *  @return const char pointer to string representation of the BankSwitchCause
       ****************************************************************************
       */
      static const char* to_str(const BankSwitchCause bsc);

      /**
       ****************************************************************************
       *  @brief To string helper for displaying SetResult
       *
       *  @param sr SetResult to parse
       *
       *  @return const char pointer to string representation of the SetResult
       ****************************************************************************
       */
      static const char* to_str(const SetResult sr);

      /**
       ****************************************************************************
       *  @brief  Get the currently running boot bank
       *
       *  @return Boot bank result
       ****************************************************************************
       */
      virtual BootBankResult current_bank();

      /**
       ****************************************************************************
       *  @brief  Get the selected boot bank
       *
       *  @return Boot bank result
       ****************************************************************************
       */
      virtual BootBankResult selected_bank();

      /**
       ****************************************************************************
       *  @brief  Get the boot counter value
       *
       *  @return Boot counter result
       ****************************************************************************
       */
      virtual BootCounterResult boot_counter();

      /**
       ****************************************************************************
       *  @brief  Check if the boot bank is marked valid or not
       *
       *  @param  bank Boot bank to check (valid: 0 or 1)
       *
       *  @return Boot bank valid result
       ****************************************************************************
       */
      virtual BootBankValidResult boot_bank_valid(BootBank bank);

      /**
       ****************************************************************************
       *  @brief  Get the cause of the last bank switch
       *
       *  @return Boot bank switch cause result
       ****************************************************************************
       */
      virtual BankSwitchCauseResult bank_switch_cause();

      /**
       ****************************************************************************
       *  @brief  Get the current boot mode
       *
       *  @return Boot mode result
       ****************************************************************************
       */
      virtual BootModeResult boot_mode();

      /**
       ****************************************************************************
       *  @brief  Get time stamp of the last boot flags update from CSP
       *
       *  @return time stamp (time since boot (ms))
       ****************************************************************************
       */
      virtual uint64_t update_timestamp();


      /**
       ****************************************************************************
       *  @brief  Set boot bank
       *
       *  @details The boot counter will automatically be reset to 0 when setting
       *  a different boot bank than currently selected.
       *
       *  @note This function prevents its usage on the TPA partition, logging
       *  an error on the journal and returning an error (INVALID_PARTITION).
       *
       *  @param  bank Boot bank to set (valid: 0 or 1)
       *
       *  @param  switch_cause The cause of the bank switch as determined by the
       *          application. Valid reasons to set is LAPI..LAPI_9, default is LAPI.
       *          Other reasons can only be set by u-boot or CSP.
       *
       *  @return Set result type = SUCCESS if successful or error otherwise
       ****************************************************************************
       */
      virtual SetResult set_boot_bank(BootBank bank,
            BankSwitchCause switch_cause = BankSwitchCause::LAPI);

      /**
       ****************************************************************************
       *  @brief  Set boot bank valid or not
       *
       *  @note This function prevents its usage on the TPA partition, logging
       *  an error on the journal and returning an error (INVALID_PARTITION).
       *
       *  @param  bank Boot bank to set (valid: 0 or 1)
       *
       *  @param  valid Set bank as valid (true) or invalid (false)
       *
       *  @return Set result type = SUCCESS if successful or error otherwise
       ****************************************************************************
       */
      virtual SetResult set_boot_bank_valid(BootBank bank, bool valid);

      /**
       ****************************************************************************
       *  @brief  Perform bank switch
       *
       *  @details Perform a bank switch if there is another valid bank for booting.
       *  The boot counter will automatically be reset to 0.
       *
       *  @note This will update the active boot bank in the boot flags, but will NOT
       *  reboot the system, so a system reboot have to be triggered by other means.
       *  This function prevents its usage on the TPA partition, logging an
       *  error on the journal and returning an error (INVALID_PARTITION).
       *
       *  @param  switch_cause The cause of the bank switch as determined by the
       *          application. Valid reasons to set is LAPI..LAPI_9, default is LAPI.
       *          Other reasons can only be set by u-boot or CSP.
       *
       *  @return Set result type = SUCCESS if successful or error otherwise
       ****************************************************************************
       */
      virtual SetResult switch_bank(
            BankSwitchCause switch_cause = BankSwitchCause::LAPI);

      /**
       ****************************************************************************
       *  @brief  Get log / debug friendly boot flags string
       *
       *  @details Will show currently cached values without waiting for them to
       *  be received on MQTT
       *
       *  @param multiline Get multiline output instead of a one-liner (default false)
       *
       *  @return string representing boot flags
       ****************************************************************************
       */
      virtual std::string boot_flags_string(bool multiline = false);


      /**
       * @brief Default constructor is deleted!
       */
      BootFlagsClient() = delete;

      /*
       *=====================================================================================
       * Private
       *=====================================================================================
       */
   private:
      class PublishObserver: public com::ISubscriptionObserver
      {
      public:
         PublishObserver(BootFlagsClient *client) : m_client(client) {}

      private:
         void message(const std::string& topic, const com::Message& message) override;
         BootFlagsClient* m_client;
      };

      // -----------------------------------------------------------------------------

      bool wait_for_message(std::unique_lock<std::mutex>& lock);
      SetResult set_boot_flags(const pb::BootFlags& boot_flags);
      bool is_boot_flags_received() { return m_update_timestamp > 0; }

      // -----------------------------------------------------------------------------

      com::ITransportClient_ptr m_tclient;
      com::ISubscriptionObserver_ptr m_pub_obs;

      BootBank         m_current_bank;            ///< Current boot bank
      BootBank         m_boot_bank;               ///< Selected boot bank
      uint32_t         m_boot_counter;            ///< Boot counter for current bank
      bool             m_boot_valid_bank0;        ///< Is boot bank 0 valid
      bool             m_boot_valid_bank1;        ///< Is boot bank 1 valid
      BankSwitchCause  m_boot_bank_switch_cause;  ///< Boot bank switch cause
      BootMode         m_boot_mode;               ///< Selected boot mode
      uint64_t         m_update_timestamp;        ///< Time of last received

      std::condition_variable m_cv;
      std::mutex m_cv_mutex;
      int m_receive_timeout;

   protected:
   /**
    ****************************************************************************
    *
    * @brief Partition object unique pointer only accessible from derived
    * classes
    *
    ****************************************************************************
    */
      std::unique_ptr<Partition> m_partition;
};
/*
 *=====================================================================================
 * Public constant definitions for external access
 *=====================================================================================
 */

/**
 ****************************************************************************
 *
 * Convenience typedef to a shared pointer type of this class
 *
 ****************************************************************************
 */
typedef BootFlagsClient::ptr_t BootFlagsClient_ptr;

/**
 *******************************************************************************
 *
 *  @brief Create a BootFlags Client
 *
 *  @details Creates a new BootFlags Client using the supplied ITransportClient
 *  Note that there can be only one BootFlags client per ITransportClient
 *
 *  @param tclient An ITransportClient for internal communication
 *
 *  @return BootFlagsClient_ptr to the single instance of the BootFlags client
 *
 *******************************************************************************
 */
BootFlagsClient_ptr
create_boot_flags_client(com::ITransportClient_ptr tclient);

}}}

#endif /* BOOT_FLAGS_H_ */
