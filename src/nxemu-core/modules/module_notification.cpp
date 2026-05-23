#include "module_notification.h"
#include "notification.h"

void ModuleNotification::DisplayError(const char * message, const char * title)
{
    if (g_notify != nullptr)
    {
        g_notify->DisplayError(message, title);
    }
}

NotificationResponse ModuleNotification::Query(const char * message, const char * title)
{
    if (g_notify != nullptr)
    {
        return g_notify->Query(message, title);
    }
    return NotificationResponse::No;
}

void ModuleNotification::BreakPoint(const char * fileName, uint32_t lineNumber)
{
    if (g_notify != nullptr)
    {
        g_notify->BreakPoint(fileName, lineNumber);
    }
}
