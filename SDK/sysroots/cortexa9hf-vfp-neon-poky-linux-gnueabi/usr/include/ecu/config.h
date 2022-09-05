/*
***************************************************************************************
***************************************************************************************
***
***     File: config.h
***
***     Project: Alexandra
***
***     Copyright (C) Stoneridge Electronics AB 1990 - 2016
***
***************************************************************************************
***************************************************************************************


    Subject:

    Document reference:

    Description of this header file:

    Version:


***************************************************************************************
*/


/*
**=====================================================================================
** Header definition
**=====================================================================================
*/
#ifndef CONFIG_H
#define CONFIG_H

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/

#include <map>
#include <set>
#include <string>
#include <memory>
#include <sstream>

/*
**=====================================================================================
** Public type definitions
**=====================================================================================
*/
namespace ecu {
namespace lapi {
/**
 * @brief Contains all configuration related functionality
 */
namespace config {

/**
 *******************************************************************************
 *
 * @brief A representation of a configuration which shall be used be application
 * which depend on the alex LAPI framework.
 *
 * @details The Configuration is wrapper around std map which will be initialized
 * on creation and provides convenience methods to access the data.
 * Creation can Either be done via the factory method create_from_input() or a
 * give Configuration can be initialized with an ifstream using the
 * Configuration::set_config_from_input() method.
 * The Configuration_ptr shall be used when an instantiating a Configuration on
 * the heap (to avoid leaking of resources).
 *
 * Example usage:
 * @code
 *  config::Configuration_ptr config(new config::Configuration());
 *  if ( !arg_config_file.empty() ) {
 *     std::ifstream cf;
 *     cf.open(arg_config_file, std::ifstream::in);
 *
 *     if (!cf.is_open())
 *     {
 *        LOG(ERROR) << "Unable to open config file.";
 *        exit(EXIT_FAILURE);
 *     }
 *     config::set_config_from_input(config, cf);
 *  }
 *  //config->add_required("dlxml");
 *  config->set_default("client_name", "SRE MQTT App");
 *
 * @endcode
 *
 * Please note: A ecu::lapi::com::ITransportClient always requires a
 * configuration, therefore see the factory-method ecu::lapi::com::create_client
 *
 *******************************************************************************
 */
class Configuration
{
public:
   /**
    ****************************************************************************
    *
    * Convenience typedef to a unique pointer type of this class
    *
    ****************************************************************************
    */
   typedef std::unique_ptr<Configuration> ptr_t;
   /**
    ****************************************************************************
    *
    * Key Type to be used in the class
    *
    ****************************************************************************
    */
   typedef std::string Key;
   /**
    ****************************************************************************
    *
    * Value Type to be used in the class
    *
    ****************************************************************************
    */
   typedef std::string Value;


   /**
    ****************************************************************************
    *
    * @brief Factory method which creates a configuration from an in stream
    *
    * @details The input stream will most likely be a istream provided via an
    *           ifstream from a configuration file.
    *
    * @param in The input stream from which config parameter shall be extracted
    *            from.
    *
    * @return Returns the Configuration
    *
    ****************************************************************************
    */
   static ptr_t create_from_input(std::istream& in);

   /**
    ****************************************************************************
    *
    * @brief Default constructor initializes the required and defaults config
    *    elements with default values see, DEFAULT_VALUES and REQUIRED_VALUES
    *
    ****************************************************************************
    */
   Configuration();

   /**
    ****************************************************************************
    *
    * @brief Verifies the Configuration instance against the default and
    *    required values.
    *
    * @details The caller must take care in the case the verification will not
    * pass.
    *
    * @return True if the verification passed, else false.
    *
    ****************************************************************************
    */
   bool verify();


   /**
    ****************************************************************************
    *
    * @brief Allows marking certain configuration entries as required
    *
    * @details Will be used in @sa verify()
    *
    * @param key The configuration key which should be marked required.
    *
    ****************************************************************************
    */
   void add_required(const Key& key) { m_required.insert(key); }

   /**
    ****************************************************************************
    *
    * @brief Allows setting default values to the configuration.
    *
    * @details If the caller does not implicitly set any value, the provided
    * values set with this function will be assumed.
    *
    * @param key The configuration key which should have a default value
    * @param val The desired value
    *
    ****************************************************************************
    */
   void set_default(const Key& key, const Value& val) { m_defaults[key] = val; }

   /**
    ****************************************************************************
    *
    * @brief Gets the value for a give configuration key.
    *
    * @return Will return the value corresponding to the provided key. If the
    * key does not exists an empty string ("") will be returned.
    *
    * @param key The configuration key.
    *
    ****************************************************************************
    */
   const Value& get(const Key& key);


   /**
    ****************************************************************************
    *
    * @brief Sets the value for a give configuration key.
    *
    * @param key The configuration key.
    * @param val The configuration value.
    *
    ****************************************************************************
    */
   void set(const Key& key, const Value& val) { entries[key] = val; }

   /**
    ****************************************************************************
    *
    * The entries map of the Configuration, basic key-value map.
    * TODO this should be private, I guess
    *
    ****************************************************************************
    */
   std::map<Key, Value> entries;
private:
   std::map<Key, Value> m_defaults; /**< On construction, the entries shall be
                                         initialized with this values. */
   std::set<Key> m_required;        /**< During verification, verify(), it will
                                         checked if all required keys are
                                         present. */
   bool m_is_verified;
};
/**
 *******************************************************************************
 *
 * @brief convenience typedef to the pointer type.
 *
 * @details This typedef shall be preferred over Configuration::ptr_t
 *
 *******************************************************************************
 */
typedef Configuration::ptr_t Configuration_ptr;

/*
**=====================================================================================
** Public constant definitions for external access
**=====================================================================================
*/

/**
 *******************************************************************************
 *
 * Default map entries for the configuration. This is used when creating a
 * new Configuration instance.
 *
 *******************************************************************************
 */
static const std::map<Configuration::Key, Configuration::Value> DEFAULT_VALUES{
   std::make_pair("host", "localhost"),
   std::make_pair("port", "1883")
};
/**
 *******************************************************************************
 *
 * @brief default required values
 *
 * @details The default required values are used for the MQTT communication
 *
 *******************************************************************************
 */
static const std::set<Configuration::Key> REQUIRED_VALUES{
   "host",
   "port",
   "client_name"
};

/**
 *******************************************************************************
 *
 * @brief Factory method which creates a configuration from an in stream
 *
 * @details The input stream will most likely be a istream provided via an
 *          ifstream from a configuration file.
 *
 * @param config The Configuration that shall be set based on in
 *
 * @param in The input stream from which config parameter shall be extracted
 *           from.
 *
 *******************************************************************************
 */
void set_config_from_input(Configuration_ptr& config, std::istream& in);

}}} // end namespaces

#endif /* CONFIG_H */
