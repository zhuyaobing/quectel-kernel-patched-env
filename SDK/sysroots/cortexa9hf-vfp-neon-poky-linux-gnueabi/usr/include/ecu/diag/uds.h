#ifndef UDS_H
#define UDS_H

/*
 *******************************************************************************
 *******************************************************************************
 ***
 ***     File: uds.h
 ***
 ***     Project: Alexandra
 ***
 ***     Copyright (C) Stoneridge Electronics AB 1990 - 2017
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

#include <future>
#include <condition_variable>
#include <vector>

#include <ecu/com/client.h>
#include <ecu/common.h>

namespace ecu {
namespace lapi {

/**
 *******************************************************************************
 *
 * @brief Contains diagnostic/UDS related artifacts
 *
 *******************************************************************************
 */
namespace diag {

/**
 *******************************************************************************
 *
 * @brief Configuration class for a UDS interface allocation
 *
 * @details When a UDS interface should be allocated using
 * UdsClient::allocate_interface*() an instance of this class need to be
 * provided. This is used to select a UdsTransportProtocol and UdsInterface
 * (physical) which shall be used for subsequent UdsRequests. Along with the
 * transport protocol and the interface type, a target address needs to be
 * provided. For the time of the interface allocation, the instance of a
 * UdsClient is able to use the interface for the specified ECU, transport
 * protocol and physical interface.
 *
 *******************************************************************************
 */
struct UdsInterfaceAllocationConfig {
   /**
    * @brief The UDS transport protocol to be used for the UDS interface
    * allocation
    */
   enum UdsTransportProtocol {
      UDS_TRANSPORT_PROTOCOL_UNKNOWN = 0, ///< Nothing specified, selecting this
                                          ///< will lead to an error when
                                          ///< allocating the interface
      UDS_TRANSPORT_PROTOCOL_ISO,   ///< ISO_TP (regular UDS over CAN)
   } protocol; ///< The transport protocol to be used for the UDS interface
               ///< allocation

   /**
    * @brief The physical interface to be used for the UDS interface allocation
    *
    * @details Select on which CAN port the UDS requests will be sent
    */
   enum UdsInterface {
      UDS_INTERFACE_UNKNOWN = 0, ///< Nothing specified, selecting this will
                                 ///< lead to an error when allocation the
                                 ///< interface
      UDS_INTERFACE_CAN1,        ///< CAN port 1
      UDS_INTERFACE_CAN2,        ///< CAN port 2
   } interface; ///< The physical interface to be used for the allocation

   /**
    * @brief The ECU target address
    */
   uint32_t target;
};

/**
 *******************************************************************************
 *
 * @brief Used for UDS request and response
 *
 * @details Both a UDS request and response is represented with this class.
 * In case of a request, the UdsMessage::address should be the target address
 * of the request.
 *
 * In case of a UDS response, the UdsMessage::address represents a source
 * address of the response
 *
 * UdsMessage::data represents the raw data that of either request or response
 *
 *******************************************************************************
 */
struct UdsMessage
{
   /**
    * @brief the target or source address of the request or response
    *
    * @details In case of a response, the address need to match the
    * UdsInterfaceAllocationConfig::target field used during the allocation
    * process.
    */
   uint32_t address;

   /**
    * @brief The raw data.
    */
   std::vector<uint8_t> data;
};

/**
 *******************************************************************************
 *
 * @brief Used to set UDS client parameters
 *
 * @details All variable UDS parameters can be adjusted through this class.
 *
 *******************************************************************************
 */
struct UdsParameters
{
#ifndef DOXYGEN_IGNORE
   /**
    * Default UDS request timeout value
    */
   static constexpr uint32_t UDS_REQUEST_TIMEOUT_MS = 150000;
#endif /* ifndef DOXYGEN_IGNORE */

   /**
    * @brief STMin for response
    *
    * @details Inter message delay for CAN transport to be used in responses
    * Value should be between 0 and 255
    * 0 - no delay
    * 255 | 0xFF - let the protocol decide what to use
    * Default: 0xFF
    */
   uint8_t st_min_res{0xFF};

   /**
    * @brief STMin for request
    *
    * @details Inter message delay for CAN transport to be used in responses
    * Value should be between 0 and 255
    * 0 - no delay
    * 255 | 0xFF - Default value from system parameter will be used
    * Default: 0xFF
    */
   uint8_t st_min_req{0xFF};

