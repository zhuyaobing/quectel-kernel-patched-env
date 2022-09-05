/*
***************************************************************************************
***************************************************************************************
***
***     File: backend.h
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

#ifndef BACKEND_H_
#define BACKEND_H_

#include <atomic>
#include <functional>
#include <mutex>
#include <ecu/com/client.h>
#include <ecu/com/observer.h>
#include <ecu/com/message.h>

namespace ecu {
namespace lapi {
namespace com {

/**
 ****************************************************************************
 *
 *  @brief Backend Client API for transmitting and receiving data to/from the Backend
 *
 *  @details The Backend Client API supports transmitting and receiving data to and
 *  from the Backend as well as receiving connection state changes.
 *
 *  The Backend Client API allows communication with the Backend through the
 *  Backend proxy. Each application should initialize one ITransportClient
 *  for all the internal communication and pass that client to the constructor of
 *  a BackendClient that will then be used for relaying raw data in the form of
 *  a com::Message with the Backend.
 *
 *  The client can register a callback function to receive changes in the backend
 *  connection state. The current connection state can also be retrieved via the
 *  is_connected() method.
 *
 *  The BackendClient is thread-safe to use if one thread implements the main
 *  business logic and another thread handles the MQTT client observer callbacks.
 *
 *  ### Transmitting data to the Backend
 *
 *  Transmitting data to the backend is done by passing a Message with the raw
 *  data, content ID and TX options to BackendClient::publish(...)
 *  The set of TxOptions is available for the client to customize the handling
 *  of each message in the Backend proxy and it's send buffers.
 *  If desired, the client can register a callback function for each message
 *  that is published. The callback is called by the BackendClient with the
 *  publish-request-ID and a status parameter when the Backend proxy has
 *  processed the message. The callback function can be shared between all
 *  calls to Backend proxy publish. Note that any valid c++11 functional binding
 *  mechanism can be used on the std::function<...> callback object.
 *
 *  ### Receiving data from the Backend
 *
 *  Incoming data will be received by the Backend proxy where the VCP is parsed,
 *  validity checked, payload is extracted and published on the appropriate internal
 *  topics.
 *
 *  There are two ways an application developer can receive data from the backend.
 *  1) Register a callback in the BackendClient API for desired content IDs
 *     Pass any valid c++11 std::function<...> together with a content id to the
 *     add_content_callback(...) method to get a callback when a message is received
 *     for that content ID. The payload format is determined by the content ID
 *     and is deserialized using the PbBackendMessageAdapter.
 *  2) Subscribe to the content ID topic via a ISubscriptionObserver
 *     Add a ISubscriptionObserver to the internal ITransportClient to subscribe
 *     to the desired topic and use the PbInternalMessageAdapter to deserialize
 *     the payload. The payload format is determined by the content ID and
 *     published on a topic with the following format:
 *     Topic:  'app/cid/[content_id]'
 *
 *  Where 1) is the preferred way of implementing a listener.
 *
 * Example usage:
 * @code
 *
 *   // *** SETUP ***
 *   // Setup client
 *   ITransportClient_ptr internal_client = create_client(config);
 *   BackendClient_ptr be_client = create_be_client(internal_client, config);
 *
 *   // Register a connection state callback lambda function
 *   be_client->add_connection_callback( [](BackendClient::ConnectionState cs) {
 *      std::cout << "Backend connection "
 *         << (cs == BackendClient::ConnectionState::CON_CONNECTED ? "established" : "lost") << std::endl;
 *   });
 *
 *
 *   // *** TRANSMITTING DATA TO BACKEND ***
 *   // Create a Message containing a serialized data buffer to send to Backend
 *   pb::RtSignal rt_signal;;
 *   PbBackendlMessageAdapter<pb::RtSignal> adapter;
 *   auto serialize_result = adapter.serialize(rt_signal);
 *   if(serialize_result.ok()) {
 *      com::BackendClient::TxOptions txOptions;
 *
 *      // Set a callback lambda function
 *      txOptions.callback = [](uint64_t rid, int status) {
 *         std::cout << "Message with req_id=" << rid << " processed with status=" << status << std::endl;
 *      };
 *
 *      // OR you can bind a member function in an instantiated object
 *      txOptions.callback = std::bind( &MyRequestHandler::statusResponseReceiver, m_my_request_handler, _1, _2);
 *
 *      // Publish it to the backend using content_id 25
 *      uint64_t requst_id = be_client->publish(serialize_result.val(), 25, txOptions);
 *      LOG(DEBUG) << "Publishing to Backend with request ID=" << request_id;
 *   }
 *
 *   // *** RECEIVING DATA FROM BACKEND ***
 *   // Example of a member content callback handler
 *   class MyContentHandler {
 *       public:
 *       void content25Callback(const uint32_t cid, const Message& msg) {
 *          PbBackendMessageAdapter<pb::RtSignal> adapter;
 *          auto result = adapter.deserialize(msg);
 *          if (result.ok())
 *             std::cout << "Content ID: " << cid
 *                       << " Data: " << result.val().DebugString() << std::endl;
 *       }
 *   };
 *
 *   // Bind a member function in an instantiated object to content ID 25
 *   be_client->add_content_callback(25,
 *      std::bind( &MyContentHandler::content25Callback, m_my_content_handler, _1, _2) );
 *
 *
 * @endcode
 *
 ***************************************************************************
 */

