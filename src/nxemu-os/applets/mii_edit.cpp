// SPDX-FileCopyrightText: Copyright 2022 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "applets/mii_edit.h"
#include "yuzu_common/logging/log.h"

void DefaultMiiEditApplet::Close()
{
}

void DefaultMiiEditApplet::ShowMiiEdit(void * user_data, SimpleFinishedFn finished) const
{
    LOG_WARNING(Service_AM, "(STUBBED) called");
    finished(user_data);
}