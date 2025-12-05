#include "os_settings.h"
#include "os_settings_identifiers.h"
#include <common/json.h>
#include <common/std_string.h>
#include <nxemu-module-spec/base.h>
#include <yuzu_common/settings_enums.h>
#include <yuzu_common/settings.h>
#include <yuzu_common/yuzu_assert.h>

extern IModuleSettings * g_settings;

namespace
{
    enum class SettingType { StringSetting, StringValue, AudioEngine, AudioMode, U8, BooleanSetting, BooleanValue, UnsignedInt, ControllerType };

    class OsSetting
    {
    public:
        OsSetting(const char * id, const char * path, Settings::SwitchableSetting<std::string> * val);
        OsSetting(const char * id, const char * path, Settings::SwitchableSetting<Settings::AudioEngine> * val);
        OsSetting(const char * id, const char * path, Settings::SwitchableSetting<Settings::AudioMode, true>* val);
        OsSetting(const char * id, const char * path, Settings::SwitchableSetting<u8, true> * val);
        OsSetting(const char * id, const char * path, Settings::Setting<bool, false> * val);
        OsSetting(const char * id, const char * path, bool * val, bool defaultValue);
        OsSetting(const char * id, const char * path, int * val, int defaultValue);
        OsSetting(const char * id, const char * path, uint32_t * val, uint32_t defaultValue);
        OsSetting(const char * id, const char * path, std::string * val, const char * defaultValue);
        OsSetting(const char * id, const char * path, InputSettings::ControllerType * val, InputSettings::ControllerType defaultValue);

        const char * identifier;
        const char * json_path;
        SettingType settingType;
        union
        {
            Settings::SwitchableSetting<std::string> * stringSetting;
            Settings::SwitchableSetting<Settings::AudioEngine> * audioEngine;
            Settings::SwitchableSetting<Settings::AudioMode, true> * audioMode;
            Settings::SwitchableSetting<u8, true> * u8;
            Settings::Setting<bool, false> * booleanSetting;
            bool * boolValue;
            uint32_t * uint32Value;
            std::string * stringValue;
            InputSettings::ControllerType * controllerType;
        } setting;
        union
        {
            bool boolValue;
            uint32_t uint32Value;
            const char * sringValue;
            InputSettings::ControllerType controllerType;
        } defaultValue;
    };

