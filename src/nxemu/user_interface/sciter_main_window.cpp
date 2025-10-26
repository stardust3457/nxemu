#include "sciter_main_window.h"
#include "settings/input_config.h"
#include "settings/system_config.h"
#include "settings/ui_settings.h"
#include "user_interface/settings/system_config.h"
#include "user_interface/settings/input_config.h"
#include <common/std_string.h>
#include <nxemu-core/machine/switch_system.h>
#include <nxemu-core/settings/identifiers.h>
#include <nxemu-core/settings/settings.h>
#include <nxemu-core/version.h>
#include <nxemu-os/os_settings_identifiers.h>
#include <nxemu-video/video_settings_identifiers.h>
#include <sciter_element.h>
#include <widgets/menubar.h>
#include <yuzu_common/settings_input.h>
#include <yuzu_common/settings.h>

#include <Windows.h>

SciterMainWindow::SciterMainWindow(ISciterUI & sciterUI, const char * windowTitle) :
    m_sciterUI(sciterUI),
    m_window(nullptr),
    m_renderWindow(nullptr),
    m_windowTitle(windowTitle),
    m_volumePopup(false)
{
    SettingsStore & settings = SettingsStore::GetInstance();
    settings.RegisterCallback(NXCoreSetting::EmulationRunning, SciterMainWindow::EmulationRunning, this);
    settings.RegisterCallback(NXCoreSetting::GameFile, SciterMainWindow::GameFileChanged, this);
    settings.RegisterCallback(NXCoreSetting::GameName, SciterMainWindow::GameNameChanged, this);
    settings.RegisterCallback(NXCoreSetting::RomLoading, SciterMainWindow::RomLoadingChanged, this);
    settings.RegisterCallback(NXCoreSetting::DisplayedFrames, SciterMainWindow::DisplayedFramesChanged, this);
    settings.RegisterCallback(NXOsSetting::AudioVolume, SciterMainWindow::SettingChanged, this);
}

SciterMainWindow::~SciterMainWindow()
{
}

void SciterMainWindow::ResetMenu()
{
    if (m_menuBar == nullptr)
    {
        return;
    }
    MenuBarItemList fileMenu;
    fileMenu.push_back(MenuBarItem(ID_FILE_LOAD_FILE, "Load File..."));
    fileMenu.push_back(MenuBarItem(MenuBarItem::SPLITER));

    Stringlist & recentFiles = uiSettings.recentFiles;
    MenuBarItemList RecentGameMenu;
    if (recentFiles.size() > 0)
    {
        int32_t recentFileIndex = 0;
        for (Stringlist::const_iterator itr = recentFiles.begin(); itr != recentFiles.end(); itr++)
        {
            stdstr_f MenuString("%d %s", recentFileIndex + 1, itr->c_str());
            RecentGameMenu.push_back(MenuBarItem(ID_RECENT_FILE_START + recentFileIndex, MenuString.c_str()));
            recentFileIndex += 1;
        }
        fileMenu.emplace_back(MenuBarItem::SUB_MENU, "Recent Games", &RecentGameMenu);
        fileMenu.push_back(MenuBarItem(MenuBarItem::SPLITER));
    }
    fileMenu.push_back(MenuBarItem(ID_FILE_EXIT, "Exit"));

    MenuBarItemList emulationMenu;
    emulationMenu.push_back(MenuBarItem(ID_EMULATION_CONTROLLERS, "Controllers..."));
    emulationMenu.push_back(MenuBarItem(ID_EMULATION_CONFIGURE, "Configure..."));

    MenuBarItemList mainTitleMenu;
    mainTitleMenu.push_back(MenuBarItem(MenuBarItem::SUB_MENU, "File", &fileMenu));
    mainTitleMenu.push_back(MenuBarItem(MenuBarItem::SUB_MENU, "Emulation", &emulationMenu));

    m_menuBar->AddSink(this);
    m_menuBar->SetMenuContent(mainTitleMenu);
}

