#pragma once
#include <sciter_ui.h>
#include <sciter_element.h>
#include <widgets/page_nav.h>

class SystemConfig;

class SystemConfigSystem
{
public:
    SystemConfigSystem(ISciterUI & sciterUI, SystemConfig & config, SciterElement page);
    ~SystemConfigSystem() = default;

    void SaveSetting(void);

private:
    SystemConfigSystem() = delete;
    SystemConfigSystem(const SystemConfigSystem &) = delete;
    SystemConfigSystem & operator=(const SystemConfigSystem &) = delete;

    ISciterUI & m_sciterUI;
    SystemConfig & m_config;
    SciterElement m_page;
};
