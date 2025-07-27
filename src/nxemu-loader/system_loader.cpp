#include "system_loader.h"
#include "file_format/nro.h"
#include "file_format/nacp.h"
#include <fmt/core.h>
#include <common/path.h>
#include <nxemu-core/settings/identifiers.h>
#include "core/core.h"
#include "core/file_sys/patch_manager.h"
#include "core/file_sys/registered_cache.h"
#include "core/file_sys/filesystem.h"
#include "core/file_sys/romfs_factory.h"
#include "core/file_sys/vfs/vfs_real.h"
#include "core/file_sys/vfs/vfs_types.h"
#include "core/file_sys/system_archive/system_archive.h"
#include "core/loader/loader.h"

extern IModuleSettings * g_settings;

namespace {

    StorageId GetStorageIdForFrontendSlot(
        std::optional<FileSys::ContentProviderUnionSlot> slot) {
        if (!slot.has_value()) {
            return StorageId::None;
        }

        switch (*slot) {
        case FileSys::ContentProviderUnionSlot::UserNAND:
            return StorageId::NandUser;
        case FileSys::ContentProviderUnionSlot::SysNAND:
            return StorageId::NandSystem;
        case FileSys::ContentProviderUnionSlot::SDMC:
            return StorageId::SdCard;
        case FileSys::ContentProviderUnionSlot::FrontendManual:
            return StorageId::Host;
        default:
            return StorageId::None;
        }
    }

} // Anonymous namespace

struct Systemloader::Impl {
    explicit Impl(Systemloader & loader, ISwitchSystem& system) :
        m_loader(loader),
        m_system(system),
        m_fsController(loader),
        m_provider(std::make_unique<FileSys::ManualContentProvider>()),
        m_titleID(0)
    {
    }

    std::unique_ptr<Loader::AppLoader> app_loader;
    Systemloader & m_loader;
    ISwitchSystem & m_system;
    /// RealVfsFilesystem instance
    FileSys::VirtualFilesystem m_virtualFilesystem;
    /// ContentProviderUnion instance
    std::unique_ptr<FileSys::ContentProviderUnion> m_contentProvider;
    FileSys::FileSystemController m_fsController;
    std::unique_ptr<FileSys::ManualContentProvider> m_provider;
    std::unique_ptr<Nro> m_nro;
    uint64_t m_titleID;
};

Systemloader::Systemloader(ISwitchSystem & system) :
    impl(std::make_unique<Impl>(*this, system))
{
}

Systemloader::~Systemloader()
{
}

bool Systemloader::Initialize(void)
{
    if (impl->m_virtualFilesystem == nullptr) {
        impl->m_virtualFilesystem = std::make_shared<FileSys::RealVfsFilesystem>();
    }
    if (impl->m_contentProvider == nullptr) {
        impl->m_contentProvider = std::make_unique<FileSys::ContentProviderUnion>();
    }
    impl->m_contentProvider->SetSlot(FileSys::ContentProviderUnionSlot::FrontendManual, impl->m_provider.get());
    GetFileSystemController().CreateFactories(*GetFilesystem(), false);
    return true;
}

bool Systemloader::SelectAndLoad(void * parentWindow)
{
    Path fileToOpen;
    std::string fileName;
    const char * filter = "Switch Files (*.nro)\0*.nro\0All files (*.*)\0*.*\0";
    if (fileToOpen.FileSelect(parentWindow, Path(Path::MODULE_DIRECTORY), filter, true))
    {
        fileName = (const std::string &)fileToOpen;
    }
    if (fileName.length() == 0)
    {
        return false;
    }
    return LoadRom(fileName.c_str());
}

