#include "notification.h"
#include "settings/ui_settings.h"
#include <Windows.h>
#include <common/std_string.h>

std::unique_ptr<Notification> Notification::s_instance;

Notification::Notification()
{
}

void Notification::DisplayError(const char * message, const char * title) const
{
    MessageBoxW(nullptr, stdstr(message).ToUTF16().c_str(), stdstr(title).ToUTF16().c_str(), MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
}

void Notification::BreakPoint(const char * fileName, uint32_t lineNumber)
{
    DisplayError(stdstr_f("Break point found at\n%s\n%d", fileName, lineNumber).c_str(), "Error");
    if (IsDebuggerPresent() != 0)
    {
        DebugBreak();
    }
}

void Notification::AppInitDone(void)
{
    LoadUISetting();
}

Notification & Notification::GetInstance()
{
    if (s_instance == nullptr)
    {
        s_instance = std::make_unique<Notification>();
    }
    return *s_instance;
}

void Notification::CleanUp()
{
    s_instance.reset();
}