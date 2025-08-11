#pragma once
#include <string>

enum class ConfigSettingType { ComboBox, CheckBox, Slider, InputText };

class ConfigSetting
{
public:
    enum TYPE_COMBOBOX
    {
        ComboBox
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

    ConfigSetting(TYPE_COMBOBOX type, const char * elementId, uint32_t settingIndex, const char * StoreSettingId);
    ConfigSetting(TYPE_CHECKBOX type, const char * elementId, const char * StoreSettingId);
    ConfigSetting(TYPE_SLIDER type, const char * elementId, const char * StoreSettingId);
    ConfigSetting(TYPE_INPUT_TEXT type, const char * elementId, const char * StoreSettingId);

    ConfigSettingType Type() const;
    const char * ElementId() const;
    uint32_t SettingIndex() const;
    const char * StoreSettingId() const;

private:
    ConfigSettingType m_type;
    std::string m_elementId;
    uint32_t m_settingIndex;
    std::string m_storeSettingId;
};