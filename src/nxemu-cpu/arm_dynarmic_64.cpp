#include "arm_dynarmic_64.h"
#include "dynarmic/interface/exclusive_monitor.h"
#include <common/maths.h>

extern IModuleNotification * g_notify;

ArmDynarmic64::ArmDynarmic64(Dynarmic::ExclusiveMonitor & monitor, ISystemModules & modules, ICpuInfo & cpuInfo, uint32_t coreIndex) :
    m_jit(nullptr),
    m_modules(modules),
    m_CpuInfo(cpuInfo),
    m_memory(cpuInfo.Memory()),
    m_OperatingSystem(modules.OperatingSystem()),
    m_monitor(monitor),
    m_coreIndex(coreIndex)
{
    m_jit = MakeJit(monitor);
    m_reg.SetJit(m_jit.get());
}

IArm64Executor::HaltReason ArmDynarmic64::Execute()
{
    m_jit->ClearExclusiveState();
    Dynarmic::HaltReason Reason = m_jit->Run(); 
    switch (Reason)
    {
    case Dynarmic::HaltReason::UserDefined2: return IArm64Executor::HaltReason::BreakLoop;
    case Dynarmic::HaltReason::UserDefined3: return IArm64Executor::HaltReason::SupervisorCall;
    case (Dynarmic::HaltReason::UserDefined2and3): return IArm64Executor::HaltReason::SupervisorCallBreakLoop;
    }

    g_notify->BreakPoint(__FILE__, __LINE__);
    return HaltReason::Stopped;
}

void ArmDynarmic64::InvalidateCacheRange(uint64_t addr, uint64_t size)
{
    m_jit->InvalidateCacheRange(addr, size);
}

void ArmDynarmic64::HaltExecution(HaltReason hr)
{
    switch (hr)
    {
    case HaltReason::BreakLoop: m_jit->HaltExecution(Dynarmic::HaltReason::UserDefined2); break;
    case HaltReason::SupervisorCall: m_jit->HaltExecution(Dynarmic::HaltReason::UserDefined3); break;
    default:
        g_notify->BreakPoint(__FILE__, __LINE__);
    }
}

std::unique_ptr<Dynarmic::A64::Jit> ArmDynarmic64::MakeJit(Dynarmic::ExclusiveMonitor & monitor)
{
    Dynarmic::A64::UserConfig config;
    config.callbacks = this;
    config.page_table = nullptr; // reinterpret_cast<void **>(page_table->pointers.data());
    config.page_table_address_space_bits = 27; //address_space_bits;
    config.page_table_pointer_mask_bits = 2;   // Common::PageTable::ATTRIBUTE_BITS;
    config.silently_mirror_page_table = false;
    config.absolute_offset_page_table = true;
    config.detect_misaligned_access_via_page_table = 16 | 32 | 64 | 128;
    config.only_detect_misalignment_via_page_table_on_page_boundary = true;

    config.fastmem_pointer = nullptr; //page_table->fastmem_arena;
    config.fastmem_address_space_bits = 27; //address_space_bits;
    config.silently_mirror_fastmem = false;

    config.fastmem_exclusive_access = config.fastmem_pointer != nullptr;
    config.recompile_on_exclusive_fastmem_failure = true;
    config.processor_id = m_coreIndex;
    config.global_monitor = &monitor;

    // System registers
    config.tpidrro_el0 = &m_reg.m_tpidrro_el0;
    config.tpidr_el0 = &m_reg.m_tpidr_el0;
    config.dczid_el0 = 4;
    config.ctr_el0 = 0x8444c004;
    config.cntfrq_el0 = 0x124f800; //Hardware::CNTFREQ;

    // Unpredictable instructions
    config.define_unpredictable_behaviour = true;

    // Timing
    config.wall_clock_cntpct = true; //m_uses_wall_clock;
    config.enable_cycle_counting = false; //!m_uses_wall_clock;

    // Code cache size
    config.code_cache_size = 0x20000000;
    return std::make_unique<Dynarmic::A64::Jit>(config);
}

std::uint8_t ArmDynarmic64::MemoryRead8(std::uint64_t vaddr)
{
    uint8_t Value;
    if (m_CpuInfo.ReadMemory(vaddr, (uint8_t *)&Value, sizeof(Value)))
    {
        return Value;
    }
    return 0;
}

std::uint16_t ArmDynarmic64::MemoryRead16(std::uint64_t vaddr)
{
    uint16_t Value;
    if (m_CpuInfo.ReadMemory(vaddr, (uint8_t *)&Value, sizeof(Value)))
    {
        return Value;
    }
    return 0;
}

std::uint32_t ArmDynarmic64::MemoryRead32(std::uint64_t vaddr)
{
    return m_memory.Read32(vaddr);
}

std::uint64_t ArmDynarmic64::MemoryRead64(std::uint64_t vaddr)
{
    return m_memory.Read64(vaddr);
}

