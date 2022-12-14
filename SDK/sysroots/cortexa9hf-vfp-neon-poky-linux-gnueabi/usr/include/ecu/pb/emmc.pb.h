// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: emmc.proto

#ifndef PROTOBUF_emmc_2eproto__INCLUDED
#define PROTOBUF_emmc_2eproto__INCLUDED

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
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace ecu {
namespace lapi {
namespace pb {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_emmc_2eproto();
void protobuf_AssignDesc_emmc_2eproto();
void protobuf_ShutdownFile_emmc_2eproto();

class EmmcStatus;

// ===================================================================

class EmmcStatus : public ::google::protobuf::Message {
 public:
  EmmcStatus();
  virtual ~EmmcStatus();

  EmmcStatus(const EmmcStatus& from);

  inline EmmcStatus& operator=(const EmmcStatus& from) {
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
  static const EmmcStatus& default_instance();

  void Swap(EmmcStatus* other);

  // implements Message ----------------------------------------------

  EmmcStatus* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const EmmcStatus& from);
  void MergeFrom(const EmmcStatus& from);
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

  // optional uint32 wear_level_user = 1;
  inline bool has_wear_level_user() const;
  inline void clear_wear_level_user();
  static const int kWearLevelUserFieldNumber = 1;
  inline ::google::protobuf::uint32 wear_level_user() const;
  inline void set_wear_level_user(::google::protobuf::uint32 value);

  // optional uint32 wear_level_system = 2;
  inline bool has_wear_level_system() const;
  inline void clear_wear_level_system();
  static const int kWearLevelSystemFieldNumber = 2;
  inline ::google::protobuf::uint32 wear_level_system() const;
  inline void set_wear_level_system(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:ecu.lapi.pb.EmmcStatus)
 private:
  inline void set_has_wear_level_user();
  inline void clear_has_wear_level_user();
  inline void set_has_wear_level_system();
  inline void clear_has_wear_level_system();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint32 wear_level_user_;
  ::google::protobuf::uint32 wear_level_system_;
  friend void  protobuf_AddDesc_emmc_2eproto();
  friend void protobuf_AssignDesc_emmc_2eproto();
  friend void protobuf_ShutdownFile_emmc_2eproto();

  void InitAsDefaultInstance();
  static EmmcStatus* default_instance_;
};
// ===================================================================


// ===================================================================

// EmmcStatus

// optional uint32 wear_level_user = 1;
inline bool EmmcStatus::has_wear_level_user() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void EmmcStatus::set_has_wear_level_user() {
  _has_bits_[0] |= 0x00000001u;
}
inline void EmmcStatus::clear_has_wear_level_user() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void EmmcStatus::clear_wear_level_user() {
  wear_level_user_ = 0u;
  clear_has_wear_level_user();
}
inline ::google::protobuf::uint32 EmmcStatus::wear_level_user() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.EmmcStatus.wear_level_user)
  return wear_level_user_;
}
inline void EmmcStatus::set_wear_level_user(::google::protobuf::uint32 value) {
  set_has_wear_level_user();
  wear_level_user_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.EmmcStatus.wear_level_user)
}

// optional uint32 wear_level_system = 2;
inline bool EmmcStatus::has_wear_level_system() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void EmmcStatus::set_has_wear_level_system() {
  _has_bits_[0] |= 0x00000002u;
}
inline void EmmcStatus::clear_has_wear_level_system() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void EmmcStatus::clear_wear_level_system() {
  wear_level_system_ = 0u;
  clear_has_wear_level_system();
}
inline ::google::protobuf::uint32 EmmcStatus::wear_level_system() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.EmmcStatus.wear_level_system)
  return wear_level_system_;
}
inline void EmmcStatus::set_wear_level_system(::google::protobuf::uint32 value) {
  set_has_wear_level_system();
  wear_level_system_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.EmmcStatus.wear_level_system)
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace pb
}  // namespace lapi
}  // namespace ecu

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_emmc_2eproto__INCLUDED
