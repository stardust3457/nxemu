#pragma once
#include <nxemu-module-spec/system_loader.h>
#include <memory>
#include "core/file_sys/vfs/vfs_types.h"
#include "core/file_sys/registered_cache.h"

namespace FileSys
{
class FileSystemController;
class ContentProvider;
}

class Systemloader :
    public ISystemloader
{
public:
    Systemloader(ISystemModules & modules);
    ~Systemloader();

#ifdef tofix
    ISystemModules & GetSystemModules();
#endif
    FileSys::ContentProvider & GetContentProvider();
    FileSys::VirtualFilesystem GetFilesystem();
    FileSys::FileSystemController & GetFileSystemController();
    void RegisterContentProvider(FileSys::ContentProviderUnionSlot slot, FileSys::ContentProvider* provider);

    void SetProcessID(uint64_t processID);
    void SetTitleID(uint64_t titleID);
    
    //ISystemloader
    bool Initialize() override;
    bool SelectAndLoad(void * parentWindow) override;
    bool LoadRom(const char * fileName) override;
    IRomInfo * RomInfo(const char * fileName) override;

    IFileSystemController & FileSystemController() override;
    IVirtualFile * SynthesizeSystemArchive(const uint64_t title_id) override;
    uint32_t GetContentProviderEntriesCount(bool useTitleType, LoaderTitleType titleType, bool useContentRecordType, LoaderContentRecordType contentRecordType, bool useTitleId, unsigned long long titleId) override;
    uint32_t GetContentProviderEntries(bool useTitleType, LoaderTitleType titleType, bool useContentRecordType, LoaderContentRecordType contentRecordType, bool useTitleId, unsigned long long titleId, ContentProviderEntry * entries, uint32_t entryCount) override;
    IFileSysNCA * GetContentProviderEntry(uint64_t title_id, LoaderContentRecordType type) override;
    IFileSysNACP * GetPMControlMetadata(uint64_t programID) override;

private:
    Systemloader() = delete;
    Systemloader(const Systemloader &) = delete;
    Systemloader & operator=(const Systemloader &) = delete;

    struct Impl;
    std::unique_ptr<Impl> impl;
};
