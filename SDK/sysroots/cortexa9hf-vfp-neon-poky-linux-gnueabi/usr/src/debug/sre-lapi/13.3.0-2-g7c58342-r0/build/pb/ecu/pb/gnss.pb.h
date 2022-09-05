// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: gnss.proto

#ifndef PROTOBUF_gnss_2eproto__INCLUDED
#define PROTOBUF_gnss_2eproto__INCLUDED

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

namespace ecu {
namespace lapi {
namespace pb {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_gnss_2eproto();
void protobuf_AssignDesc_gnss_2eproto();
void protobuf_ShutdownFile_gnss_2eproto();

class GnssData;
class GnssSource;

enum GnssSource_Source {
  GnssSource_Source_UNKNOWN = 0,
  GnssSource_Source_VCM = 1,
  GnssSource_Source_TBM3 = 2
};
bool GnssSource_Source_IsValid(int value);
const GnssSource_Source GnssSource_Source_Source_MIN = GnssSource_Source_UNKNOWN;
const GnssSource_Source GnssSource_Source_Source_MAX = GnssSource_Source_TBM3;
const int GnssSource_Source_Source_ARRAYSIZE = GnssSource_Source_Source_MAX + 1;

const ::google::protobuf::EnumDescriptor* GnssSource_Source_descriptor();
inline const ::std::string& GnssSource_Source_Name(GnssSource_Source value) {
  return ::google::protobuf::internal::NameOfEnum(
    GnssSource_Source_descriptor(), value);
}
inline bool GnssSource_Source_Parse(
    const ::std::string& name, GnssSource_Source* value) {
  return ::google::protobuf::internal::ParseNamedEnum<GnssSource_Source>(
    GnssSource_Source_descriptor(), name, value);
}
// ===================================================================

class GnssData : public ::google::protobuf::Message {
 public:
  GnssData();
  virtual ~GnssData();

  GnssData(const GnssData& from);

  inline GnssData& operator=(const GnssData& from) {
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
  static const GnssData& default_instance();

  void Swap(GnssData* other);

  // implements Message ----------------------------------------------

  GnssData* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const GnssData& from);
  void MergeFrom(const GnssData& from);
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

  // optional float longitude = 1;
  inline bool has_longitude() const;
  inline void clear_longitude();
  static const int kLongitudeFieldNumber = 1;
  inline float longitude() const;
  inline void set_longitude(float value);

  // optional float latitude = 2;
  inline bool has_latitude() const;
  inline void clear_latitude();
  static const int kLatitudeFieldNumber = 2;
  inline float latitude() const;
  inline void set_latitude(float value);

  // optional float speed = 3;
  inline bool has_speed() const;
  inline void clear_speed();
  static const int kSpeedFieldNumber = 3;
  inline float speed() const;
  inline void set_speed(float value);

  // optional float heading = 4;
  inline bool has_heading() const;
  inline void clear_heading();
  static const int kHeadingFieldNumber = 4;
  inline float heading() const;
  inline void set_heading(float value);

  // optional fixed32 num_satellites = 5;
  inline bool has_num_satellites() const;
  inline void clear_num_satellites();
  static const int kNumSatellitesFieldNumber = 5;
  inline ::google::protobuf::uint32 num_satellites() const;
  inline void set_num_satellites(::google::protobuf::uint32 value);

  // optional uint32 quality = 6;
  inline bool has_quality() const;
  inline void clear_quality();
  static const int kQualityFieldNumber = 6;
  inline ::google::protobuf::uint32 quality() const;
  inline void set_quality(::google::protobuf::uint32 value);

