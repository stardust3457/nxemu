#include "input_config.h"
#include "input_config_player.h"
#include <common/std_string.h>
#include <nxemu-core/machine/switch_system.h>
#include <widgets/combo_box.h>
#include <unordered_map>
#include <numbers>
#include <stdexcept>
#include "user_interface/key_mappings.h"
#include <yuzu_common/string_util.h>
#include <yuzu_common/vector_math.h>

namespace
{
    enum VirtualKeyCodes
    {
        VK_SHIFT = 0x10,
        VK_LEFT = 0x25,
        VK_UP = 0x26,
        VK_RIGHT = 0x27,
        VK_DOWN = 0x28,
    };

    static constexpr int ANALOG_SUB_BUTTONS_NUM = 4;

    constexpr char KEY_VALUE_SEPARATOR = ':';
    constexpr char PARAM_SEPARATOR = ',';

    constexpr char ESCAPE_CHARACTER = '$';
    constexpr char KEY_VALUE_SEPARATOR_ESCAPE[] = "$0";
    constexpr char PARAM_SEPARATOR_ESCAPE[] = "$1";
    constexpr char ESCAPE_CHARACTER_ESCAPE[] = "$2";

    constexpr char EMPTY_PLACEHOLDER[] = "[empty]";
    using DataType = std::unordered_map<std::string, std::string>;

    class ButtonMappingListPtr
    {
    public:
        explicit ButtonMappingListPtr(IButtonMappingList* ptr = nullptr) : 
            m_ptr(ptr) 
        {
        }

        ~ButtonMappingListPtr()
        {
            if (m_ptr)
            {
                m_ptr->Release();
            }
        }

        ButtonMappingListPtr(ButtonMappingListPtr&& other) noexcept : 
            m_ptr(other.m_ptr)
        {
            other.m_ptr = nullptr;
        }

        ButtonMappingListPtr& operator=(ButtonMappingListPtr&& other) noexcept
        {
            if (this != &other)
            {
                if (m_ptr)
                {
                    m_ptr->Release();
                }
                m_ptr = other.m_ptr;
                other.m_ptr = nullptr;
            }
            return *this;
        }

        IButtonMappingList* operator->() const { return m_ptr; }
        IButtonMappingList& operator*() const { return *m_ptr; }
        IButtonMappingList* get() const { return m_ptr; }
        explicit operator bool() const { return m_ptr != nullptr; }

    private:
        ButtonMappingListPtr(const ButtonMappingListPtr&) = delete;
        ButtonMappingListPtr& operator=(const ButtonMappingListPtr&) = delete;

        IButtonMappingList* m_ptr;
    };

    class ParamPackage :
        public IParamPackage
    {
    public:
        ParamPackage() :
            m_package(nullptr)
        {
        }

        ParamPackage(const std::string & serialized) :
            m_package(nullptr)
        {
            if (serialized == EMPTY_PLACEHOLDER)
            {
                return;
            }
            std::vector<std::string> pairs;
            Common::SplitString(serialized, PARAM_SEPARATOR, pairs);

            for (const std::string& pair : pairs)
            {
                std::vector<std::string> key_value;
                Common::SplitString(pair, KEY_VALUE_SEPARATOR, key_value);
                if (key_value.size() != 2) 
                {
                    continue;
                }

                for (std::string& part : key_value) {
                    part = Common::ReplaceAll(part, KEY_VALUE_SEPARATOR_ESCAPE, { KEY_VALUE_SEPARATOR });
                    part = Common::ReplaceAll(part, PARAM_SEPARATOR_ESCAPE, { PARAM_SEPARATOR });
                    part = Common::ReplaceAll(part, ESCAPE_CHARACTER_ESCAPE, { ESCAPE_CHARACTER });
                }

                Set(key_value[0], std::move(key_value[1]));
            }
        }

        ParamPackage(std::initializer_list<DataType::value_type> list) : 
            data(list) ,
            m_package(nullptr)
        {
        }

        ParamPackage(ParamPackage&& other) noexcept:
            data(std::move(other.data)),
            m_package(std::exchange(other.m_package, nullptr))
        {
        }

        ParamPackage(IParamPackage * package) :
            m_package(package)
        {
        }

        ~ParamPackage()
        {
            if (m_package)
            {
                m_package->Release();
                m_package = nullptr;
            }
        }

        ParamPackage& operator=(const ParamPackage& other)
        {
            if (this != &other)
            {
                if (m_package)
                {
                    m_package->Release();
                    m_package = nullptr;
                }

                m_package = other.m_package;
                data = std::move(other.data);
                m_serializeData = m_serializeData;
            }
            return *this;
        }

        ParamPackage& operator=(ParamPackage && other)
        {
            if (this != &other)
            {
                if (m_package)
                {
                    m_package->Release();
                    m_package = nullptr;
                }

                m_package = other.m_package;
                data = std::move(other.data);
                m_serializeData = std::move(other.m_serializeData);
                other.m_package = nullptr;
            }
            return *this;
        }

        operator IParamPackage & () const
        {
            if (m_package == nullptr)
            {
                return *((IParamPackage *)this);
            }
            return *m_package;
        }

        IParamPackage * operator->() const
        {
            if (m_package == nullptr)
            {
                return (IParamPackage*)this;
            }
            return m_package;
        }
    

        bool Has(const char * key) const override
        {
            if (m_package != nullptr)
            {
                return m_package->Has(key);
            }
            return data.find(key) != data.end();
        }

        bool GetBool(const char * key, bool default_value) const override
        {
            if (m_package != nullptr)
            {
                return m_package->GetBool(key, default_value);
            }
            auto pair = data.find(key);
            if (pair == data.end()) 
            {
                return default_value;
            }

            try 
            {
                return std::stoi(pair->second) != 0;
            }
            catch (const std::logic_error&) 
            {
                return default_value;
            }
        }

        int32_t GetInt(const char * key, int32_t default_value) const override
        {
            if (m_package != nullptr)
            {
                return m_package->GetInt(key, default_value);
            }
            auto pair = data.find(key);
            if (pair == data.end())
            {
                return default_value;
            }

            try
            {
                return std::stoi(pair->second);
            }
            catch (const std::logic_error&)
            {
                return default_value;
            }
        }

        float GetFloat(const char * key, float default_value) const override
        {
            if (m_package != nullptr)
            {
                return m_package->GetFloat(key, default_value);
            }
            __debugbreak();
            return 0.0f;
        }

        const char * GetString(const char * key, const char * default_value) const override
        {
            if (m_package != nullptr)
            {
                return m_package->GetString(key, default_value);
            }
            auto pair = data.find(key);
            if (pair == data.end()) 
            {
                return default_value;
            }
            return pair->second.c_str();
        }

        void SetFloat(const char * key, float value) override
        {
            if (m_package != nullptr)
            {
                m_package->SetFloat(key, value);
            }
            else
            {
                data.insert_or_assign(key, std::to_string(value));
            }
        }

        const char * Serialize() const override
        {
            if (m_package != nullptr)
            {
                return m_package->Serialize();
            }

            if (data.empty())
            {
                return EMPTY_PLACEHOLDER;
            }

            std::string result;

            for (const auto& pair : data) {
                std::array<std::string, 2> key_value{ {pair.first, pair.second} };
                for (std::string& part : key_value) {
                    part = Common::ReplaceAll(part, { ESCAPE_CHARACTER }, ESCAPE_CHARACTER_ESCAPE);
                    part = Common::ReplaceAll(part, { PARAM_SEPARATOR }, PARAM_SEPARATOR_ESCAPE);
                    part = Common::ReplaceAll(part, { KEY_VALUE_SEPARATOR }, KEY_VALUE_SEPARATOR_ESCAPE);
                }
                result += key_value[0] + KEY_VALUE_SEPARATOR + key_value[1] + PARAM_SEPARATOR;
            }

            result.pop_back();
            m_serializeData = result;
            return m_serializeData.c_str();
        }

        void Release()
        {
        }

        void Set(const std::string& key, std::string value) {
            data.insert_or_assign(key, std::move(value));
        }

        void Set(const std::string& key, int value) {
            data.insert_or_assign(key, std::to_string(value));
        }

        void Set(const std::string& key, float value) {
            data.insert_or_assign(key, std::to_string(value));
        }

    private:
        ParamPackage(const ParamPackage&) = delete;

        IParamPackage * m_package;
        DataType data;
        mutable std::string m_serializeData;
    };

