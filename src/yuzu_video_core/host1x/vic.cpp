// SPDX-FileCopyrightText: Copyright 2020 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <array>

extern "C" {
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
}

#include "yuzu_common/yuzu_assert.h"
#include "yuzu_common/bit_field.h"
#include "yuzu_common/logging/log.h"

#include "yuzu_video_core/engines/maxwell_3d.h"
#include "yuzu_video_core/host1x/host1x.h"
#include "yuzu_video_core/host1x/nvdec.h"
#include "yuzu_video_core/host1x/vic.h"
#include "yuzu_video_core/memory_manager.h"
#include "yuzu_video_core/textures/decoders.h"

namespace Tegra {

namespace Host1x {

namespace {
enum class VideoPixelFormat : u64_le {
    RGBA8 = 0x1f,
    BGRA8 = 0x20,
    RGBX8 = 0x23,
    YUV420 = 0x44,
};
} // Anonymous namespace

union VicConfig {
    u64_le raw{};
    BitField<0, 7, VideoPixelFormat> pixel_format;
    BitField<7, 2, u64_le> chroma_loc_horiz;
    BitField<9, 2, u64_le> chroma_loc_vert;
    BitField<11, 4, u64_le> block_linear_kind;
    BitField<15, 4, u64_le> block_linear_height_log2;
    BitField<32, 14, u64_le> surface_width_minus1;
    BitField<46, 14, u64_le> surface_height_minus1;
};

Vic::Vic(Host1x& host1x_, std::shared_ptr<Nvdec> nvdec_processor_) :
    host1x(host1x_),
    nvdec_processor(std::move(nvdec_processor_))   
{
}

Vic::~Vic() = default;

void Vic::ProcessMethod(Method method, u32 argument) {
    LOG_DEBUG(HW_GPU, "Vic method 0x{:X}", static_cast<u32>(method));
    const u64 arg = static_cast<u64>(argument) << 8;
    switch (method) {
    case Method::Execute:
        Execute();
        break;
    case Method::SetConfigStructOffset:
        config_struct_address = arg;
        break;
    case Method::SetOutputSurfaceLumaOffset:
        output_surface_luma_address = arg;
        break;
    case Method::SetOutputSurfaceChromaOffset:
        output_surface_chroma_address = arg;
        break;
    default:
        break;
    }
}

void Vic::Execute() {
    UNIMPLEMENTED();
}

} // namespace Host1x

} // namespace Tegra
