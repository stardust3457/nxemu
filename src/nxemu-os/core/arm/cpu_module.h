#pragma once

#include "core/arm/arm_interface.h"
#include <memory>

__interface ISystemModules;
__interface IArm64Executor;

namespace Core
{
class System;
class CpuModuleCallback;

class ArmCpuModule final : public ArmInterface
{
public:
    ArmCpuModule(Core::System & system, bool is64Bit, bool usesWallClock, Kernel::KProcess * process, uint32_t coreIndex);
    ~ArmCpuModule();

    Architecture GetArchitecture() const override;
    HaltReason RunThread(Kernel::KThread * thread) override;
    HaltReason StepThread(Kernel::KThread * thread) override;

    void GetContext(Kernel::Svc::ThreadContext & ctx) const override;
    void SetContext(const Kernel::Svc::ThreadContext & ctx) override;
    void SetTpidrroEl0(u64 value) override;

    void GetSvcArguments(std::span<uint64_t, 8> args) const override;
    void SetSvcArguments(std::span<const uint64_t, 8> args) override;
    u32 GetSvcNumber() const override;

    void SignalInterrupt(Kernel::KThread * thread) override;
    void ClearInstructionCache() override;
    void InvalidateCacheRange(u64 addr, std::size_t size) override;

protected:
    const Kernel::DebugWatchpoint * HaltedWatchpoint() const override;
    void RewindBreakpointInstruction() override;

private:
    friend class CpuModuleCallback;
    Core::System & m_system;
    std::unique_ptr<CpuModuleCallback> m_cb{};
    IArm64Executor * m_arm64Executor;
};
}