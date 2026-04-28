// SPDX-FileCopyrightText: Copyright 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "android_common.h"

#include <mutex>
#include <string>
#include <string_view>

#include <jni.h>

#include "yuzu_common/string_util.h"

namespace
{

std::mutex g_java_box_mutex;
jclass g_double_class = nullptr;
jmethodID g_double_ctor = nullptr;
jfieldID g_double_value = nullptr;
jclass g_integer_class = nullptr;
jmethodID g_integer_ctor = nullptr;
jfieldID g_integer_value = nullptr;
jclass g_boolean_class = nullptr;
jmethodID g_boolean_ctor = nullptr;
jfieldID g_boolean_value = nullptr;

void EnsureJavaBoxCache(JNIEnv * env)
{
    std::lock_guard lock{g_java_box_mutex};
    if (g_double_class != nullptr)
    {
        return;
    }
    jclass local_double = env->FindClass("java/lang/Double");
    jclass local_integer = env->FindClass("java/lang/Integer");
    jclass local_boolean = env->FindClass("java/lang/Boolean");
    if (!local_double || !local_integer || !local_boolean)
    {
        if (local_double)
        {
            env->DeleteLocalRef(local_double);
        }
        if (local_integer)
        {
            env->DeleteLocalRef(local_integer);
        }
        if (local_boolean)
        {
            env->DeleteLocalRef(local_boolean);
        }
        return;
    }
    g_double_class = static_cast<jclass>(env->NewGlobalRef(local_double));
    g_double_ctor = env->GetMethodID(local_double, "<init>", "(D)V");
    g_double_value = env->GetFieldID(local_double, "value", "D");
    g_integer_class = static_cast<jclass>(env->NewGlobalRef(local_integer));
    g_integer_ctor = env->GetMethodID(local_integer, "<init>", "(I)V");
    g_integer_value = env->GetFieldID(local_integer, "value", "I");
    g_boolean_class = static_cast<jclass>(env->NewGlobalRef(local_boolean));
    g_boolean_ctor = env->GetMethodID(local_boolean, "<init>", "(Z)V");
    g_boolean_value = env->GetFieldID(local_boolean, "value", "Z");
    env->DeleteLocalRef(local_double);
    env->DeleteLocalRef(local_integer);
    env->DeleteLocalRef(local_boolean);
}

} // namespace

namespace Common::Android
{

std::string GetJString(JNIEnv * env, jstring jstr)
{
    if (!jstr)
    {
        return {};
    }

    const jchar * jchars = env->GetStringChars(jstr, nullptr);
    const jsize length = env->GetStringLength(jstr);
    const std::u16string_view string_view(reinterpret_cast<const char16_t *>(jchars), static_cast<u32>(length));
    const std::string converted_string = Common::UTF16ToUTF8(string_view);
    env->ReleaseStringChars(jstr, jchars);

    return converted_string;
}

jstring ToJString(JNIEnv * env, std::string_view str)
{
    const std::u16string converted_string = Common::UTF8ToUTF16(str);
    return env->NewString(reinterpret_cast<const jchar *>(converted_string.data()), static_cast<jint>(converted_string.size()));
}

jstring ToJString(JNIEnv * env, std::u16string_view str)
{
    return ToJString(env, Common::UTF16ToUTF8(str));
}

double GetJDouble(JNIEnv * env, jobject jdouble)
{
    EnsureJavaBoxCache(env);
    if (!g_double_value)
    {
        return 0.0;
    }
    return env->GetDoubleField(jdouble, g_double_value);
}

jobject ToJDouble(JNIEnv * env, double value)
{
    EnsureJavaBoxCache(env);
    if (!g_double_class || !g_double_ctor)
    {
        return nullptr;
    }
    return env->NewObject(g_double_class, g_double_ctor, value);
}

s32 GetJInteger(JNIEnv * env, jobject jinteger)
{
    EnsureJavaBoxCache(env);
    if (!g_integer_value)
    {
        return 0;
    }
    return env->GetIntField(jinteger, g_integer_value);
}

jobject ToJInteger(JNIEnv * env, s32 value)
{
    EnsureJavaBoxCache(env);
    if (!g_integer_class || !g_integer_ctor)
    {
        return nullptr;
    }
    return env->NewObject(g_integer_class, g_integer_ctor, value);
}

bool GetJBoolean(JNIEnv * env, jobject jboolean)
{
    EnsureJavaBoxCache(env);
    if (!g_boolean_value)
    {
        return false;
    }
    return env->GetBooleanField(jboolean, g_boolean_value) != JNI_FALSE;
}

jobject ToJBoolean(JNIEnv * env, bool value)
{
    EnsureJavaBoxCache(env);
    if (!g_boolean_class || !g_boolean_ctor)
    {
        return nullptr;
    }
    return env->NewObject(g_boolean_class, g_boolean_ctor, static_cast<jboolean>(value ? JNI_TRUE : JNI_FALSE));
}

} // namespace Common::Android