class BackendClient
{
   public:
      /**
       ****************************************************************************
       *
       * Convenience typedef to a shared pointer type of this class
       *
       ****************************************************************************
       */
      typedef std::shared_ptr<BackendClient> ptr_t;

      /**
       ****************************************************************************
       *
       *  @brief Connection state of the Backend (ECU <-> Backend)
       *
       ****************************************************************************
       */
      enum ConnectionState {
         CON_ERROR        = 0,   ///< state when there is an error (incorrect auth etc)
         CON_DISCONNECTED = 1,   ///< state when backend is disconnected
         CON_CONNECTED    = 2    ///< state used backend is connected
      };

      /**
       ****************************************************************************
       *
       *  @brief Typedef of a Backend client callback to receive connection state
       *         changes of the ECU <-> Backend connection.
       *
       *  @param ConnectionState - current connection state
       *
       ****************************************************************************
       */
      typedef std::function<void(ConnectionState)> ConnectionStateCallback_t;

      /**
       ****************************************************************************
       *
       *  @brief Backend Send-Queue Status information
       *
       ***************************************************************************
       */
      class QueueStatus {
         public:
            QueueStatus() :
               size(0),
               available(0),
               in_flight(0),
               kl30robust(0),
               timestamp(0) {}

            //! Number of messages in send queue (total)
            uint32_t size;
            //! Number of available slots left in send queue
            uint32_t available;
            //! Number of messages currently in-flight (sent but not acknowledged yet)
            uint32_t in_flight;
            //! Number of kl30 robust messages in send queue
            uint32_t kl30robust;
            //! Timestamp of last received queue status.
            //! 0 = Never received, all QueueStatus values are then 0
            uint64_t timestamp;
      };

      /**
       ****************************************************************************
       *
       *  @brief Typedef of a Backend client callback to receive queue status changes
       *
       *  @param QueueStatus - current send queue status
       *
       ****************************************************************************
       */
      typedef std::function<void(const QueueStatus&)> QueueStatusCallback_t;

      /**
       ****************************************************************************
       *
       *  @brief Status response from the Backend proxy passed to the ResponseCallback_t
       *
       ***************************************************************************
       */
      enum ResponseStatus
      {
         UNKNOWN = 0, ///< Unkown / no status recevied
         ERROR = 1,   ///< Error occured mesage might be lost
         SENT = 2,    ///< Transmitted to and Acknowledged by backend
         QUEUED = 3,  ///< Saved in send queue for later delivery
         DROPPED = 4  ///< Dropped from send queue, will not be sent
      };

      /**
       ****************************************************************************
       *
       *  @brief Typedef of a Backend client callback to receive status of a
       *  publish request once the Backend proxy has processed the message
       *
       *  @param uint64_t - The request ID, same as returned by the call to publish()
       *  @param ResponseStatus - enum of reported status
       *
       ****************************************************************************
       */
      typedef std::function<void(const uint64_t, ResponseStatus)> ResponseCallback_t;

      /**
       ****************************************************************************
       *
       *  @brief Typedef of a Backend client content callback to be called when
       *  data has been received from the backend on a specific content ID
       *
       *  @param uint32_t - The content ID describing the payload
       *  @param Message - The message containing the serialized payload
       *
       ****************************************************************************
       */
      typedef std::function<void(const uint32_t, const Message&)> ContentCallback_t;

