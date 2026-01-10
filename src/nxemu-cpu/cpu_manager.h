#pragma once
#include <nxemu-module-spec/cpu.h>
#include "dynarmic/interface/exclusive_monitor.h"

class CpuInterface :
    public ICpu
{
public:
    CpuInterface(ISystemModules & modules, uint32_t processorCount);
    ~CpuInterface();

    //ICpu
    bool Initialize(void) override;
    IExclusiveMonitor * CreateExclusiveMonitor(IMemory & memory) override;
    IArm64Executor * CreateArm64Executor(ICpuInfo & info, bool is64Bit, bool usesWallClock, uint32_t coreIndex) override;
    void DestroyArm64Executor(IArm64Executor * executor) override;

private:
    CpuInterface() = delete;
    CpuInterface(const CpuInterface &) = delete;
    CpuInterface & operator=(const CpuInterface &) = delete;

    ISystemModules & m_modules;
    Dynarmic::ExclusiveMonitor m_monitor;
};
