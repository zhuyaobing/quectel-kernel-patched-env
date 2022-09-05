/*
 *******************************************************************************
 *******************************************************************************
 ***
 ***     File: acl.h
 ***
 ***     Project: Alexandra
 ***
 ***     Copyright (C) Stoneridge Electronics AB 1990 - 2021
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
#ifndef ACL_H
#define ACL_H

#include <ecu/com/client.h>
#include <ecu/common.h>

namespace ecu
{
namespace lapi
{

/**
*******************************************************************************
*
* @brief Contains all Linux data-layer related artifacts
*
*******************************************************************************
*/
namespace com
{

/**
 *******************************************************************************
 *
 * @brief Defines what lapi feature clients are enabled for subscribe/publish
 *
 * @details When a new ACL for MQTT broker is to be defined, one app can, without
 * the knowledge of every topic, state which clients is going to use through
 * this struct. The AclClient will make sure all necessary topics will be added
 * on publish/subscribe list configured on MQTT broker.
 *
 *******************************************************************************
 */
struct LapiFeatures {
   bool udsclient{false};           ///< Allow access to diag::UdsClient
   bool ecuvar{false};              ///< Allow access to var::VariablesClient for
                                    ///< ECU variables
   bool vehvar{false};              ///< Allow access to var::VariablesClient for
                                    ///< Vehicle variables
   bool pgnreq{false};              ///< Allow access to diag::PgnRequestClient
   bool sysparam{false};            ///< Allow access to var::SysParamsClient
   bool backend{false};             ///< Allow access to com::BackendClient. CID
                                    ///< topics must be defined explicitly in
                                    ///< topics allowed for subscription as
                                    ///< `app/cid/<cid number>`
};

/**
 *******************************************************************************
 *
 * @brief Status for ACL request / response
 *
 *******************************************************************************
 */
enum AclResponseError
{
   ACL_RESPONSE_NOK,               ///< ACL negative response received correctly
   ACL_RESPONSE_ERR_INPUTDATA,     ///< Invalid parameters for request supplied
   ACL_RESPONSE_ERR_SERIALIZATION, ///< ACL Request serialization error
   ACL_RESPONSE_ERR_TIMEOUT,       ///< ACL request timed out, response was not
                                   ///< received in time
   ACL_RESPONSE_ERR_REJECTED,      ///< Action is not allowed for current app
   ACL_RESPONSE_ERR_UNKNOWN,       ///< Other error
};

/**
 *******************************************************************************
 *
 * @brief ACL Client API for sending configurations for any configurable ACL
 *
 * The AclClient reuses an existing ITransportClient instance. Thus, this
 * has to be created and initialized beforehand. The ITransportClient instance
 * is used for internal communication only - request, response towards the ACL
 * request handler.
 *
 * The AclClient should be instantiated with the create_acl_client factory
 * function to create a shared_ptr wrapped AclClient instance.
 *
 * The AclClient provides only one way of communicating to the ACL daemon on
 * the platform: synchronously. That means all functions will block and return
 * when an answer has been received.
 *
 * The user of the AclClient need to handle the negative responses
 * accordingly, via AclResponseError.
 *
 * __Sync example usage__
 * @code
 *   #include <ecu/com/client.h>
 *   #include <ecu/com/acl.h>
 *
 *   using namespace ecu::lapi;
 *   // .....
 *
 *   // ITransportClient_ptr
 *   auto tclient = com::create_client(config);
 *
 *   auto result = diag::create_acl_client(tclient);
 *   if (result.nok())
 *   {
 *      //error handling. Means that ACL feature is not available
 *   }
 *   else
 *   {
 *      auto acl_client = r.take_val();
 *   }
 *
 *   auto result = client->add_mqtt_user("user1", "passwd1");
 *   if (result.nok())
 *   {
 *      // something was wrong
 *      LOG(ERROR) << "Request did not succeed. error: " << result.err_val();
 *      return;
 *   }
 * @endcode
 *
 *******************************************************************************
 */
class AclClient
{
public:

   /**
    * Default ACL request timeout value
    */
   static constexpr const uint32_t ACL_REQUEST_TIMEOUT_MS = 150000;

   /**
    ****************************************************************************
    *
    * @brief Generic result type for an ACL command without any data to return
    *
    ****************************************************************************
    */
   using AclResult = Result<bool, AclResponseError>;

   /**
   *****************************************************************************
   *
   *  @brief Constructor for the AclClient
   *
   *  @details Needs to be initialized with an existing ITransportClient,
   *  therefore the default constructor is deleted
   *
   *  @param tclient An initialized internal transport client
   *****************************************************************************
   */
   explicit AclClient(com::ITransportClient_ptr& tclient) {};

   /**
    ****************************************************************************
    *
    * @brief delete copy constructor
    ****************************************************************************
    */
   AclClient(const AclClient&) = delete;

   /**
    ****************************************************************************
    *
    * @brief Default destructor, allows inheritance
    ****************************************************************************
    */
   virtual ~AclClient() {};

   /**
    ****************************************************************************
    * @brief Add a new user on MQTT broker
    *
    * @details Include a new user into the broker, being able to get connected,
    * subscribe and publish into topics, considering the ACL for the user.
    *
    * @param user Username used to get connected to the broker.
    * Cannot contain special characters.
    *
    * @param passwd Password for the user. Special characters !@#%,./?~^{}[]
    * are allowed.
    *
    * @return The AclResult which indicates if the request resulted in a
    * response (result.ok()) or not in which case the Error code is provided.
    ****************************************************************************
    */
   virtual AclResult add_mqtt_user(const std::string& user,
      const std::string& passwd) = 0;

