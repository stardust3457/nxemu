// SPDX-FileCopyrightText: Copyright 2019 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <ctime>
#include <fstream>
#include <iomanip>

#include <fmt/chrono.h>
#include <fmt/format.h>

#include "yuzu_common/fs/file.h"
#include "yuzu_common/fs/fs.h"
#include "yuzu_common/fs/path_util.h"
#include "yuzu_common/hex_util.h"
#include "yuzu_common/settings.h"
#include "core/core.h"
#include "core/hle/kernel/k_page_table.h"
#include "core/hle/kernel/k_process.h"
#include "core/hle/result.h"
#include "core/hle/service/hle_ipc.h"
#include "core/memory.h"
#include "core/reporter.h"

namespace {

std::filesystem::path GetPath(std::string_view type, u64 title_id, std::string_view timestamp) {
    return Common::FS::GetYuzuPath(Common::FS::YuzuPath::LogDir) / type /
           fmt::format("{:016X}_{}.json", title_id, timestamp);
}

std::string GetTimestamp() {
    const auto time = std::time(nullptr);
    return fmt::format("{:%FT%H-%M-%S}", *std::localtime(&time));
}

} // Anonymous namespace

namespace Core {

Reporter::Reporter(System& system_) : system(system_) {
    ClearFSAccessLog();
}

Reporter::~Reporter() = default;

void Reporter::SaveCrashReport(u64 title_id, Result result, u64 set_flags, u64 entry_point, u64 sp,
                               u64 pc, u64 pstate, u64 afsr0, u64 afsr1, u64 esr, u64 far,
                               const std::array<u64, 31>& registers,
                               const std::array<u64, 32>& backtrace, u32 backtrace_size,
                               const std::string& arch, u32 unk10) const {
    if (!IsReportingEnabled()) {
        return;
    }

    UNIMPLEMENTED();
}

void Reporter::SaveSvcBreakReport(u32 type, bool signal_debugger, u64 info1, u64 info2,
                                  const std::optional<std::vector<u8>>& resolved_buffer) const {
    if (!IsReportingEnabled()) {
        return;
    }

    UNIMPLEMENTED();
}

void Reporter::SaveUnimplementedFunctionReport(Service::HLERequestContext& ctx, u32 command_id,
                                               const std::string& name,
                                               const std::string& service_name) const {
    if (!IsReportingEnabled()) {
        return;
    }

    UNIMPLEMENTED();
}

void Reporter::SaveUnimplementedAppletReport(
    u32 applet_id, u32 common_args_version, u32 library_version, u32 theme_color,
    bool startup_sound, u64 system_tick, const std::vector<std::vector<u8>>& normal_channel,
    const std::vector<std::vector<u8>>& interactive_channel) const {
    if (!IsReportingEnabled()) {
        return;
    }

    UNIMPLEMENTED();
}

void Reporter::SavePlayReport(PlayReportType type, u64 title_id,
                              const std::vector<std::span<const u8>>& data,
                              std::optional<u64> process_id, std::optional<u128> user_id) const {
    if (!IsReportingEnabled()) {
        return;
    }

    UNIMPLEMENTED();
}

void Reporter::SaveErrorReport(u64 title_id, Result result,
                               const std::optional<std::string>& custom_text_main,
                               const std::optional<std::string>& custom_text_detail) const {
    if (!IsReportingEnabled()) {
        return;
    }

    UNIMPLEMENTED();
}

void Reporter::SaveFSAccessLog(std::string_view log_message) const {
    const auto access_log_path =
        Common::FS::GetYuzuPath(Common::FS::YuzuPath::SDMCDir) / "FsAccessLog.txt";

    void(Common::FS::AppendStringToFile(access_log_path, Common::FS::FileType::TextFile,
                                        log_message));
}

void Reporter::SaveUserReport() const {
    if (!IsReportingEnabled()) {
        return;
    }
    UNIMPLEMENTED();
}

void Reporter::ClearFSAccessLog() const {
    const auto access_log_path =
        Common::FS::GetYuzuPath(Common::FS::YuzuPath::SDMCDir) / "FsAccessLog.txt";

    Common::FS::IOFile access_log_file{access_log_path, Common::FS::FileAccessMode::Write,
                                       Common::FS::FileType::TextFile};

    if (!access_log_file.IsOpen()) {
        LOG_ERROR(Common_Filesystem, "Failed to clear the filesystem access log.");
    }
}

bool Reporter::IsReportingEnabled() const {
    return Settings::values.reporting_services.GetValue();
}

} // namespace Core
