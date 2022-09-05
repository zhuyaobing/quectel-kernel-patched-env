/*
 *******************************************************************************
 *******************************************************************************
 ***
 ***     File: messageadatpter.h
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
#ifndef MESSAGEADAPTER_H
#define MESSAGEADAPTER_H

/*
 **=============================================================================
 ** Imported definitions
 **=============================================================================
 */

#include <type_traits>
#include <functional>

#include <google/protobuf/message.h>

#include <ecu/com/protocol.h>
#include <ecu/com/message.h>
#include <ecu/common.h>
#include <ecu/logging.h>
#include <ecu/datetime.h>

#include <ecu/pb/header.pb.h>
#include <ecu/pb/message.pb.h>


/*
 **=============================================================================
 ** Public type definitions
 **=============================================================================
 */

namespace ecu {
namespace lapi {
namespace com {


// errors
namespace err {
/**
 ****************************************************************************
 *
 * @brief Serilization errors that can occur for Pb internal messages
 *
 ****************************************************************************
 */
enum PbInternalDeserializationError
{
   PARSE_OK = 0,                       /**< Parsing went fine */
   PARSE_TOTAL_LENGTH_TOO_SHORT_ERR,   /**< Indicates that the total length
                                          of the message was to short */
   PARSE_HDR_MALFORMATED_ERR,          /**< Header of message was malformed */
   PARSE_PROTOCOL_NOT_USED_ERR,        /**< Protocol in message hdr undefined */
   PARSE_HDR_LENGTH_ERR,               /**< Header length malformed */
   PARSE_BODY_ERR                      /**< Message body malformed */
};
} // end err namespace

/**
 *******************************************************************************

 * @brief Interface for LAPI defined Message Adapters
 *
 * @details A IMessageAdapter shall be used to support (de)serialization of
 * dataformat specific generated message types, such as ProtoBuf. The pure
 * virtual serialize() shall always produce a Message with the serialized data
 * set in the Message's databuffer, see IMessageAdapter::SerializeResult.
 *
 * Apart from just serializing a specific message type, the serialization can
 * also handle transport protocol specific artifacts. For example, the alex
 * specific internal MQTT message format always includes a header which will
 * always be included in a Message instance, however completely hidden to the
 * user of LAPI. Thus the deserialization works in similar fashion.
 *
 * Note: classes implementing this interface can provide a way to set
 * message type which shall be serialized, i.e. as done in
 * PbInternalMessageAdapter
 *
 * @tparam Type A generic type of an data class which shall be (de)serialized
 * @tparam DeserializeErrorType As the name suggests, when deserialization
 * failed, this will be the Type which can be used details.
 *
 *******************************************************************************
 */
template<class Type, class DeserializeErrorType>
class IMessageAdapter
{
public:

   /**
    ****************************************************************************
    *
    * @brief Result type for serialization. @see serialize for usage details
    *
    * @details The Result will contain the actual Message instance which has
    * been generated during serialization.
    *
    ****************************************************************************
    */
   typedef Result<Message> SerializeResult;

   /**
    ****************************************************************************
    *
    * @brief Result type for deserialization. @see deserialize for usage details
    *
    * @details The Result will contain the actual TypeMessage instance which
    * has been generated during deserialization.
    * In case deserialization failed, the specific Error type will be set.
    *
    ****************************************************************************
    */
   typedef Result<Type, DeserializeErrorType> DeserializeResult;

   /**
    ****************************************************************************
    *
    * @brief Virtual destructor to allow inheritance
    *
    ****************************************************************************
    */
   virtual ~IMessageAdapter() {}

   /**
    ****************************************************************************
    *
    * @brief Serialize the desired message type into a Message
    *
    * @details The serialized data will be part of the Result type.
    *
    * @param typemessage data of InternalType to serialize into a com::Message
    *
    * @returns The result of the serialization. The payload of the result will
    * be a Message instance if serialization worked. If failed, the
    * Result::err_val() can be queried.
    *
    ****************************************************************************
    */
   virtual SerializeResult serialize(const Type& typemessage) = 0;

