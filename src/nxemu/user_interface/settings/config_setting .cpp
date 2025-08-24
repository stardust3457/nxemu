#include "config_setting.h"

ConfigSetting::ConfigSetting(TYPE_COMBOBOX /*type*/, const char * elementId, bool canChangeWhenRunning, uint32_t settingIndex, const char * storeSettingId) :
    m_type(ConfigSettingType::ComboBox),
    m_elementId(elementId),
    m_canChangeWhenRunning(canChangeWhenRunning),
    m_settingIndex(settingIndex),
    m_storeSettingId(storeSettingId)
{
}

ConfigSetting::ConfigSetting(TYPE_COMBOBOXVALUE /*type*/, const char * elementId, bool canChangeWhenRunning, const char * storeSettingId) :
    m_type(ConfigSettingType::ComboBoxValue),
    m_elementId(elementId),
    m_canChangeWhenRunning(canChangeWhenRunning),
    m_settingIndex(0),
    m_storeSettingId(storeSettingId)
{
}

ConfigSetting::ConfigSetting(TYPE_CHECKBOX /*type*/, const char * elementId, bool canChangeWhenRunning, const char * storeSettingId) :
    m_type(ConfigSettingType::CheckBox),
    m_elementId(elementId),
    m_canChangeWhenRunning(canChangeWhenRunning),
    m_settingIndex(0),
    m_storeSettingId(storeSettingId)
{
}

ConfigSetting::ConfigSetting(TYPE_SLIDER /*type*/, const char * elementId, bool canChangeWhenRunning, const char * storeSettingId) :
    m_type(ConfigSettingType::Slider),
    m_elementId(elementId),
    m_canChangeWhenRunning(canChangeWhenRunning),
    m_settingIndex(0),
    m_storeSettingId(storeSettingId)
{
}

ConfigSetting::ConfigSetting(TYPE_INPUT_TEXT /*type*/, const char * elementId, bool canChangeWhenRunning, const char * storeSettingId) :
    m_type(ConfigSettingType::InputText),
    m_elementId(elementId),
    m_canChangeWhenRunning(canChangeWhenRunning),
    m_settingIndex(0),
    m_storeSettingId(storeSettingId)
{
}

bool ConfigSetting::CanChangeWhenRunning() const
{
    return m_canChangeWhenRunning;
}

ConfigSettingType ConfigSetting::Type() const
{
    return m_type;
}

const char * ConfigSetting::ElementId() const
{
    return m_elementId.c_str();
}

uint32_t ConfigSetting::SettingIndex() const
{
    return m_settingIndex;
}

const char * ConfigSetting::StoreSettingId() const
{
    return m_storeSettingId.c_str();
}