    static OsSetting settings[] = {
        { nullptr, "controller\\player_0\\Connected", &Settings::values.players.GetValue(true)[0].connected, true },
        { nullptr, "controller\\player_0\\ControllerType", &Settings::values.players.GetValue(true)[0].controller_type, InputSettings::ControllerType::ProController },
        { nullptr, "controller\\player_0\\Button\\A", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::A], "engine:keyboard,code:67,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\B", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::B], "engine:keyboard,code:88,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\X", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::X], "engine:keyboard,code:86,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\Y", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::Y], "engine:keyboard,code:90,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\LStick", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::LStick], "engine:keyboard,code:70,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\RStick", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::RStick], "engine:keyboard,code:71,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\L", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::L], "engine:keyboard,code:81,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\R", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::R], "engine:keyboard,code:69,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\ZL", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::ZL], "engine:keyboard,code:82,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\ZR", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::ZR], "engine:keyboard,code:84,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\Plus", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::Plus], "engine:keyboard,code:77,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\Minus", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::Minus], "engine:keyboard,code:78,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\DLeft", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::DLeft], "engine:keyboard,code:37,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\DUp", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::DUp], "engine:keyboard,code:38,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\DRight", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::DRight], "engine:keyboard,code:39,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\DDown", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::DDown], "engine:keyboard,code:40,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\SLLeft", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::SLLeft], "engine:keyboard,code:81,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\SRLeft", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::SRLeft], "engine:keyboard,code:69,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\Home", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::Home], "engine:keyboard,code:0,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\Screenshot", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::Screenshot], "engine:keyboard,code:0,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\SLRight", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::SLRight], "engine:keyboard,code:81,toggle:0" },
        { nullptr, "controller\\player_0\\Button\\SRRight", &Settings::values.players.GetValue(true)[0].buttons[(size_t)NativeButtonValues::SRRight], "engine:keyboard,code:69,toggle:0" },
        { nullptr, "controller\\player_0\\Analog\\LStick", &Settings::values.players.GetValue(true)[0].analogs[(size_t)NativeAnalogValues::LStick], "engine:analog_from_button,up:engine$0keyboard$1code$087$1toggle$00,left:engine$0keyboard$1code$065$1toggle$00,modifier:engine$0keyboard$1code$016$1toggle$00,down:engine$0keyboard$1code$083$1toggle$00,right:engine$0keyboard$1code$068$1toggle$00" },
        { nullptr, "controller\\player_0\\Analog\\RStick", &Settings::values.players.GetValue(true)[0].analogs[(size_t)NativeAnalogValues::RStick], "engine:analog_from_button,up:engine$0keyboard$1code$073$1toggle$00,left:engine$0keyboard$1code$074$1toggle$00,modifier:engine$0keyboard$1code$00$1toggle$00,down:engine$0keyboard$1code$075$1toggle$00,right:engine$0keyboard$1code$076$1toggle$00" },
        { nullptr, "controller\\player_0\\Motion\\Left", &Settings::values.players.GetValue(true)[0].motions[(size_t)NativeMotionValues::MotionLeft], "engine:keyboard,code:55,toggle:0" },
        { nullptr, "controller\\player_0\\Motion\\Right", &Settings::values.players.GetValue(true)[0].motions[(size_t)NativeMotionValues::MotionRight], "engine:keyboard,code:56,toggle:0" },
        { nullptr, "controller\\player_0\\Vibration\\Enabled", &Settings::values.players.GetValue(true)[0].vibration_enabled, false },
        { nullptr, "controller\\player_0\\Vibration\\Strength", &Settings::values.players.GetValue(true)[0].vibration_strength, 0 },
        { nullptr, "controller\\player_0\\BodyColor\\Left", &Settings::values.players.GetValue(true)[0].body_color_left, 0 },
        { nullptr, "controller\\player_0\\BodyColor\\Right", &Settings::values.players.GetValue(true)[0].body_color_right, 0 },
        { nullptr, "controller\\player_0\\ButtonColor\\Left", &Settings::values.players.GetValue(true)[0].button_color_left, 0 },
        { nullptr, "controller\\player_0\\ButtonColor\\Right", &Settings::values.players.GetValue(true)[0].button_color_right, 0 },
        { nullptr, "controller\\player_0\\ProfileName", &Settings::values.players.GetValue(true)[0].profile_name, "" },
        { nullptr, "controller\\player_0\\Vibration\\UseSystem", &Settings::values.players.GetValue(true)[0].use_system_vibrator, false },

        { nullptr, "controller\\player_1\\Connected", &Settings::values.players.GetValue(true)[1].connected, false },
        { nullptr, "controller\\player_1\\ControllerType", &Settings::values.players.GetValue(true)[1].controller_type, InputSettings::ControllerType::ProController },
        { nullptr, "controller\\player_1\\Button\\A", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::A], "" },
        { nullptr, "controller\\player_1\\Button\\B", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::B], "" },
        { nullptr, "controller\\player_1\\Button\\X", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::X], "" },
        { nullptr, "controller\\player_1\\Button\\Y", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::Y], "" },
        { nullptr, "controller\\player_1\\Button\\LStick", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::LStick], "" },
        { nullptr, "controller\\player_1\\Button\\RStick", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::RStick], "" },
        { nullptr, "controller\\player_1\\Button\\L", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::L], "" },
        { nullptr, "controller\\player_1\\Button\\R", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::R], "" },
        { nullptr, "controller\\player_1\\Button\\ZL", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::ZL], "" },
        { nullptr, "controller\\player_1\\Button\\ZR", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::ZR], "" },
        { nullptr, "controller\\player_1\\Button\\Plus", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::Plus], "" },
        { nullptr, "controller\\player_1\\Button\\Minus", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::Minus], "" },
        { nullptr, "controller\\player_1\\Button\\DLeft", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::DLeft], "" },
        { nullptr, "controller\\player_1\\Button\\DUp", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::DUp], "" },
        { nullptr, "controller\\player_1\\Button\\DRight", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::DRight], "" },
        { nullptr, "controller\\player_1\\Button\\DDown", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::DDown], "" },
        { nullptr, "controller\\player_1\\Button\\SLLeft", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::SLLeft], "" },
        { nullptr, "controller\\player_1\\Button\\SRLeft", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::SRLeft], "" },
        { nullptr, "controller\\player_1\\Button\\Home", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::Home], "" },
        { nullptr, "controller\\player_1\\Button\\Screenshot", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::Screenshot], "" },
        { nullptr, "controller\\player_1\\Button\\SLRight", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::SLRight], "" },
        { nullptr, "controller\\player_1\\Button\\SRRight", &Settings::values.players.GetValue(true)[1].buttons[(size_t)NativeButtonValues::SRRight], "" },
        { nullptr, "controller\\player_1\\Analog\\LStick", &Settings::values.players.GetValue(true)[1].analogs[(size_t)NativeAnalogValues::LStick], "" },
        { nullptr, "controller\\player_1\\Analog\\RStick", &Settings::values.players.GetValue(true)[1].analogs[(size_t)NativeAnalogValues::RStick], "" },
        { nullptr, "controller\\player_1\\Motion\\Left", &Settings::values.players.GetValue(true)[1].motions[(size_t)NativeMotionValues::MotionLeft], "" },
        { nullptr, "controller\\player_1\\Motion\\Right", &Settings::values.players.GetValue(true)[1].motions[(size_t)NativeMotionValues::MotionRight], "" },
        { nullptr, "controller\\player_1\\Vibration\\Enabled", &Settings::values.players.GetValue(true)[1].vibration_enabled, false },
        { nullptr, "controller\\player_1\\Vibration\\Strength", &Settings::values.players.GetValue(true)[1].vibration_strength, 0 },
        { nullptr, "controller\\player_1\\BodyColor\\Left", &Settings::values.players.GetValue(true)[1].body_color_left, 0 },
        { nullptr, "controller\\player_1\\BodyColor\\Right", &Settings::values.players.GetValue(true)[1].body_color_right, 0 },
        { nullptr, "controller\\player_1\\ButtonColor\\Left", &Settings::values.players.GetValue(true)[1].button_color_left, 0 },
        { nullptr, "controller\\player_1\\ButtonColor\\Right", &Settings::values.players.GetValue(true)[1].button_color_right, 0 },
        { nullptr, "controller\\player_1\\ProfileName", &Settings::values.players.GetValue(true)[1].profile_name, "" },
        { nullptr, "controller\\player_1\\Vibration\\UseSystem", &Settings::values.players.GetValue(true)[1].use_system_vibrator, false },

        {nullptr, "controller\\player_2\\Connected", &Settings::values.players.GetValue(true)[2].connected, false},
        {nullptr, "controller\\player_2\\ControllerType", &Settings::values.players.GetValue(true)[2].controller_type, InputSettings::ControllerType::ProController},
        {nullptr, "controller\\player_2\\Button\\A", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::A], ""},
        {nullptr, "controller\\player_2\\Button\\B", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::B], ""},
        {nullptr, "controller\\player_2\\Button\\X", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::X], ""},
        {nullptr, "controller\\player_2\\Button\\Y", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::Y], ""},
        {nullptr, "controller\\player_2\\Button\\LStick", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::LStick], ""},
        {nullptr, "controller\\player_2\\Button\\RStick", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::RStick], ""},
        {nullptr, "controller\\player_2\\Button\\L", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::L], ""},
        {nullptr, "controller\\player_2\\Button\\R", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::R], ""},
        {nullptr, "controller\\player_2\\Button\\ZL", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::ZL], ""},
        {nullptr, "controller\\player_2\\Button\\ZR", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::ZR], ""},
        {nullptr, "controller\\player_2\\Button\\Plus", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::Plus], ""},
        {nullptr, "controller\\player_2\\Button\\Minus", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::Minus], ""},
        {nullptr, "controller\\player_2\\Button\\DLeft", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::DLeft], ""},
        {nullptr, "controller\\player_2\\Button\\DUp", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::DUp], ""},
        {nullptr, "controller\\player_2\\Button\\DRight", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::DRight], ""},
        {nullptr, "controller\\player_2\\Button\\DDown", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::DDown], ""},
        {nullptr, "controller\\player_2\\Button\\SLLeft", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::SLLeft], ""},
        {nullptr, "controller\\player_2\\Button\\SRLeft", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::SRLeft], ""},
        {nullptr, "controller\\player_2\\Button\\Home", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::Home], ""},
        {nullptr, "controller\\player_2\\Button\\Screenshot", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::Screenshot], ""},
        {nullptr, "controller\\player_2\\Button\\SLRight", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::SLRight], ""},
        {nullptr, "controller\\player_2\\Button\\SRRight", &Settings::values.players.GetValue(true)[2].buttons[(size_t)NativeButtonValues::SRRight], ""},
        {nullptr, "controller\\player_2\\Analog\\LStick", &Settings::values.players.GetValue(true)[2].analogs[(size_t)NativeAnalogValues::LStick], ""},
        {nullptr, "controller\\player_2\\Analog\\RStick", &Settings::values.players.GetValue(true)[2].analogs[(size_t)NativeAnalogValues::RStick], ""},
        {nullptr, "controller\\player_2\\Motion\\Left", &Settings::values.players.GetValue(true)[2].motions[(size_t)NativeMotionValues::MotionLeft], ""},
        {nullptr, "controller\\player_2\\Motion\\Right", &Settings::values.players.GetValue(true)[2].motions[(size_t)NativeMotionValues::MotionRight], ""},
        {nullptr, "controller\\player_2\\Vibration\\Enabled", &Settings::values.players.GetValue(true)[2].vibration_enabled, false},
        {nullptr, "controller\\player_2\\Vibration\\Strength", &Settings::values.players.GetValue(true)[2].vibration_strength, 0},
        {nullptr, "controller\\player_2\\BodyColor\\Left", &Settings::values.players.GetValue(true)[2].body_color_left, 0},
        {nullptr, "controller\\player_2\\BodyColor\\Right", &Settings::values.players.GetValue(true)[2].body_color_right, 0},
        {nullptr, "controller\\player_2\\ButtonColor\\Left", &Settings::values.players.GetValue(true)[2].button_color_left, 0},
        {nullptr, "controller\\player_2\\ButtonColor\\Right", &Settings::values.players.GetValue(true)[2].button_color_right, 0},
        {nullptr, "controller\\player_2\\ProfileName", &Settings::values.players.GetValue(true)[2].profile_name, ""},
        {nullptr, "controller\\player_2\\Vibration\\UseSystem", &Settings::values.players.GetValue(true)[2].use_system_vibrator, false},

        {nullptr, "controller\\player_3\\Connected", &Settings::values.players.GetValue(true)[3].connected, false},
        {nullptr, "controller\\player_3\\ControllerType", &Settings::values.players.GetValue(true)[3].controller_type, InputSettings::ControllerType::ProController},
        {nullptr, "controller\\player_3\\Button\\A", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::A], ""},
        {nullptr, "controller\\player_3\\Button\\B", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::B], ""},
        {nullptr, "controller\\player_3\\Button\\X", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::X], ""},
        {nullptr, "controller\\player_3\\Button\\Y", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::Y], ""},
        {nullptr, "controller\\player_3\\Button\\LStick", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::LStick], ""},
        {nullptr, "controller\\player_3\\Button\\RStick", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::RStick], ""},
        {nullptr, "controller\\player_3\\Button\\L", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::L], ""},
        {nullptr, "controller\\player_3\\Button\\R", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::R], ""},
        {nullptr, "controller\\player_3\\Button\\ZL", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::ZL], ""},
        {nullptr, "controller\\player_3\\Button\\ZR", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::ZR], ""},
        {nullptr, "controller\\player_3\\Button\\Plus", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::Plus], ""},
        {nullptr, "controller\\player_3\\Button\\Minus", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::Minus], ""},
        {nullptr, "controller\\player_3\\Button\\DLeft", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::DLeft], ""},
        {nullptr, "controller\\player_3\\Button\\DUp", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::DUp], ""},
        {nullptr, "controller\\player_3\\Button\\DRight", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::DRight], ""},
        {nullptr, "controller\\player_3\\Button\\DDown", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::DDown], ""},
        {nullptr, "controller\\player_3\\Button\\SLLeft", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::SLLeft], ""},
        {nullptr, "controller\\player_3\\Button\\SRLeft", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::SRLeft], ""},
        {nullptr, "controller\\player_3\\Button\\Home", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::Home], ""},
        {nullptr, "controller\\player_3\\Button\\Screenshot", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::Screenshot], ""},
        {nullptr, "controller\\player_3\\Button\\SLRight", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::SLRight], ""},
        {nullptr, "controller\\player_3\\Button\\SRRight", &Settings::values.players.GetValue(true)[3].buttons[(size_t)NativeButtonValues::SRRight], ""},
        {nullptr, "controller\\player_3\\Analog\\LStick", &Settings::values.players.GetValue(true)[3].analogs[(size_t)NativeAnalogValues::LStick], ""},
        {nullptr, "controller\\player_3\\Analog\\RStick", &Settings::values.players.GetValue(true)[3].analogs[(size_t)NativeAnalogValues::RStick], ""},
        {nullptr, "controller\\player_3\\Motion\\Left", &Settings::values.players.GetValue(true)[3].motions[(size_t)NativeMotionValues::MotionLeft], ""},
        {nullptr, "controller\\player_3\\Motion\\Right", &Settings::values.players.GetValue(true)[3].motions[(size_t)NativeMotionValues::MotionRight], ""},
        {nullptr, "controller\\player_3\\Vibration\\Enabled", &Settings::values.players.GetValue(true)[3].vibration_enabled, false},
        {nullptr, "controller\\player_3\\Vibration\\Strength", &Settings::values.players.GetValue(true)[3].vibration_strength, 0},
        {nullptr, "controller\\player_3\\BodyColor\\Left", &Settings::values.players.GetValue(true)[3].body_color_left, 0},
        {nullptr, "controller\\player_3\\BodyColor\\Right", &Settings::values.players.GetValue(true)[3].body_color_right, 0},
        {nullptr, "controller\\player_3\\ButtonColor\\Left", &Settings::values.players.GetValue(true)[3].button_color_left, 0},
        {nullptr, "controller\\player_3\\ButtonColor\\Right", &Settings::values.players.GetValue(true)[3].button_color_right, 0},
        {nullptr, "controller\\player_3\\ProfileName", &Settings::values.players.GetValue(true)[3].profile_name, ""},
        {nullptr, "controller\\player_3\\Vibration\\UseSystem", &Settings::values.players.GetValue(true)[3].use_system_vibrator, false},

        {nullptr, "controller\\player_4\\Connected", &Settings::values.players.GetValue(true)[4].connected, false},
        {nullptr, "controller\\player_4\\ControllerType", &Settings::values.players.GetValue(true)[4].controller_type, InputSettings::ControllerType::ProController},
        {nullptr, "controller\\player_4\\Button\\A", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::A], ""},
        {nullptr, "controller\\player_4\\Button\\B", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::B], ""},
        {nullptr, "controller\\player_4\\Button\\X", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::X], ""},
        {nullptr, "controller\\player_4\\Button\\Y", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::Y], ""},
        {nullptr, "controller\\player_4\\Button\\LStick", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::LStick], ""},
        {nullptr, "controller\\player_4\\Button\\RStick", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::RStick], ""},
        {nullptr, "controller\\player_4\\Button\\L", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::L], ""},
        {nullptr, "controller\\player_4\\Button\\R", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::R], ""},
        {nullptr, "controller\\player_4\\Button\\ZL", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::ZL], ""},
        {nullptr, "controller\\player_4\\Button\\ZR", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::ZR], ""},
        {nullptr, "controller\\player_4\\Button\\Plus", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::Plus], ""},
        {nullptr, "controller\\player_4\\Button\\Minus", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::Minus], ""},
        {nullptr, "controller\\player_4\\Button\\DLeft", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::DLeft], ""},
        {nullptr, "controller\\player_4\\Button\\DUp", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::DUp], ""},
        {nullptr, "controller\\player_4\\Button\\DRight", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::DRight], ""},
        {nullptr, "controller\\player_4\\Button\\DDown", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::DDown], ""},
        {nullptr, "controller\\player_4\\Button\\SLLeft", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::SLLeft], ""},
        {nullptr, "controller\\player_4\\Button\\SRLeft", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::SRLeft], ""},
        {nullptr, "controller\\player_4\\Button\\Home", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::Home], ""},
        {nullptr, "controller\\player_4\\Button\\Screenshot", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::Screenshot], ""},
        {nullptr, "controller\\player_4\\Button\\SLRight", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::SLRight], ""},
        {nullptr, "controller\\player_4\\Button\\SRRight", &Settings::values.players.GetValue(true)[4].buttons[(size_t)NativeButtonValues::SRRight], ""},
        {nullptr, "controller\\player_4\\Analog\\LStick", &Settings::values.players.GetValue(true)[4].analogs[(size_t)NativeAnalogValues::LStick], ""},
        {nullptr, "controller\\player_4\\Analog\\RStick", &Settings::values.players.GetValue(true)[4].analogs[(size_t)NativeAnalogValues::RStick], ""},
        {nullptr, "controller\\player_4\\Motion\\Left", &Settings::values.players.GetValue(true)[4].motions[(size_t)NativeMotionValues::MotionLeft], ""},
        {nullptr, "controller\\player_4\\Motion\\Right", &Settings::values.players.GetValue(true)[4].motions[(size_t)NativeMotionValues::MotionRight], ""},
        {nullptr, "controller\\player_4\\Vibration\\Enabled", &Settings::values.players.GetValue(true)[4].vibration_enabled, false},
        {nullptr, "controller\\player_4\\Vibration\\Strength", &Settings::values.players.GetValue(true)[4].vibration_strength, 0},
        {nullptr, "controller\\player_4\\BodyColor\\Left", &Settings::values.players.GetValue(true)[4].body_color_left, 0},
        {nullptr, "controller\\player_4\\BodyColor\\Right", &Settings::values.players.GetValue(true)[4].body_color_right, 0},
        {nullptr, "controller\\player_4\\ButtonColor\\Left", &Settings::values.players.GetValue(true)[4].button_color_left, 0},
        {nullptr, "controller\\player_4\\ButtonColor\\Right", &Settings::values.players.GetValue(true)[4].button_color_right, 0},
        {nullptr, "controller\\player_4\\ProfileName", &Settings::values.players.GetValue(true)[4].profile_name, ""},
        {nullptr, "controller\\player_4\\Vibration\\UseSystem", &Settings::values.players.GetValue(true)[4].use_system_vibrator, false},

        {nullptr, "controller\\player_5\\Connected", &Settings::values.players.GetValue(true)[5].connected, false},
        {nullptr, "controller\\player_5\\ControllerType", &Settings::values.players.GetValue(true)[5].controller_type, InputSettings::ControllerType::ProController},
        {nullptr, "controller\\player_5\\Button\\A", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::A], ""},
        {nullptr, "controller\\player_5\\Button\\B", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::B], ""},
        {nullptr, "controller\\player_5\\Button\\X", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::X], ""},
        {nullptr, "controller\\player_5\\Button\\Y", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::Y], ""},
        {nullptr, "controller\\player_5\\Button\\LStick", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::LStick], ""},
        {nullptr, "controller\\player_5\\Button\\RStick", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::RStick], ""},
        {nullptr, "controller\\player_5\\Button\\L", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::L], ""},
        {nullptr, "controller\\player_5\\Button\\R", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::R], ""},
        {nullptr, "controller\\player_5\\Button\\ZL", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::ZL], ""},
        {nullptr, "controller\\player_5\\Button\\ZR", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::ZR], ""},
        {nullptr, "controller\\player_5\\Button\\Plus", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::Plus], ""},
        {nullptr, "controller\\player_5\\Button\\Minus", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::Minus], ""},
        {nullptr, "controller\\player_5\\Button\\DLeft", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::DLeft], ""},
        {nullptr, "controller\\player_5\\Button\\DUp", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::DUp], ""},
        {nullptr, "controller\\player_5\\Button\\DRight", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::DRight], ""},
        {nullptr, "controller\\player_5\\Button\\DDown", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::DDown], ""},
        {nullptr, "controller\\player_5\\Button\\SLLeft", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::SLLeft], ""},
        {nullptr, "controller\\player_5\\Button\\SRLeft", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::SRLeft], ""},
        {nullptr, "controller\\player_5\\Button\\Home", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::Home], ""},
        {nullptr, "controller\\player_5\\Button\\Screenshot", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::Screenshot], ""},
        {nullptr, "controller\\player_5\\Button\\SLRight", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::SLRight], ""},
        {nullptr, "controller\\player_5\\Button\\SRRight", &Settings::values.players.GetValue(true)[5].buttons[(size_t)NativeButtonValues::SRRight], ""},
        {nullptr, "controller\\player_5\\Analog\\LStick", &Settings::values.players.GetValue(true)[5].analogs[(size_t)NativeAnalogValues::LStick], ""},
        {nullptr, "controller\\player_5\\Analog\\RStick", &Settings::values.players.GetValue(true)[5].analogs[(size_t)NativeAnalogValues::RStick], ""},
        {nullptr, "controller\\player_5\\Motion\\Left", &Settings::values.players.GetValue(true)[5].motions[(size_t)NativeMotionValues::MotionLeft], ""},
        {nullptr, "controller\\player_5\\Motion\\Right", &Settings::values.players.GetValue(true)[5].motions[(size_t)NativeMotionValues::MotionRight], ""},
        {nullptr, "controller\\player_5\\Vibration\\Enabled", &Settings::values.players.GetValue(true)[5].vibration_enabled, false},
        {nullptr, "controller\\player_5\\Vibration\\Strength", &Settings::values.players.GetValue(true)[5].vibration_strength, 0},
        {nullptr, "controller\\player_5\\BodyColor\\Left", &Settings::values.players.GetValue(true)[5].body_color_left, 0},
        {nullptr, "controller\\player_5\\BodyColor\\Right", &Settings::values.players.GetValue(true)[5].body_color_right, 0},
        {nullptr, "controller\\player_5\\ButtonColor\\Left", &Settings::values.players.GetValue(true)[5].button_color_left, 0},
        {nullptr, "controller\\player_5\\ButtonColor\\Right", &Settings::values.players.GetValue(true)[5].button_color_right, 0},
        {nullptr, "controller\\player_5\\ProfileName", &Settings::values.players.GetValue(true)[5].profile_name, ""},
        {nullptr, "controller\\player_5\\Vibration\\UseSystem", &Settings::values.players.GetValue(true)[5].use_system_vibrator, false},

        {nullptr, "controller\\player_6\\Connected", &Settings::values.players.GetValue(true)[6].connected, false},
        {nullptr, "controller\\player_6\\ControllerType", &Settings::values.players.GetValue(true)[6].controller_type, InputSettings::ControllerType::ProController},
        {nullptr, "controller\\player_6\\Button\\A", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::A], ""},
        {nullptr, "controller\\player_6\\Button\\B", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::B], ""},
        {nullptr, "controller\\player_6\\Button\\X", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::X], ""},
        {nullptr, "controller\\player_6\\Button\\Y", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::Y], ""},
        {nullptr, "controller\\player_6\\Button\\LStick", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::LStick], ""},
        {nullptr, "controller\\player_6\\Button\\RStick", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::RStick], ""},
        {nullptr, "controller\\player_6\\Button\\L", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::L], ""},
        {nullptr, "controller\\player_6\\Button\\R", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::R], ""},
        {nullptr, "controller\\player_6\\Button\\ZL", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::ZL], ""},
        {nullptr, "controller\\player_6\\Button\\ZR", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::ZR], ""},
        {nullptr, "controller\\player_6\\Button\\Plus", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::Plus], ""},
        {nullptr, "controller\\player_6\\Button\\Minus", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::Minus], ""},
        {nullptr, "controller\\player_6\\Button\\DLeft", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::DLeft], ""},
        {nullptr, "controller\\player_6\\Button\\DUp", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::DUp], ""},
        {nullptr, "controller\\player_6\\Button\\DRight", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::DRight], ""},
        {nullptr, "controller\\player_6\\Button\\DDown", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::DDown], ""},
        {nullptr, "controller\\player_6\\Button\\SLLeft", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::SLLeft], ""},
        {nullptr, "controller\\player_6\\Button\\SRLeft", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::SRLeft], ""},
        {nullptr, "controller\\player_6\\Button\\Home", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::Home], ""},
        {nullptr, "controller\\player_6\\Button\\Screenshot", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::Screenshot], ""},
        {nullptr, "controller\\player_6\\Button\\SLRight", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::SLRight], ""},
        {nullptr, "controller\\player_6\\Button\\SRRight", &Settings::values.players.GetValue(true)[6].buttons[(size_t)NativeButtonValues::SRRight], ""},
        {nullptr, "controller\\player_6\\Analog\\LStick", &Settings::values.players.GetValue(true)[6].analogs[(size_t)NativeAnalogValues::LStick], ""},
        {nullptr, "controller\\player_6\\Analog\\RStick", &Settings::values.players.GetValue(true)[6].analogs[(size_t)NativeAnalogValues::RStick], ""},
        {nullptr, "controller\\player_6\\Motion\\Left", &Settings::values.players.GetValue(true)[6].motions[(size_t)NativeMotionValues::MotionLeft], ""},
        {nullptr, "controller\\player_6\\Motion\\Right", &Settings::values.players.GetValue(true)[6].motions[(size_t)NativeMotionValues::MotionRight], ""},
        {nullptr, "controller\\player_6\\Vibration\\Enabled", &Settings::values.players.GetValue(true)[6].vibration_enabled, false},
        {nullptr, "controller\\player_6\\Vibration\\Strength", &Settings::values.players.GetValue(true)[6].vibration_strength, 0},
        {nullptr, "controller\\player_6\\BodyColor\\Left", &Settings::values.players.GetValue(true)[6].body_color_left, 0},
        {nullptr, "controller\\player_6\\BodyColor\\Right", &Settings::values.players.GetValue(true)[6].body_color_right, 0},
        {nullptr, "controller\\player_6\\ButtonColor\\Left", &Settings::values.players.GetValue(true)[6].button_color_left, 0},
        {nullptr, "controller\\player_6\\ButtonColor\\Right", &Settings::values.players.GetValue(true)[6].button_color_right, 0},
        {nullptr, "controller\\player_6\\ProfileName", &Settings::values.players.GetValue(true)[6].profile_name, ""},
        {nullptr, "controller\\player_6\\Vibration\\UseSystem", &Settings::values.players.GetValue(true)[6].use_system_vibrator, false},

        {nullptr, "controller\\player_7\\Connected", &Settings::values.players.GetValue(true)[7].connected, false},
        {nullptr, "controller\\player_7\\ControllerType", &Settings::values.players.GetValue(true)[7].controller_type, InputSettings::ControllerType::ProController},
        {nullptr, "controller\\player_7\\Button\\A", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::A], ""},
        {nullptr, "controller\\player_7\\Button\\B", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::B], ""},
        {nullptr, "controller\\player_7\\Button\\X", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::X], ""},
        {nullptr, "controller\\player_7\\Button\\Y", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::Y], ""},
        {nullptr, "controller\\player_7\\Button\\LStick", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::LStick], ""},
        {nullptr, "controller\\player_7\\Button\\RStick", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::RStick], ""},
        {nullptr, "controller\\player_7\\Button\\L", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::L], ""},
        {nullptr, "controller\\player_7\\Button\\R", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::R], ""},
        {nullptr, "controller\\player_7\\Button\\ZL", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::ZL], ""},
        {nullptr, "controller\\player_7\\Button\\ZR", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::ZR], ""},
        {nullptr, "controller\\player_7\\Button\\Plus", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::Plus], ""},
        {nullptr, "controller\\player_7\\Button\\Minus", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::Minus], ""},
        {nullptr, "controller\\player_7\\Button\\DLeft", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::DLeft], ""},
        {nullptr, "controller\\player_7\\Button\\DUp", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::DUp], ""},
        {nullptr, "controller\\player_7\\Button\\DRight", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::DRight], ""},
        {nullptr, "controller\\player_7\\Button\\DDown", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::DDown], ""},
        {nullptr, "controller\\player_7\\Button\\SLLeft", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::SLLeft], ""},
        {nullptr, "controller\\player_7\\Button\\SRLeft", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::SRLeft], ""},
        {nullptr, "controller\\player_7\\Button\\Home", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::Home], ""},
        {nullptr, "controller\\player_7\\Button\\Screenshot", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::Screenshot], ""},
        {nullptr, "controller\\player_7\\Button\\SLRight", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::SLRight], ""},
        {nullptr, "controller\\player_7\\Button\\SRRight", &Settings::values.players.GetValue(true)[7].buttons[(size_t)NativeButtonValues::SRRight], ""},
        {nullptr, "controller\\player_7\\Analog\\LStick", &Settings::values.players.GetValue(true)[7].analogs[(size_t)NativeAnalogValues::LStick], ""},
        {nullptr, "controller\\player_7\\Analog\\RStick", &Settings::values.players.GetValue(true)[7].analogs[(size_t)NativeAnalogValues::RStick], ""},
        {nullptr, "controller\\player_7\\Motion\\Left", &Settings::values.players.GetValue(true)[7].motions[(size_t)NativeMotionValues::MotionLeft], ""},
        {nullptr, "controller\\player_7\\Motion\\Right", &Settings::values.players.GetValue(true)[7].motions[(size_t)NativeMotionValues::MotionRight], ""},
        {nullptr, "controller\\player_7\\Vibration\\Enabled", &Settings::values.players.GetValue(true)[7].vibration_enabled, false},
        {nullptr, "controller\\player_7\\Vibration\\Strength", &Settings::values.players.GetValue(true)[7].vibration_strength, 0},
        {nullptr, "controller\\player_7\\BodyColor\\Left", &Settings::values.players.GetValue(true)[7].body_color_left, 0},
        {nullptr, "controller\\player_7\\BodyColor\\Right", &Settings::values.players.GetValue(true)[7].body_color_right, 0},
        {nullptr, "controller\\player_7\\ButtonColor\\Left", &Settings::values.players.GetValue(true)[7].button_color_left, 0},
        {nullptr, "controller\\player_7\\ButtonColor\\Right", &Settings::values.players.GetValue(true)[7].button_color_right, 0},
        {nullptr, "controller\\player_7\\ProfileName", &Settings::values.players.GetValue(true)[7].profile_name, ""},
        {nullptr, "controller\\player_7\\Vibration\\UseSystem", &Settings::values.players.GetValue(true)[7].use_system_vibrator, false},

        {nullptr, "controller\\player_8\\Connected", &Settings::values.players.GetValue(true)[8].connected, false},
        {nullptr, "controller\\player_8\\ControllerType", &Settings::values.players.GetValue(true)[8].controller_type, InputSettings::ControllerType::ProController},
        {nullptr, "controller\\player_8\\Button\\A", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::A], ""},
        {nullptr, "controller\\player_8\\Button\\B", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::B], ""},
        {nullptr, "controller\\player_8\\Button\\X", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::X], ""},
        {nullptr, "controller\\player_8\\Button\\Y", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::Y], ""},
        {nullptr, "controller\\player_8\\Button\\LStick", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::LStick], ""},
        {nullptr, "controller\\player_8\\Button\\RStick", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::RStick], ""},
        {nullptr, "controller\\player_8\\Button\\L", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::L], ""},
        {nullptr, "controller\\player_8\\Button\\R", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::R], ""},
        {nullptr, "controller\\player_8\\Button\\ZL", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::ZL], ""},
        {nullptr, "controller\\player_8\\Button\\ZR", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::ZR], ""},
        {nullptr, "controller\\player_8\\Button\\Plus", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::Plus], ""},
        {nullptr, "controller\\player_8\\Button\\Minus", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::Minus], ""},
        {nullptr, "controller\\player_8\\Button\\DLeft", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::DLeft], ""},
        {nullptr, "controller\\player_8\\Button\\DUp", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::DUp], ""},
        {nullptr, "controller\\player_8\\Button\\DRight", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::DRight], ""},
        {nullptr, "controller\\player_8\\Button\\DDown", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::DDown], ""},
        {nullptr, "controller\\player_8\\Button\\SLLeft", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::SLLeft], ""},
        {nullptr, "controller\\player_8\\Button\\SRLeft", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::SRLeft], ""},
        {nullptr, "controller\\player_8\\Button\\Home", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::Home], ""},
        {nullptr, "controller\\player_8\\Button\\Screenshot", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::Screenshot], ""},
        {nullptr, "controller\\player_8\\Button\\SLRight", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::SLRight], ""},
        {nullptr, "controller\\player_8\\Button\\SRRight", &Settings::values.players.GetValue(true)[8].buttons[(size_t)NativeButtonValues::SRRight], ""},
        {nullptr, "controller\\player_8\\Analog\\LStick", &Settings::values.players.GetValue(true)[8].analogs[(size_t)NativeAnalogValues::LStick], ""},
        {nullptr, "controller\\player_8\\Analog\\RStick", &Settings::values.players.GetValue(true)[8].analogs[(size_t)NativeAnalogValues::RStick], ""},
        {nullptr, "controller\\player_8\\Motion\\Left", &Settings::values.players.GetValue(true)[8].motions[(size_t)NativeMotionValues::MotionLeft], ""},
        {nullptr, "controller\\player_8\\Motion\\Right", &Settings::values.players.GetValue(true)[8].motions[(size_t)NativeMotionValues::MotionRight], ""},
        {nullptr, "controller\\player_8\\Vibration\\Enabled", &Settings::values.players.GetValue(true)[8].vibration_enabled, false},
        {nullptr, "controller\\player_8\\Vibration\\Strength", &Settings::values.players.GetValue(true)[8].vibration_strength, 0},
        {nullptr, "controller\\player_8\\BodyColor\\Left", &Settings::values.players.GetValue(true)[8].body_color_left, 0},
        {nullptr, "controller\\player_8\\BodyColor\\Right", &Settings::values.players.GetValue(true)[8].body_color_right, 0},
        {nullptr, "controller\\player_8\\ButtonColor\\Left", &Settings::values.players.GetValue(true)[8].button_color_left, 0},
        {nullptr, "controller\\player_8\\ButtonColor\\Right", &Settings::values.players.GetValue(true)[8].button_color_right, 0},
        {nullptr, "controller\\player_8\\ProfileName", &Settings::values.players.GetValue(true)[8].profile_name, ""},
        {nullptr, "controller\\player_8\\Vibration\\UseSystem", &Settings::values.players.GetValue(true)[8].use_system_vibrator, false},

        {nullptr, "controller\\player_9\\Connected", &Settings::values.players.GetValue(true)[9].connected, false},
        {nullptr, "controller\\player_9\\ControllerType", &Settings::values.players.GetValue(true)[9].controller_type, InputSettings::ControllerType::ProController},
        {nullptr, "controller\\player_9\\Button\\A", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::A], ""},
        {nullptr, "controller\\player_9\\Button\\B", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::B], ""},
        {nullptr, "controller\\player_9\\Button\\X", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::X], ""},
        {nullptr, "controller\\player_9\\Button\\Y", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::Y], ""},
        {nullptr, "controller\\player_9\\Button\\LStick", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::LStick], ""},
        {nullptr, "controller\\player_9\\Button\\RStick", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::RStick], ""},
        {nullptr, "controller\\player_9\\Button\\L", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::L], ""},
        {nullptr, "controller\\player_9\\Button\\R", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::R], ""},
        {nullptr, "controller\\player_9\\Button\\ZL", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::ZL], ""},
        {nullptr, "controller\\player_9\\Button\\ZR", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::ZR], ""},
        {nullptr, "controller\\player_9\\Button\\Plus", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::Plus], ""},
        {nullptr, "controller\\player_9\\Button\\Minus", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::Minus], ""},
        {nullptr, "controller\\player_9\\Button\\DLeft", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::DLeft], ""},
        {nullptr, "controller\\player_9\\Button\\DUp", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::DUp], ""},
        {nullptr, "controller\\player_9\\Button\\DRight", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::DRight], ""},
        {nullptr, "controller\\player_9\\Button\\DDown", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::DDown], ""},
        {nullptr, "controller\\player_9\\Button\\SLLeft", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::SLLeft], ""},
        {nullptr, "controller\\player_9\\Button\\SRLeft", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::SRLeft], ""},
        {nullptr, "controller\\player_9\\Button\\Home", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::Home], ""},
        {nullptr, "controller\\player_9\\Button\\Screenshot", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::Screenshot], ""},
        {nullptr, "controller\\player_9\\Button\\SLRight", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::SLRight], ""},
        {nullptr, "controller\\player_9\\Button\\SRRight", &Settings::values.players.GetValue(true)[9].buttons[(size_t)NativeButtonValues::SRRight], ""},
        {nullptr, "controller\\player_9\\Analog\\LStick", &Settings::values.players.GetValue(true)[9].analogs[(size_t)NativeAnalogValues::LStick], ""},
        {nullptr, "controller\\player_9\\Analog\\RStick", &Settings::values.players.GetValue(true)[9].analogs[(size_t)NativeAnalogValues::RStick], ""},
        {nullptr, "controller\\player_9\\Motion\\Left", &Settings::values.players.GetValue(true)[9].motions[(size_t)NativeMotionValues::MotionLeft], ""},
        {nullptr, "controller\\player_9\\Motion\\Right", &Settings::values.players.GetValue(true)[9].motions[(size_t)NativeMotionValues::MotionRight], ""},
        {nullptr, "controller\\player_9\\Vibration\\Enabled", &Settings::values.players.GetValue(true)[9].vibration_enabled, false},
        {nullptr, "controller\\player_9\\Vibration\\Strength", &Settings::values.players.GetValue(true)[9].vibration_strength, 0},
        {nullptr, "controller\\player_9\\BodyColor\\Left", &Settings::values.players.GetValue(true)[9].body_color_left, 0},
        {nullptr, "controller\\player_9\\BodyColor\\Right", &Settings::values.players.GetValue(true)[9].body_color_right, 0},
        {nullptr, "controller\\player_9\\ButtonColor\\Left", &Settings::values.players.GetValue(true)[9].button_color_left, 0},
        {nullptr, "controller\\player_9\\ButtonColor\\Right", &Settings::values.players.GetValue(true)[9].button_color_right, 0},
        {nullptr, "controller\\player_9\\ProfileName", &Settings::values.players.GetValue(true)[9].profile_name, ""},
        {nullptr, "controller\\player_9\\Vibration\\UseSystem", &Settings::values.players.GetValue(true)[9].use_system_vibrator, false},

        {NXOsSetting::AudioSinkId, "audio\\sink_id", &Settings::values.sink_id},
        { NXOsSetting::AudioOutputDeviceId, "audio\\output_device_id", &Settings::values.audio_output_device_id },
        { NXOsSetting::AudioInputDeviceId, "audio\\input_device_id", &Settings::values.audio_input_device_id },
        { NXOsSetting::AudioMode, "audio\\mode", &Settings::values.sound_index },
        { NXOsSetting::AudioVolume, "audio\\volume", &Settings::values.volume },
        { NXOsSetting::AudioMuted, "audio\\muted", &Settings::values.audio_muted },
    };

    JsonValue GetNestedValue(const JsonValue & section, const char * key)
    {
        strvector parts = stdstr(key).Tokenize('\\');
        JsonValue current = section;

        for (const stdstr & part : parts)
        {
            if (!current.isObject())
            {
                return JsonValue();
            }
            current = current[part];
        }
        return current;
    }

    void SetNestedValue(JsonValue& section, const char* key, const JsonValue& value)
    {
        strvector parts = stdstr(key).Tokenize('\\');
        if (parts.empty())
        {
            return;
        }

        JsonValue * current = &section;
        for (size_t i = 0; i < parts.size() - 1; i++)
        {
            if (!(*current)[parts[i]].isObject())
            {
                (*current)[parts[i]] = JsonValue(JsonValueType::Object);
            }
            current = &(*current)[parts[i]];
        }
        (*current)[parts.back()] = value;
    }

    template <typename T>
    std::vector<std::pair<std::string, T>> Canonicalizations();

    // Specialization for ControllerType
    template <>
    std::vector<std::pair<std::string, InputSettings::ControllerType>> Canonicalizations<InputSettings::ControllerType>()
    {
        return {
            {"ProController", InputSettings::ControllerType::ProController},
            {"DualJoyconDetached", InputSettings::ControllerType::DualJoyconDetached},
            {"LeftJoycon", InputSettings::ControllerType::LeftJoycon},
            {"RightJoycon", InputSettings::ControllerType::RightJoycon},
            {"Handheld", InputSettings::ControllerType::Handheld},
            {"GameCube", InputSettings::ControllerType::GameCube},
            {"Pokeball", InputSettings::ControllerType::Pokeball},
            {"NES", InputSettings::ControllerType::NES},
            {"SNES", InputSettings::ControllerType::SNES},
            {"N64", InputSettings::ControllerType::N64},
            {"SegaGenesis", InputSettings::ControllerType::SegaGenesis},
        };
    };

    template <typename T>
    std::string CanonicalizeEnum(T value)
    {
        using CanonicalItem = std::pair<std::string, T>;
        using CanonicalList = std::vector<CanonicalItem>;
        const CanonicalList group = Canonicalizations<T>();

        for (const CanonicalItem & item : group)
        {
            if (item.second == value)
            {
                return item.first;
            }
        }
        return "";
    }

    template <typename T>
    T ParseEnum(const std::string& canonicalization)
    {
        using CanonicalItem = std::pair<std::string, T>;
        using CanonicalList = std::vector<CanonicalItem>;
        const CanonicalList group = Canonicalizations<T>();

        for (const CanonicalItem& item : group)
        {
            if (item.first == canonicalization)
            {
                return item.second;
            }
        }
        return {};
    }
}

