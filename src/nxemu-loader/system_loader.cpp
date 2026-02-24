#include "system_loader.h"
#include <fmt/core.h>
#include <common/path.h>
#include <nxemu-core/settings/identifiers.h>
#include "core/core.h"
#include "core/file_sys/card_image.h"
#include "core/file_sys/content_archive.h"
#include "core/file_sys/patch_manager.h"
#include "core/file_sys/registered_cache.h"
#include "core/file_sys/filesystem.h"
#include "core/file_sys/romfs_factory.h"
#include "core/file_sys/submission_package.h"
#include "core/file_sys/vfs/vfs_real.h"
#include "core/file_sys/vfs/vfs_types.h"
#include "core/file_sys/system_archive/system_archive.h"
#include "core/loader/loader.h"
#include "rom_info.h"

extern IModuleSettings * g_settings;
extern IModuleNotification * g_notify;

namespace
{

StorageId GetStorageIdForFrontendSlot(std::optional<FileSys::ContentProviderUnionSlot> slot)
{
    if (!slot.has_value())
    {
        return StorageId::None;
    }

    switch (*slot)
    {
    case FileSys::ContentProviderUnionSlot::UserNAND: return StorageId::NandUser;
    case FileSys::ContentProviderUnionSlot::SysNAND: return StorageId::NandSystem;
    case FileSys::ContentProviderUnionSlot::SDMC: return StorageId::SdCard;
    case FileSys::ContentProviderUnionSlot::FrontendManual:return StorageId::Host;
    }
    return StorageId::None;
}

    bool HasSupportedFileExtension(const char * fileName)
    {
        static const char * supported_extensions[] = {
            "nro",
            "dxci", 
            "dnsp"
        };

        std::string ext = Path(fileName).GetExtension();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        for (const char* supported : supported_extensions) 
        {
            if (ext == supported) 
            {
                return true;
            }
        }
        return false;
    }
} // Anonymous namespace

struct Systemloader::Impl {
    explicit Impl(Systemloader & loader, ISystemModules & modules) :
        m_loader(loader),
        m_modules(modules),
        m_fsController(loader),
        m_manualContentProvider(std::make_unique<ManualContentProviderImpl>()),
        m_processID(0),
        m_titleID(0)
    {
    }
    ~Impl()
    {
        m_appLoader.reset();
    }

    FileSys::VirtualFile m_file;
    std::shared_ptr<Loader::AppLoader> m_appLoader;
    Systemloader & m_loader;
    ISystemModules & m_modules;
    /// RealVfsFilesystem instance
    FileSys::VirtualFilesystem m_virtualFilesystem;
    std::unique_ptr<FileSys::ContentProviderUnion> m_contentProvider;
    ::FileSystemController m_fsController;
    std::unique_ptr<ManualContentProviderImpl> m_manualContentProvider;
    uint64_t m_processID;
    uint64_t m_titleID;
};

Systemloader::Systemloader(ISystemModules & modules) :
    impl(std::make_unique<Impl>(*this, modules))
{
}

Systemloader::~Systemloader()
{
}

bool Systemloader::Initialize()
{
    if (impl->m_virtualFilesystem == nullptr)
    {
        impl->m_virtualFilesystem = std::make_shared<FileSys::RealVfsFilesystem>();
    }
    if (impl->m_contentProvider == nullptr)
    {
        impl->m_contentProvider = std::make_unique<FileSys::ContentProviderUnion>();
    }
    impl->m_contentProvider->SetSlot(FileSys::ContentProviderUnionSlot::FrontendManual, &impl->m_manualContentProvider->Provider());
    impl->m_fsController.CreateFactories(*impl->m_virtualFilesystem, false);
    return true;
}

