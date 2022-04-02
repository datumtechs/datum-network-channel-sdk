// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: io_channel.proto

#include "io_channel.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG
namespace io_channel {
constexpr SendRequest::SendRequest(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : nodeid_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , id_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , data_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string){}
struct SendRequestDefaultTypeInternal {
  constexpr SendRequestDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~SendRequestDefaultTypeInternal() {}
  union {
    SendRequest _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT SendRequestDefaultTypeInternal _SendRequest_default_instance_;
constexpr RetCode::RetCode(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : code_(0){}
struct RetCodeDefaultTypeInternal {
  constexpr RetCodeDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~RetCodeDefaultTypeInternal() {}
  union {
    RetCode _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT RetCodeDefaultTypeInternal _RetCode_default_instance_;
}  // namespace io_channel
static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_io_5fchannel_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_io_5fchannel_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_io_5fchannel_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_io_5fchannel_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::io_channel::SendRequest, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::io_channel::SendRequest, nodeid_),
  PROTOBUF_FIELD_OFFSET(::io_channel::SendRequest, id_),
  PROTOBUF_FIELD_OFFSET(::io_channel::SendRequest, data_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::io_channel::RetCode, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::io_channel::RetCode, code_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::io_channel::SendRequest)},
  { 8, -1, sizeof(::io_channel::RetCode)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::io_channel::_SendRequest_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::io_channel::_RetCode_default_instance_),
};

const char descriptor_table_protodef_io_5fchannel_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\020io_channel.proto\022\nio_channel\"7\n\013SendRe"
  "quest\022\016\n\006nodeid\030\001 \001(\t\022\n\n\002id\030\002 \001(\t\022\014\n\004dat"
  "a\030\003 \001(\014\"\027\n\007RetCode\022\014\n\004code\030\001 \001(\0052C\n\tIoCh"
  "annel\0226\n\004Send\022\027.io_channel.SendRequest\032\023"
  ".io_channel.RetCode\"\000B\003\370\001\001b\006proto3"
  ;
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_io_5fchannel_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_io_5fchannel_2eproto = {
  false, false, 194, descriptor_table_protodef_io_5fchannel_2eproto, "io_channel.proto", 
  &descriptor_table_io_5fchannel_2eproto_once, nullptr, 0, 2,
  schemas, file_default_instances, TableStruct_io_5fchannel_2eproto::offsets,
  file_level_metadata_io_5fchannel_2eproto, file_level_enum_descriptors_io_5fchannel_2eproto, file_level_service_descriptors_io_5fchannel_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK ::PROTOBUF_NAMESPACE_ID::Metadata
descriptor_table_io_5fchannel_2eproto_metadata_getter(int index) {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_io_5fchannel_2eproto);
  return descriptor_table_io_5fchannel_2eproto.file_level_metadata[index];
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY static ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptorsRunner dynamic_init_dummy_io_5fchannel_2eproto(&descriptor_table_io_5fchannel_2eproto);
namespace io_channel {

// ===================================================================

class SendRequest::_Internal {
 public:
};

SendRequest::SendRequest(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:io_channel.SendRequest)
}
SendRequest::SendRequest(const SendRequest& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  nodeid_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_nodeid().empty()) {
    nodeid_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_nodeid(), 
      GetArena());
  }
  id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_id().empty()) {
    id_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_id(), 
      GetArena());
  }
  data_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_data().empty()) {
    data_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_data(), 
      GetArena());
  }
  // @@protoc_insertion_point(copy_constructor:io_channel.SendRequest)
}