bool SciterMainWindow::Show(void)
{
    enum
    {
        WINDOW_HEIGHT = 507,
        WINDOW_WIDTH = 760,
    };

    if (!m_sciterUI.WindowCreate(nullptr, "main_window.html", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SUIW_MAIN, m_window))
    {
        return false;
    }
    SciterElement rootElement(m_window->GetRootElement());
    m_sciterUI.AttachHandler(rootElement, IID_IKEYSINK, (IKeySink*)this);
    m_window->OnDestroySinkAdd(this);
    m_window->CenterWindow();
    SetCaption(m_windowTitle);

    SciterElement menuElement(rootElement.GetElementByID("MainMenu"));
    std::shared_ptr<void> interfacePtr = menuElement.IsValid() ? m_sciterUI.GetElementInterface(menuElement, IID_IMENUBAR) : nullptr;
    if (interfacePtr)
    {
        m_menuBar = std::static_pointer_cast<IMenuBar>(interfacePtr);
        ResetMenu();
    }
    m_sciterUI.UpdateWindow(rootElement.GetElementHwnd(true));
    SciterElement::RECT rect = {20, 20, 660, 500};
    SciterElement mainContents(rootElement.GetElementByID("MainContents"));
    if (mainContents.IsValid())
    {
        m_sciterUI.AttachHandler(mainContents, IID_IRESIZESINK, (IResizeSink*)this);
        rect = mainContents.GetLocation();
    }

    CreateRenderWindow();
    SwitchSystem::Create(*this);

    UpdateStatusbar();
    m_sciterUI.AttachHandler(rootElement, IID_IMOUSEUPDOWNSINK, (IMouseUpDownSink*)this);
    m_sciterUI.AttachHandler(rootElement.GetElementByID("renderer"), IID_ICLICKSINK, (IClickSink*)this);
    m_sciterUI.AttachHandler(rootElement.GetElementByID("volume"), IID_ICLICKSINK, (IClickSink*)this);
    m_sciterUI.AttachHandler(rootElement.GetElementByID("audioVolume"), IID_ISTATECHANGESINK, (IStateChangeSink*)this);

    m_sciterUI.AttachHandler(rootElement, IID_ITIMERSINK, (ITimerSink*)this);
    rootElement.SetTimer(25, (uint32_t*)TIMER_UPDATE_INPUT);

    if (!uiSettings.hasBrokenVulkan)
    {
        PopulateVulkanRecords(m_vkDeviceRecords, RenderSurface());
    }
    return true;
}

void SciterMainWindow::ShowLoadingScreen()
{
    SciterElement rootElement(m_window->GetRootElement());
    SciterElement mainContents(rootElement.GetElementByID("MainContents"));
    if (mainContents.IsValid())
    {
        m_sciterUI.SetElementHtmlFromResource(mainContents, "loading.html");
    }
}

