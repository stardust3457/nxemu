#pragma once
#include <memory>

namespace Core
{
class System;
}

class EmuThread
{
public:
    EmuThread(Core::System & system_, Kernel::KProcess * & process);
    ~EmuThread();

    void Start();
    void Stop();
    void SetRunning(bool should_run);
    bool IsRunning() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};