// SPDX-FileCopyrightText: Copyright 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "yuzu_hid_core/hid_result.h"
#include "yuzu_hid_core/hid_types.h"

namespace Service::HID {

constexpr bool IsNpadIdValid(const NpadIdType npad_id) {
    switch (npad_id) {
    case NpadIdType::Player1:
    case NpadIdType::Player2:
    case NpadIdType::Player3:
    case NpadIdType::Player4:
    case NpadIdType::Player5:
    case NpadIdType::Player6:
    case NpadIdType::Player7:
    case NpadIdType::Player8:
    case NpadIdType::Other:
    case NpadIdType::Handheld:
        return true;
    default:
        return false;
    }
}

constexpr Result IsSixaxisHandleValid(const Core::HID::SixAxisSensorHandle& handle) {
    const auto npad_id = IsNpadIdValid(static_cast<NpadIdType>(handle.npad_id));
    const bool device_index = handle.device_index < Core::HID::DeviceIndex::MaxDeviceIndex;

    if (!npad_id) {
        return ResultInvalidNpadId;
    }
    if (!device_index) {
        return NpadDeviceIndexOutOfRange;
    }

    return ResultSuccess;
}

constexpr Result IsVibrationHandleValid(const Core::HID::VibrationDeviceHandle& handle) {
    switch (handle.npad_type) {
    case NpadStyleIndex::Fullkey:
    case NpadStyleIndex::Handheld:
    case NpadStyleIndex::JoyconDual:
    case NpadStyleIndex::JoyconLeft:
    case NpadStyleIndex::JoyconRight:
    case NpadStyleIndex::GameCube:
    case NpadStyleIndex::N64:
    case NpadStyleIndex::SystemExt:
    case NpadStyleIndex::System:
        // These support vibration
        break;
    default:
        return ResultVibrationInvalidStyleIndex;
    }

    if (!IsNpadIdValid(static_cast<NpadIdType>(handle.npad_id))) {
        return ResultVibrationInvalidNpadId;
    }

    if (handle.device_index >= Core::HID::DeviceIndex::MaxDeviceIndex) {
        return ResultVibrationDeviceIndexOutOfRange;
    }

    return ResultSuccess;
}

/// Converts a NpadIdType to an array index.
constexpr size_t NpadIdTypeToIndex(NpadIdType npad_id_type) {
    switch (npad_id_type) {
    case NpadIdType::Player1:
        return 0;
    case NpadIdType::Player2:
        return 1;
    case NpadIdType::Player3:
        return 2;
    case NpadIdType::Player4:
        return 3;
    case NpadIdType::Player5:
        return 4;
    case NpadIdType::Player6:
        return 5;
    case NpadIdType::Player7:
        return 6;
    case NpadIdType::Player8:
        return 7;
    case NpadIdType::Handheld:
        return 8;
    case NpadIdType::Other:
        return 9;
    default:
        return 8;
    }
}

/// Converts an array index to a NpadIdType
constexpr NpadIdType IndexToNpadIdType(size_t index) {
    switch (index) {
    case 0:
        return NpadIdType::Player1;
    case 1:
        return NpadIdType::Player2;
    case 2:
        return NpadIdType::Player3;
    case 3:
        return NpadIdType::Player4;
    case 4:
        return NpadIdType::Player5;
    case 5:
        return NpadIdType::Player6;
    case 6:
        return NpadIdType::Player7;
    case 7:
        return NpadIdType::Player8;
    case 8:
        return NpadIdType::Handheld;
    case 9:
        return NpadIdType::Other;
    default:
        return NpadIdType::Invalid;
    }
}

constexpr NpadStyleSet GetStylesetByIndex(std::size_t index) {
    switch (index) {
    case 0:
        return NpadStyleSet::Fullkey;
    case 1:
        return NpadStyleSet::Handheld;
    case 2:
        return NpadStyleSet::JoyDual;
    case 3:
        return NpadStyleSet::JoyLeft;
    case 4:
        return NpadStyleSet::JoyRight;
    case 5:
        return NpadStyleSet::Palma;
    default:
        return NpadStyleSet::None;
    }
}

} // namespace Service::HID