int32_t SciterMainWindow::SciterKeyToSwitchKey(SciterKeys key)
{
    static const std::unordered_map<uint32_t, int32_t> keyMappings = {
        {'A', InputSettings::NativeKeyboard::A},
        {'B', InputSettings::NativeKeyboard::B},
        {'C', InputSettings::NativeKeyboard::C},
        {'D', InputSettings::NativeKeyboard::D},
        {'E', InputSettings::NativeKeyboard::E},
        {'F', InputSettings::NativeKeyboard::F},
        {'G', InputSettings::NativeKeyboard::G},
        {'H', InputSettings::NativeKeyboard::H},
        {'I', InputSettings::NativeKeyboard::I},
        {'J', InputSettings::NativeKeyboard::J},
        {'K', InputSettings::NativeKeyboard::K},
        {'L', InputSettings::NativeKeyboard::L},
        {'M', InputSettings::NativeKeyboard::M},
        {'N', InputSettings::NativeKeyboard::N},
        {'O', InputSettings::NativeKeyboard::O},
        {'P', InputSettings::NativeKeyboard::P},
        {'Q', InputSettings::NativeKeyboard::Q},
        {'R', InputSettings::NativeKeyboard::R},
        {'S', InputSettings::NativeKeyboard::S},
        {'T', InputSettings::NativeKeyboard::T},
        {'U', InputSettings::NativeKeyboard::U},
        {'v', InputSettings::NativeKeyboard::V},
        {'W', InputSettings::NativeKeyboard::W},
        {'X', InputSettings::NativeKeyboard::X},
        {'Y', InputSettings::NativeKeyboard::Y},
        {'Z', InputSettings::NativeKeyboard::Z},
        {'1', InputSettings::NativeKeyboard::N1},
        {'2', InputSettings::NativeKeyboard::N2},
        {'3', InputSettings::NativeKeyboard::N3},
        {'4', InputSettings::NativeKeyboard::N4},
        {'5', InputSettings::NativeKeyboard::N5},
        {'6', InputSettings::NativeKeyboard::N6},
        {'7', InputSettings::NativeKeyboard::N7},
        {'8', InputSettings::NativeKeyboard::N8},
        {'9', InputSettings::NativeKeyboard::N9},
        {'0', InputSettings::NativeKeyboard::N0},
        {SCITER_KEY_ENTER, InputSettings::NativeKeyboard::Return},
        {SCITER_KEY_ESCAPE, InputSettings::NativeKeyboard::Escape},
        {SCITER_KEY_TAB, InputSettings::NativeKeyboard::Tab},
        {SCITER_KEY_BACKSPACE, InputSettings::NativeKeyboard::Space},
        {SCITER_KEY_F1, InputSettings::NativeKeyboard::F1},
        {SCITER_KEY_F2, InputSettings::NativeKeyboard::F2},
        {SCITER_KEY_F3, InputSettings::NativeKeyboard::F3},
        {SCITER_KEY_F4, InputSettings::NativeKeyboard::F4},
        {SCITER_KEY_F5, InputSettings::NativeKeyboard::F5},
        {SCITER_KEY_F6, InputSettings::NativeKeyboard::F6},
        {SCITER_KEY_F7, InputSettings::NativeKeyboard::F7},
        {SCITER_KEY_F8, InputSettings::NativeKeyboard::F8},
        {SCITER_KEY_F9, InputSettings::NativeKeyboard::F9},
        {SCITER_KEY_F10, InputSettings::NativeKeyboard::F10},
        {SCITER_KEY_F11, InputSettings::NativeKeyboard::F11},
        {SCITER_KEY_F12, InputSettings::NativeKeyboard::F12},
        {SCITER_KEY_RIGHT, InputSettings::NativeKeyboard::Right},
        {SCITER_KEY_LEFT, InputSettings::NativeKeyboard::Left},
        {SCITER_KEY_DOWN, InputSettings::NativeKeyboard::Down},
        {SCITER_KEY_UP, InputSettings::NativeKeyboard::Up},
        {SCITER_KEY_F13, InputSettings::NativeKeyboard::F13},
        {SCITER_KEY_F14, InputSettings::NativeKeyboard::F14},
        {SCITER_KEY_F15, InputSettings::NativeKeyboard::F15},
        {SCITER_KEY_F16, InputSettings::NativeKeyboard::F16},
        {SCITER_KEY_F17, InputSettings::NativeKeyboard::F17},
        {SCITER_KEY_F18, InputSettings::NativeKeyboard::F18},
        {SCITER_KEY_F19, InputSettings::NativeKeyboard::F19},
        {SCITER_KEY_F20, InputSettings::NativeKeyboard::F20},
        {SCITER_KEY_F21, InputSettings::NativeKeyboard::F21},
        {SCITER_KEY_F22, InputSettings::NativeKeyboard::F22},
        {SCITER_KEY_F23, InputSettings::NativeKeyboard::F23},
        {SCITER_KEY_F24, InputSettings::NativeKeyboard::F24},
    };

    if (keyMappings.find(key) != keyMappings.end())
    {
        return keyMappings.at(key);
    }
    return 0;
}

