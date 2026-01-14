// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <yuzu_common/settings.h>
#include "arm_dynarmic_64.h"
#include "dynarmic/interface/exclusive_monitor.h"
#include <common/maths.h>
#include <yuzu_common/settings.h>

extern IModuleNotification * g_notify;

class DynarmicCallbacks64 :
    public Dynarmic::A64::UserCallbacks
{
public:
    explicit DynarmicCallbacks64(ArmDynarmic64 & parent, ICpuInfo & cpuInfo, IKernelProcess & process) :
        m_parent{parent}, 
        m_memory(process.GetMemory()),
        m_CpuInfo(cpuInfo),
        m_process(process),
        m_debugger_enabled(false),
        m_check_memory_access{m_debugger_enabled || !Settings::values.cpuopt_ignore_memory_aborts.GetValue()}
    {
    }

    uint8_t MemoryRead8(uint64_t vaddr) override
    {
        if (m_check_memory_access)
        {
            CheckMemoryAccess(vaddr, 1, CpuDebugWatchpointType::Read);        
        }
        return m_memory.Read8(vaddr);
    }
    uint16_t MemoryRead16(uint64_t vaddr) override
    {
        if (m_check_memory_access)
        {
            CheckMemoryAccess(vaddr, 2, CpuDebugWatchpointType::Read);
        }
        return m_memory.Read16(vaddr);
    }
    uint32_t MemoryRead32(uint64_t vaddr) override
    {
        if (m_check_memory_access)
        {
            CheckMemoryAccess(vaddr, 4, CpuDebugWatchpointType::Read);
        }
        return m_memory.Read32(vaddr);
    }
    uint64_t MemoryRead64(uint64_t vaddr) override
    {
        if (m_check_memory_access)
        {
            CheckMemoryAccess(vaddr, 8, CpuDebugWatchpointType::Read);
        }
        return m_memory.Read64(vaddr);
    }
    Dynarmic::A64::Vector MemoryRead128(uint64_t vaddr)
    {
        Dynarmic::A64::Vector Value;
        if (m_memory.ReadBlock(vaddr, (uint8_t *)&Value, sizeof(Value)))
        {
            return Value;
        }
        return {0, 0};
    }

    void MemoryWrite8(uint64_t vaddr, uint8_t value) override
    {
        if (!m_check_memory_access || CheckMemoryAccess(vaddr, 1, CpuDebugWatchpointType::Write))
        {
            m_memory.Write8(vaddr, value);
        }
    }

    void MemoryWrite16(uint64_t vaddr, uint16_t value) override
    {
        if (!m_check_memory_access || CheckMemoryAccess(vaddr, 2, CpuDebugWatchpointType::Write))
        {
            m_memory.Write16(vaddr, value);
        }
    }

    void MemoryWrite32(uint64_t vaddr, uint32_t value) override
    {
        if (!m_check_memory_access || CheckMemoryAccess(vaddr, 4, CpuDebugWatchpointType::Write))
        {
            m_memory.Write32(vaddr, value);
        }
    }
    void MemoryWrite64(uint64_t vaddr, uint64_t value) override
    {
        if (!m_check_memory_access || CheckMemoryAccess(vaddr, 8, CpuDebugWatchpointType::Write))
        {
            m_memory.Write64(vaddr, value);
        }
    }
    void MemoryWrite128(std::uint64_t vaddr, Dynarmic::A64::Vector value)
    {
        if (!m_check_memory_access || CheckMemoryAccess(vaddr, 16, CpuDebugWatchpointType::Write))
        {
            m_memory.Write64(vaddr, value[0]);
            m_memory.Write64(vaddr + 8, value[1]);
        }
    }

    bool MemoryWriteExclusive8(u64 vaddr, std::uint8_t value, std::uint8_t expected) override
    {
        return CheckMemoryAccess(vaddr, 1, CpuDebugWatchpointType::Write) && m_memory.WriteExclusive8(vaddr, value, expected);
    }
    bool MemoryWriteExclusive16(u64 vaddr, std::uint16_t value, std::uint16_t expected) override
    {
        return CheckMemoryAccess(vaddr, 2, CpuDebugWatchpointType::Write) && m_memory.WriteExclusive16(vaddr, value, expected);
    }
    bool MemoryWriteExclusive32(u64 vaddr, std::uint32_t value, std::uint32_t expected) override
    {
        return CheckMemoryAccess(vaddr, 4, CpuDebugWatchpointType::Write) && m_memory.WriteExclusive32(vaddr, value, expected);
    }
    bool MemoryWriteExclusive64(u64 vaddr, std::uint64_t value, std::uint64_t expected) override
    {
        return CheckMemoryAccess(vaddr, 8, CpuDebugWatchpointType::Write) && m_memory.WriteExclusive64(vaddr, value, expected);
    }
    bool MemoryWriteExclusive128(std::uint64_t vaddr, Dynarmic::A64::Vector value, Dynarmic::A64::Vector expected)
    {
        return CheckMemoryAccess(vaddr, 16, CpuDebugWatchpointType::Write) && m_memory.WriteExclusive128(vaddr, value[1], value[0], expected[1], expected[0]);
    }

    void InterpreterFallback(std::uint64_t /*pc*/, size_t /*num_instructions*/)
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
    }

    void ExceptionRaised(std::uint64_t /*pc*/, Dynarmic::A64::Exception /*exception*/)
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
    }

    void CallSVC(std::uint32_t swi)
    {
        m_CpuInfo.ServiceCall(swi);
    }

    bool IsReadOnlyMemory(std::uint64_t /*vaddr*/)
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
        return false;
    }

    void DataCacheOperationRaised(Dynarmic::A64::DataCacheOperation /*op*/, std::uint64_t /*value*/)
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
    }

    void InstructionCacheOperationRaised(Dynarmic::A64::InstructionCacheOperation /*op*/, std::uint64_t /*value*/)
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
    }

    void InstructionSynchronizationBarrierRaised()
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
    }

    void AddTicks(std::uint64_t /*ticks*/)
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
    }

    std::uint64_t GetTicksRemaining()
    {
        g_notify->BreakPoint(__FILE__, __LINE__);
        return 0;
    }

    std::uint64_t GetCNTPCT()
    {
        const uint64_t BASE_CLOCK_RATE = 1019215872; // Switch clock speed - 1020MHz
        const uint64_t COUNT_FREQ = 19200000;

        uint64_t ticks = m_CpuInfo.CpuTicks();
        uint64_t hi, rem;
        uint64_t lo = mull128_u64(ticks, COUNT_FREQ, &hi);
        return div128_to_64(hi, lo, BASE_CLOCK_RATE, &rem);
    }

    bool CheckMemoryAccess(uint64_t /*addr*/, uint64_t /*size*/, CpuDebugWatchpointType /*type*/)
    {
        if (!m_check_memory_access)
        {
            return true;
        }
        g_notify->BreakPoint(__FILE__, __LINE__);
        return true;
    }

    ArmDynarmic64 & m_parent;
    IMemory & m_memory;
    ICpuInfo & m_CpuInfo;
    IKernelProcess & m_process;
    u64 m_tpidr_el0{};
    const bool m_debugger_enabled{};
    const bool m_check_memory_access{};
};

