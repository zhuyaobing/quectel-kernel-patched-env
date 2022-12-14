// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: com/scv/protobuf/mainmessage/Messages.proto

#ifndef PROTOBUF_com_2fscv_2fprotobuf_2fmainmessage_2fMessages_2eproto__INCLUDED
#define PROTOBUF_com_2fscv_2fprotobuf_2fmainmessage_2fMessages_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_com_2fscv_2fprotobuf_2fmainmessage_2fMessages_2eproto();
void protobuf_AssignDesc_com_2fscv_2fprotobuf_2fmainmessage_2fMessages_2eproto();
void protobuf_ShutdownFile_com_2fscv_2fprotobuf_2fmainmessage_2fMessages_2eproto();

class MainMessage;
class MainMessage_RefTimeStamp;
class MainMessage_Driver;

enum MainMessage_TimeSrc {
  MainMessage_TimeSrc_unknown = 0,
  MainMessage_TimeSrc_gps = 1,
  MainMessage_TimeSrc_dtco = 2,
  MainMessage_TimeSrc_gsm = 3,
  MainMessage_TimeSrc_server = 4
};
bool MainMessage_TimeSrc_IsValid(int value);
const MainMessage_TimeSrc MainMessage_TimeSrc_TimeSrc_MIN = MainMessage_TimeSrc_unknown;
const MainMessage_TimeSrc MainMessage_TimeSrc_TimeSrc_MAX = MainMessage_TimeSrc_server;
const int MainMessage_TimeSrc_TimeSrc_ARRAYSIZE = MainMessage_TimeSrc_TimeSrc_MAX + 1;

const ::google::protobuf::EnumDescriptor* MainMessage_TimeSrc_descriptor();
inline const ::std::string& MainMessage_TimeSrc_Name(MainMessage_TimeSrc value) {
  return ::google::protobuf::internal::NameOfEnum(
    MainMessage_TimeSrc_descriptor(), value);
}
inline bool MainMessage_TimeSrc_Parse(
    const ::std::string& name, MainMessage_TimeSrc* value) {
  return ::google::protobuf::internal::ParseNamedEnum<MainMessage_TimeSrc>(
    MainMessage_TimeSrc_descriptor(), name, value);
}
enum MainMessage_DriverIdType {
  MainMessage_DriverIdType_eu_tachograph_card = 0,
  MainMessage_DriverIdType_driver_license = 1
};
bool MainMessage_DriverIdType_IsValid(int value);
const MainMessage_DriverIdType MainMessage_DriverIdType_DriverIdType_MIN = MainMessage_DriverIdType_eu_tachograph_card;
const MainMessage_DriverIdType MainMessage_DriverIdType_DriverIdType_MAX = MainMessage_DriverIdType_driver_license;
const int MainMessage_DriverIdType_DriverIdType_ARRAYSIZE = MainMessage_DriverIdType_DriverIdType_MAX + 1;

const ::google::protobuf::EnumDescriptor* MainMessage_DriverIdType_descriptor();
inline const ::std::string& MainMessage_DriverIdType_Name(MainMessage_DriverIdType value) {
  return ::google::protobuf::internal::NameOfEnum(
    MainMessage_DriverIdType_descriptor(), value);
}
inline bool MainMessage_DriverIdType_Parse(
    const ::std::string& name, MainMessage_DriverIdType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<MainMessage_DriverIdType>(
    MainMessage_DriverIdType_descriptor(), name, value);
}
// ===================================================================

class MainMessage_RefTimeStamp : public ::google::protobuf::Message {
 public:
  MainMessage_RefTimeStamp();
  virtual ~MainMessage_RefTimeStamp();

  MainMessage_RefTimeStamp(const MainMessage_RefTimeStamp& from);

