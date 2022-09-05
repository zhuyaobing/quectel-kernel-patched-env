/*
 *******************************************************************************
 *******************************************************************************
 ***
 ***     File: signaladapter.h
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

/*
 **=============================================================================
 ** Header definition
 **=============================================================================
 */
#ifndef SIGNALADAPTER_H
#define SIGNALADAPTER_H

#include "_internal.h"

// stl
#include <vector>
#include <string>
#include <atomic>

// lapi
#include <ecu/common.h>
#include <ecu/com/message.h>

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
 * @brief Contains classes related to RT data
 *
 *******************************************************************************
 */
namespace rt {

//------------------------------------------------------------------------------

#ifndef DOXYGEN_IGNORE
class SignalAdapter;
class SignalGroup;
#endif /* ifndef DOXYGEN_IGNORE */

/**
 *******************************************************************************
 *
 * @brief Represents a Signal from the real-time partition
 *
 * @details A Signal is always contained within a SignalGroup and should
 * never be used without SignalGroup context.
 *
 * Signals contain data which can be updated using set_data function. However,
 * since Signals are always expected to be created with a fixed data size. When
 * using set_data, the provided DATA is only allowed to contain less or equal
 * the amount currently held by the signal (data().size()).
 *
 * In order to create Signals which are contained in SignalGroup, use
 * SignalAdapter::create_sg
 *
 * If the Signal is part of SignalGroup which does not relate to a CAN message,
 * the name() will be: SignalGroup::name() + "_signal"
 *
 * @note If the Signal is handled as an enum, is_enum(), the status check needs
 * to be maintained by the application developer, since the Signal definition
 * does not have any semantic knowledge which value of the signal is considered
 * invalid.
 *
 * @sa SignalAdapter
 *
 *******************************************************************************
 */
class Signal
{
public:
   /**
    ****************************************************************************
    *
    * @brief Delete assignment operator
    *
    ****************************************************************************
    */
   Signal& operator=(const Signal&) = delete;

   /**
    ****************************************************************************
    *
    * @brief Define copy constructor
    *
    ****************************************************************************
    */
   Signal(const Signal& other);

   /**
    ****************************************************************************
    *
    * @brief Delete default constructor
    *
    ****************************************************************************
    */
   Signal() = delete;

   /**
    ****************************************************************************
    *
    * @brief typedef of a BYTE
    *
    ****************************************************************************
    */
   using BYTE = uint8_t;

   /**
    ****************************************************************************
    *
    * @brief typedef of a DATA vector using BYTE
    *
    ****************************************************************************
    */
   using DATA = std::vector<BYTE>;

   /**
    ****************************************************************************
    *
    * @brief Defines the status of a Signal
    *
    * @details Everything different than STATUS_OK is consider an error
    *
    ****************************************************************************
    */
   enum Status {
      STATUS_NOT_AVAILABLE, ///< The Signal data indicates not available
      STATUS_ERROR,         ///< An error occurred, e.g. CAN error
      STATUS_OUT_OF_BOUND,  ///< The received it not within the min, max boundaries
      STATUS_OK             ///< the receive signal is valid
   };

   /**
    ****************************************************************************
    *
    * @brief Equal operator
    *
    * @param other The element to compare to
    *
    * @return true if equal otherwise false
    *
    ****************************************************************************
    */
   bool operator==(const Signal& other) const;

   /**
    ****************************************************************************
    *
    * @brief Not equal operator
    *
    * @param other The element to compare to
    *
    * @return true if not equal otherwise true
    *
    ****************************************************************************
    */
   bool operator!=(const Signal& other) const
   {
      return !(*this == other);
   }

    /**
    ****************************************************************************
    *
    * @brief Returns the scaling factor which should be used for this signal
    *
    * @return The factor
    *
    ****************************************************************************
    */
   double factor() const;

   /**
    ****************************************************************************
    *
    * @brief The min value related to this signal
    *
    * @return The min value
    *
    ****************************************************************************
    */
   unsigned int min() const;

   /**
    ****************************************************************************
    *
    * @brief The max value related to this signal
    *
    * @return The max value
    *
    ****************************************************************************
    */
   unsigned int max() const;

   /**
    ****************************************************************************
    *
    * @brief The offset used for this Signal
    *
    * @return offset
    *
    ****************************************************************************
    */
   int offset() const;

   /**
    ****************************************************************************
    *
    * @brief The size of the data type in bits
    *
    * @return size
    *
    ****************************************************************************
    */
   int data_type_size_bits() const;

   /**
    ****************************************************************************
    *
    * @brief The size of the data type in bytes
    *
    * @return size
    *
    ****************************************************************************
    */
   int data_type_size_bytes() const;

   /**
    ****************************************************************************
    *
    * @brief The total size of the data payload in bits
    *
    * @return size
    *
    ****************************************************************************
    */
   int data_payload_size_bits() const;

   /**
    ****************************************************************************
    *
    * @brief The total size of the data payload in bytes
    *
    * @return size
    *
    ****************************************************************************
    */
   int data_payload_size_bytes() const;

