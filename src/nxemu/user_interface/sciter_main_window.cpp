#include "sciter_main_window.h"
#include "settings/input_config.h"
#include "settings/system_config.h"
#include "settings/ui_settings.h"
#include "user_interface/key_mappings.h"
#include "user_interface/settings/system_config.h"
#include "user_interface/settings/input_config.h"
#include <common/std_string.h>
#include <nxemu-core/settings/identifiers.h>
#include <nxemu-core/settings/settings.h>
#include <nxemu-core/version.h>
#include <nxemu-os/os_settings_identifiers.h>
#include <nxemu-video/video_settings_identifiers.h>
#include <sciter_element.h>
#include <widgets/menubar.h>
#include <yuzu_common/settings_input.h>
#include <yuzu_common/settings.h>
#include <nxemu-module-spec/video.h>
#include <nxemu-module-spec/system_loader.h>

#include <Windows.h>

namespace
{
enum
{
    EVENT_EMULATION_LOADING = 0x2000,
    EVENT_EMULATION_RUNNING = 0x2001,
    EVENT_EMULATION_STOPPED = 0x2002,
    EVENT_EMULATION_FIRST_FRAME = 0x2003,
};
}

SciterMainWindow::SciterMainWindow(ISciterUI & sciterUI, const char * windowTitle) :
    m_sciterUI(sciterUI),
    m_window(nullptr),
    m_renderWindow(nullptr),
    m_windowTitle(windowTitle),
    m_volumePopup(false),
    m_emulationRunning(false)
{
    SettingsStore & settings = SettingsStore::GetInstance();
    settings.RegisterCallback(NXCoreSetting::EmulationRunning, SciterMainWindow::EmulationRunning, this);
    settings.RegisterCallback(NXCoreSetting::EmulationState, SciterMainWindow::EmulationStateChanged, this);
    settings.RegisterCallback(NXCoreSetting::GameFile, SciterMainWindow::GameFileChanged, this);
    settings.RegisterCallback(NXCoreSetting::GameName, SciterMainWindow::GameNameChanged, this);
    settings.RegisterCallback(NXCoreSetting::DisplayedFrames, SciterMainWindow::DisplayedFramesChanged, this);
    settings.RegisterCallback(NXOsSetting::AudioVolume, SciterMainWindow::SettingChanged, this);
}

SciterMainWindow::~SciterMainWindow()
{
    SettingsStore & settings = SettingsStore::GetInstance();
    settings.UnregisterCallback(NXCoreSetting::EmulationRunning, SciterMainWindow::EmulationRunning, this);
    settings.UnregisterCallback(NXCoreSetting::EmulationState, SciterMainWindow::EmulationStateChanged, this);
    settings.UnregisterCallback(NXCoreSetting::GameFile, SciterMainWindow::GameFileChanged, this);
    settings.UnregisterCallback(NXCoreSetting::GameName, SciterMainWindow::GameNameChanged, this);
    settings.UnregisterCallback(NXCoreSetting::DisplayedFrames, SciterMainWindow::DisplayedFramesChanged, this);
    settings.UnregisterCallback(NXOsSetting::AudioVolume, SciterMainWindow::SettingChanged, this);

    settings.SetBool(NXCoreSetting::ShuttingDown, true);
    m_modules.ShutDown();
}

