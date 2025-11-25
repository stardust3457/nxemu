#pragma once
#include "base.h"
#include "operating_system.h"

enum class LoaderTitleType : uint8_t {
    SystemProgram = 0x01,
    SystemDataArchive = 0x02,
    SystemUpdate = 0x03,
    FirmwarePackageA = 0x04,
    FirmwarePackageB = 0x05,
    Application = 0x80,
    Update = 0x81,
    AOC = 0x82,
    DeltaTitle = 0x83,
};

enum class LoaderContentRecordType : uint8_t {
    Meta = 0,
    Program = 1,
    Data = 2,
    Control = 3,
    HtmlDocument = 4,
    LegalInformation = 5,
    DeltaFragment = 6,
};

enum class LoaderResultStatus : uint16_t {
    Success,
    ErrorAlreadyLoaded,
    ErrorNotImplemented,
    ErrorNotInitialized,
    ErrorBadNPDMHeader,
    ErrorBadACIDHeader,
    ErrorBadACIHeader,
    ErrorBadFileAccessControl,
    ErrorBadFileAccessHeader,
    ErrorBadKernelCapabilityDescriptors,
    ErrorBadPFSHeader,
    ErrorIncorrectPFSFileSize,
    ErrorBadNCAHeader,
    ErrorMissingProductionKeyFile,
    ErrorMissingHeaderKey,
    ErrorIncorrectHeaderKey,
    ErrorNCA2,
    ErrorNCA0,
    ErrorMissingTitlekey,
    ErrorMissingTitlekek,
    ErrorInvalidRightsID,
    ErrorMissingKeyAreaKey,
    ErrorIncorrectKeyAreaKey,
    ErrorIncorrectTitlekeyOrTitlekek,
    ErrorXCIMissingProgramNCA,
    ErrorNCANotProgram,
    ErrorNoExeFS,
    ErrorBadXCIHeader,
    ErrorXCIMissingPartition,
    ErrorNullFile,
    ErrorMissingNPDM,
    Error32BitISA,
    ErrorUnableToParseKernelMetadata,
    ErrorNoRomFS,
    ErrorIncorrectELFFileSize,
    ErrorLoadingNRO,
    ErrorLoadingNSO,
    ErrorNoIcon,
    ErrorNoControl,
    ErrorBadNAXHeader,
    ErrorIncorrectNAXFileSize,
    ErrorNAXKeyHMACFailed,
    ErrorNAXValidationHMACFailed,
    ErrorNAXKeyDerivationFailed,
    ErrorNAXInconvertibleToNCA,
    ErrorBadNAXFilePath,
    ErrorMissingSDSeed,
    ErrorMissingSDKEKSource,
    ErrorMissingAESKEKGenerationSource,
    ErrorMissingAESKeyGenerationSource,
    ErrorMissingSDSaveKeySource,
    ErrorMissingSDNCAKeySource,
    ErrorNSPMissingProgramNCA,
    ErrorBadBKTRHeader,
    ErrorBKTRSubsectionNotAfterRelocation,
    ErrorBKTRSubsectionNotAtEnd,
    ErrorBadRelocationBlock,
    ErrorBadSubsectionBlock,
    ErrorBadRelocationBuckets,
    ErrorBadSubsectionBuckets,
    ErrorMissingBKTRBaseRomFS,
    ErrorNoPackedUpdate,
    ErrorBadKIPHeader,
    ErrorBLZDecompressionFailed,
    ErrorBadINIHeader,
    ErrorINITooManyKIPs,
    ErrorIntegrityVerificationNotImplemented,
    ErrorIntegrityVerificationFailed,
};

enum class VirtualFileOpenMode : uint32_t {
    Read = (1 << 0),
    Write = (1 << 1),
    AllowAppend = (1 << 2),

    ReadWrite = (Read | Write),
    All = (ReadWrite | AllowAppend),
};

enum class SaveDataSpaceId : uint8_t
{
    System = 0,
    User = 1,
    SdSystem = 2,
    Temporary = 3,
    SdUser = 4,

    ProperSystem = 100,
    SafeMode = 101,
};

enum class SaveDataType : uint8_t
{
    System = 0,
    Account = 1,
    Bcat = 2,
    Device = 3,
    Temporary = 4,
    Cache = 5,
    SystemBcat = 6,
};

enum class SaveDataRank : uint8_t
{
    Primary = 0,
    Secondary = 1,
};

struct ContentProviderEntry
{
    uint64_t titleID;
    LoaderContentRecordType type;
};

struct SaveDataAttribute 
{
    uint64_t program_id;
    uint64_t user_id[2];
    uint64_t system_save_data_id;
    SaveDataType type;
    SaveDataRank rank;
    uint16_t index;
    uint8_t padding[0x1C];
};
static_assert(sizeof(SaveDataAttribute) == 0x40);