  // repeated string nmeas = 7;
  inline int nmeas_size() const;
  inline void clear_nmeas();
  static const int kNmeasFieldNumber = 7;
  inline const ::std::string& nmeas(int index) const;
  inline ::std::string* mutable_nmeas(int index);
  inline void set_nmeas(int index, const ::std::string& value);
  inline void set_nmeas(int index, const char* value);
  inline void set_nmeas(int index, const char* value, size_t size);
  inline ::std::string* add_nmeas();
  inline void add_nmeas(const ::std::string& value);
  inline void add_nmeas(const char* value);
  inline void add_nmeas(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& nmeas() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_nmeas();

  // optional int32 status = 8;
  inline bool has_status() const;
  inline void clear_status();
  static const int kStatusFieldNumber = 8;
  inline ::google::protobuf::int32 status() const;
  inline void set_status(::google::protobuf::int32 value);

  // optional int32 fix = 9;
  inline bool has_fix() const;
  inline void clear_fix();
  static const int kFixFieldNumber = 9;
  inline ::google::protobuf::int32 fix() const;
  inline void set_fix(::google::protobuf::int32 value);

  // optional float altitude = 10;
  inline bool has_altitude() const;
  inline void clear_altitude();
  static const int kAltitudeFieldNumber = 10;
  inline float altitude() const;
  inline void set_altitude(float value);

  // optional uint64 utc_time = 11;
  inline bool has_utc_time() const;
  inline void clear_utc_time();
  static const int kUtcTimeFieldNumber = 11;
  inline ::google::protobuf::uint64 utc_time() const;
  inline void set_utc_time(::google::protobuf::uint64 value);

  // optional uint64 timestamp = 14;
  inline bool has_timestamp() const;
  inline void clear_timestamp();
  static const int kTimestampFieldNumber = 14;
  inline ::google::protobuf::uint64 timestamp() const;
  inline void set_timestamp(::google::protobuf::uint64 value);

  // optional float accuracy = 15;
  inline bool has_accuracy() const;
  inline void clear_accuracy();
  static const int kAccuracyFieldNumber = 15;
  inline float accuracy() const;
  inline void set_accuracy(float value);

  // optional float hdop = 16;
  inline bool has_hdop() const;
  inline void clear_hdop();
  static const int kHdopFieldNumber = 16;
  inline float hdop() const;
  inline void set_hdop(float value);

  // optional float vdop = 17;
  inline bool has_vdop() const;
  inline void clear_vdop();
  static const int kVdopFieldNumber = 17;
  inline float vdop() const;
  inline void set_vdop(float value);

  // @@protoc_insertion_point(class_scope:ecu.lapi.pb.GnssData)
 private:
  inline void set_has_longitude();
  inline void clear_has_longitude();
  inline void set_has_latitude();
  inline void clear_has_latitude();
  inline void set_has_speed();
  inline void clear_has_speed();
  inline void set_has_heading();
  inline void clear_has_heading();
  inline void set_has_num_satellites();
  inline void clear_has_num_satellites();
  inline void set_has_quality();
  inline void clear_has_quality();
  inline void set_has_status();
  inline void clear_has_status();
  inline void set_has_fix();
  inline void clear_has_fix();
  inline void set_has_altitude();
  inline void clear_has_altitude();
  inline void set_has_utc_time();
  inline void clear_has_utc_time();
  inline void set_has_timestamp();
  inline void clear_has_timestamp();
  inline void set_has_accuracy();
  inline void clear_has_accuracy();
  inline void set_has_hdop();
  inline void clear_has_hdop();
  inline void set_has_vdop();
  inline void clear_has_vdop();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  float longitude_;
  float latitude_;
  float speed_;
  float heading_;
  ::google::protobuf::uint32 num_satellites_;
  ::google::protobuf::uint32 quality_;
  ::google::protobuf::RepeatedPtrField< ::std::string> nmeas_;
  ::google::protobuf::int32 status_;
  ::google::protobuf::int32 fix_;
  ::google::protobuf::uint64 utc_time_;
  float altitude_;
  float accuracy_;
  ::google::protobuf::uint64 timestamp_;
  float hdop_;
  float vdop_;
  friend void  protobuf_AddDesc_gnss_2eproto();
  friend void protobuf_AssignDesc_gnss_2eproto();
  friend void protobuf_ShutdownFile_gnss_2eproto();

  void InitAsDefaultInstance();
  static GnssData* default_instance_;
};
// -------------------------------------------------------------------

class GnssSource : public ::google::protobuf::Message {
 public:
  GnssSource();
  virtual ~GnssSource();

  GnssSource(const GnssSource& from);

  inline GnssSource& operator=(const GnssSource& from) {
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
  static const GnssSource& default_instance();

  void Swap(GnssSource* other);

  // implements Message ----------------------------------------------

  GnssSource* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const GnssSource& from);
  void MergeFrom(const GnssSource& from);
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

