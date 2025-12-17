#pragma once
#include <sciter_ui.h>
#include <sciter_element.h>
#include <sciter_handler.h>
#include <widgets/combo_box.h>
#include <widgets/page_nav.h>

class SystemConfig;
class SystemModules;

class SystemConfigAudio :
    public IPagesSink,
    public IStateChangeSink
{
public:
    SystemConfigAudio(ISciterUI & sciterUI, SystemConfig & config, SystemModules & modules, HWINDOW parent, SciterElement page);
    ~SystemConfigAudio() = default;

    void SaveSetting(void);

    // IPagesSink
    bool PageNavChangeFrom(const std::string& pageName, SCITER_ELEMENT pageNav) override;
    bool PageNavChangeTo(const std::string& pageName, SCITER_ELEMENT pageNav) override;
    void PageNavCreatedPage(const std::string& pageName, SCITER_ELEMENT page) override;
    void PageNavPageChanged(const std::string& pageName, SCITER_ELEMENT pageNav) override;

    // IStateChangeSink
    bool OnStateChange(SCITER_ELEMENT elem, uint32_t eventReason, void * data) override;
    
private:
    SystemConfigAudio() = delete;
    SystemConfigAudio(const SystemConfigAudio &) = delete;
    SystemConfigAudio & operator=(const SystemConfigAudio &) = delete;

    void SetupAudioPage(SciterElement page);
    void updateVolumeDisplay();
    void updateAudioDevices(int32_t audioSinkId, const char * audioOutputDeviceId, const char * audioInputDeviceId);

    ISciterUI & m_sciterUI;
    SystemConfig & m_config;
    SystemModules & m_modules;
    HWINDOW m_parent;
    SciterElement m_page;
    std::shared_ptr<IPageNav> m_pageNav;
    SciterElement m_audioPage;
    std::shared_ptr<IComboBox> m_outputEngine;
    std::shared_ptr<IComboBox> m_audioOutputDevice;
    std::shared_ptr<IComboBox> m_audioInputDevice;
    std::shared_ptr<IComboBox> m_audioMode;
};