   /**
    ****************************************************************************
    *
    * @brief Deserialize a Message to the desired message type
    *
    * @details The deserialized data will be part of the Result type.
    *
    * @param message serialized message to deserialize into the InternalType
    *
    * @return Result of the deserialization. The payload of the result will
    * be a InternalType instance if deserialization worked. If failed, the
    * Result::err_val() can be queried.
    *
    ****************************************************************************
    */
   virtual DeserializeResult deserialize(const Message& message) = 0;

protected:
   /**
    ****************************************************************************
    *
    * @brief Hide constructor to prohibit external construction
    *
    ****************************************************************************
    */
   IMessageAdapter() {}
};

//------------------------------------------------------------------------------



/**
 *******************************************************************************
 *
 * @brief A template for the internal ProtoBuf message format
 *
 * @details All messages which shall be send via an ITransportClient using the
 * internal protobuf message format (including the SRE header) must use this
 * template to serialize and deserialize the InternalType to and from a Message
 * instance.
 *
 * The default ProtocolType will be PROTOCOL_TYPE_SRE_PROTOBUF and should not
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
 *
 * Example usage serialize:
 * @code
 *   pb::RtSignal rt_signal;;
 *   rt_signal.set_timestamp(0x23422342);
 *   rt_signal.set_data("0123123");
 *
 *   PbInternalMessageAdapter<pb::RtSignal> adapter;
 *   auto serialize_result = adapter.serialize( rt_signal);
 *   EXPECT_TRUE(serialize_result.ok());
 *
 *   // serialize_result.val() returns com::Message&
 *   // serialize_result.take_val() returns com::Message&&
 *   tranport_client.publish("<topic>", serialize_result.val());
 * @endcode
 *
 *
 * Example usage deserialize:
 * @code
 *   // received com::Message& msg
 *   PbInternalMessageAdapter<pb::RtSignal> adapter;
 *   auto result = adapter.deserialize( msg);
 *   EXPECT_TRUE(result.ok());
 *
 *   auto rt_msg = result.take_val();
 *   EXPECT_EQ(rt_msg.data(), "0123123");
 * @endcode
 *
 *
 * @tparam InternalType A subtype of google::protobuf::Message or std::string
 *
 *******************************************************************************
 */
template<class InternalType>
class PbInternalMessageAdapter :
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
   PbInternalMessageAdapter() ;

   /**
    ****************************************************************************
    *
    * @brief virtual destructor to allow inheritance
    *
    ****************************************************************************
    */
   virtual ~PbInternalMessageAdapter () {}

   /**
    ****************************************************************************
    *
    * @brief Serialize the desired message type into a Message
    *
    * @details The serialized data will be part of the Result type and could
    * be extracted via SerializeResult::take_val()
    *
    * @param typemessage data of InternalType to serialize into a com::Message
    *
    * @returns The result of the serialization. The payload of the result will
    * be a Message instance if serialization worked. If failed, the
    * Result::err_val() can be queried.
    *
    ****************************************************************************
    */
   virtual SerializeResult serialize(const InternalType& typemessage) override;

   /**
    ****************************************************************************
    *
    * @brief Deserialize a Message to the desired message type
    *
    * @details The deserialized data will be part of the Result type and could
    * be extracted via DeserializeResult::take_val()
    *
    * @param message serialized message to deserialize into the InternalType
    *
    * @return Result of the deserialization. The payload of the result will
    * be a InternalType instance if deserialization worked. If failed, the
    * Result::err_val() can be queried.
    *
    ****************************************************************************
    */
   virtual DeserializeResult deserialize(const Message& message) override;

