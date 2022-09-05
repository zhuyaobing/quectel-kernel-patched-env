#ifndef MAIN_REQ_H
#define MAIN_REQ_H

/*
 *******************************************************************************
 *******************************************************************************
 ***
 ***     File: main_req.h
 ***
 ***     Project: Alexandra
 ***
 ***     Copyright (C) Stoneridge Electronics AB 1990 - 2017
 ***
 *******************************************************************************
 *******************************************************************************
 ***
 ***
 ***   Subject: LAPI Maintenance Request interface
 ***
 ***   Document reference:
 ***
 ***   Description of this header file:
 ***
 ***   Version:
 ***
 ***
 *******************************************************************************
 */

#include <mutex>
#include <condition_variable>

#include <ecu/com/client.h>
#include <ecu/common.h>

namespace ecu {
namespace lapi {
namespace diag {


/**
 *******************************************************************************
 *
 * @brief Client for executing maintenance requests
 *
 * @details An application interested in executing maintenance requests
 * should instantiate a MaintenanceRequestClient.
 *
 * The MaintenanceRequestClient should be instantiated using the
 * ecu::lapi::diag::create_main_req_client factory function which creates a
 * shared_ptr of the MaintenanceRequestClient.
 *
 * When the client has been instantiated, an application can create a
 * MaintenanceRequestClient::Request object and use request() to send the
 * request out on CAN. The request function is blocking for the specified
 * timeout_ms or until a response (MaintenanceRequestClient::Response) has been
 * received. In case of a timeout, the result of request() will indicate an
 * error (nok() == true).
 *
 * _Example usage__
 * @code
 *   #include <ecu/com/client.h>
 *   #include <ecu/diag/main_req.h>
 *
 *   using namespace ecu::lapi;
 *   // .....
 *
 *   // ITransportClient_ptr
 *   auto tclient = com::create_client(config);
 *
 *   auto client = diag::create_main_req_client(tclient);
 *
 *   MaintenanceRequestClient::Request req{ /\* data *\/ };
 *
 *   // use default timeout of 5000ms
 *   auto result = client->request(req);
 *
 *   if ( result.nok() )
 *   {
 *      // error handling: timeout or serialization issue
 *      // LOG(ERROR) << result.err_val();
 *      // ...
 *
 *   }
 *
 *   // get instance of MaintenanceRequestClient::Response from result
 *   auto response = result.take_val();
 *
 *   // handle received response
 *
 * @endcode
 *
 *******************************************************************************
 */
class MaintenanceRequestClient
{
public:

#ifndef DOXYGEN_IGNORE
   /**
    * Internal topic definitions, not to be used by application
    */
   static constexpr const char* TOPIC_MAINT_REQ  = "rt/mancan/tx/fws_fb";
   /**
    * Internal topic definitions, not to be used by application
    */
   static constexpr const char* TOPIC_MAINT_RESP = "rt/mancan/rx/fws_27";
#endif /* ifndef DOXYGEN_IGNORE */

   /**
    ****************************************************************************
    *
    * The Request object of a MaintenanceRequest
    *
    ****************************************************************************
    */
   struct Request {
      uint8_t aaf;                  ///< aaf
      uint8_t cid;                  ///< cid
      uint8_t abfragezws;           ///< abfragezws
      uint8_t abfragetype;          ///< abfragetype
      uint8_t einheitdaten;         ///< einheitdaten
      uint8_t qualitaet_empfohlen;  ///< qualitaet_empfohlen
      uint8_t qualitaet_vorhanden;  ///< qualitaet_vorhanden
      uint8_t reihenfolge;          ///< reihenfolge
      uint8_t typmeldung;           ///< typmeldung

      /**
       * Check for equality
       */
      bool operator==(const Request& rhs) const;
   };

   /**
    ****************************************************************************
    *
    * The Response object of a MaintenanceRequest
    *
    ****************************************************************************
    */