int32_t SciterMainWindow::SciterKeyToVKCode(SciterKeys key)
{
    static const std::unordered_map<uint32_t, int32_t> keyMappings = {
        {'A', 'A'},
        {'B', 'B'},
        {'C', 'C'},
        {'D', 'D'},
        {'E', 'E'},
        {'F', 'F'},
        {'G', 'G'},
        {'H', 'H'},
        {'I', 'I'},
        {'J', 'J'},
        {'K', 'K'},
        {'L', 'L'},
        {'M', 'M'},
        {'N', 'N'},
        {'O', 'O'},
        {'P', 'P'},
        {'Q', 'Q'},
        {'R', 'R'},
        {'S', 'S'},
        {'T', 'T'},
        {'U', 'U'},
        {'v', 'V'},
        {'W', 'W'},
        {'X', 'X'},
        {'Y', 'Y'},
        {'Z', 'Z'},
        {'1', '1'},
        {'2', '2'},
        {'3', '3'},
        {'4', '4'},
        {'5', '5'},
        {'6', '6'},
        {'7', '7'},
        {'8', '8'},
        {'9', '9'},
        {'0', '0'},
        {SCITER_KEY_ENTER, VK_RETURN},
        {SCITER_KEY_ESCAPE, VK_ESCAPE},
        {SCITER_KEY_TAB, VK_TAB},
        {SCITER_KEY_BACKSPACE, VK_SPACE},
        {SCITER_KEY_F1, VK_F1},
        {SCITER_KEY_F2, VK_F2},
        {SCITER_KEY_F3, VK_F3},
        {SCITER_KEY_F4, VK_F4},
        {SCITER_KEY_F5, VK_F5},
        {SCITER_KEY_F6, VK_F6},
        {SCITER_KEY_F7, VK_F7},
        {SCITER_KEY_F8, VK_F8},
        {SCITER_KEY_F9, VK_F9},
        {SCITER_KEY_F10, VK_F10},
        {SCITER_KEY_F11, VK_F11},
        {SCITER_KEY_F12, VK_F12},
        {SCITER_KEY_RIGHT, VK_RIGHT},
        {SCITER_KEY_LEFT, VK_LEFT},
        {SCITER_KEY_DOWN, VK_DOWN},
        {SCITER_KEY_UP, VK_UP},
        {SCITER_KEY_F13, VK_F13},
        {SCITER_KEY_F14, VK_F14},
        {SCITER_KEY_F15, VK_F15},
        {SCITER_KEY_F16, VK_F16},
        {SCITER_KEY_F17, VK_F17},
        {SCITER_KEY_F18, VK_F18},
        {SCITER_KEY_F19, VK_F19},
        {SCITER_KEY_F20, VK_F20},
        {SCITER_KEY_F21, VK_F21},
        {SCITER_KEY_F22, VK_F22},
        {SCITER_KEY_F23, VK_F23},
        {SCITER_KEY_F24, VK_F24},
    };

    if (keyMappings.find(key) != keyMappings.end())
    {
        return keyMappings.at(key);
    }
    return 0;
}

void SciterMainWindow::UpdateStatusbar()
{
    SettingsStore & settings = SettingsStore::GetInstance();
    SciterElement rootElement(m_window->GetRootElement());
    SciterElement renderer(rootElement.GetElementByID("renderer"));
    stdstr_f text("%s", Settings::CanonicalizeEnum((Settings::RendererBackend)settings.GetInt(NXVideoSetting::GraphicsAPI)).c_str());
    renderer.SetHTML((const uint8_t*)text.c_str(), text.size());

    SciterElement volume(rootElement.GetElementByID("volume"));
    text.Format("VOLUME: %d %%", settings.GetInt(NXOsSetting::AudioVolume));
    volume.SetHTML((const uint8_t *)text.c_str(), text.size());
}