   /**
    ****************************************************************************
    *
    * @brief Used to check if the Signal should be handled as an enum type
    *
    * @details If the Signal is an enum, status handling need to be maintained
    * by the application developer.
    *
    * @return True if is of enum type, otherwise false
    *
    ****************************************************************************
    */
   bool is_enum() const;

   /**
    ****************************************************************************
    *
    * @brief Returns the status of the signal based on its data
    *
    * @note The functions only evaluate the status for can related topics,
    * other topics need to be handled manually.
    *
    * @note If the Signal is handled as an enum, is_enum(), the status check needs
    * to be maintained by the application developer, since the Signal definition
    * does not have any semantic knowledge which value of the signal is considered
    * valid or invalid.
    *
    * @return Returns a Signal::Status instance indicating of the signals is
    * STATUS_OK or not
    *
    ****************************************************************************
    */
   Status status() const;

   /**
    ****************************************************************************
    *
    * @brief The name of the signal
    *
    * @return The signal name
    *
    ****************************************************************************
    */
   const std::string& name() const { return m_name; }

   /**
    ****************************************************************************
    *
    * @brief The raw data of the Signal in DATA
    *
    * @return the Data
    *
    ****************************************************************************
    */
   const DATA& data() const { return m_data; }

   /**
    ****************************************************************************
    *
    * @brief Setter for the raw data
    *
    * @details The signal always has a specific size which will be set by
    * instantiation of a Signal via the SignalAdapter. Therefore, when the data
    * should be updated, only less or the same amount of DATA are allowed to be
    * set. If more bytes will be passed in, this set function will return false.
    *
    * @param data The raw data in DATA
    *
    * @return true if set operation succeeded, otherwise false
    *
    ****************************************************************************
    */
   bool set_data(const DATA& data);


   /**
    ****************************************************************************
    *
    * @brief Result-type used by value_scaled()
    *
    ****************************************************************************
    */
   typedef Result<double, std::string> ScaleResult;

   /**
    ****************************************************************************
    *
    * @brief Get the data_value scaled
    *
    * @warning This is only possible for CAN topics.
    *
    * @note The status of a Signal (using status()) should be evaluated before
    * using this function.
    *
    * @details The function is only useful for CAN related topics because only
    * those have a predefined data type size. All other topics should work with
    * the raw data provided by data().
    * Internally, status() (for CAN messages) will be called to evaluate if
    * the contained data is valid, if not the Result will indicate a negative
    * result.
    *
    * @return returns a positive result (result.ok() == true) when scaling
    * succeeded
    *
    ****************************************************************************
    */
   ScaleResult value_scaled() const;


   /**
    ****************************************************************************
    *
    * @brief Setter for the scaled data
    *
    * @details If the signal is a CAN signal this function allows setting its
    * raw data based on a provided scaled value.
    *
    * @param value The scaled value to be set
    *
    * @return true if set operation succeeded, otherwise false
    *
    ****************************************************************************
    */
   bool set_scaled_value(double value);
private:
   Signal(std::string name,
         const _internal::signal_def* sig_def,
         DATA data) :
      m_name(std::move(name)),
      m_sig_def(sig_def),
      m_data(std::move(data)) {}

   std::string m_name;
   const _internal::signal_def* m_sig_def = nullptr;
   int m_offset;
   DATA m_data;

   friend class SignalGroup;
   friend class SignalAdapter;
};

//------------------------------------------------------------------------------

/**
 *******************************************************************************
 *
 * @brief A container instance of multiple signals
 *
 * @details For CAN related data, a SignalGroup represents a CAN message
 *
 *******************************************************************************
 */
class SignalGroup
{
public:
   /**
    ****************************************************************************
    *
    * @brief typedef of the Signal vector
    *
    ****************************************************************************
    */
   typedef std::vector<Signal> SignalVec;

   /**
    ****************************************************************************
    *
    * @brief The CAN bus used for this SignalGroup
    *
    ****************************************************************************
    */
   enum CanBus
   {
      CAN_BUS_1,       ///< CAN bus 1
      CAN_BUS_2,       ///< CAN bus 2
      CAN_BUS_NO_CAN,  ///< Not related to any CAN
   };

   /**
    ****************************************************************************
    *
    * @brief The CAN variant used for this SignalGroup
    *
    ****************************************************************************
    */

   /**
    ****************************************************************************
    *
    * @brief Equal operator
    *
    * @note The comparison will ignore the timestamp!
    *
    * @param other The element to compare to
    *
    * @return true if equal otherwise false
    *
    ****************************************************************************
    */
   bool operator==(const SignalGroup& other) const;

   /**
    ****************************************************************************
    *
    * @brief Not Equal operator
    *
    * @note The comparison will ignore the timestamp!
    *
    * @param other The element to compare to
    *
    * @return false if equal otherwise true
    *
    ****************************************************************************
    */
   bool operator!=(const SignalGroup& other) const
   {
      return ! (*this == other);
   }

   /**
    ****************************************************************************
    *
    * @brief The topic which the SignalGroup relates to
    *
    * @return The topic as string. In case the SignalGroup is not related to
    * any topic, empty string will be returned
    *
    ****************************************************************************
    */
   const std::string& topic() const { return m_topic; }


