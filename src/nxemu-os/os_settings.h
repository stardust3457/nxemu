#pragma once
#include "yuzu_common/settings.h"

struct OSSettings
{
    Settings::Linkage linkage{};

    // Applet
    Settings::Setting<Settings::AppletMode> cabinet_applet_mode{linkage, Settings::AppletMode::LLE, "cabinet_applet_mode", Settings::Category::LibraryApplet};
    Settings::Setting<Settings::AppletMode> controller_applet_mode{linkage, Settings::AppletMode::HLE, "controller_applet_mode", Settings::Category::LibraryApplet};
    Settings::Setting<Settings::AppletMode> data_erase_applet_mode{linkage, Settings::AppletMode::HLE, "data_erase_applet_mode", Settings::Category::LibraryApplet};
    Settings::Setting<Settings::AppletMode> error_applet_mode{linkage, Settings::AppletMode::LLE, "error_applet_mode", Settings::Category::LibraryApplet};
    Settings::Setting<Settings::AppletMode> net_connect_applet_mode{linkage, Settings::AppletMode::HLE, "net_connect_applet_mode", Settings::Category::LibraryApplet};
    Settings::Setting<Settings::AppletMode> player_select_applet_mode{linkage, Settings::AppletMode::HLE, "player_select_applet_mode", Settings::Category::LibraryApplet};
    Settings::Setting<Settings::AppletMode> swkbd_applet_mode{linkage, Settings::AppletMode::LLE, "swkbd_applet_mode", Settings::Category::LibraryApplet};
    Settings::Setting<Settings::AppletMode> mii_edit_applet_mode{linkage, Settings::AppletMode::LLE, "mii_edit_applet_mode", Settings::Category::LibraryApplet};
    Settings::Setting<Settings::AppletMode> web_applet_mode{linkage, Settings::AppletMode::HLE, "web_applet_mode", Settings::Category::LibraryApplet};
    Settings::Setting<Settings::AppletMode> shop_applet_mode{linkage, Settings::AppletMode::HLE, "shop_applet_mode", Settings::Category::LibraryApplet};
    Settings::Setting<Settings::AppletMode> photo_viewer_applet_mode{linkage, Settings::AppletMode::LLE, "photo_viewer_applet_mode", Settings::Category::LibraryApplet};
    Settings::Setting<Settings::AppletMode> offline_web_applet_mode{linkage, Settings::AppletMode::LLE, "offline_web_applet_mode", Settings::Category::LibraryApplet};
    Settings::Setting<Settings::AppletMode> login_share_applet_mode{linkage, Settings::AppletMode::HLE, "login_share_applet_mode", Settings::Category::LibraryApplet};
    Settings::Setting<Settings::AppletMode> wifi_web_auth_applet_mode{linkage, Settings::AppletMode::HLE, "wifi_web_auth_applet_mode", Settings::Category::LibraryApplet};
    Settings::Setting<Settings::AppletMode> my_page_applet_mode{linkage, Settings::AppletMode::LLE, "my_page_applet_mode", Settings::Category::LibraryApplet};

    // Audio
    Settings::SwitchableSetting<Settings::AudioEngine> sink_id{linkage, Settings::AudioEngine::Auto, "output_engine", Settings::Category::Audio, Settings::Specialization::RuntimeList};
    Settings::SwitchableSetting<std::string> audio_output_device_id{linkage, "auto", "output_device", Settings::Category::Audio, Settings::Specialization::RuntimeList};
    Settings::SwitchableSetting<std::string> audio_input_device_id{linkage, "auto", "input_device", Settings::Category::Audio, Settings::Specialization::RuntimeList};
    Settings::SwitchableSetting<Settings::AudioMode, true> sound_index{linkage, Settings::AudioMode::Stereo, Settings::AudioMode::Mono, Settings::AudioMode::Surround, "sound_index", Settings::Category::SystemAudio, Settings::Specialization::Default, true, true};
    Settings::SwitchableSetting<u8, true> volume{linkage, 100, 0, 200, "volume", Settings::Category::Audio, Settings::Specialization::Scalar | Settings::Specialization::Percentage, true, true};
    Settings::Setting<bool, false> audio_muted{linkage, false, "audio_muted", Settings::Category::Audio, Settings::Specialization::Default, true, true};
    Settings::Setting<bool, false> dump_audio_commands{linkage, false, "dump_audio_commands", Settings::Category::Audio, Settings::Specialization::Default, false};

#ifdef ANDROID
    SwitchableSetting<ConsoleMode> use_docked_mode{linkage, ConsoleMode::Handheld, "use_docked_mode", Category::System, Specialization::Radio, true, true};
#else
    Settings::SwitchableSetting<Settings::ConsoleMode> use_docked_mode{linkage, Settings::ConsoleMode::Docked, "use_docked_mode", Settings::Category::System, Settings::Specialization::Radio, true, true};
#endif
};

extern OSSettings osSettings;

void SetupOsSetting(void);
void SaveOsSettings(void);
