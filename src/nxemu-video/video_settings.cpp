#include "video_settings.h"
#include "video_settings_identifiers.h"
#include <common/json.h>
#include <nxemu-module-spec/base.h>
#include <yuzu_common/settings_enums.h>
#include <yuzu_common/settings.h>
#include <yuzu_common/yuzu_assert.h>

extern IModuleSettings * g_settings;

namespace
{
    enum class SettingType
    {
        Boolean,
        IntValue,
        IntValueRanged,
        RendererBackend,
        ShaderBackend,
        AstcDecodeMode,
        VSyncMode,
        NvdecEmulation,
        FullscreenMode,
        AspectRatio,
        ResolutionSetup,
        ScalingFilter,
        AntiAliasing,
        GpuAccuracy,
        AnisotropyMode,
        AstcRecompression,
        VramUsageMode,
    };

    class VideoSetting
    {
    public:
        VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<bool> * val);
        VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<int> * val);
        VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<int, true> * val);
        VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::RendererBackend, true> * val);
        VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::ShaderBackend, true> * val);
        VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::AstcDecodeMode, true> * val);
        VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::VSyncMode, true> * val);
        VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::NvdecEmulation> * val);
        VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::FullscreenMode, true> * val);
        VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::AspectRatio, true> * val);
        VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::ResolutionSetup> * val);
        VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::ScalingFilter> * val);
        VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::AntiAliasing> * val);
        VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::GpuAccuracy, true> * val);
        VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::AnisotropyMode, true> * val);
        VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::AstcRecompression, true> * val);
        VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::VramUsageMode, true> * val);

        const char * identifier;
        const char * json_section;
        const char * json_key;
        SettingType settingType;
        union
        {
            Settings::SwitchableSetting<bool> * boolValue;
            Settings::SwitchableSetting<int> * intValue;
            Settings::SwitchableSetting<int, true> * intValueRanged;
            Settings::SwitchableSetting<Settings::RendererBackend, true>* rendererBackend;
            Settings::SwitchableSetting<Settings::ShaderBackend, true> * shaderBackend;
            Settings::SwitchableSetting<Settings::AstcDecodeMode, true> * astcDecodeMode;
            Settings::SwitchableSetting<Settings::VSyncMode, true> * vSyncMode;
            Settings::SwitchableSetting<Settings::NvdecEmulation> * nvdecEmulation;
            Settings::SwitchableSetting<Settings::FullscreenMode, true> * fullscreenMode;
            Settings::SwitchableSetting<Settings::AspectRatio, true> * aspectRatio;
            Settings::SwitchableSetting<Settings::ResolutionSetup> * resolutionSetup;
            Settings::SwitchableSetting<Settings::ScalingFilter> * scalingFilter;
            Settings::SwitchableSetting<Settings::AntiAliasing> * antiAliasing;
            Settings::SwitchableSetting<Settings::GpuAccuracy, true> * gpuAccuracy;
            Settings::SwitchableSetting<Settings::AnisotropyMode, true> * anisotropyMode;
            Settings::SwitchableSetting<Settings::AstcRecompression, true> * astcRecompression;
            Settings::SwitchableSetting<Settings::VramUsageMode, true> * vramUsageMode;
        } setting;
    };

    static VideoSetting settings[] = {
        { NXVideoSetting::GraphicsAPI, "video", "renderer_backend", &Settings::values.renderer_backend },
        { NXVideoSetting::ShaderBackend, "video", "shader_backend", &Settings::values.shader_backend },
        { NXVideoSetting::VulkanDevice, "video", "vulkan_device", &Settings::values.vulkan_device },
        { NXVideoSetting::UseDiskPipelineCache, "video", "use_disk_shader_cache", &Settings::values.use_disk_shader_cache },
        { NXVideoSetting::UseAsynchronousGPUEmulation, "video", "use_asynchronous_gpu_emulation", &Settings::values.use_asynchronous_gpu_emulation },
        { NXVideoSetting::AstcDecodeMode, "video", "astc_decode_mode", &Settings::values.accelerate_astc },
        { NXVideoSetting::VSyncMode, "video", "vsync_mode", &Settings::values.vsync_mode },
        { NXVideoSetting::NvdecEmulation, "video", "nvdec_emulation", &Settings::values.nvdec_emulation },
        { NXVideoSetting::FullscreenMode, "video", "fullscreen_mode", &Settings::values.fullscreen_mode },
        { NXVideoSetting::AspectRatio, "video", "aspect_ratio", &Settings::values.aspect_ratio },
        { NXVideoSetting::ResolutionSetup, "video", "resolution_setup", &Settings::values.resolution_setup },
        { NXVideoSetting::ScalingFilter, "video", "scaling_filter", &Settings::values.scaling_filter },
        { NXVideoSetting::AntiAliasing, "video", "anti_aliasing", &Settings::values.anti_aliasing },
        { NXVideoSetting::FSPSharpness, "video", "fsr_sharpening_slider", &Settings::values.fsr_sharpening_slider },
        { NXVideoSetting::EnableAsynchronousPresentation, "video", "async_presentation", &Settings::values.async_presentation },
        { NXVideoSetting::ForceMaximumClocks, "video", "renderer_force_max_clock", &Settings::values.renderer_force_max_clock },
        { NXVideoSetting::EnableReactiveFlushing, "video", "use_reactive_flushing", &Settings::values.use_reactive_flushing },
        { NXVideoSetting::UseAsynchronousShaderBuilding, "video", "use_asynchronous_shaders", &Settings::values.use_asynchronous_shaders },
        { NXVideoSetting::FastGPUTime, "video", "use_fast_gpu_time", &Settings::values.use_fast_gpu_time },
        { NXVideoSetting::UseVulkanPipelineCache, "video", "use_vulkan_driver_pipeline_cache", &Settings::values.use_vulkan_driver_pipeline_cache },
        { NXVideoSetting::SyncToFramerateOfVideoPlayback, "video", "use_video_framerate", &Settings::values.use_video_framerate },
        { NXVideoSetting::BarrierFeedbackLoops, "video", "barrier_feedback_loops", &Settings::values.barrier_feedback_loops },
        { NXVideoSetting::AccuracyLevel, "video", "accuracy_level", &Settings::values.gpu_accuracy },
        { NXVideoSetting::AnisotropicFiltering, "video", "anisotropic_filtering", &Settings::values.max_anisotropy },
        { NXVideoSetting::ASTCRecompressionMethod, "video", "astc_recompression_method", &Settings::values.astc_recompression },
        { NXVideoSetting::VRAMUsageMode, "video", "vram_usage_mode", &Settings::values.vram_usage_mode },
    };
}

