#pragma once
#include <sciter_ui.h>
#include <sciter_element.h>
#include <widgets/page_nav.h>

class SystemConfig;

class SystemConfigSystem : public IPagesSink
{
public:
    SystemConfigSystem(ISciterUI & sciterUI, SystemConfig & config, SciterElement page);
    ~SystemConfigSystem() = default;

    void SaveSetting(void);

    bool PageNavChangeFrom(const std::string & pageName, SCITER_ELEMENT pageNav) override;
    bool PageNavChangeTo(const std::string & pageName, SCITER_ELEMENT pageNav) override;
    void PageNavCreatedPage(const std::string & pageName, SCITER_ELEMENT page) override;
    void PageNavPageChanged(const std::string & pageName, SCITER_ELEMENT pageNav) override;

private:
    SystemConfigSystem() = delete;
    SystemConfigSystem(const SystemConfigSystem &) = delete;
    SystemConfigSystem & operator=(const SystemConfigSystem &) = delete;

    void SetupSystemPage(SciterElement page);

    ISciterUI & m_sciterUI;
    SystemConfig & m_config;
    SciterElement m_page;
    std::shared_ptr<IPageNav> m_pageNav;
    SciterElement m_systemPage;
};