   /**
    ****************************************************************************
    *
    * @brief The name of the SignalGroup
    *
    * @details In case of CAN message, this will be the CAN message name
    *
    * @return The name of the SignalGroup, if not set, an empty string will be
    * returned
    *
    ****************************************************************************
    */
   const std::string& name() const { return m_name; }

#ifndef DOXYGEN_IGNORE
   /**
    ****************************************************************************
    *
    * @brief DataLayer identifier from the RT partition
    *
    * @note Regular applications will not make use of this function
    *
    * @return the real-time id
    *
    ****************************************************************************
    */
   unsigned int rtid() const { return m_rtid; }
#endif /* ifndef DOXYGEN_IGNORE */

   /**
    ****************************************************************************
    *
    * @brief The CAN bus associated to the SignalGroup
    *
    * @note If the SignalGroup does not relate to a CAN message
    * CAN_BUS_NO_CAN will be returned
    *
    * @return the CanBus
    *
    ****************************************************************************
    */
   CanBus can_bus() const  { return m_can_bus; }

   /**
    ****************************************************************************
    *
    * @brief Add a signal to the SignalGroup
    *
    * @details This function will be used by SignalAdapter when creating
    * a SignalGroup related to a topic
    *
    ****************************************************************************
    */
   inline void add_signal(Signal signal)
   {
      m_signals.push_back(std::move(signal));
   }
   /**
    ****************************************************************************
    *
    * @brief Stores the set signals on a vector
    *
    * @details This function will be used by SignalAdapter to keep track of the
    * set signals
    *
    ****************************************************************************
    */
   void add_set_signals(const std::string& sig_name);

   /**
    ****************************************************************************
    *
    * @brief Returns all Signals for the SignalGroup
    *
    * @return A SignalVec of the all contained signals
    *
    ****************************************************************************
    */
   const SignalVec& signals() const { return m_signals; }

   /**
    ****************************************************************************
    *
    * @brief Returns all set Signals names for the SignalGroup
    *
    * @return A vector of the all set signals names
    *
    ****************************************************************************
    */
   const std::vector<std::string> set_signals() const {

      return m_set_signals;
   }

   /**
    ****************************************************************************
    *
    * @brief Returns a const pointer to Signal matching the provided name
    *
    * @details Returns a const pointer to a Signal (which is owned by the
    * SignalGroup instance) when found by name in signal vector of SignalGroup.
    * If not nullptr will be returned.
    *
    * @note The search for name using the __signal_name__ parameter is done
    * case-insensitive.
    *
    * @param signal_name Name of the signal that shall be returned
    *
    * @return const pointer to a signal if found, otherwise return **nullptr**
    *
    ****************************************************************************
    */
   const Signal* signal(const std::string& signal_name) const;

   /**
    ****************************************************************************
    *
    * @brief Is the SignalGroup and its topic handle retained on the MQTT
    *
    * @details In case the SignalGroup is a CAN message this could be
    * a CAN transmission timeout etc.
    *
    * @return true if the signal group and handle is retained, otherwise false
    *
    ****************************************************************************
    */
   bool is_retain() const { return m_retain; }

   /**
    ****************************************************************************
    *
    * @brief The original timestamp when the SignalGroup was created
    *
    * @details This relates to the creation of the CAN message on the real-time
    * partition.
    *
    * The returned timestamp is in ms since the boot of the system.
    *
    * @return timestamp in ms
    *
    ****************************************************************************
    */
   uint64_t timestamp() const { return m_timestamp; }

   /**
    ****************************************************************************
    *
    * @brief Sets the timestamp of a signal group, e.g. for testing
    *
    * @note When a signal has been received via RT the timestamp does NOT need
    * to be set manually.
    *
    * @param timestamp in ms
    *
    ****************************************************************************
    */
    void set_timestamp(uint64_t timestamp) { m_timestamp = timestamp; }

   /**
    ****************************************************************************
    *
    * @brief Checks if the signal group contains valid signals
    *
    * @details Will iterate through contained signals an checks if status of the
    * signals is Signal::STATUS_OK
    *
    * return Returns true if all contained signals status is Signal::STATUS_OK,
    * other false
    *
    ****************************************************************************
    */
    bool is_valid() const;

   /**
    ****************************************************************************
    *
    * @brief Set the data for a given signal by name
    *
    * @details The get_signals function will only return a constant SignalVec
    * of the signals contained in this SignalGroup. In order to set data to an
    * actual signal, this function should be used.
    *
    * If the SignalGroup does not contain a signal with the name provided or
    * the provided data is too big, the function will return false
    *
    * @return true if setting of data for given signal name succeeded otherwise
    * false
    *
    * @sa Signal::set_data
    *
    ****************************************************************************
    */
   bool set_signal_data(const std::string& signal_name,
                        const Signal::DATA& data);

