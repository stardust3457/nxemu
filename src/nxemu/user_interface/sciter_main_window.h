#pragma once
#include <memory>
#include <nxemu-core/modules/system_modules.h>
#include <nxemu-module-spec/base.h>
#include <sciter_ui.h>
#include <sciter_handler.h>
#include <widgets/menubar.h>
#include "startup_checks.h"

class SystemConfig;
class InputConfig;

class SciterMainWindow :
    public IWindowDestroySink,
    public IMenuBarSink,
    public IRenderWindow,
    public IKeySink,
    public IResizeSink,
    public IClickSink,
    public IMouseUpDownSink,
    public IStateChangeSink,
    public ITimerSink
{
    enum MainMenuID
    {
        // File Menu
        ID_FILE_LOAD_FILE = 1,
        ID_FILE_EXIT,

        // Emulation Menu
        ID_EMULATION_CONTROLLERS,
        ID_EMULATION_CONFIGURE,

        // Recent files
        ID_RECENT_FILE_START,
        ID_RECENT_FILE_END = ID_RECENT_FILE_START + 20,
    };

    enum
    {
        TIMER_UPDATE_INPUT = 5000,
    };

public:
    SciterMainWindow(ISciterUI & sciterUI, const char * windowTitle);
    ~SciterMainWindow();

    void ResetMenu();
    bool Show(void);

    // IRenderWindow
    void * RenderSurface(void) const override;
    float PixelRatio(void) const override; 

private:
    SciterMainWindow(void) = delete;
    SciterMainWindow(const SciterMainWindow &) = delete;
    SciterMainWindow & operator=(const SciterMainWindow &) = delete;

    void CreateRenderWindow(void);
    void SetCaption(const std::string & caption);
    static void EmulationRunning(const char * setting, void * userData);
    static void GameFileChanged(const char * setting, void * userData);
    static void GameNameChanged(const char * setting, void * userData);
    static void RomLoadingChanged(const char * setting, void * userData);
    static void DisplayedFramesChanged(const char * setting, void * userData);
    void ShowLoadingScreen(void);
    void UpdateStatusbar();
    void DismissvolumePopup(SCITER_ELEMENT source, int32_t x, int32_t y);
    void UpdateInputDrivers();

    void OnOpenFile(void);
    void OnFileExit(void);
    void OnSystemConfig(void);
    void OnInputConfig(void);
    void OnRecetGame(uint32_t fileIndex);

    // IWindowDestroySink
    void OnWindowDestroy(HWINDOW hWnd) override;

    // IMenuBarSink
    void OnMenuItem(int32_t id, SCITER_ELEMENT item) override;

    // IKeySink
    bool OnKeyDown(SCITER_ELEMENT element, SCITER_ELEMENT item, SciterKeys keyCode, uint32_t keyboardState) override;
    bool OnKeyUp(SCITER_ELEMENT element, SCITER_ELEMENT item, SciterKeys keyCode, uint32_t keyboardState) override;
    bool OnKeyChar(SCITER_ELEMENT element, SCITER_ELEMENT item, SciterKeys keyCode, uint32_t keyboardState) override;

    // IResizeSink
    bool OnSizeChanged(SCITER_ELEMENT elem) override;

    // IClickSink
    bool OnClick(SCITER_ELEMENT element, SCITER_ELEMENT source, uint32_t reason) override;

    // IMouseUpDownSink
    bool OnMouseUp(SCITER_ELEMENT element, SCITER_ELEMENT source, uint32_t x, uint32_t y) override;
    bool OnMouseDown(SCITER_ELEMENT element, SCITER_ELEMENT source, uint32_t x, uint32_t y) override;

    // IStateChangeSink
    bool OnStateChange(SCITER_ELEMENT elem, uint32_t eventReason, void* data) override;

    // ITimerSink    
    bool OnTimer(SCITER_ELEMENT Element, uint32_t* TimerId) override;

    static void SettingChanged(const char* setting, void* userData);

    ISciterUI & m_sciterUI;
    ISciterWindow * m_window;
    SystemModules m_modules;
    std::vector<VkDeviceRecord> m_vkDeviceRecords;
    std::shared_ptr<IMenuBar> m_menuBar;
    void * m_renderWindow;
    std::string m_windowTitle;
    std::unique_ptr<SystemConfig> m_systemConfig;
    std::unique_ptr<InputConfig> m_inputConfig;
    bool m_volumePopup;
    bool m_emulationRunning;
};
