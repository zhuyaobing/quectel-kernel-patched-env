#ifndef SYSPARAMS_H
#define SYSPARAMS_H

/*
 *******************************************************************************
 *******************************************************************************
 ***
 ***     File: sysparams.h
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

#include <ecu/common.h>
#include <ecu/com/client.h>
#include <ecu/partition.h>

#include <ecu/pb/variables.pb.h>

#include <map>
#include <mutex>
#include <condition_variable>

namespace ecu {
namespace lapi {
namespace var {

/**
 * @brief System parameters Client API for retrieving ECU platform system parameters.
 *
 * @details The SysParamsClient supports retrieval (get_all()) and setting (set_all()
 * and write()) of ECU system parameters which are persisted in the NOR flash.
 * The permissions to this API are controlled by the underlying MQTT topic:
 *
 *  * READ  "system/sysparams" (Subscribe)
 *  * WRITE "system/sysparams/write" (Publish)
 *
 * An application should reuse and share the ecu::lapi::com::ITransportClient
 * instance for the SysParamsClient instance. Hence, an instance of the
 * ecu::lapi::com::ITransportClient needs to be provided in the constructor of
 * SysParamsClient.
 *
 * __Regular application__
 *
 * An application that just want to use specific system parameters, should use
 * the get() function.
 *
 *
 * __Privileged application__
 *
 * System parameters are always handled atomically by the SysParamsClient. When
 * reading using get_all() a map instance containing all parameters stored on
 * the ECU will be returned. Similarly, write() will write all parameters
 * at once. The parameters could be updated using the set_all() and merge()
 * functions.
 *
 * _Note_: The updating and writing of the parameters have intentionally been
 * split to raise awareness that the write operation could be disruptive.
 *
 * _Note_: the validity of the system parameters have to be ensured by the
 * application using this API, i.e. hashing.
 *
 * __Example usage__
 * @code
 *   // ITransportClient_ptr - shared with general topic subscriptions
 *   auto tclient = ecu::lapi::com::create_client(config);
 *
 *   // SysParamsClient_ptr
 *   auto handler = ecu::lapi::var::create_sysparams_client(tclient);
 *
 *   // Connect the internal Transport client without starting it
 *   tclient->connect();
 *
 *   // **************
 *   // Single parameter query - for regular applications
 *   // **************
 *
 *   // get the system parameter PIN_CODE, use OTAID for PIN_CODE
 *   int pin_otaid = 42;
 *   auto param = handler->get(pin_otaid);
 *
 *   // ....
 *
 *   // **************
 *   // For privileged apps that are able to write system parameters
 *   // **************
 *
 *   // get all system parameters
 *   auto sys_params = handler->get_all();
 *
 *   // get the system parameter "PIN_CODE"
 *   // use OTAID for PIN_CODE
 *   int pin_otaid = 42;
 *   auto pin_code = sys_params[pin_otaid];
 *
 *   // set a new system parameter
 *   sys_params[pin_otaid] = "2342";
 *
 *   // set the system parameters
 *   handler->set_all(sys_param);
 *
 *   // persist/write the system parameter to the unit
 *   // NOTE: this requires writing rights!
 *   if ( handler->write().nok() )
 *   {
 *      // error handling
 *   }
 *
 * @endcode
 *
 */
class SysParamsClient
{
public:
#ifndef DOXYGEN_IGNORE
   /**
    * Internal topic definitions, not to be used by application
    */
   static constexpr const char* TOPIC_REQ = "system/vard/request/sysparam";
   static constexpr const char* TOPIC_PUB = "system/sysparam/"; // + id
   static constexpr const char* TOPIC_ALL_REQ = "system/vard/request/sysparams";
   static constexpr const char* TOPIC_ALL_PUB = "system/sysparams";
   static constexpr const char* TOPIC_ALL_WRITE = "system/sysparams/write";
#endif /* ifndef DOXYGEN_IGNORE */


   /**
    ****************************************************************************
    *
    * @brief key-type of the parameter map
    *
    ****************************************************************************
    */
   typedef uint32_t ParamKey;

   /**
    ****************************************************************************
    *
    * @brief value-type of the parameter map
    *
    ****************************************************************************
    */
   typedef pb::SystemParameterEntry ParamValue;

