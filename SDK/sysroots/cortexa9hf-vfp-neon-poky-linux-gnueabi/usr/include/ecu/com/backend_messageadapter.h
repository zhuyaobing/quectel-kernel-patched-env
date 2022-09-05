/*
 *******************************************************************************
 *******************************************************************************
 ***
 ***     File: backend_messageadatpter.h
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
#ifndef BACKEND_MESSAGEADAPTER_H
#define BACKEND_MESSAGEADAPTER_H

/*
 **=============================================================================
 ** Imported definitions
 **=============================================================================
 */
#include <ecu/com/messageadapter.h>

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
 * @brief Message adapter for de-/serializing ProtoBuf messages to/from the
 *        Backend via the BackendClient API
 *
 * @details Data which shall be sent and received via the BackendClient API
 * using a protobuf message format must use this template to serialize and
 * deserialize the InternalType to and from a Message instance.
 *
 * The default ProtocolType will be PROTOCOL_TYPE_SRE_PROTOBUF and should not
 * be changed.
 *
 * Note: the template can only be used when the message type base class is
 * of type google::protobuf::Message or std::string (char), otherwise
 * compilation will fail with a static_assert.
 *
 * Note: PbBackendMessageAdapter::deserialize can not be used to deserialize a
 * message that was serialized using PbBackendMessageAdapter::serialize !
 *
 *
 * Example usage serialize => Send to Backend:
 * @code
 *   std::string payload("This is some char test data");
 *   PbBackendMessageAdapter<std::string> adapter;
 *   auto serialize_result = adapter.serialize(payload);
 *   EXPECT_TRUE(serialize_result.ok());
 *
 *   // serialize_result.val() returns Message&
 *   // serialize_result.take_val() returns com::Message&&
 *   backend_client.publish( serialize_result.val(), content_id, tx_options);
 * @endcode
 *
 *
 * Example usage deserialize => Receive from Backend:
 * @code
 *   // Message& received_be_message;
 *   PbBackendMessageAdapter<std::string> adapter;
 *   auto result = adapter.deserialize(received_message);
 *   EXPECT_TRUE(result.ok());
 *
 *   auto received_be_string = result.take_val();
 *   EXPECT_EQ(received_be_string, "This is some char test data");
 * @endcode
 *
 * @tparam InternalType A subtype of google::protobuf::Message or std::string
 *
 *******************************************************************************
 */
template<class InternalType>
class PbBackendMessageAdapter :
   public PbInternalMessageAdapter<InternalType>
{
private:
   typedef PbInternalMessageAdapter<InternalType> InternalMessageAdapterType;

public:
   /**
    ****************************************************************************
    *
    * @brief Result type for serialization. @see serialize for usage details
    *
    ****************************************************************************
    */
   typedef typename InternalMessageAdapterType::SerializeResult SerializeResult;

   /**
    ****************************************************************************
    *
    * @brief Result type for deserialization. @see deserialize for usage details
    *
    ****************************************************************************
    */
   typedef typename InternalMessageAdapterType::DeserializeResult DeserializeResult;

   PbBackendMessageAdapter() :
      InternalMessageAdapterType() { }

   /**
    *******************************************************************************
    *
    * @brief Serialize data to send to backend using the BackendClient API
    *
    * @details Use to serialize a typemessage into a Message which can be used
    * in the call to BackendClient::publish(message, ...).
    *
    * @param typemessage data of InternalType to serialize into a com::Message
    *
    * @returns The result of the serialization. The payload of the result will
    * be a Message instance if serialization worked. If failed, the
    * Result::err_val() can be queried.
    *
    *******************************************************************************
    */
   SerializeResult serialize(const InternalType& typemessage) override;

   /**
    *******************************************************************************
    *
    * @brief Deserialize data received from backend using the BackendClient API
    *
    * @details Use to deserialize the Message received from the
    * BackendClient::ContentCallback_t, as well as a manually subscribed
    * com::Message on topic 'app/cid/#'.
    *
    * @param message serialized message to deserialize into the InternalType
    *
    * @return Result of the deserialization. The payload of the result will
    * be a InternalType instance if deserialization worked. If failed, the
    * Result::err_val() can be queried.
    *
    *******************************************************************************
    */
   DeserializeResult deserialize(const Message& message) override;
};

//------------------------------------------------------------------------------

template<class InternalType>
typename PbBackendMessageAdapter<InternalType>::SerializeResult
PbBackendMessageAdapter<InternalType>::serialize(const InternalType& typemessage)
{
   auto logmod = logging::module("lapi.com.bemessageadapter");

   if (this->m_protocol == ecu::lapi::com::PROTOCOL_TYPE_NOT_USED) {
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
typename PbBackendMessageAdapter<std::string>::SerializeResult
PbBackendMessageAdapter<std::string>::serialize(const std::string& typemessage) ;

//------------------------------------------------------------------------------

template<class InternalType>
typename PbBackendMessageAdapter<InternalType>::DeserializeResult
PbBackendMessageAdapter<InternalType>::deserialize(const Message& msg)
{
   return InternalMessageAdapterType::deserialize(msg);
}

}}} /* end namespace */

#endif /* BACKEND_MESSAGEADAPTER_H */
