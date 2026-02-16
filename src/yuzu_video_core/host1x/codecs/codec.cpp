// SPDX-FileCopyrightText: Copyright 2020 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "yuzu_common/yuzu_assert.h"
#include "yuzu_common/settings.h"
#include "yuzu_video_core/host1x/codecs/codec.h"
#include "yuzu_video_core/host1x/codecs/h264.h"
#include "yuzu_video_core/host1x/codecs/vp8.h"
#include "yuzu_video_core/host1x/codecs/vp9.h"
#include "yuzu_video_core/host1x/host1x.h"
#include "yuzu_video_core/memory_manager.h"

namespace Tegra {

Codec::Codec(Host1x::Host1x& host1x_, const Host1x::NvdecCommon::NvdecRegisters& regs) : 
    host1x(host1x_),
    state{regs},
    h264_decoder(std::make_unique<Decoder::H264>(host1x)),
    vp8_decoder(std::make_unique<Decoder::VP8>(host1x)),
    vp9_decoder(std::make_unique<Decoder::VP9>(host1x))
{
}

Codec::~Codec() = default;

void Codec::Initialize()
{
    UNIMPLEMENTED();
    initialized = false;  // decode_api.Initialize(current_codec);
}

void Codec::SetTargetCodec(Host1x::NvdecCommon::VideoCodec codec)
{
    if (current_codec != codec)
    {
        current_codec = codec;
        LOG_INFO(Service_NVDRV, "NVDEC video codec initialized to {}", GetCurrentCodecName());
    }
}

Host1x::NvdecCommon::VideoCodec Codec::GetCurrentCodec() const
{
    return current_codec;
}

std::string_view Codec::GetCurrentCodecName() const
{
    switch (current_codec)
    {
    case Host1x::NvdecCommon::VideoCodec::None:
        return "None";
    case Host1x::NvdecCommon::VideoCodec::H264:
        return "H264";
    case Host1x::NvdecCommon::VideoCodec::VP8:
        return "VP8";
    case Host1x::NvdecCommon::VideoCodec::H265:
        return "H265";
    case Host1x::NvdecCommon::VideoCodec::VP9:
        return "VP9";
    default:
        return "Unknown";
    }
}
} // namespace Tegra
