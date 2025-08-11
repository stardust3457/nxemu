#include "switch_system.h"
#include "notification.h"
#include "settings/core_settings.h"
#include "settings/identifiers.h"
#include "settings/settings.h"

std::unique_ptr<SwitchSystem> SwitchSystem::m_instance;

bool SwitchSystem::Create(IRenderWindow & window)
{
    ShutDown();
    m_instance.reset(new SwitchSystem());
    if (m_instance == nullptr)
    {
        return false;
    }
    return m_instance->Initialize(window);
}

void SwitchSystem::ShutDown(void)
{
    if (m_instance)
    {
        m_instance.reset();
    }
}

SwitchSystem * SwitchSystem::GetInstance()
{
    return m_instance.get();
}

SwitchSystem::SwitchSystem() :
    m_emulationRunning(false)
{
}

SwitchSystem::~SwitchSystem()
{
    StopEmulation();
}

bool SwitchSystem::Initialize(IRenderWindow & window)
{
    if (!m_modules.Initialize(window, *this))
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
        return false;
    }
    return true;
}

void SwitchSystem::StartEmulation(void)
{
    SettingsStore & settings = SettingsStore::GetInstance();
    settings.SetBool(NXCoreSetting::EmulationRunning, true);
    m_emulationRunning = true;
    m_modules.StartEmulation();
}

void SwitchSystem::StopEmulation(void)
{
    if (!m_emulationRunning)
    {
        return;
    }
    m_emulationRunning = false;
    m_modules.StopEmulation();
}

void SwitchSystem::FlushSettings(void)
{
    m_modules.FlushSettings();
    SaveCoreSetting();
}

IVideo & SwitchSystem::Video(void)
{
    return *m_modules.Video();
}

ISystemloader & SwitchSystem::Systemloader()
{
    return *m_modules.Systemloader();
}

IOperatingSystem & SwitchSystem::OperatingSystem()
{
    return *m_modules.OperatingSystem();
}

ICpu & SwitchSystem::Cpu(void)
{
    return *m_modules.Cpu();
}