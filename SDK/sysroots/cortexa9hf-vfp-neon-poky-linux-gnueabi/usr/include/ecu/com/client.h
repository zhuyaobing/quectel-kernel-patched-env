/*
********************************************************************************
********************************************************************************
***
***     File: client.h
***
***     Project: Alexandra
***
***     Copyright (C) Stoneridge Electronics AB 1990 - 2016
***
********************************************************************************
********************************************************************************


    Subject:

    Document reference:

    Description of this header file:

    Version:


********************************************************************************
*/

/**
**==============================================================================
** Header definition
**==============================================================================
*/
#ifndef CLIENT_H
#define CLIENT_H

/**
**==============================================================================
** Imported definitions
**==============================================================================
*/

// stl
#include <string>
#include <memory>
#include <chrono>

// sre
#include <ecu/config.h>

#include <ecu/com/message.h>
#include <ecu/com/observer.h>
#include <ecu/com/protocol.h>

/**
 *==============================================================================
 * Public type definitions
 *==============================================================================
 */
namespace ecu {
/**
 * @brief Contains application related artifacts
 */
namespace lapi {
namespace com {

/**
 *******************************************************************************
 *
 * @brief Transport Client communication protocol type
 *
 * @details TYPE_MQTT is the default for the alexandra platform
 *
 *******************************************************************************
 */
enum TransportType
{
   TYPE_MQTT    = 0 /**< The MQTT communication protocol*/
};

/**
 *******************************************************************************
 *
 * Globally defined topic which shall be used for accounting RT topic subscriptions
 *
 * @details A message shall be sent to this topic if an application intents to
 * receive events on a specific RT topic. The ITransportClient::subscribe() will
 * automatically perform the announcement for topics leading with "rt/".
 *
 *******************************************************************************
 */
static std::string ANNOUNCEMENT_TOPIC = "subscription/announcement";

/**
 *******************************************************************************
 *
 * Globally defined topic on which IPCd publish list of currently subscribed RT topics
 *
 *******************************************************************************
 */
static std::string IPC_SUBSCRIPTIONS_TOPIC = "system/ipc/subscriptions";

/**
 *******************************************************************************
 *
 *  @brief Interface for a Transport Client which should provide functionality
 *         to publish and subscribe message using specific topics.
 *
 *  @details Users of the ITransportClient should instantiate one, _and only one_
 *           client per application. The suggested program flow is described in
 *           the example below where one ITransportClient is configured and
 *           created in the beginning of the application and shutdown right before
 *           the application exit.
 *
 *           A user shall be able to use an ITransportClient to publish and
 *           subscribe to certain 'topics' and notify registered
 *           ISubscriptionObserver on reception of such, see subscribe().
 *
 *
 *           The intention of the abstract interface class ITransportClient is
 *           to give the users of LAPI the choice to either use the included
 *           default TransportType of MQTT or implement their own transport
 *           protocol whilst keeping a stable interface (this) towards the
 *           all applications.
 *
 *           Derived transport client implementations have to implement all
 *           abstract methods and take great care to understand their intentions.
 *
 *
 * Example usage:
 * @code
 * #include <ecu/com/client.h>
 * #include <ecu/com/observer.h>
 *
 *   // Setup client
 *   ITransportClient_ptr tclient = ecu::lapi::com::create_client(config);
 *
 *   // Instantiate an observer (implementing an ISubscriptionObserver)
 *   ISubscriptionObserver_ptr my_obs = ISubscriptionObserver_ptr(new MyObserver());
 *
 *   // Connect to internal broker and wait for an ACK (blocking)
 *   tclient->connect();
 *
 *   // Create any desired LAPI clients using the created ITransportClient_ptr
 *   // e.g. a VariablesClient, BackendClient etc.
 *
 *   // Start client loop (non-blocking)
 *   tclient->start();
 *
 *   // Subscribe to topics
 *   tclient->subscribe("some/topic", my_obs);
 *
 *   // Main application work is carried out
 *   // When it returns, program is done
 *   application->run();
 *
 *   // Disconnect the transport client
 *   tclient->disconnect();
 *
 *   // Wait for client to finish (blocking)
 *   tclient->join();
 *
 * @endcode
 *
 *******************************************************************************
 */
class ITransportClient
{
public:
   /**
    ****************************************************************************
    *
    * Convenience typedef to a unique pointer type of this class
    *
    ****************************************************************************
    */
   typedef std::unique_ptr<ITransportClient> uptr_t;