   /**
    ****************************************************************************
    *
    * @brief Set signal data as a scaled value
    *
    * @details If the desired signal is a CAN signal, the function set its signal
    * data based on its scaled value, for example: set the actual
    * temperature instead of its raw byte representation for a signal that
    * represents a temperature value.
    *
    * If the SignalGroup does not contain a signal with the name provided or
    * the provided data is too big, the function will return false
    *
    * @return true if setting of data for given signal name succeeded otherwise
    * false
    *
    * @sa Signal::set_scaled_value
    *
    * @param signal_name The name of the signal which should be set
    * @param value The scaled value which should be converted to raw data
    * and set
    *
    ****************************************************************************
    */
   bool set_signal_scaled_value(const std::string& signal_name, double value);

   /**
    ****************************************************************************
    *
    * @brief Queries if a signal with the given name exists in the SignalGroup
    *
    * @note The search for name using the __name__ parameter is done
    * case-insensitive.
    *
    * @return True if the signal exists, otherwise false
    *
    ****************************************************************************
    */
   bool has_signal(const std::string& signal_name) const;

private:
   SignalGroup() {};
   SignalGroup(
         unsigned int rtid,
         std::string topic,
         const std::string& name,
         CanBus bus,
         bool retain,
         uint64_t timestamp);

   std::string m_topic;
   unsigned int m_rtid;
   std::string m_name;
   SignalVec m_signals;
   std::vector<std::string> m_set_signals;
   CanBus m_can_bus;
   bool m_retain;
   uint64_t m_timestamp;