   /**
    ****************************************************************************
    *
    * @brief Get the protocol type of the adapter instance
    *
    * @details Internal use, this will in most cases be
    * PROTOCOL_TYPE_SRE_PROTOBUF.
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
    * @details PROTOCOL_TYPE_SRE_PROTOBUF is default, no need to set it
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
PbInternalMessageAdapter<InternalType>::PbInternalMessageAdapter() :
   m_protocol(com::PROTOCOL_TYPE_SRE_PROTOBUF)
{
   static_assert(
         std::is_convertible<InternalType*, google::protobuf::Message*>::value,
         "No ProtoBuf Message type");
}
template<>
PbInternalMessageAdapter<std::string>::PbInternalMessageAdapter() ;


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
PbInternalMessageAdapter<InternalType>::get_timestamp(const InternalType& /*typemessage*/)
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
PbInternalMessageAdapter<ecu::lapi::pb::RtSignal>::get_timestamp(
      const ecu::lapi::pb::RtSignal& typemessage);

//------------------------------------------------------------------------------

template<class InternalType>
typename PbInternalMessageAdapter<InternalType>::SerializeResult
PbInternalMessageAdapter<InternalType>::serialize(const InternalType& typemessage)
{
   auto logmod = logging::module("lapi.com.messageadapter");

   void* out_buf = nullptr;
   const auto timestamp = get_timestamp(typemessage);
   const auto body_size = typemessage.ByteSize();
   ecu::lapi::pb::SREHeader hdr;
   hdr.set_sre_header(1);
   hdr.set_timestamp(timestamp);
   ::google::protobuf::uint64 h = 0;
   const auto hdr_size = hdr.ByteSize();
   uint64_t n = UINTMAX_MAX;
   h |= (n & body_size) << 32;
   h |= (n & hdr_size) << 16;
   h |= (n & m_protocol);
   hdr.set_sre_header(h);

   Message::DataBuffer buffer(body_size + hdr_size);

   try {
      out_buf = (void*) buffer.data();
      if (!hdr.SerializeToArray(out_buf, hdr_size)) {
         throw std::runtime_error("unable to serialize SREheader!");
      }
      if (!typemessage.SerializeToArray(
               (void*) ((uint8_t*) out_buf + hdr_size), body_size)) {
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

   auto msg = Message(std::move(buffer));
   msg.set_timestamp(timestamp);
   return SerializeResult(true, std::move(msg));
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
typename PbInternalMessageAdapter<std::string>::SerializeResult
PbInternalMessageAdapter<std::string>::serialize(const std::string& typemessage) ;

//------------------------------------------------------------------------------

template<class InternalType>
typename PbInternalMessageAdapter<InternalType>::DeserializeResult
PbInternalMessageAdapter<InternalType>::deserialize(const Message& msg)
{
   using namespace err;

   auto logmod = logging::module("lapi.com.messageadapter");

   const auto& buffer = msg.get_buffer();

   if (buffer.size() == 0 )
   {
      return DeserializeResult(false, PARSE_TOTAL_LENGTH_TOO_SHORT_ERR);
   }

   if (buffer.size() < SRE_HEADER_LENGTH )
   {
      LOG_MOD(ERROR, logmod)
         << "Total length is smaller then expected header length";
      return DeserializeResult(false, PARSE_TOTAL_LENGTH_TOO_SHORT_ERR);
   }

   ecu::lapi::pb::SREHeader header;
   if ( !header.ParseFromArray((void*)&buffer.front(), SRE_HEADER_LENGTH) )
   {
      LOG_MOD(ERROR, logmod) << "Unable to decode the header.";
      return DeserializeResult(false, PARSE_HDR_MALFORMATED_ERR);
   }

   const auto m_hdr = header.sre_header();
   m_protocol = static_cast<com::ProtocolType>(m_hdr & 0xFFFF);

   if ( m_protocol == com::PROTOCOL_TYPE_NOT_USED)
   {
      LOG_MOD(ERROR,logmod) << "ProtocolType is set to not used!";
      return DeserializeResult(false, PARSE_PROTOCOL_NOT_USED_ERR);
   }

   // header length sanity check, has to be SRE_HEADER_LENGTH
   const auto hl = (m_hdr >> 16) & 0xFFFF;
   if ( hl != SRE_HEADER_LENGTH )
   {
      LOG_MOD(ERROR, logmod) << "Specified hdr-len does not comply.";
      return DeserializeResult(false, PARSE_HDR_LENGTH_ERR);
   }

   const auto ml = (m_hdr >> 32) & 0xFFFFFFFF;

   InternalType typemessage{};
   // cast to char* to avoid pointer arithmetic on void*
   const uint16_t offset = SRE_HEADER_LENGTH;
   if ((ml+offset) > buffer.size() || !typemessage.ParseFromArray(
            ((Message::BufferType*)&buffer.front()) + offset, ml))
   {
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
typename PbInternalMessageAdapter<std::string>::DeserializeResult
PbInternalMessageAdapter<std::string>::deserialize(const Message& msg) ;


}}} /* end namespace */

#endif /* MESSAGEADAPTER_H */