   /**
    * @brief Block size for response
    *
    * @details Block size for CAN transport to be used in responses
    * Value should be between 0 and 4095
    * 0xFFFF - Default value from system parameter will be used
    * Default: 0xFF
    */
   uint16_t block_size_res{0xFFFF};

   /**
    * @brief P2 Timeout (ms)
    *
    * @details Timeout value for the server to start with the response message
    * after the reception of a request message
    *
    * Default: 150ms
    * It will use default for 0 or values greater than 30000ms
    */
   uint32_t p2_timeout_ms{150};

   /**
    * @brief P2* Timeout (ms)
    *
    * @details Timeout value for the server to start with the response message
    * after the transmission of a negative response message with response pending
    * (NRC 78)
    *
    * Default: 5000ms
    * It will use default for 0 or values greater than 30000ms
    */
   uint32_t p2star_timeout_ms{5000};

   /**
    * @brief Transaction timeout
    *
    * @details A guard timeout in ms to make sure the call is not
    * blocked forever. RT partition will use this value to handle UDS request
    * timeout: default 150000
    */
   uint32_t timeout_ms{UDS_REQUEST_TIMEOUT_MS};

   /**
    * @brief Source address
    *
    * @details Source address to be used in UDS requests
    *
    * Defaults:
    * VCM (all variants): 0xFF (will use 0xF0 as source address)
    * TBM3 (all variants but NHMR): 0xFF (will use 0xFB as source address)
    * TBM3 NHMR: It must be explicitly set to 0xF0 or 0xF1
    */
   uint32_t src_address{0xFF};

   /**
    * @brief Maximum response length
    *
    * @details Maximum allowed length for a response
    *
    * Minimum length is 1
    * 4096 or greater: It will use default determined by system parameter
    * Default: 4096
    */
   uint32_t max_len_resp{4096};

};



/**
 *******************************************************************************
 *
 * @brief States of the UDS interface in respect to the user of the interface
 *
 * @details The States indicated are always user specific. For example, a
 * UDS_IF_STATE_READY indicated by the UDS interface means that the current
 * instance of the UDSClient is able to issue request towards the interface.
 * A UDS_IF_STATE_BUSY means that the UDS interface is currently use by
 * another client, i.e. another UdsClient instance in another Application
 * running on the platform.
 *
 *******************************************************************************
 */
enum UdsInterfaceState
{
   UDS_IF_STATE_FREE,       ///< UDS Interface is free to be allocated by a
                            ///< client
   UDS_IF_STATE_READY,      ///< UDS Interface is allocated to current user
                            ///< and request can be issued
   UDS_IF_STATE_BUSY,       ///< UDS Interface is currently allocated by an
                            ///< other client
   UDS_IF_STATE_TIMEOUT,    ///< Allocation timeout for current user
   UDS_IF_STATE_PROCESSING, ///< Interface is allocated by current user and
                            ///< a request is currently processing
   UDS_IF_STATE_UNKNOWN,    ///< Interface could not be determined:
                            ///< initialized, connection error, serialization
                            ///< issue ?
};


/**
 *******************************************************************************
 *
 * @brief Error related to an UdsResponse
 *
 *******************************************************************************
 */
enum UdsResponseError
{
   UDS_RESPONSE_OK,            ///< UDS response received correctly
   UDS_RESPONSE_ERR_TIMEOUT,   ///< UDS request timed out, response was not
                               ///< received in time
   UDS_RESPONSE_ERR_BUSY,      ///< UDS interface is allocated by another
                               ///< UDS client
   UDS_RESPONSE_ERR_NOT_ALLOC, ///< Request was issued but the interface
                               ///< not been allocated prior to the request
   UDS_RESPONSE_ERR_RT,        ///< An error was indicated from the RealTime
                               ///< partition. Interface will stay allocated!
   UDS_RESPONSE_ERR_UNKNOWN,   ///< Other error
};


/**
 *******************************************************************************
 * @brief Status for UDS request call in async case
 *
 * @details Will be used as return value when calling
 * UdsClient::send_request_async.
 *
 *******************************************************************************
 */
enum UdsRequestStatus
{
   UDS_REQUEST_OK,                  ///< The Request was send correctly
   UDS_REQUEST_NOT_ALLOCATED,       ///< Connection to the UDS interface handler
                                ///< has not been established correctly. Did
                                ///< UdsClient allocation succeed?
   UDS_REQUEST_SERIALIZATION_ERROR, ///< Uds Request serialization error
   UDS_REQUEST_UNKNOWN_ERROR        ///< An unknown error occurred
};


/**
 *******************************************************************************
 * @brief FlowStatus definition of a flow control request
 *******************************************************************************
 */
enum FlowStatus
{
   UDS_FS_CTS      = 0, /// Clear to send
   UDS_FS_WAIT     = 1, /// Wait: Currently not supported
   UDS_FS_SUPPRESS = 2, /// Supress Frame
   UDS_FS_OVERFLOW = 3, /// Overflow: Use to terminate current transaction
};

/**
 *******************************************************************************
 *
 * @brief The observer interface for a UdsClient in an async usage of the UdsClient
 *
 * @details All UdsClient callbacks will be handled via this interface. Therefore
 * applications should provide an implementation of IUdsClientObserver interface
 * to the UdsClient. Note that this is required if the UdsClient is used
 * asynchronously (all *_async functions of the UdsClient).
 *
 * All callbacks will be handled asynchronously which need to be considered
 * during the interface implementation.
 *
 * See UdsClient for more information
 *
 *******************************************************************************
 */
class IUdsClientObserver
{
public:

   /**
    ****************************************************************************
    *
    * @brief Make dtor virtual to allow inheritance
    *
    ****************************************************************************
    */
   virtual ~IUdsClientObserver() { }

   /**
    ****************************************************************************
    *
    * @brief Callback function for UdsClient connection state changes
    *
    * @details The callback will indicate if the application has successfully
    * granted access to the UDS interface by providing UDS_IF_STATE_READY, in
    * case an interface allocation was issued.
    * In a negative case one of the other UdsInterfaceState values
    * will be indicated in the callback.
    *
    * All connection state changes will be indicated via this function, e.g.
    * timeout of an allocation.
    *
    * @param state The state change of the UdsClient connection.
    *
    ****************************************************************************
    */
   virtual void on_connection_change(UdsInterfaceState state) = 0;


   /**
    ****************************************************************************
    *
    * @brief Callback function to indicate Uds Response to previously issued
    * Uds Request.
    *
    * @details In case a Request has been issued, timeouts can occur from
    * either the UDS daemon or from the UDS interface as such (CAN - RT). This
    * will be indicated in repsonse.state().
    *
    * A Request timeout will not release the UDS interface allocation, however,
    * it will stop the ongoing request.
    *
    * @param response The UDS response to a previously issued request.
    *
    ****************************************************************************
    */
   virtual void on_response(UdsMessage response) = 0;
protected:
private:
};

/**
 *******************************************************************************
 *
 * @brief Raw UDS Client API for sending diagnostic requests on CAN
 *
 * The UdsClient reuses an existing ITransportClient instance. Thus, this
 * has to be created and initialized beforehand. The ITransportClient instance
 * is used for internal communication only - request, response towards the UDS
 * request handler.
 *
 * The UdsClient should be instantiated with the create_uds_client factory
 * function to create a shared_ptr wrapped UdsClient instance.
 * A IUdsClientObserver instance can be passed in to receive callbacks, however
 * this is not required in which case nullptr (default) should be used.
 *
 * The UdsClient provides two different ways of communicating to the UDS
 * daemon on the platform: synchronously and asynchronously.
 *
 * In the **sync** case, allocate_interface, send_request and
 * release_interface should be used. All calls will block until an answers from
 * the UDS daemon has been received or the provided timeout fire (parameter).
 *
 * In the **async** case, allocate_interface_async, send_request_async and
 * release_interface_async should used. Instead of blocking until an answers
 * has been received, the registered IUdsClientObserver will be called back -
 * see IUdsClientObserver for more details.
 *
 * It is recommended to not mix sync and async calls. However, an observer can
 * still be registered, even though the sync functions will be used.
 *
 * Since the platform only allows exclusive access to the UDS interface,
 * the UdsClient needs to request/allocate access to the UDS interface using
 * allocate_interface() (or async). In the sync case, the function will
 * block and return when an answer has been received. In the async case, the
 * ctor provided IUdsClientObserver will be called back to indicate if the
 * interface allocation succeeded,
 * @ref IUdsClientObserver::on_connection_change().
 *
 * The allocation as such should be done using an instance of
 * UdsInterfaceAllocationConfig. This will allocate a UDS interface for the
 * specified physical interface (e.g. CAN1), a transport protocol (e.g. ISO_TP)
 * and a specific ECU address. Any UDSRequest sent out after the allocation
 * will be directed to towards the ECU on the given communication channel. Other
 * application which want to use this communication channel configuration,
 * including the ECU address, will not be able to allocate the interface.
 *
 * If the allocation was successful, the UdsClient is allowed to send UDS
 * requests towards the interface, therefore the
 * send_request() or  send_request_async() should be used. Responses to such
 * requests will, again, be indicated via a callback
 * towards the registered IUdsClientObserver instance, using the
 * IUdsClientObserver::on_response() function in the async execution. In the
 * sync case it will block until the response, error or timeout has been
 * received. The user of the UdsClient need to handle the negative responses
 * accordingly, via UdsResponseError.
 *
 * UDS requests should be send in raw CAN data format. Therefore the application
 * need to handle all session related messages itself, including e.g.
 * `TesterPresent` messages.
 *
 * When the client application has finished its UDS requests, it should
 * release the interface, in order to allow other applications to use UDS.
 * For that, the applications is required to call release_interface().
 *
 * Timeouts are distinguished between allocation and request.
 *
 * 1. If the interface allocation has been started by a UdsClient but no
 * UDS request has been sent yet, an allocation timeout will be indicated via
 * IUdsClientObserver::on_connection_change carrying a
 * UDS_IF_STATE_TIMEOUT state.
 *
 * 2. If the interface has been allocated and a request has been sent, a
 * timeout related to the request will be indicated via
 * IUdsClientObserver::on_response carrying
 * UdsResponseError::UDS_RESPONSE_ERR_TIMEOUT. Also, the interface will be
 * set back to UdsInterfaceState::UDS_IF_STATE_READY using the
 * on_connection_change function.
 *
 * Note: If no request is outstanding (waiting for a response) and a certain
 * timeout has been reached, the UDS interface will be released from the UDS
 * request handler, allowing other applications to initiate UDS requests:
 * IUdsClientObserver::on_connection_change() with
 * UDS_IF_STATE_TIMEOUT. When the API is used synchronously, the
 * user will take notice of such timeout on the next request.
 *
 * __Sync example usage__
 * @code
 *   #include <ecu/com/client.h>
 *   #include <ecu/diga/uds.h>
 *
 *   using namespace ecu::lapi;
 *   // .....
 *
 *   // ITransportClient_ptr
 *   auto tclient = com::create_client(config);
 *
 *   auto client = diag::create_uds_client(tclient);
 *
 *   UdsInterfaceAllocationConfig alloc_config{
 *       UdsInterfaceAllocationConfig::UDS_TRANSPORT_PROTOCOL_ISO,
 *       UdsInterfaceAllocationConfig::UDS_INTERFACE_CAN1,
 *       0xEE,
 *   };
 *
 *   auto result = client->allocate_interface(alloc_config);
 *   if ( result != UdsInterfaceState::UDS_IF_STATE_READY )
 *   {
 *      // error handling
 *   }
 *
 *   // make request
 *   UdsMessage request;
 *   request.address = 0xEE;
 *   request.data = { 0x23, 0x83, 0x24 }; // raw can data
 *
 *   auto response = client->send_request(request);
 *   if ( response.nok() )
 *   {
 *      // something was wrong
 *   }
 *
 *   client->release_interface();
 *
 * @endcode
 *
 * __Parallel UDS allocations from the same application__
 *
 * In order to execute parallel UDS requests from the same application (using
 * the same ITransportClient) towards different ECUs (or a different TP/
 * physical interface) an application developer needs to instantiate two
 * distinct instances of a UdsClient. Each UdsClient can only handle one UDS
 * interface allocation. The application developer therefore need to guarantee
 * that UDSRequests are sent on the correct UdsClient instance - UDS
 * response will be routed to the correct UdsClient instance automatically.
 *
 *******************************************************************************
 */
class UdsClient
{
public:
#ifndef DOXYGEN_IGNORE
   /**
    * Internal topic definitions, not to be used by application
    */
   static constexpr const char* TOPIC_UDS_ALLOC    = "system/udsifd/allocate";
   static constexpr const char* TOPIC_UDS_RELEASE  = "system/udsifd/release";
   static constexpr const char* TOPIC_UDS_FLOWCTRL = "system/udsifd/flowctrl";
   static constexpr const char* TOPIC_UDS_WITHDRAW = "system/udsifd/withdraw";
   static constexpr const char* TOPIC_UDS_REQ      = "system/udsifd/request";
   //
   // responses to client will be published on:
   //    app/[clientname]/uds/status
   //    app/[clientname]/uds/response
   //

#endif /* ifndef DOXYGEN_IGNORE */