   friend class SignalAdapter;
};

/**
 ****************************************************************************
 *
 * @brief Get the string representation of provided CanBus
 *
 * @param cb CanBus enum description
 * @return CanBus string description
 *
 ****************************************************************************
 */
const char* to_str(const SignalGroup::CanBus cb);


//------------------------------------------------------------------------------


/**
 *******************************************************************************
 *
 * @brief Adapter for packing and unpacking of SignalGroups from RT
 *
 * @details All data that will be received from the real-time partition will
 * always be of type SignalGroup which contains multiple Signal.
 *
 * Applications that want to receive or send rt data, shall use the standard
 * subscription and publishing mechanisms as described in
 * the com::ITransportClient class (subscription to rt/# topics).
 *
 * @note The SignalAdapter can be obtained via instance() since it is handled
 * as a singleton instance.
 * **For unit tests**, see enforce_definition() or enforce_support_lib()
 *
 * @par Unpack received RT data
 *
 * The implementation of the com::ISubscriptionObserver interface will receive
 * callbacks when a message arrives on the subscribed topic. The callback
 * function com::ISubscriptionObserver::message() will indicate the topic as
 * a std::string and a com::Message instance which contains the actual payload.
 *
 * Such received payload should then be provided to the singleton instance of the
 * SignalAdapter in order to unpack the payload into a SignalGroup. Therefore,
 * SignalAdapter::unpack() should be called.
 *
 * The unpack function will return an instance SignalAdapter::UnpackResult
 * which, in case the unpacking succeeded, will carry a SignalGroup instance
 * (SignalAdapter::UnpackResult::take_val()).
 *
 * _Example for unpacking rt data received on a rt topic_
 * @code
 * // implementation of com::ISubscriptionObserver interface, message()
 * // callback function
 *
 * void
 * TestSubObserver::message(const std::string& topic, const Message& message)
 * {
 *    // if more topics are subscribed, check if it is an rt topic
 *    if ( topic.compare(0, "rt/", 3) != 0)
 *    { // other topic }
 *
 *    // SignalAdapter singleton instance should be used
 *    auto unpack_result = SignalAdapter::instance().unpack(topic, message);
 *
 *    if ( unpack_result.nok() )
 *    { // Unpacking failed
 *       LOG(WARNING) << "Unpacking of SignalGroup from topic '" << topic
 *       << "' failed with: " << unpack_result.err_val().details;
 *       return;
 *    }
 *
 *    auto signal_group = unpack_result.take_val();
 *
 *    // check status, go through signals etc.
 * }
 *
 * @endcode
 *
 * @par Pack RT data for sending
 *
 * Some real-time topics allow sending data, for example, setting an LED or
 * sending a CAN message maintenance request. As all data is
 * handled as SignalGroup (see above) on rt related topics, one needs to create
 * a **topic specific** SignalGroup instance which is then to be used together
 * with a com::ITransportClient instance to publish it on MQTT.
 *
 * The SignalAdapter is aware of SignalGroup format (and contained signals)
 * which should be used for a specific topic. Therefore,
 * SignalAdapter::create_sg() should be used to create a SignalGroup instance
 * (topic as parameter). The returned SignalGroup can now be filled
 * with actual data: setting raw data to all contained signals using
 * SignalGroup::set_signal_data().
 *
 * After all data has been set, SignalAdapter::pack() shall be called to pack
 * the SignalGroup into a com::Message instance which can then be used for
 * sending it on MQTT using a com::ITransportClient.
 *
 * _Example for packing and sending rt data to CAN topic_
 * @code
 *
 * // send data on rt/system/status_gnss_antenna_open_circuit
 * std::string topic = "rt/system/status_gnss_antenna_open_circuit";
 *
 * // topics under rt/system/ and rt/io are represented with a SignalGroup
 * // containing only one signal, where the signal name is the sanme as the
 * // the SignalGroup name.
 * auto signal_name = "status_gnss_antenna_open_circuit_signal";
 *
 * // note: 'auto&' need to be used, because the copy-ctor has been deleted
 * auto& signal_adapter = SignalAdapter::instance();
 *
 * auto sg_create_result = signal_adapter.create_sg(topic);
 * if ( sg_create_result.nok() )
 * {
 *    LOG(WARNING) << "unable to create SignalGroup for topic: " << topic;
 *    return;
 * }
 *
 * auto signal_group = sg_create_result.take_val();
 *
 * // single byte signal!
 * Signal::DATA data{1};
 * if ( ! signal_group.set_signal_data(signal_name, data) )
 * {
 *    LOG(WARNING) << "Unable to set data for signal: " << signal_name << \'n';
 *    return;
 * }
 *
 * // pack SignalGroup into com::Message instance
 * auto pack_result = signal_adapter.pack(signal_group);
 *
 * if ( pack_result.nok() )
 * { // packing failed
 *    LOG(WARNING) << "packing of SignalGroup for topic '" << topic
 *                 << "' failed with: " << pack_result.err_val().details;
 *    return;
 * }
 *
 * // take the message from result!
 * auto message = pack_result.take_val();
 *
 * // NOTE: it is assumed a com::ITransportClient_ptr transport_client has
 * // been created beforehand!
 *
 * // publish message
 * transport_client->publish(topic, message);
 * @endcode
 *
 * @par Relation SignalGroup to CAN message and other RT data
 *
 * A **CAN message** contains multiple signals, which can be considered a group,
 * hence the name SignalGroup. If a can message is received (on topics of the
 * format `rt/(fms|[i|c]can)/#`), the extracted SignalGroup will represent this
 * CAN message.
 *
 * **Other real-time topics, such as `rt/io/#` and `rt/system/#`** will also be
 * packed and unpacked as a SignalGroup containing one Signal
 * (`SignalGroup::signals().size() == 1`). The single Signal will have the same name
 * as the SignalGroup. This is required  when e.g. setting signal data using
 * SignalGroup::set_signal_data().
 *
 * Dynamic data messages will always contain "nof_bytes" and "data" signals.
 * Total length of "data" payload is given by "nof_bytes".
 *
 * For __sending__ dynamic data, the total amount of bytes must be set with
 * SignalGroup::set_signal_scaled_value("nof_bytes", total_bytes).
 *
 * @code
 * rt::Signal::DATA data(payload.begin(), payload.end());
 * auto& adapter = rt::SignalAdapter::instance();
 * auto sg = adapter.create_sg(topicname).take_val();
 *
 * sg.set_signal_data("data", data);
 * sg.set_signal_scaled_value("nof_bytes",
 *                            payload.size()/length());
 * @endcode
 *
 * For __reading__ dynamic data, "nof_bytes" must be read and only this amount
 * of data must be considered from "data" payload.
 *
 * @code
 * // Let `sg` be a valid `SignalGroup` instance from an RT topic.
 * // Skipped checking for sake of simplicity.
 * auto sig_data = sg.signal("data");
 *
 * const auto signal = sg.signal("nof_bytes");
 *
 * if ( !signal )
 * {
 *    // Log error and go out;
 *    return;
 * }
 *
 * auto length = signal->value_scaled();
 * if ( !length.ok() )
 * {
 *    // Log error and go out;
 *    return;
 * }
 *
 * // Use `length` to extract relevant data from your payload.
 * std::stringstream ssr;
 * for (int i = 0; i < length; ++i)
 *     ssr << std::hex << (int)sig_data->data()[i] << ' ';
 * @endcode
 *
 * @par Unit tests in SDK environment
 *
 * When unit tests are executed within a __SDK environment__ on qemu-arm, the
 * SignalAdapter::instance() will __automatically load__ the lapi rtsupport shipped
 * with the SDK. Therefore, the mapping stub, as defined in the paragraph below
 * do not need to be used.
 *
 * @note The library shipped with the SDK contains exactly the same mapping data
 * definition as the support library loaded on target.
 *
 * In case another lapi rtsupport library should be loaded, this can be
 * enforced with the enforce_support_lib() function.
 *
 * @par Unit tests with mapping definition
 *
 * For unit tests, a stub of the SignalAdapter mapping defintion can be created
 * according to the following examples. Follow the definition described in the
 * lapi_rtsupport.html documentation and set the values according to what is
 * expected in the test.
 *
 * To create a stub for the SignalAdapter for CAN messages
 * @code
 * // Note: TcoTimeSource is a class defined in another header-file
 *
 * # include <ecu/rt/signaladapter.h>
 *
 * class StubSGDef : public SignalGroupsDefinition
 * {
 *    public:
 *       TopicSignalGroupDefMap get_topics_group_mapping() const override {
 *          return
 *          {
 *
 * //          TopicSignalGroupDefPair(<full-topic>,
 * //             SignalGroupDef (
 * //                { <rtid>, "<signal-group-name>", <can-bus>, <can-variant>, <is-retained>, },  // canmsg_def
 * //                {
 * //                   { "<signal-name-char*>", <data-type-size-bits-int>, <raw-sig-size-bits-int>, <factor-double>, <min-uint>, <max-uint>, <offset-int>, <unit-str>, <status-eval-type> },
 * //                   // more signals ...
 * //                }
 * //            )
 * //          ),
 * //          Note: <data-type-size-bits-int> e.g. uint8_t is 8 bits
 * //
 * //          whereas status_evaluation is;
 * //              status_evaluation (enum):
 * //                  status_eval_j1939 (0)  // for can signals
 * //                  status_eval_datatype_max (1) // for other signals, e.g. rt/io or rt/system
 * //                  status_eval_enum (2) // table values (see lapi signal definition documentation)
 * //                  status_eval_with_status (3)  // used internal
 * //                  status_eval_none (4) // when the status of the signal should not be evaluated at all
 *
 *
 *             TopicSignalGroupDefPair("rt/j1939can/rx/td_ee",//TcoTimeSource::TOPIC_TIMEDATE,
 *                   SignalGroupDef (
 *                      { 172, "timedate_var1", can_bus_1, false, },
 *                      {
 *                        { TcoTimeSource::SIG_N_DAY, 8, 1*8, 0.25, 0,  250, 0, "-" , status_eval_j1939},
 *                        { TcoTimeSource::SIG_N_HOURS,8,  1*8, 1.0, 0,  250, 0, "-" , status_eval_j1939 },
 *                        { "localhouroffset_basis", 8, 1*8, 1.0,  0,  250, -125, "-" , status_eval_j1939, },
 *                        { "localminuteoffset_basis", 8, 1*8, 1.0, 0,  250, -125, "-" , status_eval_j1939},
 *                        { TcoTimeSource::SIG_N_MINS, 8, 1*8, 1.0,  0,  250, 0, "-" , status_eval_j1939 },
 *                        { TcoTimeSource::SIG_N_MONTH, 8, 1*8, 1.0, 0,  250, 0, "-" , status_eval_j1939 },
 *                        { TcoTimeSource::SIG_N_SECS, 8, 1*8, 0.25, 0,  250, 0, "-" , status_eval_j1939 },
 *                        { TcoTimeSource::SIG_N_YEAR, 8, 1*8, 1.0,  0,  250, 1985, "-" , status_eval_j1939 },
 *                        }
 *                      )
 *                   ),
 *          };
 *       }
 * };
 * @endcode
 *
 * To create a stub for the SignalAdapter for other messages then CAN messages
 * @code
 * class StubSGDef : public SignalGroupsDefinition
 * {
 *    public:
 *       TopicSignalGroupDefMap get_topics_group_mapping() const override {
 *         return
 *         {
 *            TopicSignalGroupDefPair("rt/system/vehicle_id",
 *               SignalGroupDef (
 *                  { 195, "vehicle_id", can_bus_no_can, true, },
 *                  {
 *                  { "data", 8, 18*8, 1.0, 0,  0,  0,"-", status_eval_none },
 *                  { "length", 8,  1*8, 1.0, 0,  0,  0, "-" , status_eval_none,},
 *                  }
 *                  )
 *               ),
 *          };
 *      }
 * };
 * @endcode
 *
 * Use the stub in the unit test program like this
 * @code
 * TEST(TestEnvVar, VehicleIdVariable)
 * {
 *    // enforce definition!!!!
 *    rt::SignalAdapter::enforce_definition(new StubSGDef());
 *    auto& adapter = rt::SignalAdapter::instance();
 *
 *    auto sg = adapter.create_sg("rt/system/vehicle_id").take_val();
 *
 *    // the actual test ...........
 * }
 * @endcode
 *
 *******************************************************************************
 */
class SignalAdapter
{
public:
   /**
    ****************************************************************************
    *
    * @brief struct used for errors occurring during pack and unpack
    *
    * @sa UnpackResult PackResult
    *
    ****************************************************************************
    */
   struct PackagingError
   {
      /**
       * @brief Error code for the PackagingError
       */
      enum Code
      {
         ERROR_INVALID_TOPIC,     ///< topic unknown with the mapping definition
         ERROR_PAYLOAD_TOO_SMALL, ///< provided payload of raw data, e.g. unpack,
                                  ///< is smaller then expected!
         ERROR_PB_ERROR,          ///< Error during ProtoBuf operation when
                                  ///< packing or unpacking a SignalGroup
      };
      Code code;              ///< The error code
      std::string details;    ///< additional information about the error
   };