void SciterMainWindow::ResetMenu()
{
    if (m_menuBar == nullptr)
    {
        return;
    }
    MenuBarItemList mainTitleMenu;
    MenuBarItemList fileMenu;
    fileMenu.push_back(MenuBarItem(ID_FILE_LOAD_FILE, "Load File..."));

    Stringlist & recentFiles = uiSettings.recentFiles;
    MenuBarItemList RecentFileMenu;
    if (recentFiles.size() > 0)
    {
        int32_t recentFileIndex = 0;
        for (Stringlist::const_iterator itr = recentFiles.begin(); itr != recentFiles.end(); itr++)
        {
            stdstr_f MenuString("%d %s", recentFileIndex + 1, itr->c_str());
            RecentFileMenu.push_back(MenuBarItem(ID_RECENT_FILE_START + recentFileIndex, MenuString.c_str()));
            recentFileIndex += 1;
        }
        fileMenu.emplace_back(MenuBarItem::SUB_MENU, "Recent File", &RecentFileMenu);
    }

    fileMenu.push_back(MenuBarItem(MenuBarItem::SPLITER));
    fileMenu.push_back(MenuBarItem(ID_FILE_EXIT, "Exit"));
    mainTitleMenu.push_back(MenuBarItem(MenuBarItem::SUB_MENU, "File", &fileMenu));

    MenuBarItemList systemMenu;
    if (m_emulationRunning)
    {
        systemMenu.push_back(MenuBarItem(ID_SYSTEM_STOP, "Stop"));
        mainTitleMenu.push_back(MenuBarItem(MenuBarItem::SUB_MENU, "System", &systemMenu));    
    }

    MenuBarItemList optionsMenu;
    optionsMenu.push_back(MenuBarItem(ID_EMULATION_CONTROLLERS, "Controllers..."));
    optionsMenu.push_back(MenuBarItem(ID_EMULATION_CONFIGURE, "Configure..."));
    mainTitleMenu.push_back(MenuBarItem(MenuBarItem::SUB_MENU, "Options", &optionsMenu));

    m_menuBar->AddSink(this);
    m_menuBar->SetMenuContent(mainTitleMenu);
}

bool SciterMainWindow::Show()
{
    enum
    {
        WINDOW_HEIGHT = 507,
        WINDOW_WIDTH = 760,
    };

    if (!m_sciterUI.WindowCreate(nullptr, "main_window.html", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SUIW_MAIN | SUIW_HIDDEN, m_window))
    {
        return false;
    }
    m_rootElement = m_window->GetRootElement();
    m_sciterUI.AttachHandler(m_rootElement, IID_IKEYSINK, (IKeySink *)this);
    m_sciterUI.AttachHandler(m_rootElement, IID_EVENTSINK, (IEventSink *)this);
    m_window->OnDestroySinkAdd(this);
    m_window->CenterWindow();
    SetCaption(m_windowTitle);

    SciterElement menuElement(m_rootElement.GetElementByID("MainMenu"));
    std::shared_ptr<void> interfacePtr = menuElement.IsValid() ? m_sciterUI.GetElementInterface(menuElement, IID_IMENUBAR) : nullptr;
    if (interfacePtr)
    {
        m_menuBar = std::static_pointer_cast<IMenuBar>(interfacePtr);
        ResetMenu();
    }
    m_sciterUI.UpdateWindow(m_rootElement.GetElementHwnd(true));
    SciterElement::RECT rect = {20, 20, 660, 500};
    SciterElement mainContents(m_rootElement.GetElementByID("MainContents"));
    if (mainContents.IsValid())
    {
        m_sciterUI.AttachHandler(mainContents, IID_IRESIZESINK, (IResizeSink *)this);
        rect = mainContents.GetLocation();
    }

    CreateRenderWindow();
    m_modules.Setup(*this);
    UpdateStatusbar();

    m_sciterUI.AttachHandler(m_rootElement, IID_IMOUSEUPDOWNSINK, (IMouseUpDownSink *)this);
    m_sciterUI.AttachHandler(m_rootElement.GetElementByID("renderer"), IID_ICLICKSINK, (IClickSink *)this);
    m_sciterUI.AttachHandler(m_rootElement.GetElementByID("volume"), IID_ICLICKSINK, (IClickSink *)this);
    m_sciterUI.AttachHandler(m_rootElement.GetElementByID("audioVolume"), IID_ISTATECHANGESINK, (IStateChangeSink *)this);

    m_sciterUI.AttachHandler(m_rootElement, IID_ITIMERSINK, (ITimerSink *)this);
    m_rootElement.SetTimer(25, (uint32_t *)TIMER_UPDATE_INPUT);

    if (!uiSettings.hasBrokenVulkan)
    {
        PopulateVulkanRecords(m_vkDeviceRecords, RenderSurface());
    }
    m_window->Show();
    return true;
}

