/*
 *******************************************************************************
 *******************************************************************************
 ***
 ***     File: message.h
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
#ifndef MESSAGE_H
#define MESSAGE_H

/*
 **=============================================================================
 ** Imported definitions
 **=============================================================================
 */
#include <vector>
#include <memory>

/*
 **=============================================================================
 ** Public type definitions
 **=============================================================================
 */
namespace ecu {
namespace lapi {

/**
 *******************************************************************************
 *
 * @brief Contains all Linux data-layer related artifacts
 *
 *******************************************************************************
 */
namespace com {

/**
 *******************************************************************************
 *
 * @brief Generic Message Type which contains the serialized message contents
 *
 * @details The Message class acts as a container for serialized messages which
 * can be defined in any arbitrary format. For example, internal messages will
 * be serialized as ProtoBuf. An instance of Message will just contain the
 * serialized output of such message. For serialization, the IMessageAdapter and
 * its derived class should be used.
 *
 * A Message also contains a timestamp which will be used by reconstructed dl
 * function. The timestamp will be set during (de)serialization and does not
 * need to be maintained manually.
 *
 * Further if the transport protocol supports Quality of Service (QoS), the
 * Message::set_qos can be used to enforce a certain qos-level. Note: classes
 * that implemented the ITransportClient interface can make use of this, i.e
 * MqttClient.
 *
 *******************************************************************************
 */
class Message
{
public:
   /**
    ****************************************************************************
    *
    * The type of the internal data-container, used for serialized messages
    *
    ****************************************************************************
    */
   typedef unsigned char BufferType;

   /**
    ****************************************************************************
    *
    * Convenience typedef to a unique pointer type of this class
    *
    ****************************************************************************
    */
   typedef std::unique_ptr<Message> uptr_t;

   /**
    ****************************************************************************
    *
    * Convenience typedef to a shared pointer type of this class
    *
    ****************************************************************************
    */
   typedef std::shared_ptr<Message> sptr_t;

   /**
    ****************************************************************************
    *
    * Convenience typedef of the DataBuffer, which represents the container for
    * serialized message data.
    *
    ****************************************************************************
    */
   typedef std::vector<BufferType> DataBuffer;

   /**
    ****************************************************************************
    *
    * @brief default constructor, nothing will be set
    *
    ****************************************************************************
    */
   Message() :
      m_qos(0),
      m_timestamp(0),
      m_timestamp_extracted(false),
      m_retain(false)
   { }

   /**
    ****************************************************************************
    *
    * @brief Consuming constructor for a buffer
    *
    * @details The created instance of Message will take ownership of the
    * provided DataBuffer.
    *
    * @param buffer The DataBuffer which shall be consumed
    *
    ****************************************************************************
    */
   Message(DataBuffer&& buffer) :
      m_qos(0),
      m_timestamp(0),
      m_timestamp_extracted(false),
      m_buffer(std::move(buffer)),
      m_retain(false)
   { }

   /**
    ****************************************************************************
    *
    * @brief Destructor
    *
    * @details marked as virtual to allow inheritance
    *
    ****************************************************************************
    */
   virtual ~Message () { }


   /**
    ****************************************************************************
    *
    * @brief Set the internal data buffer and take ownership of the provided
    * buffer.
    *
    * @param buffer The DataBuffer which shall be set and taken
    *
    ****************************************************************************
    */
   void set_buffer(DataBuffer&& buffer) { m_buffer = std::move(buffer); }

   /**
    ****************************************************************************
    *
    * @brief Return the internal DataBuffer
    *
    * @details This function shall be used be any implementation of the
    * ITransportClient to send data. The Message class will retain ownership of
    * the buffer.
    *
    * @return A const reference of the internal DataBuffer
    *
    ****************************************************************************
    */
   inline const DataBuffer& get_buffer() const { return m_buffer; }

   /**
    ****************************************************************************
    *
    * @brief Enforce setting the timestamp. This should normally be done by a
    * IMessageAdapter
    *
    * @param timestamp The timestamp which shall be set
    *
    ****************************************************************************
    */
   inline void set_timestamp(uint64_t timestamp)
   {
      m_timestamp_extracted = true;
      m_timestamp = timestamp;
   }

   /**
    ****************************************************************************
    *
    * @brief Return the timestamp of a message
    *
    * @return The timestamp
    *
    ****************************************************************************
    */
   uint64_t get_timestamp() const;

   /**
    ****************************************************************************
    *
    * @brief Set the qos for a message, if required
    *
    * @param qos The qos value which shall be set
    *
    ****************************************************************************
    */
   void set_qos(uint8_t qos) { m_qos = qos; }

   /**
    ****************************************************************************
    *
    * @brief Return the qos value for a Message
    *
    * @return The current qos value
    *
    ****************************************************************************
    */
   uint8_t get_qos() const { return m_qos; }

   /**
    ****************************************************************************
    *
    * @brief Set the retain flag for a message, if needed
    *
    * @param retain The retain flag value to be set.
    *
    ****************************************************************************
    */
   void set_retain(bool retain) { m_retain = retain; }

   /**
    ****************************************************************************
    *
    * @brief Return the retain flag value for a Message
    *
    * @return The current retain flag value
    *
    ****************************************************************************
    */
   bool get_retain() const { return m_retain; }

protected:
   uint8_t m_qos;                               ///< MQTT message Quality of service
   mutable uint64_t m_timestamp;                ///< Timestamp of message, automatically set
   mutable bool m_timestamp_extracted = false;  ///< Timestamp has been extracted / set
   DataBuffer m_buffer;                         ///< Actual message data buffer
   bool m_retain = false;                       ///< MQTT message flag retained
};

/*
 **=============================================================================
 ** Public constant definitions for external access
 **=============================================================================
 */

/**
 *******************************************************************************
 *
 * Convenience typedef for external use
 *
 *******************************************************************************
 */
typedef Message::uptr_t Message_uptr;

/**
 *******************************************************************************
 *
 * Convenience typedef for external use
 *
 *******************************************************************************
 */
typedef Message::sptr_t Message_sptr;

/*
 **=============================================================================
 ** Function prototype declarations for external access
 **=============================================================================
 */
}}} /* end namespace */

#endif /* MESSAGE_H */
