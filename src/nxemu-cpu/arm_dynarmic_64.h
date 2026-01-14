// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

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
    ArmDynarmic64(Dynarmic::ExclusiveMonitor & monitor, ISystemModules & modules, ICoreSystem & system, IKernelProcess & process, uint32_t coreIndex);
    ~ArmDynarmic64();

    IArm64Reg & Reg(void) { return m_reg; }

    // ICpuCore
    uint32_t GetSvcNumber() const override;
    CpuHaltReason Execute(void);
    void GetContext(CpuThreadContext & ctx) const override;
    void InvalidateCacheRange(uint64_t addr, uint64_t size) override;
    void HaltExecution(CpuHaltReason hr);
    void Release() override;

private:
    ArmDynarmic64() = delete;
    ArmDynarmic64(const ArmDynarmic64 &) = delete;
    ArmDynarmic64 & operator=(const ArmDynarmic64 &) = delete;

    std::unique_ptr<Dynarmic::A64::Jit> MakeJit(Dynarmic::ExclusiveMonitor & monitor);

    std::unique_ptr<Dynarmic::A64::Jit> m_jit{};
    std::unique_ptr<DynarmicCallbacks64> m_cb{};
    ISystemModules & m_modules;
    ICoreSystem & m_system;
    IMemory & m_memory;
    IOperatingSystem & m_OperatingSystem;
    Dynarmic::ExclusiveMonitor & m_monitor;
    uint32_t m_svc;
    A64Registers m_reg;
    IKernelProcess & m_process;
    uint32_t m_coreIndex;
};