   /**
    ****************************************************************************
    *
    * @brief Result type for unpack operation
    *
    * @details If successful, take_val() will be the desired SignalGroup
    * otherwise PackagingError will be set
    *
    ****************************************************************************
    */
   using UnpackResult = Result<SignalGroup, PackagingError>;

   /**
    ****************************************************************************
    *
    * @brief Result type for pack operation
    *
    * @details If successful, take_val() will be the desired com::Message
    * otherwise PackagingError will be set
    *
    ****************************************************************************
    */
   using PackResult = Result<com::Message, PackagingError>;

   /**
    ****************************************************************************
    *
    * @brief Result type for create_sg operation
    *
    * @details If successful, take_val() will be the desired SignalGroup
    * otherwise an error message will be set (err_val()).
    *
    ****************************************************************************
    */
   using CreateSGResult = Result<SignalGroup, std::string>;

   /**
    ****************************************************************************
    *
    * @brief Checks if given topic is known within the SignalAdapter mapping
    *
    * @details Will check the mapping if the given topic is defined.
    *
    * @param topic The topic to check
    *
    * @return true if topic is valid, otherwise false.
    *
    ****************************************************************************
    */
   bool is_valid_topic(const std::string& topic) const;

   /**
    ****************************************************************************
    *
    * @brief Get all topics known to the signal adapter
    *
    * @details Returns a vector of all known RT signal topics,
    * optionally filtered by the specified prefix
    *
    * @param prefix (optional) to filter the resulting list (e.g. "rt/ican")
    *
    * @return vector of the matching topics
    *
    ****************************************************************************
    */
   std::vector<std::string> get_topics(const std::string& prefix = "") const;