bool Systemloader::SelectAndLoad(void * parentWindow)
{
    Path fileToOpen;
    std::string fileName;
    const char * filter = "Switch Files (*.dxci, *.dnsp, *.nro)\0*.dxci;*.dnsp;*.nro;\0All files (*.*)\0*.*\0";
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

bool Systemloader::LoadRom(const char * fileName)
{
    g_settings->SetInt(NXCoreSetting::EmulationState, (int32_t)EmulationState::LoadingRom);
    impl->m_file = Core::GetGameFileFromPath(impl->m_virtualFilesystem, fileName);
    impl->m_appLoader = Loader::GetLoader(*this, impl->m_file, 0, 0);
    if (!impl->m_appLoader)
    {
        g_notify->DisplayError("The file format is not supported.", "Error loading file!");
        g_settings->SetBool(NXCoreSetting::EmulationState, (int32_t)EmulationState::Stopped);
        return false;
    }

    Loader::AppLoader * app_loader = impl->m_appLoader.get();
    const LoaderFileType file_type = impl->m_appLoader->GetFileType();
    if (file_type == LoaderFileType::Unknown || file_type == LoaderFileType::Error) 
    {
        g_notify->DisplayError("The file format is not supported.", "Error loading file!");
        g_settings->SetBool(NXCoreSetting::EmulationState, (int32_t)EmulationState::Stopped);
        return false;
    }
    uint64_t program_id = 0;
    const LoaderResultStatus res = app_loader->ReadProgramId(program_id);
    if (res == LoaderResultStatus::Success && file_type == LoaderFileType::NCA) 
    {
        UNIMPLEMENTED();
    }
    else if (res == LoaderResultStatus::Success && (file_type == LoaderFileType::XCI || file_type == LoaderFileType::NSP))
    {
        const auto nsp = file_type == LoaderFileType::NSP ? std::make_shared<FileSys::NSP>(impl->m_file) : FileSys::XCI{impl->m_file}.GetSecurePartitionNSP();
        for (const auto & title : nsp->GetNCAs())
        {
            for (const auto & entry : title.second)
            {
                FileSys::VirtualFile file = entry.second->GetBaseFile();
                impl->m_manualContentProvider->AddEntry(entry.first.first, entry.first.second, title.first, std::make_unique<VirtualFileImpl>(file).release());
            }
        }
    }
    else if (res != LoaderResultStatus::Success)
    {
        LOG_ERROR(Core, "Failed to find title id for ROM!");
    }

    uint32_t title_size = 0;
    std::string title = "Unknown program";
    LoaderResultStatus result = app_loader->ReadTitle(nullptr, &title_size);
    if (result == LoaderResultStatus::Success)
    {
        title.resize(title_size);
        result = app_loader->ReadTitle(title.data(), &title_size);
    }
    if (result != LoaderResultStatus::Success)
    {
        LOG_ERROR(Core, "Failed to read title for ROM!");
    }
    
    const auto [load_result, load_parameters] = app_loader->Load(*this, impl->m_modules);
    if (load_result != LoaderResultStatus::Success)
    {
        LOG_CRITICAL(Core, "Failed to load ROM (Error {})!", load_result);
        g_settings->SetBool(NXCoreSetting::EmulationState, (int32_t)EmulationState::Stopped);
        return false;
    }
    std::vector<u8> nacp_data;
    FileSys::NACP nacp;
    if (app_loader->ReadControlData(nacp) == LoaderResultStatus::Success)
    {
        nacp_data = nacp.GetRawBytes();
    }
    else
    {
        nacp_data.resize(sizeof(FileSys::RawNACP));
    }

    FileSys::PatchManager pm(impl->m_titleID, impl->m_fsController, *impl->m_contentProvider);
    uint32_t version = pm.GetGameVersion().value_or(0);

    g_settings->SetString(NXCoreSetting::GameName, title.c_str());
    g_settings->SetString(NXCoreSetting::GameFile, fileName);
    g_settings->SetBool(NXCoreSetting::EmulationRunning, true);

    // TODO(DarkLordZach): When FSController/Game Card Support is added, if
    // current_process_game_card use correct StorageId
    StorageId baseGameStorageId = GetStorageIdForFrontendSlot(impl->m_contentProvider->GetSlotForEntry(impl->m_titleID, LoaderContentRecordType::Program));
    StorageId updateStorageId = GetStorageIdForFrontendSlot(impl->m_contentProvider->GetSlotForEntry(FileSys::GetUpdateTitleID(impl->m_titleID), LoaderContentRecordType::Program));

    IOperatingSystem & operatingSystem = impl->m_modules.OperatingSystem();
    operatingSystem.StartApplicationProcess(load_parameters->main_thread_priority, load_parameters->main_thread_stack_size, version, baseGameStorageId, updateStorageId, nacp_data.data(), (uint32_t)nacp_data.size());
    return true;
}

IRomInfo * Systemloader::RomInfo(const char * fileName, uint64_t programId, uint64_t programIndex)
{
    if (!HasSupportedFileExtension(fileName))
    {
        return nullptr;
    }
    const FileSys::VirtualFile file = impl->m_virtualFilesystem->OpenFile(fileName, VirtualFileOpenMode::Read);
    if (!file)
    {
        return nullptr;
    }

    std::shared_ptr<Loader::AppLoader> loader = Loader::GetLoader(*this, file, programId, programIndex);
    if (!loader)
    {
        return nullptr;
    }
    std::unique_ptr<::RomInfo> info = std::make_unique<::RomInfo>(file, std::move(loader));
    return info.release();
}

IRomInfo * Systemloader::LoadedRomInfo()
{
    std::unique_ptr<::RomInfo> info = std::make_unique<::RomInfo>(impl->m_file, impl->m_appLoader);
    return info.release();
}

FileSys::VirtualFilesystem Systemloader::GetFilesystem()
{
    return impl->m_virtualFilesystem;
}

FileSystemController & Systemloader::GetFileSystemController()
{
    return impl->m_fsController;
}

FileSys::ContentProvider & Systemloader::GetContentProvider()
{
    return *impl->m_contentProvider;
}

void Systemloader::RegisterContentProvider(FileSys::ContentProviderUnionSlot slot, FileSys::ContentProvider* provider) 
{
    impl->m_contentProvider->SetSlot(slot, provider);
}

void Systemloader::SetProcessID(uint64_t processID)
{
    impl->m_processID = processID;
}

void Systemloader::SetTitleID(uint64_t titleID)
{
    impl->m_titleID = titleID;
}

IFileSystemController & Systemloader::FileSystemController()
{
    return impl->m_fsController;
}

IVirtualFile * Systemloader::SynthesizeSystemArchive(const uint64_t title_id)
{
    FileSys::VirtualFile file = FileSys::SystemArchive::SynthesizeSystemArchive(title_id);
    return std::make_unique<VirtualFileImpl>(file).release();
}

uint32_t Systemloader::GetContentProviderEntriesCount(bool useTitleType, LoaderTitleType titleType, bool useContentRecordType, LoaderContentRecordType contentRecordType, bool useTitleId, unsigned long long titleId)
{
    const FileSys::ContentProviderUnion & rcu = *impl->m_contentProvider;
    std::optional<LoaderTitleType> title = useTitleType ? std::optional<LoaderTitleType>((LoaderTitleType)titleType) : std::nullopt;
    std::optional<LoaderContentRecordType> record = useContentRecordType ? std::optional<LoaderContentRecordType>((LoaderContentRecordType)contentRecordType) : std::nullopt;
    std::optional<uint64_t> id = useTitleId ? std::optional<uint64_t>(titleId) : std::nullopt;
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
    std::optional<uint64_t> id = useTitleId ? std::optional<uint64_t>(titleId) : std::nullopt;
    const std::vector<ContentProviderEntry> list = rcu.ListEntriesFilter(title, record, id);
    entryCount = std::min((uint32_t)list.size(), entryCount);
    memcpy(entries, list.data(), entryCount * sizeof(ContentProviderEntry));
    return entryCount;
}

IFileSysNCA * Systemloader::GetContentProviderEntry(uint64_t title_id, LoaderContentRecordType type)
{
    return GetContentProvider().GetEntryNCA(title_id, type).release();
}

IFileSysNACP * Systemloader::GetPMControlMetadata(uint64_t programID)
{
    const FileSys::PatchManager pm(programID, GetFileSystemController(), GetContentProvider());
    FileSys::PatchManager::Metadata metadata = pm.GetControlMetadata();
    if (metadata.first == nullptr)
    {
        return nullptr;
    }
    return metadata.first.release();
}

IManualContentProvider & Systemloader::ManualContentProvider()
{
    return *(impl->m_manualContentProvider.get());
}
