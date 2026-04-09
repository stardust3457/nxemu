// SPDX-FileCopyrightText: Copyright 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "yuzu_common/guest_memory.h"
#include "yuzu_video_core/host1x/gpu_device_memory_manager.h"

namespace Tegra {
class MemoryManager;
}

namespace Tegra::Memory {

template <typename T, GuestMemoryFlags FLAGS>
using DeviceGuestMemory = GuestMemory<Tegra::MaxwellDeviceMemoryManager, T, FLAGS>;
template <typename T, GuestMemoryFlags FLAGS>
using DeviceGuestMemoryScoped = GuestMemoryScoped<Tegra::MaxwellDeviceMemoryManager, T, FLAGS>;
template <typename T, GuestMemoryFlags FLAGS>
using GpuGuestMemory = GuestMemory<Tegra::MemoryManager, T, FLAGS>;
template <typename T, GuestMemoryFlags FLAGS>
using GpuGuestMemoryScoped = GuestMemoryScoped<Tegra::MemoryManager, T, FLAGS>;

} // namespace Tegra::Memory
