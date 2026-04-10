// SPDX-FileCopyrightText: 2021 yuzu Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include "yuzu_video_core/host1x/host1x.h"

namespace Tegra {

namespace Host1x {

Host1x::Host1x(IDeviceMemory & deviceMemory)
    : syncpoint_manager{},
      memory_manager(deviceMemory), gmmu_manager{memory_manager, 32, 0, 12},
      allocator{std::make_unique<Common::FlatAllocator<u32, 0, 32>>(1 << 12)} {}

Host1x::~Host1x() = default;

} // namespace Host1x

} // namespace Tegra
