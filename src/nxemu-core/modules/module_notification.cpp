#include "module_notification.h"
#include "notification.h"

void ModuleNotification::DisplayError(const char * message, const char * title)
{
    g_notify->DisplayError(message, title);
}

void ModuleNotification::BreakPoint(const char * fileName, uint32_t lineNumber)
{
    g_notify->BreakPoint(fileName, lineNumber);
}
