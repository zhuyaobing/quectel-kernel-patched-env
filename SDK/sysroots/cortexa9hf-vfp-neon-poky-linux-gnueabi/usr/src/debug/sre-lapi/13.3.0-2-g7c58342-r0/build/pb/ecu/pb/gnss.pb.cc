// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: gnss.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "gnss.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace ecu {
namespace lapi {
namespace pb {

namespace {

const ::google::protobuf::Descriptor* GnssData_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  GnssData_reflection_ = NULL;
const ::google::protobuf::Descriptor* GnssSource_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  GnssSource_reflection_ = NULL;
const ::google::protobuf::EnumDescriptor* GnssSource_Source_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_gnss_2eproto() {
  protobuf_AddDesc_gnss_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "gnss.proto");
  GOOGLE_CHECK(file != NULL);
  GnssData_descriptor_ = file->message_type(0);
  static const int GnssData_offsets_[15] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssData, longitude_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssData, latitude_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssData, speed_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssData, heading_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssData, num_satellites_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssData, quality_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssData, nmeas_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssData, status_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssData, fix_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssData, altitude_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssData, utc_time_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssData, timestamp_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssData, accuracy_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssData, hdop_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssData, vdop_),
  };
  GnssData_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      GnssData_descriptor_,
      GnssData::default_instance_,
      GnssData_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssData, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssData, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(GnssData));
  GnssSource_descriptor_ = file->message_type(1);
  static const int GnssSource_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssSource, source_),
  };
  GnssSource_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      GnssSource_descriptor_,
      GnssSource::default_instance_,
      GnssSource_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssSource, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GnssSource, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(GnssSource));
  GnssSource_Source_descriptor_ = GnssSource_descriptor_->enum_type(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_gnss_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    GnssData_descriptor_, &GnssData::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    GnssSource_descriptor_, &GnssSource::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_gnss_2eproto() {
  delete GnssData::default_instance_;
  delete GnssData_reflection_;
  delete GnssSource::default_instance_;
  delete GnssSource_reflection_;
}

void protobuf_AddDesc_gnss_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\ngnss.proto\022\013ecu.lapi.pb\"\211\002\n\010GnssData\022\021"
    "\n\tlongitude\030\001 \001(\002\022\020\n\010latitude\030\002 \001(\002\022\r\n\005s"
    "peed\030\003 \001(\002\022\017\n\007heading\030\004 \001(\002\022\026\n\016num_satel"
    "lites\030\005 \001(\007\022\017\n\007quality\030\006 \001(\r\022\r\n\005nmeas\030\007 "
    "\003(\t\022\016\n\006status\030\010 \001(\005\022\013\n\003fix\030\t \001(\005\022\020\n\010alti"
    "tude\030\n \001(\002\022\020\n\010utc_time\030\013 \001(\004\022\021\n\ttimestam"
    "p\030\016 \001(\004\022\020\n\010accuracy\030\017 \001(\002\022\014\n\004hdop\030\020 \001(\002\022"
    "\014\n\004vdop\030\021 \001(\002\"f\n\nGnssSource\022.\n\006source\030\001 "
    "\001(\0162\036.ecu.lapi.pb.GnssSource.Source\"(\n\006S"
    "ource\022\013\n\007UNKNOWN\020\000\022\007\n\003VCM\020\001\022\010\n\004TBM3\020\002", 397);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "gnss.proto", &protobuf_RegisterTypes);
  GnssData::default_instance_ = new GnssData();
  GnssSource::default_instance_ = new GnssSource();
  GnssData::default_instance_->InitAsDefaultInstance();
  GnssSource::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_gnss_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_gnss_2eproto {
  StaticDescriptorInitializer_gnss_2eproto() {
    protobuf_AddDesc_gnss_2eproto();
  }
} static_descriptor_initializer_gnss_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int GnssData::kLongitudeFieldNumber;
const int GnssData::kLatitudeFieldNumber;
const int GnssData::kSpeedFieldNumber;
const int GnssData::kHeadingFieldNumber;
const int GnssData::kNumSatellitesFieldNumber;
const int GnssData::kQualityFieldNumber;
const int GnssData::kNmeasFieldNumber;
const int GnssData::kStatusFieldNumber;
const int GnssData::kFixFieldNumber;
const int GnssData::kAltitudeFieldNumber;
const int GnssData::kUtcTimeFieldNumber;
const int GnssData::kTimestampFieldNumber;
const int GnssData::kAccuracyFieldNumber;
const int GnssData::kHdopFieldNumber;
const int GnssData::kVdopFieldNumber;
#endif  // !_MSC_VER