void VideoSettingChanged(const char * setting, void * /*userData*/)
{
    for (const VideoSetting & videoSetting : settings)
    {
        if (strcmp(videoSetting.identifier, setting) != 0)
        {
            continue;
        }
        switch (videoSetting.settingType)
        {
        case SettingType::Boolean:
            videoSetting.setting.boolValue->SetValue(g_settings->GetBool(setting));
            break;
        case SettingType::IntValue:
            videoSetting.setting.intValue->SetValue(g_settings->GetInt(setting));
            break;
        case SettingType::IntValueRanged:
            videoSetting.setting.intValueRanged->SetValue(g_settings->GetInt(setting));
            break;
        case SettingType::RendererBackend:
            videoSetting.setting.rendererBackend->SetValue((Settings::RendererBackend)g_settings->GetInt(setting));
            break;
        case SettingType::ShaderBackend:
            videoSetting.setting.shaderBackend->SetValue((Settings::ShaderBackend)g_settings->GetInt(setting));
            break;
        case SettingType::AstcDecodeMode:
            videoSetting.setting.astcDecodeMode->SetValue((Settings::AstcDecodeMode)g_settings->GetInt(setting));
            break;
        case SettingType::VSyncMode:
            videoSetting.setting.vSyncMode->SetValue((Settings::VSyncMode)g_settings->GetInt(setting));
            break;
        case SettingType::NvdecEmulation:
            videoSetting.setting.nvdecEmulation->SetValue((Settings::NvdecEmulation)g_settings->GetInt(setting));
            break;
        case SettingType::FullscreenMode:
            videoSetting.setting.fullscreenMode->SetValue((Settings::FullscreenMode)g_settings->GetInt(setting));
            break;
        case SettingType::AspectRatio:
            videoSetting.setting.aspectRatio->SetValue((Settings::AspectRatio)g_settings->GetInt(setting));
            break;
        case SettingType::ResolutionSetup:
            videoSetting.setting.resolutionSetup->SetValue((Settings::ResolutionSetup)g_settings->GetInt(setting));
            break;
        case SettingType::ScalingFilter:
            videoSetting.setting.scalingFilter->SetValue((Settings::ScalingFilter)g_settings->GetInt(setting));
            break;
        case SettingType::AntiAliasing:
            videoSetting.setting.antiAliasing->SetValue((Settings::AntiAliasing)g_settings->GetInt(setting));
            break;
        case SettingType::GpuAccuracy:
            videoSetting.setting.gpuAccuracy->SetValue((Settings::GpuAccuracy)g_settings->GetInt(setting));
            break;
        case SettingType::AnisotropyMode:
            videoSetting.setting.anisotropyMode->SetValue((Settings::AnisotropyMode)g_settings->GetInt(setting));
            break;
        case SettingType::AstcRecompression:
            videoSetting.setting.astcRecompression->SetValue((Settings::AstcRecompression)g_settings->GetInt(setting));
            break;
        case SettingType::VramUsageMode:
            videoSetting.setting.vramUsageMode->SetValue((Settings::VramUsageMode)g_settings->GetInt(setting));
            break;
        default:
            UNIMPLEMENTED();
        }
    }
}

