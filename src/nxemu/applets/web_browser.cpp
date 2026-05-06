#include "web_browser.h"

#include <string>
#include <string_view>

#include <windows.h>
#include <shellapi.h>

namespace
{

std::wstring Utf8ToWide(std::string_view utf8)
{
    if (utf8.empty())
    {
        return {};
    }
    const int n = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), nullptr, 0);
    if (n <= 0)
    {
        return {};
    }
    std::wstring wide(static_cast<std::size_t>(n), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), wide.data(), n);
    return wide;
}

std::string FileUriFromLocalPath(std::string_view local_url_utf8)
{
    std::string uri{"file:///"};
    uri.reserve(local_url_utf8.size() + 8);
    for (const char c : local_url_utf8)
    {
        if (c == '\\')
        {
            uri.push_back('/');
        }
        else
        {
            uri.push_back(c);
        }
    }
    return uri;
}

void InvokeOpenResult(OpenWebPageFn callback, void * user_data, bool success, const char * last_url_utf8)
{
    if (callback == nullptr)
    {
        return;
    }
    callback(user_data, static_cast<uint32_t>(success ? WebExitReasonHost::EndButtonPressed : WebExitReasonHost::WindowClosed), last_url_utf8 != nullptr ? last_url_utf8 : "");
}

bool ShellOpen(HWND owner, const std::string & url_utf8)
{
    const std::wstring wide_url = Utf8ToWide(url_utf8);
    if (wide_url.empty())
    {
        return false;
    }
    const HINSTANCE result = ShellExecuteW(owner, L"open", wide_url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    return reinterpret_cast<INT_PTR>(result) > 32;
}

} // namespace

WebBrowserApplet::WebBrowserApplet() :
    m_hwnd(nullptr)
{
}

void WebBrowserApplet::AttachToWindow(const void * hwnd)
{
    m_hwnd = const_cast<void *>(hwnd);
}

void WebBrowserApplet::DetachWindow()
{
    m_hwnd = nullptr;
}

void WebBrowserApplet::Close()
{
}

void WebBrowserApplet::OpenLocalWebPage(const char * local_url_utf8, void * extract_user_data, ExtractRomFsFn extract_romfs, void * open_user_data, OpenWebPageFn open_callback) const
{
    if (extract_romfs != nullptr)
    {
        extract_romfs(extract_user_data);
    }

    const std::string local_path = local_url_utf8 != nullptr ? local_url_utf8 : "";
    if (local_path.empty())
    {
        InvokeOpenResult(open_callback, open_user_data, false, "");
        return;
    }

    const std::string file_uri = FileUriFromLocalPath(local_path);
    const bool success = ShellOpen(static_cast<HWND>(m_hwnd), file_uri);
    InvokeOpenResult(open_callback, open_user_data, success, success ? file_uri.c_str() : "");
}

void WebBrowserApplet::OpenExternalWebPage(const char * external_url_utf8, void * user_data, OpenWebPageFn callback) const
{
    const std::string external_url = external_url_utf8 != nullptr ? external_url_utf8 : "";
    if (external_url.empty())
    {
        InvokeOpenResult(callback, user_data, false, "");
        return;
    }

    const bool success = ShellOpen(static_cast<HWND>(m_hwnd), external_url);
    InvokeOpenResult(callback, user_data, success, success ? external_url.c_str() : "");
}
