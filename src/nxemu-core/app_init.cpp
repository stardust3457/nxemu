#include "app_init.h"
#include "logging.h"
#include "notification.h"
#include "settings/core_settings.h"
#include "settings/settings.h"

bool AppInit(INotification * notification)
{
    g_notify = notification;
    if (!SettingsStore::GetInstance().Initialize())
    {
        return false;
    }
    SetupCoreSetting();
    LoggingSetup();

    if (notification)
    {
        notification->AppInitDone();
    }
    return true;
}

void AppCleanup(void)
{
    LoggingShutdown();
    SettingsStore::CleanUp();
}
