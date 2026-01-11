#include "cpu_manager.h"
#include "arm_dynarmic_64.h"
#if defined(ARCHITECTURE_x86_64) || defined(ARCHITECTURE_arm64)
#include "exclusive_monitor_interface.h"
#endif

CpuInterface::CpuInterface(ISystemModules & modules, uint32_t processorCount) :
    m_modules(modules),
    m_monitor(processorCount)
{
}

CpuInterface::~CpuInterface()
{
}

bool CpuInterface::Initialize(void)
{
    return true;
}

IExclusiveMonitor * CpuInterface::CreateExclusiveMonitor(IMemory & memory)
{
#if defined(ARCHITECTURE_x86_64) || defined(ARCHITECTURE_arm64)
    return new ExclusiveMonitor(memory, m_monitor);
#else
    // TODO(merry): Passthrough exclusive monitor
    return nullptr;
#endif
}

ICpuCore * CpuInterface::CreateCpuCore(ICpuInfo & info, bool /*is64Bit*/, bool /*usesWallClock*/, uint32_t coreIndex)
{
    return new ArmDynarmic64(m_monitor, m_modules, info, coreIndex);
}   