   /**
    ****************************************************************************
    *
    * Convenience typedef to a shared pointer type of this class
    *
    ****************************************************************************
    */
   using ptr_t =  std::shared_ptr<UdsClient>;

   /**
    ****************************************************************************
    *
    * @brief Delete copy constructor
    *
    ****************************************************************************
    */
   UdsClient(const UdsClient&) = delete;

   /**
    ****************************************************************************
    *
    * @brief Default destructor, allows inheritance
    *
    ****************************************************************************
    */
   virtual ~UdsClient() = default;

   /**
    ****************************************************************************
    *
    * @brief Attempt allocate exclusive access to the UDS interface, synchronously
    *
    * @details To be able to send out UDS requests, the UdsClient instance need
    * to be granted exclusive access to the UDS interface using this function.
    *
    * This call will block until an allocation response has been received.
    *
    * Note: If the allocation succeeded and all request/response has been
    * executed, the interface should be released explicitly,
    * using release_interface().
    *
    * If the allocation failes with BUSY or TIMEOUT, the request will be
    * queued at the udsifd and will be allocated when possible, unless udsifd
    * internal allocation timeout expires. If the allocation failed and
    * the client do not want a later allocation to take place it should withdraw
    * the interface allocation request via withdraw_interface().
    *
    * @param alloc_config The allocation configuration to be used for the UDS
    * session.
    * @param timeout_ms A guard timeout in ms to make sure the call is not
    * blocked forever.
    *
    * @return UDS_IF_STATE_READY in case the allocation succeeded and any of the
    * other enum values in case of error.
    *
    * @sa allocate_interface_async
    *
    ****************************************************************************
    */
   virtual UdsInterfaceState allocate_interface(
         const UdsInterfaceAllocationConfig& alloc_config,
         uint32_t timeout_ms = UdsParameters::UDS_REQUEST_TIMEOUT_MS) = 0;