void OsSettingChanged(const char * setting, void * /*userData*/)
{
    for (const OsSetting & osSetting : settings)
    {
        if (osSetting.identifier == nullptr)
        {
            continue;
        }

        if (strcmp(osSetting.identifier, setting) != 0)
        {
            continue;
        }
        switch (osSetting.settingType)
        {
        case SettingType::StringSetting:
            osSetting.setting.stringSetting->SetValue(g_settings->GetString(setting));
            break;
        case SettingType::AudioEngine:
            osSetting.setting.audioEngine->SetValue((Settings::AudioEngine)g_settings->GetInt(setting));
            break;
        case SettingType::AudioMode:
            osSetting.setting.audioMode->SetValue((Settings::AudioMode)g_settings->GetInt(setting));
            break;
        case SettingType::U8:
            osSetting.setting.u8->SetValue(g_settings->GetInt(setting));
            break;
        case SettingType::BooleanSetting:
            osSetting.setting.booleanSetting->SetValue(g_settings->GetBool(setting));
            break;
        case SettingType::BooleanValue:
            *osSetting.setting.boolValue = g_settings->GetBool(setting);
            break;
        default:
            UNIMPLEMENTED();
        }
    }
}

void SetupOsSetting(void)
{
    for (const OsSetting & osSetting : settings)
    {
        switch (osSetting.settingType)
        {
        case SettingType::StringSetting:
            osSetting.setting.stringSetting->SetValue(osSetting.setting.stringSetting->GetDefault());
            break;
        case SettingType::AudioEngine:
            osSetting.setting.audioEngine->SetValue(osSetting.setting.audioEngine->GetDefault());
            break;
        case SettingType::AudioMode:
            osSetting.setting.audioMode->SetValue(osSetting.setting.audioMode->GetDefault());
            break;
        case SettingType::U8:
            osSetting.setting.u8->SetValue(osSetting.setting.u8->GetDefault());
            break;
        case SettingType::BooleanSetting:
            osSetting.setting.booleanSetting->SetValue(osSetting.setting.booleanSetting->GetDefault());
            break;
        case SettingType::BooleanValue:
            *osSetting.setting.boolValue = osSetting.defaultValue.boolValue;
            break;
        case SettingType::UnsignedInt:
            *osSetting.setting.uint32Value = osSetting.defaultValue.uint32Value;
            break;
        case SettingType::StringValue:
            *osSetting.setting.stringValue = osSetting.defaultValue.sringValue;
            break;
        case SettingType::ControllerType:
            *osSetting.setting.controllerType = osSetting.defaultValue.controllerType;
            break;
        default:
            UNIMPLEMENTED();
        }
    }
    
    JsonValue root;
    JsonReader reader;
    std::string json = g_settings->GetSectionSettings("nxemu-os");

    if (!json.empty() && reader.Parse(json.data(), json.data() + json.size(), root))
    {
        for (const OsSetting & osSetting : settings)
        {
            JsonValue value = GetNestedValue(root, osSetting.json_path);
            switch (osSetting.settingType)
            {
            case SettingType::StringSetting:
                if (value.isString())
                {
                    osSetting.setting.stringSetting->SetValue(value.asString());
                }
                break;
            case SettingType::AudioEngine:
                if (value.isString())
                {
                    osSetting.setting.audioEngine->SetValue(Settings::ToEnum<Settings::AudioEngine>(value.asString()));
                }
                break;
            case SettingType::AudioMode:
                if (value.isString())
                {
                    osSetting.setting.audioMode->SetValue(Settings::ToEnum<Settings::AudioMode>(value.asString()));
                }
                break;
            case SettingType::U8:
                if (value.isInt())
                {
                    osSetting.setting.u8->SetValue((uint8_t)value.asInt64());
                }
                break;
            case SettingType::BooleanSetting:
                if (value.isBool())
                {
                    osSetting.setting.booleanSetting->SetValue(value.asBool());
                }
                break;
            case SettingType::BooleanValue:
                if (value.isBool())
                {
                    *osSetting.setting.boolValue = value.asBool();
                }
                break;
            case SettingType::UnsignedInt:
                if (value.isInt())
                {
                    *osSetting.setting.uint32Value = (uint32_t)value.asUInt64();
                }
                break;
            case SettingType::StringValue:
                if (value.isString())
                {
                    *osSetting.setting.stringValue = value.asString();
                }
                break;
            case SettingType::ControllerType:
                if (value.isString())
                {
                    *osSetting.setting.controllerType = ParseEnum<InputSettings::ControllerType>(value.asString());
                }
                break;
            default:
                UNIMPLEMENTED();
            }
        }
    }

    for (const OsSetting & osSetting : settings)
    {
        if (osSetting.identifier == nullptr)
        {
            continue;
        }

        switch (osSetting.settingType)
        {
        case SettingType::StringSetting:
            g_settings->SetDefaultString(osSetting.identifier, osSetting.setting.stringSetting->GetDefault().c_str());
            g_settings->SetString(osSetting.identifier, osSetting.setting.stringSetting->GetValue().c_str());
            break;
        case SettingType::AudioEngine:
            g_settings->SetDefaultInt(osSetting.identifier, (int32_t)osSetting.setting.audioEngine->GetDefault());
            g_settings->SetInt(osSetting.identifier, (int32_t)osSetting.setting.audioEngine->GetValue());
            break;
        case SettingType::AudioMode:
            g_settings->SetDefaultInt(osSetting.identifier, (int32_t)osSetting.setting.audioMode->GetDefault());
            g_settings->SetInt(osSetting.identifier, (int32_t)osSetting.setting.audioMode->GetValue());
            break;
        case SettingType::U8:
            g_settings->SetDefaultInt(osSetting.identifier, (int32_t)osSetting.setting.u8->GetDefault());
            g_settings->SetInt(osSetting.identifier, (int32_t)osSetting.setting.u8->GetValue());
            break;
        case SettingType::BooleanSetting:
            g_settings->SetDefaultBool(osSetting.identifier, osSetting.setting.booleanSetting->GetDefault() != 0);
            g_settings->SetBool(osSetting.identifier, osSetting.setting.booleanSetting->GetValue() != 0);
            break;
        case SettingType::BooleanValue:
            g_settings->SetDefaultBool(osSetting.identifier, osSetting.defaultValue.boolValue);
            g_settings->SetBool(osSetting.identifier, *osSetting.setting.boolValue);
            break;
        default:
            UNIMPLEMENTED();
        }
        g_settings->RegisterCallback(osSetting.identifier, OsSettingChanged, nullptr);
    }
}

