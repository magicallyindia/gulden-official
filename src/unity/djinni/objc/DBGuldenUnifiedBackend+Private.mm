// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from libunity.djinni

#import "DBGuldenUnifiedBackend+Private.h"
#import "DBGuldenUnifiedBackend.h"
#import "DBGuldenUnifiedFrontend+Private.h"
#import "DBQrcodeRecord+Private.h"
#import "DBTransactionRecord+Private.h"
#import "DBUriRecipient+Private.h"
#import "DBUriRecord+Private.h"
#import "DJICppWrapperCache+Private.h"
#import "DJIError.h"
#import "DJIMarshal+Private.h"
#include <exception>
#include <stdexcept>
#include <utility>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@interface DBGuldenUnifiedBackend ()

- (id)initWithCpp:(const std::shared_ptr<::GuldenUnifiedBackend>&)cppRef;

@end

@implementation DBGuldenUnifiedBackend {
    ::djinni::CppProxyCache::Handle<std::shared_ptr<::GuldenUnifiedBackend>> _cppRefHandle;
}

- (id)initWithCpp:(const std::shared_ptr<::GuldenUnifiedBackend>&)cppRef
{
    if (self = [super init]) {
        _cppRefHandle.assign(cppRef);
    }
    return self;
}

+ (int32_t)InitUnityLib:(nonnull NSString *)dataDir
                signals:(nullable id<DBGuldenUnifiedFrontend>)signals {
    try {
        auto objcpp_result_ = ::GuldenUnifiedBackend::InitUnityLib(::djinni::String::toCpp(dataDir),
                                                                   ::djinni_generated::GuldenUnifiedFrontend::toCpp(signals));
        return ::djinni::I32::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

+ (void)TerminateUnityLib {
    try {
        ::GuldenUnifiedBackend::TerminateUnityLib();
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

+ (nonnull DBQrcodeRecord *)QRImageFromString:(nonnull NSString *)qrString
                                    widthHint:(int32_t)widthHint {
    try {
        auto objcpp_result_ = ::GuldenUnifiedBackend::QRImageFromString(::djinni::String::toCpp(qrString),
                                                                        ::djinni::I32::toCpp(widthHint));
        return ::djinni_generated::QrcodeRecord::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

+ (nonnull NSString *)GetReceiveAddress {
    try {
        auto objcpp_result_ = ::GuldenUnifiedBackend::GetReceiveAddress();
        return ::djinni::String::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

+ (nonnull NSString *)GetRecoveryPhrase {
    try {
        auto objcpp_result_ = ::GuldenUnifiedBackend::GetRecoveryPhrase();
        return ::djinni::String::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

+ (nonnull DBUriRecipient *)IsValidRecipient:(nonnull DBUriRecord *)request {
    try {
        auto objcpp_result_ = ::GuldenUnifiedBackend::IsValidRecipient(::djinni_generated::UriRecord::toCpp(request));
        return ::djinni_generated::UriRecipient::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

+ (BOOL)performPaymentToRecipient:(nonnull DBUriRecipient *)request {
    try {
        auto objcpp_result_ = ::GuldenUnifiedBackend::performPaymentToRecipient(::djinni_generated::UriRecipient::toCpp(request));
        return ::djinni::Bool::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

+ (nonnull NSArray<DBTransactionRecord *> *)getTransactionHistory {
    try {
        auto objcpp_result_ = ::GuldenUnifiedBackend::getTransactionHistory();
        return ::djinni::List<::djinni_generated::TransactionRecord>::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}


namespace djinni_generated {

auto GuldenUnifiedBackend::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return objc->_cppRefHandle.get();
}

auto GuldenUnifiedBackend::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return ::djinni::get_cpp_proxy<DBGuldenUnifiedBackend>(cpp);
}

}  // namespace djinni_generated

@end
