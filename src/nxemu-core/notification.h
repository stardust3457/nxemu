#pragma once
#include <stdint.h>

__interface INotification
{
    //Error Messages
    virtual void DisplayError(const char * message, const char * title) const = 0;
    virtual void BreakPoint(const char * fileName, uint32_t lineNumber) = 0;

    virtual void AppInitDone(void) = 0;
};

extern INotification * g_notify;
