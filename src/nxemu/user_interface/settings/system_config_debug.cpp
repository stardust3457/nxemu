#include "config_setting.h"
#include "system_config.h"
#include "system_config_debug.h"
#include <nxemu-core/settings/identifiers.h>

namespace
{
    static ConfigSetting loggingSettings[] = {
        ConfigSetting(ConfigSetting::CheckBox, "ShowLogConsole", true, NXCoreSetting::ShowLogConsole),
        ConfigSetting(ConfigSetting::InputText, "LogFilter", true, NXCoreSetting::LogFilter),
    };
}

SystemConfigDebug::SystemConfigDebug(ISciterUI & sciterUI, SystemConfig & config, HWINDOW parent, SciterElement page) :
    m_sciterUI(sciterUI),
    m_config(config),
    m_parent(parent),
    m_page(page),
    m_loggingPage(nullptr)
{
    SciterElement pageNav = page.GetElementByID("DebugTabNav");
    std::shared_ptr<void> interfacePtr = pageNav.IsValid() ? m_sciterUI.GetElementInterface(pageNav, IID_IPAGENAV) : nullptr;
    if (interfacePtr)
    {
        m_pageNav = std::static_pointer_cast<IPageNav>(interfacePtr);
        m_pageNav->AddSink(this);
    }
}

bool SystemConfigDebug::PageNavChangeFrom(const std::string& /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
    return true;
}

bool SystemConfigDebug::PageNavChangeTo(const std::string& /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
    return true;
}

void SystemConfigDebug::PageNavCreatedPage(const std::string& pageName, SCITER_ELEMENT page)
{
    if (pageName == "Logging")
    {
        SetupLoggingPage(page);
    }
}

void SystemConfigDebug::PageNavPageChanged(const std::string& /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
}

void SystemConfigDebug::SetupLoggingPage(SciterElement page)
{
    m_loggingPage = page;
    m_config.SetupPage(page, loggingSettings, sizeof(loggingSettings) / sizeof(loggingSettings[0]));
}

void SystemConfigDebug::SaveSetting(void)
{
    if (m_loggingPage != nullptr)
    {
        m_config.SavePage(m_loggingPage, loggingSettings, sizeof(loggingSettings) / sizeof(loggingSettings[0]));
    }
}