struct SaveDataSize {
    uint64_t normal;
    uint64_t journal;
};
static_assert(sizeof(SaveDataSize) == 0x10, "SaveDataSize has invalid size.");

__interface IVirtualFile;

__interface IVirtualDirectory
{
    IVirtualDirectory * CreateSubdirectory(const char * path) const = 0;
    IVirtualDirectory * GetDirectoryRelative(const char * path) const = 0;
    IVirtualDirectory * GetSubdirectory(const char * name) const = 0;
    IVirtualDirectory * Duplicate() = 0;
    IVirtualFile * CreateFile(const char * name) const = 0;
    IVirtualFile * GetFile(const char * name) const = 0;
    IVirtualFile * GetFileRelative(const char * relative_path) const = 0;
    IVirtualFile * OpenFile(const char * path, VirtualFileOpenMode perms) = 0;
    void Release() = 0;
};

__interface IVirtualFile
{
    uint64_t GetSize() const = 0;
    bool Resize(uint64_t size) = 0;
    uint64_t ReadBytes(uint8_t * data, uint64_t datalen, uint64_t offset) = 0;
    uint64_t WriteBytes(const uint8_t * data, uint64_t datalen, uint64_t offset) = 0;
    IVirtualDirectory * ExtractRomFS() = 0;
    IVirtualFile * Duplicate() = 0;
    void Release() = 0;
};

__interface ISaveDataFactory
{
    bool OpenSaveData(IVirtualDirectory ** out_save_data, SaveDataSpaceId space, const SaveDataAttribute & attribute) = 0;
    void Release() = 0;
};

__interface IRomFsController
{
    IVirtualFile * OpenCurrentProcess(uint64_t currentProcessTitleId) const = 0;
    void Release() = 0;
};

__interface IFileSysNACP
{
    uint32_t GetSupportedLanguages() const = 0;
    uint32_t GetParentalControlFlag() const = 0;
    bool GetRatingAge(uint8_t * buffer, uint32_t bufferSize) const = 0;
    const char * GetVersionString() const = 0;
    void Release() = 0;
};

__interface IFileSysNCA
{
    LoaderResultStatus GetStatus() const = 0;
    IVirtualFile * GetRomFS() = 0;
    void Release() = 0;
};

__interface IFileSysRegisteredCache
{
    IFileSysNCA * GetEntry(uint64_t title_id, LoaderContentRecordType type) const = 0;
};

__interface ISaveDataController
{
    uint32_t CreateSaveData(IVirtualDirectory** out_save_data, SaveDataSpaceId space, const SaveDataAttribute & attribute) = 0;
    void Release() = 0;
};

__interface IFileSystemController
{
    IFileSysRegisteredCache * GetSystemNANDContents() const = 0;
    ISaveDataController * OpenSaveDataController() const = 0;
    
    uint64_t GetFreeSpaceSize(StorageId id) const = 0;
    uint64_t GetTotalSpaceSize(StorageId id) const = 0;

    bool OpenProcess(uint64_t * programId, ISaveDataFactory ** saveDataFactory, IRomFsController ** romFsController, uint64_t processId) = 0;
    bool OpenSDMC(IVirtualDirectory** out_sdmc) const = 0;
};

__interface IRomInfo
{
    void Release() = 0;
};

__interface ISystemloader
{
    bool Initialize() = 0;
    bool SelectAndLoad(void * parentWindow) = 0;
    bool LoadRom(const char * fileName) = 0;
    IRomInfo * RomInfo(const char * fileName) = 0;

    IFileSystemController & FileSystemController() = 0;
    IVirtualFile * SynthesizeSystemArchive(const uint64_t title_id) = 0;
    uint32_t GetContentProviderEntriesCount(bool useTitleType, LoaderTitleType titleType, bool useContentRecordType, LoaderContentRecordType contentRecordType, bool useTitleId, unsigned long long titleId) = 0;
    uint32_t GetContentProviderEntries(bool useTitleType, LoaderTitleType titleType, bool useContentRecordType, LoaderContentRecordType contentRecordType, bool useTitleId, unsigned long long titleId, ContentProviderEntry* entries, uint32_t entryCount) = 0;
    IFileSysNCA * GetContentProviderEntry(uint64_t title_id, LoaderContentRecordType type) = 0;
    IFileSysNACP * GetPMControlMetadata(uint64_t programID) = 0;
};

EXPORT ISystemloader * CALL CreateSystemLoader(ISystemModules & modules);
EXPORT void CALL DestroySystemLoader(ISystemloader * systemloader);