  inline MainMessage_RefTimeStamp& operator=(const MainMessage_RefTimeStamp& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const MainMessage_RefTimeStamp& default_instance();

  void Swap(MainMessage_RefTimeStamp* other);

  // implements Message ----------------------------------------------

  MainMessage_RefTimeStamp* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MainMessage_RefTimeStamp& from);
  void MergeFrom(const MainMessage_RefTimeStamp& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional uint64 utc = 1;
  inline bool has_utc() const;
  inline void clear_utc();
  static const int kUtcFieldNumber = 1;
  inline ::google::protobuf::uint64 utc() const;
  inline void set_utc(::google::protobuf::uint64 value);

  // optional .MainMessage.TimeSrc src = 2;
  inline bool has_src() const;
  inline void clear_src();
  static const int kSrcFieldNumber = 2;
  inline ::MainMessage_TimeSrc src() const;
  inline void set_src(::MainMessage_TimeSrc value);

  // optional uint32 powerup_cycle = 3;
  inline bool has_powerup_cycle() const;
  inline void clear_powerup_cycle();
  static const int kPowerupCycleFieldNumber = 3;
  inline ::google::protobuf::uint32 powerup_cycle() const;
  inline void set_powerup_cycle(::google::protobuf::uint32 value);

  // optional uint64 ticks = 4;
  inline bool has_ticks() const;
  inline void clear_ticks();
  static const int kTicksFieldNumber = 4;
  inline ::google::protobuf::uint64 ticks() const;
  inline void set_ticks(::google::protobuf::uint64 value);

  // @@protoc_insertion_point(class_scope:MainMessage.RefTimeStamp)
 private:
  inline void set_has_utc();
  inline void clear_has_utc();
  inline void set_has_src();
  inline void clear_has_src();
  inline void set_has_powerup_cycle();
  inline void clear_has_powerup_cycle();
  inline void set_has_ticks();
  inline void clear_has_ticks();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint64 utc_;
  int src_;
  ::google::protobuf::uint32 powerup_cycle_;
  ::google::protobuf::uint64 ticks_;
  friend void  protobuf_AddDesc_com_2fscv_2fprotobuf_2fmainmessage_2fMessages_2eproto();
  friend void protobuf_AssignDesc_com_2fscv_2fprotobuf_2fmainmessage_2fMessages_2eproto();
  friend void protobuf_ShutdownFile_com_2fscv_2fprotobuf_2fmainmessage_2fMessages_2eproto();

  void InitAsDefaultInstance();
  static MainMessage_RefTimeStamp* default_instance_;
};
// -------------------------------------------------------------------

class MainMessage_Driver : public ::google::protobuf::Message {
 public:
  MainMessage_Driver();
  virtual ~MainMessage_Driver();

  MainMessage_Driver(const MainMessage_Driver& from);

  inline MainMessage_Driver& operator=(const MainMessage_Driver& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const MainMessage_Driver& default_instance();

  void Swap(MainMessage_Driver* other);

  // implements Message ----------------------------------------------

  MainMessage_Driver* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MainMessage_Driver& from);
  void MergeFrom(const MainMessage_Driver& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional string identification = 1;
  inline bool has_identification() const;
  inline void clear_identification();
  static const int kIdentificationFieldNumber = 1;
  inline const ::std::string& identification() const;
  inline void set_identification(const ::std::string& value);
  inline void set_identification(const char* value);
  inline void set_identification(const char* value, size_t size);
  inline ::std::string* mutable_identification();
  inline ::std::string* release_identification();
  inline void set_allocated_identification(::std::string* identification);

  // optional .MainMessage.DriverIdType identification_type = 2;
  inline bool has_identification_type() const;
  inline void clear_identification_type();
  static const int kIdentificationTypeFieldNumber = 2;
  inline ::MainMessage_DriverIdType identification_type() const;
  inline void set_identification_type(::MainMessage_DriverIdType value);

  // @@protoc_insertion_point(class_scope:MainMessage.Driver)
 private:
  inline void set_has_identification();
  inline void clear_has_identification();
  inline void set_has_identification_type();
  inline void clear_has_identification_type();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* identification_;
  int identification_type_;
  friend void  protobuf_AddDesc_com_2fscv_2fprotobuf_2fmainmessage_2fMessages_2eproto();
  friend void protobuf_AssignDesc_com_2fscv_2fprotobuf_2fmainmessage_2fMessages_2eproto();
  friend void protobuf_ShutdownFile_com_2fscv_2fprotobuf_2fmainmessage_2fMessages_2eproto();

  void InitAsDefaultInstance();
  static MainMessage_Driver* default_instance_;
};
// -------------------------------------------------------------------

class MainMessage : public ::google::protobuf::Message {
 public:
  MainMessage();
  virtual ~MainMessage();

  MainMessage(const MainMessage& from);

