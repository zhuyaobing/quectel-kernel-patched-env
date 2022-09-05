/*
***************************************************************************************
***************************************************************************************
***
***     File: observer.h
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
#ifndef OBSERVER_H
#define OBSERVER_H

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/
#include <memory>

#include <ecu/com/message.h>

/*
**=====================================================================================
** Public type definitions
**=====================================================================================
*/
namespace ecu {
namespace lapi {
namespace com {

/**
 *******************************************************************************
 *
 * @brief Can be used to observe the connection status of a ITransportClient.
 *
 * @warning The callback functions are executed by the TransportClient thread
 *          and will therefore block the reception of further MQTT data.
 *          This thread must not be used to call functions that are expecting
 *          a synchronous response over MQTT. Functions that shall not be
 *          called from the message function are for example the get function
 *          on the VariableClient and the SysParamsClient or the send_request
 *          function on the UdsClient. The BackendClient is safe to use.
 *
 * @details The is a abstract class where the default implementation of the
 *          provided will not do anything. Classes derived from this class have
 *          the possibility to perform actions on certain events related to the
 *          Connection, i.e. MQTT.
 *
 *******************************************************************************
 */
class IConnectionObserver
{
public:
   /**
    ****************************************************************************
    *
    * Convenience typedef to a shared pointer type of this class
    *
    ****************************************************************************
    */
   typedef std::shared_ptr<IConnectionObserver> ptr_t;

   /**
    ****************************************************************************
    *
    * Destructor to allow inheritance.
    *
    ****************************************************************************
    */
   virtual ~IConnectionObserver();

   /**
    ****************************************************************************
    *
    * @brief Will be called when the associated client has established a connection.
    * TODO, figured out what rc shall be used for
    *
    * @param rc Not sure :)
    *
    ****************************************************************************
    */
   virtual void on_connect(int rc);

   /**
    ****************************************************************************
    *
    * @brief Will be called when the associated client has last connection.
    * TODO, figured out what rc shall be used for
    *
    * @param rc Not sure :)
    *
    ****************************************************************************
    */
   virtual void on_disconnect(int rc);

   /**
    ****************************************************************************
    *
    * @brief Will be called when a message has been successfully published
    *
    * @param mid The local message identifier previously returned by publish(...)
    *
    ****************************************************************************
    */
   virtual void on_publish(int mid);

   /**
    ****************************************************************************
    *
    * @brief Used to relay the client internal logging.
    *
    * @param level The log-level
    * @param str The log message as such!
    *
    ****************************************************************************
    */
   virtual void on_log(int level, const std::string& str);

   /**
    ****************************************************************************
    *
    * @brief Will be called when the associated client has encountered an error.
    * TODO: shouldn't the error be part of the function call?
    *
    ****************************************************************************
    */
   virtual void on_error();

protected:
   /* hide constructor */
   IConnectionObserver() {}

private:
   /* data */
};

/**
 *******************************************************************************
 *
 * @brief convenience typedef to the pointer type.
 *
 * @details This typedef shall be preferred before IConnectionObserver::ptr_t
 *
 *******************************************************************************
 */
typedef IConnectionObserver::ptr_t IConnectionObserver_ptr;

/**
 *******************************************************************************
 *
 *  @brief Observer interface for subscriptions. Classes that shall be subject
 *         of receiving message for certain topics.
 *
 *******************************************************************************
 */
class ISubscriptionObserver
{
public:
   /**
    ****************************************************************************
    *
    * Convenience typedef to a shared pointer type of this class
    *
    ****************************************************************************
    */
   typedef std::shared_ptr<ISubscriptionObserver> ptr_t;

   /**
    ****************************************************************************
    *
    * Destructor to allow inheritance.
    *
    ****************************************************************************
    */
   virtual ~ISubscriptionObserver()
   { }

   /**
    ****************************************************************************
    *
    * @brief Called when a message has been received for a certain MQTT topic
    *        subscription.
    *
    * @warning The message function is executed by the TransportClient thread
    *          and will therefore block the reception of further MQTT data.
    *          This thread must not be used to call functions that are expecting
    *          a synchronous response over MQTT. Functions that shall not be
    *          called from the message function are for example the get function
    *          on the VariableClient and the SysParamsClient or the send_request
    *          function on the UdsClient. The BackendClient is safe to use.
    *
    * @details This function is full virtual and has to be overridden by derived
    *          classes implementing this abstract class/interface.
    *
    * @param topic The topic on which the messsage has been received.
    * @param message The message as such containing the payload of the
    *        message.
    *
    ****************************************************************************
    */
   virtual void message(const std::string& topic, const Message& message) = 0;

protected:
   /* hide constructor */
   ISubscriptionObserver() {}
};
/**
 *******************************************************************************

 * @brief convenience typedef to the pointer type.
 *
 * @details This typedef shall be preferred before ISubscriptionObserver::ptr_t
 *
 *******************************************************************************
 */
typedef ISubscriptionObserver::ptr_t ISubscriptionObserver_ptr;

}}} // end namespace

#endif /* OBSERVER_H */
