#pragma once
#include <sciter_ui.h>
#include <sciter_element.h>
#include <sciter_handler.h>
#include <widgets/page_nav.h>

class SystemConfig;

class SystemConfigDebug :
    public IPagesSink
{
public:
    SystemConfigDebug(ISciterUI & sciterUI, SystemConfig & config, HWINDOW parent, SciterElement page);
    ~SystemConfigDebug() = default;

    void SaveSetting(void);

    // IPagesSink
    bool PageNavChangeFrom(const std::string& pageName, SCITER_ELEMENT pageNav) override;
    bool PageNavChangeTo(const std::string& pageName, SCITER_ELEMENT pageNav) override;
    void PageNavCreatedPage(const std::string& pageName, SCITER_ELEMENT page) override;
    void PageNavPageChanged(const std::string& pageName, SCITER_ELEMENT pageNav) override;

private:
    SystemConfigDebug() = delete;
    SystemConfigDebug(const SystemConfigDebug &) = delete;
    SystemConfigDebug & operator=(const SystemConfigDebug &) = delete;

    void SetupLoggingPage(SciterElement page);

    ISciterUI & m_sciterUI;
    SystemConfig & m_config;
    HWINDOW m_parent;
    SciterElement m_page;
    std::shared_ptr<IPageNav> m_pageNav;
    SciterElement m_loggingPage;
};