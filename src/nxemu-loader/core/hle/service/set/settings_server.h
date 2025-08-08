// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "core/hle/service/set/settings_types.h"

namespace Core {
class System;
}

namespace Service::Set {

LanguageCode GetLanguageCodeFromIndex(std::size_t idx);

} // namespace Service::Set