    std::string GetKeyName(int key_code) 
    {
        enum Key : int
        {
            Shift = 0x01000020,
            Control = 0x01000021,
            Alt = 0x01000023,
            Meta = 0x01000022,
        };
        
        switch (key_code) {
        case Key::Shift: return "Shift";
        case Key::Control: return "Ctrl";
        case Key::Alt: return "Alt";
        case Key::Meta: return {};
        }
        return KeyCodeToString(key_code);
    }

    std::string GenerateKeyboardParam(int key_code) 
    {
        ParamPackage param;
        param.Set("engine", "keyboard");
        param.Set("code", key_code);
        param.Set("toggle", false);
        return param.Serialize();
    }

    void SetAnalogParam(const ParamPackage & input_param, ParamPackage & analog_param, const std::string& button_name) 
    {
        // The poller returned a complete axis, so set all the buttons
        if (input_param.Has("axis_x") && input_param.Has("axis_y"))
        {
            analog_param = input_param;
            return;
        }
        // Check if the current configuration has either no engine or an axis binding.
        // Clears out the old binding and adds one with analog_from_button.
        if (!analog_param.Has("engine") || analog_param.Has("axis_x") || analog_param.Has("axis_y"))
        {
            analog_param = {
                {"engine", "analog_from_button"},
            };
        }
        analog_param.Set(button_name, input_param.Serialize());
    }

    const std::array<std::string, ANALOG_SUB_BUTTONS_NUM> analog_sub_buttons{ { "up", "down", "left", "right", } };

    const std::array<int, (uint32_t)NativeButtonValues::NumButtons> default_buttons = {
        'C', 'X', 'V', 'Z', 'F',
        'G', 'Q', 'E', 'R', 'T',
        'M', 'N', VK_LEFT, VK_UP, VK_RIGHT,
        VK_DOWN, 'Q', 'E', 0, 0,
        'Q', 'E',
    };

    const std::array<std::array<int, 4>, (size_t)NativeAnalogValues::NumAnalogs> default_analogs{ {
    {
        'W','S','A','D',
    },
    {
        'I','K','J','L',
    },}};
    
    const std::array<int, (uint32_t)NativeMotionValues::NumMotions>default_motions = { '7', '8', };

    const std::array<int, 2> default_stick_mod = { VK_SHIFT, 0, };

    std::string points_string(const std::array<std::pair<float, float>, 4>& v) 
    {
        return stdstr_f("%f,%f %f,%f %f,%f %f,%f", v[0].first, v[0].second, v[1].first, v[1].second, v[2].first, v[2].second, v[3].first, v[3].second);
    }
}

InputConfigPlayer::InputConfigPlayer(ISciterUI & sciterUI, InputConfig & config, HWINDOW parent, SciterElement page, NpadIdType controllerIndex) :
    m_operatingSystem(SwitchSystem::GetInstance()->OperatingSystem()),
    m_sciterUI(sciterUI),
    m_config(config),
    m_parent(parent),
    m_page(page),
    m_controllerIndex(controllerIndex),
    m_emulatedController(nullptr),
    m_emulatedControllerPlayer(SwitchSystem::GetInstance()->OperatingSystem().GetEmulatedController(controllerIndex)),
    m_inputDeviceList(config.InputDeviceList()),
    m_timeoutTimerActive(false),
    m_pollingType(PollingInputType::None),
    m_pollingButtonId(0)
{
    memset(&m_buttonValues, 0, sizeof(m_buttonValues));
    m_emulatedController = &m_emulatedControllerPlayer;
    m_emulatedController->SetControllerEventCallback(stControllerEventCallback,this);
    BindControls();
    LoadConfiguration();

    UpdateInputDeviceCombobox();
}

