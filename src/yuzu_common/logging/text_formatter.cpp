// SPDX-FileCopyrightText: 2014 Citra Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <array>
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef ANDROID
#include <android/log.h>
#endif

#include "yuzu_common/yuzu_assert.h"
#include "yuzu_common/logging/filter.h"
#include "yuzu_common/logging/log.h"
#include "yuzu_common/logging/log_entry.h"
#include "yuzu_common/logging/text_formatter.h"

namespace Common::Log {

std::string FormatLogMessage(const Entry& entry) {
    unsigned int time_seconds = static_cast<unsigned int>(entry.timestamp.count() / 1000000);
    unsigned int time_fractional = static_cast<unsigned int>(entry.timestamp.count() % 1000000);

    const char* class_name = GetLogClassName(entry.log_class);
    const char* level_name = GetLevelName(entry.log_level);

    return fmt::format("[{:4d}.{:06d}] {} <{}> {}:{}:{}: {}", time_seconds, time_fractional,
                       class_name, level_name, entry.filename, entry.function, entry.line_num,
                       entry.message);
}

void PrintMessage(const Entry& entry) {
    const auto str = FormatLogMessage(entry).append(1, '\n');
    fputs(str.c_str(), stderr);
}

void PrintColoredMessage(const Entry& entry) {
#ifdef _WIN32
    HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
    if (console_handle == INVALID_HANDLE_VALUE) {
        return;
    }

    CONSOLE_SCREEN_BUFFER_INFO original_info = {};
    GetConsoleScreenBufferInfo(console_handle, &original_info);

    WORD color = 0;
    switch (entry.log_level) {
    case LogLevel::Trace: // Grey
        color = FOREGROUND_INTENSITY;
        break;
    case LogLevel::Debug: // Cyan
        color = FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
    case LogLevel::Info: // Bright gray
        color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
    case LogLevel::Warning: // Bright yellow
        color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    case LogLevel::Error: // Bright red
        color = FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;
    case LogLevel::Critical: // Bright magenta
        color = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;
    case LogLevel::Count:
        UNREACHABLE();
    }

    SetConsoleTextAttribute(console_handle, color);
#else
#define ESC "\x1b"
    const char* color = "";
    switch (entry.log_level) {
    case LogLevel::Trace: // Grey
        color = ESC "[1;30m";
        break;
    case LogLevel::Debug: // Cyan
        color = ESC "[0;36m";
        break;
    case LogLevel::Info: // Bright gray
        color = ESC "[0;37m";
        break;
    case LogLevel::Warning: // Bright yellow
        color = ESC "[1;33m";
        break;
    case LogLevel::Error: // Bright red
        color = ESC "[1;31m";
        break;
    case LogLevel::Critical: // Bright magenta
        color = ESC "[1;35m";
        break;
    case LogLevel::Count:
        UNREACHABLE();
    }

    fputs(color, stderr);
#endif

    PrintMessage(entry);

#ifdef _WIN32
    SetConsoleTextAttribute(console_handle, original_info.wAttributes);
#else
    fputs(ESC "[0m", stderr);
#undef ESC
#endif
}

void PrintMessageToLogcat(const Entry& entry) {
#ifdef ANDROID
    const auto str = FormatLogMessage(entry);

    android_LogPriority android_log_priority;
    switch (entry.log_level) {
    case LogLevel::Trace:
        android_log_priority = ANDROID_LOG_VERBOSE;
        break;
    case LogLevel::Debug:
        android_log_priority = ANDROID_LOG_DEBUG;
        break;
    case LogLevel::Info:
        android_log_priority = ANDROID_LOG_INFO;
        break;
    case LogLevel::Warning:
        android_log_priority = ANDROID_LOG_WARN;
        break;
    case LogLevel::Error:
        android_log_priority = ANDROID_LOG_ERROR;
        break;
    case LogLevel::Critical:
        android_log_priority = ANDROID_LOG_FATAL;
        break;
    case LogLevel::Count:
        UNREACHABLE();
    }
    __android_log_print(android_log_priority, "YuzuNative", "%s", str.c_str());
#endif
}
} // namespace Common::Log