void SciterMainWindow::DismissvolumePopup(SCITER_ELEMENT source, int32_t x, int32_t y)
{
    if (!m_volumePopup)
    {
        return;
    }
    SciterElement rootElement(m_window->GetRootElement());
    if (source == rootElement.GetElementByID("volume"))
    {
        return;
    }
    SciterElement volumePopup(rootElement.GetElementByID("VolumePopup"));
    SciterElement::RECT rc = volumePopup.GetLocation(SciterElement::ROOT_RELATIVE | SciterElement::BORDER_BOX);
    if (x >= rc.left && y >= rc.top && x <= rc.right && y <= rc.bottom)
    {
        return;
    }
    m_sciterUI.PopupHide(rootElement.GetElementByID("VolumePopup"));
    m_volumePopup = false;
}

void SciterMainWindow::UpdateInputDrivers()
{
    SwitchSystem * system = SwitchSystem::GetInstance();
    if (system != nullptr)
    {
        IOperatingSystem & operatingSystem = system->OperatingSystem();
        operatingSystem.PumpInputEvents();
    }
}

void SciterMainWindow::CreateRenderWindow(void)
{
    SciterElement rootElement(m_window->GetRootElement());
    SciterElement mainContents(rootElement.GetElementByID("MainContents"));
    SciterElement::RECT rect = mainContents.GetLocation();
    uint32_t width = rect.right - rect.left;
    uint32_t height = rect.bottom - rect.top;
    m_renderWindow = CreateWindowExW(0, L"Static", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
        rect.left, rect.top, width, height, (HWND)m_window->GetHandle(), nullptr, GetModuleHandle(nullptr), nullptr);
    ShowWindow((HWND)m_renderWindow, SW_HIDE);

    SwitchSystem * system = SwitchSystem::GetInstance();
    if (system != nullptr)
    {
        IVideo & video = system->Video();
        video.UpdateFramebufferLayout(rect.right - rect.left, rect.bottom - rect.top);
    }
}

void SciterMainWindow::SetCaption(const std::string & caption)
{
    SciterElement rootElement(m_window->GetRootElement());
    rootElement.Eval(stdstr_f("Window.this.caption = \"%s\";", caption.c_str()).c_str());
    SciterElement captionElement(rootElement.FindFirst("[role='window-caption'] > span"));
    if (captionElement.IsValid())
    {
        captionElement.SetHTML((uint8_t *)caption.data(), caption.size());
    }
}