GnssData::GnssData()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:ecu.lapi.pb.GnssData)
}

void GnssData::InitAsDefaultInstance() {
}

GnssData::GnssData(const GnssData& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:ecu.lapi.pb.GnssData)
}

void GnssData::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  longitude_ = 0;
  latitude_ = 0;
  speed_ = 0;
  heading_ = 0;
  num_satellites_ = 0u;
  quality_ = 0u;
  status_ = 0;
  fix_ = 0;
  altitude_ = 0;
  utc_time_ = GOOGLE_ULONGLONG(0);
  timestamp_ = GOOGLE_ULONGLONG(0);
  accuracy_ = 0;
  hdop_ = 0;
  vdop_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

GnssData::~GnssData() {
  // @@protoc_insertion_point(destructor:ecu.lapi.pb.GnssData)
  SharedDtor();
}

void GnssData::SharedDtor() {
  if (this != default_instance_) {
  }
}

void GnssData::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* GnssData::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return GnssData_descriptor_;
}

const GnssData& GnssData::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_gnss_2eproto();
  return *default_instance_;
}

GnssData* GnssData::default_instance_ = NULL;

GnssData* GnssData::New() const {
  return new GnssData;
}

void GnssData::Clear() {
#define OFFSET_OF_FIELD_(f) (reinterpret_cast<char*>(      \
  &reinterpret_cast<GnssData*>(16)->f) - \
   reinterpret_cast<char*>(16))

#define ZR_(first, last) do {                              \
    size_t f = OFFSET_OF_FIELD_(first);                    \
    size_t n = OFFSET_OF_FIELD_(last) - f + sizeof(last);  \
    ::memset(&first, 0, n);                                \
  } while (0)

  if (_has_bits_[0 / 32] & 191) {
    ZR_(longitude_, quality_);
    status_ = 0;
  }
  if (_has_bits_[8 / 32] & 32512) {
    ZR_(fix_, vdop_);
  }

#undef OFFSET_OF_FIELD_
#undef ZR_

  nmeas_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool GnssData::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:ecu.lapi.pb.GnssData)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(16383);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional float longitude = 1;
      case 1: {
        if (tag == 13) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &longitude_)));
          set_has_longitude();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(21)) goto parse_latitude;
        break;
      }

      // optional float latitude = 2;
      case 2: {
        if (tag == 21) {
         parse_latitude:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &latitude_)));
          set_has_latitude();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(29)) goto parse_speed;
        break;
      }

      // optional float speed = 3;
      case 3: {
        if (tag == 29) {
         parse_speed:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &speed_)));
          set_has_speed();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(37)) goto parse_heading;
        break;
      }

      // optional float heading = 4;
      case 4: {
        if (tag == 37) {
         parse_heading:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &heading_)));
          set_has_heading();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(45)) goto parse_num_satellites;
        break;
      }

      // optional fixed32 num_satellites = 5;
      case 5: {
        if (tag == 45) {
         parse_num_satellites:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_FIXED32>(
                 input, &num_satellites_)));
          set_has_num_satellites();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(48)) goto parse_quality;
        break;
      }

      // optional uint32 quality = 6;
      case 6: {
        if (tag == 48) {
         parse_quality:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &quality_)));
          set_has_quality();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(58)) goto parse_nmeas;
        break;
      }

      // repeated string nmeas = 7;
      case 7: {
        if (tag == 58) {
         parse_nmeas:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->add_nmeas()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->nmeas(this->nmeas_size() - 1).data(),
            this->nmeas(this->nmeas_size() - 1).length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "nmeas");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(58)) goto parse_nmeas;
        if (input->ExpectTag(64)) goto parse_status;
        break;
      }

      // optional int32 status = 8;
      case 8: {
        if (tag == 64) {
         parse_status:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &status_)));
          set_has_status();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(72)) goto parse_fix;
        break;
      }

      // optional int32 fix = 9;
      case 9: {
        if (tag == 72) {
         parse_fix:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &fix_)));
          set_has_fix();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(85)) goto parse_altitude;
        break;
      }

      // optional float altitude = 10;
      case 10: {
        if (tag == 85) {
         parse_altitude:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &altitude_)));
          set_has_altitude();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(88)) goto parse_utc_time;
        break;
      }

      // optional uint64 utc_time = 11;
      case 11: {
        if (tag == 88) {
         parse_utc_time:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64>(
                 input, &utc_time_)));
          set_has_utc_time();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(112)) goto parse_timestamp;
        break;
      }

      // optional uint64 timestamp = 14;
      case 14: {
        if (tag == 112) {
         parse_timestamp:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64>(
                 input, &timestamp_)));
          set_has_timestamp();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(125)) goto parse_accuracy;
        break;
      }

      // optional float accuracy = 15;
      case 15: {
        if (tag == 125) {
         parse_accuracy:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &accuracy_)));
          set_has_accuracy();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(133)) goto parse_hdop;
        break;
      }

      // optional float hdop = 16;
      case 16: {
        if (tag == 133) {
         parse_hdop:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &hdop_)));
          set_has_hdop();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(141)) goto parse_vdop;
        break;
      }

      // optional float vdop = 17;
      case 17: {
        if (tag == 141) {
         parse_vdop:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &vdop_)));
          set_has_vdop();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:ecu.lapi.pb.GnssData)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:ecu.lapi.pb.GnssData)
  return false;