void SaveOsSettings(void)
{
    JsonValue root;

    for (const OsSetting& osSetting : settings)
    {
        switch (osSetting.settingType)
        {
        case SettingType::StringSetting:
            if (osSetting.setting.stringSetting->GetValue() != osSetting.setting.stringSetting->GetDefault())
            {
                SetNestedValue(root, osSetting.json_path, osSetting.setting.stringSetting->GetValue());
            }
            break;
        case SettingType::AudioEngine:
            if (osSetting.setting.audioEngine->GetValue() != osSetting.setting.audioEngine->GetDefault())
            {
                SetNestedValue(root, osSetting.json_path, Settings::CanonicalizeEnum(osSetting.setting.audioEngine->GetValue()));
            }
            break;
        case SettingType::AudioMode:
            if (osSetting.setting.audioMode->GetValue() != osSetting.setting.audioMode->GetDefault())
            {
                SetNestedValue(root, osSetting.json_path, Settings::CanonicalizeEnum(osSetting.setting.audioMode->GetValue()));
            }
            break;
        case SettingType::U8:
            if (osSetting.setting.u8->GetValue() != osSetting.setting.u8->GetDefault())
            {
                SetNestedValue(root, osSetting.json_path, (int32_t)osSetting.setting.u8->GetValue());
            }
            break;
        case SettingType::BooleanSetting:
            if (osSetting.setting.booleanSetting->GetValue() != osSetting.setting.booleanSetting->GetDefault())
            {
                SetNestedValue(root, osSetting.json_path, osSetting.setting.booleanSetting->GetValue() != 0);
            }
            break;
        case SettingType::BooleanValue:
            if (*osSetting.setting.boolValue != osSetting.defaultValue.boolValue)
            {
                SetNestedValue(root, osSetting.json_path, *osSetting.setting.boolValue != 0);
            }
            break;
        case SettingType::UnsignedInt:
            if (*osSetting.setting.uint32Value != osSetting.defaultValue.uint32Value)
            {
                SetNestedValue(root, osSetting.json_path, *osSetting.setting.uint32Value);
            }
            break;
        case SettingType::StringValue:
            if (*osSetting.setting.stringValue != osSetting.defaultValue.sringValue)
            {
                SetNestedValue(root, osSetting.json_path, *osSetting.setting.stringValue);
            }
            break;
        case SettingType::ControllerType:
            if (*osSetting.setting.controllerType != osSetting.defaultValue.controllerType)
            {
                SetNestedValue(root, osSetting.json_path, CanonicalizeEnum(*osSetting.setting.controllerType));
            }
            break;
        default:
            UNIMPLEMENTED();
        }
    }
    g_settings->SetSectionSettings("nxemu-os", root.isNull() ? "" : JsonStyledWriter().write(root));
}

