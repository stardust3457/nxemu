// SPDX-FileCopyrightText: 2014 Citra Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "yuzu_common/fs/fs.h"
#include "yuzu_common/string_util.h"
#include "core/core.h"
#include "core/file_sys/registered_cache.h"
#include "core/file_sys/vfs/vfs_concat.h"
#include "core/file_sys/vfs/vfs.h"
#include <fmt/format.h>

namespace Core {


FileSys::VirtualFile GetGameFileFromPath(const FileSys::VirtualFilesystem& vfs,
                                         const std::string& path) {
    // To account for split 00+01+etc files.
    std::string dir_name;
    std::string filename;
    Common::SplitPath(path, &dir_name, &filename, nullptr);

    if (filename == "00") {
        const auto dir = vfs->OpenDirectory(dir_name, VirtualFileOpenMode::Read);
        std::vector<FileSys::VirtualFile> concat;

        for (u32 i = 0; i < 0x10; ++i) {
            const auto file_name = fmt::format("{:02X}", i);
            auto next = dir->GetFile(file_name);

            if (next != nullptr) {
                concat.push_back(std::move(next));
            } else {
                next = dir->GetFile(file_name);

                if (next == nullptr) {
                    break;
                }

                concat.push_back(std::move(next));
            }
        }

        return FileSys::ConcatenatedVfsFile::MakeConcatenatedFile(dir->GetName(),
                                                                  std::move(concat));
    }

    if (Common::FS::IsDir(path)) {
        return vfs->OpenFile(path + "/main", VirtualFileOpenMode::Read);
    }

    return vfs->OpenFile(path, VirtualFileOpenMode::Read);
}

} // namespace Core