ArmDynarmic64::ArmDynarmic64(Dynarmic::ExclusiveMonitor & monitor, ISystemModules & modules, ICpuInfo & cpuInfo, IKernelProcess & process, uint32_t coreIndex) :
    m_jit(nullptr),
    m_modules(modules),
    m_CpuInfo(cpuInfo),
    m_memory(process.GetMemory()),
    m_OperatingSystem(modules.OperatingSystem()),
    m_monitor(monitor),
    m_cb(std::make_unique<DynarmicCallbacks64>(*this, cpuInfo, process)),
    m_process(process),
    m_coreIndex(coreIndex)
{
    m_jit = MakeJit(monitor);
    m_reg.SetJit(m_jit.get());
}

ArmDynarmic64::~ArmDynarmic64()
{
}

CpuHaltReason ArmDynarmic64::Execute()
{
    m_jit->ClearExclusiveState();
    Dynarmic::HaltReason Reason = m_jit->Run(); 
    switch (Reason)
    {
    case Dynarmic::HaltReason::UserDefined2: return CpuHaltReason::BreakLoop;
    case Dynarmic::HaltReason::UserDefined3: return CpuHaltReason::SupervisorCall;
    case (Dynarmic::HaltReason::UserDefined2and3): return CpuHaltReason::SupervisorCallBreakLoop;
    }

    g_notify->BreakPoint(__FILE__, __LINE__);
    return CpuHaltReason::BreakLoop;
}

void ArmDynarmic64::InvalidateCacheRange(uint64_t addr, uint64_t size)
{
    m_jit->InvalidateCacheRange(addr, size);
}

void ArmDynarmic64::HaltExecution(CpuHaltReason hr)
{
    switch (hr)
    {
    case CpuHaltReason::BreakLoop: m_jit->HaltExecution(Dynarmic::HaltReason::UserDefined2); break;
    case CpuHaltReason::SupervisorCall: m_jit->HaltExecution(Dynarmic::HaltReason::UserDefined3); break;
    default:
        g_notify->BreakPoint(__FILE__, __LINE__);
    }
}

void ArmDynarmic64::Release()
{
    delete this;
}

void ArmDynarmic64::GetContext(CpuThreadContext & ctx) const
{
    Dynarmic::A64::Jit& j = *m_jit;
    auto gpr = j.GetRegisters();

    // TODO: this is inconvenient
    for (size_t i = 0; i < 29; i++)
    {
        ctx.r[i] = gpr[i];
    }
    ctx.fp = gpr[29];
    ctx.lr = gpr[30];

    ctx.sp = j.GetSP();
    ctx.pc = j.GetPC();
    ctx.pstate = j.GetPstate();
    memcpy(ctx.v, j.GetVectorData(), sizeof(ctx.v));
    ctx.fpcr = j.GetFpcr();
    ctx.fpsr = j.GetFpsr();
    ctx.tpidr = m_cb->m_tpidr_el0;
}

std::unique_ptr<Dynarmic::A64::Jit> ArmDynarmic64::MakeJit(Dynarmic::ExclusiveMonitor & monitor)
{
    Dynarmic::A64::UserConfig config;
    config.callbacks = m_cb.get();
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