   /**
    ****************************************************************************
    *
    * @brief The parameter map pair type.
    *
    ****************************************************************************
    */
   typedef std::pair<ParamKey, ParamValue> ParamPair;
   /**
    ****************************************************************************
    *
    * @brief The parameter map type.
    *
    ****************************************************************************
    */
   typedef std::map<ParamKey, ParamValue> ParamMap;

   /**
    ****************************************************************************
    *
    * Result type for getting System Parameter from a SysParamsClient
    *
    ****************************************************************************
    */
   typedef Result<ParamValue> GetResult;

   /**
    ****************************************************************************
    *
    * Convenience typedef to a shared pointer type of this class
    *
    ****************************************************************************
    */
   typedef std::shared_ptr<SysParamsClient> ptr_t;

   /**
    ****************************************************************************
    *
    * Disallow default constructor
    *
    ****************************************************************************
    */
   SysParamsClient() =delete;

   /**
    ****************************************************************************
    *
    *  @brief Constructor for the SysParamsClient
    *
    *  @details Needs to be initialized with an existing ITransportClient,
    *  therefore the default constructor is deleted
    *
    *  @param tclient An initialized internal transport client
    *
    ****************************************************************************
    */
   SysParamsClient(com::ITransportClient_ptr tclient);

   /**
    ****************************************************************************
    *
    * @brief Default destructor, allows inheritance
    *
    ****************************************************************************
    */
   virtual ~SysParamsClient();


   /**
    ****************************************************************************
    *
    * @brief Force the system parameters to be fetched from the daemon
    *
    * @details In order to receive updates from the daemon handling system
    * parameters, the fetching need to be enforced - which this function can be
    * used for. This function is blocking.
    *
    * @warning This will override all parameters that have been updated and
    * never been written back to the platform (SysParamsClient::write)
    *
    ****************************************************************************
    */
   virtual void fetch();

   /**
    ****************************************************************************
    *
    * @brief Get a system parameter by ParamKey
    *
    * @details This will request a specific system parameter based on the given
    * key. If no parameter has been received, yet, this call will be blocking.
    *
    * @param key The ParamKey which should be returned
    * @param force_fetch Used to force a refetch of the system parameter from
    * the daemon. If false, the cached value will be returned (if present).
    *
    * @return A GetResult type which indicates if the desired parameter exists.
    * If so, an instance of ParamValue will be present.
    *
    ****************************************************************************
    */
   virtual GetResult get(ParamKey key, bool force_fetch = false);


   /**
    ****************************************************************************
    *
    * @brief Get all system parameters currently used on the ECU
    *
    * @details Blocking function which will return all parameters available on
    * the system.
    *
    * @param force_fetch Used to force a refetch of the system parameter from
    * the daemon. If false, the cached values will be returned (if present).
    *
    *
    * @return Will return a copy of the SysParamsClient::ParamMap
    *
    ****************************************************************************
    */
   virtual ParamMap get_all(bool force_fetch = false);

   /**
    ****************************************************************************
    *
    * @brief Sets/overwrites the internal system parameter map
    *
    * @details This will overwrite all internal system parameters associated
    * with SysParamsClient instance. Nothing will yet be persisted, therefore
    * the write() should to be used.
    *
    * The provided const reference will be copied.
    *
    * @note This function prevents its usage on the TPA partition, logging an
    * error on the journal and not setting any internal system parameter.
    *
    * @sa merge()
    *
    * @param parameter_map The ParamMap that shall be set.
    *
    ****************************************************************************
    */
   virtual void set_all(const ParamMap& parameter_map);

   /**
    ****************************************************************************
    *
    * @brief Merges the give ParamMap with the internal system parameter map
    *
    * @details The given \p parameter_map will be merged with the internally kept
    * ParamMap, where the internal one will be the base for the merge. Values
    * that exists in both maps, will be overwritten with the provided
    * \p parameter_map. As the set_all() function, nothing will be written,
    * therefore write() needs to be called explicitly.
    *
    * @note This function prevents its usage on the TPA partition, logging an
    * error on the journal and not setting any internal system parameter.
    *
    * @sa set_all()
    *
    * @param parameter_map The ParamMap that shall be merged
    *
    ****************************************************************************
    */
   virtual void merge(const ParamMap& parameter_map);


