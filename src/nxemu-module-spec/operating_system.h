#pragma once
#include "base.h"
#include <stdint.h>

enum class ProgramAddressSpaceType : uint8_t
{
    Is32Bit = 0,
    Is36Bit = 1,
    Is32BitNoMap = 2,
    Is39Bit = 3,
};

enum class PoolPartition : uint32_t
{
    Application = 0,
    Applet = 1,
    System = 2,
    SystemNonSecure = 3,
};

enum class StorageId : uint8_t {
    None = 0,
    Host = 1,
    GameCard = 2,
    NandSystem = 3,
    NandUser = 4,
    SdCard = 5,
};

__interface IProgramMetadata
{
    bool Is64BitProgram() const = 0;
    ProgramAddressSpaceType GetAddressSpaceType() const = 0;
    uint8_t GetMainThreadPriority() const = 0;
    uint8_t GetMainThreadCore() const = 0;
    uint32_t GetMainThreadStackSize() const = 0;
    uint64_t GetTitleID() const = 0;
    uint64_t GetFilesystemPermissions() const = 0;
    uint32_t GetSystemResourceSize() const = 0;
    PoolPartition GetPoolPartition() const = 0;
    const uint32_t * GetKernelCapabilities() const = 0;
    uint32_t GetKernelCapabilitiesSize() const = 0;
    const char * GetName() const;
};

__interface IModuleInfo
{
    const uint8_t * Data(void) const = 0;
    uint32_t DataSize(void) const = 0;
    uint64_t CodeSegmentAddr(void) const = 0;
    uint64_t CodeSegmentOffset(void) const = 0;
    uint64_t CodeSegmentSize(void) const = 0;
    uint64_t RODataSegmentAddr(void) const = 0;
    uint64_t RODataSegmentOffset(void) const = 0;
    uint64_t RODataSegmentSize(void) const = 0;
    uint64_t DataSegmentAddr(void) const = 0;
    uint64_t DataSegmentOffset(void) const = 0;
    uint64_t DataSegmentSize(void) const = 0;
};

__interface IDeviceMemory
{
    const uint8_t * BackingBasePointer() const = 0;
};

__interface ICacheInvalidator 
{
    virtual void OnCacheInvalidation(uint64_t address, uint32_t size) = 0;
};

typedef void (*DeviceEnumCallback)(const char * device, void * userData);

__interface IOperatingSystem
{
    bool Initialize() = 0;
    bool CreateApplicationProcess(uint64_t codeSize, const IProgramMetadata & metaData, uint64_t & baseAddress, uint64_t & processID, bool is_hbl) = 0;
    void StartApplicationProcess(int32_t priority, int64_t stackSize, uint32_t version, StorageId baseGameStorageId, StorageId updateStorageId, uint8_t * nacpData, uint32_t nacpDataLen) = 0;
    bool LoadModule(const IModuleInfo & module, uint64_t baseAddress) = 0;
    IDeviceMemory & DeviceMemory() = 0;
    void KeyboardKeyPress(int modifier, int keyIndex, int keyCode) = 0;
    void KeyboardKeyRelease(int modifier, int keyIndex, int keyCode) = 0;
    void GatherGPUDirtyMemory(ICacheInvalidator * invalidator) = 0;
    uint64_t GetGPUTicks() = 0;
    uint64_t GetProgramId() = 0;
    void GameFrameEnd() = 0;
    void AudioGetSyncIDs(uint32_t * ids, uint32_t maxCount, uint32_t * actualCount) = 0;
    void AudioGetDeviceListForSink(uint32_t sinkId, bool capture, DeviceEnumCallback callback, void * userData) = 0;
    void RegisterHostThread() = 0;
};

EXPORT IOperatingSystem * CALL CreateOperatingSystem(ISwitchSystem & system);
EXPORT void CALL DestroyOperatingSystem(IOperatingSystem * operatingSystem);