void SetupVideoSetting(void)
{
    for (const VideoSetting & videoSetting : settings)
    {
        switch (videoSetting.settingType)
        {
        case SettingType::Boolean:
            videoSetting.setting.boolValue->SetValue(videoSetting.setting.boolValue->GetDefault());
            break;
        case SettingType::IntValue:
            videoSetting.setting.intValue->SetValue(videoSetting.setting.intValue->GetDefault());
            break;
        case SettingType::IntValueRanged:
            videoSetting.setting.intValueRanged->SetValue(videoSetting.setting.intValueRanged->GetDefault());
            break;
        case SettingType::RendererBackend:
            videoSetting.setting.rendererBackend->SetValue(videoSetting.setting.rendererBackend->GetDefault());
            break;
        case SettingType::ShaderBackend:
            videoSetting.setting.shaderBackend->SetValue(videoSetting.setting.shaderBackend->GetDefault());
            break;
        case SettingType::AstcDecodeMode:
            videoSetting.setting.astcDecodeMode->SetValue(videoSetting.setting.astcDecodeMode->GetDefault());
            break;
        case SettingType::VSyncMode:
            videoSetting.setting.vSyncMode->SetValue(videoSetting.setting.vSyncMode->GetDefault());
            break;
        case SettingType::NvdecEmulation:
            videoSetting.setting.nvdecEmulation->SetValue(videoSetting.setting.nvdecEmulation->GetDefault());
            break;
        case SettingType::FullscreenMode:
            videoSetting.setting.fullscreenMode->SetValue(videoSetting.setting.fullscreenMode->GetDefault());
            break;
        case SettingType::AspectRatio:
            videoSetting.setting.aspectRatio->SetValue(videoSetting.setting.aspectRatio->GetDefault());
            break;
        case SettingType::ResolutionSetup:
            videoSetting.setting.resolutionSetup->SetValue(videoSetting.setting.resolutionSetup->GetDefault());
            break;
        case SettingType::ScalingFilter:
            videoSetting.setting.scalingFilter->SetValue(videoSetting.setting.scalingFilter->GetDefault());
            break;
        case SettingType::AntiAliasing:
            videoSetting.setting.antiAliasing->SetValue(videoSetting.setting.antiAliasing->GetDefault());
            break;
        case SettingType::GpuAccuracy:
            videoSetting.setting.gpuAccuracy->SetValue(videoSetting.setting.gpuAccuracy->GetDefault());
            break;
        case SettingType::AnisotropyMode:
            videoSetting.setting.anisotropyMode->SetValue(videoSetting.setting.anisotropyMode->GetDefault());
            break;
        case SettingType::AstcRecompression:
            videoSetting.setting.astcRecompression->SetValue(videoSetting.setting.astcRecompression->GetDefault());
            break;
        case SettingType::VramUsageMode:
            videoSetting.setting.vramUsageMode->SetValue(videoSetting.setting.vramUsageMode->GetDefault());
            break;
        default:
            UNIMPLEMENTED();
        }
    }

    JsonValue root;
    JsonReader reader;
    std::string json = g_settings->GetSectionSettings("nxemu-video");

    if (!json.empty() && reader.Parse(json.data(), json.data() + json.size(), root))
    {
        for (const VideoSetting & videoSetting : settings)
        {
            JsonValue section = root[videoSetting.json_section];
            if (!section.isObject())
            {
                continue;
            }
            JsonValue value = section[videoSetting.json_key];
            switch (videoSetting.settingType)
            {
            case SettingType::Boolean:
                if (value.isBool())
                {
                    videoSetting.setting.boolValue->SetValue(value.asBool());
                }
                break;
            case SettingType::IntValue:
                if (value.isInt())
                {
                    videoSetting.setting.intValue->SetValue((int32_t)value.asInt64());
                }
                break;
            case SettingType::IntValueRanged:
                if (value.isInt())
                {
                    videoSetting.setting.intValueRanged->SetValue((int32_t)value.asInt64());
                }
                break;
            case SettingType::RendererBackend:
                if (value.isString())
                {
                    videoSetting.setting.rendererBackend->SetValue(Settings::ToEnum<Settings::RendererBackend>(value.asString()));
                }
                break;
            case SettingType::ShaderBackend:
                if (value.isString())
                {
                    videoSetting.setting.shaderBackend->SetValue(Settings::ToEnum<Settings::ShaderBackend>(value.asString()));
                }
                break;
            case SettingType::AstcDecodeMode:
                if (value.isString())
                {
                    videoSetting.setting.astcDecodeMode->SetValue(Settings::ToEnum<Settings::AstcDecodeMode>(value.asString()));
                }
                break;
            case SettingType::VSyncMode:
                if (value.isString())
                {
                    videoSetting.setting.vSyncMode->SetValue(Settings::ToEnum<Settings::VSyncMode>(value.asString()));
                }
                break;
            case SettingType::NvdecEmulation:
                if (value.isString())
                {
                    videoSetting.setting.nvdecEmulation->SetValue(Settings::ToEnum<Settings::NvdecEmulation>(value.asString()));
                }
                break;
            case SettingType::FullscreenMode:
                if (value.isString())
                {
                    videoSetting.setting.fullscreenMode->SetValue(Settings::ToEnum<Settings::FullscreenMode>(value.asString()));
                }
                break;
            case SettingType::AspectRatio:
                if (value.isString())
                {
                    videoSetting.setting.aspectRatio->SetValue(Settings::ToEnum<Settings::AspectRatio>(value.asString()));
                }
                break;
            case SettingType::ResolutionSetup:
                if (value.isString())
                {
                    videoSetting.setting.resolutionSetup->SetValue(Settings::ToEnum<Settings::ResolutionSetup>(value.asString()));
                }
                break;
            case SettingType::ScalingFilter:
                if (value.isString())
                {
                    videoSetting.setting.scalingFilter->SetValue(Settings::ToEnum<Settings::ScalingFilter>(value.asString()));
                }
                break;
            case SettingType::AntiAliasing:
                if (value.isString())
                {
                    videoSetting.setting.antiAliasing->SetValue(Settings::ToEnum<Settings::AntiAliasing>(value.asString()));
                }
                break;
            case SettingType::GpuAccuracy:
                if (value.isString())
                {
                    videoSetting.setting.gpuAccuracy->SetValue(Settings::ToEnum<Settings::GpuAccuracy>(value.asString()));
                }
                break;
            case SettingType::AnisotropyMode:
                if (value.isString())
                {
                    videoSetting.setting.anisotropyMode->SetValue(Settings::ToEnum<Settings::AnisotropyMode>(value.asString()));
                }
                break;
            case SettingType::AstcRecompression:
                if (value.isString())
                {
                    videoSetting.setting.astcRecompression->SetValue(Settings::ToEnum<Settings::AstcRecompression>(value.asString()));
                }
                break;
            case SettingType::VramUsageMode:
                if (value.isString())
                {
                    videoSetting.setting.vramUsageMode->SetValue(Settings::ToEnum<Settings::VramUsageMode>(value.asString()));
                }
                break;
            default:
                UNIMPLEMENTED();
            }
        }
    }

    for (const VideoSetting & videoSetting : settings)
    {
        switch (videoSetting.settingType)
        {
        case SettingType::Boolean:
            g_settings->SetDefaultBool(videoSetting.identifier, videoSetting.setting.boolValue->GetDefault());
            g_settings->SetBool(videoSetting.identifier, videoSetting.setting.boolValue->GetValue());
            break;
        case SettingType::IntValue:
            g_settings->SetDefaultInt(videoSetting.identifier, (int32_t)videoSetting.setting.intValue->GetDefault());
            g_settings->SetInt(videoSetting.identifier, (int32_t)videoSetting.setting.intValue->GetValue());
            break;
        case SettingType::IntValueRanged:
            g_settings->SetDefaultInt(videoSetting.identifier, (int32_t)videoSetting.setting.intValueRanged->GetDefault());
            g_settings->SetInt(videoSetting.identifier, (int32_t)videoSetting.setting.intValueRanged->GetValue());
            break;
        case SettingType::RendererBackend:
            g_settings->SetDefaultInt(videoSetting.identifier, (int32_t)videoSetting.setting.rendererBackend->GetDefault());
            g_settings->SetInt(videoSetting.identifier, (int32_t)videoSetting.setting.rendererBackend->GetValue());
            break;
        case SettingType::ShaderBackend:
            g_settings->SetDefaultInt(videoSetting.identifier, (int32_t)videoSetting.setting.shaderBackend->GetDefault());
            g_settings->SetInt(videoSetting.identifier, (int32_t)videoSetting.setting.shaderBackend->GetValue());
            break;
        case SettingType::AstcDecodeMode:
            g_settings->SetDefaultInt(videoSetting.identifier, (int32_t)videoSetting.setting.astcDecodeMode->GetDefault());
            g_settings->SetInt(videoSetting.identifier, (int32_t)videoSetting.setting.astcDecodeMode->GetValue());
            break;
        case SettingType::VSyncMode:
            g_settings->SetDefaultInt(videoSetting.identifier, (int32_t)videoSetting.setting.vSyncMode->GetDefault());
            g_settings->SetInt(videoSetting.identifier, (int32_t)videoSetting.setting.vSyncMode->GetValue());
            break;
        case SettingType::NvdecEmulation:
            g_settings->SetDefaultInt(videoSetting.identifier, (int32_t)videoSetting.setting.nvdecEmulation->GetDefault());
            g_settings->SetInt(videoSetting.identifier, (int32_t)videoSetting.setting.nvdecEmulation->GetValue());
            break;
        case SettingType::FullscreenMode:
            g_settings->SetDefaultInt(videoSetting.identifier, (int32_t)videoSetting.setting.fullscreenMode->GetDefault());
            g_settings->SetInt(videoSetting.identifier, (int32_t)videoSetting.setting.fullscreenMode->GetValue());
            break;
        case SettingType::AspectRatio:
            g_settings->SetDefaultInt(videoSetting.identifier, (int32_t)videoSetting.setting.aspectRatio->GetDefault());
            g_settings->SetInt(videoSetting.identifier, (int32_t)videoSetting.setting.aspectRatio->GetValue());
            break;
        case SettingType::ResolutionSetup:
            g_settings->SetDefaultInt(videoSetting.identifier, (int32_t)videoSetting.setting.resolutionSetup->GetDefault());
            g_settings->SetInt(videoSetting.identifier, (int32_t)videoSetting.setting.resolutionSetup->GetValue());
            break;
        case SettingType::ScalingFilter:
            g_settings->SetDefaultInt(videoSetting.identifier, (int32_t)videoSetting.setting.scalingFilter->GetDefault());
            g_settings->SetInt(videoSetting.identifier, (int32_t)videoSetting.setting.scalingFilter->GetValue());
            break;
        case SettingType::AntiAliasing:
            g_settings->SetDefaultInt(videoSetting.identifier, (int32_t)videoSetting.setting.antiAliasing->GetDefault());
            g_settings->SetInt(videoSetting.identifier, (int32_t)videoSetting.setting.antiAliasing->GetValue());
            break;
        case SettingType::GpuAccuracy:
            g_settings->SetDefaultInt(videoSetting.identifier, (int32_t)videoSetting.setting.gpuAccuracy->GetDefault());
            g_settings->SetInt(videoSetting.identifier, (int32_t)videoSetting.setting.gpuAccuracy->GetValue());
            break;
        case SettingType::AnisotropyMode:
            g_settings->SetDefaultInt(videoSetting.identifier, (int32_t)videoSetting.setting.anisotropyMode->GetDefault());
            g_settings->SetInt(videoSetting.identifier, (int32_t)videoSetting.setting.anisotropyMode->GetValue());
            break;
        case SettingType::AstcRecompression:
            g_settings->SetDefaultInt(videoSetting.identifier, (int32_t)videoSetting.setting.astcRecompression->GetDefault());
            g_settings->SetInt(videoSetting.identifier, (int32_t)videoSetting.setting.astcRecompression->GetValue());
            break;
        case SettingType::VramUsageMode:
            g_settings->SetDefaultInt(videoSetting.identifier, (int32_t)videoSetting.setting.vramUsageMode->GetDefault());
            g_settings->SetInt(videoSetting.identifier, (int32_t)videoSetting.setting.vramUsageMode->GetValue());
            break;
        default:
            UNIMPLEMENTED();
        }
        g_settings->RegisterCallback(videoSetting.identifier, VideoSettingChanged, nullptr);
    }
}