  typedef GnssSource_Source Source;
  static const Source UNKNOWN = GnssSource_Source_UNKNOWN;
  static const Source VCM = GnssSource_Source_VCM;
  static const Source TBM3 = GnssSource_Source_TBM3;
  static inline bool Source_IsValid(int value) {
    return GnssSource_Source_IsValid(value);
  }
  static const Source Source_MIN =
    GnssSource_Source_Source_MIN;
  static const Source Source_MAX =
    GnssSource_Source_Source_MAX;
  static const int Source_ARRAYSIZE =
    GnssSource_Source_Source_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  Source_descriptor() {
    return GnssSource_Source_descriptor();
  }
  static inline const ::std::string& Source_Name(Source value) {
    return GnssSource_Source_Name(value);
  }
  static inline bool Source_Parse(const ::std::string& name,
      Source* value) {
    return GnssSource_Source_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // optional .ecu.lapi.pb.GnssSource.Source source = 1;
  inline bool has_source() const;
  inline void clear_source();
  static const int kSourceFieldNumber = 1;
  inline ::ecu::lapi::pb::GnssSource_Source source() const;
  inline void set_source(::ecu::lapi::pb::GnssSource_Source value);

  // @@protoc_insertion_point(class_scope:ecu.lapi.pb.GnssSource)
 private:
  inline void set_has_source();
  inline void clear_has_source();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  int source_;
  friend void  protobuf_AddDesc_gnss_2eproto();
  friend void protobuf_AssignDesc_gnss_2eproto();
  friend void protobuf_ShutdownFile_gnss_2eproto();

  void InitAsDefaultInstance();
  static GnssSource* default_instance_;
};
// ===================================================================


// ===================================================================

// GnssData

// optional float longitude = 1;
inline bool GnssData::has_longitude() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void GnssData::set_has_longitude() {
  _has_bits_[0] |= 0x00000001u;
}
inline void GnssData::clear_has_longitude() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void GnssData::clear_longitude() {
  longitude_ = 0;
  clear_has_longitude();
}
inline float GnssData::longitude() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.GnssData.longitude)
  return longitude_;
}
inline void GnssData::set_longitude(float value) {
  set_has_longitude();
  longitude_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.GnssData.longitude)
}

// optional float latitude = 2;
inline bool GnssData::has_latitude() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void GnssData::set_has_latitude() {
  _has_bits_[0] |= 0x00000002u;
}
inline void GnssData::clear_has_latitude() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void GnssData::clear_latitude() {
  latitude_ = 0;
  clear_has_latitude();
}
inline float GnssData::latitude() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.GnssData.latitude)
  return latitude_;
}
inline void GnssData::set_latitude(float value) {
  set_has_latitude();
  latitude_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.GnssData.latitude)
}

// optional float speed = 3;
inline bool GnssData::has_speed() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void GnssData::set_has_speed() {
  _has_bits_[0] |= 0x00000004u;
}
inline void GnssData::clear_has_speed() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void GnssData::clear_speed() {
  speed_ = 0;
  clear_has_speed();
}
inline float GnssData::speed() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.GnssData.speed)
  return speed_;
}
inline void GnssData::set_speed(float value) {
  set_has_speed();
  speed_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.GnssData.speed)
}

// optional float heading = 4;
inline bool GnssData::has_heading() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void GnssData::set_has_heading() {
  _has_bits_[0] |= 0x00000008u;
}
inline void GnssData::clear_has_heading() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void GnssData::clear_heading() {
  heading_ = 0;
  clear_has_heading();
}
inline float GnssData::heading() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.GnssData.heading)
  return heading_;
}
inline void GnssData::set_heading(float value) {
  set_has_heading();
  heading_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.GnssData.heading)
}

// optional fixed32 num_satellites = 5;
inline bool GnssData::has_num_satellites() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void GnssData::set_has_num_satellites() {
  _has_bits_[0] |= 0x00000010u;
}
inline void GnssData::clear_has_num_satellites() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void GnssData::clear_num_satellites() {
  num_satellites_ = 0u;
  clear_has_num_satellites();
}
inline ::google::protobuf::uint32 GnssData::num_satellites() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.GnssData.num_satellites)
  return num_satellites_;
}
inline void GnssData::set_num_satellites(::google::protobuf::uint32 value) {
  set_has_num_satellites();
  num_satellites_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.GnssData.num_satellites)
}

// optional uint32 quality = 6;
inline bool GnssData::has_quality() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void GnssData::set_has_quality() {
  _has_bits_[0] |= 0x00000020u;
}
inline void GnssData::clear_has_quality() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void GnssData::clear_quality() {
  quality_ = 0u;
  clear_has_quality();
}
inline ::google::protobuf::uint32 GnssData::quality() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.GnssData.quality)
  return quality_;
}
inline void GnssData::set_quality(::google::protobuf::uint32 value) {
  set_has_quality();
  quality_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.GnssData.quality)
}