   /**
    ****************************************************************************
    *
    * @brief Error id that can occur when write SysParams back to the platform
    *
    ****************************************************************************
    */
   enum WriteErrorId
   {
      WRITE_ERROR_NO_ERROR = 0, ///< No error occurred
      WRITE_ERROR_UNCHANGED,    ///< map is unchanged nothing needs to be written
      WRITE_ERROR_EMPTY_MAP,    ///< map is empty, write call suppressed
      WRITE_ERROR_INCOMPLETE,   ///< Not all values could be written, most likely
                                ///< due to rights management on RT side.
                                ///< Values which could not be written, will be
                                ///< part of the WriteResult::val
      WRITE_ERROR_COMMUNICATION,///< communication error with service handling
                                ///< parameter writing
      WRITE_ERROR_SERIALIZE,    ///< serialization error when trying to send
                                ///< parameter map
   };

   /**
    ****************************************************************************
    *
    * @brief Type which should be used as a write result
    *
    * @details In case all parameter could be written, the Result will respond
    * true on ok(), otherwise \p false.
    *
    * @note In case not all values could be written due rights management on
    * the real-time partition (WRITE_ERROR_INCOMPLETE), the WriteResult will
    * indicate nok() == true and the result.val() will carry a ParamMap of the
    * parameters which could not be written.
    *
    ****************************************************************************
    */
   using WriteResult = Result<ParamMap, WriteErrorId>;

   /**
    ****************************************************************************
    *
    * @brief Persist/Write the internal ParamMap to the platform
    *
    * @details The function will write the internally kept ParamMap (associated
    * to the SysParamsClient instance) persistently.
    *
    * The return value will indicate if the writing has been successful.
    *
    * After the write request has been issued, all parameters will be retrieved
    * again. If the internal cache (\p get_all()) does not match the parameters
    * that has just been received again, the WriteResult will indicate that. If
    * both parameter maps are the same - the one that has been issued for
    * writing and the one actually present in the ECU - \p a positive result
    * will be returned
    *
    * Due to above mentioned verification, this functions is blocking!
    *
    * Client applications that want to write system parameter, need to have
    * write access (MQTT topic).
    *
    * @return ok() if successful, otherwise \p nok()! In the \p nok() case
    * and err_vall() is WRITE_ERROR_INCOMPLETE, the internal map of the
    * SysParamsClient instance will be set to the last received
    * values from the ECU and the SysParamsClient::WriteResult will carry the
    * values which could not be written successfully.
    *
    ****************************************************************************
    */
   virtual WriteResult write();

   /**
    ****************************************************************************
    *
    * @brief Set the connection timeout when receiving system parameter.
    *
    * @details The system parameter will be received via an ITransportClient
    * instance (@see SysParamsClient()). The timeout for this connection when
    * receiving the system parameters can be set via this function.
    * The default timeout is 60s which should be sufficient most use cases.
    *
    * @param timeout The timeout which should be set.
    *
    ****************************************************************************
    */
   virtual void set_receive_timeout(int timeout);

private:
   class PublishObserver: public com::ISubscriptionObserver
   {
   public:
      PublishObserver(var::SysParamsClient* client) : m_client(client) {}

   private:
      void message(const std::string& topic, const com::Message& message) override;
      var::SysParamsClient* m_client;
   };

   void retrieve_all();

   com::ITransportClient_ptr m_tclient;
   com::ISubscriptionObserver_ptr m_pub_obs;
   ParamMap m_sysp_map;
   std::mutex m_sysp_map_mutex;
   std::condition_variable m_cv;
   std::mutex m_cv_mutex;
   int m_receive_timeout;
   bool m_is_changed;

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
 *==============================================================================
 * Public constant definitions for external access
 *==============================================================================
 */

/**
 *******************************************************************************
 *
 * Convenience typedef to a shared pointer type of this class
 *
 *******************************************************************************
 */
typedef SysParamsClient::ptr_t SysParamsClient_ptr;

/**
 *******************************************************************************
 *
 *  @brief Create a SysParamsClient
 *
 *  @details Creates a new SysParamsClient using the supplied ITransportClient
 *  Note that there can be only one SysParamsClient per ITransportClient
 *
 *  @param tclient An ITransportClient for internal communication
 *
 *  @return SysParamsClient_ptr to the single instance of the SysParamsClient
 *
 *******************************************************************************
 */
SysParamsClient_ptr
create_sysparams_client(com::ITransportClient_ptr tclient);

}}}

#endif /* SYSPARAMS_H */
