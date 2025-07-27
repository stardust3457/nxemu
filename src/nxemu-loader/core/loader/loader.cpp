// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <memory>
#include "yuzu_common/concepts.h"
#include "yuzu_common/fs/path_util.h"
#include "yuzu_common/logging/log.h"
#include "yuzu_common/string_util.h"
#include "yuzu_common/yuzu_assert.h"
#include "loader.h"
#include "core/loader/nro.h"

namespace Loader {

namespace {

template <Common::DerivedFrom<AppLoader> T>
std::optional<FileType> IdentifyFileLoader(FileSys::VirtualFile file) {
    const auto file_type = T::IdentifyType(file);
    if (file_type != FileType::Error) {
        return file_type;
    }
    return std::nullopt;
}

} // namespace

FileType IdentifyFile(FileSys::VirtualFile file) {
    if (const auto nro_type = IdentifyFileLoader<AppLoader_NRO>(file)) {
        return *nro_type;
    } else {
        return FileType::Unknown;
    }
}

FileType GuessFromFilename(const std::string& name) {
    const std::string extension =
        Common::ToLower(std::string(Common::FS::GetExtensionFromFilename(name)));

    if (extension == "nro")
        return FileType::NRO;

    return FileType::Unknown;
}

std::string GetFileTypeString(FileType type) {
    switch (type) {
    case FileType::NRO:
        return "NRO";
    case FileType::Error:
    case FileType::Unknown:
        break;
    }

    return "unknown";
}

AppLoader::AppLoader(FileSys::VirtualFile file_) : file(std::move(file_)) {}
AppLoader::~AppLoader() = default;

/**
 * Get a loader for a file with a specific type
 * @param system The system context to use.
 * @param file   The file to retrieve the loader for
 * @param type   The file type
 * @param program_index Specifies the index within the container of the program to launch.
 * @return std::unique_ptr<AppLoader> a pointer to a loader object;  nullptr for unsupported type
 */
static std::unique_ptr<AppLoader> GetFileLoader(Systemloader & loader, FileSys::VirtualFile file,
                                                FileType type, u64 program_id,
                                                std::size_t program_index) {
    switch (type) {
    // NX NRO file format.
    case FileType::NRO:
        return std::make_unique<AppLoader_NRO>(std::move(file));
    }
    UNIMPLEMENTED();
    return nullptr;
}

std::unique_ptr<AppLoader> GetLoader(Systemloader& loader, FileSys::VirtualFile file,
                                     u64 program_id, std::size_t program_index) {
    if (!file) {
        return nullptr;
    }

    FileType type = IdentifyFile(file);
    const FileType filename_type = GuessFromFilename(file->GetName());

    LOG_DEBUG(Loader, "Loading file {} as {}...", file->GetName(), GetFileTypeString(type));

    return GetFileLoader(loader, std::move(file), type, program_id, program_index);
}

} // namespace Loader
