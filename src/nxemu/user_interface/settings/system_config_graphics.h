#pragma once
#include <sciter_ui.h>
#include <sciter_element.h>
#include <sciter_handler.h>
#include <widgets/page_nav.h>

class SystemConfig;

class SystemConfigGraphics :
    public IPagesSink,
    public IStateChangeSink
{
public:
    SystemConfigGraphics(ISciterUI & sciterUI, SystemConfig & config, HWINDOW parent, SciterElement page);
    ~SystemConfigGraphics() = default;

    void SaveSetting(void);

    // IPagesSink
    bool PageNavChangeFrom(const std::string & pageName, SCITER_ELEMENT pageNav) override;
    bool PageNavChangeTo(const std::string & pageName, SCITER_ELEMENT pageNav) override;
    void PageNavCreatedPage(const std::string & pageName, SCITER_ELEMENT page) override;
    void PageNavPageChanged(const std::string & pageName, SCITER_ELEMENT pageNav) override;

    // IStateChangeSink
    bool OnStateChange(SCITER_ELEMENT elem, uint32_t eventReason, void * data) override;

private:
    SystemConfigGraphics() = delete;
    SystemConfigGraphics(const SystemConfigGraphics &) = delete;
    SystemConfigGraphics & operator=(const SystemConfigGraphics &) = delete;

    void SetupAdvancedPage(SciterElement page);
    void SetupGraphicsPage(SciterElement page);
    void UpdateGraphicsAPI();
    void UpdateFSPSharpnessDisplay();
    void UpdateVSyncMode();

    ISciterUI & m_sciterUI;
    SystemConfig & m_config;
    HWINDOW m_parent;
    SciterElement m_page;
    std::shared_ptr<IPageNav> m_pageNav;
    SciterElement m_graphicsPage;
    SciterElement m_advancedPage;
};