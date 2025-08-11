#include "system_config_graphics.h"
#include "config_setting.h"
#include "system_config.h"
#include <common/std_string.h>
#include <yuzu_common/settings_enums.h>
#include <nxemu-video/video_settings_identifiers.h>
#include <widgets/combo_box.h>

namespace 
{
    static ConfigSetting graphicsSettings[] = {
        ConfigSetting(ConfigSetting::ComboBox, "GraphicsAPI", Settings::EnumMetadata<Settings::RendererBackend>::Index(), NXVideoSetting::GraphicsAPI),
        ConfigSetting(ConfigSetting::ComboBox, "ShaderBackend", Settings::EnumMetadata<Settings::ShaderBackend>::Index(), NXVideoSetting::ShaderBackend),
        ConfigSetting(ConfigSetting::ComboBox, "VulkanDevices", (uint32_t)SystemConfig::TranslationType::VulkanDevice, NXVideoSetting::VulkanDevice),
        ConfigSetting(ConfigSetting::CheckBox, "UseDiskPipelineCache", NXVideoSetting::UseDiskPipelineCache),
        ConfigSetting(ConfigSetting::CheckBox, "UseAsynchronousGPUEmulation", NXVideoSetting::UseAsynchronousGPUEmulation),
        ConfigSetting(ConfigSetting::ComboBox, "AstcDecodeMode", Settings::EnumMetadata<Settings::AstcDecodeMode>::Index(), NXVideoSetting::AstcDecodeMode),
        ConfigSetting(ConfigSetting::ComboBox, "NvdecEmulation", Settings::EnumMetadata<Settings::NvdecEmulation>::Index(), NXVideoSetting::NvdecEmulation),
        ConfigSetting(ConfigSetting::ComboBox, "FullscreenMode", Settings::EnumMetadata<Settings::FullscreenMode>::Index(), NXVideoSetting::FullscreenMode),
        ConfigSetting(ConfigSetting::ComboBox, "AspectRatio", Settings::EnumMetadata<Settings::AspectRatio>::Index(), NXVideoSetting::AspectRatio),
        ConfigSetting(ConfigSetting::ComboBox, "ResolutionSetup", Settings::EnumMetadata<Settings::ResolutionSetup>::Index(), NXVideoSetting::ResolutionSetup),
        ConfigSetting(ConfigSetting::ComboBox, "ScalingFilter", Settings::EnumMetadata<Settings::ScalingFilter>::Index(), NXVideoSetting::ScalingFilter),
        ConfigSetting(ConfigSetting::ComboBox, "AntiAliasing", Settings::EnumMetadata<Settings::AntiAliasing>::Index(), NXVideoSetting::AntiAliasing),
        ConfigSetting(ConfigSetting::Slider, "FSPSharpness", NXVideoSetting::FSPSharpness),
    };

    static ConfigSetting advancedSettings[] = {
        ConfigSetting(ConfigSetting::CheckBox, "EnableAsynchronousPresentation", NXVideoSetting::EnableAsynchronousPresentation),
        ConfigSetting(ConfigSetting::CheckBox, "ForceMaximumClocks", NXVideoSetting::ForceMaximumClocks),
        ConfigSetting(ConfigSetting::CheckBox, "EnableReactiveFlushing", NXVideoSetting::EnableReactiveFlushing),
        ConfigSetting(ConfigSetting::CheckBox, "UseAsynchronousShaderBuilding", NXVideoSetting::UseAsynchronousShaderBuilding),
        ConfigSetting(ConfigSetting::CheckBox, "FastGPUTime", NXVideoSetting::FastGPUTime),
        ConfigSetting(ConfigSetting::CheckBox, "UseVulkanPipelineCache", NXVideoSetting::UseVulkanPipelineCache),
        ConfigSetting(ConfigSetting::CheckBox, "SyncToFramerateOfVideoPlayback", NXVideoSetting::SyncToFramerateOfVideoPlayback),
        ConfigSetting(ConfigSetting::CheckBox, "BarrierFeedbackLoops", NXVideoSetting::BarrierFeedbackLoops),
    };
}

SystemConfigGraphics::SystemConfigGraphics(ISciterUI & sciterUI, SystemConfig & config, HWINDOW parent, SciterElement page) :
    m_sciterUI(sciterUI),
    m_config(config),
    m_parent(parent),
    m_page(page),
    m_graphicsPage(nullptr),
    m_advancedPage(nullptr)
{
    SciterElement pageNav = page.GetElementByID("GraphicsTabNav");
    std::shared_ptr<void> interfacePtr = pageNav.IsValid() ? m_sciterUI.GetElementInterface(pageNav, IID_IPAGENAV) : nullptr;
    if (interfacePtr)
    {
        m_pageNav = std::static_pointer_cast<IPageNav>(interfacePtr);
        m_pageNav->AddSink(this);
    }
    m_sciterUI.AttachHandler(page.GetElementByID("FSPSharpness"), IID_ISTATECHANGESINK, (IStateChangeSink*)this);
}