   /**
    ****************************************************************************
    *
    * @brief Attempt allocate exclusive access to the UDS interface
    *
    * @details To be able to send out UDS requests, the UdsClient instance need
    * to be granted exclusive access to the UDS interface using this function.
    * The response to the allocation attempt is signaled back to the registered
    * IUdsClientObserver using IUdsClientObserver::on_connection_change().
    *
    * The allocation request is asynchronously.
    *
    * Note: If the allocation succeeded and all request/response has been
    * executed, the interface should be release explicitly using
    * release_interface_async().
    *
    * If the allocation failes with BUSY or TIMEOUT, the request will be
    * queued at the udsifd and will be allocated when possible, unless udsifd
    * internal allocation timeout expires. If the allocation failed and
    * the client do not want a later allocation to take place it should withdraw
    * the interface allocation request via withdraw_interface().
    *
    * @param alloc_config The allocation configuration to be used for the UDS
    * session.
    *
    * @return True if the allocation request was send correctly otherwise false
    *
    * @sa allocate_interface
    *
    ****************************************************************************
    */
   virtual bool allocate_interface_async(
         const UdsInterfaceAllocationConfig& alloc_config) const = 0;

   /**
    ****************************************************************************
    *
    * @brief Release the UDS interface allocation (sync)
    *
    * @details Will release the current client from the UDS interface
    * allocation if it currently owns it, otherwise the request will fail with
    * BUSY and instead withdraw_interface() should be used to force withdraw
    * any pending interface allocations for current client.
    *
    * At the end of each UDS session, one should always release the interface
    * via this function or the asynchronous version release_interface_async().
    *
    * This function is blocking until the UDS daemon does response
    * or the timeout_ms occurs
    *
    * @param timeout_ms A guard timeout in ms to make sure the call is not
    * blocked forever.
    *
    * @return true if the release succeeded
    *
    ****************************************************************************
    */
   virtual bool release_interface(uint32_t timeout_ms = 5000) = 0;