// repeated string nmeas = 7;
inline int GnssData::nmeas_size() const {
  return nmeas_.size();
}
inline void GnssData::clear_nmeas() {
  nmeas_.Clear();
}
inline const ::std::string& GnssData::nmeas(int index) const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.GnssData.nmeas)
  return nmeas_.Get(index);
}
inline ::std::string* GnssData::mutable_nmeas(int index) {
  // @@protoc_insertion_point(field_mutable:ecu.lapi.pb.GnssData.nmeas)
  return nmeas_.Mutable(index);
}
inline void GnssData::set_nmeas(int index, const ::std::string& value) {
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.GnssData.nmeas)
  nmeas_.Mutable(index)->assign(value);
}
inline void GnssData::set_nmeas(int index, const char* value) {
  nmeas_.Mutable(index)->assign(value);
  // @@protoc_insertion_point(field_set_char:ecu.lapi.pb.GnssData.nmeas)
}
inline void GnssData::set_nmeas(int index, const char* value, size_t size) {
  nmeas_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:ecu.lapi.pb.GnssData.nmeas)
}
inline ::std::string* GnssData::add_nmeas() {
  return nmeas_.Add();
}
inline void GnssData::add_nmeas(const ::std::string& value) {
  nmeas_.Add()->assign(value);
  // @@protoc_insertion_point(field_add:ecu.lapi.pb.GnssData.nmeas)
}
inline void GnssData::add_nmeas(const char* value) {
  nmeas_.Add()->assign(value);
  // @@protoc_insertion_point(field_add_char:ecu.lapi.pb.GnssData.nmeas)
}
inline void GnssData::add_nmeas(const char* value, size_t size) {
  nmeas_.Add()->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_add_pointer:ecu.lapi.pb.GnssData.nmeas)
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
GnssData::nmeas() const {
  // @@protoc_insertion_point(field_list:ecu.lapi.pb.GnssData.nmeas)
  return nmeas_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
GnssData::mutable_nmeas() {
  // @@protoc_insertion_point(field_mutable_list:ecu.lapi.pb.GnssData.nmeas)
  return &nmeas_;
}

// optional int32 status = 8;
inline bool GnssData::has_status() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void GnssData::set_has_status() {
  _has_bits_[0] |= 0x00000080u;
}
inline void GnssData::clear_has_status() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void GnssData::clear_status() {
  status_ = 0;
  clear_has_status();
}
inline ::google::protobuf::int32 GnssData::status() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.GnssData.status)
  return status_;
}
inline void GnssData::set_status(::google::protobuf::int32 value) {
  set_has_status();
  status_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.GnssData.status)
}

// optional int32 fix = 9;
inline bool GnssData::has_fix() const {
  return (_has_bits_[0] & 0x00000100u) != 0;
}
inline void GnssData::set_has_fix() {
  _has_bits_[0] |= 0x00000100u;
}
inline void GnssData::clear_has_fix() {
  _has_bits_[0] &= ~0x00000100u;
}
inline void GnssData::clear_fix() {
  fix_ = 0;
  clear_has_fix();
}
inline ::google::protobuf::int32 GnssData::fix() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.GnssData.fix)
  return fix_;
}
inline void GnssData::set_fix(::google::protobuf::int32 value) {
  set_has_fix();
  fix_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.GnssData.fix)
}

// optional float altitude = 10;
inline bool GnssData::has_altitude() const {
  return (_has_bits_[0] & 0x00000200u) != 0;
}
inline void GnssData::set_has_altitude() {
  _has_bits_[0] |= 0x00000200u;
}
inline void GnssData::clear_has_altitude() {
  _has_bits_[0] &= ~0x00000200u;
}
inline void GnssData::clear_altitude() {
  altitude_ = 0;
  clear_has_altitude();
}
inline float GnssData::altitude() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.GnssData.altitude)
  return altitude_;
}
inline void GnssData::set_altitude(float value) {
  set_has_altitude();
  altitude_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.GnssData.altitude)
}

