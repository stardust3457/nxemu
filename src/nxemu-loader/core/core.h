// SPDX-FileCopyrightText: 2014 Citra Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "core/file_sys/vfs/vfs_types.h"

namespace Core {

FileSys::VirtualFile GetGameFileFromPath(const FileSys::VirtualFilesystem& vfs,
                                         const std::string& path);

} // namespace Core
