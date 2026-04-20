#include "system_config_system.h"
#include "config_setting.h"
#include "system_config.h"
#include <nxemu-os/os_settings_identifiers.h>
#include <yuzu_common/settings_enums.h>

namespace
{
static ConfigSetting systemSettings[] = {
    ConfigSetting(ConfigSetting::ComboBox, "consoleMode", true, Settings::EnumMetadata<Settings::ConsoleMode>::Index(), NXOsSetting::DockedMode),
};
}

SystemConfigSystem::SystemConfigSystem(ISciterUI & sciterUI, SystemConfig & config, SciterElement page) :
    m_sciterUI(sciterUI),
    m_config(config),
    m_page(page)
{
    SciterElement pageNav = page.GetElementByID("SystemTabNav");
    std::shared_ptr<void> interfacePtr = pageNav.IsValid() ? m_sciterUI.GetElementInterface(pageNav, IID_IPAGENAV) : nullptr;
    if (interfacePtr)
    {
        m_pageNav = std::static_pointer_cast<IPageNav>(interfacePtr);
        m_pageNav->AddSink(this);
    }
}

void SystemConfigSystem::SaveSetting(void)
{
    if (m_systemPage != nullptr)
    {
        m_config.SavePage(m_systemPage, systemSettings, sizeof(systemSettings) / sizeof(systemSettings[0]));
    }
}

bool SystemConfigSystem::PageNavChangeFrom(const std::string & /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
    return true;
}

bool SystemConfigSystem::PageNavChangeTo(const std::string & /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
    return true;
}

void SystemConfigSystem::PageNavCreatedPage(const std::string & pageName, SCITER_ELEMENT page)
{
    if (pageName == "System")
    {
        SetupSystemPage(page);
    }
}

void SystemConfigSystem::PageNavPageChanged(const std::string & /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
}

void SystemConfigSystem::SetupSystemPage(SciterElement page)
{
    m_systemPage = page;
    m_config.SetupPage(page, systemSettings, sizeof(systemSettings) / sizeof(systemSettings[0]));
}