// optional uint64 utc_time = 11;
inline bool GnssData::has_utc_time() const {
  return (_has_bits_[0] & 0x00000400u) != 0;
}
inline void GnssData::set_has_utc_time() {
  _has_bits_[0] |= 0x00000400u;
}
inline void GnssData::clear_has_utc_time() {
  _has_bits_[0] &= ~0x00000400u;
}
inline void GnssData::clear_utc_time() {
  utc_time_ = GOOGLE_ULONGLONG(0);
  clear_has_utc_time();
}
inline ::google::protobuf::uint64 GnssData::utc_time() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.GnssData.utc_time)
  return utc_time_;
}
inline void GnssData::set_utc_time(::google::protobuf::uint64 value) {
  set_has_utc_time();
  utc_time_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.GnssData.utc_time)
}

// optional uint64 timestamp = 14;
inline bool GnssData::has_timestamp() const {
  return (_has_bits_[0] & 0x00000800u) != 0;
}
inline void GnssData::set_has_timestamp() {
  _has_bits_[0] |= 0x00000800u;
}
inline void GnssData::clear_has_timestamp() {
  _has_bits_[0] &= ~0x00000800u;
}
inline void GnssData::clear_timestamp() {
  timestamp_ = GOOGLE_ULONGLONG(0);
  clear_has_timestamp();
}
inline ::google::protobuf::uint64 GnssData::timestamp() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.GnssData.timestamp)
  return timestamp_;
}
inline void GnssData::set_timestamp(::google::protobuf::uint64 value) {
  set_has_timestamp();
  timestamp_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.GnssData.timestamp)
}

// optional float accuracy = 15;
inline bool GnssData::has_accuracy() const {
  return (_has_bits_[0] & 0x00001000u) != 0;
}
inline void GnssData::set_has_accuracy() {
  _has_bits_[0] |= 0x00001000u;
}
inline void GnssData::clear_has_accuracy() {
  _has_bits_[0] &= ~0x00001000u;
}
inline void GnssData::clear_accuracy() {
  accuracy_ = 0;
  clear_has_accuracy();
}
inline float GnssData::accuracy() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.GnssData.accuracy)
  return accuracy_;
}
inline void GnssData::set_accuracy(float value) {
  set_has_accuracy();
  accuracy_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.GnssData.accuracy)
}

// optional float hdop = 16;
inline bool GnssData::has_hdop() const {
  return (_has_bits_[0] & 0x00002000u) != 0;
}
inline void GnssData::set_has_hdop() {
  _has_bits_[0] |= 0x00002000u;
}
inline void GnssData::clear_has_hdop() {
  _has_bits_[0] &= ~0x00002000u;
}
inline void GnssData::clear_hdop() {
  hdop_ = 0;
  clear_has_hdop();
}
inline float GnssData::hdop() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.GnssData.hdop)
  return hdop_;
}
inline void GnssData::set_hdop(float value) {
  set_has_hdop();
  hdop_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.GnssData.hdop)
}

// optional float vdop = 17;
inline bool GnssData::has_vdop() const {
  return (_has_bits_[0] & 0x00004000u) != 0;
}
inline void GnssData::set_has_vdop() {
  _has_bits_[0] |= 0x00004000u;
}
inline void GnssData::clear_has_vdop() {
  _has_bits_[0] &= ~0x00004000u;
}
inline void GnssData::clear_vdop() {
  vdop_ = 0;
  clear_has_vdop();
}
inline float GnssData::vdop() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.GnssData.vdop)
  return vdop_;
}
inline void GnssData::set_vdop(float value) {
  set_has_vdop();
  vdop_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.GnssData.vdop)
}

// -------------------------------------------------------------------

// GnssSource

// optional .ecu.lapi.pb.GnssSource.Source source = 1;
inline bool GnssSource::has_source() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void GnssSource::set_has_source() {
  _has_bits_[0] |= 0x00000001u;
}
inline void GnssSource::clear_has_source() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void GnssSource::clear_source() {
  source_ = 0;
  clear_has_source();
}
inline ::ecu::lapi::pb::GnssSource_Source GnssSource::source() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.GnssSource.source)
  return static_cast< ::ecu::lapi::pb::GnssSource_Source >(source_);
}
inline void GnssSource::set_source(::ecu::lapi::pb::GnssSource_Source value) {
  assert(::ecu::lapi::pb::GnssSource_Source_IsValid(value));
  set_has_source();
  source_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.GnssSource.source)
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace pb
}  // namespace lapi
}  // namespace ecu

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::ecu::lapi::pb::GnssSource_Source> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::ecu::lapi::pb::GnssSource_Source>() {
  return ::ecu::lapi::pb::GnssSource_Source_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_gnss_2eproto__INCLUDED