bool InputConfigPlayer::OnClick(SCITER_ELEMENT element, SCITER_ELEMENT /*source*/, uint32_t /*reason*/)
{
    for (uint32_t i = 0, n = (uint32_t)NativeButtonValues::NumButtons; i < n; i++)
    {
        if (!m_buttonMap[i].IsValid() || element != m_buttonMap[i])
        {
            continue;
        }
        HandleClick(m_buttonMap[i], i, PollingInputType::Button);
    }
    return false;
}

bool InputConfigPlayer::OnStateChange(SCITER_ELEMENT elem, uint32_t /*eventReason*/, void * /*data*/)
{
    if (m_page.GetElementByID("comboDevices") == elem)
    {
        UpdateMappingWithDefaults();
    }
    else if (m_analogMapDeadzoneSlider[0] == elem)
    {
        DeadzoneSliderChanged(0);
    }
    else if (m_analogMapDeadzoneSlider[1] == elem)
    {
        DeadzoneSliderChanged(1);
    }
    return false;
}

bool InputConfigPlayer::OnTimer(SCITER_ELEMENT /*element*/, uint32_t * timerId)
{
    if (timerId == (uint32_t*)TIMER_POLL)
    {
        ParamPackage param(m_operatingSystem.GetNextInput());
        if (param.Has("engine") && IsInputAcceptable(param))
        {
            if (m_pollingType == PollingInputType::Button)
            {
                m_emulatedController->SetButtonParam(m_pollingButtonId, param);
            }
            PollingDone();
            return false;
        }
    }
    else if (timerId == (uint32_t*)TIMER_TIMEOUT)
    {
        PollingDone();
        return false;
    }
    return true;
}

bool InputConfigPlayer::OnKeyDown(SCITER_ELEMENT /*element*/, SCITER_ELEMENT /*item*/, SciterKeys keyCode, uint32_t /*keyboardState*/)
{
    if (m_pollingType == PollingInputType::None)
    {
        return false;
    }
    if (keyCode != SCITER_KEY_ESCAPE)
    {
        int keyIndex = SciterKeyToSwitchKey(keyCode);
        if (keyIndex != 0)
        {
            m_operatingSystem.KeyboardKeyPress(0, keyIndex, SciterKeyToVKCode(keyCode));
        }
    }
    return false;
}

bool InputConfigPlayer::OnKeyUp(SCITER_ELEMENT /*element*/, SCITER_ELEMENT /*item*/, SciterKeys keyCode, uint32_t /*keyboardState*/)
{
    if (m_pollingType == PollingInputType::None)
    {
        return false;
    }
    if (keyCode != SCITER_KEY_ESCAPE)
    {
        int keyIndex = SciterKeyToSwitchKey(keyCode);
        if (keyIndex != 0)
        {
            m_operatingSystem.KeyboardKeyRelease(0, keyIndex, SciterKeyToVKCode(keyCode));
        }
    }
    return false;
}

bool InputConfigPlayer::OnKeyChar(SCITER_ELEMENT /*element*/, SCITER_ELEMENT /*item*/, SciterKeys /*keyCode*/, uint32_t /*keyboardState*/)
{
    return false;
}

void InputConfigPlayer::BindControls()
{
    m_sciterUI.AttachHandler(m_page, IID_ITIMERSINK, (ITimerSink*)this);

    m_buttonMap[0] = m_page.GetElementByID("ButtonA");
    m_buttonMap[1] = m_page.GetElementByID("ButtonB");
    m_buttonMap[2] = m_page.GetElementByID("ButtonX");
    m_buttonMap[3] = m_page.GetElementByID("ButtonY");
    m_buttonMap[4] = m_page.GetElementByID("ButtonLStick");
    m_buttonMap[5] = m_page.GetElementByID("ButtonRStick");
    m_buttonMap[6] = m_page.GetElementByID("ButtonL");
    m_buttonMap[7] = m_page.GetElementByID("ButtonR");
    m_buttonMap[8] = m_page.GetElementByID("ButtonZL");
    m_buttonMap[9] = m_page.GetElementByID("ButtonZR");
    m_buttonMap[10] = m_page.GetElementByID("ButtonPlus");
    m_buttonMap[11] = m_page.GetElementByID("ButtonMinus");
    m_buttonMap[12] = m_page.GetElementByID("ButtonDpadLeft");
    m_buttonMap[13] = m_page.GetElementByID("ButtonDpadUp");
    m_buttonMap[14] = m_page.GetElementByID("ButtonDpadRight");
    m_buttonMap[15] = m_page.GetElementByID("ButtonDpadDown");
    m_buttonMap[16] = m_page.GetElementByID("ButtonSLLeft");
    m_buttonMap[17] = m_page.GetElementByID("ButtonSRLeft");
    m_buttonMap[18] = m_page.GetElementByID("ButtonHome");
    m_buttonMap[19] = m_page.GetElementByID("ButtonScreenshot");
    m_buttonMap[20] = m_page.GetElementByID("ButtonSLRight");
    m_buttonMap[21] = m_page.GetElementByID("ButtonSRRight");

    m_motionMap[0] = m_page.GetElementByID("buttonMotionLeft");
    m_motionMap[1] = m_page.GetElementByID("buttonMotionRight");

    m_analogMapButtons[0][0] = m_page.GetElementByID("ButtonLStickUp");
    m_analogMapButtons[0][1] = m_page.GetElementByID("ButtonLStickDown");
    m_analogMapButtons[0][2] = m_page.GetElementByID("ButtonLStickLeft");
    m_analogMapButtons[0][3] = m_page.GetElementByID("ButtonLStickRight");

    m_analogMapButtons[1][0] = m_page.GetElementByID("ButtonRStickUp");
    m_analogMapButtons[1][1] = m_page.GetElementByID("ButtonRStickDown");
    m_analogMapButtons[1][2] = m_page.GetElementByID("ButtonRStickLeft");
    m_analogMapButtons[1][3] = m_page.GetElementByID("ButtonRStickRight");

    m_analogMapModifierButton[0] = m_page.GetElementByID("ButtonLStickMod");
    m_analogMapModifierButton[1] = m_page.GetElementByID("ButtonRStickMod");

    m_analogMapDeadzoneLabel[0] = m_page.GetElementByID("labelLStickDeadzone");
    m_analogMapDeadzoneLabel[1] = m_page.GetElementByID("labelRStickDeadzone");
    m_analogMapDeadzoneSlider[0] = m_page.GetElementByID("sliderLStickDeadzone");
    m_analogMapDeadzoneSlider[1] = m_page.GetElementByID("sliderRStickDeadzone");
    m_analogMapModifierGroupbox[0] = m_page.GetElementByID("buttonLStickModGroup");
    m_analogMapModifierGroupbox[1] = m_page.GetElementByID("buttonRStickModGroup");
    m_analogMapModifierButton[0] = m_page.GetElementByID("buttonLStickMod");
    m_analogMapModifierButton[1] = m_page.GetElementByID("buttonRStickMod");
    m_analogMapModifierLabel[0] = m_page.GetElementByID("labelLStickModifierRange");
    m_analogMapModifierLabel[1] = m_page.GetElementByID("labelRStickModifierRange");
    m_analogMapModifierSlider[0] = m_page.GetElementByID("sliderLStickModifierRange");
    m_analogMapModifierSlider[1] = m_page.GetElementByID("sliderRStickModifierRange");
    m_analogMapRangeGroupbox[0] = m_page.GetElementByID("buttonLStickRangeGroup");
    m_analogMapRangeGroupbox[1] = m_page.GetElementByID("buttonRStickRangeGroup");
    m_analogMapRangeSpinbox[0] = m_page.GetElementByID("spinboxLStickRange");
    m_analogMapRangeSpinbox[1] = m_page.GetElementByID("spinboxRStickRange");

    for (uint32_t i = 0; i < (uint32_t)NativeAnalogValues::NumAnalogs; i++)
    {
        if (m_analogMapDeadzoneSlider[i].IsValid())
        {
            m_sciterUI.AttachHandler(m_analogMapDeadzoneSlider[i], IID_ISTATECHANGESINK, (IStateChangeSink*)this);
        }
    }

    for (uint32_t i = 0, n = (uint32_t)NativeButtonValues::NumButtons; i < n; i++)
    {
        if (!m_buttonMap[i].IsValid())
        {
            continue;
        }
        m_sciterUI.AttachHandler(m_buttonMap[i], IID_ICLICKSINK, (IClickSink*)this);
    }

    SciterElement comboDevices = m_page.GetElementByID("comboDevices");
    std::shared_ptr<void> interfacePtr = m_sciterUI.GetElementInterface(comboDevices, IID_ICOMBOBOX);
    if (interfacePtr)
    {
        m_comboDevices = std::static_pointer_cast<IComboBox>(interfacePtr);
    }
    m_sciterUI.AttachHandler(comboDevices, IID_ISTATECHANGESINK, (IStateChangeSink*)this);
}