   /**
    ****************************************************************************
    * @brief Adds padding bytes to data field
    *
    * @param sg The signal group of which data should be padded
    *
    * @return A vector with the padding bytes
    ****************************************************************************
    */

   std::string add_padding(const SignalGroup& sg) const;

   /**
    ****************************************************************************
    *
    * @brief Unpacks a given com::Message based on the provided topic
    *
    * @details The given topic dictates how the provided com::Message should
    * be unpacked. If the topic does not exist or the payload of the
    * com::Message does not match the expected mapping definition a negative
    * result will be returned.
    *
    * Internally the given message will first be deserialized using a
    * com::PbInternalMessageAdapter and then tried to be mapped to the
    * structure related to the topic. Hence, the result type can return
    * different error types.
    *
    * @param topic The topic which the message should be matched to
    * @param msg The raw message which should be unpacked
    *
    * @return An UnpackResult which indicates if the unpack operation succeeded
    * (result.ok()) or not
    *
    ****************************************************************************
    */
   UnpackResult unpack(const std::string& topic, const com::Message& msg) const;


   /**
    ****************************************************************************
    *
    * @brief Packs a given SignalGroup as a com::Message
    *
    * @details The provided SignalGroup is always related to a topic (
    * see SignalGroup::topic()), therefore a topic does not need to be
    * provided. However, it is crucial that a correct SignalGroup instance
    * has been created for a topic, thus create_sg() should be used when
    * creating a new SignalGroup instance.
    *
    * Internally the SignalGroup will first be packed into a byte representation
    * and then serialized using a com::PbInternalMessageAdapter. Hence, different
    * errors can occur which is indicated in the return PackResult.
    *
    * @param signal_group The SignalGroup which should be packed into a
    * com::Message
    *
    * @return a PackResult which indicates if the pack operation succeeded
    * (result.ok()) or not
    *
    ****************************************************************************
    */
   PackResult pack(const SignalGroup& signal_group) const;

   /**
    ****************************************************************************
    *
    * @brief Creates a SignalGroup instance for a provided topic
    *
    * @details The adapter will create a SignalGroup for the provided topic,
    * setting all sizes for the containing signals. The returned group signals
    * can then be updated with the desired data using
    * SignalGroup::set_signal_data().
    *
    * If the given topic is not defined within the internal mapping, the
    * return type will indicate an error.
    *
    * @param topic The topic for which a SignalGroup shall be created
    *
    * @return Result type indicating if the creation succeeded
    *
    ****************************************************************************
    */
   CreateSGResult create_sg(const std::string& topic) const;

#ifndef DOXYGEN_IGNORE
   /**
    ****************************************************************************
    *
    * @brief Maps a provided realtime id to a topic
    *
    * @details All SignalGroups relate to a topic and unique id which is
    * derived from the DataLayer definition on the real-time partition.
    *
    * When raw data is received from the real-time partition only the rtid is
    * known. In order to relate this to a topic, this function should be used.
    *
    * @note Regular applications will not make use of this function
    *
    * @param rtid The real-time id which is to be mapped to a topic
    *
    * @return a string containing the topic for the rtid, or an empty string if
    * the rtid is known or no mapping exists.
    *
    ****************************************************************************
    */
   std::string map_rtid_to_topic(unsigned int rtid) const;

   /**
    ****************************************************************************
    *
    * @brief Maps a provided topic to a real-time id
    *
    * @details All SignalGroups relate to a topic and unique id which is
    * derived from the DataLayer definition on the real-time partition.
    *
    * When raw data is to be sent to the real-time partition, only the topic
    * is known. In order to relate the topic to a real-time id, this function
    * should be used.
    *
    * @note Regular applications will not make use of this function
    *
    * @param topic The topic which is to be mapped to a real-time id
    *
    * @return an unsigned reprinting the rtid, or 0 if the topic is not defined
    *
    ****************************************************************************
    */
   unsigned int map_topic_to_rtid(const std::string& topic) const;