void SaveVideoSettings(void)
{
    typedef std::map<std::string, JsonValue> SectionMap;
    SectionMap sections;

    for (const VideoSetting & videoSetting : settings)
    {
        switch (videoSetting.settingType)
        {
        case SettingType::Boolean:
            if (videoSetting.setting.boolValue->GetValue() != videoSetting.setting.boolValue->GetDefault())
            {
                sections[videoSetting.json_section][videoSetting.json_key] = videoSetting.setting.boolValue->GetValue();
            }
            break;
        case SettingType::IntValue:
            if (videoSetting.setting.intValue->GetValue() != videoSetting.setting.intValue->GetDefault())
            {
                sections[videoSetting.json_section][videoSetting.json_key] = videoSetting.setting.intValue->GetValue();
            }
            break;
        case SettingType::IntValueRanged:
            if (videoSetting.setting.intValueRanged->GetValue() != videoSetting.setting.intValueRanged->GetDefault())
            {
                sections[videoSetting.json_section][videoSetting.json_key] = videoSetting.setting.intValueRanged->GetValue();
            }
            break;
        case SettingType::RendererBackend:
            if (videoSetting.setting.rendererBackend->GetValue() != videoSetting.setting.rendererBackend->GetDefault())
            {
                sections[videoSetting.json_section][videoSetting.json_key] = Settings::CanonicalizeEnum(videoSetting.setting.rendererBackend->GetValue());
            }
            break;
        case SettingType::ShaderBackend:
            if (videoSetting.setting.shaderBackend->GetValue() != videoSetting.setting.shaderBackend->GetDefault())
            {
                sections[videoSetting.json_section][videoSetting.json_key] = Settings::CanonicalizeEnum(videoSetting.setting.shaderBackend->GetValue());
            }
            break;
        case SettingType::AstcDecodeMode:
            if (videoSetting.setting.astcDecodeMode->GetValue() != videoSetting.setting.astcDecodeMode->GetDefault())
            {
                sections[videoSetting.json_section][videoSetting.json_key] = Settings::CanonicalizeEnum(videoSetting.setting.astcDecodeMode->GetValue());
            }
            break;
        case SettingType::VSyncMode:
            if (videoSetting.setting.vSyncMode->GetValue() != videoSetting.setting.vSyncMode->GetDefault())
            {
                sections[videoSetting.json_section][videoSetting.json_key] = Settings::CanonicalizeEnum(videoSetting.setting.vSyncMode->GetValue());
            }
            break;
        case SettingType::NvdecEmulation:
            if (videoSetting.setting.nvdecEmulation->GetValue() != videoSetting.setting.nvdecEmulation->GetDefault())
            {
                sections[videoSetting.json_section][videoSetting.json_key] = Settings::CanonicalizeEnum(videoSetting.setting.nvdecEmulation->GetValue());
            }
            break;
        case SettingType::FullscreenMode:
            if (videoSetting.setting.fullscreenMode->GetValue() != videoSetting.setting.fullscreenMode->GetDefault())
            {
                sections[videoSetting.json_section][videoSetting.json_key] = Settings::CanonicalizeEnum(videoSetting.setting.fullscreenMode->GetValue());
            }
            break;
        case SettingType::AspectRatio:
            if (videoSetting.setting.aspectRatio->GetValue() != videoSetting.setting.aspectRatio->GetDefault())
            {
                sections[videoSetting.json_section][videoSetting.json_key] = Settings::CanonicalizeEnum(videoSetting.setting.aspectRatio->GetValue());
            }
            break;
        case SettingType::ResolutionSetup:
            if (videoSetting.setting.resolutionSetup->GetValue() != videoSetting.setting.resolutionSetup->GetDefault())
            {
                sections[videoSetting.json_section][videoSetting.json_key] = Settings::CanonicalizeEnum(videoSetting.setting.resolutionSetup->GetValue());
            }
            break;
        case SettingType::ScalingFilter:
            if (videoSetting.setting.scalingFilter->GetValue() != videoSetting.setting.scalingFilter->GetDefault())
            {
                sections[videoSetting.json_section][videoSetting.json_key] = Settings::CanonicalizeEnum(videoSetting.setting.scalingFilter->GetValue());
            }
            break;
        case SettingType::AntiAliasing:
            if (videoSetting.setting.antiAliasing->GetValue() != videoSetting.setting.antiAliasing->GetDefault())
            {
                sections[videoSetting.json_section][videoSetting.json_key] = Settings::CanonicalizeEnum(videoSetting.setting.antiAliasing->GetValue());
            }
            break;
        case SettingType::GpuAccuracy:
            if (videoSetting.setting.gpuAccuracy->GetValue() != videoSetting.setting.gpuAccuracy->GetDefault())
            {
                sections[videoSetting.json_section][videoSetting.json_key] = Settings::CanonicalizeEnum(videoSetting.setting.gpuAccuracy->GetValue());
            }
            break;
        case SettingType::AnisotropyMode:
            if (videoSetting.setting.anisotropyMode->GetValue() != videoSetting.setting.anisotropyMode->GetDefault())
            {
                sections[videoSetting.json_section][videoSetting.json_key] = Settings::CanonicalizeEnum(videoSetting.setting.anisotropyMode->GetValue());
            }
            break;
        case SettingType::AstcRecompression:
            if (videoSetting.setting.astcRecompression->GetValue() != videoSetting.setting.astcRecompression->GetDefault())
            {
                sections[videoSetting.json_section][videoSetting.json_key] = Settings::CanonicalizeEnum(videoSetting.setting.astcRecompression->GetValue());
            }
            break;
        case SettingType::VramUsageMode:
            if (videoSetting.setting.vramUsageMode->GetValue() != videoSetting.setting.vramUsageMode->GetDefault())
            {
                sections[videoSetting.json_section][videoSetting.json_key] = Settings::CanonicalizeEnum(videoSetting.setting.vramUsageMode->GetValue());
            }
            break;
        default:
            UNIMPLEMENTED();
        }
    }

    JsonValue json;
    for (SectionMap::const_iterator it = sections.begin(); it != sections.end(); ++it)
    {
        if (it->second.size() > 0)
        {
            json[it->first] = it->second;
        }
    }
    g_settings->SetSectionSettings("nxemu-video", json.isNull() ? "" : JsonStyledWriter().write(json));
}

