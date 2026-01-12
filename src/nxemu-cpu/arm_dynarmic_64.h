#pragma once
#include "dynarmic/interface/A64/a64.h"
#include "arm64_registers.h"
#include "cpu_manager.h"

class DynarmicCallbacks64;

class ArmDynarmic64 final :
    public ICpuCore
{
    friend class DynarmicCallbacks64;

public:
    ArmDynarmic64(Dynarmic::ExclusiveMonitor & monitor, ISystemModules & modules, ICpuInfo & cpuInfo, uint32_t coreIndex);

    IArm64Reg & Reg(void) { return m_reg; }

    //ICpuCore
    HaltReason Execute(void);
    void InvalidateCacheRange(uint64_t addr, uint64_t size) override;
    void HaltExecution(HaltReason hr);
    void Release() override;

private:
    ArmDynarmic64() = delete;
    ArmDynarmic64(const ArmDynarmic64 &) = delete;
    ArmDynarmic64 & operator=(const ArmDynarmic64 &) = delete;

    std::unique_ptr<Dynarmic::A64::Jit> MakeJit(Dynarmic::ExclusiveMonitor & monitor);

    std::unique_ptr<Dynarmic::A64::Jit> m_jit{};
    std::unique_ptr<DynarmicCallbacks64> m_cb{};
    ISystemModules & m_modules;
    ICpuInfo & m_CpuInfo;
    IMemory & m_memory;
    IOperatingSystem & m_OperatingSystem;
    Dynarmic::ExclusiveMonitor & m_monitor;
    A64Registers m_reg;
    uint32_t m_coreIndex;
};
