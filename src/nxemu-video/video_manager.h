#pragma once
#include <nxemu-module-spec/cpu.h>
#include <nxemu-module-spec/video.h>
#include <memory>

namespace Tegra {
class MemoryManager;
}

class VideoManager :
    public IVideo
{
public:
    VideoManager(IRenderWindow & window, ISwitchSystem & system);
    ~VideoManager();

    void EmulationStarting();

    //IVideo
    bool Initialize(void) override;
    uint32_t AllocAsEx(uint64_t addressSpaceBits, uint64_t splitAddress, uint64_t bigPageBits, uint64_t pageBits) override;
    uint64_t MapBufferEx(uint64_t gpuAddr, uint64_t deviceAddr, uint64_t size, uint16_t kind, bool isBigPages) override;
    uint64_t Map(uint32_t gmmu, uint64_t gpuAddr, uint64_t deviceAddr, uint64_t size, uint16_t kind, bool isBigPages) override;
    uint64_t MapSparse(uint32_t gmmu, uint64_t gpuAddr, uint64_t size, bool isBigPages) override;
    uint64_t MemoryAllocate(uint64_t size) override;
    void Unmap(uint32_t gmmu, uint64_t gpuAddr, uint64_t size) override;
    void MemoryTrackContinuity(uint64_t address, uint64_t virtualAddress, uint64_t size, uint64_t asid) override;
    void MemoryMap(uint64_t address, uint64_t virtualAddress, uint64_t size, uint64_t asid, bool track) override;
    void MemoryUnmap(uint64_t address, uint64_t size) override;
    void RequestComposite(VideoFramebufferConfig * layers, uint32_t layerCount, VideoNvFence * fences, uint32_t fenceCount) override;
    uint64_t RegisterProcess(IMemory* memory) override;
    void UpdateFramebufferLayout(uint32_t width, uint32_t height) override;
    IChannelState * AllocateChannel() override;
    void PushGPUEntries(int32_t bindId, const uint64_t * commandList, uint32_t commandListSize, const uint32_t * prefetchCommandlist, uint32_t prefetchCommandlistSize);
    void ApplyOpOnDeviceMemoryPointer(const uint8_t * pointer, uint32_t* scratchBuffer, size_t scratchBufferSize, DeviceMemoryOperation operation, void* userData) override;
    RasterizerDownloadArea OnCPURead(uint64_t addr, uint64_t size) override;
    bool OnCPUWrite(uint64_t addr, uint64_t size) override;
    void DeregisterHostAction(uint32_t syncpoint_id, uint32_t handle) override;
    uint32_t HostSyncpointValue(uint32_t id) override;
    uint32_t HostSyncpointRegisterAction(uint32_t fence_id, uint32_t target_value, HostActionCallback operation, uint32_t slot, void * userData) override;
    void WaitHost(uint32_t syncpoint_id, uint32_t expected_value) override;
    uint32_t Host1xAllocate(uint32_t size) override;
    void Host1xMap(uint64_t address, uint64_t virtual_address, uint64_t size) override;

private:
    VideoManager() = delete;
    VideoManager(const VideoManager&) = delete;
    VideoManager& operator=(const VideoManager&) = delete;

    struct Impl;
    std::unique_ptr<Impl> impl;
};