void InputConfigPlayer::LoadConfiguration()
{
    m_emulatedController->ReloadFromSettings();
    UpdateUI();
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

void InputConfigPlayer::UpdateUI()
{
    for (uint32_t i = 0, n = (uint32_t)NativeButtonValues::NumButtons; i < n; i++)
    {
        ParamPackage param(m_emulatedController->GetButtonParamPtr(i));
        ParamPackage b;
        b = m_emulatedController->GetButtonParamPtr(i);
        if (m_buttonMap[i].IsValid())
        {
            m_buttonMap[i].SetText(ButtonToText(param).c_str());
        }
    }
    const ParamPackage ZL_param(m_emulatedController->GetButtonParamPtr((uint32_t)NativeButtonValues::ZL));
    if (ZL_param->Has("threshold")) {
        const int button_threshold = static_cast<int>(ZL_param->GetFloat("threshold", 0.5f) * 100.0f);
        SciterElement sliderZLThreshold = m_page.GetElementByID("sliderZLThreshold");
        if (sliderZLThreshold.IsValid())
        {
            sliderZLThreshold.SetValue(button_threshold);
        }
    }

    const ParamPackage ZR_param(m_emulatedController->GetButtonParamPtr((uint32_t)NativeButtonValues::ZR));
    if (ZR_param->Has("threshold")) 
    {
        const int button_threshold = static_cast<int>(ZR_param.GetFloat("threshold", 0.5f) * 100.0f);
        SciterElement sliderZRThreshold = m_page.GetElementByID("sliderZRThreshold");
        if (sliderZRThreshold.IsValid())
        {
            sliderZRThreshold.SetValue(button_threshold);
        }
    }

    for (int i = 0, n = (uint32_t)NativeMotionValues::NumMotions; i < n; i++)
    {
        if (m_motionMap[i].IsValid())
        {
            ParamPackage param(m_emulatedController->GetMotionParamPtr(i));
            m_motionMap[i].SetText(ButtonToText(param).c_str());
        }
    }

    for (int analog_id = 0; analog_id < (size_t)NativeAnalogValues::NumAnalogs; ++analog_id)
    {
        const ParamPackage param(m_emulatedController->GetStickParamPtr(analog_id));
        for (int sub_button_id = 0; sub_button_id < ANALOG_SUB_BUTTONS_NUM; ++sub_button_id)
        {
            SciterElement & analog_button = m_analogMapButtons[analog_id][sub_button_id];
            if (!analog_button.IsValid()) 
            {
                continue;
            }
            analog_button.SetText(AnalogToText(param, analog_sub_buttons[sub_button_id]).c_str());
        }

        if (m_analogMapModifierButton[analog_id].IsValid())
        {
            m_analogMapModifierButton[analog_id].SetText(ButtonToText(ParamPackage(param->GetString("modifier", ""))).c_str());
        }

        const SciterElement & deadzone_label = m_analogMapDeadzoneLabel[analog_id];
        const SciterElement & deadzone_slider = m_analogMapDeadzoneSlider[analog_id];
        const SciterElement & modifier_groupbox = m_analogMapModifierGroupbox[analog_id];
        const SciterElement & modifier_label = m_analogMapModifierLabel[analog_id];
        const SciterElement & modifier_slider = m_analogMapModifierSlider[analog_id];
        const SciterElement & range_groupbox = m_analogMapRangeGroupbox[analog_id];
        const SciterElement & range_spinbox = m_analogMapRangeSpinbox[analog_id];

        const bool is_controller = m_operatingSystem.IsController(param);
        if (deadzone_label.IsValid())
        {
            deadzone_label.SetStyleAttribute("display", is_controller ? "" : "none");
        }
        if (deadzone_slider.IsValid())
        {
            deadzone_slider.SetStyleAttribute("display", is_controller ? "" : "none");
        }
        if (modifier_groupbox.IsValid())
        {
            modifier_groupbox.SetStyleAttribute("display", !is_controller ? "" : "none");
        }
        if (modifier_label.IsValid())
        {
            modifier_label.SetStyleAttribute("display", !is_controller ? "" : "none");
        }
        if (modifier_slider.IsValid())
        {
            modifier_slider.SetStyleAttribute("display", !is_controller ? "" : "none");
        }
        if (range_groupbox.IsValid())
        {
            range_groupbox.SetStyleAttribute("display", is_controller ? "" : "none");
        }

        int slider_value;
        if (is_controller) 
        {
            slider_value = (int)(param.GetFloat("deadzone", 0.15f) * 100);
            if (deadzone_label.IsValid())
            {
                deadzone_label.SetText(stdstr_f("Deadzone: %d%%", slider_value).c_str());
            }
            if (deadzone_slider.IsValid())
            {
                deadzone_slider.SetValue(slider_value);
            }
            if (range_spinbox.IsValid())
            {
                range_spinbox.SetValue((int)(param.GetFloat("range", 0.95f) * 100));
            }
        }
        else
        {
            slider_value = (int)(param.GetFloat("modifier_scale", 0.5f) * 100);
            if (modifier_label.IsValid())
            {
                modifier_label.SetText(stdstr_f("Modifier Range: %d%%", slider_value).c_str());
            }
            if (modifier_slider.IsValid())
            {
                modifier_slider.SetValue(slider_value);
            }
        }
    }
}

std::string InputConfigPlayer::ButtonToText(const IParamPackage& param)
{
    if (!param.Has("engine")) 
    {
        return "[not set]";
    }

    const std::string toggle = param.GetBool("toggle", false) ? "~" : "";
    const std::string inverted = param.GetBool("inverted", false) ? "!" : "";
    const std::string invert = std::string(param.GetString("invert", "+")) == "-" ? "-" : "";
    const std::string turbo = param.GetBool("turbo", false) ? "$" : "";

    const ButtonNames common_button_name = m_operatingSystem.GetButtonName(param);

    // Retrieve the names from Qt
    if (std::string(param.GetString("engine", "")) == "keyboard") 
    {
        const std::string button_str = GetKeyName(param.GetInt("code", 0));
        return stdstr_f("%s%s%s%s", turbo.c_str(), toggle.c_str(), inverted.c_str(), button_str.c_str());
    }

    if (common_button_name == ButtonNames::Invalid) 
    {
        return "[invalid]";
    }

    if (common_button_name == ButtonNames::Engine) 
    {
        return param.GetString("engine", "");
    }

    if (common_button_name == ButtonNames::Value) 
    {
        if (param.Has("hat")) 
        {
            return stdstr_f("%s%s%sHat %s", turbo.c_str(), toggle.c_str(), inverted.c_str(), param.GetString("direction", ""));
        }
        if (param.Has("axis")) 
        {
            return stdstr_f("%s%s%sAxis %s", toggle.c_str(), inverted.c_str(), invert.c_str(), param.GetString("axis", ""));
        }
        if (param.Has("axis_x") && param.Has("axis_y") && param.Has("axis_z")) 
        {
            return stdstr_f("%s%sAxis %s,%s,%s", toggle.c_str(), inverted.c_str(), param.GetString("axis_x", ""), param.GetString("axis_y", ""), param.GetString("axis_z", ""));
        }
        if (param.Has("motion")) 
        {
            return stdstr_f("%s%sMotion %s", toggle.c_str(), inverted.c_str(), param.GetString("motion", ""));
        }
        if (param.Has("button")) 
        {
            return stdstr_f("%s%s%sButton %s", turbo.c_str(), toggle.c_str(), inverted.c_str(), param.GetString("button", ""));
        }
    }

    std::string button_name = GetButtonName(common_button_name);
    if (param.Has("hat"))
    {
        return stdstr_f("%s%s%sHat %s", turbo.c_str(), toggle.c_str(), inverted.c_str(), button_name.c_str());
    }
    if (param.Has("axis"))
    {
        return stdstr_f("%s%s%sAxis %s", turbo.c_str(), toggle.c_str(), inverted.c_str(), button_name.c_str());
    }
    if (param.Has("motion"))
    {
        return stdstr_f("%s%sAxis %s", toggle.c_str(), inverted.c_str(), button_name.c_str());
    }
    if (param.Has("button")) 
    {
        return stdstr_f("%s%s3Button %s", turbo.c_str(), toggle.c_str(), inverted.c_str(), button_name.c_str());
    }
    return "[unknown]";
}

std::string InputConfigPlayer::AnalogToText(const IParamPackage& param, const std::string& dir)
{
    if (!param.Has("engine"))
    {
        return "[not set]";
    }

    if (std::string(param.GetString("engine", "")) == "analog_from_button") 
    {
        return ButtonToText(ParamPackage({ param.GetString(dir.c_str(), "") }));
    }

    if (!param.Has("axis_x") || !param.Has("axis_y"))
    {
        return "[unknown]"  ;
    }

    if (dir == "modifier") 
    {
        return "[unused]";
    }

    const bool invert_x = std::string(param.GetString("invert_x", "+")) == "-";
    const bool invert_y = std::string(param.GetString("invert_y", "+")) == "-";
    if (dir == "left")
    {
        return stdstr_f("Axis %s%s", param.GetString("axis_x", ""), invert_x ? "+" : "-");
    }
    if (dir == "right") 
    {
        return stdstr_f("Axis %s%s", param.GetString("axis_x", ""), invert_x ? "-" : "+");
    }
    if (dir == "up") 
    {
        return stdstr_f("Axis %s%s", param.GetString("axis_y", ""), invert_y ? "-" : "+");
    }
    if (dir == "down") 
    {
        return stdstr_f("Axis %s%s", param.GetString("axis_y", ""), invert_y ? "+" : "-");
    }
    return "[unknown]";
}
void InputConfigPlayer::UpdateMappingWithDefaults()
{
    if (m_comboDevices->CurrentIndex() == 0)
    {
        return;
    }

    for (uint32_t i = 0, n = (uint32_t)NativeButtonValues::NumButtons; i < n; i++)
    {
        if (!m_buttonMap[i].IsValid()) 
        {
            continue;
        }
        m_emulatedController->SetButtonParam(i, ParamPackage());
    }

    for (int analog_id = 0; analog_id < (size_t)NativeAnalogValues::NumAnalogs; analog_id++)
    {
        bool found = false;
        for (int sub_button_id = 0; sub_button_id < ANALOG_SUB_BUTTONS_NUM; sub_button_id++)
        {
            SciterElement & analog_button = m_analogMapButtons[analog_id][sub_button_id];
            if (!analog_button.IsValid()) 
            {
                continue;
            }
            found = true;
            break;
        }
        if (found)
        {
            m_emulatedController->SetStickParam(analog_id, ParamPackage());
        }
    }
    
    for (int i = 0, n = (uint32_t)NativeMotionValues::NumMotions; i < n; i++)
    {
        if (m_motionMap[i].IsValid())
        {
            continue;
        }
        m_emulatedController->SetMotionParam(i, ParamPackage());
    }

    // Reset keyboard or mouse bindings
    if (m_comboDevices->CurrentIndex() == 1 || m_comboDevices->CurrentIndex() == 2) 
    {
        for (uint32_t i = 0, n = (uint32_t)NativeButtonValues::NumButtons; i < n; i++)
        {
            m_emulatedController->SetButtonParam(i, ParamPackage(GenerateKeyboardParam(default_buttons[i]) ));
        }
        for (int analog_id = 0; analog_id < (size_t)NativeAnalogValues::NumAnalogs; ++analog_id)
        {
            ParamPackage analog_param;
            for (int sub_button_id = 0; sub_button_id < ANALOG_SUB_BUTTONS_NUM; ++sub_button_id)
            {
                ParamPackage params{ GenerateKeyboardParam( default_analogs[analog_id][sub_button_id]) };
                SetAnalogParam(params, analog_param, analog_sub_buttons[sub_button_id]);
            }

            analog_param.Set("modifier", GenerateKeyboardParam(default_stick_mod[analog_id]));
            m_emulatedController->SetStickParam(analog_id, analog_param);
        }

        for (int i = 0, n = (uint32_t)NativeMotionValues::NumMotions; i < n; i++)
        {
            m_emulatedController->SetMotionParam(i, ParamPackage( GenerateKeyboardParam(default_motions[i]) ));
        }

        // If mouse is selected we want to override with mappings from the driver
        if (m_comboDevices->CurrentIndex() == 1) 
        {
            UpdateUI();
            return;
        }
    }

    // Reset controller bindings
    const IParamPackage & device = m_inputDeviceList.GetParamPackage(m_comboDevices->CurrentIndex());
    ButtonMappingListPtr button_mappings(m_operatingSystem.GetButtonMappingForDevice(device));
    ButtonMappingListPtr analog_mappings(m_operatingSystem.GetAnalogMappingForDevice(device));
    ButtonMappingListPtr motion_mappings(m_operatingSystem.GetMotionMappingForDevice(device));
    
    for (uint32_t i = 0; i < button_mappings->GetCount(); ++i)
    {
        const uint32_t index = button_mappings->GetIndex(i);
        m_emulatedController->SetButtonParam(index, button_mappings->GetParamPackage(i));
    }

    for (uint32_t i = 0; i < analog_mappings->GetCount(); ++i)
    {
        const uint32_t index = analog_mappings->GetIndex(i);
        m_emulatedController->SetStickParam(index, analog_mappings->GetParamPackage(i));
    }

    for (uint32_t i = 0; i < motion_mappings->GetCount(); ++i)
    {
        const uint32_t index = motion_mappings->GetIndex(i);
        m_emulatedController->SetMotionParam(index, motion_mappings->GetParamPackage(i));
    }
    UpdateUI();
}

void InputConfigPlayer::HandleClick(SciterElement & button, uint32_t buttonId, PollingInputType type)
{
    if (m_timeoutTimerActive) 
    {
        return;
    }
    if (button == m_motionMap[0] || button == m_motionMap[1])
    {
        button.SetText("Shake!");
    }
    else
    {
        button.SetText("[waiting]");
    }
    button.SetState(SciterElement::STATE_FOCUS, 0, true);

    m_pollingButtonId = buttonId;
    m_pollingType = type;
    m_operatingSystem.BeginMapping(type);
    if (m_pollingType == PollingInputType::Button)
    {
        UpdateButtonState();
    }

    m_timeoutTimerActive = true;
    m_page.SetTimer(4000, (uint32_t*)TIMER_TIMEOUT);
    m_page.SetTimer(25, (uint32_t*)TIMER_POLL);
}

void InputConfigPlayer::ControllerEventCallback(ControllerTriggerType type)
{
    switch (type) {
    case ControllerTriggerType::Button:
    case ControllerTriggerType::Trigger:
        m_emulatedController->GetButtonsStatus(m_buttonValues, sizeof(m_buttonValues)/sizeof(m_buttonValues[0]));
        UpdateButtonState();
        break;
    case ControllerTriggerType::Stick:
        m_stickValues = m_emulatedController->GetSticksValues();
        // Y axis is inverted
        m_stickValues.status[(size_t)NativeAnalogValues::LStick].y.value = -m_stickValues.status[(size_t)NativeAnalogValues::LStick].y.value;
        m_stickValues.status[(size_t)NativeAnalogValues::LStick].y.raw_value = -m_stickValues.status[(size_t)NativeAnalogValues::LStick].y.raw_value;
        m_stickValues.status[(size_t)NativeAnalogValues::RStick].y.value = -m_stickValues.status[(size_t)NativeAnalogValues::RStick].y.value;
        m_stickValues.status[(size_t)NativeAnalogValues::RStick].y.raw_value = -m_stickValues.status[(size_t)NativeAnalogValues::RStick].y.raw_value;
        UpdateStickDisplay(NativeAnalogValues::LStick);
        UpdateStickDisplay(NativeAnalogValues::RStick);
        break;
    case ControllerTriggerType::Motion:
        m_motionValues = m_emulatedController->GetMotions();
        UpdateMotionCube();
        break;
    }
}

void InputConfigPlayer::UpdateButtonState()
{
    struct BtnMap 
    {
        uint32_t idx; 
        const char* sel; 
    };

    static const BtnMap kBtnMap[] = {
        {(uint32_t)NativeButtonValues::A,          ".buttons-face .button.A" },
        {(uint32_t)NativeButtonValues::B,          ".buttons-face .button.B" },
        {(uint32_t)NativeButtonValues::X,          ".buttons-face .button.X" },
        {(uint32_t)NativeButtonValues::Y,          ".buttons-face .button.Y" },
        {(uint32_t)NativeButtonValues::DUp,        ".dpad .up"    },
        {(uint32_t)NativeButtonValues::DDown,      ".dpad .down"  },
        {(uint32_t)NativeButtonValues::DLeft,      ".dpad .left"  },
        {(uint32_t)NativeButtonValues::DRight,     ".dpad .right" },
        {(uint32_t)NativeButtonValues::ZL,         ".trigger.ZL"  },
        {(uint32_t)NativeButtonValues::ZR,         ".trigger.ZR"  },
        {(uint32_t)NativeButtonValues::L,          ".trigger.L"  },
        {(uint32_t)NativeButtonValues::R,          ".trigger.R"  },
        {(uint32_t)NativeButtonValues::Minus,      ".button.minus" },
        {(uint32_t)NativeButtonValues::Plus,       ".button.plus" },
        {(uint32_t)NativeButtonValues::Screenshot, ".button.screenshot" },
        {(uint32_t)NativeButtonValues::Home,       ".button.home" },
        {(uint32_t)NativeButtonValues::SLLeft,     ".button.SL-left"},
        {(uint32_t)NativeButtonValues::SRLeft,     ".button.SR-left"},
        {(uint32_t)NativeButtonValues::SLRight,    ".button.SL-right"},
        {(uint32_t)NativeButtonValues::SRRight,    ".button.SR-right"},
    };

    SciterElement controllerSvg = GetControllerSvg();
    if (!controllerSvg.IsValid())
    {
        return;
    }

    for (const BtnMap & m : kBtnMap)
    {
        const button_status_t & bs = m_buttonValues[m.idx];
        SciterElements els = controllerSvg.FindAll(m.sel);
        if (els.empty())
        {
            continue;
        }

        for (SciterElement & el : els)
        {
            if (m_pollingType == PollingInputType::Button && m.idx == m_pollingButtonId)
            {
                el.SetAttribute("data-setting", "1");
            }
            else
            {
                el.RemoveAttribute("data-setting");
            }

            if (bs.value)
            {
                el.SetAttribute("data-pressed", "1");
            }
            else
            {
                el.RemoveAttribute("data-pressed");
            }
        }
    }
}

void InputConfigPlayer::UpdateMotionCube()
{
    float size = 15.0f;
    const vec3f_t & eulerValue = m_motionValues.motion[(uint32_t)NativeMotionValues::MotionLeft].euler;
    const Common::Vec3f euler(eulerValue.x, eulerValue.y, eulerValue.z);

    std::array<Common::Vec3f, 8> cube {{
        {-0.7f, -1, -0.5f}, {-0.7f, 1, -0.5f}, {0.7f, 1, -0.5f}, {0.7f, -1, -0.5f},
        {-0.7f, -1, 0.5f}, {-0.7f, 1, 0.5f}, {0.7f, 1, 0.5f}, {0.7f, -1, 0.5f},
    }};

    for (Common::Vec3f & point : cube)
    {
        point.RotateFromOrigin(euler.x, euler.y, euler.z);
        point *= size;
    }

    std::array<std::pair<float, float>, 4> front = {{
        {cube[0].x, cube[0].y}, {cube[1].x, cube[1].y},
        {cube[2].x, cube[2].y}, {cube[3].x, cube[3].y}
    }};
    std::array<std::pair<float, float>, 4> back = {{
        {cube[4].x, cube[4].y}, {cube[5].x, cube[5].y},
        {cube[6].x, cube[6].y}, {cube[7].x, cube[7].y}
    }};

    SciterElement motion = m_page.FindFirst("#motion");
    if (!motion.IsValid())
    {
        return;
    }

    SciterElement poly_front = motion.FindFirst(".cube.face.front");
    if (poly_front.IsValid())
    {
        poly_front.SetAttribute("points", points_string(front).c_str());
    }
    SciterElement poly_back = motion.FindFirst(".cube.face.back");
    if (poly_back.IsValid())
    {
        poly_back.SetAttribute("points", points_string(back).c_str());
    }

    const int A[4] = { 0,1,2,3 };
    const int B[4] = { 4,5,6,7 };
    for (int i = 0; i < 4; ++i) 
    {
        std::string sel = std::string(".cube.edge.e") + char('0' + i);
        SciterElement e = motion.FindFirst(sel.c_str());
        if (!e.IsValid())
        {
            continue;
        }

        e.SetAttribute("x1", stdstr_f("%f", cube[A[i]].x).c_str());
        e.SetAttribute("y1", stdstr_f("%f", cube[A[i]].y).c_str());
        e.SetAttribute("x2", stdstr_f("%f", cube[B[i]].x).c_str());
        e.SetAttribute("y2", stdstr_f("%f", cube[B[i]].y).c_str());
    }
}

void InputConfigPlayer::UpdateStickDisplay(NativeAnalogValues analog)
{
    if (analog != NativeAnalogValues::LStick && analog != NativeAnalogValues::RStick)
    {
        return;
    }
    StickStatus & status = m_stickValues.status[(size_t)analog];
    SciterElement svg = GetControllerSvg();
    if (!svg.IsValid())
    {
        return;
    }

    SciterElement el = svg.FindFirst(analog == NativeAnalogValues::LStick ? ".left-stick-visual" : ".right-stick-visual");
    if (!el.IsValid())
    {
        return;
    }
    SciterElement range = el.FindFirst(".joystick-range");
    float radius = 45.0f;
    if (range.IsValid())
    {
        radius = strtof(range.GetAttribute("r").c_str(), nullptr);
    }
    SciterElement rawDot = el.FindFirst(".joystick-dot-raw");
    if (rawDot.IsValid())
    {
        rawDot.SetAttribute("cx", stdstr_f("%f", status.x.raw_value * radius).c_str());
        rawDot.SetAttribute("cy", stdstr_f("%f", status.y.raw_value * radius).c_str());
    }
    SciterElement rawCalibrated = el.FindFirst(".joystick-dot-calibrated");
    if (rawCalibrated.IsValid())
    {
        rawCalibrated.SetAttribute("cx", stdstr_f("%f", status.x.value * radius).c_str());
        rawCalibrated.SetAttribute("cy", stdstr_f("%f", status.y.value * radius).c_str());
    }
    SciterElement leftStick = analog == NativeAnalogValues::LStick ? svg.FindFirst(".left-stick-movable") : SciterElement();
    if (leftStick.IsValid())
    {
        leftStick.SetAttribute("transform", stdstr_f("translate(%f,%f)", status.x.value * 10.0f, status.y.value * 10.0f).c_str());
    }
    SciterElement rightStick = analog == NativeAnalogValues::RStick ? svg.FindFirst(".right-stick-movable") : SciterElement();
    if (rightStick.IsValid())
    {
        rightStick.SetAttribute("transform", stdstr_f("translate(%f,%f)", status.x.value * 9.5f, status.y.value * 9.5f).c_str());
    }

    SciterElement proStick;
    if (analog == NativeAnalogValues::LStick)
    {
        proStick = svg.FindFirst(".pro-left-stick");
    }
    else if (analog == NativeAnalogValues::RStick)
    {
        proStick = svg.FindFirst(".pro-right-stick");
    }

    if (proStick.IsValid())
    {
        SciterElement movable = proStick.FindFirst(".movable");
        SciterElement outerEll = proStick.FindFirst(".outer");
        SciterElement innerEll = proStick.FindFirst(".inner");
        SciterElement innerShift = proStick.FindFirst(".inner-shift");
        if (movable.IsValid() && outerEll.IsValid() && innerEll.IsValid() && innerShift.IsValid())
        {
            const float x = status.x.value;
            const float y = status.y.value;
            const float mag = std::min(1.0f, std::hypot(x, y));
            const float amp = std::max(0.0f, 1.0f - mag * 0.1f);
            const float angleDeg = std::atan2(y, x) * 180.0f / (float)(std::numbers::pi);
            const float scalar = 11.0f;
                
            movable.SetAttribute("transform", stdstr_f("translate(%f,%f) rotate(%f)", x * scalar, y * scalar, angleDeg).c_str());
            outerEll.SetAttribute("rx", stdstr_f("%f", 24.0f * amp).c_str());
            innerEll.SetAttribute("rx", stdstr_f("%f", 17.0f * amp).c_str());
            innerShift.SetAttribute("transform", stdstr_f("translate(%f,0)", ((24.0f - 17.0f) * 0.4f) * mag).c_str());
        }
    }
}

void InputConfigPlayer::DeadzoneSliderChanged(uint32_t analogId)
{
    ParamPackage param(m_emulatedController->GetStickParamPtr(analogId));
    const int slider_value = m_analogMapDeadzoneSlider[analogId].GetValue().GetValueInt();
    if (m_analogMapDeadzoneLabel[analogId].IsValid())
    {
        m_analogMapDeadzoneLabel[analogId].SetText(stdstr_f("Deadzone: %d%%",slider_value).c_str());
    }
    param.SetFloat("deadzone", slider_value / 100.0f);
    m_emulatedController->SetStickParam(analogId, param);

    SciterElement controllerSvg = GetControllerSvg();
    if (controllerSvg.IsValid())
    {
        SciterElement deadzoneEl = controllerSvg.FindFirst(analogId == 0 ? ".left-stick-visual .joystick-deadzone" : ".right-stick-visual .joystick-deadzone");
        if (deadzoneEl.IsValid())
        {
            deadzoneEl.SetAttribute("r", stdstr_f("%d", (int)((slider_value / 100.0f) * 45)).c_str());
        }
    }
}

SciterElement InputConfigPlayer::GetControllerSvg()
{
    return m_page.GetElementByID("ProController");
}

bool InputConfigPlayer::IsInputAcceptable(const IParamPackage & params) const
{
    if (m_comboDevices->CurrentIndex() == 0)
    {
        return true;
    }

    if (params.Has("motion"))
    {
        return true;
    }

    // Keyboard/Mouse
    if (m_comboDevices->CurrentIndex() == 1 || m_comboDevices->CurrentIndex() == 2)
    {
        return strcmp(params.GetString("engine", ""), "keyboard") == 0 || strcmp(params.GetString("engine", ""), "mouse") == 0;
    }

    const IParamPackage & device = m_inputDeviceList.GetParamPackage(m_comboDevices->CurrentIndex());
    return strcmp(params.GetString("engine", ""), device.GetString("engine", "")) == 0 && (strcmp(params.GetString("guid", ""), device.GetString("guid", "")) == 0 || strcmp(params.GetString("guid", ""), device.GetString("guid2", "")) == 0) && params.GetInt("port", 0) == device.GetInt("port", 0);
}

void InputConfigPlayer::PollingDone()
{
    if (m_pollingType == PollingInputType::Button && m_buttonMap[m_pollingButtonId].IsValid())
    {
        ParamPackage param(m_emulatedController->GetButtonParamPtr(m_pollingButtonId));
        m_buttonMap[m_pollingButtonId].SetText(ButtonToText(param).c_str());
    }
    m_operatingSystem.StopMapping();
    m_page.SetTimer(0, (uint32_t *)TIMER_TIMEOUT);
    m_page.SetTimer(0, (uint32_t*)TIMER_POLL);
    m_timeoutTimerActive = false;
    m_pollingType = PollingInputType::None;
    m_pollingButtonId = 0;
    UpdateButtonState();
}

void InputConfigPlayer::stControllerEventCallback(ControllerTriggerType type, void * user)
{
    ((InputConfigPlayer*)user)->ControllerEventCallback(type);
}