void SciterMainWindow::EmulationRunning(const char * /*setting*/, void * userData)
{
    bool emulationRunning = SettingsStore::GetInstance().GetBool(NXCoreSetting::EmulationRunning);
    SciterMainWindow * impl = (SciterMainWindow*)userData;
    if (emulationRunning)
    {
        if (impl->m_renderWindow != nullptr)
        {
            DestroyWindow((HWND)impl->m_renderWindow);
            impl->m_renderWindow = nullptr;
        }
        impl->CreateRenderWindow();
    }
    SciterElement rootElement(impl->m_window->GetRootElement());
    SciterElement renderer(rootElement.GetElementByID("renderer"));
    if (renderer)
    {
        renderer.SetState(emulationRunning ? SciterElement::STATE_DISABLED : 0, emulationRunning ? 0 : SciterElement::STATE_DISABLED, true);
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

void SciterMainWindow::RomLoadingChanged(const char * /*setting*/, void * userData)
{
    SciterMainWindow * impl = (SciterMainWindow*)userData;

    bool loading = SettingsStore::GetInstance().GetBool(NXCoreSetting::RomLoading);
    if (loading)
    {
        impl->ShowLoadingScreen();
    }
    else if (strlen(SettingsStore::GetInstance().GetString(NXCoreSetting::GameFile)) == 0)
    {
        SciterElement rootElement(impl->m_window->GetRootElement());
        SciterElement mainContents(rootElement.GetElementByID("MainContents"));
        if (mainContents.IsValid())
        {
            mainContents.SetHTML((uint8_t*)"", 0, SciterElement::SIH_REPLACE_CONTENT);
        }
    }
}

void SciterMainWindow::DisplayedFramesChanged(const char * /*setting*/, void * userData)
{
    SciterMainWindow * impl = (SciterMainWindow*)userData;

    SciterElement rootElement(impl->m_window->GetRootElement());
    SciterElement mainContents(rootElement.GetElementByID("MainContents"));
    if (mainContents.IsValid())
    {
        mainContents.SetHTML((uint8_t *)"", 0, SciterElement::SIH_REPLACE_CONTENT);
    }
    ShowWindow((HWND)impl->m_renderWindow, SW_SHOW);
}

void SciterMainWindow::OnOpenFile(void)
{
    SwitchSystem * system = SwitchSystem::GetInstance();
    system->Systemloader().SelectAndLoad((void *)m_window->GetHandle());
}

void SciterMainWindow::OnFileExit(void)
{
    m_sciterUI.Stop();
}

void SciterMainWindow::OnSystemConfig(void)
{
    m_systemConfig.reset(new SystemConfig(m_sciterUI, m_vkDeviceRecords));
    m_systemConfig->Display((void*)m_window->GetHandle());
}

void SciterMainWindow::OnInputConfig(void)
{
    m_inputConfig.reset(new InputConfig(m_sciterUI));
    m_inputConfig->Display((void*)m_window->GetHandle());
}
        
void SciterMainWindow::OnRecetGame(uint32_t fileIndex)
{
    Stringlist & recentFiles = uiSettings.recentFiles;
    if (fileIndex < recentFiles.size())
    {
        SwitchSystem* system = SwitchSystem::GetInstance();
        system->Systemloader().LoadRom(recentFiles[fileIndex].c_str());
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
    case ID_EMULATION_CONTROLLERS: OnInputConfig(); break;
    case ID_EMULATION_CONFIGURE: OnSystemConfig(); break;
    default:
        if (id >= ID_RECENT_FILE_START && id <= ID_RECENT_FILE_END)
        {
            OnRecetGame(id - ID_RECENT_FILE_START);
        }
    }
}

void * SciterMainWindow::RenderSurface(void) const
{
    return m_renderWindow;
}

float SciterMainWindow::PixelRatio(void) const
{
    return 1.0;
}

bool SciterMainWindow::OnKeyDown(SCITER_ELEMENT /*element*/, SCITER_ELEMENT /*item*/, SciterKeys keyCode, uint32_t /*keyboardState*/)
{
    SwitchSystem* system = SwitchSystem::GetInstance();
    if (system != nullptr)
    {
        IOperatingSystem& OperatingSystem = system->OperatingSystem();
        int keyIndex = SciterKeyToSwitchKey(keyCode);
        if (keyIndex != 0)
        {
            OperatingSystem.KeyboardKeyPress(0, keyIndex, SciterKeyToVKCode(keyCode));
        }
    }
    return false;
}

bool SciterMainWindow::OnKeyUp(SCITER_ELEMENT /*element*/, SCITER_ELEMENT /*item*/, SciterKeys keyCode, uint32_t /*keyboardState*/)
{
    SwitchSystem* system = SwitchSystem::GetInstance();
    if (system != nullptr)
    {
        IOperatingSystem& OperatingSystem = system->OperatingSystem();
        int keyIndex = SciterKeyToSwitchKey(keyCode);
        if (keyIndex != 0)
        {
            OperatingSystem.KeyboardKeyRelease(0, keyIndex, SciterKeyToVKCode(keyCode));
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
        SwitchSystem * system = SwitchSystem::GetInstance();
        if (system != nullptr)
        {
            IVideo & video = system->Video();
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
        SwitchSystem * system = SwitchSystem::GetInstance();
        if (system != nullptr)
        {
            system->FlushSettings();
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
            SwitchSystem * system = SwitchSystem::GetInstance();
            if (system != nullptr)
            {
                system->FlushSettings();
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

bool SciterMainWindow::OnTimer(SCITER_ELEMENT Element, uint32_t* TimerId)
{
    if (TimerId == (uint32_t*)TIMER_UPDATE_INPUT)
    {
        UpdateInputDrivers();
    }
    return true;
}