      /**
       ****************************************************************************
       *
       *  @brief Backend Transmission options
       *
       *  @details Supply a set of transmission options when publishing a message
       *           to the Backend
       *
       *  @param priority Priority in the transmission buffer, 1 - 10 (1 is highest)<br>
       *                  Default: 10 (lowest)
       *  @param timeout Seconds the message is left unsent in the transmission buffer
       *                 before being purged. 0 = use configured maximum timeout<br>
       *                 Default: 0
       *  @param kl15robust Message is saved in non-volatile memory (flash) upon a
       *                    clean system shutdown (kl15 / ignition off)<br>
       *                    Default: true
       *  @param kl30robust Message is immediately saved in non-volatile memory (flash)
       *                    before delivery is attempted and kept there until
       *                    backend has acknowledged the transaction.<br>
       *                    Default: false<br>
       *         <b>!ALERT! Use the kl30robust flag sparsely and only in exceptional
       *                    cases as it adds unnecessary wear on the flash! !ALERT!</b>
       *  @param addDriverId The driver id information is requested to be
       *                     included, when it is available, into the message to
       *                     the Backend, if this flag is set as true<br>
       *                     Default: false<br>
       *         <b>NOTE: The driver id information is available when the
       *                  backend proxy daemon receives it from MAN application
       *                  on topic "be/driver_id".</b>
       *  @param callback Optional callback function to receive a status response
       *                  for the publish request once the Backend proxy process it.
       *                  If not supplied, the Backend proxy will not send any response.
       *
       ***************************************************************************
       */
      class TxOptions
      {
         public:
            TxOptions() :
               priority(10),
               timeout(0),
               kl15robust(true),
               kl30robust(false),
               addDriverId(false),
               callback() {}

            int32_t priority;            ///< priority, 1-10, default 10
            int32_t timeout;             ///< timeout (sec), default 0
            bool kl15robust;             ///< kl15 robust flag, default true
            bool kl30robust;             ///< kl30 robust flag, default false
            bool addDriverId;            ///< add driver id flag, default false
            ResponseCallback_t callback; ///< response callback function
      };

      /**
       ****************************************************************************
       *
       * Disallow default constructor
       *
       ****************************************************************************
       */
      BackendClient() =delete;

      /**
       ****************************************************************************
       *
       *  @brief Constructor for a Backend Client for communicating with the Backend
       *
       *  @details Needs to be initialized with an existing ITransportClient,
       *  therefore the default constructor is hidden
       *
       *  @param tclient An initialized internal transport client
       *
       ***************************************************************************
       */
      BackendClient(ITransportClient_ptr& tclient);

      virtual ~BackendClient() {
         // Consider the case of another thread still using the mutex, wait until
         // it is done, then proceed with destruction here
         std::lock_guard<std::recursive_mutex> lock{m_mutex};
      }

      /**
       ****************************************************************************
       *
       *  @brief Get the current Backend connection state (ECU <-> Backend)
       *
       *  @return Returns true if the backend is currently connected, otherwise false
       *
       ****************************************************************************
       */
      virtual bool is_connected();

      /**
       ****************************************************************************
       *
       *  @brief Get the current Backend send queue status (ECU => Backend)
       *
       *  @details Queue status subscription will be enabled upon first call to this
       *  method or add_queue_status_callback(). It is therefore recommended to
       *  call this method once during app initialization to start receiving queue
       *  status updates as they are only published on change.
       *
       *  @note QueueStatus::timestamp will be 0 if no queue status has been received
       *        and all QueueStatus values will therefore be 0
       *
       *  @return reference to last received send queue status
       *
       ****************************************************************************
       */
      virtual QueueStatus queue_status();

      /**
       ****************************************************************************
       *
       * @brief Add a connection callback
       *
       * @details Backend client callback to receive connection state changes
       *          of the ECU <-> Backend connection
       *
       * @param cb the callback function pointer
       *
       ****************************************************************************
       */
      virtual void add_connection_callback(ConnectionStateCallback_t cb);

      /**
       ****************************************************************************
       *
       * @brief Add queue status callback
       *
       * @details Backend client callback to receive backend queue status changes.
       *          @see queue_status()
       *
       * @param cb the callback function pointer
       *
       ****************************************************************************
       */
      virtual void add_queue_status_callback(QueueStatusCallback_t cb);

      /**
       ****************************************************************************
       *
       * @brief Add or rewrite the content callback for specified content ID
       *
       * @details Backend client content callback to be called when data has been
       *          received from the backend on a specific content ID. If a callback
       *          has been specified previously, this method will overwrite the
       *          callback for the given content ID.
       *
       * @param cid - The content ID describing the payload
       * @param cb - The callback function pointer
       *
       ****************************************************************************
       */

      virtual void add_content_callback(const uint32_t cid, ContentCallback_t cb);

      /**
       ****************************************************************************
       *
       * @brief Removes the content callback for a specified content ID
       *
       * @details Warning: this method must NOT be called within a content
       *          callback because this could potentially destroy the context
       *          of the callback currently being executed.
       *
       * @param cid - The content ID describing the payload
       *
       ****************************************************************************
       */
      virtual void clear_content_callbacks(const uint32_t cid);

