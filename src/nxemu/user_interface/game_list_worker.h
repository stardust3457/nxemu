#pragma once
#include <nxemu-module-spec/base.h>
#include <string>
#include <thread>

class SystemModules;
nxinterface IManualContentProvider;

class GameListWorker
{
    enum class ScanTarget
    {
        FillManualContentProvider,
        PopulateGameList,
    };

public:
    GameListWorker(IManualContentProvider & provider_, SystemModules & modules);
    ~GameListWorker();

    void Start();
    void Stop();

private:
    void Run();
    void ScanFileSystem(ScanTarget target, const std::string & game_dir, bool deep_scan);

    IManualContentProvider & m_provider;
    SystemModules & m_modules;
    std::thread m_thread;
    bool m_stop;
};