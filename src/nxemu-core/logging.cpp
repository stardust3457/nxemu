#include "logging.h"
#include "settings/core_settings.h"
#include <yuzu_common/logging/backend.h>

extern "C" int __stdcall AllocConsole();

void LoggingSetup(void)
{
    Common::Log::Initialize(nullptr);
    Common::Log::Start();
    Common::Log::SetColorConsoleBackendEnabled(coreSettings.showConsole);
    if (coreSettings.showConsole)
    {
        if (AllocConsole())
        {
            FILE* fp;
            freopen_s(&fp, "CONOUT$", "w", stdout);
            freopen_s(&fp, "CONOUT$", "w", stderr);
            freopen_s(&fp, "CONIN$", "r", stdin);
        }
    }
}

void LoggingShutdown(void)
{
    Common::Log::Stop();
}