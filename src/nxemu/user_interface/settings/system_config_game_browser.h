#pragma once
#include <sciter_ui.h>
#include <sciter_element.h>
#include <sciter_handler.h>
#include <widgets/page_nav.h>

class SystemConfig;

class SystemConfigGameBrowser :
    public IClickSink
{
public:
    SystemConfigGameBrowser(ISciterUI & sciterUI, SystemConfig & config, ISciterWindow & window, SciterElement page);
    ~SystemConfigGameBrowser() = default;

    void SaveSetting(void);

    // IClickSink
    bool OnClick(SCITER_ELEMENT element, SCITER_ELEMENT source, uint32_t reason) override;

private:
    SystemConfigGameBrowser() = delete;
    SystemConfigGameBrowser(const SystemConfigGameBrowser&) = delete;
    SystemConfigGameBrowser& operator=(const SystemConfigGameBrowser&) = delete;

    void SelectDirectoryItem(const SciterElement & gameDirectoryList, SCITER_ELEMENT source);
    void RemoveSelectedDirectory();

    ISciterUI & m_sciterUI;
    SystemConfig & m_config;
    ISciterWindow & m_window;
    SciterElement m_page;
};