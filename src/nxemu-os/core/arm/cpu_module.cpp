#include "cpu_module.h"
#include "core/hle/kernel/k_process.h"
#include "core/hle/kernel/svc.h"
#include "core/core_timing.h"
#include <nxemu-module-spec/cpu.h>

namespace Core
{
class CpuModuleCallback : public ICpuInfo
{
public:
    explicit CpuModuleCallback(IArm64Executor *& arm64Executor, Core::System & system, Kernel::KProcess * process) :
        m_arm64Executor(arm64Executor),
        m_system(system),
        m_process(process),
        m_memory(process->GetMemory()),
        m_svn(0)
    {
    }

    uint32_t svn()
    {
        return m_svn;
    }

    uint64_t CpuTicks()
    {
        return m_system.CoreTiming().GetClockTicks();
    }

    void ServiceCall(uint32_t index)
    {
        m_svn = index;
        m_arm64Executor->HaltExecution(IArm64Executor::HaltReason::SupervisorCall);
    }

    IMemory & Memory() override
    {
        return m_memory;
    }

    bool ReadMemory(uint64_t addr, uint8_t * Buffer, uint32_t Len)
    {
        return m_memory.ReadBlock(addr, Buffer, Len);
    }

    bool WriteMemory(uint64_t addr, const uint8_t * buffer, uint32_t len)
    {
        return m_memory.WriteBlock(addr, buffer, len);
    }