namespace
{
    VideoSetting::VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<bool> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::Boolean)
    {
        setting.boolValue = val;
    }

    VideoSetting::VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<int> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::IntValue)
    {
        setting.intValue = val;
    }

    VideoSetting::VideoSetting(const char* id, const char* section, const char* key, Settings::SwitchableSetting<int, true>* val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::IntValueRanged)
    {
        setting.intValueRanged = val;
    }

    VideoSetting::VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::RendererBackend, true> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::RendererBackend)
    {
        setting.rendererBackend = val;
    }

    VideoSetting::VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::ShaderBackend, true> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::ShaderBackend)
    {
        setting.shaderBackend = val;
    }

    VideoSetting::VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::AstcDecodeMode, true> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::AstcDecodeMode)
    {
        setting.astcDecodeMode = val;
    }

    VideoSetting::VideoSetting(const char* id, const char * section, const char * key, Settings::SwitchableSetting<Settings::VSyncMode, true> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::VSyncMode)
    {
        setting.vSyncMode = val;
    }

    VideoSetting::VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::NvdecEmulation> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::NvdecEmulation)
    {
        setting.nvdecEmulation = val;
    }

    VideoSetting::VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::FullscreenMode, true> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::FullscreenMode)
    {
        setting.fullscreenMode = val;
    }

    VideoSetting::VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::AspectRatio, true> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::AspectRatio)
    {
        setting.aspectRatio = val;
    }

    VideoSetting::VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::ResolutionSetup> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::ResolutionSetup)
    {
        setting.resolutionSetup = val;
    }

    VideoSetting::VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::ScalingFilter> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::ScalingFilter)
    {
        setting.scalingFilter = val;
    }

    VideoSetting::VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::AntiAliasing> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::AntiAliasing)
    {
        setting.antiAliasing = val;
    }

    VideoSetting::VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::GpuAccuracy, true> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::GpuAccuracy)
    {
        setting.gpuAccuracy = val;
    }

    VideoSetting::VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::AnisotropyMode, true> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::AnisotropyMode)
    {
        setting.anisotropyMode = val;
    }

    VideoSetting::VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::AstcRecompression, true> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::AstcRecompression)
    {
        setting.astcRecompression = val;
    }

    VideoSetting::VideoSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::VramUsageMode, true> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::VramUsageMode)
    {
        setting.vramUsageMode = val;
    }
}