   /**
    ****************************************************************************
    *
    * Convenience typedef to a shared pointer type of this class
    *
    ****************************************************************************
    */
   typedef std::shared_ptr<ITransportClient> sptr_t;

   /**
    ****************************************************************************
    *
    * Destructor to allow inheritance.
    *
    ****************************************************************************
    */
   virtual ~ITransportClient() {}

   /**
    ****************************************************************************
    *
    *  @brief Connect to a broker: Blocking!
    *
    *  @details Connect to a broker, without starting the message loop.
    *           NOTE! Will try to connect forever and block execution until a
    *           successful connection has been established.
    *
    *           Automatic retries will be performed if the connection is lost.
    *
    *           Calling connect() is optional. To start the client non-blocking
    *           simply instantiate it and call start() which will connect
    *           asynchronously.
    *
    *  @return true if the client has been connected,
    *          false if there are initialization failures
    *                (due to incorrect config etc.)
    *
    ****************************************************************************
    */
   virtual bool connect() = 0;

   /**
    ****************************************************************************
    *
    *  @brief Disconnects from the broker
    *
    *  @details Disconnects from the broker and stops the client. Any waiting
    *           join() will return.
    *
    ***************************************************************************
    */
   virtual void disconnect() = 0;

   /**
    ****************************************************************************
    *
    *  @brief Get the current connection state
    *
    *  @return true if the client is currently connected, false otherwise
    *
    ****************************************************************************
    */
   virtual bool isConnected() = 0;


   /**
    ****************************************************************************
    *
    *  @brief Get the ITransportClient current ready state
    *
    *  @details The ITransportClient is deemed ready if
    *  - It is connected, connect()
    *  - It is started, start()
    *  - All pending subscriptions have been acknowledged by the MQTTT broker
    *  - All pending announcements (RT topic subscriptions) have been
    *    acknowledged by the IPCd
    *
    *  For early bird data, this function should be verified true before signaling
    *  application is ready.
    *
    *  Consider using the wait_ready() for a blocking version.
    *
    *  @sa ecu::lapi::application_ready()
    *  @sa wait_ready()
    *
    *  @return true if the client is ready, false otherwise
    *
    ****************************************************************************
    */
   virtual bool is_ready() = 0;

   /**
    ****************************************************************************
    *
    *  @brief Wait until ITransportClient is ready
    *
    *  @details Blocking version of is_ready() to wait for it to become ready
    *
    *  @sa is_ready()
    *
    *  @param timeout Timeout before returning false
    *
    *  @return true if the client is ready, false if timeout occurred
    *
    ****************************************************************************
    */
   virtual bool wait_ready(
         const std::chrono::seconds timeout) = 0;

   /**
    ****************************************************************************
    *
    *  @brief Publish a message to a given topic
    *
    *  @details Tries to publish a message on a given topic. If the message
    *           can't be published, e.g. if the broker isn't connected and
    *               *) QoS == 0, the message will be lost
    *               *) QoS > 0 the client will try to resend the message forever
    *
    *  @param topic The topic on which the message shall be published
    *  @param msg The message that shall be published
    *
    *  @return A locally unique identifier of the message that is: \n
    *          > 0: if the message could be published immediately \n
    *          < 0: if publish failed but will keep trying (QoS > 0) \n
    *          = 0: if the message failed to publish and has been dropped (QoS = 0)
    *
    ***************************************************************************
    */
   virtual int publish(const std::string& topic, const Message& msg) = 0;