bool Systemloader::LoadRom(const char * romFile)
{
    g_settings->SetBool(NXCoreSetting::RomLoading, true);
    const FileSys::VirtualFile file = Core::GetGameFileFromPath(impl->m_virtualFilesystem, romFile);
    impl->app_loader = Loader::GetLoader(*this, file, 0, 0);
    if (!impl->app_loader)
    {
        g_settings->SetBool(NXCoreSetting::RomLoading, false);
        return false;
    }

    const Loader::FileType file_type = impl->app_loader->GetFileType();
    if (file_type == Loader::FileType::Unknown || file_type == Loader::FileType::Error) 
    {
        g_settings->SetBool(NXCoreSetting::RomLoading, false);
        return false;
    }
    u64 program_id = 0;
    const LoaderResultStatus res = impl->app_loader->ReadProgramId(program_id);
    if (res != LoaderResultStatus::Success)
    {
        LOG_ERROR(Core, "Failed to find title id for ROM!");
    }

    std::string name = "Unknown program";
    if (impl->app_loader->ReadTitle(name) != LoaderResultStatus::Success)
    {
        LOG_ERROR(Core, "Failed to read title for ROM!");
    }

    const auto [load_result, load_parameters] = impl->app_loader->Load(*this);
    if (load_result != LoaderResultStatus::Success)
    {
        LOG_CRITICAL(Core, "Failed to load ROM (Error {})!", load_result);
        g_settings->SetBool(NXCoreSetting::RomLoading, false);
        return false;
    }
    {
        std::vector<u8> nacp_data;
        FileSys::NACP nacp;
        if (impl->app_loader->ReadControlData(nacp) == LoaderResultStatus::Success)
        {
            nacp_data = nacp.GetRawBytes();
        }
        else
        {
            nacp_data.resize(sizeof(FileSys::RawNACP));
        }

        //launch.title_id = process.GetProgramId();
        FileSys::PatchManager pm{ impl->m_titleID, impl->m_fsController, *impl->m_contentProvider };
        uint32_t version = pm.GetGameVersion().value_or(0);

        // TODO(DarkLordZach): When FSController/Game Card Support is added, if
        // current_process_game_card use correct StorageId
        StorageId baseGameStorageId = GetStorageIdForFrontendSlot(impl->m_contentProvider->GetSlotForEntry(impl->m_titleID, LoaderContentRecordType::Program));
        StorageId updateStorageId = GetStorageIdForFrontendSlot(impl->m_contentProvider->GetSlotForEntry(FileSys::GetUpdateTitleID(impl->m_titleID), LoaderContentRecordType::Program));

        IOperatingSystem& operatingSystem = impl->m_system.OperatingSystem();
        operatingSystem.StartApplicationProcess(load_parameters->main_thread_priority, load_parameters->main_thread_stack_size, version, baseGameStorageId, updateStorageId, nacp_data.data(), (uint32_t)nacp_data.size());
    }

    std::string title;
    impl->app_loader->ReadTitle(title);
    g_settings->SetString(NXCoreSetting::GameName, title.c_str());
    g_settings->SetBool(NXCoreSetting::RomLoading, false);
    g_settings->SetString(NXCoreSetting::GameFile, romFile);
    impl->m_system.StartEmulation();
    return true;
}

ISwitchSystem & Systemloader::GetSystem() {
    return impl->m_system;
}

FileSys::ContentProvider & Systemloader::GetContentProvider() {
    return *impl->m_contentProvider;
}

FileSys::VirtualFilesystem Systemloader::GetFilesystem() {
    return impl->m_virtualFilesystem;
}

FileSys::FileSystemController & Systemloader::GetFileSystemController() {
    return impl->m_fsController;
}

void Systemloader::RegisterContentProvider(FileSys::ContentProviderUnionSlot slot, FileSys::ContentProvider* provider) 
{
    impl->m_contentProvider->SetSlot(slot, provider);
}

IFileSystemController & Systemloader::FileSystemController()
{
    return impl->m_fsController;
}

IVirtualFile * Systemloader::SynthesizeSystemArchive(const uint64_t title_id)
{
    FileSys::VirtualFile file = FileSys::SystemArchive::SynthesizeSystemArchive(title_id);
    return std::make_unique<VirtualFilePtr>(file).release();
}

uint32_t Systemloader::GetContentProviderEntriesCount(bool useTitleType, LoaderTitleType titleType, bool useContentRecordType, LoaderContentRecordType contentRecordType, bool useTitleId, unsigned long long titleId)
{
    const FileSys::ContentProviderUnion & rcu = *impl->m_contentProvider;
    std::optional<LoaderTitleType> title = useTitleType ? std::optional<LoaderTitleType>((LoaderTitleType)titleType) : std::nullopt;
    std::optional<LoaderContentRecordType> record = useContentRecordType ? std::optional<LoaderContentRecordType>((LoaderContentRecordType)contentRecordType) : std::nullopt;
    std::optional<u64> id = useTitleId ? std::optional<u64>(titleId) : std::nullopt;
    const std::vector<ContentProviderEntry> list = rcu.ListEntriesFilter(title, record, id);
    return (uint32_t)list.size();
}

uint32_t Systemloader::GetContentProviderEntries(bool useTitleType, LoaderTitleType titleType, bool useContentRecordType, LoaderContentRecordType contentRecordType, bool useTitleId, unsigned long long titleId, ContentProviderEntry * entries, uint32_t entryCount)
{
    if (entries == nullptr || entryCount == 0)
    {
        return 0;
    }
    const FileSys::ContentProviderUnion& rcu = *impl->m_contentProvider;
    std::optional<LoaderTitleType> title = useTitleType ? std::optional<LoaderTitleType>((LoaderTitleType)titleType) : std::nullopt;
    std::optional<LoaderContentRecordType> record = useContentRecordType ? std::optional<LoaderContentRecordType>((LoaderContentRecordType)contentRecordType) : std::nullopt;
    std::optional<u64> id = useTitleId ? std::optional<u64>(titleId) : std::nullopt;
    const std::vector<ContentProviderEntry> list = rcu.ListEntriesFilter(title, record, id);
    entryCount = std::min((uint32_t)list.size(), entryCount);
    memcpy(entries, list.data(), entryCount * sizeof(ContentProviderEntry));
    return entryCount;
}

IFileSysNCA * Systemloader::GetContentProviderEntry(uint64_t title_id, LoaderContentRecordType type)
{
    UNIMPLEMENTED();
    return nullptr;
}

IFileSysNACP * Systemloader::GetPMControlMetadata(uint64_t programID)
{
    UNIMPLEMENTED();
    return nullptr;
}