   struct Response {
      uint32_t wartungs_info;       ///< wartungs_info
      uint8_t aaf;                  ///< aaf
      uint8_t bwi;                  ///< bwi
      uint8_t cid;                  ///< cid
      uint8_t einheitdaten;         ///< einheitdaten
      uint8_t optimierung;          ///< optimierung
      uint8_t qualitaet_empfohlen;  ///< qualitaet_empfohlen
      uint8_t qualitaet_vorhanden;  ///< qualitaet_vorhanden
      uint8_t reihenfolge;          ///< reihenfolge
      uint8_t statusdaten;          ///< statusdaten
      uint8_t weitere_termine;      ///< weitere_termine

      /**
       * Check for equality
       */
      bool operator==(const Response& rhs) const;
   };


   /**
    ****************************************************************************
    *
    * @brief Result type of a maintenance request
    *
    * @details the error is of type std::string to indicate an error message
    * which can be printed
    *
    ****************************************************************************
    */
   typedef Result<Response, std::string> RequestResult;

   /**
    ****************************************************************************
    *
    * @brief Send a MaintenanceRequest on CAN using the Request object
    *
    * @details The request object has to be instantiated and initialized with
    * the desired values beforehand.
    *
    * @note This call is blocking until an answer has been received or timeout
    * occurred.
    *
    * @param request An instance of a Request, where the desired values have
    * been set.
    * @param timeout_ms Wait until the function returns in case no response
    * arrives within the specified time (default: 5000 ms = 5s).
    *
    * @return result.ok() == true if a response has been received -
    * result.take_val() to take out the Response instance. Otherwise
    * result.nok() == true - in case of timeout or communication error.
    *
    ****************************************************************************
    */
    virtual RequestResult request(const Request& request, unsigned timeout_ms = 5000);

   /**
    ****************************************************************************
    *
    * Convenience typedef to a shared pointer type of this class
    *
    ****************************************************************************
    */
   typedef std::shared_ptr<MaintenanceRequestClient> ptr_t;

   /**
    ****************************************************************************
    *
    * Disallow default constructor
    *
    ****************************************************************************
    */
   MaintenanceRequestClient() =delete;

   /**
    ****************************************************************************
    *
    * @brief Constructor for the MaintenanceRequestClient
    *
    * @details Needs to be initialized with an existing com::ITransportClient,
    * therefore the default constructor is deleted.
    *
    * @param tclient An initialized internal transport client
    *
    ****************************************************************************
    */
   MaintenanceRequestClient(com::ITransportClient_ptr& tclient);

   /**
    ****************************************************************************
    *
    * @brief Default destructor, allows inheritance
    *
    ****************************************************************************
    */
   virtual ~MaintenanceRequestClient();

protected:
private:
   class PublishWrapper: public com::ISubscriptionObserver
   {
   public:
      PublishWrapper(MaintenanceRequestClient* client) : m_client(client) {}

   private:
      void message(const std::string& topic, const com::Message& message) override;
      MaintenanceRequestClient* m_client;
   };

   void handle_response(const com::Message& message);

   com::ITransportClient_ptr m_tclient;
   com::ISubscriptionObserver_ptr m_pub_wrapper;
   std::mutex m_cv_mutex;
   std::condition_variable m_cv;
   RequestResult m_response;
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
typedef MaintenanceRequestClient::ptr_t MaintenanceRequestClient_ptr;

/**
 *******************************************************************************
 *
 *  @brief Create a MaintenanceRequestClient_ptr
 *
 *  @details Creates a new MaintenanceRequestClient using the supplied
 *  ITransportClient
 *
 *  @note that there can be only one MaintenanceRequestClient per
 *  ITransportClient
 *
 *  @param tclient An ITransportClient for internal communication
 *
 *  @return MaintenanceRequestClient_ptr to the shared_ptr instance of the
 *  MaintenanceRequestClient
 *
 *******************************************************************************
 */
MaintenanceRequestClient_ptr
create_main_req_client(com::ITransportClient_ptr& tclient);

}}}

#endif /* MAIN_REQ_H */