   /**
    ****************************************************************************
    *
    *  @brief Publish a message to a given topic and wait for delivery acknowledgment
    *
    *  @details Tries to publish a message on a given topic and blocks until
    *           the message has been acknowledged by the broker for delivery or
    *           the timeout has been reached.
    *
    *  @note    This method is only valid for messages with QoS 1 or 2 and will
    *           NOT work for QoS 0, which will return immediately regardless of
    *           delivery result!
    *
    *  @note    This method can NOT be called from within an ISubscriptionObserver
    *           or IConnectionObserver callback as they are executed by the same
    *           ITransportClient thread that is used to receive the ACK.
    *
    *  @param topic The topic on which the message shall be published
    *  @param msg The message that shall be published
    *  @param timeout Timeout before returning, NOTE that the message could still
    *         be delivered at a later time, even after the timeout.
    *
    *  @return A locally unique identifier of the message that is: \n
    *          > 0: if the message was published and acknowledged by the broker \n
    *          < 0: if the message might have been published and the timeout
    *               was reached before an acknowledgment was received \n
    *          = 0: if the message failed to publish and has been dropped
    *
    ***************************************************************************
    */
   virtual int publish_wait_ack(const std::string& topic,
         const Message& msg,
         const std::chrono::seconds timeout = std::chrono::seconds(10)) = 0;

   /**
    ****************************************************************************
    *
    *  @brief Adds a subscription to the client
    *
    *  @details If the client already subscribes to the given topic, the given
    *           will just be added as an additional observer, unless the
    *           requested QoS is higher than the previously set, in which case
    *           the new QoS will be set.
    *
    *           If the client is connected but not started, subscription will be
    *           sent once it's started.
    *
    *           Announcement of rt/ topics is implicitly handled by the LAPI
    *           and does not need to be performed by the client application.
    *
    *  @note Each rt/ topic must be subscribed to explicitly! MQTT wild cards
    *        (like rt/# rt/+/sig) is NOT allowed since each topic needs to be
    *        announced separately to the IPCd by the LAPI internals.
    *
    *  @param topic The topic to which the client shall subscribe to
    *  @param qos Set QoS for the subscription
    *  @param observer Observer of the subscription
    *
    ***************************************************************************
    */
   virtual void subscribe(const std::string& topic,
         uint8_t qos,
         ISubscriptionObserver_ptr observer) = 0;

   /**
    ***************************************************************************
    *
    *  @brief Removes a subscription observer from the client
    *
    *  @details Removes the observer from the list of subscribers for a specific
    *           topic. If there is no observers left, the topic is unsubscribed
    *           from the client.
    *
    *  @param topic The topic to which the observer is subscribing
    *  @param observer Observer of the subscription
    *
    ***************************************************************************
    */
   virtual void unsubscribe(const std::string& topic,
         ISubscriptionObserver_ptr observer) = 0;

   /**
    ****************************************************************************
    *
    * @brief Add an connection observer to the client
    *
    * @details This methods has to be overridden by derived classes.
    *
    * @param observer The connection observer
    *
    ****************************************************************************
    */
   virtual void add_connection_observer(IConnectionObserver_ptr observer) = 0;

   /**
    ****************************************************************************
    *
    *  @brief Start and run the client: Blocking the current thread
    *
    *  @details Run the client until disconnect() or stop() is called.
    *           An unexpected drop of the connection will trigger
    *           reconnection attempts.
    *
    *           The client will be connected if it isn't already.
    *
    *  @param threadSafe Deprecated. Always runs in threadSafe mode
    *
    ***************************************************************************
    */
   virtual void run(bool threadSafe) = 0;

   /**
    ****************************************************************************
    *
    *  @brief Start and run the client: Not blocking the current thread
    *
    *  @details Run the client without blocking the current thread.
    *           Use join() to wait for the thread to finish.
    *           It will run until disconnect() or stop() is called.
    *
    *           An unexpected drop of the connection will trigger
    *           reconnection attempts.
    *
    *           The client will be connected if it isn't already.
    *
    ***************************************************************************
    */
   virtual void start() = 0;

   /**
    ****************************************************************************
    *
    *  @brief Stop the running client thread
    *
    *  @details Terminate the running thread and wait for a clean shutdown
    *           of the connection.
    *           Effectively calls disconnect() followed by join().
    *
    *  @warning Once stopped, the transport client _can not_ be started again!
    *
    ***************************************************************************
    */
   virtual void stop() = 0;

