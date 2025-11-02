#include "input_config.h"
#include "input_config_player.h"
#include <nxemu-core/machine/switch_system.h>
#include <widgets/combo_box.h>

InputConfigPlayer::InputConfigPlayer(ISciterUI & sciterUI, InputConfig & config, HWINDOW parent, SciterElement page, NpadIdType controllerIndex) :
    m_sciterUI(sciterUI),
    m_config(config),
    m_parent(parent),
    m_page(page),
    m_controllerIndex(controllerIndex),
    m_emulatedController(nullptr),
    m_emulatedControllerPlayer(SwitchSystem::GetInstance()->OperatingSystem().GetEmulatedController(controllerIndex)),
    m_inputDeviceList(config.InputDeviceList())
{
    m_emulatedController = &m_emulatedControllerPlayer;
    UpdateInputDeviceCombobox();
}

void InputConfigPlayer::UpdateInputDeviceCombobox(void)
{
    std::shared_ptr<void> interfacePtr = m_sciterUI.GetElementInterface(m_page.GetElementByID("comboDevices"), IID_ICOMBOBOX);
    if (!interfacePtr)
    {
        return;
    }
    std::shared_ptr<IComboBox> comboBox = std::static_pointer_cast<IComboBox>(interfacePtr);

    // Skip input device persistence if "Input Devices" is set to "Any".
    if (comboBox->CurrentIndex() == 0)
    {
        UpdateInputDevices();
        return;
    }

    IParamPackageList * devices = m_emulatedController->GetMappedDevicesPtr();
    UpdateInputDevices();

    if (devices->GetCount() == 0)
    {
        return;
    }

    if (devices->GetCount() > 2) 
    {
        comboBox->SelectItem(0);
        return;
    }

    const IParamPackage & firstDevice = devices->GetParamPackage(0);
    const std::string first_engine = firstDevice.GetString("engine", "");
    const std::string first_guid = firstDevice.GetString("guid", "");
    const int32_t first_port = firstDevice.GetInt("port", 0);
    const int32_t first_pad = firstDevice.GetInt("pad", 0);

    if (devices->GetCount() == 1) 
    {
        uint32_t device_index = 0;
        const uint32_t device_count = m_inputDeviceList.GetCount();

        for (uint32_t i = 0; i < device_count; ++i) 
        {
            const IParamPackage & param = m_inputDeviceList.GetParamPackage(i);
            if (param.GetString("engine", "") == first_engine &&
                param.GetString("guid", "") == first_guid &&
                param.GetInt("port", 0) == first_port &&
                param.GetInt("pad", 0) == first_pad)
            {
                device_index = i;
                break;
            }
        }

        comboBox->SelectItem(device_index);
        return;
    }
}

void InputConfigPlayer::UpdateInputDevices(void)
{
    std::shared_ptr<void> interfacePtr = m_sciterUI.GetElementInterface(m_page.GetElementByID("comboDevices"), IID_ICOMBOBOX);
    if (!interfacePtr)
    {
        return;
    }
    std::shared_ptr<IComboBox> comboBox = std::static_pointer_cast<IComboBox>(interfacePtr);
    comboBox->ClearContents();
    for (uint32_t i = 0, n = m_inputDeviceList.GetCount(); i < n; i++)
    {
        IParamPackage & device = m_inputDeviceList.GetParamPackage(i);
        const char * display = device.GetString("display", "Unknown");
        comboBox->AddItem(display, display);
    }
}
