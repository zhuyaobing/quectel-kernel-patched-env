/*
***************************************************************************************
***************************************************************************************
***
***     File: datasource.h
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

#ifndef DATASOURCE_H
#define DATASOURCE_H

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/

#include <memory>

namespace ecu {
namespace lapi {
namespace ipc {

/*
**=====================================================================================
** Public type definitions
**=====================================================================================
*/

/*
 *******************************************************************************
 **
 **  @brief Interface for a data consumer
 **
 **  @details The consumer will continuously data form the data source on which
 **           it has been registered.
 **
 **  @tparam Data The data type that shall be used when receiving data from the
 **          source.
 **
 *******************************************************************************
 */

template<class Data>
class IDataConsumer
{
public:
   typedef std::unique_ptr<IDataConsumer> ptr_t;
   virtual ~IDataConsumer() {}

   /**
    ****************************************************************************
    *
    * @brief Used by a data source to send data to the consumer.
    *
    ***************************************************************************
    */
   virtual void consume(const Data& data) = 0;

   /**
    ****************************************************************************
    *
    * @brief Abstract function to indicate if Data consumption is finished.
    *
    ****************************************************************************
    */
   virtual void finished() { }

   /**
    ****************************************************************************
    *
    * @brief Abstract function to indicate if Data frame consumption is finished.
    *
    * @details if frame is defined as: all data read until a 300ms timeout
    * while waiting for the next data entry
    *
    ****************************************************************************
    */
   virtual void frame_finished() { }
protected:
   /* Nothing */
private:
   /* Nothing */
};

// convenience type def
template<class Data>
using IDataConsumer_ptr = typename IDataConsumer<Data>::ptr_t;


/*
 *******************************************************************************
 **
 **  @brief Interface for a DataSource which will be consumed be a IDataConsumer
 **
 **  @tparam Data The Data type that shall used to notify consumer about new
 **          data.
 **
 **  @details A specific data source shall continuously receive which will then
 **           populated to the consumer. Not
 **
 *******************************************************************************
 */
template<class Data>
class IDataSource
{
public:
   typedef std::unique_ptr<IDataSource> ptr_t;

   virtual ~IDataSource() {}

   /*
    ****************************************************************************
    **
    **  @brief Sets the consumer of the data source.
    **
    **  @param consumer The consumer of the Data that will be received.
    **  @sa receive_data
    **
    ***************************************************************************
    */
   virtual void set_consumer(IDataConsumer_ptr<Data> consumer) = 0;

   /*
    ****************************************************************************
    **
    **  @brief Start receiving for the data source
    **
    **  @details Depending on the implementation of the actual source, this can
    **           start a thread which polling a resource or start a timer which
    **           continuously fires and fetches data.
    **
    ***************************************************************************
    */
   virtual void receive_data() = 0;

protected:
   /* Nothing */
   IDataSource() =default;
private:
   /* Nothing */
   // TODO remove this
   /** Non-copyable */
   IDataSource(const IDataSource&) =delete;
   IDataSource& operator=(const IDataSource&) =delete;
};


// convenience type def
template<class Data>
using IDataSource_ptr = typename IDataSource<Data>::ptr_t;

}}} /* namespace */

#endif /* DATASOURCE_H */
