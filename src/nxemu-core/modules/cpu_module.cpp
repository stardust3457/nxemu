#include "cpu_module.h"
#include "notification.h"

CpuModule::CpuModule() :
    m_createCpu(dummyCreateCpu),
    m_destroyCpu(dummyDestroyCpu)
{
}

void CpuModule::UnloadModule(void)
{
    m_createCpu = dummyCreateCpu;
    m_destroyCpu = dummyDestroyCpu;
}

bool CpuModule::LoadFunctions(void)
{
    m_createCpu = (tyCreateCpu)DynamicLibraryGetProc(m_lib, "CreateCpu");
    m_destroyCpu = (tyDestroyCpu)DynamicLibraryGetProc(m_lib, "DestroyCpu");

    bool res = true;
    if (m_createCpu == nullptr)
    {
        m_createCpu = dummyCreateCpu;
        res = false;
    }
    if (m_destroyCpu == nullptr)
    {
        m_destroyCpu = dummyDestroyCpu;
        res = false;
    }
    return res;
}

MODULE_TYPE CpuModule::ModuleType() const
{
    return MODULE_TYPE_CPU;
}

ICpu * CpuModule::dummyCreateCpu(ISystemModules & /*system*/)
{
    return nullptr;
}

void CpuModule::dummyDestroyCpu(ICpu * /*cpu*/)
{
}
