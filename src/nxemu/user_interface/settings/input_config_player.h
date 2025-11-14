#pragma once
#include <sciter_element.h>
#include <widgets/combo_box.h>
#include <nxemu-module-spec/operating_system.h>

class InputConfig;

class InputConfigPlayer :
    public IStateChangeSink
{
public:
    InputConfigPlayer(ISciterUI& sciterUI, InputConfig & config, HWINDOW parent, SciterElement page, NpadIdType controllerIndex);
    ~InputConfigPlayer() = default;

    // IStateChangeSink
    bool OnStateChange(SCITER_ELEMENT elem, uint32_t eventReason, void * data) override;

private:
    InputConfigPlayer() = delete;
    InputConfigPlayer(const InputConfigPlayer&) = delete;
    InputConfigPlayer& operator=(const InputConfigPlayer&) = delete;

    void BindControls();
    void UpdateInputDeviceCombobox();
    void UpdateInputDevices();
    void UpdateUI();
    void UpdateMappingWithDefaults();
    void ControllerEventCallback(ControllerTriggerType type);
    void UpdateMotionCube();

    static void stControllerEventCallback(ControllerTriggerType type, void * user);

    IOperatingSystem & m_operatingSystem;
    ISciterUI & m_sciterUI;
    InputConfig & m_config;
    HWINDOW m_parent;
    SciterElement m_page;
    NpadIdType m_controllerIndex;
    IEmulatedController * m_emulatedController;
    IEmulatedController & m_emulatedControllerPlayer;
    SciterElement m_buttonMap[22];
    SciterElement m_motionMap[2];
    SciterElement m_analogMapButtons[2][4];
    std::shared_ptr<IComboBox> m_comboDevices;
    const IParamPackageList & m_inputDeviceList;
    MotionState motion_values;
};
