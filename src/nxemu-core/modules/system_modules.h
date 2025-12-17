#pragma once
#include <memory>

__interface ISystemModules;
__interface IRenderWindow;

class SystemModules
{
public:
    SystemModules();
    ~SystemModules();

    void Setup(IRenderWindow & window);
    void ShutDown();
    void FlushSettings();
    bool IsValid() const;
    ISystemModules & Modules();

private:
    SystemModules(const SystemModules &) = delete;
    SystemModules & operator=(const SystemModules &) = delete;

    struct Impl;
    std::unique_ptr<Impl> impl;
};
