#pragma once
#include <nxemu-module-spec/cpu.h>
#include <memory>

class ExclusiveMonitor;

class CpuManager :
    public ICpu
{
public:
    CpuManager(ISystemModules & modules);
    ~CpuManager();

    //ICpu
    bool Initialize(void);
    IExclusiveMonitor * CreateExclusiveMonitor(IMemory & memory, uint32_t processorCount);
    void DestroyExclusiveMonitor(IExclusiveMonitor * monitor);
    IArm64Executor * CreateArm64Executor(IExclusiveMonitor * monitor, ICpuInfo & info, uint32_t coreIndex);
    void DestroyArm64Executor(IArm64Executor * executor);

private:
    CpuManager() = delete;
    CpuManager(const CpuManager &) = delete;
    CpuManager & operator=(const CpuManager &) = delete;

    std::unique_ptr<ExclusiveMonitor> m_exclusiveMonitor;
    ISystemModules & m_modules;
};
