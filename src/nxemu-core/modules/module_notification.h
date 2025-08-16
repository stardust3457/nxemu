#pragma once

#include "module_base.h"

class ModuleNotification :
    public IModuleNotification
{
public:

    //IModuleNotification
    void DisplayError(const char * message, const char * title) override;
    void BreakPoint(const char * fileName, uint32_t lineNumber) override;
};