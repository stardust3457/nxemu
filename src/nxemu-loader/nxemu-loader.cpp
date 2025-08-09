#include "system_loader.h"
#include <memory>
#include <stdio.h>
#include <yuzu_common/logging/backend.h>

std::unique_ptr<Systemloader> g_loaderManager;
IModuleNotification * g_notify = nullptr;
IModuleSettings * g_settings = nullptr;

/*
Function: GetModuleInfo
Purpose: Fills the MODULE_INFO structure with information about the DLL.
Input: A pointer to a MODULE_INFO structure to be populated.
Output: none
*/
void CALL GetModuleInfo(MODULE_INFO * info)
{
    info->version = MODULE_LOADER_SPECS_VERSION;
    info->type = MODULE_TYPE_LOADER;
#ifdef _DEBUG
    sprintf(info->name, "NxEmu Loader Module (Debug)");
#else
    sprintf(info->name, "NxEmu Loader Module");
#endif
}

/*
Function: ModuleInitialize
Purpose: Initializes the module for global use.
Input: None
Output: Returns 0 on success
*/
int CALL ModuleInitialize(ModuleInterfaces & interfaces)
{
    g_notify = interfaces.notification;
    g_settings = interfaces.settings;

    if (g_notify == nullptr || g_settings == nullptr || interfaces.logger == nullptr)
    {
        return -1;
    }
    Common::Log::Initialize(interfaces.logger);
    return 0;
}

/*
Function: ModuleCleanup
Purpose: Cleans up global resources used by the module.
Input: None
Output: None
*/
void CALL ModuleCleanup()
{
}

/*
Function: EmulationStarting
Purpose: Called when emulation is starting
Input: None.
Output: None.
*/
void CALL EmulationStarting()
{
}

/*
Function: EmulationStopping
Purpose: Called when emulation is stopping
Input: None
Output: None
*/
void CALL EmulationStopping()
{
}

/*
Function: FlushSettings
Purpose: Called when emulation is saving settings
Input: None
Output: None
*/
EXPORT void CALL FlushSettings()
{
}

ISystemloader * CALL CreateSystemLoader(ISwitchSystem & System)
{
    if (g_loaderManager.get() != nullptr)
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
        return nullptr;
    }
    g_loaderManager = std::make_unique<Systemloader>(System);
    return g_loaderManager.get();
}

void CALL DestroySystemLoader(ISystemloader * systemloader)
{
    if (systemloader == nullptr || g_loaderManager.get() != systemloader)
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
        return;
    }
    g_loaderManager = nullptr;
}

extern "C" int __stdcall DllMain(void * /*hinst*/, unsigned long /*fdwReason*/, void * /*lpReserved*/)
{
    return true;
}
