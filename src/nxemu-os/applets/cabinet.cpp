// SPDX-FileCopyrightText: Copyright 2022 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "yuzu_common/logging/log.h"
#include "applets/cabinet.h"

#include <thread>

void DefaultCabinetApplet::Close() {}

void DefaultCabinetApplet::ShowCabinetApplet(void * user_data, CabinetFinishedFn on_finished, const CabinetParameters * parameters)
{
    LOG_WARNING(Service_AM, "(STUBBED) called");
    on_finished(user_data, false, {});
}