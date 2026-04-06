#pragma once
#include <memory>
#include <nxemu-core/modules/system_modules.h>
#include <nxemu-module-spec/base.h>
#include <sciter_ui.h>
#include <sciter_handler.h>
#include <sciter_element.h>
#include <widgets/menubar.h>
#include "startup_checks.h"
#include "user_interface/widgets/rom_browser.h"

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
    public ITimerSink,
    public IEventSink
{
    enum MainMenuID
    {
        // File Menu
        ID_FILE_LOAD_FILE = 1,
        ID_FILE_EXIT,

        // System Menu
        ID_SYSTEM_STOP,
        
        // Emulation Menu
        ID_EMULATION_CONTROLLERS,
        ID_EMULATION_CONFIGURE,

        // Recent files
        ID_RECENT_FILE_START,
        ID_RECENT_FILE_END = ID_RECENT_FILE_START + 20,
    };

    enum
    {
        TIMER_UPDATE_UI = 5000,
        TIMER_UPDATE_INPUT,
        TIMER_UPDATE_STATUS,
    };

public:
    SciterMainWindow(ISciterUI & sciterUI, const char * windowTitle);
    ~SciterMainWindow();

    void ResetMenu();
    bool Show();
    void ShowConfig(const char * startPage);
    void LoadGame(const char * path);

    // IRenderWindow
    void * RenderSurface() const override;
    float PixelRatio() const override; 

private:
    SciterMainWindow() = delete;
    SciterMainWindow(const SciterMainWindow &) = delete;
    SciterMainWindow & operator=(const SciterMainWindow &) = delete;

    void CreateRenderWindow();
    void SetCaption(const std::string & caption);
    static void EmulationRunning(const char * setting, void * userData);
    static void EmulationStateChanged(const char * setting, void * userData);
    static void GameFileChanged(const char * setting, void * userData);
    static void GameNameChanged(const char * setting, void * userData);
    static void DisplayedFramesChanged(const char * setting, void * userData);
    static void HotKeysChanged(const char * setting, void * userData);
    void UpdateStatusbar();
    void DismissvolumePopup(SCITER_ELEMENT source, int32_t x, int32_t y);
    void UpdateInputDrivers();
    void PreventOSSleep();
    void AllowOSSleep();

    void OnOpenFile();
    void OnFileExit();
    void OnStopGame();
    void OnSystemConfig();
    void OnInputConfig();
    void OnRecetGame(uint32_t fileIndex);
    void UpdateStatusBar();
    const MenuBarAccelerator * HotkeyAccelerator(const char * name);
    const char * IsMenuBarAccelerator(uint32_t keyCode, uint32_t keyboardState);
    bool ProcessMenuBarAccelerator(const char * hotkeyId);

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

    // IEventSink
    bool OnEvent(SCITER_ELEMENT element, SCITER_ELEMENT source, uint32_t event_code, uint64_t reason);

    static void SettingChanged(const char* setting, void* userData);

    ISciterUI & m_sciterUI;
    ISciterWindow * m_window;
    SciterElement m_rootElement;
    SystemModules m_modules;
    std::vector<VkDeviceRecord> m_vkDeviceRecords;
    std::shared_ptr<IMenuBar> m_menuBar;
    std::shared_ptr<IRomBrowser> m_romBrowser;
    void * m_renderWindow;
    std::string m_windowTitle;
    std::unique_ptr<SystemConfig> m_systemConfig;
    std::unique_ptr<InputConfig> m_inputConfig;
    float m_resolutionUpFactor;
    bool m_volumePopup;
    bool m_useMultiCore;
    bool m_useSpeedLimit;
    uint32_t m_speedLimit;
    bool m_emulationRunning;
};