   /**
    ****************************************************************************
    *
    *  @brief Wait for the client thread to finish: Blocking!
    *
    *  @details Wait indefinitely for the thread to finish.
    *           Use disconnect() to trigger a shutdown of the thread.
    *
    *  @warning Once stopped, the transport client _can not_ be started again!
    *
    ***************************************************************************
    */
   virtual void join() = 0;


   /**
    ****************************************************************************
    *
    *  @brief Get the client name for the TransportClient
    *
    *  @return The client name
    *
    ***************************************************************************
    */
   virtual const std::string& client_name() const = 0;

   /**
    ****************************************************************************
    *
    * @brief Set the last will and testament (LWT) to the Client
    *
    * @details The TransportClient supports sending a last will, when it is
    *          unexpectedly disconnected from the server/broker (without
    *          explicitly sending it via a publish call).
    *
    *          The server will therefore keep the last will payload internally
    *          and sends it whenever the specific client connection is
    *          unexpectedly lost (e.g. crash).
    *
    *          This function is used to set the last will for a specific topic.
    *
    * @note This function needs to be called before connect() and before start()
    *
    * @param topic The topic on which the last will should be sent out
    * @param message The payload which should be sent out
    *
    * @return true if success, false otherwise
    *
    ***************************************************************************
    */
   virtual bool set_will(const std::string& topic, const Message& message) = 0;

   /**
    ****************************************************************************
    *
    * @brief Will clear all previously set wills
    *
    * @sa set_will
    *
    * @return true if success, false otherwise
    *
    ***************************************************************************
    */
   virtual bool clear_will() = 0;

protected:
   /**
    * Internal state of a communication client
    */
   enum STATE {
      STATE_UNINITIALIZED, /**< used when nothing has been initialized yet*/
      STATE_INITIALIZED,   /**< used when client has been initialized */
      STATE_DISCONNECTED,  /**< state when client is disconnected */
      STATE_CONNECTED      /**< state used client is connected */
   };


   /**
    ****************************************************************************
    *
    * @brief default constructor, initialize to STATE_UNINITIALIZED
    *
    ****************************************************************************
    */
   ITransportClient() : m_state(STATE_UNINITIALIZED)
   { }

   /**
    ****************************************************************************
    *
    * The state for the internal state-machine of a client
    *
    ****************************************************************************
    */
   STATE m_state;
};

/**
 *==============================================================================
 * Public constant definitions for external access
 *==============================================================================
 */


/**
 *******************************************************************************
 *
 * @brief Convenience type for unique_ptr for external use
 *
 *******************************************************************************
 */
typedef ITransportClient::uptr_t ITransportClient_uptr;

/**
 *******************************************************************************
 *
 * @brief Convenience type for shared_ptr for external use
 *
 *******************************************************************************
 */
typedef ITransportClient::sptr_t ITransportClient_sptr;

/**
 *******************************************************************************
 *
 * @brief Convenience type for shared_ptr for external use (legacy usage)
 *
 *******************************************************************************
 */
typedef ITransportClient_sptr ITransportClient_ptr;

/**
 *==============================================================================
 * Function prototype declarations for external access
 *==============================================================================
 */

/**
 *******************************************************************************
 *
 * @brief Create a default transport client (MQTT)
 *
 * @details Creates a new TransportClient using the supplied configuration
 *
 *          A client instance always requires configuration entries, config parameter.
 *
 * @param config Configuration to start the internal transport client
 *        * "client_name": A string representation of a client-application
 *          default: is not set! This needs to be provided otherwise the config
 *          validation will fail.
 *        * "host": Default: localhost
 *        * "port": Default: 1883 (for MQTT communication)
 *        * "mqtt_version" : Default 3.1, accepts 3.1 & 3.1.1
 *
 * @param transport_type Which transport protocol shall be used for the client,
 *                       default MQTT.
 *
 *******************************************************************************
 */
ITransportClient_ptr create_client(
      config::Configuration_ptr& config,
      TransportType transport_type = TYPE_MQTT);


}}} // namespace

#endif /* CLIENT_H */
