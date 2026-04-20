#pragma once
#include "yuzu_common/settings.h"

struct OSSettings
{
    Settings::Linkage linkage{};

#ifdef ANDROID
    SwitchableSetting<ConsoleMode> use_docked_mode{linkage, ConsoleMode::Handheld, "use_docked_mode", Category::System, Specialization::Radio, true, true};
#else
    Settings::SwitchableSetting<Settings::ConsoleMode> use_docked_mode{linkage, Settings::ConsoleMode::Docked, "use_docked_mode", Settings::Category::System, Settings::Specialization::Radio, true, true};
#endif
};

extern OSSettings osSettings;

void SetupOsSetting(void);
void SaveOsSettings(void);
