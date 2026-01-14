#pragma once

#include <nxemu-module-spec/cpu.h>
#include "core/hardware_properties.h"
#include <memory>
#include <span>

__interface ISystemModules;
__interface IArm64Executor;

namespace Kernel
{
class KProcess;
class KThread;
}

namespace Core
{
class System;
class CpuModuleCallback;

enum class HaltReason : uint64_t
{
    StepThread = 0x00000001,
    DataAbort = 0x00000004,
    BreakLoop = 0x02000000,
    SupervisorCall = 0x04000000,
    InstructionBreakpoint = 0x08000000,
    PrefetchAbort = 0x20000000,
};

class ArmCpuModule
{
    using WatchpointArray = std::array<CpuDebugWatchpoint, Core::Hardware::NUM_WATCHPOINTS>;

public:
    ArmCpuModule(Core::System & system, bool is64Bit, bool usesWallClock, Kernel::KProcess * process, uint32_t coreIndex);
    ~ArmCpuModule();

    void Initialize();
    void LockThread(Kernel::KThread * thread);
    void UnlockThread(Kernel::KThread * thread);
    ProcessorArchitecture GetArchitecture() const;

    HaltReason RunThread(Kernel::KThread * thread);
    HaltReason StepThread(Kernel::KThread * thread);

    void GetContext(CpuThreadContext & ctx) const;
    void SetContext(const CpuThreadContext & ctx);
    void SetTpidrroEl0(uint64_t value);

    void GetSvcArguments(std::span<uint64_t, 8> args) const;
    void SetSvcArguments(std::span<const uint64_t, 8> args);
    uint32_t GetSvcNumber() const;

    void SignalInterrupt(Kernel::KThread * thread);
    void ClearInstructionCache();
    void InvalidateCacheRange(uint64_t addr, std::uint64_t size);

    void RewindBreakpointInstruction();
    void SetWatchpointArray(const CpuDebugWatchpoint * watchpoints, uint32_t count);

protected:
    const CpuDebugWatchpoint * HaltedWatchpoint() const;

private:
    friend class CpuModuleCallback;
    Core::System & m_system;
    std::unique_ptr<CpuModuleCallback> m_cb{};
    ICpuCore * m_cpuCore;
    WatchpointArray m_watchpoints{};
};
}