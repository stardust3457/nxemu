#include "input_config.h"
#include "input_config_player.h"
#include <sciter_ui.h>
#include <nxemu-core/machine/switch_system.h>

InputConfig::InputConfig(ISciterUI & SciterUI) :
    m_sciterUI(SciterUI),
    m_window(nullptr),
    m_inputDeviceList(nullptr)
{
    SwitchSystem * system = SwitchSystem::GetInstance();
    if (system != nullptr)
    {
        IOperatingSystem & OperatingSystem = system->OperatingSystem();
        m_inputDeviceList = OperatingSystem.GetInputDevices();
    }
}

InputConfig::~InputConfig()
{
    if (m_inputDeviceList != nullptr)
    {
        m_inputDeviceList->Release();
        m_inputDeviceList = nullptr;
    }
}

void InputConfig::Display(void * parentWindow)
{
    if (!m_sciterUI.WindowCreate(parentWindow, "input_config.html", 0, 0, 300, 300, SUIW_CHILD, m_window))
    {
        return;
    }

    SciterElement root(m_window->GetRootElement());
    if (root.IsValid())
    {
        SciterElement pageNav = root.GetElementByID("MainTabNav");
        std::shared_ptr<void> interfacePtr = pageNav.IsValid() ? m_sciterUI.GetElementInterface(pageNav, IID_IPAGENAV) : nullptr;
        if (interfacePtr)
        {
            m_pageNav = std::static_pointer_cast<IPageNav>(interfacePtr);
            m_pageNav->AddSink(this);
        }

        SciterElement okButton = root.FindFirst("button[role=\"window-ok\"]");
        m_sciterUI.AttachHandler(okButton, IID_ICLICKSINK, (IClickSink*)this);
    }

    m_window->FixMinSize();
    m_window->CenterWindow();
}

const IParamPackageList & InputConfig::InputDeviceList() const
{
    return *m_inputDeviceList;
}

bool InputConfig::PageNavChangeFrom(const std::string & /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
    return true;
}

bool InputConfig::PageNavChangeTo(const std::string & /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
    return true;
}

void InputConfig::PageNavCreatedPage(const std::string & pageName, SCITER_ELEMENT page)
{
    if (pageName == "Player1")
    {
        m_playerConfig[0].reset(new InputConfigPlayer(m_sciterUI, *this, m_window->GetHandle(), page, NpadIdType::Player1));
    }
}

void InputConfig::PageNavPageChanged(const std::string & /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
}

bool InputConfig::OnClick(SCITER_ELEMENT element, SCITER_ELEMENT /*source*/, uint32_t /*reason*/)
{
    SciterElement clickElem(element);
    if (clickElem.GetAttribute("role") == "window-ok")
    {
        m_window->Destroy();
    }
    return false;
}