void SciterMainWindow::ShowConfig(const char * startPage)
{
    m_systemConfig.reset(new SystemConfig(m_sciterUI, m_modules, m_vkDeviceRecords));
    m_systemConfig->Display((void *)m_window->GetHandle(), startPage);
}

void SciterMainWindow::LoadGame(const char * path)
{
    ISystemloader & loader = m_modules.Modules().Systemloader();
    loader.LoadRom(path);
}

void SciterMainWindow::UpdateStatusbar()
{
    SettingsStore & settings = SettingsStore::GetInstance();
    SciterElement renderer(m_rootElement.GetElementByID("renderer"));
    if (renderer.IsValid())
    {
        stdstr_f text("%s", Settings::CanonicalizeEnum((Settings::RendererBackend)settings.GetInt(NXVideoSetting::GraphicsAPI)).c_str());
        renderer.SetHTML((const uint8_t *)text.c_str(), text.size());
    }

    SciterElement volume(m_rootElement.GetElementByID("volume"));
    if (volume.IsValid())
    {
        stdstr_f text("VOLUME: %d %%", settings.GetInt(NXOsSetting::AudioVolume));
        volume.SetHTML((const uint8_t *)text.c_str(), text.size());    
    }
}

void SciterMainWindow::DismissvolumePopup(SCITER_ELEMENT source, int32_t x, int32_t y)
{
    if (!m_volumePopup)
    {
        return;
    }
    if (source == m_rootElement.GetElementByID("volume"))
    {
        return;
    }
    SciterElement volumePopup(m_rootElement.GetElementByID("VolumePopup"));
    SciterElement::RECT rc = volumePopup.GetLocation(SciterElement::ROOT_RELATIVE | SciterElement::BORDER_BOX);
    if (x >= rc.left && y >= rc.top && x <= rc.right && y <= rc.bottom)
    {
        return;
    }
    m_sciterUI.PopupHide(m_rootElement.GetElementByID("VolumePopup"));
    m_volumePopup = false;
}

void SciterMainWindow::UpdateInputDrivers()
{
    if (m_modules.IsValid())
    {
        IOperatingSystem & operatingSystem = m_modules.Modules().OperatingSystem();
        operatingSystem.PumpInputEvents();
    }
}

void SciterMainWindow::CreateRenderWindow()
{
    SciterElement mainContents(m_rootElement.GetElementByID("MainContents"));
    SciterElement::RECT rect = mainContents.GetLocation();
    uint32_t width = rect.right - rect.left;
    uint32_t height = rect.bottom - rect.top;
    m_renderWindow = CreateWindowExW(0, L"Static", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
        rect.left, rect.top, width, height, (HWND)m_window->GetHandle(), nullptr, GetModuleHandle(nullptr), nullptr);
    ShowWindow((HWND)m_renderWindow, SW_HIDE);

    if (m_modules.IsValid())
    {
        IVideo & video = m_modules.Modules().Video();
        video.UpdateFramebufferLayout(rect.right - rect.left, rect.bottom - rect.top);
    }
}

void SciterMainWindow::SetCaption(const std::string & caption)
{
    m_rootElement.Eval(stdstr_f("Window.this.caption = \"%s\";", caption.c_str()).c_str());
    SciterElement captionElement(m_rootElement.FindFirst("[role='window-caption'] > span"));
    if (captionElement.IsValid())
    {
        captionElement.SetHTML((uint8_t *)caption.data(), caption.size());
    }
}

void SciterMainWindow::EmulationRunning(const char * /*setting*/, void * userData)
{
    SciterMainWindow * impl = (SciterMainWindow *)userData;
    SettingsStore & settings = SettingsStore::GetInstance();
    impl->m_emulationRunning = settings.GetBool(NXCoreSetting::EmulationRunning);
    if (settings.GetBool(NXCoreSetting::ShuttingDown))
    {
        return;
    }
    if (impl->m_emulationRunning)
    {
        if (impl->m_renderWindow != nullptr)
        {
            DestroyWindow((HWND)impl->m_renderWindow);
            impl->m_renderWindow = nullptr;
        }
        impl->CreateRenderWindow();
    }
    SciterElement renderer(impl->m_rootElement.GetElementByID("renderer"));
    if (renderer)
    {
        renderer.SetState(impl->m_emulationRunning ? SciterElement::STATE_DISABLED : 0, impl->m_emulationRunning ? 0 : SciterElement::STATE_DISABLED, true);
    }
    impl->ResetMenu();
}

