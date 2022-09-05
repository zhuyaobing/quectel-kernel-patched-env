/*
 *******************************************************************************
 *******************************************************************************
 ***
 ***     File: variables.h
 ***
 ***     Project: Alexandra
 ***
 ***     Copyright (C) Stoneridge Electronics AB 1990 - 2016
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

#ifndef VARIABLES_H_
#define VARIABLES_H_

#include <map>
#include <memory>
#include <condition_variable>

#include <ecu/com/client.h>
#include <ecu/com/message.h>
#include <ecu/com/observer.h>

namespace ecu {
namespace lapi {
namespace var {

/**
 ****************************************************************************
 *
 *  @brief Variables Client API for retrieving ECU variables and system parameters
 *
 *  @details The Variables Client API supports retrieving ECU variables and system
 *  parameters different system daemons that translate and handle the data.
 *  Each application should initialize one ITransportClient for all the internal
 *  communication and pass that client to the constructor of a Variables Client
 *  that will then be used for retrieving variables.
 *
 *  Note! It is important to create and connect the internal ITransportClient
 *  before trying to use the Variables Client, however you do not have to start
 *  it, just create and connect.
 *
 *  To get a variable simply call the get("variable identifier") method with
 *  the predefined variable id. It is a blocking call that will return the
 *  variable value as a std::string. At first request it will be fetched
 *  externally and then cached and automatically kept up to date.
 *
 *  Available variable identifiers depends on the running system daemons and
 *  is not maintained inside the VariablesClient API in the LAPI.
 *
 *
 * Example usage:
 * @code
 *   // Create ITransportClient_ptr
 *   auto tclient = ecu::lapi::com::create_client(config)
 *
 *   // Connect the internal Transport client without starting it
 *   tclient->connect();
 *
 *   // Create VariablesClient_ptr
 *   auto vars = ecu::lapi::var::create_var_client(tclient)
 *
 *   // Retrieve a variable, e.g. VIN
 *   auto vin = vars->get("vehicleid");
 *
 *   // Retrieve a variable, e.g. serial number, using a default
 *   auto sn = vars->get("serialnumber", "default serialnumber");
 *
 *   // Start the ITransportClient message pump
 *   tclient->start();
 * @endcode
 *
 ***************************************************************************
 */
class VariablesClient
{
   public:
      /**
       ****************************************************************************
       *
       * Convenience typedef to a shared pointer type of this class
       *
       ****************************************************************************
       */
      typedef std::shared_ptr<VariablesClient> ptr_t;

#ifndef DOXYGEN_IGNORE
      /**
       * Internal topic definitions, not to be used by application
       */
      static constexpr const char* TOPIC_REQ = "system/vard/request"; // TODO: TO BE REMOVED
      static constexpr const char* TOPIC_PUB = "system/var/"; // + variable name
#endif /* ifndef DOXYGEN_IGNORE */

      /**
       ****************************************************************************
       *
       *  @brief Constructor for the Variables Client
       *
       *  @details Needs to be initialized with an existing ITransportClient,
       *  therefore the default constructor is hidden
       *
       *  @param tclient An initialized internal transport client
       *
       ***************************************************************************
       */
      VariablesClient(com::ITransportClient_ptr tclient);
      virtual ~VariablesClient();

      /**
       ****************************************************************************
       *
       *  @brief Get an ECU Variable
       *
       *  @details Retrieves an ECU variable. At first request it will be fetched
       *  externally and then cached and automatically kept up to date.
       *
       *  @param variable String identifier of the variable to fetch
       *  @param default_value Optional string that will be returned if the requested
       *                  value is not found or the request timed out.
       *  @param timeout Optional timeout (ms) of the request before returning the
       *                 default_value.
       *
       *  @return The value of the requested variable on success,
       *          An empty string or default_value if provided, if the variable
       *          was not yet available, unknown or the request timed out.
       *
       ***************************************************************************
       */
      virtual const std::string& get(
            const std::string& variable,
            const std::string& default_value = "",
            int timeout = 60000);

      /**
       ****************************************************************************
       *
       *  @brief Get an ECU Variable with Time Stamp
       *
       *  @details Retrieves an ECU variable. At first request it will be fetched
       *  externally and then cached and automatically kept up to date.
       *
       *  @param[out] timestamp_ms  The time (ms) when requested variable was last
       *                            received/updated.
       *  @param variable String identifier of the variable to fetch
       *  @param default_value Optional string that will be returned if the requested
       *                  value is not found or the request timed out.
       *  @param timeout Optional timeout (ms) of the request before returning the
       *                 default_value.
       *
       *  @return The value of the requested variable on success,
       *          An empty string or default_value if provided, if the variable
       *          was not yet available, unknown or the request timed out.
       *
       ***************************************************************************
       */
      virtual const std::string& get_extended(
            uint64_t& timestamp_ms,
            const std::string& variable,
            const std::string& default_value = "",
            int timeout = 60000);

      /**
       * @brief Default constructor is deleted!
       */
      VariablesClient() = delete;
/*
 *=====================================================================================
 * Private
 *=====================================================================================
 */
   private:
      class PublishObserver: public com::ISubscriptionObserver
      {
         public:
            PublishObserver(VariablesClient *client) : m_client(client) {}

         private:
            void message(const std::string& topic, const com::Message& message) override;
            VariablesClient* m_client;
      };

// -----------------------------------------------------------------------------

      com::ITransportClient_ptr m_tclient;
      com::ISubscriptionObserver_ptr m_pub_obs;

      struct Variable {
            Variable() :
               value(""),
               timestamp(0),
               cv()
            {}
            std::string value;
            uint64_t timestamp;
            std::condition_variable cv;
      };
      std::mutex m_cv_mutex;
      typedef std::map<const std::string, Variable> VariablesMap_t;
      VariablesMap_t m_variables_map;
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
typedef VariablesClient::ptr_t VariablesClient_ptr;

/**
 *******************************************************************************
 *
 *  @brief Create a Variables Client
 *
 *  @details Creates a new Variables Client using the supplied ITransportClient
 *  Note that there can be only one Variables client per ITransportClient
 *
 *  @param tclient An ITransportClient for internal communication
 *
 *  @return VariablesClient_ptr to the single instance of the Variables client
 *
 *******************************************************************************
 */
VariablesClient_ptr
create_var_client(com::ITransportClient_ptr tclient);

}}}

#endif /* VARIABLES_H_ */
