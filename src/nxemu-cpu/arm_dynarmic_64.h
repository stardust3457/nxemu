#pragma once
#include "dynarmic/interface/A64/a64.h"
#include "arm64_registers.h"
#include "cpu_manager.h"

class ArmDynarmic64 :
    public IArm64Executor,
    private Dynarmic::A64::UserCallbacks
{
public:
    ArmDynarmic64(Dynarmic::ExclusiveMonitor * monitor, ISystemModules & modules, ICpuInfo & cpuInfo, uint32_t coreIndex);

    IArm64Reg & Reg(void) { return m_reg; }

    //IArm64Executor
    HaltReason Execute(void);
    void InvalidateCacheRange(uint64_t addr, uint64_t size);
    void HaltExecution(HaltReason hr);

private:
    ArmDynarmic64() = delete;
    ArmDynarmic64(const ArmDynarmic64 &) = delete;
    ArmDynarmic64 & operator=(const ArmDynarmic64 &) = delete;

    std::unique_ptr<Dynarmic::A64::Jit> MakeJit(Dynarmic::ExclusiveMonitor * monitor);

    //Dynarmic::A64::UserCallbacks
    std::uint8_t MemoryRead8(std::uint64_t vaddr);
    std::uint16_t MemoryRead16(std::uint64_t vaddr);
    std::uint32_t MemoryRead32(std::uint64_t vaddr);
    std::uint64_t MemoryRead64(std::uint64_t vaddr);
    Dynarmic::A64::Vector MemoryRead128(std::uint64_t vaddr);
    void MemoryWrite8(std::uint64_t vaddr, std::uint8_t value);
    void MemoryWrite16(std::uint64_t vaddr, std::uint16_t value);
    void MemoryWrite32(std::uint64_t vaddr, std::uint32_t value);
    void MemoryWrite64(std::uint64_t vaddr, std::uint64_t value);
    void MemoryWrite128(std::uint64_t vaddr, Dynarmic::A64::Vector value);
    bool MemoryWriteExclusive8(std::uint64_t /*vaddr*/, std::uint8_t /*value*/, std::uint8_t /*expected*/);
    bool MemoryWriteExclusive16(std::uint64_t /*vaddr*/, std::uint16_t /*value*/, std::uint16_t /*expected*/);
    bool MemoryWriteExclusive32(std::uint64_t /*vaddr*/, std::uint32_t /*value*/, std::uint32_t /*expected*/);
    bool MemoryWriteExclusive64(std::uint64_t /*vaddr*/, std::uint64_t /*value*/, std::uint64_t /*expected*/);
    bool MemoryWriteExclusive128(std::uint64_t /*vaddr*/, Dynarmic::A64::Vector /*value*/, Dynarmic::A64::Vector /*expected*/);
    bool IsReadOnlyMemory(std::uint64_t /*vaddr*/);
    void InterpreterFallback(std::uint64_t pc, size_t num_instructions);
    void CallSVC(std::uint32_t swi);
    void ExceptionRaised(std::uint64_t pc, Dynarmic::A64::Exception exception);
    void DataCacheOperationRaised(Dynarmic::A64::DataCacheOperation /*op*/, std::uint64_t /*value*/);
    void InstructionCacheOperationRaised(Dynarmic::A64::InstructionCacheOperation /*op*/, std::uint64_t /*value*/);
    void InstructionSynchronizationBarrierRaised();
    void AddTicks(std::uint64_t ticks);
    std::uint64_t GetTicksRemaining();
    std::uint64_t GetCNTPCT();

    std::unique_ptr<Dynarmic::A64::Jit> m_jit{};
    ISystemModules & m_modules;
    ICpuInfo & m_CpuInfo;
    IMemory & m_memory;
    IOperatingSystem & m_OperatingSystem;
    Dynarmic::ExclusiveMonitor * m_monitor;
    A64Registers m_reg;
    uint32_t m_coreIndex;
};
