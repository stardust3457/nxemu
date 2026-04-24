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

    // System
    Settings::SwitchableSetting<Settings::Language, true> language_index{linkage, Settings::Language::EnglishAmerican, Settings::Language::Japanese, Settings::Language::PortugueseBrazilian, "language_index", Settings::Category::System};
    Settings::SwitchableSetting<Settings::Region, true> region_index{linkage, Settings::Region::Usa, Settings::Region::Japan, Settings::Region::Taiwan, "region_index", Settings::Category::System};
    Settings::SwitchableSetting<Settings::TimeZone, true> time_zone_index{linkage, Settings::TimeZone::Auto, Settings::TimeZone::Auto, Settings::TimeZone::Zulu, "time_zone_index", Settings::Category::System};

    // Measured in seconds since epoch
    Settings::SwitchableSetting<bool> custom_rtc_enabled{linkage, false, "custom_rtc_enabled", Settings::Category::System, Settings::Specialization::Paired, true, true};
    Settings::SwitchableSetting<s64> custom_rtc{linkage, 0, "custom_rtc", Settings::Category::System, Settings::Specialization::Time, false, true, &custom_rtc_enabled};
    Settings::SwitchableSetting<s64, true> custom_rtc_offset{linkage, 0, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), "custom_rtc_offset", Settings::Category::System, Settings::Specialization::Countable, true, true};
    Settings::SwitchableSetting<bool> rng_seed_enabled{linkage, false, "rng_seed_enabled", Settings::Category::System, Settings::Specialization::Paired, true, true};
    Settings::SwitchableSetting<u32> rng_seed{linkage, 0, "rng_seed", Settings::Category::System, Settings::Specialization::Hex, true, true, &rng_seed_enabled};
    Settings::Setting<std::string> device_name{linkage, "NxEmu", "device_name", Settings::Category::System, Settings::Specialization::Default, true, true};

    Settings::Setting<s32> current_user{linkage, 0, "current_user", Settings::Category::System};

#ifdef ANDROID
    SwitchableSetting<ConsoleMode> use_docked_mode{linkage, ConsoleMode::Handheld, "use_docked_mode", Category::System, Specialization::Radio, true, true};
#else
    Settings::SwitchableSetting<Settings::ConsoleMode> use_docked_mode{linkage, Settings::ConsoleMode::Docked, "use_docked_mode", Settings::Category::System, Settings::Specialization::Radio, true, true};
#endif

    // Linux
    Settings::SwitchableSetting<bool> enable_gamemode{linkage, true, "enable_gamemode", Settings::Category::Linux};

    // Controls
    Settings::InputSetting<std::array<InputSettings::PlayerInput, 10>> players;
    // Only read/write enable_raw_input on Windows platforms
#ifdef _WIN32
    Settings::Setting<bool> enable_raw_input{linkage, false, "enable_raw_input", Settings::Category::Controls, Settings::Specialization::Default, true};
#else
    Settings::Setting<bool> enable_raw_input{linkage, false, "enable_raw_input", Settings::Category::Controls, Settings::Specialization::Default, false};
#endif

    Settings::Setting<bool> controller_navigation{linkage, true, "controller_navigation", Settings::Category::Controls};
    Settings::Setting<bool> enable_joycon_driver{linkage, true, "enable_joycon_driver", Settings::Category::Controls};
    Settings::Setting<bool> enable_procon_driver{linkage, false, "enable_procon_driver", Settings::Category::Controls};

    Settings::SwitchableSetting<bool> vibration_enabled{linkage, true, "vibration_enabled", Settings::Category::Controls};
    Settings::SwitchableSetting<bool> enable_accurate_vibrations{linkage, false, "enable_accurate_vibrations", Settings::Category::Controls};

    Settings::SwitchableSetting<bool> motion_enabled{linkage, true, "motion_enabled", Settings::Category::Controls};
    Settings::Setting<std::string> udp_input_servers{linkage, "127.0.0.1:26760", "udp_input_servers", Settings::Category::Controls};
    Settings::Setting<bool> enable_udp_controller{linkage, false, "enable_udp_controller", Settings::Category::Controls};

    Settings::Setting<bool> pause_tas_on_load{linkage, true, "pause_tas_on_load", Settings::Category::Controls};
    Settings::Setting<bool> tas_enable{linkage, false, "tas_enable", Settings::Category::Controls};
    Settings::Setting<bool> tas_loop{linkage, false, "tas_loop", Settings::Category::Controls};

    Settings::Setting<bool> mouse_panning{linkage, false, "mouse_panning", Settings::Category::Controls, Settings::Specialization::Default, false};
    Settings::Setting<u8, true> mouse_panning_sensitivity{linkage, 50, 1, 100, "mouse_panning_sensitivity", Settings::Category::Controls};
    Settings::Setting<bool> mouse_enabled{linkage, false, "mouse_enabled", Settings::Category::Controls};

    Settings::Setting<u8, true> mouse_panning_x_sensitivity{linkage, 50, 1, 100, "mouse_panning_x_sensitivity", Settings::Category::Controls};
    Settings::Setting<u8, true> mouse_panning_y_sensitivity{linkage, 50, 1, 100, "mouse_panning_y_sensitivity", Settings::Category::Controls};
    Settings::Setting<u8, true> mouse_panning_deadzone_counterweight{linkage, 20, 0, 100, "mouse_panning_deadzone_counterweight", Settings::Category::Controls};
    Settings::Setting<u8, true> mouse_panning_decay_strength{linkage, 18, 0, 100, "mouse_panning_decay_strength", Settings::Category::Controls};
    Settings::Setting<u8, true> mouse_panning_min_decay{linkage, 6, 0, 100, "mouse_panning_min_decay", Settings::Category::Controls};

    Settings::Setting<bool> emulate_analog_keyboard{linkage, false, "emulate_analog_keyboard", Settings::Category::Controls};
    Settings::Setting<bool> keyboard_enabled{linkage, false, "keyboard_enabled", Settings::Category::Controls};

    Settings::Setting<bool> debug_pad_enabled{linkage, false, "debug_pad_enabled", Settings::Category::Controls};
    InputSettings::ButtonsRaw debug_pad_buttons;
    InputSettings::AnalogsRaw debug_pad_analogs;

    InputSettings::TouchscreenInput touchscreen;

    Settings::Setting<std::string> touch_device{linkage, "min_x:100,min_y:50,max_x:1800,max_y:850", "touch_device", Settings::Category::Controls};
    Settings::Setting<int> touch_from_button_map_index{linkage, 0, "touch_from_button_map", Settings::Category::Controls};
    std::vector<Settings::TouchFromButtonMap> touch_from_button_maps;

    Settings::Setting<bool> enable_ring_controller{linkage, true, "enable_ring_controller", Settings::Category::Controls};
    InputSettings::RingconRaw ringcon_analogs;

    Settings::Setting<bool> enable_ir_sensor{linkage, false, "enable_ir_sensor", Settings::Category::Controls};
    Settings::Setting<std::string> ir_sensor_device{linkage, "auto", "ir_sensor_device", Settings::Category::Controls};

    Settings::Setting<bool> random_amiibo_id{linkage, false, "random_amiibo_id", Settings::Category::Controls};
};

extern OSSettings osSettings;

void SetupOsSetting(void);
void SaveOsSettings(void);
