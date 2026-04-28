// SPDX-FileCopyrightText: Copyright 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#ifdef ANDROID

#include <functional>
#include <future>
#include <jni.h>

namespace Common::Android {

/// Called from JNI_OnLoad before any native worker thread uses JNI.
void SetJavaVM(JavaVM* vm);

JNIEnv* GetEnvForThread();

/**
 * Runs JNI on a fresh std::thread so it is safe when the caller is on a fiber
 * or stack that must not call AttachCurrentThread.
 */
template <typename T = void>
T RunJNIOnFiber(const std::function<T(JNIEnv*)>& work) {
    std::future<T> j_result = std::async(std::launch::async, [&] {
        auto* env = GetEnvForThread();
        return work(env);
    });
    return j_result.get();
}

} // namespace Common::Android

#endif