   /**
    ****************************************************************************
    *
    * @brief Is a provided topic marked as retained
    *
    * @details Some rt related topics will have their data retained by the
    * MQTT broker.
    *
    * @note Regular applications will not make use of this function
    * This function can be used to identify if the given topic is retained
    *
    * @note Regular applications will not make use of this function
    *
    * @param topic the topic to check
    * @return is marked as retained or not
    *
    ****************************************************************************
    */
   bool is_topic_retained(const std::string& topic) const;
#endif /* ifndef DOXYGEN_IGNORE */

   ~SignalAdapter();

   /**
    ****************************************************************************
    *
    * @brief Delete assignment operator
    *
    ****************************************************************************
    */
   SignalAdapter& operator=(const SignalAdapter&) = delete;

   /**
    ****************************************************************************
    *
    * @brief Delete copy constructor
    *
    ****************************************************************************
    */
   SignalAdapter(const SignalAdapter&) = delete;

   /**
    ****************************************************************************
    *
    * @brief Delete default constructor
    *
    ****************************************************************************
    */
   SignalAdapter() = delete;


   /**
    ****************************************************************************
    *
    * @brief Enforces the SignalAdapter instance to use a predefined SG definition
    *
    * @pre The provided definition must not be nullptr
    *
    * @details In order to support testing, the SignalAdapter
    * singleton instance can be enforced to use a specific SignalGroup definition
    * using this function.
    *
    * Unit tests should call this function with a predefined definition,
    * which should reflect what is expected in production environment.
    *
    * Calling the function twice will delete the definition provided in the
    * first function call.
    *
    * @note The provided **def** will be owned by the SignalAdapter instance.
    *
    * @param def The predefined SignalGroup definition
    *
    ****************************************************************************
    */
   static void enforce_definition(const _internal::SignalGroupsDefinition* def);

   /**
    ****************************************************************************
    *
    * @brief Enforces the SignalAdapter instance load the provided rtsupport lib
    *
    * @pre The provided path fqn_lib_path must exist
    *
    * @details In order to support testing, the SignalAdapter
    * singleton instance can be enforced to load a mapping definition from a
    * provided lapi rtsupport library using this function.
    *
    * By default the SignalAdapter will try to load either the
    * `LAPI_RT_SUPPORT_LIB =
    * /etc/alex/libcslclapi_rtsupport.so` on the target, or
    * `$ENV("SDKTARGETSYSROOT")/LAPI_RT_SUPPORT_LIB` when executed in a SDK
    * environment running within qemu-arm.
    *
    * Calling this function will delete and reset the previous set definition,
    * e.g. if enforce_definition was used.
    *
    * @param fqn_lib_path The fully qualified name/path of the lapi rtsupport
    * library
    *
    ****************************************************************************
    */
   static void enforce_support_lib(const std::string& fqn_lib_path);

    /**
     ****************************************************************************
     *
     * @brief Returns a singleton instance of the SignalAdapter
     *
     * @details In case enforce_definition has been called before, the function
     * will return the SignalAdapter using the SignalGroup definition provided
     * in the enforce_definition call - useful in unit test environment.
     *
     * In production code, the SignalAdapter instance will be instantiate with
     * lapi support library located on the target system.
     *
     * @note In case the SignalAdapter instance could not be created - this
     * call will assert!
     *
     * @return A reference to the singleton SignalAdapter instance
     *
     ****************************************************************************
     */
   static const SignalAdapter& instance();

   /**
    ****************************************************************************
    *
    * @brief Reset the singleton instance of SignalAdapter and free it's resources
    *
    * @details It is useful to be able to reset / delete the SignalAdapter instance
    * in Unit Tests and when memory profiling an application
    *
    * @warning It is up to the user to make sure it is safe to delete the instance
    * and that no other thread is currently accessing it's resources!
    *
    ****************************************************************************
    */
   static void reset_instance();


private:
   SignalAdapter(const _internal::SignalGroupsDefinition* def);

   _internal::SignalGroupsDefinition::TopicSignalGroupDefMap& get_mapping() const;


   const _internal::SignalGroupsDefinition* m_definition = nullptr;
   mutable _internal::SignalGroupsDefinition::TopicSignalGroupDefMap m_topic_map;

   typedef std::pair<unsigned int, std::reference_wrapper<const std::string>> RtIdMapPair;
   typedef std::map<RtIdMapPair::first_type, RtIdMapPair::second_type > RtId2TopicMap;
   mutable RtId2TopicMap m_rtid2topic_map;

   static SignalAdapter* c_instance;
   static std::string c_rt_support_library;
};

//------------------------------------------------------------------------------

/**
 ****************************************************************************
 *
 * Convenience typedef to a shared pointer type of this SignalAdapter class
 *
 ****************************************************************************
 */
//using SignalAdapter_ptr = SignalAdapter::sptr;

//------------------------------------------------------------------------------

}}} // namespace

#endif /* SIGNALADAPTER_H */