namespace
{
    OsSetting::OsSetting(const char * id,const char * path, Settings::SwitchableSetting<std::string> * val) :
        identifier(id),
        json_path(path),
        settingType(SettingType::StringSetting)
    {
        setting.stringSetting = val;
    }

    OsSetting::OsSetting(const char * id, const char * path, Settings::SwitchableSetting<Settings::AudioEngine> * val) :
        identifier(id),
        json_path(path),
        settingType(SettingType::AudioEngine)
    {
        setting.audioEngine = val;
    }

    OsSetting::OsSetting(const char * id, const char * path, Settings::SwitchableSetting<Settings::AudioMode, true> * val) :
        identifier(id),
        json_path(path),
        settingType(SettingType::AudioMode)
    {
        setting.audioMode = val;
    }

    OsSetting::OsSetting(const char * id, const char * path, Settings::SwitchableSetting<u8, true> * val) :
        identifier(id),
        json_path(path),
        settingType(SettingType::U8)
    {
        setting.u8 = val;
    }
    
    OsSetting::OsSetting(const char* id, const char * path, Settings::Setting<bool, false> * val) :
        identifier(id),
        json_path(path),
        settingType(SettingType::BooleanSetting)
    {
        setting.booleanSetting = val;
    }

    OsSetting::OsSetting(const char * id, const char * path, bool * val, bool defaultValue_) :
        identifier(id),
        json_path(path),
        settingType(SettingType::BooleanValue)
    {
        setting.boolValue = val;
        defaultValue.boolValue = defaultValue_;
    }

    OsSetting::OsSetting(const char * id, const char * path, int * val, int defaultValue_) :
        identifier(id),
        json_path(path),
        settingType(SettingType::UnsignedInt)
    {
        setting.uint32Value = (uint32_t *)val;
        defaultValue.uint32Value = defaultValue_;
    }

    OsSetting::OsSetting(const char * id, const char * path, uint32_t * val, uint32_t defaultValue_) :
        identifier(id),
        json_path(path),
        settingType(SettingType::UnsignedInt)
    {
        setting.uint32Value = val;
        defaultValue.uint32Value = defaultValue_;
    }

    OsSetting::OsSetting(const char * id, const char * path, std::string * val, const char * defaultValue_) :
        identifier(id),
        json_path(path),
        settingType(SettingType::StringValue)
    {
        setting.stringValue = val;
        defaultValue.sringValue = defaultValue_;
    }

    OsSetting::OsSetting(const char * id, const char * path, InputSettings::ControllerType * val, InputSettings::ControllerType defaultValue_) :
        identifier(id),
        json_path(path),
        settingType(SettingType::ControllerType)
    {
        setting.controllerType = val;
        defaultValue.controllerType = defaultValue_;
    }
}