// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from libunity.djinni

#pragma once

#include "djinni_support.hpp"
#include "transaction_type.hpp"

namespace djinni_generated {

class NativeTransactionType final : ::djinni::JniEnum {
public:
    using CppType = ::TransactionType;
    using JniType = jobject;

    using Boxed = NativeTransactionType;

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return static_cast<CppType>(::djinni::JniClass<NativeTransactionType>::get().ordinal(jniEnv, j)); }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, CppType c) { return ::djinni::JniClass<NativeTransactionType>::get().create(jniEnv, static_cast<jint>(c)); }

private:
    NativeTransactionType() : JniEnum("com/gulden/jniunifiedbackend/TransactionType") {}
    friend ::djinni::JniClass<NativeTransactionType>;
};

}  // namespace djinni_generated
