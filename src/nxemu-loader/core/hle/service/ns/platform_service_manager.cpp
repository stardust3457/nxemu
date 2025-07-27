// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <algorithm>
#include <cstring>
#include <vector>

#include "yuzu_common/yuzu_assert.h"
#include "yuzu_common/common_types.h"
#include "yuzu_common/logging/log.h"
#include "yuzu_common/swap.h"
#include "core/core.h"
#include "core/file_sys/content_archive.h"
#include "core/file_sys/nca_metadata.h"
#include "core/file_sys/registered_cache.h"
#include "core/file_sys/romfs.h"
#include "core/file_sys/system_archive/system_archive.h"
#include "core/hle/kernel/physical_memory.h"
#include "core/hle/service/ns/platform_service_manager.h"

namespace Service::NS {

struct FontRegion {
    u32 offset;
    u32 size;
};

// The below data is specific to shared font data dumped from Switch on f/w 2.2
// Virtual address and offsets/sizes likely will vary by dump
[[maybe_unused]] constexpr VAddr SHARED_FONT_MEM_VADDR{0x00000009d3016000ULL};
constexpr u32 EXPECTED_RESULT{0x7f9a0218}; // What we expect the decrypted bfttf first 4 bytes to be
constexpr u32 EXPECTED_MAGIC{0x36f81a1e};  // What we expect the encrypted bfttf first 4 bytes to be
constexpr u64 SHARED_FONT_MEM_SIZE{0x1100000};
constexpr FontRegion EMPTY_REGION{0, 0};

static void DecryptSharedFont(const std::vector<u32>& input, Kernel::PhysicalMemory& output,
                              std::size_t& offset) {
    ASSERT_MSG(offset + (input.size() * sizeof(u32)) < SHARED_FONT_MEM_SIZE,
               "Shared fonts exceeds 17mb!");
    ASSERT_MSG(input[0] == EXPECTED_MAGIC, "Failed to derive key, unexpected magic number");

    const u32 KEY = input[0] ^ EXPECTED_RESULT; // Derive key using an inverse xor
    std::vector<u32> transformed_font(input.size());
    // TODO(ogniK): Figure out a better way to do this
    std::transform(input.begin(), input.end(), transformed_font.begin(),
                   [&KEY](u32 font_data) { return Common::swap32(font_data ^ KEY); });
    transformed_font[1] = Common::swap32(transformed_font[1]) ^ KEY; // "re-encrypt" the size
    std::memcpy(output.data() + offset, transformed_font.data(),
                transformed_font.size() * sizeof(u32));
    offset += transformed_font.size() * sizeof(u32);
}

void DecryptSharedFontToTTF(const std::vector<u32>& input, std::vector<u8>& output) {
    ASSERT_MSG(input[0] == EXPECTED_MAGIC, "Failed to derive key, unexpected magic number");

    if (input.size() < 2) {
        LOG_ERROR(Service_NS, "Input font is empty");
        return;
    }

    const u32 KEY = input[0] ^ EXPECTED_RESULT; // Derive key using an inverse xor
    std::vector<u32> transformed_font(input.size());
    // TODO(ogniK): Figure out a better way to do this
    std::transform(input.begin(), input.end(), transformed_font.begin(),
                   [&KEY](u32 font_data) { return Common::swap32(font_data ^ KEY); });
    std::memcpy(output.data(), transformed_font.data() + 2,
                (transformed_font.size() - 2) * sizeof(u32));
}

void EncryptSharedFont(const std::vector<u32>& input, std::vector<u8>& output,
                       std::size_t& offset) {
    ASSERT_MSG(offset + (input.size() * sizeof(u32)) < SHARED_FONT_MEM_SIZE,
               "Shared fonts exceeds 17mb!");

    const auto key = Common::swap32(EXPECTED_RESULT ^ EXPECTED_MAGIC);
    std::vector<u32> transformed_font(input.size() + 2);
    transformed_font[0] = Common::swap32(EXPECTED_MAGIC);
    transformed_font[1] = Common::swap32(static_cast<u32>(input.size() * sizeof(u32))) ^ key;
    std::transform(input.begin(), input.end(), transformed_font.begin() + 2,
                   [key](u32 in) { return in ^ key; });
    std::memcpy(output.data() + offset, transformed_font.data(),
                transformed_font.size() * sizeof(u32));
    offset += transformed_font.size() * sizeof(u32);
}

// Helper function to make BuildSharedFontsRawRegions a bit nicer
static u32 GetU32Swapped(const u8* data) {
    u32 value;
    std::memcpy(&value, data, sizeof(value));
    return Common::swap32(value);
}

} // namespace Service::NS
