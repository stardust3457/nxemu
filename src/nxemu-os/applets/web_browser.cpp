// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "applets/web_browser.h"
#include "yuzu_common/logging/log.h"

void DefaultWebBrowserApplet::Close()
{
}

void DefaultWebBrowserApplet::OpenLocalWebPage(const char * local_url_utf8, void * extract_user_data, ExtractRomFsFn extract_romfs, void * open_user_data, OpenWebPageFn open_callback) const
{
    LOG_WARNING(Service_AM, "(STUBBED) called, backend requested to open local web page at {}", local_url_utf8 != nullptr ? local_url_utf8 : "");

    if (extract_romfs != nullptr)
    {
        extract_romfs(extract_user_data);
    }
    if (open_callback != nullptr)
    {
        open_callback(open_user_data, static_cast<uint32_t>(WebExitReasonHost::WindowClosed), "http://localhost/");
    }
}

void DefaultWebBrowserApplet::OpenExternalWebPage(const char * external_url_utf8, void * user_data, OpenWebPageFn callback) const
{
    LOG_WARNING(Service_AM, "(STUBBED) called, backend requested to open external web page at {}", external_url_utf8 != nullptr ? external_url_utf8 : "");

    if (callback != nullptr)
    {
        callback(user_data, static_cast<uint32_t>(WebExitReasonHost::WindowClosed), "http://localhost/");
    }
}
