/*
********************************************************************************
********************************************************************************
***
***     File: api/ecu/diag/pgn.h
***
***     Project: Alexandra
***
***     Copyright (C) Stoneridge Electronics AB 1990 - 2018
***
***  The information in this document belongs to Stoneridge, Inc. or its
***  subsidiaries and may be confidential. This document and information
***  in it may not be copied, reproduced, revealed or appropriated, either
***  in whole or in part, without the prior written consent of Stoneridge, Inc.
***
********************************************************************************
********************************************************************************
*/

/**
**==============================================================================
** Header definition
**==============================================================================
*/
#ifndef API_ECU_DIAG_PGN_H_
#define API_ECU_DIAG_PGN_H_

/**
**==============================================================================
** Imported definitions
**==============================================================================
*/
#include <memory>
#include <chrono>
#include <string>

#include <ecu/com/client.h>
#include <ecu/common.h>

/**
 *==============================================================================
 * Public type definitions
 *==============================================================================
 */
namespace ecu {
namespace lapi {
namespace diag {

/**
 *******************************************************************************
 *
 * @brief PGN request API for acquiring a request token for an single, ECU
 * specific, PGN request topic.
 *
 * @details Each application should initialize one com::ITransportClient for all
 * internal communication and pass that client to the factory-function
 * (\ref create_pgn_request_client) of the PgnRequestClient that will then be used
 * for requesting a PGN request token.
 *
 * The PgnRequestClient should be used to acquire a request token for an
 * ECU **before** sending out a single PGN request. A PGN request is normally
 * being published on a real-time topic, for example  "rt/j1939can/tx/rqst_fb_2a".
 * This **token** is only **valid for one single request** for a specific ECU.
 * If an application wants to sends multiple requests to the same ECU, it always
 * has to get the token again.
 *
 * When a PGN request token is requested by an application using the
 * get_request_token() member (with PGN request topic provided), the `pgnreqd`
 * is being contacted which in turn decides if the related ECU (e.g. 'acc') is
 * allowed to be requested, or if another request (e.g. from another application)
 * is still processing.
 *
 * The get_request_token() function is blocking, hence PgnRequestClient will try
 * to acquire the token from pgnregd until the timeout (parameter of
 * get_request_token() ) occurs. In a timeout and other error cases, the
 * returned PgnRequestClient::TokenResult will carry a PgnRequestClient::Error
 * instance which can be evaluated by the application.
 *
 * In case an application decides to not use the just acquired request token,
 * it can explicitly be released using \ref release_token. Further, if an
 * acquired **token isn't used within 10 seconds, it'll implicitly be released**
 * by `pgnreqd`. Hence, if an application immediately sends out a PGN request
 * after acquiring the token, an explicitly \ref release_token is **not
 * required** as the token will be released when the real-time partition has
 * processed the PGN request.
 *
 * ### Example usage
 *
 * @code
 *   #include <chrono>
 *   #include <ecu/com/client.h>
 *   #include <ecu/diag/pgn.h>
 *   #include <ecu/logging.h>
 *
 *   using namespace ecu::lapi;
 *
 *   const std::string pgn_req_topic{"rt/j1939can/tx/rqst_fb_2a"};
 *
 *   // [...]
 *   // config has been create before  ..
 *   //
 *   // create an ITransportClient_ptr
 *   auto tclient = com::create_client(config);
 *
 *   auto pgn_req_client = diag::create_pgn_request_client(tclient);
 *
 *   // Try to get the token, wait 60 seconds for it
 *   auto token_result =
 *       pgn_req_client->get_request_token(
 *          pgn_req_topic,
 *          std::chrono::seconds(60));
 *
 *   if ( token_result.nok() )
 *   {
 *       LOG(ERROR) << "Unable to acquire PGN request token: "
 *          << PgnRequestClient::error_str(token_result.err_val());
 *       // [...]
 *       return;
 *   }
 *
 *   // Token acquired ... continue sending a PGN request using SignalAdapter
 *   // API and publish it on 'pgn_req_topic' with the 'tclient' (see above).
 *
 * @endcode
 *
 *******************************************************************************
 */
class PgnRequestClient
{
public:
   /**
    ****************************************************************************
    *
    * @brief delete copy constructor
    *
    ****************************************************************************
    */
   PgnRequestClient(const PgnRequestClient&) = delete;