void SendRequest::SharedCtor() {
nodeid_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
data_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

SendRequest::~SendRequest() {
  // @@protoc_insertion_point(destructor:io_channel.SendRequest)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void SendRequest::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  nodeid_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  id_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  data_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void SendRequest::ArenaDtor(void* object) {
  SendRequest* _this = reinterpret_cast< SendRequest* >(object);
  (void)_this;
}
void SendRequest::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void SendRequest::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void SendRequest::Clear() {
// @@protoc_insertion_point(message_clear_start:io_channel.SendRequest)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  nodeid_.ClearToEmpty();
  id_.ClearToEmpty();
  data_.ClearToEmpty();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* SendRequest::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // string nodeid = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_nodeid();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "io_channel.SendRequest.nodeid"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string id = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_id();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "io_channel.SendRequest.id"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // bytes data = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 26)) {
          auto str = _internal_mutable_data();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* SendRequest::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:io_channel.SendRequest)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string nodeid = 1;
  if (this->nodeid().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_nodeid().data(), static_cast<int>(this->_internal_nodeid().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "io_channel.SendRequest.nodeid");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_nodeid(), target);
  }

  // string id = 2;
  if (this->id().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_id().data(), static_cast<int>(this->_internal_id().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "io_channel.SendRequest.id");
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_id(), target);
  }

  // bytes data = 3;
  if (this->data().size() > 0) {
    target = stream->WriteBytesMaybeAliased(
        3, this->_internal_data(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:io_channel.SendRequest)
  return target;
}

size_t SendRequest::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:io_channel.SendRequest)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string nodeid = 1;
  if (this->nodeid().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_nodeid());
  }

  // string id = 2;
  if (this->id().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_id());
  }

  // bytes data = 3;
  if (this->data().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
        this->_internal_data());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void SendRequest::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:io_channel.SendRequest)
  GOOGLE_DCHECK_NE(&from, this);
  const SendRequest* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<SendRequest>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:io_channel.SendRequest)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:io_channel.SendRequest)
    MergeFrom(*source);
  }
}

void SendRequest::MergeFrom(const SendRequest& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:io_channel.SendRequest)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.nodeid().size() > 0) {
    _internal_set_nodeid(from._internal_nodeid());
  }
  if (from.id().size() > 0) {
    _internal_set_id(from._internal_id());
  }
  if (from.data().size() > 0) {
    _internal_set_data(from._internal_data());
  }
}

void SendRequest::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:io_channel.SendRequest)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void SendRequest::CopyFrom(const SendRequest& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:io_channel.SendRequest)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool SendRequest::IsInitialized() const {
  return true;
}

void SendRequest::InternalSwap(SendRequest* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  nodeid_.Swap(&other->nodeid_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  id_.Swap(&other->id_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  data_.Swap(&other->data_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}

::PROTOBUF_NAMESPACE_ID::Metadata SendRequest::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

class RetCode::_Internal {
 public:
};

RetCode::RetCode(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:io_channel.RetCode)
}
RetCode::RetCode(const RetCode& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  code_ = from.code_;
  // @@protoc_insertion_point(copy_constructor:io_channel.RetCode)
}

void RetCode::SharedCtor() {
code_ = 0;
}

RetCode::~RetCode() {
  // @@protoc_insertion_point(destructor:io_channel.RetCode)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void RetCode::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void RetCode::ArenaDtor(void* object) {
  RetCode* _this = reinterpret_cast< RetCode* >(object);
  (void)_this;
}
void RetCode::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void RetCode::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void RetCode::Clear() {
// @@protoc_insertion_point(message_clear_start:io_channel.RetCode)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  code_ = 0;
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* RetCode::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // int32 code = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          code_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* RetCode::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:io_channel.RetCode)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 code = 1;
  if (this->code() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(1, this->_internal_code(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:io_channel.RetCode)
  return target;
}

size_t RetCode::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:io_channel.RetCode)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // int32 code = 1;
  if (this->code() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
        this->_internal_code());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void RetCode::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:io_channel.RetCode)
  GOOGLE_DCHECK_NE(&from, this);
  const RetCode* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<RetCode>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:io_channel.RetCode)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:io_channel.RetCode)
    MergeFrom(*source);
  }
}

void RetCode::MergeFrom(const RetCode& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:io_channel.RetCode)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.code() != 0) {
    _internal_set_code(from._internal_code());
  }
}

void RetCode::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:io_channel.RetCode)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void RetCode::CopyFrom(const RetCode& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:io_channel.RetCode)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RetCode::IsInitialized() const {
  return true;
}

void RetCode::InternalSwap(RetCode* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  swap(code_, other->code_);
}

::PROTOBUF_NAMESPACE_ID::Metadata RetCode::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace io_channel
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::io_channel::SendRequest* Arena::CreateMaybeMessage< ::io_channel::SendRequest >(Arena* arena) {
  return Arena::CreateMessageInternal< ::io_channel::SendRequest >(arena);
}
template<> PROTOBUF_NOINLINE ::io_channel::RetCode* Arena::CreateMaybeMessage< ::io_channel::RetCode >(Arena* arena) {
  return Arena::CreateMessageInternal< ::io_channel::RetCode >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>