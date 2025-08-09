#include "video_manager.h"
#include "video_settings.h"
#include <memory>
#include <stdio.h>
#include <yuzu_common/logging/backend.h>

std::unique_ptr<VideoManager> g_videoManager;
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
    info->version = MODULE_VIDEO_SPECS_VERSION;
    info->type = MODULE_TYPE_VIDEO;
#ifdef _DEBUG
    sprintf(info->name, "NxEmu Video Plugin (Debug)");
#else
    sprintf(info->name, "NxEmu Video Plugin");
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
    if (g_videoManager)
    {
        g_videoManager->EmulationStarting();
    }
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
    SaveVideoSettings();
}

IVideo * CALL CreateVideo(IRenderWindow & renderWindow, ISwitchSystem & system)
{
    if (g_notify == nullptr)
    {
        return nullptr;
    }
    if (g_videoManager.get() != nullptr)
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
        return nullptr;
    }
    g_videoManager = std::make_unique<VideoManager>(renderWindow, system);
    return g_videoManager.get();
}

void CALL DestroyVideo(IVideo * video)
{
    if (video == nullptr || g_videoManager.get() != video)
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
        return;
    }
    g_videoManager = nullptr;
}

extern "C" int __stdcall DllMain(void * /*hinst*/, unsigned long /*fdwReason*/, void * /*lpReserved*/)
{
    return true;
}
