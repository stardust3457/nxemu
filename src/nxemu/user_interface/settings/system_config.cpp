#include "config_setting.h"
#include "system_config.h"
#include "system_config_audio.h"
#include "system_config_debug.h"
#include "system_config_graphics.h"
#include <common/std_string.h>
#include <nxemu-core/machine/switch_system.h>
#include <nxemu-core/settings/identifiers.h>
#include <nxemu-core/settings/settings.h>
#include <nxemu-core/notification.h>
#include <sciter_ui.h>
#include <sciter_element.h>
#include <sciter_handler.h>
#include <widgets/page_nav.h>
#include <yuzu_common/settings_enums.h>

SystemConfig::SystemConfig(ISciterUI & SciterUI, std::vector<VkDeviceRecord> & vkDeviceRecords) :
    m_sciterUI(SciterUI),
    m_vkDeviceRecords(vkDeviceRecords),
    m_window(nullptr)
{
}

SystemConfig::~SystemConfig()
{
}

void SystemConfig::Display(void * parentWindow)
{
    InitializeTranslations();

    enum
    {
        WINDOW_HEIGHT = 300,
        WINDOW_WIDTH = 300,
    };

    if (!m_sciterUI.WindowCreate(parentWindow, "system_config.html", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SUIW_CHILD, m_window))
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
}

void SystemConfig::SavePage(SCITER_ELEMENT pageElement, const ConfigSetting* settings, size_t settingsCount)
{
    SettingsStore & settingsStore = SettingsStore::GetInstance();
    SciterElement page(pageElement);

    for (size_t i = 0; i < settingsCount; ++i)
    {
        const ConfigSetting& setting = settings[i];
        if (setting.Type() == ConfigSettingType::ComboBox)
        {
            SaveComboBox(page, setting, true);
        }
        else if (setting.Type() == ConfigSettingType::ComboBoxValue)
        {
            SaveComboBox(page, setting, false);
        }
        else if (setting.Type() == ConfigSettingType::CheckBox)
        {
            SciterElement element = page.GetElementByID(setting.ElementId());
            if (element)
            {
                settingsStore.SetBool(setting.StoreSettingId(), (element.GetState() & SciterElement::STATE_CHECKED) != 0);
            }
        }
        else if (setting.Type() == ConfigSettingType::Slider)
        {
            SciterElement element = page.GetElementByID(setting.ElementId());
            if (element)
            {
                SciterValue value = element.GetValue();
                if (value.isInt())
                {
                    settingsStore.SetInt(setting.StoreSettingId(), value.GetValueInt());
                }
            }
        }
        else if (setting.Type() == ConfigSettingType::InputText)
        {
            SciterElement element = page.GetElementByID(setting.ElementId());
            if (element)
            {
                SciterValue value = element.GetValue();
                if (value.isString())
                {
                    settingsStore.SetString(setting.StoreSettingId(), value.GetValueStr().c_str());
                }
            }
        }
        else
        {
            g_notify->BreakPoint(__FILE__, __LINE__);
        }
    }
}

void SystemConfig::SetupPage(SCITER_ELEMENT pageElement, const ConfigSetting * settings, size_t settingsCount)
{
    SettingsStore & settingsStore = SettingsStore::GetInstance();
    SciterElement page(pageElement);
    bool emulationRunning = settingsStore.GetBool(NXCoreSetting::EmulationRunning);

    for (size_t i = 0; i < settingsCount; ++i) 
    {
        const ConfigSetting & setting = settings[i];
        if (emulationRunning && !setting.CanChangeWhenRunning())
        {
            SciterElement element = page.GetElementByID(setting.ElementId());
            if (element)
            {
                element.SetState(SciterElement::STATE_DISABLED, 0, true);
                element = page.FindFirst("[for='%s']", setting.ElementId());
                if (element)
                {
                    element.SetState(SciterElement::STATE_DISABLED, 0, true);
                }
            }
        }
        if (setting.Type() == ConfigSettingType::ComboBox)
        {
            SetupComboBox(page, setting);
        }
        else if (setting.Type() == ConfigSettingType::ComboBoxValue)
        {
            // do nothing
        }
        else if (setting.Type() == ConfigSettingType::CheckBox)
        {
            SciterElement element = page.GetElementByID(setting.ElementId());
            if (element)
            {
                bool checked = settingsStore.GetBool(setting.StoreSettingId());
                element.SetState(checked ? SciterElement::STATE_CHECKED : 0, checked ? 0 : SciterElement::STATE_CHECKED, true);
            }
        }
        else if (setting.Type() == ConfigSettingType::Slider)
        {
            SciterElement element = page.GetElementByID(setting.ElementId());
            if (element)
            {
                element.SetValue(SciterValue(settingsStore.GetInt(setting.StoreSettingId())));
            }
        }
        else if (setting.Type() == ConfigSettingType::InputText)
        {
            SciterElement element = page.GetElementByID(setting.ElementId());
            if (element)
            {
                element.SetValue(SciterValue(std::string(settingsStore.GetString(setting.StoreSettingId()))));
            }
        }
        else
        {
            g_notify->BreakPoint(__FILE__, __LINE__);
        }
    }
}

