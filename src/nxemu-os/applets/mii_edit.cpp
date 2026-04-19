// SPDX-FileCopyrightText: Copyright 2022 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "applets/mii_edit.h"
#include "yuzu_common/logging/log.h"

MiiEditApplet::~MiiEditApplet() = default;

void DefaultMiiEditApplet::Close() const
{
}

void DefaultMiiEditApplet::ShowMiiEdit(const MiiEditCallback & callback) const
{
    LOG_WARNING(Service_AM, "(STUBBED) called");

    callback();
}