   /**
    ****************************************************************************
    *
    * @brief Make dtor virtual to allow inheritance
    *
    ****************************************************************************
    */
   virtual ~PgnRequestClient() {}

   /**
    ****************************************************************************
    *
    * @brief Return error type which indicates that the token couldn't be
    * acquired.
    *
    ****************************************************************************
    */
   enum struct Error {
      Unknown,      ///< Unknown error, application should do another
                    ///< \ref get_request_token
      Timeout,      ///< Timeout occurred, application should probably retry to
                    ///< get token.
      ECU_unknown,  ///< `pgnreqd` wasn't able to relate the given
                    ///< `request_topic` to any ECU - make sure the given topic
                    ///< is a valid PGN request topic (e.g. contains 'tbm2').
   };

   /**
    ****************************************************************************
    *
    * @brief Get string representation of Error
    *
    * @param error The Error to which get the string representation from
    *
    * @return The string representation of the provided error.
    *
    ****************************************************************************
    */
   static const char* error_str(const Error& error);


   /**
    ****************************************************************************
    *
    * @brief Result type used for \ref get_request_token
    *
    * @note If request token could be acquired, TokenResult::ok() is true,
    * otherwise false (vice versa for TokenResult::nok()).
    *
    ****************************************************************************
    */
   using TokenResult = Result<bool, Error>;

   /**
    ****************************************************************************
    *
    * @brief Get the request token for provided PGN `request_topic`.
    *
    * @details The provided request_topic will be used to deduce the ECU to be
    * requested. If the ECU is busy processing another request, the
    * PgnRequestClient will retry to acquire the token until the provided
    * `timeout` occur, hence this will block the calling thread.
    *
    * If a timeout occures, the application can just call \ref get_request_token
    * again.
    *
    * @note Will block until timeout occurred, or token could be acquired.
    *
    * @param request_topic The topic for which to acquire a token for
    * @param timeout Timeout to be used to try to acquire a PGN request token.
    *
    * @return a \ref TokenResult instance indicating if the token could be
    * acquired. In case of error `TokenResult::ok() == false` (also for
    * timeout). Error value for timeouts will be indicated as
    * `TokenResult::err_val() == Error::Timeout`.
    *
    ****************************************************************************
    */
   virtual TokenResult get_request_token(
         const std::string& request_topic,
         std::chrono::seconds timeout) = 0;


   /**
    ****************************************************************************
    *
    * @brief Explicitly release a request token
    *
    * @details In case an application decides that a previously acquired
    * request token isn't needed anymore, this function can be used to
    * explicitly release it.
    *
    * @note The default behaviour for releasing a acquired token is implicit!
    * The token is released after the PGN request for a topic has been sent out
    * and processed successfully by the real-time partition or no PGN request
    * has been sent for 10 seconds after acquiring the token. Hence calling
    * this function isn't required for the standard use-case!
    *
    * @param request_topic The PGN request topic.
    *
    ****************************************************************************
    */
   virtual void release_token(const std::string& request_topic) = 0;

//-----------------------------------------------------------------------------
protected:
   /**
    ****************************************************************************
    *
    * @brief default constructor only accessible from derived classes
    *
    ****************************************************************************
    */
   PgnRequestClient() = default;
};

/**
 ****************************************************************************
 *
 * @brief shared pointer convenience typedef
 *
 ****************************************************************************
 */
using PgnRequestClient_ptr = std::shared_ptr<PgnRequestClient>;

/**
 *******************************************************************************
 *
 *  @brief Create a PGN request client (PgnRequestClient)
 *
 *  @details Creates a new PGN request Client using supplied transport client.
 *
 *  @note This function prevents the PGN client creation on TPA partition,
 *  returning a nullptr instead of the actual client and logs an error on the
 *  journal.
 *
 *  @param tclient An ITransportClient for internal communication
 *
 *  @return PgnRequestClient_ptr to the single instance of the PGN req client if
 *  successful or nullptr otherwise.
 *
 *******************************************************************************
 */
PgnRequestClient_ptr
create_pgn_request_client(com::ITransportClient_ptr& tclient);

}  // namespace diag
}  // namespace lapi
}  // namespace ecu

#endif  // API_ECU_DIAG_PGN_H_