#undef DO_
}

void GnssData::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:ecu.lapi.pb.GnssData)
  // optional float longitude = 1;
  if (has_longitude()) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(1, this->longitude(), output);
  }

  // optional float latitude = 2;
  if (has_latitude()) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(2, this->latitude(), output);
  }

  // optional float speed = 3;
  if (has_speed()) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(3, this->speed(), output);
  }

  // optional float heading = 4;
  if (has_heading()) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(4, this->heading(), output);
  }

  // optional fixed32 num_satellites = 5;
  if (has_num_satellites()) {
    ::google::protobuf::internal::WireFormatLite::WriteFixed32(5, this->num_satellites(), output);
  }

  // optional uint32 quality = 6;
  if (has_quality()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(6, this->quality(), output);
  }

  // repeated string nmeas = 7;
  for (int i = 0; i < this->nmeas_size(); i++) {
  ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
    this->nmeas(i).data(), this->nmeas(i).length(),
    ::google::protobuf::internal::WireFormat::SERIALIZE,
    "nmeas");
    ::google::protobuf::internal::WireFormatLite::WriteString(
      7, this->nmeas(i), output);
  }

  // optional int32 status = 8;
  if (has_status()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(8, this->status(), output);
  }

  // optional int32 fix = 9;
  if (has_fix()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(9, this->fix(), output);
  }

  // optional float altitude = 10;
  if (has_altitude()) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(10, this->altitude(), output);
  }

  // optional uint64 utc_time = 11;
  if (has_utc_time()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt64(11, this->utc_time(), output);
  }

  // optional uint64 timestamp = 14;
  if (has_timestamp()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt64(14, this->timestamp(), output);
  }

  // optional float accuracy = 15;
  if (has_accuracy()) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(15, this->accuracy(), output);
  }

  // optional float hdop = 16;
  if (has_hdop()) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(16, this->hdop(), output);
  }

  // optional float vdop = 17;
  if (has_vdop()) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(17, this->vdop(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:ecu.lapi.pb.GnssData)
}

