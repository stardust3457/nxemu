#pragma once
#include <string>

enum class ConfigSettingType { ComboBox, ComboBoxValue, CheckBox, Slider, InputText };

class ConfigSetting
{
public:
    enum TYPE_COMBOBOX
    {
        ComboBox
    };
    enum TYPE_COMBOBOXVALUE
    {
        ComboBoxValue
    };
    enum TYPE_CHECKBOX
    {
        CheckBox
    };
    enum TYPE_SLIDER
    {
        Slider
    };
    enum TYPE_INPUT_TEXT
    {
        InputText
    };

    ConfigSetting(TYPE_COMBOBOX type, const char * elementId, bool canChangeWhenRunning, uint32_t settingIndex, const char * storeSettingId);
    ConfigSetting(TYPE_COMBOBOXVALUE type, const char * elementId, bool canChangeWhenRunning, const char * storeSettingId);
    ConfigSetting(TYPE_CHECKBOX type, const char * elementId, bool canChangeWhenRunning, const char * storeSettingId);
    ConfigSetting(TYPE_SLIDER type, const char * elementId, bool canChangeWhenRunning, const char * storeSettingId);
    ConfigSetting(TYPE_INPUT_TEXT type, const char * elementId, bool canChangeWhenRunning, const char * storeSettingId);

    bool CanChangeWhenRunning() const;
    ConfigSettingType Type() const;
    const char * ElementId() const;
    uint32_t SettingIndex() const;
    const char * StoreSettingId() const;

private:
    ConfigSettingType m_type;
    std::string m_elementId;
    bool m_canChangeWhenRunning;
    uint32_t m_settingIndex;
    std::string m_storeSettingId;
};