void SystemConfig::SaveComboBox(const SciterElement & page, const ConfigSetting & setting, bool intValue)
{
    SettingsStore & settingsStore = SettingsStore::GetInstance();
    std::shared_ptr<void> interfacePtr = m_sciterUI.GetElementInterface(page.GetElementByID(setting.ElementId()), IID_ICOMBOBOX);
    if (interfacePtr)
    {
        std::shared_ptr<IComboBox> comboBox = std::static_pointer_cast<IComboBox>(interfacePtr);
        SciterElement element = comboBox->GetSelectedItem();
        if (element)
        {
            std::string value = element.GetAttribute("value");
            if (value.size() > 0)
            {
                if (intValue)
                {
                    settingsStore.SetInt(setting.StoreSettingId(), std::stoi(value.c_str()));
                }
                else
                {
                    settingsStore.SetString(setting.StoreSettingId(), value.c_str());
                }
            }
        }
    }
}

void SystemConfig::SetupComboBox(const SciterElement & page, const ConfigSetting & setting)
{
    SettingsStore & settingsStore = SettingsStore::GetInstance();
    std::shared_ptr<void> interfacePtr = m_sciterUI.GetElementInterface(page.GetElementByID(setting.ElementId()), IID_ICOMBOBOX);
    SettingTranslationMap::iterator itr = m_settingTranslations.find(setting.SettingIndex());
    if (interfacePtr && itr != m_settingTranslations.end())
    {
        int32_t settingValue = settingsStore.GetInt(setting.StoreSettingId());
        std::shared_ptr<IComboBox> comboBox = std::static_pointer_cast<IComboBox>(interfacePtr);
        SettingTranslationList & translation = itr->second;
        int32_t selectedIndex = -1;
        for (size_t i = 0, n = translation.size(); i < n; i++)
        {
            int32_t index = comboBox->AddItem(translation[i].second.c_str(), stdstr_f("%d", translation[i].first).c_str());
            if (settingValue == translation[i].first)
            {
                selectedIndex = index;
            }
        }
        if (selectedIndex >= 0)
        {
            comboBox->SelectItem(selectedIndex);
        }
    }
}

bool SystemConfig::PageNavChangeFrom(const std::string & /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
    return true;
}

bool SystemConfig::PageNavChangeTo(const std::string & /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
    return true;
}

void SystemConfig::PageNavCreatedPage(const std::string & pageName, SCITER_ELEMENT page)
{
    if (pageName == "Audio")
    {
        m_systemConfigAudio.reset(new SystemConfigAudio(m_sciterUI, *this, m_window->GetHandle(), page));
    }
    else if (pageName == "Debug")
    {
        m_systemConfigDebug.reset(new SystemConfigDebug(m_sciterUI, *this, m_window->GetHandle(), page));
    }
    else if (pageName == "Graphics")
    {
        m_systemConfigGraphics.reset(new SystemConfigGraphics(m_sciterUI, *this, m_window->GetHandle(), page));
    }   
}

void SystemConfig::PageNavPageChanged(const std::string & /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
}

