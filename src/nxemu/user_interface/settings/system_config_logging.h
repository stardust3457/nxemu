#pragma once
#include <sciter_ui.h>
#include <sciter_element.h>
#include <sciter_handler.h>
#include <widgets/page_nav.h>

class SystemConfig;

class SystemConfigLogging
{
public:
    SystemConfigLogging(ISciterUI & sciterUI, SystemConfig & config, ISciterWindow & window, SciterElement page);
    ~SystemConfigLogging() = default;

    void SaveSetting(void);

private:
    SystemConfigLogging() = delete;
    SystemConfigLogging(const SystemConfigLogging &) = delete;
    SystemConfigLogging & operator=(const SystemConfigLogging &) = delete;

    ISciterUI & m_sciterUI;
    SystemConfig & m_config;
    ISciterWindow & m_window;
    SciterElement m_page;
};