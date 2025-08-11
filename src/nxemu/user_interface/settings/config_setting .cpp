#include "config_setting.h"

ConfigSetting::ConfigSetting(TYPE_COMBOBOX /*type*/, const char * elementId, uint32_t settingIndex, const char * StoreSettingId) :
    m_type(ConfigSettingType::ComboBox),
    m_elementId(elementId),
    m_settingIndex(settingIndex),
    m_storeSettingId(StoreSettingId)
{
}

ConfigSetting::ConfigSetting(TYPE_CHECKBOX /*type*/, const char * elementId, const char * StoreSettingId) :
    m_type(ConfigSettingType::CheckBox),
    m_elementId(elementId),
    m_settingIndex(0),
    m_storeSettingId(StoreSettingId)
{
}

ConfigSetting::ConfigSetting(TYPE_SLIDER /*type*/, const char * elementId, const char * StoreSettingId) :
    m_type(ConfigSettingType::Slider),
    m_elementId(elementId),
    m_settingIndex(0),
    m_storeSettingId(StoreSettingId)
{
}

ConfigSetting::ConfigSetting(TYPE_INPUT_TEXT /*type*/, const char * elementId, const char * StoreSettingId) :
    m_type(ConfigSettingType::InputText),
    m_elementId(elementId),
    m_settingIndex(0),
    m_storeSettingId(StoreSettingId)
{
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

