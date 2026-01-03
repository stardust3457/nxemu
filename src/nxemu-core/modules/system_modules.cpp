#include "system_modules.h"
#include "cpu_module.h"
#include "loader_module.h"
#include "module_notification.h"
#include "module_settings.h"
#include "operating_system_module.h"
#include "video_module.h"
#include "notification.h"
#include "settings/core_settings.h"
#include <vector>

namespace
{
typedef std::vector<ModuleBase *> BaseModules;

template <typename plugin_type>
void LoadModule(const std::string & fileName, std::unique_ptr<plugin_type> & plugin, ModuleNotification * moduleNotification, ModuleSettings * moduleSettings)
{
    Path fullPath((const char *)coreSettings.moduleDir, fileName.c_str());
    plugin = std::make_unique<plugin_type>();
    if (plugin.get() == nullptr || !fullPath.FileExists() || !plugin->Load(fullPath, moduleNotification, moduleSettings))
    {
        plugin = nullptr;
    }
}
} // namespace

struct SystemModules::Impl :
    public ISystemModules
{
    explicit Impl(IRenderWindow & window_) :
        window(window_),
        systemLoader(nullptr),
        video(nullptr),
        cpu(nullptr),
        operatingsystem(nullptr),
        valid(false)
    {
    }

    void StartEmulation()
    {
        for (BaseModules::iterator itr = baseModules.begin(); itr != baseModules.end(); itr++)
        {
            (*itr)->EmulationStarting();
        }
    }

    ISystemloader & Systemloader()
    {
        return *systemLoader;
    }

    IOperatingSystem & OperatingSystem()
    {
        return *operatingsystem;
    }

    IVideo & Video()
    {
        return *video;
    }

    ICpu & Cpu()
    {
        return *cpu;
    }

    IRenderWindow & window;
    BaseModules baseModules;
    ModuleNotification moduleNotification;
    ModuleSettings moduleSettings;
    std::string loaderFile;
    std::string cpuFile;
    std::string videoFile;
    std::string operatingsystemFile;
    std::unique_ptr<LoaderModule> loaderModule;
    std::unique_ptr<CpuModule> cpuModule;
    std::unique_ptr<VideoModule> videoModule;
    std::unique_ptr<OperatingSystemModule> operatingsystemModule;
    ISystemloader * systemLoader;
    IVideo * video;
    ICpu * cpu;
    IOperatingSystem * operatingsystem;
    bool valid;
};

SystemModules::SystemModules()
{
}

SystemModules::~SystemModules()
{
    ShutDown();
}

void SystemModules::Setup(IRenderWindow & window)
{
    ShutDown();
    impl = std::make_unique<Impl>(window);
    impl->valid = false;

    impl->loaderFile = coreSettings.moduleLoader;
    impl->cpuFile = coreSettings.moduleCpu;
    impl->videoFile = coreSettings.moduleVideo;
    impl->operatingsystemFile = coreSettings.moduleOs;

    LoadModule(impl->loaderFile, impl->loaderModule, &impl->moduleNotification, &impl->moduleSettings);
    LoadModule(impl->cpuFile, impl->cpuModule, &impl->moduleNotification, &impl->moduleSettings);
    LoadModule(impl->videoFile, impl->videoModule, &impl->moduleNotification, &impl->moduleSettings);
    LoadModule(impl->operatingsystemFile, impl->operatingsystemModule, &impl->moduleNotification, &impl->moduleSettings);

    if (impl->loaderModule.get() == nullptr || 
        impl->cpuModule.get() == nullptr || 
        impl->videoModule.get() == nullptr || 
        impl->operatingsystemModule.get() == nullptr)
    {
        return;
    }
    impl->systemLoader = impl->loaderModule->CreateSystemLoader(*impl);
    if (impl->systemLoader == nullptr)
    {
        return;
    }
    impl->cpu = impl->cpuModule->CreateCpu(*impl);
    if (impl->cpu == nullptr)
    {
        return;
    }
    impl->operatingsystem = impl->operatingsystemModule->CreateOS(*impl);
    if (impl->operatingsystem == nullptr)
    {
        return;
    }
    impl->video = impl->videoModule->CreateVideo(impl->window, *impl);
    if (impl->video == nullptr)
    {
        return;
    }

    if (!impl->systemLoader->Initialize())
    {
        return;
    }
    if (!impl->cpu->Initialize())
    {
        return;
    }
    if (!impl->video->Initialize())
    {
        return;
    }
    if (!impl->operatingsystem->Initialize())
    {
        return;
    }

    impl->baseModules.push_back(impl->cpuModule.get());
    impl->baseModules.push_back(impl->videoModule.get());
    impl->baseModules.push_back(impl->operatingsystemModule.get());
    impl->baseModules.push_back(impl->loaderModule.get());
    impl->valid = true;
}

void SystemModules::ShutDown()
{
    if (impl == nullptr)
    {
        return;
    }
    for (BaseModules::iterator itr = impl->baseModules.begin(); itr != impl->baseModules.end(); itr++)
    {
        (*itr)->ModuleCleanup();
    }
    impl->baseModules.clear();
    if (impl->cpu != nullptr && impl->cpuModule.get() != nullptr)
    {
        impl->cpuModule->DestroyCpu(impl->cpu);
        impl->cpu = nullptr;
    }
    if (impl->video != nullptr && impl->videoModule.get() != nullptr)
    {
        impl->videoModule->DestroyVideo(impl->video);
        impl->video = nullptr;
    }
    if (impl->operatingsystem != nullptr && impl->operatingsystemModule.get() != nullptr)
    {
        impl->operatingsystemModule->DestroyOS(impl->operatingsystem);
        impl->operatingsystem = nullptr;
    }
    impl = nullptr;
}

void SystemModules::FlushSettings(void)
{
    for (BaseModules::iterator itr = impl->baseModules.begin(); itr != impl->baseModules.end(); itr++)
    {
        (*itr)->FlushSettings();
    }
}

bool SystemModules::IsValid() const
{
    return impl.get() != nullptr ? impl->valid : false;
}

ISystemModules & SystemModules::Modules()
{
    return *(impl.get());
}