   /**
    ****************************************************************************
    *
    * @brief Release exclusive access to the UDS interface
    *
    * @details Will release the current client from the UDS interface
    * allocation if it currently owns it, otherwise the request will fail with
    * BUSY and instead withdraw_interface() should be used to force withdraw
    * any pending interface allocations for current client.
    *
    * At the end of each UDS session, one should always release the interface
    * via this function or the synchronous version release_interface().
    * This will lead to connection state change callback to the registered
    * IUdsClientObserver.
    *
    * @return True if the release request was send correctly otherwise false
    *
    ****************************************************************************
    */
   virtual bool release_interface_async() const = 0;

   /**
    ****************************************************************************
    *
    * @brief Withdraw pending UDS interface allocation requests (sync)
    *
    * @details Will withdraw all pending allocation requests for the specific
    * UDS interface issued from this client. If the interface is allocated or
    * in use by the current or other client it will not be withdrawn as an
    * explicit release is then still needed.
    * This function is blocking until the UDS daemon respond
    * or the timeout_ms occurs
    *
    * @param timeout_ms A guard timeout in ms to make sure the call is not
    * blocked forever.
    *
    * @return true if the withdraw succeeded
    *
    ****************************************************************************
    */
   virtual bool withdraw_interface(uint32_t timeout_ms = 5000) = 0;

   /**
    ****************************************************************************
    *
    * @brief Withdraw pending UDS interface allocation requests
    *
    * @details Will withdraw all pending allocation requests for the specific
    * UDS interface issued from this client. If the interface is allocated or
    * in use by the current or other client it will not be withdrawn as an
    * explicit release is then still needed.
    *
    * @return True if the withdraw request was send correctly otherwise false
    *
    ****************************************************************************
    */
   virtual bool withdraw_interface_async() const = 0;

   /**
    ****************************************************************************
    *
    * @brief Result type for a synchronous Request send
    *
    * @details The error type is set to InterfaceState to figure out what has
    * been received when error is indicated
    *
    ****************************************************************************
    */
   typedef Result<UdsMessage, UdsResponseError> RequestResult;

   /**
    ****************************************************************************
    *
    * @brief Send a UDS request synchronously
    *
    * @details This function call will block until a response has been received
    * from the UDS interface daemon. The timeout can be used make sure the
    * whole thread is not blocking forever since the UDS daemon did not answer.
    *
    * Timeouts related to a request-response will be handled internally. If
    * the UDS interface daemon is not responding within specified timeout, the
    * request will be canceled. The default timeout takes into account that the
    * RT partition will handle _response pending_ communication for a certain
    * amount of time, before signaling it back to telematics linux.
    *
    * @param request The request to be send to the UDS interface
    *
    * @param params Parameters to be set for this UDS request
    *
    * @return The RequestResult which indicates if the request resulted in a
    * response (result.ok()) or not in which case the Error code is provided.
    *
    ****************************************************************************
    */
   virtual RequestResult
   send_request(
         const UdsMessage& request,
         const UdsParameters& params) = 0;

   /**
    ****************************************************************************
    * @deprecated @ref send_request (UdsMessage&, UdsParameters&) should be used.
    *
    * @brief DEPRECATED Send a UDS request synchronously
    *
    * @details
    *
    * **NOTE** the default value of _timeout_ms_ is sufficient and should not
    * be adjusted!
    *
    * @param request The request to be send to the UDS interface
    *
    * @param timeout_ms A guard timeout in ms to make sure the call is not
    * blocked forever. RT partition will use this value to handle UDS request
    * timeout: default 150000
    *
    * @param st_min Inter message delay for CAN transport. Value should be
    * between 0 and 255.
    * * 0 - no delay
    * * 255 | 0xff - let the protocol decide what to use
    *
    * @param src_address Source address to be used in UDS requests
    * Defaults:
    * VCM (all variants): 0xFF (will use 0xF0 as source address)
    * TBM3 (all variants but NHMR): 0xFF (will use 0xFB as source address)
    * TBM3 NHMR: It must be explicitly set to 0xF0 or 0xF1
    *
    * @return The RequestResult which indicates if the request resulted in a
    * response (result.ok()) or not in which case the Error code is provided.
    *
    ****************************************************************************
    */
   virtual RequestResult
   send_request(
         const UdsMessage& request,
         uint32_t timeout_ms = UdsParameters::UDS_REQUEST_TIMEOUT_MS,
         uint8_t st_min = 0xFF,
         uint32_t src_address = 0xFF) = 0;

