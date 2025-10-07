#include "modules.h"
#include "notification.h"
#include "settings/core_settings.h"

Modules::Modules() :
    m_loaderModule(nullptr),
    m_cpuModule(nullptr),
    m_videoModule(nullptr),
    m_operatingsystemModule(nullptr)
{
    CreateModules();
}

Modules::~Modules()
{
    for (BaseModules::iterator itr = m_baseModules.begin(); itr != m_baseModules.end(); itr++)
    {
        (*itr)->ModuleCleanup();
    }
    m_baseModules.clear();
    if (m_cpu != nullptr && m_cpuModule.get() != nullptr)
    {
        m_cpuModule->DestroyCpu(m_cpu);
        m_cpu = nullptr;
    }
    if (m_video != nullptr && m_videoModule.get() != nullptr)
    {
        m_videoModule->DestroyVideo(m_video);
        m_video = nullptr;
    }
    if (m_operatingsystem != nullptr && m_operatingsystemModule.get() != nullptr)
    {
        m_operatingsystemModule->DestroyOS(m_operatingsystem);
        m_operatingsystem = nullptr;
    }
}

bool Modules::Initialize(IRenderWindow & RenderWindow, ISystemModules & modules)
{
    if (m_loaderModule.get() == nullptr || m_cpuModule.get() == nullptr || m_videoModule.get() == nullptr || m_operatingsystemModule.get() == nullptr)
    {
        return false;
    }
    m_systemLoader = m_loaderModule->CreateSystemLoader(modules);
    if (m_systemLoader == nullptr)
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
        return false;
    }
    m_cpu = m_cpuModule->CreateCpu(modules);
    if (m_cpu == nullptr)
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
        return false;
    }
    m_operatingsystem = m_operatingsystemModule->CreateOS(modules);
    if (m_operatingsystem == nullptr)
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
        return false;
    }
    m_video = m_videoModule->CreateVideo(RenderWindow, modules);
    if (m_video == nullptr)
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
        return false;
    }

    if (!m_systemLoader->Initialize())
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
        return false;
    }
    if (!m_cpu->Initialize())
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
        return false;
    }
    if (!m_video->Initialize())
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
        return false;
    }
    if (!m_operatingsystem->Initialize())
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
        return false;
    }
    return true;
}

void Modules::StartEmulation(void)
{
    for (BaseModules::iterator itr = m_baseModules.begin(); itr != m_baseModules.end(); itr++)
    {
        (*itr)->EmulationStarting();
    }
}

void Modules::StopEmulation(void)
{
    for (BaseModules::iterator itr = m_baseModules.begin(); itr != m_baseModules.end(); itr++)
    {
        (*itr)->EmulationStopping();
    }
}

void Modules::FlushSettings(void)
{
    for (BaseModules::iterator itr = m_baseModules.begin(); itr != m_baseModules.end(); itr++)
    {
        (*itr)->FlushSettings();
    }
}

void Modules::CreateModules(void)
{
    m_loaderFile = coreSettings.moduleLoader;
    m_cpuFile = coreSettings.moduleCpu;
    m_videoFile = coreSettings.moduleVideo;
    m_operatingsystemFile = coreSettings.moduleOs;

    LoadModule(m_loaderFile, m_loaderModule);
    LoadModule(m_cpuFile, m_cpuModule);
    LoadModule(m_videoFile, m_videoModule);
    LoadModule(m_operatingsystemFile, m_operatingsystemModule);

    if (m_cpuModule.get() != nullptr)
    {
        m_baseModules.push_back(m_cpuModule.get());
    }
    if (m_videoModule.get() != nullptr)
    {
        m_baseModules.push_back(m_videoModule.get());
    }
    if (m_operatingsystemModule.get() != nullptr)
    {
        m_baseModules.push_back(m_operatingsystemModule.get());
    }
}

ISystemloader * Modules::Systemloader(void)
{
    return m_systemLoader;
}

IVideo * Modules::Video(void)
{
    return m_video;
}

ICpu * Modules::Cpu(void)
{
    return m_cpu;
}

IOperatingSystem * Modules::OperatingSystem(void)
{
    return m_operatingsystem;
}

template <typename plugin_type>
void Modules::LoadModule(const std::string & fileName, std::unique_ptr<plugin_type> & plugin)
{
    Path fullPath((const char *)coreSettings.moduleDir, fileName.c_str());
    plugin = std::make_unique<plugin_type>();
    if (plugin.get() == nullptr || !fullPath.FileExists() || !plugin->Load(fullPath, &m_moduleNotification, &m_moduleSettings))
    {
        plugin = nullptr;
    }
}

template void Modules::LoadModule(const std::string & fileName, std::unique_ptr<CpuModule> & plugin);
template void Modules::LoadModule(const std::string & fileName, std::unique_ptr<VideoModule> & plugin);
template void Modules::LoadModule(const std::string & fileName, std::unique_ptr<OperatingSystemModule> & plugin);
