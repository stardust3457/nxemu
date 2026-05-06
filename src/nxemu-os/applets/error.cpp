// SPDX-FileCopyrightText: Copyright 2019 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "applets/error.h"
#include "core/hle/result.h"
#include "yuzu_common/logging/log.h"

ErrorApplet::~ErrorApplet() = default;

void DefaultErrorApplet::Close()
{
}

void DefaultErrorApplet::ShowError(uint32_t result_raw, void * user_data, SimpleFinishedFn finished) const
{
    const Result error{result_raw};
    LOG_CRITICAL(Service_Fatal, "Application requested error display: {:04}-{:04} (raw={:08X})", error.GetModule(), error.GetDescription(), error.raw);
    if (finished != nullptr)
    {
        finished(user_data);
    }
}

void DefaultErrorApplet::ShowErrorWithTimestamp(uint32_t result_raw, int64_t time_unix_seconds, void * user_data, SimpleFinishedFn finished) const
{
    const Result error{result_raw};
    LOG_CRITICAL(Service_Fatal, "Application requested error display: {:04X}-{:04X} (raw={:08X}) with timestamp={}", error.GetModule(), error.GetDescription(), error.raw, time_unix_seconds);
    if (finished != nullptr)
    {
        finished(user_data);
    }
}

void DefaultErrorApplet::ShowCustomErrorText(uint32_t result_raw, const char * dialog_text_utf8, const char * fullscreen_text_utf8, void * user_data, SimpleFinishedFn finished) const
{
    const Result error{result_raw};
    LOG_CRITICAL(Service_Fatal, "Application requested custom error with error_code={:04X}-{:04X} (raw={:08X})", error.GetModule(), error.GetDescription(), error.raw);
    LOG_CRITICAL(Service_Fatal, "    Dialog Text: {}", dialog_text_utf8 != nullptr ? dialog_text_utf8 : "");
    LOG_CRITICAL(Service_Fatal, "    Fullscreen Text: {}", fullscreen_text_utf8 != nullptr ? fullscreen_text_utf8 : "");
    if (finished != nullptr)
    {
        finished(user_data);
    }
}