bool SystemConfig::OnClick(SCITER_ELEMENT element, SCITER_ELEMENT /*source*/, uint32_t /*reason*/)
{
    SciterElement clickElem(element);
    if (clickElem.GetAttribute("role") == "window-ok")
    {
        if (m_systemConfigAudio)
        {
            m_systemConfigAudio->SaveSetting();
        }
        if (m_systemConfigDebug)
        {
            m_systemConfigDebug->SaveSetting();
        }
        if (m_systemConfigGraphics)
        {
            m_systemConfigGraphics->SaveSetting();
        }
        SwitchSystem * system = SwitchSystem::GetInstance();
        if (system != nullptr)
        {
            system->FlushSettings();
        }
        m_window->Destroy();
    }
    return false;
}

void SystemConfig::InitializeTranslations()
{
    if (!m_settingTranslations.empty())
    {
        return;
    }

    m_settingTranslations.insert({ Settings::EnumMetadata<Settings::RendererBackend>::Index(), {
        {(uint32_t)Settings::RendererBackend::OpenGL, "OpenGL"},
        {(uint32_t)Settings::RendererBackend::Vulkan, "Vulkan"},
        {(uint32_t)Settings::RendererBackend::Null, "Null"},
    }});

    m_settingTranslations.insert({ Settings::EnumMetadata<Settings::ShaderBackend>::Index(), {
        {(uint32_t)Settings::ShaderBackend::Glsl, "GLSL"},
        {(uint32_t)Settings::ShaderBackend::Glasm, "GLASM (Assembly Shaders, NVIDIA Only)"},
        {(uint32_t)Settings::ShaderBackend::SpirV, "SPIR-V (Experimental, AMD/Mesa Only)"},
    }});

    m_settingTranslations.insert({ Settings::EnumMetadata<Settings::AstcDecodeMode>::Index(), {
        {(uint32_t)Settings::AstcDecodeMode::Cpu, "CPU"},
        {(uint32_t)Settings::AstcDecodeMode::Gpu, "GPU"},
        {(uint32_t)Settings::AstcDecodeMode::CpuAsynchronous, "CPU Asynchronous"},
    }});

    m_settingTranslations.insert({ Settings::EnumMetadata<Settings::NvdecEmulation>::Index(), {
        {(uint32_t)Settings::NvdecEmulation::Off, "No Video Output"},
        {(uint32_t)Settings::NvdecEmulation::Cpu, "CPU Video Decoding"},
        {(uint32_t)Settings::NvdecEmulation::Gpu, "GPU Video Decoding (Default)"},
    }});

    m_settingTranslations.insert({ Settings::EnumMetadata<Settings::FullscreenMode>::Index(), {
        {(uint32_t)Settings::FullscreenMode::Borderless, "Borderless Windowed"},
        {(uint32_t)Settings::FullscreenMode::Exclusive, "Exclusive Fullscreen"},
    }});

    m_settingTranslations.insert({ Settings::EnumMetadata<Settings::AspectRatio>::Index(), {
        {(uint32_t)Settings::AspectRatio::R16_9, "Default (16:9)"},
        {(uint32_t)Settings::AspectRatio::R4_3, "Force 4:3"},
        {(uint32_t)Settings::AspectRatio::R21_9, "Force 21:9"},
        {(uint32_t)Settings::AspectRatio::R16_10, "Force 16:10"},
        {(uint32_t)Settings::AspectRatio::Stretch, "Stretch to Window"},
    }});
    m_settingTranslations.insert({ Settings::EnumMetadata<Settings::ResolutionSetup>::Index(), {
        {(uint32_t)Settings::ResolutionSetup::Res1_2X, "0.5X (360p/540p) [EXPERIMENTAL]"},
        {(uint32_t)Settings::ResolutionSetup::Res3_4X, "0.75X (540p/810p) [EXPERIMENTAL]"},
        {(uint32_t)Settings::ResolutionSetup::Res1X, "1X (720p/1080p)"},
        {(uint32_t)Settings::ResolutionSetup::Res3_2X, "1.5X (1080p/1620p) [EXPERIMENTAL]"},
        {(uint32_t)Settings::ResolutionSetup::Res2X, "2X (1440p/2160p)"},
        {(uint32_t)Settings::ResolutionSetup::Res3X, "3X (2160p/3240p)"},
        {(uint32_t)Settings::ResolutionSetup::Res4X, "4X (2880p/4320p)"},
        {(uint32_t)Settings::ResolutionSetup::Res5X, "5X (3600p/5400p)"},
        {(uint32_t)Settings::ResolutionSetup::Res6X, "6X (4320p/6480p)"},
        {(uint32_t)Settings::ResolutionSetup::Res7X, "7X (5040p/7560p)"},
        {(uint32_t)Settings::ResolutionSetup::Res8X, "8X (5760p/8640p)"},
    }});
    m_settingTranslations.insert({ Settings::EnumMetadata<Settings::ScalingFilter>::Index(), {
        {(uint32_t)Settings::ScalingFilter::NearestNeighbor, "Nearest Neighbor"},
        {(uint32_t)Settings::ScalingFilter::Bilinear, "Bilinear"},
        {(uint32_t)Settings::ScalingFilter::Bicubic, "Bicubic"},
        {(uint32_t)Settings::ScalingFilter::Gaussian, "Gaussian"},
        {(uint32_t)Settings::ScalingFilter::ScaleForce, "ScaleForce"},
        {(uint32_t)Settings::ScalingFilter::Fsr, "AMD FidelityFX Super Resolution"},
    }});
    m_settingTranslations.insert({ Settings::EnumMetadata<Settings::AntiAliasing>::Index(), {
        {(uint32_t)Settings::AntiAliasing::None, "None"},
        {(uint32_t)Settings::AntiAliasing::Fxaa, "FXAA"},
        {(uint32_t)Settings::AntiAliasing::Smaa, "SMAA"},
    }});
    m_settingTranslations.insert({ Settings::EnumMetadata<Settings::GpuAccuracy>::Index(), {
        {(uint32_t)Settings::GpuAccuracy::Normal, "Normal"},
        {(uint32_t)Settings::GpuAccuracy::High, "High"},
        {(uint32_t)Settings::GpuAccuracy::Extreme, "Extreme"},
    }});
    m_settingTranslations.insert({ Settings::EnumMetadata<Settings::AnisotropyMode>::Index(), {
        {(uint32_t)Settings::AnisotropyMode::Automatic, "Automatic"},
        {(uint32_t)Settings::AnisotropyMode::Default, "Default"},
        {(uint32_t)Settings::AnisotropyMode::X2, "2x"},
        {(uint32_t)Settings::AnisotropyMode::X4, "4x"},
        {(uint32_t)Settings::AnisotropyMode::X8, "8x"},
        {(uint32_t)Settings::AnisotropyMode::X16, "16x"},
    }});
    m_settingTranslations.insert({ Settings::EnumMetadata<Settings::AstcRecompression>::Index(), {
        {(uint32_t)Settings::AstcRecompression::Uncompressed, "Uncompressed (Best quality)"},
        {(uint32_t)Settings::AstcRecompression::Bc1, "BC1 (Low quality)"},
        {(uint32_t)Settings::AstcRecompression::Bc3, "BC3 (Medium quality)"},
    }});
    m_settingTranslations.insert({ Settings::EnumMetadata<Settings::VramUsageMode>::Index(), {
        {(uint32_t)Settings::VramUsageMode::Conservative, "Conservative"},
        {(uint32_t)Settings::VramUsageMode::Aggressive, "Aggressive"},
    }});
    SettingTranslationList vulkanDeviceTranslations;
    for (size_t i = 0; i < m_vkDeviceRecords.size(); ++i)
    {
        vulkanDeviceTranslations.push_back({ (int32_t)i, m_vkDeviceRecords[i].name });
    }
    m_settingTranslations.insert({ (uint32_t)SystemConfig::TranslationType::VulkanDevice, vulkanDeviceTranslations });

    m_settingTranslations.insert({ Settings::EnumMetadata<Settings::AudioMode>::Index(), {
        {(uint32_t)Settings::AudioMode::Mono, "Mono"},
        {(uint32_t)Settings::AudioMode::Stereo, "Stereo"},
        {(uint32_t)Settings::AudioMode::Surround, "Surround"},
    }});
}