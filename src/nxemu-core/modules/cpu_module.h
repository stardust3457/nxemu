#pragma once
#include "module_base.h"
#include <nxemu-module-spec/cpu.h>

class CpuModule :
    public ModuleBase
{
public:
    typedef ICpu *(CALL * tyCreateCpu)(ISystemModules & system);
    typedef void(CALL * tyDestroyCpu)(ICpu * Cpu);

    CpuModule();
    ~CpuModule() = default;

    ICpu * CreateCpu(ISystemModules & modules) const
    {
        return m_createCpu(modules);
    }

    void DestroyCpu(ICpu * Cpu) const
    {
        m_destroyCpu(Cpu);
    }

protected:
    void UnloadModule(void);
    bool LoadFunctions(void);

    MODULE_TYPE ModuleType() const;

private:
    CpuModule(const CpuModule &) = delete;
    CpuModule & operator=(const CpuModule &) = delete;

    static ICpu * CALL dummyCreateCpu(ISystemModules & modules);
    static void CALL dummyDestroyCpu(ICpu * cpu);

    tyCreateCpu m_createCpu;
    tyDestroyCpu m_destroyCpu;
};