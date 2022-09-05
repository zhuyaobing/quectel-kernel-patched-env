// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: header.proto

#ifndef PROTOBUF_header_2eproto__INCLUDED
#define PROTOBUF_header_2eproto__INCLUDED

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
void  protobuf_AddDesc_header_2eproto();
void protobuf_AssignDesc_header_2eproto();
void protobuf_ShutdownFile_header_2eproto();

class SREHeader;

// ===================================================================

class SREHeader : public ::google::protobuf::Message {
 public:
  SREHeader();
  virtual ~SREHeader();

  SREHeader(const SREHeader& from);

  inline SREHeader& operator=(const SREHeader& from) {
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
  static const SREHeader& default_instance();

  void Swap(SREHeader* other);

  // implements Message ----------------------------------------------

  SREHeader* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const SREHeader& from);
  void MergeFrom(const SREHeader& from);
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

  // optional fixed64 sre_header = 1;
  inline bool has_sre_header() const;
  inline void clear_sre_header();
  static const int kSreHeaderFieldNumber = 1;
  inline ::google::protobuf::uint64 sre_header() const;
  inline void set_sre_header(::google::protobuf::uint64 value);

  // optional fixed64 timestamp = 2;
  inline bool has_timestamp() const;
  inline void clear_timestamp();
  static const int kTimestampFieldNumber = 2;
  inline ::google::protobuf::uint64 timestamp() const;
  inline void set_timestamp(::google::protobuf::uint64 value);

  // @@protoc_insertion_point(class_scope:ecu.lapi.pb.SREHeader)
 private:
  inline void set_has_sre_header();
  inline void clear_has_sre_header();
  inline void set_has_timestamp();
  inline void clear_has_timestamp();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint64 sre_header_;
  ::google::protobuf::uint64 timestamp_;
  friend void  protobuf_AddDesc_header_2eproto();
  friend void protobuf_AssignDesc_header_2eproto();
  friend void protobuf_ShutdownFile_header_2eproto();

  void InitAsDefaultInstance();
  static SREHeader* default_instance_;
};
// ===================================================================


// ===================================================================

// SREHeader

// optional fixed64 sre_header = 1;
inline bool SREHeader::has_sre_header() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void SREHeader::set_has_sre_header() {
  _has_bits_[0] |= 0x00000001u;
}
inline void SREHeader::clear_has_sre_header() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void SREHeader::clear_sre_header() {
  sre_header_ = GOOGLE_ULONGLONG(0);
  clear_has_sre_header();
}
inline ::google::protobuf::uint64 SREHeader::sre_header() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.SREHeader.sre_header)
  return sre_header_;
}
inline void SREHeader::set_sre_header(::google::protobuf::uint64 value) {
  set_has_sre_header();
  sre_header_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.SREHeader.sre_header)
}

// optional fixed64 timestamp = 2;
inline bool SREHeader::has_timestamp() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void SREHeader::set_has_timestamp() {
  _has_bits_[0] |= 0x00000002u;
}
inline void SREHeader::clear_has_timestamp() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void SREHeader::clear_timestamp() {
  timestamp_ = GOOGLE_ULONGLONG(0);
  clear_has_timestamp();
}
inline ::google::protobuf::uint64 SREHeader::timestamp() const {
  // @@protoc_insertion_point(field_get:ecu.lapi.pb.SREHeader.timestamp)
  return timestamp_;
}
inline void SREHeader::set_timestamp(::google::protobuf::uint64 value) {
  set_has_timestamp();
  timestamp_ = value;
  // @@protoc_insertion_point(field_set:ecu.lapi.pb.SREHeader.timestamp)
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

#endif  // PROTOBUF_header_2eproto__INCLUDED