::google::protobuf::uint8* GnssData::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:ecu.lapi.pb.GnssData)
  // optional float longitude = 1;
  if (has_longitude()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(1, this->longitude(), target);
  }

  // optional float latitude = 2;
  if (has_latitude()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(2, this->latitude(), target);
  }

  // optional float speed = 3;
  if (has_speed()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(3, this->speed(), target);
  }

  // optional float heading = 4;
  if (has_heading()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(4, this->heading(), target);
  }

  // optional fixed32 num_satellites = 5;
  if (has_num_satellites()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFixed32ToArray(5, this->num_satellites(), target);
  }

  // optional uint32 quality = 6;
  if (has_quality()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(6, this->quality(), target);
  }

  // repeated string nmeas = 7;
  for (int i = 0; i < this->nmeas_size(); i++) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->nmeas(i).data(), this->nmeas(i).length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "nmeas");
    target = ::google::protobuf::internal::WireFormatLite::
      WriteStringToArray(7, this->nmeas(i), target);
  }

  // optional int32 status = 8;
  if (has_status()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(8, this->status(), target);
  }

  // optional int32 fix = 9;
  if (has_fix()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(9, this->fix(), target);
  }

  // optional float altitude = 10;
  if (has_altitude()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(10, this->altitude(), target);
  }

  // optional uint64 utc_time = 11;
  if (has_utc_time()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(11, this->utc_time(), target);
  }

  // optional uint64 timestamp = 14;
  if (has_timestamp()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(14, this->timestamp(), target);
  }

  // optional float accuracy = 15;
  if (has_accuracy()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(15, this->accuracy(), target);
  }

  // optional float hdop = 16;
  if (has_hdop()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(16, this->hdop(), target);
  }

  // optional float vdop = 17;
  if (has_vdop()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(17, this->vdop(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:ecu.lapi.pb.GnssData)
  return target;
}

int GnssData::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional float longitude = 1;
    if (has_longitude()) {
      total_size += 1 + 4;
    }

    // optional float latitude = 2;
    if (has_latitude()) {
      total_size += 1 + 4;
    }

    // optional float speed = 3;
    if (has_speed()) {
      total_size += 1 + 4;
    }

    // optional float heading = 4;
    if (has_heading()) {
      total_size += 1 + 4;
    }

    // optional fixed32 num_satellites = 5;
    if (has_num_satellites()) {
      total_size += 1 + 4;
    }

    // optional uint32 quality = 6;
    if (has_quality()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->quality());
    }

    // optional int32 status = 8;
    if (has_status()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->status());
    }

  }
  if (_has_bits_[8 / 32] & (0xffu << (8 % 32))) {
    // optional int32 fix = 9;
    if (has_fix()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->fix());
    }

    // optional float altitude = 10;
    if (has_altitude()) {
      total_size += 1 + 4;
    }

    // optional uint64 utc_time = 11;
    if (has_utc_time()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt64Size(
          this->utc_time());
    }

    // optional uint64 timestamp = 14;
    if (has_timestamp()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt64Size(
          this->timestamp());
    }

    // optional float accuracy = 15;
    if (has_accuracy()) {
      total_size += 1 + 4;
    }

    // optional float hdop = 16;
    if (has_hdop()) {
      total_size += 2 + 4;
    }

    // optional float vdop = 17;
    if (has_vdop()) {
      total_size += 2 + 4;
    }

  }
  // repeated string nmeas = 7;
  total_size += 1 * this->nmeas_size();
  for (int i = 0; i < this->nmeas_size(); i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::StringSize(
      this->nmeas(i));
  }

  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void GnssData::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const GnssData* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const GnssData*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void GnssData::MergeFrom(const GnssData& from) {
  GOOGLE_CHECK_NE(&from, this);
  nmeas_.MergeFrom(from.nmeas_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_longitude()) {
      set_longitude(from.longitude());
    }
    if (from.has_latitude()) {
      set_latitude(from.latitude());
    }
    if (from.has_speed()) {
      set_speed(from.speed());
    }
    if (from.has_heading()) {
      set_heading(from.heading());
    }
    if (from.has_num_satellites()) {
      set_num_satellites(from.num_satellites());
    }
    if (from.has_quality()) {
      set_quality(from.quality());
    }
    if (from.has_status()) {
      set_status(from.status());
    }
  }
  if (from._has_bits_[8 / 32] & (0xffu << (8 % 32))) {
    if (from.has_fix()) {
      set_fix(from.fix());
    }
    if (from.has_altitude()) {
      set_altitude(from.altitude());
    }
    if (from.has_utc_time()) {
      set_utc_time(from.utc_time());
    }
    if (from.has_timestamp()) {
      set_timestamp(from.timestamp());
    }
    if (from.has_accuracy()) {
      set_accuracy(from.accuracy());
    }
    if (from.has_hdop()) {
      set_hdop(from.hdop());
    }
    if (from.has_vdop()) {
      set_vdop(from.vdop());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void GnssData::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void GnssData::CopyFrom(const GnssData& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool GnssData::IsInitialized() const {

  return true;
}

void GnssData::Swap(GnssData* other) {
  if (other != this) {
    std::swap(longitude_, other->longitude_);
    std::swap(latitude_, other->latitude_);
    std::swap(speed_, other->speed_);
    std::swap(heading_, other->heading_);
    std::swap(num_satellites_, other->num_satellites_);
    std::swap(quality_, other->quality_);
    nmeas_.Swap(&other->nmeas_);
    std::swap(status_, other->status_);
    std::swap(fix_, other->fix_);
    std::swap(altitude_, other->altitude_);
    std::swap(utc_time_, other->utc_time_);
    std::swap(timestamp_, other->timestamp_);
    std::swap(accuracy_, other->accuracy_);
    std::swap(hdop_, other->hdop_);
    std::swap(vdop_, other->vdop_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata GnssData::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = GnssData_descriptor_;
  metadata.reflection = GnssData_reflection_;
  return metadata;
}


// ===================================================================

const ::google::protobuf::EnumDescriptor* GnssSource_Source_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return GnssSource_Source_descriptor_;
}
bool GnssSource_Source_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

#ifndef _MSC_VER
const GnssSource_Source GnssSource::UNKNOWN;
const GnssSource_Source GnssSource::VCM;
const GnssSource_Source GnssSource::TBM3;
const GnssSource_Source GnssSource::Source_MIN;
const GnssSource_Source GnssSource::Source_MAX;
const int GnssSource::Source_ARRAYSIZE;
#endif  // _MSC_VER
#ifndef _MSC_VER
const int GnssSource::kSourceFieldNumber;
#endif  // !_MSC_VER

GnssSource::GnssSource()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:ecu.lapi.pb.GnssSource)
}

void GnssSource::InitAsDefaultInstance() {
}

GnssSource::GnssSource(const GnssSource& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:ecu.lapi.pb.GnssSource)
}

void GnssSource::SharedCtor() {
  _cached_size_ = 0;
  source_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

GnssSource::~GnssSource() {
  // @@protoc_insertion_point(destructor:ecu.lapi.pb.GnssSource)
  SharedDtor();
}

void GnssSource::SharedDtor() {
  if (this != default_instance_) {
  }
}

void GnssSource::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* GnssSource::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return GnssSource_descriptor_;
}

const GnssSource& GnssSource::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_gnss_2eproto();
  return *default_instance_;
}

