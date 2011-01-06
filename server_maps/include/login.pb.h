// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: login.proto

#ifndef PROTOBUF_login_2eproto__INCLUDED
#define PROTOBUF_login_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2003000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2003000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/service.h>
// @@protoc_insertion_point(includes)

namespace login {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_login_2eproto();
void protobuf_AssignDesc_login_2eproto();
void protobuf_ShutdownFile_login_2eproto();

class LoginRequest;
class LoginResponse;

// ===================================================================

class LoginRequest : public ::google::protobuf::Message {
 public:
  LoginRequest();
  virtual ~LoginRequest();
  
  LoginRequest(const LoginRequest& from);
  
  inline LoginRequest& operator=(const LoginRequest& from) {
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
  static const LoginRequest& default_instance();
  
  void Swap(LoginRequest* other);
  
  // implements Message ----------------------------------------------
  
  LoginRequest* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const LoginRequest& from);
  void MergeFrom(const LoginRequest& from);
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
  
  // required string urs = 1;
  inline bool has_urs() const;
  inline void clear_urs();
  static const int kUrsFieldNumber = 1;
  inline const ::std::string& urs() const;
  inline void set_urs(const ::std::string& value);
  inline void set_urs(const char* value);
  inline void set_urs(const char* value, size_t size);
  inline ::std::string* mutable_urs();
  
  // required string pwd = 2;
  inline bool has_pwd() const;
  inline void clear_pwd();
  static const int kPwdFieldNumber = 2;
  inline const ::std::string& pwd() const;
  inline void set_pwd(const ::std::string& value);
  inline void set_pwd(const char* value);
  inline void set_pwd(const char* value, size_t size);
  inline ::std::string* mutable_pwd();
  
  // @@protoc_insertion_point(class_scope:login.LoginRequest)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::std::string* urs_;
  static const ::std::string _default_urs_;
  ::std::string* pwd_;
  static const ::std::string _default_pwd_;
  friend void  protobuf_AddDesc_login_2eproto();
  friend void protobuf_AssignDesc_login_2eproto();
  friend void protobuf_ShutdownFile_login_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static LoginRequest* default_instance_;
};
// -------------------------------------------------------------------

class LoginResponse : public ::google::protobuf::Message {
 public:
  LoginResponse();
  virtual ~LoginResponse();
  
  LoginResponse(const LoginResponse& from);
  
  inline LoginResponse& operator=(const LoginResponse& from) {
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
  static const LoginResponse& default_instance();
  
  void Swap(LoginResponse* other);
  
  // implements Message ----------------------------------------------
  
  LoginResponse* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const LoginResponse& from);
  void MergeFrom(const LoginResponse& from);
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
  
  // required int32 uid = 1;
  inline bool has_uid() const;
  inline void clear_uid();
  static const int kUidFieldNumber = 1;
  inline ::google::protobuf::int32 uid() const;
  inline void set_uid(::google::protobuf::int32 value);
  
  // @@protoc_insertion_point(class_scope:login.LoginResponse)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::int32 uid_;
  friend void  protobuf_AddDesc_login_2eproto();
  friend void protobuf_AssignDesc_login_2eproto();
  friend void protobuf_ShutdownFile_login_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static LoginResponse* default_instance_;
};
// ===================================================================

class LoginService_Stub;

class LoginService : public ::google::protobuf::Service {
 protected:
  // This class should be treated as an abstract interface.
  inline LoginService() {};
 public:
  virtual ~LoginService();
  
  typedef LoginService_Stub Stub;
  
  static const ::google::protobuf::ServiceDescriptor* descriptor();
  
  virtual void Echo(::google::protobuf::RpcController* controller,
                       const ::login::LoginRequest* request,
                       ::login::LoginResponse* response,
                       ::google::protobuf::Closure* done);
  
  // implements Service ----------------------------------------------
  