void SciterMainWindow::EmulationStateChanged(const char * /*setting*/, void * userData)
{
    SciterMainWindow * impl = (SciterMainWindow *)userData;
    EmulationState state = (EmulationState)SettingsStore::GetInstance().GetInt(NXCoreSetting::EmulationState);

    if (state == EmulationState::LoadingRom)
    {
        impl->m_rootElement.PostEvent(EVENT_EMULATION_LOADING);
    }
    else if (state == EmulationState::Running)
    {
        impl->m_rootElement.PostEvent(EVENT_EMULATION_RUNNING);
    }
    else if (state == EmulationState::Stopped)
    {
        impl->m_rootElement.PostEvent(EVENT_EMULATION_STOPPED);
    }
}

void SciterMainWindow::GameFileChanged(const char * /*setting*/, void * userData)
{
    SciterMainWindow * impl = (SciterMainWindow *)userData;

    enum
    {
        maxRememberedFiles = 10
    };

    Stringlist & recentFiles = uiSettings.recentFiles;
    std::string gameFile = SettingsStore::GetInstance().GetString(NXCoreSetting::GameFile);
    for (Stringlist::const_iterator itr = recentFiles.begin(); itr != recentFiles.end(); itr++)
    {
        if (_stricmp(gameFile.c_str(), itr->c_str()) != 0)
        {
            continue;
        }
        recentFiles.erase(itr);
        break;
    }
    recentFiles.insert(recentFiles.begin(), gameFile);
    if (recentFiles.size() > maxRememberedFiles)
    {
        recentFiles.resize(maxRememberedFiles);
    }
    impl->ResetMenu();
    SaveUISetting();
}

void SciterMainWindow::GameNameChanged(const char * /*setting*/, void * userData)
{
    SciterMainWindow * impl = (SciterMainWindow *)userData;

    std::string gameName = SettingsStore::GetInstance().GetString(NXCoreSetting::GameName);
    if (gameName.length() > 0)
    {
        std::string caption;
        caption += gameName;
        caption += " | ";
        caption += impl->m_windowTitle;
        impl->SetCaption(caption);
    }
}

void SciterMainWindow::DisplayedFramesChanged(const char * /*setting*/, void * userData)
{
    SciterMainWindow * impl = (SciterMainWindow*)userData;
    impl->m_rootElement.PostEvent(EVENT_EMULATION_FIRST_FRAME);
}

void SciterMainWindow::PreventOSSleep()
{
#ifdef _WIN32
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
#elif defined(HAVE_SDL2)
    SDL_DisableScreenSaver();
#endif
}

void SciterMainWindow::AllowOSSleep()
{
#ifdef _WIN32
    SetThreadExecutionState(ES_CONTINUOUS);
#elif defined(HAVE_SDL2)
    SDL_EnableScreenSaver();
#endif
}

void SciterMainWindow::OnOpenFile()
{
    if (m_modules.IsValid())
    {
        ISystemloader & loader = m_modules.Modules().Systemloader();
        loader.SelectAndLoad((void *)m_window->GetHandle());    
    }
}

void SciterMainWindow::OnFileExit()
{
    m_sciterUI.Stop();
}

void SciterMainWindow::OnStopGame()
{
    if (!m_emulationRunning)
    {
        return;
    }
    AllowOSSleep();
    SettingsStore & settings = SettingsStore::GetInstance();
    settings.SetBool(NXCoreSetting::EmulationRunning, false);
}

void SciterMainWindow::OnSystemConfig()
{
    ShowConfig(nullptr);
}

void SciterMainWindow::OnInputConfig()
{
    m_inputConfig.reset(new InputConfig(m_sciterUI, m_modules));
    m_inputConfig->Display((void*)m_window->GetHandle());
}
        
