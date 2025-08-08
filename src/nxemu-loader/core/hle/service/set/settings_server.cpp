// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <algorithm>
#include <array>
#include <chrono>
#include "yuzu_common/logging/log.h"
#include "yuzu_common/settings.h"
#include "core/hle/service/set/settings_server.h"

namespace Service::Set {
namespace {
constexpr std::size_t PRE_4_0_0_MAX_ENTRIES = 0xF;
constexpr std::size_t POST_4_0_0_MAX_ENTRIES = 0x40;

} // Anonymous namespace

LanguageCode GetLanguageCodeFromIndex(std::size_t index) {
    return available_language_codes.at(index);
}

} // namespace Service::Set
