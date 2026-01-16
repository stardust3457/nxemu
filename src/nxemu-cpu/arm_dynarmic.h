// SPDX-FileCopyrightText: Copyright 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "dynarmic/interface/halt_reason.h"
#include <nxemu-module-spec/cpu.h>

Dynarmic::HaltReason TranslateDynarmicHaltReason(CpuHaltReason hr);
CpuHaltReason TranslateHaltReason(Dynarmic::HaltReason hr);

#ifdef __linux__

class ScopedJitExecution {
public:
    explicit ScopedJitExecution(IKernelProcess * process);
    ~ScopedJitExecution();
    static void RegisterHandler();
};

#else

class ScopedJitExecution {
public:
    explicit ScopedJitExecution(IKernelProcess * /*process*/)
    {
    }
    ~ScopedJitExecution() {}
    static void RegisterHandler() {}
};

#endif