   /**
    ****************************************************************************
    * @brief Create a new ACL for defined user for MQTT broker
    *
    * @details Create (override if one already exists) an ACL on MQTT broker,
    * defining which topics a user can subscribe and publish.
    *
    * @param user Username used to get connected to the broker
    *
    * @param sub_topics list of topics user is able to subscribe
    *
    * @param pub_topics list of topics user is able to publish
    *
    * @param lapi_features defines this API features that user is able to
    *  use(e.g. UDS/EcuVar). It is translated to topics to subscribe/publish
    *
    * @return The AclResult which indicates if the request resulted in a
    * response (result.ok()) or not in which case the Error code is provided.
    ****************************************************************************
    */
   virtual AclResult add_mqtt_acl(const std::string& user,
      const std::vector<std::string>& sub_topics,
      const std::vector<std::string>& pub_topics,
      const LapiFeatures& lapi_features) = 0;

   /**
    ****************************************************************************
    * @brief Remove user and ACL for defined user for MQTT broker
    *
    * @details Remove user and its ACL from MQTT broker
    *
    * @param user Username is going to be removed from MQTT broker's allowlist
    *
    * @return The AclResult which indicates if the request resulted in a
    * response (result.ok()) or not in which case the Error code is provided.
    ****************************************************************************
    */
   virtual AclResult rm_mqtt_user(const std::string& user) = 0;

   /**
    ****************************************************************************
    * @brief Create a new user for websocket connection
    *
    * @details Create a user(location) that will enable a websocket connection
    * on defined port
    *
    * @param user Username used to be able to communicate via websocket.
    * Cannot contain special characters.
    *
    * @param passwd Password for the user. Special characters !@#%,./?~^{}[]
    * are allowed.
    *
    * @param port What port user will get connected
    *
    * @return The AclResult which indicates if the request resulted in a
    * response (result.ok()) or not in which case the Error code is provided.
    ****************************************************************************
    */
   virtual AclResult add_websocket_user(const std::string& user,
      const std::string& passwd, uint32_t port) = 0;

   /**
    ****************************************************************************
    * @brief Remove user for websocket connection
    *
    * @details Remove user and location (has the same name) from nginx rules
    *
    * @param user User to be removed
    *
    * @return The AclResult which indicates if the request resulted in a
    * response (result.ok()) or not in which case the Error code is provided.
    ****************************************************************************
    */
   virtual AclResult rm_websocket_user(const std::string& user) = 0;

   /**
    ****************************************************************************
    * @brief Add new user on mdpd
    *
    * @details Include a new user into mdpd (rest daemon), being able to
    * subscribe and publish into topics via REST, considering user's ACL.
    *
    * @param user Username registered on mdpd to accept GET/POSTs
    * Cannot contain special characters.
    *
    * @param passwd Password for the user. Special characters !@#%,./?~^{}[]
    * are allowed.
    *
    * @return The AclResult which indicates if the request resulted in a
    * response (result.ok()) or not in which case the Error code is provided.
    ****************************************************************************
    */
   virtual AclResult add_rest_user(const std::string& user,
      const std::string& passwd) = 0;

   /**
    ****************************************************************************
    * @brief Remove existing user from mdpd
    *
    * @details Remove existing user from mdpd (rest daemon)
    *
    * @param user Username removed from mdpd
    *
    * @return The AclResult which indicates if the request resulted in a
    * response (result.ok()) or not in which case the Error code is provided.
    ****************************************************************************
    */
   virtual AclResult rm_rest_user(const std::string& user) = 0;

   /**
    ****************************************************************************
    * @brief Create new ACL for defined user for Mdpd (REST daemon)
    *
    * @details Create (override if one already exists) an ACL on mdpd,
    * defining which topics a user can subscribe and publish (via REST).
    *
    * @param user Username used to bind topics
    *
    * @param sub_topics list of topics user is able to subscribe
    *
    * @param pub_topics list of topics user is able to publish
    *
    * @return The AclResult which indicates if the request resulted in a
    * response (result.ok()) or not in which case the Error code is provided.
    ****************************************************************************
    */
   virtual AclResult add_rest_acl(const std::string& user,
      const std::vector<std::string>& sub_topics,
      const std::vector<std::string>& pub_topics) = 0;

protected:
   /**
    ****************************************************************************
    *
    * @brief default constructor only accessible from derived classes
    *
    ****************************************************************************
    */
   AclClient() = default;
};


/**
 *******************************************************************************
 *
 * Convenience typedef to a shared pointer type of this class
 *
 *******************************************************************************
 */
using AclClient_ptr = std::shared_ptr<AclClient>;

/**
 *******************************************************************************
 *
 *  @brief Create a AclClient_ptr
 *
 *  @details Creates a new AclClient using the supplied ITransportClient.
 *  Takes into consideration if the client_name is realiable to change acls and
 *  hwversion.
 *
 *  @param tclient An ITransportClient for internal communication
 *
 *  @return AclClient_ptr to the shared_ptr instance of the AclClient
 *
 *******************************************************************************
 */
Result<AclClient_ptr>
create_acl_client(com::ITransportClient_ptr& tclient);

} // namespace com

} // namespace lapi

} // namespace ecu


#endif /* ACL_H */
