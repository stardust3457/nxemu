#pragma once
#include <memory>
#include <nxemu-core/modules/modules.h>

class SwitchSystem :
    public ISystemModules
{
public:
    ~SwitchSystem();

    static bool Create(IRenderWindow & window);
    static void ShutDown();
    static SwitchSystem * GetInstance();

    void StartEmulation(void);
    void StopEmulation(void);
    void FlushSettings(void);

    //ISystemModules
    ISystemloader & Systemloader() override;
    IOperatingSystem & OperatingSystem() override;
    IVideo & Video(void) override;
    ICpu & Cpu(void) override;

private:
    SwitchSystem(const SwitchSystem &) = delete;
    SwitchSystem & operator=(const SwitchSystem &) = delete;

    SwitchSystem();

    bool Initialize(IRenderWindow & window);

    static std::unique_ptr<SwitchSystem> m_instance;
    bool m_emulationRunning;
    Modules m_modules;
};