void SciterMainWindow::OnRecetGame(uint32_t fileIndex)
{
    Stringlist & recentFiles = uiSettings.recentFiles;
    if (m_modules.IsValid() && fileIndex < recentFiles.size())
    {
        ISystemloader & loader = m_modules.Modules().Systemloader();
        loader.LoadRom(recentFiles[fileIndex].c_str());
    }
}

void SciterMainWindow::OnWindowDestroy(HWINDOW /*hWnd*/)
{
    m_sciterUI.Stop();
}

void SciterMainWindow::OnMenuItem(int32_t id, SCITER_ELEMENT /*item*/)
{
    switch (id)
    {
    case ID_FILE_LOAD_FILE: OnOpenFile(); break;
    case ID_FILE_EXIT: OnFileExit(); break;
    case ID_SYSTEM_STOP: OnStopGame(); break;
    case ID_EMULATION_CONTROLLERS: OnInputConfig(); break;
    case ID_EMULATION_CONFIGURE: OnSystemConfig(); break;
    default:
        if (id >= ID_RECENT_FILE_START && id <= ID_RECENT_FILE_END)
        {
            OnRecetGame(id - ID_RECENT_FILE_START);
        }
    }
}

void * SciterMainWindow::RenderSurface() const
{
    return m_renderWindow;
}

float SciterMainWindow::PixelRatio() const
{
    return 1.0;
}

bool SciterMainWindow::OnKeyDown(SCITER_ELEMENT /*element*/, SCITER_ELEMENT /*item*/, SciterKeys keyCode, uint32_t /*keyboardState*/)
{
    if (m_modules.IsValid())
    {
        IOperatingSystem & operatingSystem = m_modules.Modules().OperatingSystem();
        int keyIndex = SciterKeyToSwitchKey(keyCode);
        if (keyIndex != 0)
        {
            operatingSystem.KeyboardKeyPress(0, keyIndex, SciterKeyToVKCode(keyCode));
        }
    }
    return false;
}

bool SciterMainWindow::OnKeyUp(SCITER_ELEMENT /*element*/, SCITER_ELEMENT /*item*/, SciterKeys keyCode, uint32_t /*keyboardState*/)
{
    if (m_modules.IsValid())
    {
        IOperatingSystem & operatingSystem = m_modules.Modules().OperatingSystem();
        int keyIndex = SciterKeyToSwitchKey(keyCode);
        if (keyIndex != 0)
        {
            operatingSystem.KeyboardKeyRelease(0, keyIndex, SciterKeyToVKCode(keyCode));
        }
    }
    return false;
}

bool SciterMainWindow::OnKeyChar(SCITER_ELEMENT /*element*/, SCITER_ELEMENT /*item*/, SciterKeys /*keyCode*/, uint32_t /*keyboardState*/)
{
    return false;
}

bool SciterMainWindow::OnSizeChanged(SCITER_ELEMENT elem)
{
    SciterElement rootElement(m_window->GetRootElement());
    if (elem == rootElement.GetElementByID("MainContents"))
    {
        SciterElement::RECT rect = SciterElement(elem).GetLocation();
        uint32_t width = rect.right - rect.left;
        uint32_t height = rect.bottom - rect.top;
        MoveWindow((HWND)m_renderWindow, rect.left, rect.top, width, height, false);
        if (m_modules.IsValid())
        {
            IVideo & video = m_modules.Modules().Video();
            video.UpdateFramebufferLayout(width, height);
        }
    }
    return false;
}

