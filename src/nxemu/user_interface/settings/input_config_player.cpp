#include "input_config.h"
#include "input_config_player.h"
#include <common/std_string.h>
#include <nxemu-core/machine/switch_system.h>
#include <widgets/combo_box.h>
#include <unordered_map>
#include <stdexcept>
#include <Windows.h>
#include <yuzu_common/string_util.h>
#include <yuzu_common/vector_math.h>

namespace
{
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

        ParamPackage& operator=(const ParamPackage&)
        {
            __debugbreak();
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
        if (input_param.Has("axis_x") && input_param.Has("axis_y")) {
            analog_param = input_param;
            return;
        }
        // Check if the current configuration has either no engine or an axis binding.
        // Clears out the old binding and adds one with analog_from_button.
        if (!analog_param.Has("engine") || analog_param.Has("axis_x") || analog_param.Has("axis_y")) {
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
    m_inputDeviceList(config.InputDeviceList())
{
    m_emulatedController = &m_emulatedControllerPlayer;
    m_emulatedController->SetControllerEventCallback(stControllerEventCallback,this);
    BindControls();

    UpdateInputDeviceCombobox();
}

bool InputConfigPlayer::OnStateChange(SCITER_ELEMENT elem, uint32_t /*eventReason*/, void * /*data*/)
{
    if (m_page.GetElementByID("comboDevices") == elem)
    {
        UpdateMappingWithDefaults();
    }
    return false;
}

void InputConfigPlayer::BindControls()
{
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
    SciterElement comboDevices = m_page.GetElementByID("comboDevices");
    std::shared_ptr<void> interfacePtr = m_sciterUI.GetElementInterface(comboDevices, IID_ICOMBOBOX);
    if (interfacePtr)
    {
        m_comboDevices = std::static_pointer_cast<IComboBox>(interfacePtr);
    }
    m_sciterUI.AttachHandler(comboDevices, IID_ISTATECHANGESINK, (IStateChangeSink*)this);
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
void InputConfigPlayer::ControllerEventCallback(ControllerTriggerType type)
{
    switch (type) {
    case ControllerTriggerType::Motion:
        motion_values = m_emulatedController->GetMotions();
        UpdateMotionCube();
        break;
    }
}

void InputConfigPlayer::UpdateMotionCube()
{
    float size = 15.0f;
    const vec3f_t & eulerValue = motion_values.motion[(uint32_t)NativeMotionValues::MotionLeft].euler;
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

void InputConfigPlayer::stControllerEventCallback(ControllerTriggerType type, void* user)
{
    ((InputConfigPlayer*)user)->ControllerEventCallback(type);
}