      /**
       ****************************************************************************
       *
       *  @brief Publish a message to the Backend
       *
       *  @details Publish a data message for transmission to the Backend with a
       *  given content ID and TX options.
       *
       *  @param message The data message that shall be published
       *  @param content_id The content ID of the payload
       *  @param options Set of transmission options for this message
       *
       *  @return A unique (local) identifier of the published message
       *  or 0 if message failed to publish (on the local ITransportClient)
       *
       ***************************************************************************
       */
      virtual uint64_t publish(
            const Message& message,
            const uint32_t content_id,
            const TxOptions& options);

#ifndef DOXYGEN_IGNORE
      /**
       * Internal topic definitions, not to be used by application
       */
      static constexpr const char* TOPIC_PUB = "be/publish";
      static constexpr const char* TOPIC_STATUS = "be/status";
      static constexpr const char* TOPIC_QUEUE_STATUS = "be/queue/status";
      static constexpr const char* TOPIC_CONTENT = "app/cid/";
#endif /* ifndef DOXYGEN_IGNORE */

// -----------------------------------------------------------------------------

   private:
      // These observers have a known lifetime and an internal private scope
      // limited by the parent class. This provides information for using
      // raw non-owning pointers in a safe manner.
      class ResponseObserver : public ISubscriptionObserver
      {
         public:
            ResponseObserver(BackendClient* client, std::recursive_mutex* mutex) :
            m_client(client), m_mutex(mutex)
         { }
         private:
            void message(const std::string& topic, const Message& message) override;
            BackendClient* m_client;
            std::recursive_mutex* m_mutex;
      };

      class StatusObserver : public ISubscriptionObserver
      {
         public:
            StatusObserver(BackendClient* client, std::recursive_mutex* mutex) :
            m_client(client), m_mutex(mutex)
         { }
         private:
            void message(const std::string& topic, const Message& message) override;
            BackendClient* m_client;
            std::recursive_mutex* m_mutex;
      };

      class QueueStatusObserver : public ISubscriptionObserver
      {
         public:
            QueueStatusObserver(BackendClient* client, std::recursive_mutex* mutex) :
            m_client(client), m_mutex(mutex)
         { }
         private:
            void message(const std::string& topic, const Message& message) override;
            BackendClient* m_client;
            std::recursive_mutex* m_mutex;
      };

      class ContentObserver : public ISubscriptionObserver
      {
         public:
         ContentObserver(BackendClient* client, std::recursive_mutex* mutex) :
            m_client(client), m_mutex(mutex)
         { }
         private:
            void message(const std::string& topic, const Message& message) override;
            BackendClient* m_client;
            std::recursive_mutex* m_mutex;
      };

// -----------------------------------------------------------------------------

      const std::string m_client_name;
      ITransportClient_ptr m_int_client;
      std::recursive_mutex m_mutex;

      ISubscriptionObserver_ptr m_response_obs;
      typedef std::map<uint64_t, ResponseCallback_t> ResponseCallbackMap_t;
      ResponseCallbackMap_t m_response_callbacks;

      ISubscriptionObserver_ptr m_status_obs;
      typedef std::vector<ConnectionStateCallback_t> ConnectionStateCallbackSet_t;
      ConnectionStateCallbackSet_t m_constate_callbacks;

      ISubscriptionObserver_ptr m_queue_status_obs;
      typedef std::vector<QueueStatusCallback_t> QueueStatusCallbackSet_t;
      QueueStatusCallbackSet_t m_queue_status_callbacks;

      ISubscriptionObserver_ptr m_content_obs;
      typedef std::map<uint32_t, ContentCallback_t> ContentCallbackMap_t;
      ContentCallbackMap_t m_content_callbacks;

      ConnectionState m_con_state;
      QueueStatus m_queue_status;
      std::atomic_flag m_queue_status_enabled;
      std::atomic<uint64_t> m_next_tx_id; /// Counter for sent messages, use as req_id

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
typedef BackendClient::ptr_t BackendClient_ptr;

/**
 *******************************************************************************
 *
 *  @brief Create a Backend Client
 *
 *  @details Creates a new Backend Client using the supplied internal Transport client
 *  Note that there can be only one Backend client per TransportClient
 *
 *  @param tclient An ITransportClient for connection to the Backend Proxy
 *
 *  @return BackendClient_ptr to the single instance of the Backend client
 *
 *******************************************************************************
 */
BackendClient_ptr
create_be_client(ITransportClient_ptr& tclient);

}}} // namespace

#endif /* BACKEND_H_ */