bool SciterMainWindow::OnClick(SCITER_ELEMENT /*element*/, SCITER_ELEMENT source, uint32_t /*reason*/)
{
    SciterElement rootElement(m_window->GetRootElement());
    if (source == rootElement.GetElementByID("renderer"))
    {
        SettingsStore & settings = SettingsStore::GetInstance();
        Settings::RendererBackend graphicsAPI = (Settings::RendererBackend)settings.GetInt(NXVideoSetting::GraphicsAPI);
        if (graphicsAPI == Settings::RendererBackend::Vulkan)
        {
            graphicsAPI = Settings::RendererBackend::OpenGL;
        }
        else if (graphicsAPI == Settings::RendererBackend::OpenGL)
        {
            graphicsAPI = Settings::RendererBackend::Vulkan;
        }
        settings.SetInt(NXVideoSetting::GraphicsAPI, (int32_t)graphicsAPI);
        stdstr_f text("%s", Settings::CanonicalizeEnum((Settings::RendererBackend)settings.GetInt(NXVideoSetting::GraphicsAPI)).c_str());
        SciterElement(source).SetHTML((const uint8_t*)text.c_str(), text.size());
        if (m_modules.IsValid())
        {
            m_modules.FlushSettings();
        }
    }
    else if (source == rootElement.GetElementByID("volume"))
    {
        m_sciterUI.PopupShow(rootElement.GetElementByID("VolumePopup"), rootElement.GetElementByID("volume"), 8);
        m_volumePopup = true;
    }
    return true;
}

bool SciterMainWindow::OnMouseUp(SCITER_ELEMENT /*element*/, SCITER_ELEMENT source, uint32_t x, uint32_t y)
{
    DismissvolumePopup(source, x, y);
    return false;
}

bool SciterMainWindow::OnMouseDown(SCITER_ELEMENT /*element*/, SCITER_ELEMENT source, uint32_t x, uint32_t y)
{
    DismissvolumePopup(source, x, y);
    return false;
}

bool SciterMainWindow::OnStateChange(SCITER_ELEMENT elem, uint32_t /*eventReason*/, void * /*data*/)
{
    SciterElement rootElement(m_window->GetRootElement());
    if (rootElement.GetElementByID("audioVolume") == elem)
    {
        SciterValue value = SciterElement(elem).GetValue();
        if (value.isInt())
        {
            SettingsStore& settings = SettingsStore::GetInstance();
            settings.SetInt(NXOsSetting::AudioVolume, value.GetValueInt());
            if (m_modules.IsValid())
            {
                m_modules.FlushSettings();
            }
        }
    }
    return false;
}

void SciterMainWindow::SettingChanged(const char * setting, void * userData)
{
    SciterMainWindow * impl = (SciterMainWindow*)userData;
    if (strcmp(setting, NXOsSetting::AudioVolume) == 0)
    {
        impl->UpdateStatusbar();
    }
}

bool SciterMainWindow::OnTimer(SCITER_ELEMENT /*element*/, uint32_t * timerId)
{
    if (timerId == (uint32_t*)TIMER_UPDATE_INPUT)
    {
        UpdateInputDrivers();
    }
    return true;
}

bool SciterMainWindow::OnEvent(SCITER_ELEMENT /*element*/, SCITER_ELEMENT /*source*/, uint32_t event_code, uint64_t /*reason*/)
{
    if (event_code == EVENT_EMULATION_LOADING)
    {
        SciterElement LoadingPanel(m_rootElement.GetElementByID("LoadingPanel"));
        if (LoadingPanel.IsValid())
        {
            LoadingPanel.SetStyleAttribute("display", "block");
        }
    }
    else if (event_code == EVENT_EMULATION_RUNNING)
    {
        PreventOSSleep();
    }
    else if (event_code == EVENT_EMULATION_STOPPED)
    {
        ShowWindow((HWND)m_renderWindow, SW_HIDE);
        SciterElement LoadingPanel(m_rootElement.GetElementByID("LoadingPanel"));
        if (LoadingPanel.IsValid())
        {
            LoadingPanel.SetStyleAttribute("display", "none");
        }
    }
    else if (event_code == EVENT_EMULATION_FIRST_FRAME)
    {
        SettingsStore & settings = SettingsStore::GetInstance();
        if (settings.GetBool(NXCoreSetting::DisplayedFrames))
        {
            ShowWindow((HWND)m_renderWindow, SW_SHOW);        
            SciterElement LoadingPanel(m_rootElement.GetElementByID("LoadingPanel"));
            if (LoadingPanel.IsValid())
            {
                LoadingPanel.SetStyleAttribute("display", "none");
            }
        }
    }
    return false;
}
