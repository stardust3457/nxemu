// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <cstring>

#include "core/core.h"
#include "core/hle/service/acc/errors.h"
#include "core/hle/service/am/am.h"
#include "core/hle/service/am/frontend/applet_profile_select.h"
#include "core/hle/service/am/service/storage.h"
#include "yuzu_common/string_util.h"
#include "yuzu_common/yuzu_assert.h"

namespace Service::AM::Frontend
{

ProfileSelect::ProfileSelect(Core::System & system_, std::shared_ptr<Applet> applet_, LibraryAppletMode applet_mode_) :
    FrontendApplet{system_, applet_, applet_mode_}
{
}

ProfileSelect::~ProfileSelect() = default;

void ProfileSelect::Initialize()
{
    complete = false;
    status = ResultSuccess;
    final_data.clear();

    FrontendApplet::Initialize();
    profile_select_version = ProfileSelectAppletVersion{common_args.library_version};

    const std::shared_ptr<IStorage> user_config_storage = PopInData();
    ASSERT(user_config_storage != nullptr);
    const auto & user_config = user_config_storage->GetData();

    LOG_INFO(Service_AM, "Initializing Profile Select Applet with version={}", profile_select_version);

    switch (profile_select_version)
    {
    case ProfileSelectAppletVersion::Version1:
        ASSERT(user_config.size() == sizeof(UiSettingsV1));
        std::memcpy(&config_old, user_config.data(), sizeof(UiSettingsV1));
        break;
    case ProfileSelectAppletVersion::Version2:
    case ProfileSelectAppletVersion::Version3:
        ASSERT(user_config.size() == sizeof(UiSettings));
        std::memcpy(&config, user_config.data(), sizeof(UiSettings));
        break;
    default:
        UNIMPLEMENTED_MSG("Unknown profile_select_version = {}", profile_select_version);
        break;
    }
}

Result ProfileSelect::GetStatus() const
{
    return status;
}

void ProfileSelect::ExecuteInteractive()
{
    ASSERT_MSG(false, "Attempted to call interactive execution on non-interactive applet.");
}

void ProfileSelect::Execute()
{
    UNIMPLEMENTED();
}

void ProfileSelect::SelectionComplete(std::optional<Common::UUID> uuid)
{
    UiReturnArg output{};

    if (uuid.has_value() && uuid->IsValid())
    {
        output.result = 0;
        output.uuid_selected = *uuid;
    }
    else
    {
        status = Account::ResultCancelledByUser;
        output.result = Account::ResultCancelledByUser.raw;
        output.uuid_selected = Common::InvalidUUID;
    }

    final_data = std::vector<u8>(sizeof(UiReturnArg));
    std::memcpy(final_data.data(), &output, final_data.size());

    PushOutData(std::make_shared<IStorage>(system, std::move(final_data)));
    Exit();
}

Result ProfileSelect::RequestExit()
{
    UNIMPLEMENTED();
    R_SUCCEED();
}

} // namespace Service::AM::Frontend