  inline MainMessage& operator=(const MainMessage& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const MainMessage& default_instance();

  void Swap(MainMessage* other);

  // implements Message ----------------------------------------------

  MainMessage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MainMessage& from);
  void MergeFrom(const MainMessage& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  typedef MainMessage_RefTimeStamp RefTimeStamp;
  typedef MainMessage_Driver Driver;

  typedef MainMessage_TimeSrc TimeSrc;
  static const TimeSrc unknown = MainMessage_TimeSrc_unknown;
  static const TimeSrc gps = MainMessage_TimeSrc_gps;
  static const TimeSrc dtco = MainMessage_TimeSrc_dtco;
  static const TimeSrc gsm = MainMessage_TimeSrc_gsm;
  static const TimeSrc server = MainMessage_TimeSrc_server;
  static inline bool TimeSrc_IsValid(int value) {
    return MainMessage_TimeSrc_IsValid(value);
  }
  static const TimeSrc TimeSrc_MIN =
    MainMessage_TimeSrc_TimeSrc_MIN;
  static const TimeSrc TimeSrc_MAX =
    MainMessage_TimeSrc_TimeSrc_MAX;
  static const int TimeSrc_ARRAYSIZE =
    MainMessage_TimeSrc_TimeSrc_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  TimeSrc_descriptor() {
    return MainMessage_TimeSrc_descriptor();
  }
  static inline const ::std::string& TimeSrc_Name(TimeSrc value) {
    return MainMessage_TimeSrc_Name(value);
  }
  static inline bool TimeSrc_Parse(const ::std::string& name,
      TimeSrc* value) {
    return MainMessage_TimeSrc_Parse(name, value);
  }

  typedef MainMessage_DriverIdType DriverIdType;
  static const DriverIdType eu_tachograph_card = MainMessage_DriverIdType_eu_tachograph_card;
  static const DriverIdType driver_license = MainMessage_DriverIdType_driver_license;
  static inline bool DriverIdType_IsValid(int value) {
    return MainMessage_DriverIdType_IsValid(value);
  }
  static const DriverIdType DriverIdType_MIN =
    MainMessage_DriverIdType_DriverIdType_MIN;
  static const DriverIdType DriverIdType_MAX =
    MainMessage_DriverIdType_DriverIdType_MAX;
  static const int DriverIdType_ARRAYSIZE =
    MainMessage_DriverIdType_DriverIdType_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  DriverIdType_descriptor() {
    return MainMessage_DriverIdType_descriptor();
  }
  static inline const ::std::string& DriverIdType_Name(DriverIdType value) {
    return MainMessage_DriverIdType_Name(value);
  }
  static inline bool DriverIdType_Parse(const ::std::string& name,
      DriverIdType* value) {
    return MainMessage_DriverIdType_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // optional uint32 version = 1;
  inline bool has_version() const;
  inline void clear_version();
  static const int kVersionFieldNumber = 1;
  inline ::google::protobuf::uint32 version() const;
  inline void set_version(::google::protobuf::uint32 value);

  // optional uint64 message_id = 2;
  inline bool has_message_id() const;
  inline void clear_message_id();
  static const int kMessageIdFieldNumber = 2;
  inline ::google::protobuf::uint64 message_id() const;
  inline void set_message_id(::google::protobuf::uint64 value);

  // optional string originator = 3;
  inline bool has_originator() const;
  inline void clear_originator();
  static const int kOriginatorFieldNumber = 3;
  inline const ::std::string& originator() const;
  inline void set_originator(const ::std::string& value);
  inline void set_originator(const char* value);
  inline void set_originator(const char* value, size_t size);
  inline ::std::string* mutable_originator();
  inline ::std::string* release_originator();
  inline void set_allocated_originator(::std::string* originator);

  // optional .MainMessage.RefTimeStamp time = 8;
  inline bool has_time() const;
  inline void clear_time();
  static const int kTimeFieldNumber = 8;
  inline const ::MainMessage_RefTimeStamp& time() const;
  inline ::MainMessage_RefTimeStamp* mutable_time();
  inline ::MainMessage_RefTimeStamp* release_time();
  inline void set_allocated_time(::MainMessage_RefTimeStamp* time);

  // optional uint32 content_id = 5;
  inline bool has_content_id() const;
  inline void clear_content_id();
  static const int kContentIdFieldNumber = 5;
  inline ::google::protobuf::uint32 content_id() const;
  inline void set_content_id(::google::protobuf::uint32 value);

  // optional bytes payload = 6;
  inline bool has_payload() const;
  inline void clear_payload();
  static const int kPayloadFieldNumber = 6;
  inline const ::std::string& payload() const;
  inline void set_payload(const ::std::string& value);
  inline void set_payload(const char* value);
  inline void set_payload(const void* value, size_t size);
  inline ::std::string* mutable_payload();
  inline ::std::string* release_payload();
  inline void set_allocated_payload(::std::string* payload);

  // optional .MainMessage.Driver driver = 7;
  inline bool has_driver() const;
  inline void clear_driver();
  static const int kDriverFieldNumber = 7;
  inline const ::MainMessage_Driver& driver() const;
  inline ::MainMessage_Driver* mutable_driver();
  inline ::MainMessage_Driver* release_driver();
  inline void set_allocated_driver(::MainMessage_Driver* driver);

  // @@protoc_insertion_point(class_scope:MainMessage)
 private:
  inline void set_has_version();
  inline void clear_has_version();
  inline void set_has_message_id();
  inline void clear_has_message_id();
  inline void set_has_originator();
  inline void clear_has_originator();
  inline void set_has_time();
  inline void clear_has_time();
  inline void set_has_content_id();
  inline void clear_has_content_id();
  inline void set_has_payload();
  inline void clear_has_payload();
  inline void set_has_driver();
  inline void clear_has_driver();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint64 message_id_;
  ::std::string* originator_;
  ::google::protobuf::uint32 version_;
  ::google::protobuf::uint32 content_id_;
  ::MainMessage_RefTimeStamp* time_;
  ::std::string* payload_;
  ::MainMessage_Driver* driver_;
  friend void  protobuf_AddDesc_com_2fscv_2fprotobuf_2fmainmessage_2fMessages_2eproto();
  friend void protobuf_AssignDesc_com_2fscv_2fprotobuf_2fmainmessage_2fMessages_2eproto();
  friend void protobuf_ShutdownFile_com_2fscv_2fprotobuf_2fmainmessage_2fMessages_2eproto();

  void InitAsDefaultInstance();
  static MainMessage* default_instance_;
};
// ===================================================================


// ===================================================================

// MainMessage_RefTimeStamp

// optional uint64 utc = 1;
inline bool MainMessage_RefTimeStamp::has_utc() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MainMessage_RefTimeStamp::set_has_utc() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MainMessage_RefTimeStamp::clear_has_utc() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MainMessage_RefTimeStamp::clear_utc() {
  utc_ = GOOGLE_ULONGLONG(0);
  clear_has_utc();
}
inline ::google::protobuf::uint64 MainMessage_RefTimeStamp::utc() const {
  // @@protoc_insertion_point(field_get:MainMessage.RefTimeStamp.utc)
  return utc_;
}
inline void MainMessage_RefTimeStamp::set_utc(::google::protobuf::uint64 value) {
  set_has_utc();
  utc_ = value;
  // @@protoc_insertion_point(field_set:MainMessage.RefTimeStamp.utc)
}

// optional .MainMessage.TimeSrc src = 2;
inline bool MainMessage_RefTimeStamp::has_src() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void MainMessage_RefTimeStamp::set_has_src() {
  _has_bits_[0] |= 0x00000002u;
}
inline void MainMessage_RefTimeStamp::clear_has_src() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void MainMessage_RefTimeStamp::clear_src() {
  src_ = 0;
  clear_has_src();
}
inline ::MainMessage_TimeSrc MainMessage_RefTimeStamp::src() const {
  // @@protoc_insertion_point(field_get:MainMessage.RefTimeStamp.src)
  return static_cast< ::MainMessage_TimeSrc >(src_);
}
inline void MainMessage_RefTimeStamp::set_src(::MainMessage_TimeSrc value) {
  assert(::MainMessage_TimeSrc_IsValid(value));
  set_has_src();
  src_ = value;
  // @@protoc_insertion_point(field_set:MainMessage.RefTimeStamp.src)
}

// optional uint32 powerup_cycle = 3;
inline bool MainMessage_RefTimeStamp::has_powerup_cycle() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void MainMessage_RefTimeStamp::set_has_powerup_cycle() {
  _has_bits_[0] |= 0x00000004u;
}
inline void MainMessage_RefTimeStamp::clear_has_powerup_cycle() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void MainMessage_RefTimeStamp::clear_powerup_cycle() {
  powerup_cycle_ = 0u;
  clear_has_powerup_cycle();
}
inline ::google::protobuf::uint32 MainMessage_RefTimeStamp::powerup_cycle() const {
  // @@protoc_insertion_point(field_get:MainMessage.RefTimeStamp.powerup_cycle)
  return powerup_cycle_;
}
inline void MainMessage_RefTimeStamp::set_powerup_cycle(::google::protobuf::uint32 value) {
  set_has_powerup_cycle();
  powerup_cycle_ = value;
  // @@protoc_insertion_point(field_set:MainMessage.RefTimeStamp.powerup_cycle)
}

// optional uint64 ticks = 4;
inline bool MainMessage_RefTimeStamp::has_ticks() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void MainMessage_RefTimeStamp::set_has_ticks() {
  _has_bits_[0] |= 0x00000008u;
}
inline void MainMessage_RefTimeStamp::clear_has_ticks() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void MainMessage_RefTimeStamp::clear_ticks() {
  ticks_ = GOOGLE_ULONGLONG(0);
  clear_has_ticks();
}
inline ::google::protobuf::uint64 MainMessage_RefTimeStamp::ticks() const {
  // @@protoc_insertion_point(field_get:MainMessage.RefTimeStamp.ticks)
  return ticks_;
}
inline void MainMessage_RefTimeStamp::set_ticks(::google::protobuf::uint64 value) {
  set_has_ticks();
  ticks_ = value;
  // @@protoc_insertion_point(field_set:MainMessage.RefTimeStamp.ticks)
}

// -------------------------------------------------------------------

// MainMessage_Driver

// optional string identification = 1;
inline bool MainMessage_Driver::has_identification() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MainMessage_Driver::set_has_identification() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MainMessage_Driver::clear_has_identification() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MainMessage_Driver::clear_identification() {
  if (identification_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    identification_->clear();
  }
  clear_has_identification();
}
inline const ::std::string& MainMessage_Driver::identification() const {
  // @@protoc_insertion_point(field_get:MainMessage.Driver.identification)
  return *identification_;
}
inline void MainMessage_Driver::set_identification(const ::std::string& value) {
  set_has_identification();
  if (identification_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    identification_ = new ::std::string;
  }
  identification_->assign(value);
  // @@protoc_insertion_point(field_set:MainMessage.Driver.identification)
}
inline void MainMessage_Driver::set_identification(const char* value) {
  set_has_identification();
  if (identification_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    identification_ = new ::std::string;
  }
  identification_->assign(value);
  // @@protoc_insertion_point(field_set_char:MainMessage.Driver.identification)
}
inline void MainMessage_Driver::set_identification(const char* value, size_t size) {
  set_has_identification();
  if (identification_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    identification_ = new ::std::string;
  }
  identification_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:MainMessage.Driver.identification)
}
inline ::std::string* MainMessage_Driver::mutable_identification() {
  set_has_identification();
  if (identification_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    identification_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:MainMessage.Driver.identification)
  return identification_;
}
inline ::std::string* MainMessage_Driver::release_identification() {
  clear_has_identification();
  if (identification_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = identification_;
    identification_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void MainMessage_Driver::set_allocated_identification(::std::string* identification) {
  if (identification_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete identification_;
  }
  if (identification) {
    set_has_identification();
    identification_ = identification;
  } else {
    clear_has_identification();
    identification_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:MainMessage.Driver.identification)
}

// optional .MainMessage.DriverIdType identification_type = 2;
inline bool MainMessage_Driver::has_identification_type() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void MainMessage_Driver::set_has_identification_type() {
  _has_bits_[0] |= 0x00000002u;
}
inline void MainMessage_Driver::clear_has_identification_type() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void MainMessage_Driver::clear_identification_type() {
  identification_type_ = 0;
  clear_has_identification_type();
}
inline ::MainMessage_DriverIdType MainMessage_Driver::identification_type() const {
  // @@protoc_insertion_point(field_get:MainMessage.Driver.identification_type)
  return static_cast< ::MainMessage_DriverIdType >(identification_type_);
}
inline void MainMessage_Driver::set_identification_type(::MainMessage_DriverIdType value) {
  assert(::MainMessage_DriverIdType_IsValid(value));
  set_has_identification_type();
  identification_type_ = value;
  // @@protoc_insertion_point(field_set:MainMessage.Driver.identification_type)
}

// -------------------------------------------------------------------

// MainMessage

// optional uint32 version = 1;
inline bool MainMessage::has_version() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MainMessage::set_has_version() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MainMessage::clear_has_version() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MainMessage::clear_version() {
  version_ = 0u;
  clear_has_version();
}
inline ::google::protobuf::uint32 MainMessage::version() const {
  // @@protoc_insertion_point(field_get:MainMessage.version)
  return version_;
}
inline void MainMessage::set_version(::google::protobuf::uint32 value) {
  set_has_version();
  version_ = value;
  // @@protoc_insertion_point(field_set:MainMessage.version)
}

// optional uint64 message_id = 2;
inline bool MainMessage::has_message_id() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void MainMessage::set_has_message_id() {
  _has_bits_[0] |= 0x00000002u;
}
inline void MainMessage::clear_has_message_id() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void MainMessage::clear_message_id() {
  message_id_ = GOOGLE_ULONGLONG(0);
  clear_has_message_id();
}
inline ::google::protobuf::uint64 MainMessage::message_id() const {
  // @@protoc_insertion_point(field_get:MainMessage.message_id)
  return message_id_;
}
inline void MainMessage::set_message_id(::google::protobuf::uint64 value) {
  set_has_message_id();
  message_id_ = value;
  // @@protoc_insertion_point(field_set:MainMessage.message_id)
}

// optional string originator = 3;
inline bool MainMessage::has_originator() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void MainMessage::set_has_originator() {
  _has_bits_[0] |= 0x00000004u;
}
inline void MainMessage::clear_has_originator() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void MainMessage::clear_originator() {
  if (originator_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    originator_->clear();
  }
  clear_has_originator();
}
inline const ::std::string& MainMessage::originator() const {
  // @@protoc_insertion_point(field_get:MainMessage.originator)
  return *originator_;
}
inline void MainMessage::set_originator(const ::std::string& value) {
  set_has_originator();
  if (originator_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    originator_ = new ::std::string;
  }
  originator_->assign(value);
  // @@protoc_insertion_point(field_set:MainMessage.originator)
}
inline void MainMessage::set_originator(const char* value) {
  set_has_originator();
  if (originator_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    originator_ = new ::std::string;
  }
  originator_->assign(value);
  // @@protoc_insertion_point(field_set_char:MainMessage.originator)
}
inline void MainMessage::set_originator(const char* value, size_t size) {
  set_has_originator();
  if (originator_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    originator_ = new ::std::string;
  }
  originator_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:MainMessage.originator)
}
inline ::std::string* MainMessage::mutable_originator() {
  set_has_originator();
  if (originator_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    originator_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:MainMessage.originator)
  return originator_;
}
inline ::std::string* MainMessage::release_originator() {
  clear_has_originator();
  if (originator_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = originator_;
    originator_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void MainMessage::set_allocated_originator(::std::string* originator) {
  if (originator_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete originator_;
  }
  if (originator) {
    set_has_originator();
    originator_ = originator;
  } else {
    clear_has_originator();
    originator_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:MainMessage.originator)
}

// optional .MainMessage.RefTimeStamp time = 8;
inline bool MainMessage::has_time() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void MainMessage::set_has_time() {
  _has_bits_[0] |= 0x00000008u;
}
inline void MainMessage::clear_has_time() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void MainMessage::clear_time() {
  if (time_ != NULL) time_->::MainMessage_RefTimeStamp::Clear();
  clear_has_time();
}
inline const ::MainMessage_RefTimeStamp& MainMessage::time() const {
  // @@protoc_insertion_point(field_get:MainMessage.time)
  return time_ != NULL ? *time_ : *default_instance_->time_;
}
inline ::MainMessage_RefTimeStamp* MainMessage::mutable_time() {
  set_has_time();
  if (time_ == NULL) time_ = new ::MainMessage_RefTimeStamp;
  // @@protoc_insertion_point(field_mutable:MainMessage.time)
  return time_;
}
inline ::MainMessage_RefTimeStamp* MainMessage::release_time() {
  clear_has_time();
  ::MainMessage_RefTimeStamp* temp = time_;
  time_ = NULL;
  return temp;
}
inline void MainMessage::set_allocated_time(::MainMessage_RefTimeStamp* time) {
  delete time_;
  time_ = time;
  if (time) {
    set_has_time();
  } else {
    clear_has_time();
  }
  // @@protoc_insertion_point(field_set_allocated:MainMessage.time)
}

// optional uint32 content_id = 5;
inline bool MainMessage::has_content_id() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void MainMessage::set_has_content_id() {
  _has_bits_[0] |= 0x00000010u;
}
inline void MainMessage::clear_has_content_id() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void MainMessage::clear_content_id() {
  content_id_ = 0u;
  clear_has_content_id();
}
inline ::google::protobuf::uint32 MainMessage::content_id() const {
  // @@protoc_insertion_point(field_get:MainMessage.content_id)
  return content_id_;
}
inline void MainMessage::set_content_id(::google::protobuf::uint32 value) {
  set_has_content_id();
  content_id_ = value;
  // @@protoc_insertion_point(field_set:MainMessage.content_id)
}

// optional bytes payload = 6;
inline bool MainMessage::has_payload() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void MainMessage::set_has_payload() {
  _has_bits_[0] |= 0x00000020u;
}
inline void MainMessage::clear_has_payload() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void MainMessage::clear_payload() {
  if (payload_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    payload_->clear();
  }
  clear_has_payload();
}
inline const ::std::string& MainMessage::payload() const {
  // @@protoc_insertion_point(field_get:MainMessage.payload)
  return *payload_;
}
inline void MainMessage::set_payload(const ::std::string& value) {
  set_has_payload();
  if (payload_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    payload_ = new ::std::string;
  }
  payload_->assign(value);
  // @@protoc_insertion_point(field_set:MainMessage.payload)
}
inline void MainMessage::set_payload(const char* value) {
  set_has_payload();
  if (payload_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    payload_ = new ::std::string;
  }
  payload_->assign(value);
  // @@protoc_insertion_point(field_set_char:MainMessage.payload)
}
inline void MainMessage::set_payload(const void* value, size_t size) {
  set_has_payload();
  if (payload_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    payload_ = new ::std::string;
  }
  payload_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:MainMessage.payload)
}
inline ::std::string* MainMessage::mutable_payload() {
  set_has_payload();
  if (payload_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    payload_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:MainMessage.payload)
  return payload_;
}
inline ::std::string* MainMessage::release_payload() {
  clear_has_payload();
  if (payload_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = payload_;
    payload_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void MainMessage::set_allocated_payload(::std::string* payload) {
  if (payload_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete payload_;
  }
  if (payload) {
    set_has_payload();
    payload_ = payload;
  } else {
    clear_has_payload();
    payload_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:MainMessage.payload)
}

// optional .MainMessage.Driver driver = 7;
inline bool MainMessage::has_driver() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void MainMessage::set_has_driver() {
  _has_bits_[0] |= 0x00000040u;
}
inline void MainMessage::clear_has_driver() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void MainMessage::clear_driver() {
  if (driver_ != NULL) driver_->::MainMessage_Driver::Clear();
  clear_has_driver();
}
inline const ::MainMessage_Driver& MainMessage::driver() const {
  // @@protoc_insertion_point(field_get:MainMessage.driver)
  return driver_ != NULL ? *driver_ : *default_instance_->driver_;
}
inline ::MainMessage_Driver* MainMessage::mutable_driver() {
  set_has_driver();
  if (driver_ == NULL) driver_ = new ::MainMessage_Driver;
  // @@protoc_insertion_point(field_mutable:MainMessage.driver)
  return driver_;
}
inline ::MainMessage_Driver* MainMessage::release_driver() {
  clear_has_driver();
  ::MainMessage_Driver* temp = driver_;
  driver_ = NULL;
  return temp;
}
inline void MainMessage::set_allocated_driver(::MainMessage_Driver* driver) {
  delete driver_;
  driver_ = driver;
  if (driver) {
    set_has_driver();
  } else {
    clear_has_driver();
  }
  // @@protoc_insertion_point(field_set_allocated:MainMessage.driver)
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::MainMessage_TimeSrc> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::MainMessage_TimeSrc>() {
  return ::MainMessage_TimeSrc_descriptor();
}
template <> struct is_proto_enum< ::MainMessage_DriverIdType> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::MainMessage_DriverIdType>() {
  return ::MainMessage_DriverIdType_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_com_2fscv_2fprotobuf_2fmainmessage_2fMessages_2eproto__INCLUDED
