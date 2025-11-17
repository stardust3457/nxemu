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
    void LoadConfiguration();
    void UpdateInputDeviceCombobox();
    void UpdateInputDevices();
    void UpdateUI();
    std::string ButtonToText(const IParamPackage & param);
    std::string AnalogToText(const IParamPackage & param, const std::string& dir);
    void UpdateMappingWithDefaults();
    void ControllerEventCallback(ControllerTriggerType type);
    void UpdatePressedButtons();
    void UpdateMotionCube();
    void UpdateStickDisplay(NativeAnalogValues analog);
    void DeadzoneSliderChanged(uint32_t analogId);
    SciterElement GetControllerSvg();

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
    SciterElement m_analogMapModifierButton[2];
    SciterElement m_analogMapDeadzoneLabel[2];
    SciterElement m_analogMapDeadzoneSlider[2];
    SciterElement m_analogMapModifierGroupbox[2];
    SciterElement m_analogMapModifierLabel[2];
    SciterElement m_analogMapModifierSlider[2];
    SciterElement m_analogMapRangeGroupbox[2];
    SciterElement m_analogMapRangeSpinbox[2];
    std::shared_ptr<IComboBox> m_comboDevices;
    const IParamPackageList & m_inputDeviceList;
    MotionState m_motionValues;
    SticksValues m_stickValues;
    button_status_t m_buttonValues[(int32_t)NativeButtonValues::NumButtons];
};