  const ::google::protobuf::ServiceDescriptor* GetDescriptor();
  void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                  ::google::protobuf::RpcController* controller,
                  const ::google::protobuf::Message* request,
                  ::google::protobuf::Message* response,
                  ::google::protobuf::Closure* done);
  const ::google::protobuf::Message& GetRequestPrototype(
    const ::google::protobuf::MethodDescriptor* method) const;
  const ::google::protobuf::Message& GetResponsePrototype(
    const ::google::protobuf::MethodDescriptor* method) const;

 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(LoginService);
};

class LoginService_Stub : public LoginService {
 public:
  LoginService_Stub(::google::protobuf::RpcChannel* channel);
  LoginService_Stub(::google::protobuf::RpcChannel* channel,
                   ::google::protobuf::Service::ChannelOwnership ownership);
  ~LoginService_Stub();
  
  inline ::google::protobuf::RpcChannel* channel() { return channel_; }
  
  // implements LoginService ------------------------------------------
  
  void Echo(::google::protobuf::RpcController* controller,
                       const ::login::LoginRequest* request,
                       ::login::LoginResponse* response,
                       ::google::protobuf::Closure* done);
 private:
  ::google::protobuf::RpcChannel* channel_;
  bool owns_channel_;
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(LoginService_Stub);
};


// ===================================================================


// ===================================================================

// LoginRequest

// required string urs = 1;
inline bool LoginRequest::has_urs() const {
  return _has_bit(0);
}
inline void LoginRequest::clear_urs() {
  if (urs_ != &_default_urs_) {
    urs_->clear();
  }
  _clear_bit(0);
}
inline const ::std::string& LoginRequest::urs() const {
  return *urs_;
}
inline void LoginRequest::set_urs(const ::std::string& value) {
  _set_bit(0);
  if (urs_ == &_default_urs_) {
    urs_ = new ::std::string;
  }
  urs_->assign(value);
}
inline void LoginRequest::set_urs(const char* value) {
  _set_bit(0);
  if (urs_ == &_default_urs_) {
    urs_ = new ::std::string;
  }
  urs_->assign(value);
}
inline void LoginRequest::set_urs(const char* value, size_t size) {
  _set_bit(0);
  if (urs_ == &_default_urs_) {
    urs_ = new ::std::string;
  }
  urs_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* LoginRequest::mutable_urs() {
  _set_bit(0);
  if (urs_ == &_default_urs_) {
    urs_ = new ::std::string;
  }
  return urs_;
}

// required string pwd = 2;
inline bool LoginRequest::has_pwd() const {
  return _has_bit(1);
}
inline void LoginRequest::clear_pwd() {
  if (pwd_ != &_default_pwd_) {
    pwd_->clear();
  }
  _clear_bit(1);
}
inline const ::std::string& LoginRequest::pwd() const {
  return *pwd_;
}
inline void LoginRequest::set_pwd(const ::std::string& value) {
  _set_bit(1);
  if (pwd_ == &_default_pwd_) {
    pwd_ = new ::std::string;
  }
  pwd_->assign(value);
}
inline void LoginRequest::set_pwd(const char* value) {
  _set_bit(1);
  if (pwd_ == &_default_pwd_) {
    pwd_ = new ::std::string;
  }
  pwd_->assign(value);
}
inline void LoginRequest::set_pwd(const char* value, size_t size) {
  _set_bit(1);
  if (pwd_ == &_default_pwd_) {
    pwd_ = new ::std::string;
  }
  pwd_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* LoginRequest::mutable_pwd() {
  _set_bit(1);
  if (pwd_ == &_default_pwd_) {
    pwd_ = new ::std::string;
  }
  return pwd_;
}

// -------------------------------------------------------------------

// LoginResponse

// required int32 uid = 1;
inline bool LoginResponse::has_uid() const {
  return _has_bit(0);
}
inline void LoginResponse::clear_uid() {
  uid_ = 0;
  _clear_bit(0);
}
inline ::google::protobuf::int32 LoginResponse::uid() const {
  return uid_;
}
inline void LoginResponse::set_uid(::google::protobuf::int32 value) {
  _set_bit(0);
  uid_ = value;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace login

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_login_2eproto__INCLUDED