void SystemConfigGraphics::SaveSetting(void)
{
    if (m_graphicsPage != nullptr)
    {
        m_config.SavePage(m_graphicsPage, graphicsSettings, sizeof(graphicsSettings) / sizeof(graphicsSettings[0]));
    }
    if (m_advancedPage != nullptr)
    {
        m_config.SavePage(m_advancedPage, advancedSettings, sizeof(advancedSettings) / sizeof(advancedSettings[0]));
    }
}

bool SystemConfigGraphics::PageNavChangeFrom(const std::string & /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
    return true;
}

bool SystemConfigGraphics::PageNavChangeTo(const std::string & /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
    return true;
}

void SystemConfigGraphics::PageNavCreatedPage(const std::string & pageName, SCITER_ELEMENT page)
{
    if (pageName == "Graphics")
    {
        SetupGraphicsPage(page);
    }
    else if (pageName == "Advanced")
    {
        SetupAdvancedPage(page);
    }
}

void SystemConfigGraphics::PageNavPageChanged(const std::string & /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
}

bool SystemConfigGraphics::OnStateChange(SCITER_ELEMENT elem, uint32_t /*eventReason*/, void* /*data*/)
{
    if (m_graphicsPage && elem == m_graphicsPage.GetElementByID("GraphicsAPI"))
    {
        UpdateGraphicsAPI();
    }
    else if (m_graphicsPage && elem == m_graphicsPage.GetElementByID("FSPSharpness"))
    {
        UpdateFSPSharpnessDisplay();
    }
    return false;
}

void SystemConfigGraphics::SetupAdvancedPage(SciterElement page)
{
    m_advancedPage = page;
    m_config.SetupPage(page, advancedSettings, sizeof(advancedSettings) / sizeof(advancedSettings[0]));
}

void SystemConfigGraphics::SetupGraphicsPage(SciterElement page)
{
    m_graphicsPage = page;
    m_config.SetupPage(page, graphicsSettings, sizeof(graphicsSettings) / sizeof(graphicsSettings[0]));
    m_sciterUI.AttachHandler(page.GetElementByID("GraphicsAPI"), IID_ISTATECHANGESINK, (IStateChangeSink*)this);
    UpdateGraphicsAPI();
    UpdateFSPSharpnessDisplay();
}

void SystemConfigGraphics::UpdateGraphicsAPI()
{
    std::shared_ptr<void> interfacePtr = m_graphicsPage ? m_sciterUI.GetElementInterface(m_graphicsPage.GetElementByID("GraphicsAPI"), IID_ICOMBOBOX) : nullptr;
    if (!interfacePtr)
    {
        return;
    }
    std::shared_ptr<IComboBox> comboBox = std::static_pointer_cast<IComboBox>(interfacePtr);
    SciterElement element = comboBox->GetSelectedItem();
    if (!element)
    {
        return;
    }
    std::string value = element.GetAttribute("value");
    if (value.empty())
    {
        return;
    }

    SciterElement ShaderBackend = m_graphicsPage.GetElementByID("ShaderBackendRow");
    SciterElement VulkanDevices = m_graphicsPage.GetElementByID("VulkanDevicesRow");
    SciterElement VSyncMode = m_graphicsPage.GetElementByID("VSyncMode");    
    Settings::RendererBackend backend = (Settings::RendererBackend)std::stoi(value.c_str());
    switch (backend)
    {
    case Settings::RendererBackend::OpenGL:
        ShaderBackend.SetStyleAttribute("display", "block");
        VulkanDevices.SetStyleAttribute("display", "none");
        VSyncMode.RemoveAttribute("disabled");
        break;
    case Settings::RendererBackend::Vulkan:
        ShaderBackend.SetStyleAttribute("display", "none");
        VulkanDevices.SetStyleAttribute("display", "block");
        VSyncMode.RemoveAttribute("disabled");
        break;
    case Settings::RendererBackend::Null:
        ShaderBackend.SetStyleAttribute("display", "none");
        VulkanDevices.SetStyleAttribute("display", "none");
        VSyncMode.SetAttribute("disabled", "");
        break;
    }
}

void SystemConfigGraphics::UpdateFSPSharpnessDisplay()
{
    if (!m_graphicsPage)
    {
        return;
    }
    SciterElement fspSharpness = m_graphicsPage.GetElementByID("FSPSharpness");
    SciterElement fspSharpnessDisplay = m_graphicsPage.GetElementByID("FSPSharpnessDisplay");

    if (fspSharpness && fspSharpnessDisplay)
    {
        SciterValue value = fspSharpness.GetValue();
        if (value.isInt())
        {
            stdstr_f text("%d %%", value.GetValueInt());
            fspSharpnessDisplay.SetHTML((const uint8_t*)text.c_str(), text.size());
        }
    }
}