GnssSource* GnssSource::default_instance_ = NULL;

GnssSource* GnssSource::New() const {
  return new GnssSource;
}

void GnssSource::Clear() {
  source_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool GnssSource::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:ecu.lapi.pb.GnssSource)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional .ecu.lapi.pb.GnssSource.Source source = 1;
      case 1: {
        if (tag == 8) {
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::ecu::lapi::pb::GnssSource_Source_IsValid(value)) {
            set_source(static_cast< ::ecu::lapi::pb::GnssSource_Source >(value));
          } else {
            mutable_unknown_fields()->AddVarint(1, value);
          }
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:ecu.lapi.pb.GnssSource)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:ecu.lapi.pb.GnssSource)
  return false;
#undef DO_
}

void GnssSource::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:ecu.lapi.pb.GnssSource)
  // optional .ecu.lapi.pb.GnssSource.Source source = 1;
  if (has_source()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      1, this->source(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:ecu.lapi.pb.GnssSource)
}

::google::protobuf::uint8* GnssSource::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:ecu.lapi.pb.GnssSource)
  // optional .ecu.lapi.pb.GnssSource.Source source = 1;
  if (has_source()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      1, this->source(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:ecu.lapi.pb.GnssSource)
  return target;
}

int GnssSource::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional .ecu.lapi.pb.GnssSource.Source source = 1;
    if (has_source()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->source());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void GnssSource::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const GnssSource* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const GnssSource*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void GnssSource::MergeFrom(const GnssSource& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_source()) {
      set_source(from.source());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void GnssSource::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void GnssSource::CopyFrom(const GnssSource& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool GnssSource::IsInitialized() const {

  return true;
}

void GnssSource::Swap(GnssSource* other) {
  if (other != this) {
    std::swap(source_, other->source_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata GnssSource::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = GnssSource_descriptor_;
  metadata.reflection = GnssSource_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace pb
}  // namespace lapi
}  // namespace ecu

// @@protoc_insertion_point(global_scope)