   /**
    ****************************************************************************
    *
    * @brief Send a request on the UDS interface.
    *
    * @details To be able to send out UDS requests, the UdsClient need to be
    * granted access to the UDS interface. The return value will indicate if
    * has been sent. For example, if the UdsClient has not allocated the UDS
    * interface, UDS_REQUEST_NOT_ALLOCATED will be returned.
    *
    * @param request The request to be send to the UDS interface
    *
    * @param params Parameters to be set for this UDS request
    *
    * @return UdsRequestStatus to indicate if the UDS request has been send.
    *
    ****************************************************************************
    */
   virtual UdsRequestStatus send_request_async(
         const UdsMessage& request,
         const UdsParameters& params) = 0;

   /**
    ****************************************************************************
    * @deprecated @ref send_request_async (UdsMessage&, UdsParameters&) should be used.
    *
    * @brief DEPRECATED Send a UDS request synchronously
    *
    * @param request The request to be send to the UDS interface
    *
    * @param timeout_ms A guard timeout in ms to make sure the call is not
    * blocked forever. RT partition will use this value to handle UDS request
    * timeout: default 150000
    *
    * @param  st_min Inter message delay for CAN transport. Value should be
    * between 0 and 255.
    * * 0 - no delay
    * * 255 | 0xFF - let the protocol decide what to use
    *
    * @param src_address Source address to be used in UDS requests
    * Defaults:
    * VCM (all variants): 0xFF (will use 0xF0 as source address)
    * TBM3 (all variants but NHMR): 0xFF (will use 0xFB as source address)
    * TBM3 NHMR: It must be explicitly set to 0xF0 or 0xF1
    *
    * @return UdsRequestStatus to indicate if the UDS request has been send.
    *
    ****************************************************************************
    */
   virtual UdsRequestStatus send_request_async(
         const UdsMessage& request,
         uint32_t timeout_ms = UdsParameters::UDS_REQUEST_TIMEOUT_MS,
         uint8_t st_min = 0xFF,
         uint32_t src_address = 0xFF) = 0;

   /**
    ****************************************************************************
    *
    * @brief Send a flow control request on the UDS interface.
    *
    * @return UdsRequestStatus to indicate if the UDS request has been send.
    *
    ****************************************************************************
    */
   virtual UdsRequestStatus send_flowctrl_async(
      uint8_t    st_min = 0xFF,
      uint8_t    st_min_res = 0xFF,
      uint16_t   block_size_res = 0xFFFF,
      FlowStatus flow_status_res = FlowStatus::UDS_FS_CTS) = 0;

protected:
   /**
    ****************************************************************************
    *
    *  @brief Constructor for the UdsClient
    *
    *  @details Needs to be initialized with an existing ITransportClient,
    *  therefore the default constructor is deleted
    *
    *  @param tclient An initialized internal transport client
    *  @param observer The IUdsClientObserver that should be called on responses
    *  and connection state changes
    *
    ****************************************************************************
    */
   UdsClient(com::ITransportClient_ptr& tclient,
      IUdsClientObserver* observer = nullptr) {};

   /**
    ****************************************************************************
    *
    * @brief default constructor only accessible from derived classes
    *
    ****************************************************************************
    */
   UdsClient() = default;
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
typedef UdsClient::ptr_t UdsClient_ptr;

/**
 *******************************************************************************
 *
 *  @brief Create a UdsClient_ptr
 *
 *  @details Creates a new UdsClient using the supplied ITransportClient.
 *  Create multiple UdsClient instances using the same tclient to support
 *  parallel UDS queries (to different ECUs) from inside the same application.
 *
 *  @note This function prevents the UDS client creation on TPA partition,
 *  returning a nullptr instead of the actual client, and logs an error on the
 *  journal.
 *
 *  @param tclient An ITransportClient for internal communication
 *  @param observer The observer instance used by the UdsClient to be
 *  instantiated. This can be empty, which means no observer will be registered.
 *
 *  @return UdsClient_ptr to the shared_ptr instance of the UdsClient if
 *  successfull or nullptr otherwise.
 *
 *******************************************************************************
 */
UdsClient_ptr
create_uds_client(
      com::ITransportClient_ptr& tclient,
      IUdsClientObserver* observer = nullptr);

}}}

#endif /* UDS_H */
