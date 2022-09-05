/*
 *******************************************************************************
 *******************************************************************************
 ***
 ***     File: man_messageadatpter.h
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


/*
 **=============================================================================
 ** Header definition
 **=============================================================================
 */
#ifndef VCP_MESSAGEADAPTER_H
#define VCP_MESSAGEADAPTER_H

/*
 **=============================================================================
 ** Imported definitions
 **=============================================================================
 */

#include <type_traits>

#include <google/protobuf/message.h>

#include <ecu/com/protocol.h>
#include <ecu/com/message.h>
#include <ecu/com/messageadapter.h>
#include <ecu/common.h>
#include <ecu/logging.h>
#include <ecu/datetime.h>

/*
 **=============================================================================
 ** Public type definitions
 **=============================================================================
 */

namespace ecu {
namespace lapi {
namespace com {

/**
 *******************************************************************************
 *
 * @brief A template for the MAN VCP ProtoBuf message format
 *
 * @details All MQTT messages exchanged between the TBM3/VCM and Backend via a
 * ITransportClient using the VCP message format must use this template to
 * serialize to and deserialize from a Message instance. The template class
 * provides setter and getter to set and get the actual message type
 * (InternalType) before and after the (de)serialization.
 *
 * The default ProtocolType will be PROTOCOL_TYPE_VCP_PROTOBUF and should not
 * be changed.
 *
 * Note: the template can only be used when the message type base class is
 * of type google::protobuf::Message or std::string (char), otherwise
 * compilation will fail with a static_assert.
 *
 * Note: The protobuf pb::RtSignal message has a specialized implementation for
 * the get_timestamp() function since the timestamp shall be taken from within
 * the pb::RtSignal message. For the user of the API, this should not be of any
 * significance.
 *
 * Example usage:
 * Please see the documentation for the ecu::lapi::com::PbInternalMessageAdapter
 *
 *
 * @tparam InternalType A subtype of google::protobuf::Message or std::string
 *
 *******************************************************************************
 */
template<class InternalType>
class PbVcpMessageAdapter :
   public IMessageAdapter<InternalType, err::PbInternalDeserializationError>
{
private:
   typedef IMessageAdapter<InternalType,
           err::PbInternalDeserializationError> IMessageAdapterType;

public:

   /**
    ****************************************************************************
    *
    * @brief Result type for serialization
    *
    * @details The Result will contain the actual Message instance which has
    * been generated during serialization.
    *
    ****************************************************************************
    */
   typedef typename IMessageAdapterType::SerializeResult SerializeResult;

   /**
    ****************************************************************************
    *
    * @brief Result type for deserialization
    *
    * @details The Result will contain the actual TypeMessage instance which
    * has been generated during deserialization.
    * In case deserialization failed, the err::PbInternalDeserializationError
    * will be set.
    *
    ****************************************************************************
    */
   typedef typename IMessageAdapterType::DeserializeResult DeserializeResult;


   /**
    ****************************************************************************
    *
    * @brief Default constructor which will initialize an adapter
    *
    * @details Contains a static_assert to enforce InternalType to be
    * of type google::protobuf::Message
    *
    ****************************************************************************
    */
   PbVcpMessageAdapter() ;

   /**
    ****************************************************************************
    *
    * @brief virtual destructor to allow inheritance
    *
    ****************************************************************************
    */
   virtual ~PbVcpMessageAdapter () {}

   /* override from interface */
   SerializeResult serialize(const InternalType& typemessage) override;

   /* override from interface */
   DeserializeResult deserialize(const Message& message) override;

   /**
    ****************************************************************************
    *
    * @brief Get the protocol type of the adapter instance
    *
    * @details Internal use this will in most cases be
    * PROTOCOL_TYPE_VCP_PROTOBUF.
    *
    * @return the protocol used for this adapter instance
    *
    ****************************************************************************
    */
   inline com::ProtocolType get_protocol() const { return m_protocol; }

   /**
    ****************************************************************************
    *
    * @brief Set the protocol type for the adapter instance
    *
    * @details PROTOCOL_TYPE_VCP_PROTOBUF is default, no need to set it
    * manually.
    *
    * @param protocol The protocol to use for this adapter instance
    *
    ****************************************************************************
    */
   inline void set_protocol(com::ProtocolType protocol)
   { m_protocol = protocol; }

protected:
   uint64_t get_timestamp(const InternalType& typemessage);
   com::ProtocolType m_protocol; ///< Internal message transport protocol type
};


/**
 *******************************************************************************
 *
 * @brief Default constructor which will initialize an adapter
 *
 *******************************************************************************
 */
template<class InternalType>
PbVcpMessageAdapter<InternalType>::PbVcpMessageAdapter() :
   m_protocol(com::PROTOCOL_TYPE_VCP_PROTOBUF)
{
   static_assert(
         std::is_convertible<InternalType*, google::protobuf::Message*>::value,
         "No ProtoBuf Message type");
}
template<>
PbVcpMessageAdapter<std::string>::PbVcpMessageAdapter() ;

/**
 *******************************************************************************
 *
 * @brief Generic implementation will always use the current system time
 *
 * @return The system time
 *
 *******************************************************************************
 */
template<class InternalType>
uint64_t
PbVcpMessageAdapter<InternalType>::get_timestamp(const InternalType& /*typemessage*/)
{
   return DateTime::time_since_boot_ns();
}

/**
 *******************************************************************************
 *
 * @brief pb::RtSignal specific get_timestamp() will extract the timestamp from
 * pb::RtSignal::timestamp() instead of using the system time.
 *
 * @return The timestamp extracted from pb::RtSignal
 *
 *******************************************************************************
 */
template<>
uint64_t
PbVcpMessageAdapter<ecu::lapi::pb::RtSignal>::get_timestamp(
      const ecu::lapi::pb::RtSignal& typemessage);

//------------------------------------------------------------------------------

template<class InternalType>
typename PbVcpMessageAdapter<InternalType>::SerializeResult
PbVcpMessageAdapter<InternalType>::serialize(const InternalType& typemessage)
{
   auto logmod = logging::module("lapi.com.vcpmessageadapter");

   if (m_protocol == ecu::lapi::com::PROTOCOL_TYPE_NOT_USED )
   {
      LOG_MOD(ERROR, logmod)
         << "Protocol is not specified, unable to serialize!";
      return SerializeResult(false, Message());
   }

   const auto body_size = typemessage.ByteSize();
   Message::DataBuffer buffer(body_size);

   try {
      void* body_buf = (void*) buffer.data();

      if (!typemessage.SerializeToArray(
               (void*) ((uint8_t*) body_buf), body_size)) {
         throw std::runtime_error("unable to serialize body!");
      }
   } catch (std::runtime_error ex) {
      LOG_MOD(ERROR, logmod) << ex.what();
      return SerializeResult(false, Message());
   } catch (...) {
      LOG_MOD(EMERG, logmod)
         << "Unable to allocate memory for serialization!";
      return SerializeResult(false, Message());
   }

   return SerializeResult(true, Message(std::move(buffer)));
}

/**
 *******************************************************************************
 *
 * @brief Special std::String implementation of serialize,
 * also used for JSON data. @see serialize
 *
 *******************************************************************************
 */
template<>
typename PbVcpMessageAdapter<std::string>::SerializeResult
PbVcpMessageAdapter<std::string>::serialize(const std::string& typemessage) ;

//------------------------------------------------------------------------------

template<class InternalType>
typename PbVcpMessageAdapter<InternalType>::DeserializeResult
PbVcpMessageAdapter<InternalType>::deserialize(const Message& msg)
{
   using namespace err;
   const auto& buffer = msg.get_buffer();
   const auto buffer_size = buffer.size();

   InternalType typemessage{};
   // cast to char* to avoid pointer arithmetic on void*
   if (!typemessage.ParseFromArray(
            ((Message::BufferType*)&buffer.front()), buffer_size))
   {
      auto logmod = logging::module("lapi.com.vcpmessageadapter");
      LOG_MOD(EMERG, logmod) << "Unable to deserialize protobuf";
      return DeserializeResult(false, PARSE_BODY_ERR);
   }

   return DeserializeResult(true, std::move(typemessage), PARSE_OK);
}

/**
 *******************************************************************************
 *
 * @brief Special std::String implementation of deserialize,
 * also used for JSON data. @see deserialize
 *
 *******************************************************************************
 */
template<>
typename PbVcpMessageAdapter<std::string>::DeserializeResult
PbVcpMessageAdapter<std::string>::deserialize(const Message& msg) ;


}}} /* end namespace */

#endif /* VCP_MESSAGEADAPTER_H */