Dynarmic::A64::Vector ArmDynarmic64::MemoryRead128(std::uint64_t vaddr)
{
    Dynarmic::A64::Vector Value;
    if (m_CpuInfo.ReadMemory(vaddr, (uint8_t *)&Value, sizeof(Value)))
    {
        return Value;
    }
    return {0, 0};
}

void ArmDynarmic64::MemoryWrite8(std::uint64_t vaddr, std::uint8_t value)
{
    m_memory.Write8(vaddr, value);
}

void ArmDynarmic64::MemoryWrite16(std::uint64_t vaddr, std::uint16_t value)
{
    m_memory.Write16(vaddr, value);
}

void ArmDynarmic64::MemoryWrite32(std::uint64_t vaddr, std::uint32_t value)
{
    m_memory.Write32(vaddr, value);
}

void ArmDynarmic64::MemoryWrite64(std::uint64_t vaddr, std::uint64_t value)
{
    m_memory.Write64(vaddr, value);
}

void ArmDynarmic64::MemoryWrite128(std::uint64_t vaddr, Dynarmic::A64::Vector value)
{
    m_CpuInfo.WriteMemory(vaddr, (const uint8_t *)&value, sizeof(value));
}

bool ArmDynarmic64::MemoryWriteExclusive8(std::uint64_t vaddr, std::uint8_t value, std::uint8_t /*expected*/)
{
    return m_CpuInfo.WriteMemory(vaddr, (const uint8_t*)&value, sizeof(value));
}

bool ArmDynarmic64::MemoryWriteExclusive16(std::uint64_t vaddr, std::uint16_t value, std::uint16_t /*expected*/)
{
    return m_CpuInfo.WriteMemory(vaddr, (const uint8_t*)&value, sizeof(value));
}

bool ArmDynarmic64::MemoryWriteExclusive32(std::uint64_t vaddr, std::uint32_t value, std::uint32_t /*expected*/)
{
    return m_CpuInfo.WriteMemory(vaddr, (const uint8_t *)&value, sizeof(value));
}

bool ArmDynarmic64::MemoryWriteExclusive64(std::uint64_t vaddr, std::uint64_t value, std::uint64_t /*expected*/)
{
    return m_CpuInfo.WriteMemory(vaddr, (const uint8_t *)&value, sizeof(value));
}

bool ArmDynarmic64::MemoryWriteExclusive128(std::uint64_t vaddr, Dynarmic::A64::Vector value, Dynarmic::A64::Vector /*expected*/)
{
    return m_CpuInfo.WriteMemory(vaddr, (const uint8_t*)&value, sizeof(value));
}

bool ArmDynarmic64::IsReadOnlyMemory(std::uint64_t /*vaddr*/)
{
    g_notify->BreakPoint(__FILE__, __LINE__);
    return false;
}

void ArmDynarmic64::InterpreterFallback(std::uint64_t /*pc*/, size_t /*num_instructions*/)
{
    g_notify->BreakPoint(__FILE__, __LINE__);
}

void ArmDynarmic64::CallSVC(std::uint32_t swi)
{
    m_CpuInfo.ServiceCall(swi);
}

void ArmDynarmic64::ExceptionRaised(std::uint64_t /*pc*/, Dynarmic::A64::Exception /*exception*/)
{
    g_notify->BreakPoint(__FILE__, __LINE__);
}

void ArmDynarmic64::DataCacheOperationRaised(Dynarmic::A64::DataCacheOperation /*op*/, std::uint64_t /*value*/)
{
    g_notify->BreakPoint(__FILE__, __LINE__);
}

void ArmDynarmic64::InstructionCacheOperationRaised(Dynarmic::A64::InstructionCacheOperation /*op*/, std::uint64_t /*value*/)
{
    g_notify->BreakPoint(__FILE__, __LINE__);
}

void ArmDynarmic64::InstructionSynchronizationBarrierRaised()
{
    g_notify->BreakPoint(__FILE__, __LINE__);
}

void ArmDynarmic64::AddTicks(std::uint64_t /*ticks*/)
{
    g_notify->BreakPoint(__FILE__, __LINE__);
}

std::uint64_t ArmDynarmic64::GetTicksRemaining()
{
    g_notify->BreakPoint(__FILE__, __LINE__);
    return 0;
}

std::uint64_t ArmDynarmic64::GetCNTPCT()
{
    const uint64_t BASE_CLOCK_RATE = 1019215872; // Switch clock speed - 1020MHz
    const uint64_t COUNT_FREQ = 19200000;

    uint64_t ticks = m_CpuInfo.CpuTicks();
    uint64_t hi, rem;
    uint64_t lo = mull128_u64(ticks, COUNT_FREQ, &hi);
    return div128_to_64(hi, lo, BASE_CLOCK_RATE, &rem);
}
