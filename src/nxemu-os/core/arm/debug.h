// SPDX-FileCopyrightText: Copyright 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <optional>

#include "core/hle/kernel/k_thread.h"

namespace Core {

struct BacktraceEntry {
    std::string module;
    u64 address;
    u64 original_address;
    u64 offset;
    std::string name;
};

std::vector<BacktraceEntry> GetBacktraceFromContext(Kernel::KProcess * process, const CpuThreadContext & ctx);

} // namespace Core
