// SPDX-FileCopyrightText: Copyright 2022 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "yuzu_common/logging/log.h"
#include "applets/cabinet.h"

#include <thread>

CabinetApplet::~CabinetApplet() = default;

void DefaultCabinetApplet::Close() const {}

void DefaultCabinetApplet::ShowCabinetApplet(
    const CabinetCallback& callback, const CabinetParameters& parameters,
    std::shared_ptr<Service::NFC::NfcDevice> nfp_device) const {
    LOG_WARNING(Service_AM, "(STUBBED) called");
    callback(false, {});
}