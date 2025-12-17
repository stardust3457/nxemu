// SPDX-FileCopyrightText: Copyright 2020 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <algorithm>
#include <cstring>

#include "yuzu_common/yuzu_assert.h"
#include "yuzu_common/logging/log.h"
#include "yuzu_common/string_util.h"
#include "core/core.h"
#include "core/hle/result.h"
#include "core/hle/service/am/am.h"
#include "core/hle/service/am/frontend/applet_controller.h"
#include "core/hle/service/am/service/storage.h"

namespace Service::AM::Frontend {

[[maybe_unused]] constexpr Result ResultControllerSupportCanceled{ErrorModule::HID, 3101};
[[maybe_unused]] constexpr Result ResultControllerSupportNotSupportedNpadStyle{ErrorModule::HID,
                                                                               3102};

Controller::Controller(Core::System& system_, std::shared_ptr<Applet> applet_,
                       LibraryAppletMode applet_mode_)
    : FrontendApplet{system_, applet_, applet_mode_} {}

Controller::~Controller() = default;

void Controller::Initialize() {
    FrontendApplet::Initialize();

    LOG_INFO(Service_HID, "Initializing Controller Applet.");

    LOG_DEBUG(Service_HID,
              "Initializing Applet with common_args: arg_version={}, lib_version={}, "
              "play_startup_sound={}, size={}, system_tick={}, theme_color={}",
              common_args.arguments_version, common_args.library_version,
              common_args.play_startup_sound, common_args.size, common_args.system_tick,
              common_args.theme_color);

    controller_applet_version = ControllerAppletVersion{common_args.library_version};

    const std::shared_ptr<IStorage> private_arg_storage = PopInData();
    ASSERT(private_arg_storage != nullptr);

    const auto& private_arg = private_arg_storage->GetData();
    ASSERT(private_arg.size() == sizeof(ControllerSupportArgPrivate));

    std::memcpy(&controller_private_arg, private_arg.data(), private_arg.size());
    ASSERT_MSG(controller_private_arg.arg_private_size == sizeof(ControllerSupportArgPrivate),
               "Unknown ControllerSupportArgPrivate revision={} with size={}",
               controller_applet_version, controller_private_arg.arg_private_size);

    // Some games such as Cave Story+ set invalid values for the ControllerSupportMode.
    // Defer to arg_size to set the ControllerSupportMode.
    if (controller_private_arg.mode >= ControllerSupportMode::MaxControllerSupportMode) {
        switch (controller_private_arg.arg_size) {
        case sizeof(ControllerSupportArgOld):
        case sizeof(ControllerSupportArgNew):
            controller_private_arg.mode = ControllerSupportMode::ShowControllerSupport;
            break;
        case sizeof(ControllerUpdateFirmwareArg):
            controller_private_arg.mode = ControllerSupportMode::ShowControllerFirmwareUpdate;
            break;
        case sizeof(ControllerKeyRemappingArg):
            controller_private_arg.mode =
                ControllerSupportMode::ShowControllerKeyRemappingForSystem;
            break;
        default:
            UNIMPLEMENTED_MSG("Unknown ControllerPrivateArg mode={} with arg_size={}",
                              controller_private_arg.mode, controller_private_arg.arg_size);
            controller_private_arg.mode = ControllerSupportMode::ShowControllerSupport;
            break;
        }
    }

    // Some games such as Cave Story+ set invalid values for the ControllerSupportCaller.
    // This is always 0 (Application) except with ShowControllerFirmwareUpdateForSystem.
    if (controller_private_arg.caller >= ControllerSupportCaller::MaxControllerSupportCaller) {
        if (controller_private_arg.flag_1 &&
            (controller_private_arg.mode == ControllerSupportMode::ShowControllerFirmwareUpdate ||
             controller_private_arg.mode ==
                 ControllerSupportMode::ShowControllerKeyRemappingForSystem)) {
            controller_private_arg.caller = ControllerSupportCaller::System;
        } else {
            controller_private_arg.caller = ControllerSupportCaller::Application;
        }
    }

    switch (controller_private_arg.mode) {
    case ControllerSupportMode::ShowControllerSupport:
    case ControllerSupportMode::ShowControllerStrapGuide: {
        const std::shared_ptr<IStorage> user_arg_storage = PopInData();
        ASSERT(user_arg_storage != nullptr);

        const auto& user_arg = user_arg_storage->GetData();
        switch (controller_applet_version) {
        case ControllerAppletVersion::Version3:
        case ControllerAppletVersion::Version4:
        case ControllerAppletVersion::Version5:
            ASSERT(user_arg.size() == sizeof(ControllerSupportArgOld));
            std::memcpy(&controller_user_arg_old, user_arg.data(), user_arg.size());
            break;
        case ControllerAppletVersion::Version7:
        case ControllerAppletVersion::Version8:
            ASSERT(user_arg.size() == sizeof(ControllerSupportArgNew));
            std::memcpy(&controller_user_arg_new, user_arg.data(), user_arg.size());
            break;
        default:
            UNIMPLEMENTED_MSG("Unknown ControllerSupportArg revision={} with size={}",
                              controller_applet_version, controller_private_arg.arg_size);
            ASSERT(user_arg.size() >= sizeof(ControllerSupportArgNew));
            std::memcpy(&controller_user_arg_new, user_arg.data(), sizeof(ControllerSupportArgNew));
            break;
        }
        break;
    }
    case ControllerSupportMode::ShowControllerFirmwareUpdate: {
        const std::shared_ptr<IStorage> update_arg_storage = PopInData();
        ASSERT(update_arg_storage != nullptr);

        const auto& update_arg = update_arg_storage->GetData();
        ASSERT(update_arg.size() == sizeof(ControllerUpdateFirmwareArg));

        std::memcpy(&controller_update_arg, update_arg.data(), update_arg.size());
        break;
    }
    case ControllerSupportMode::ShowControllerKeyRemappingForSystem: {
        const std::shared_ptr<IStorage> remapping_arg_storage = PopInData();
        ASSERT(remapping_arg_storage != nullptr);

        const auto& remapping_arg = remapping_arg_storage->GetData();
        ASSERT(remapping_arg.size() == sizeof(ControllerKeyRemappingArg));

        std::memcpy(&controller_key_remapping_arg, remapping_arg.data(), remapping_arg.size());
        break;
    }
    default: {
        UNIMPLEMENTED_MSG("Unimplemented ControllerSupportMode={}", controller_private_arg.mode);
        break;
    }
    }
}

Result Controller::GetStatus() const {
    return status;
}

void Controller::ExecuteInteractive() {
    ASSERT_MSG(false, "Attempted to call interactive execution on non-interactive applet.");
}

Result Controller::RequestExit() {
    UNIMPLEMENTED();
    R_SUCCEED();
}

} // namespace Service::AM::Frontend
