#include "cpu_manager.h"
#include "arm_dynarmic_64.h"
#include "exclusive_monitor_interface.h"

CpuManager::CpuManager(ISystemModules & modules) :
    m_modules(modules)
{
}

CpuManager::~CpuManager()
{
}

bool CpuManager::Initialize(void)
{
    return true;
}

IExclusiveMonitor * CpuManager::CreateExclusiveMonitor(IMemory & memory, uint32_t processorCount)
{
    if (m_exclusiveMonitor.get() != nullptr)
    {
        return nullptr;
    }
    m_exclusiveMonitor.reset(std::make_unique<ExclusiveMonitor>(memory, processorCount).release());
    return m_exclusiveMonitor.get();
};

void CpuManager::DestroyExclusiveMonitor(IExclusiveMonitor * monitor)
{
    if (m_exclusiveMonitor.get() == monitor)
    {
        m_exclusiveMonitor.reset(nullptr);
    }
}

IArm64Executor * CpuManager::CreateArm64Executor(IExclusiveMonitor * monitor, ICpuInfo & info, uint32_t coreIndex)
{
    return new ArmDynarmic64(monitor == m_exclusiveMonitor.get() ? m_exclusiveMonitor.get() : nullptr, m_modules, info, coreIndex);
}

void CpuManager::DestroyArm64Executor(IArm64Executor * executor)
{
    delete (ArmDynarmic64 *)executor;
}
