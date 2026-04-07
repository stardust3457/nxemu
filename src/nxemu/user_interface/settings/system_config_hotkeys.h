#pragma once
#include "settings/ui_settings.h"
#include <sciter_element.h>
#include <sciter_handler.h>
#include <sciter_ui.h>
#include <string>
#include <widgets/page_nav.h>
#include <widgets/menubar.h>

class SystemConfig;

class SystemConfigHotkeys :
    public IDoubleClickSink,
    public IKeySink
{
public:
    SystemConfigHotkeys(ISciterUI & sciterUI, ISciterWindow & window, SciterElement page);
    ~SystemConfigHotkeys();

    void SaveSetting();

private:
    SystemConfigHotkeys() = delete;
    SystemConfigHotkeys(const SystemConfigHotkeys &) = delete;
    SystemConfigHotkeys & operator=(const SystemConfigHotkeys &) = delete;

    void BuildHotkeysTable();
    void StartCapture(const std::string & hotkeyId);
    void EndCapture(bool cancelled);
    void UpdateBindingDisplay();
    static bool AcceleratorsEqual(const MenuBarAccelerator & a, const MenuBarAccelerator & b);
    bool ConflictsWithOther(const MenuBarAccelerator & candidate, const std::string & assigningHotkeyId) const;

    bool OnDoubleClick(SCITER_ELEMENT element, SCITER_ELEMENT source) override;
    bool OnKeyDown(SCITER_ELEMENT element, SCITER_ELEMENT target, SciterKeys keyCode, uint32_t keyboardState) override;
    bool OnKeyUp(SCITER_ELEMENT element, SCITER_ELEMENT target, SciterKeys keyCode, uint32_t keyboardState) override;
    bool OnKeyChar(SCITER_ELEMENT element, SCITER_ELEMENT target, SciterKeys keyCode, uint32_t keyboardState) override;

    ISciterUI & m_sciterUI;
    ISciterWindow & m_window;
    SciterElement m_page;
    SciterElement m_root;
    SciterElement m_hotkeysTableBody;

    /// Copy of hotkeys at config open; edits and conflict checks use this until Save.
    HotkeyMap m_sessionHotkeys;

    /// Empty when not recording a new shortcut.
    std::string m_captureHotkeyId;

    bool m_keySinkAttached;
};
