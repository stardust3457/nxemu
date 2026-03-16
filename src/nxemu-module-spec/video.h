#pragma once
#include "base.h"
#include <stdint.h>

nxinterface IMemory;

struct VideoFramebufferConfig 
{
    uint64_t address;
    uint32_t offset;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t pixelFormat;
    uint32_t transformFlags;
    int32_t cropLeft;
    int32_t cropTop;
    int32_t cropRight;
    int32_t cropBottom;
    uint32_t blending;
};

struct VideoNvFence
{
    int32_t id;
    uint32_t value;
};

struct RasterizerDownloadArea 
{
    uint64_t startAddress;
    uint64_t endAddress;
    bool preemtive;
};

nxinterface IChannelState
{
    virtual bool Initialized() const = 0;
    virtual int32_t BindId() const = 0;
    virtual void Init(uint64_t programId) = 0;
    virtual void SetMemoryManager(uint32_t id) = 0;
    virtual void Release() = 0;
};

typedef void (*DeviceMemoryOperation)(uint64_t device_address, void* user_data);
typedef void (*HostActionCallback)(uint32_t slot, void * userData);

nxinterface IVideo
{
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual uint32_t AllocAsEx(uint64_t addressSpaceBits, uint64_t splitAddress, uint64_t bigPageBits, uint64_t pageBits) = 0;
    virtual uint64_t MapBufferEx(uint32_t gmmu, uint64_t gpuAddr, uint64_t deviceAddr, uint64_t size, uint16_t kind, bool isBigPages) = 0;
    virtual uint64_t MapSparse(uint32_t gmmu, uint64_t gpuAddr, uint64_t size, bool isBigPages) = 0;
    virtual void Unmap(uint32_t gmmu, uint64_t gpuAddr, uint64_t size) = 0;
    virtual uint64_t Host1xMemoryAllocate(uint64_t size) = 0;
    virtual uint32_t Host1xAllocatorAllocate(uint32_t size) = 0;
    virtual void Host1xAllocatorFree(uint32_t address, uint32_t size) = 0;
    virtual void Host1xGMMUMap(uint64_t address, uint64_t virtual_address, uint64_t size) = 0;
    virtual void Host1xGMMUUnmap(uint64_t address, uint64_t size) = 0;
    virtual void Host1xMemoryMap(uint64_t address, uint64_t virtualAddress, uint64_t size, uint64_t asid, bool track) = 0;
    virtual void Host1xMemoryUnmap(uint64_t address, uint64_t size) = 0;
    virtual void Host1xFree(uint64_t regionStart, uint64_t regionSize) = 0;
    virtual void Host1xMemoryTrackContinuity(uint64_t address, uint64_t virtualAddress, uint64_t size, uint64_t asid) = 0;
    virtual void RequestComposite(VideoFramebufferConfig * layers, uint32_t layerCount, VideoNvFence * fences, uint32_t fenceCount) = 0;
    virtual uint64_t Host1xRegisterProcess(IMemory * memory) = 0;
    virtual void UpdateFramebufferLayout(uint32_t width, uint32_t height) = 0;
    virtual IChannelState * AllocateChannel() = 0;
    virtual void PushGPUEntries(int32_t bindId, const uint64_t * commandList, uint32_t commandListSize, const uint32_t * prefetchCommandlist, uint32_t prefetchCommandlistSize) = 0;
    virtual void PushCommandBuffer(int32_t bindId, const uint32_t * commandList, uint32_t commandListSize) = 0;
    virtual void ApplyOpOnDeviceMemoryPointer(const uint8_t * pointer, uint32_t * scratchBuffer, size_t scratchBufferSize, DeviceMemoryOperation operation, void * userData) = 0;
    virtual RasterizerDownloadArea OnCPURead(uint64_t addr, uint64_t size) = 0;
    virtual bool OnCPUWrite(uint64_t addr, uint64_t size) = 0;
    virtual void Host1xUnregisterProcess(uint64_t asid) = 0;
    virtual void DeregisterHostAction(uint32_t syncpoint_id, uint32_t handle) = 0;
    virtual uint32_t HostSyncpointValue(uint32_t id) = 0;
    virtual uint32_t HostSyncpointRegisterAction(uint32_t fence_id, uint32_t target_value, HostActionCallback operation, uint32_t slot, void * userData) = 0;
    virtual void WaitHost(uint32_t syncpoint_id, uint32_t expected_value) = 0;
    virtual uint32_t ShadersBuilding() = 0;
    virtual bool UseNvdec() = 0;
    virtual void ClearCdmaInstance(uint32_t id) = 0;
};

EXPORT IVideo * CALL CreateVideo(IRenderWindow & renderWindow, ISystemModules & modules);
EXPORT void CALL DestroyVideo(IVideo * video);