    IArm64Executor *& m_arm64Executor;
    Kernel::KProcess * m_process{};
    Core::System & m_system;
    Core::Memory::Memory & m_memory;
    uint32_t m_svn;
};

ArmCpuModule::ArmCpuModule(Core::System & system, bool is64Bit, bool usesWallClock, Kernel::KProcess * process, uint32_t coreIndex) :
    ArmInterface{usesWallClock},
    m_system(system),
    m_cb(std::make_unique<CpuModuleCallback>(m_arm64Executor, system, process)),
    m_arm64Executor(nullptr)
{
    if (is64Bit)
    {
        m_arm64Executor = system.GetSwitchSystem().Cpu().CreateArm64Executor(process->GetExclusiveMonitor(), *m_cb, coreIndex);    
    }
}

ArmCpuModule::~ArmCpuModule()
{
    if (m_arm64Executor != nullptr)
    {
        m_system.GetSwitchSystem().Cpu().DestroyArm64Executor(m_arm64Executor);
        m_arm64Executor = nullptr;
    }
}

Architecture ArmCpuModule::GetArchitecture() const
{
    UNIMPLEMENTED();
    return Architecture::AArch64;
}

HaltReason ArmCpuModule::RunThread(Kernel::KThread * thread)
{
    if (m_arm64Executor != nullptr)
    {
        IArm64Executor::HaltReason reason = m_arm64Executor->Execute();
        switch (reason)
        {
        case IArm64Executor::HaltReason::BreakLoop: return HaltReason::BreakLoop;
        case IArm64Executor::HaltReason::SupervisorCall: return HaltReason::SupervisorCall;
        case IArm64Executor::HaltReason::SupervisorCallBreakLoop: return (HaltReason::SupervisorCall | HaltReason::BreakLoop);
        default:
            UNIMPLEMENTED();
        }
    }
    UNIMPLEMENTED();
    return HaltReason::DataAbort;
}

HaltReason ArmCpuModule::StepThread(Kernel::KThread * thread)
{
    UNIMPLEMENTED();
    return HaltReason::DataAbort;
}

void ArmCpuModule::GetContext(Kernel::Svc::ThreadContext & ctx) const
{
    if (m_arm64Executor != nullptr)
    {
        IArm64Reg & reg = m_arm64Executor->Reg();
        for (size_t i = 0; i < 29; i++)
        {
            ctx.r[i] = reg.Get64((IArm64Reg::Reg)(((uint32_t)IArm64Reg::Reg::X0) + i));
        }
        ctx.fp = reg.Get64(IArm64Reg::Reg::FP);
        ctx.lr = reg.Get64(IArm64Reg::Reg::LR);

        ctx.sp = reg.Get64(IArm64Reg::Reg::SP);
        ctx.pc = reg.Get64(IArm64Reg::Reg::PC);
        ctx.pstate = reg.Get32(IArm64Reg::Reg::PSTATE);
        for (size_t i = 0; i < 32; i++)
        {
            reg.Get128((IArm64Reg::Reg)(((uint32_t)IArm64Reg::Reg::Q0) + i), ctx.v[i][1], ctx.v[i][0]);
        }
        ctx.fpcr = reg.GetFPCR();
        ctx.fpsr = reg.GetFPSR();
        ctx.tpidr = reg.Get64(IArm64Reg::Reg::TPIDR_EL0);
    }
    else
    {
        UNIMPLEMENTED();
    }
}

void ArmCpuModule::SetContext(const Kernel::Svc::ThreadContext & ctx)
{
    if (m_arm64Executor != nullptr)
    {
        IArm64Reg & reg = m_arm64Executor->Reg();
        for (size_t i = 0; i < 29; i++)
        {
            reg.Set64((IArm64Reg::Reg)(((uint32_t)IArm64Reg::Reg::X0) + i), ctx.r[i]);
        }
        reg.Set64(IArm64Reg::Reg::FP, ctx.fp);
        reg.Set64(IArm64Reg::Reg::LR, ctx.lr);
        reg.Set64(IArm64Reg::Reg::SP, ctx.sp);
        reg.Set64(IArm64Reg::Reg::PC, ctx.pc);
        reg.Set32(IArm64Reg::Reg::PSTATE, ctx.pstate);
        for (size_t i = 0; i < 32; i++)
        {
            reg.Set128((IArm64Reg::Reg)(((uint32_t)IArm64Reg::Reg::Q0) + i), ctx.v[i][1], ctx.v[i][0]);
        }
        reg.SetFPCR(ctx.fpcr);
        reg.SetFPSR(ctx.fpsr);
        reg.Set64(IArm64Reg::Reg::TPIDR_EL0, ctx.tpidr);
    }
    else
    {
        UNIMPLEMENTED();
    }
}

void ArmCpuModule::SetTpidrroEl0(u64 value)
{
    if (m_arm64Executor != nullptr)
    {
        IArm64Reg & reg = m_arm64Executor->Reg();
        reg.Set64(IArm64Reg::Reg::TPIDRRO_EL0, value);
    }
    else
    {    
        UNIMPLEMENTED();
    }
}

void ArmCpuModule::GetSvcArguments(std::span<uint64_t, 8> args) const
{
    IArm64Reg & reg = m_arm64Executor->Reg();
    for (size_t i = 0; i < 8; i++)
    {
        args[i] = reg.Get64((IArm64Reg::Reg)(((uint32_t)IArm64Reg::Reg::X0) + i));
    }
}

void ArmCpuModule::SetSvcArguments(std::span<const uint64_t, 8> args)
{
    IArm64Reg & reg = m_arm64Executor->Reg();
    for (size_t i = 0; i < 8; i++)
    {
        reg.Set64((IArm64Reg::Reg)(((uint32_t)IArm64Reg::Reg::X0) + i), args[i]);
    }
}

u32 ArmCpuModule::GetSvcNumber() const
{
    return m_cb->svn();
}

void ArmCpuModule::SignalInterrupt(Kernel::KThread * thread)
{
    if (m_arm64Executor != nullptr)
    {
        m_arm64Executor->HaltExecution(IArm64Executor::HaltReason::BreakLoop);
    }
    else
    {
        UNIMPLEMENTED();
    }
}

void ArmCpuModule::ClearInstructionCache()
{
    UNIMPLEMENTED();
}

void ArmCpuModule::InvalidateCacheRange(u64 addr, std::size_t size)
{
    if (m_arm64Executor != nullptr)
    {
        m_arm64Executor->InvalidateCacheRange(addr, size);
    }
    else
    {
        UNIMPLEMENTED();
    }
}

const Kernel::DebugWatchpoint * ArmCpuModule::HaltedWatchpoint() const
{
    UNIMPLEMENTED();
    return nullptr;
}

void ArmCpuModule::RewindBreakpointInstruction()
{
    UNIMPLEMENTED();
}

}
