// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <utility>

#include "yuzu_common/yuzu_assert.h"
#include "yuzu_common/fs/fs.h"
#include "yuzu_common/fs/path_util.h"
#include "yuzu_common/settings.h"
#include "core/core.h"
#include "core/file_sys/errors.h"
#include "core/hle/service/filesystem/filesystem.h"
#include "core/hle/service/filesystem/fsp/fsp_ldr.h"
#include "core/hle/service/filesystem/fsp/fsp_pr.h"
#include "core/hle/service/filesystem/fsp/fsp_srv.h"
#include "core/hle/service/filesystem/romfs_controller.h"
#include "core/hle/service/filesystem/save_data_controller.h"
#include "core/hle/service/server_manager.h"

namespace Service::FileSystem {

static const IVirtualDirectoryPtr & GetDirectoryRelativeWrapped(const IVirtualDirectoryPtr & base, std::string_view dir_name_, IVirtualDirectoryPtr & relative_dir)
{
    std::string dir_name(Common::FS::SanitizePath(dir_name_));
    if (dir_name.empty() || dir_name == "." || dir_name == "/" || dir_name == "\\")
    {
        return base;
    }
    *relative_dir.GetAddressForSet() = base->GetDirectoryRelative(dir_name.c_str());
    return relative_dir;
}

VfsDirectoryServiceWrapper::VfsDirectoryServiceWrapper(IVirtualDirectoryPtr && backing_) : 
    backing(std::move(backing_)) 
{
}

VfsDirectoryServiceWrapper::~VfsDirectoryServiceWrapper() = default;

Result VfsDirectoryServiceWrapper::CreateFile(const std::string& path_, u64 size) const
{
    std::string path(Common::FS::SanitizePath(path_));
    
    IVirtualDirectoryPtr relativeDir;
    const IVirtualDirectoryPtr & dir = GetDirectoryRelativeWrapped(backing, Common::FS::GetParentPath(path), relativeDir);
    if (!dir)
    {
        return FileSys::ResultPathNotFound;
    }

    FileSys::DirectoryEntryType entry_type{};
    if (GetEntryType(&entry_type, path) == ResultSuccess)
    {
        return FileSys::ResultPathAlreadyExists;
    }
    IVirtualFilePtr file(dir->CreateFile(Common::FS::GetFilename(path).data()));
    if (!file)
    {
        // TODO(DarkLordZach): Find a better error code for this
        return ResultUnknown;
    }
    if (!file->Resize(size))
    {
        // TODO(DarkLordZach): Find a better error code for this
        return ResultUnknown;
    }
    return ResultSuccess;
}

Result VfsDirectoryServiceWrapper::CreateDirectory(const std::string& path_) const
{
    std::string path(Common::FS::SanitizePath(path_));

    // NOTE: This is inaccurate behavior. CreateDirectory is not recursive.
    // CreateDirectory should return PathNotFound if the parent directory does not exist.
    // This is here temporarily in order to have UMM "work" in the meantime.
    // TODO (Morph): Remove this when a hardware test verifies the correct behavior.
    const auto components = Common::FS::SplitPathComponents(path);
    std::string relative_path;
    for (const auto& component : components)
    {
        relative_path = Common::FS::SanitizePath(fmt::format("{}/{}", relative_path, component));
        auto new_dir = backing->CreateSubdirectory(relative_path.c_str());
        if (new_dir == nullptr)
        {
            // TODO(DarkLordZach): Find a better error code for this
            return ResultUnknown;
        }
    }
    return ResultSuccess;
}

Result VfsDirectoryServiceWrapper::OpenFile(IVirtualFile** out_file, const std::string& path_, VirtualFileOpenMode mode) const
{
    const std::string path(Common::FS::SanitizePath(path_));
    std::string_view npath = path;
    while (!npath.empty() && (npath[0] == '/' || npath[0] == '\\'))
    {
        npath.remove_prefix(1);
    }

    IVirtualFilePtr file(backing->GetFileRelative(npath.data()));
    if (!file)
    {
        return FileSys::ResultPathNotFound;
    }

    if (mode == VirtualFileOpenMode::AllowAppend) {
        UNIMPLEMENTED();
        //*out_file = std::make_shared<FileSys::OffsetVfsFile>(file, 0, file->GetSize());
    }
    else
    {
        *out_file = file.Detach();
    }
    return ResultSuccess;
}

Result VfsDirectoryServiceWrapper::GetEntryType(FileSys::DirectoryEntryType * out_entry_type, const std::string & path_) const
{
    std::string path(Common::FS::SanitizePath(path_));

    IVirtualDirectoryPtr relativeDir;
    const IVirtualDirectoryPtr & dir = GetDirectoryRelativeWrapped(backing, Common::FS::GetParentPath(path), relativeDir);
    if (!dir)
    {
        return FileSys::ResultPathNotFound;
    }

    std::string_view filename = Common::FS::GetFilename(path);
    // TODO(Subv): Some games use the '/' path, find out what this means.
    if (filename.empty()) 
    {
        *out_entry_type = FileSys::DirectoryEntryType::Directory;
        return ResultSuccess;
    }

    IVirtualFilePtr file(dir->GetFile(filename.data()));
    if (file)
    {
        *out_entry_type = FileSys::DirectoryEntryType::File;
        return ResultSuccess;
    }

    IVirtualDirectoryPtr subdir(dir->GetSubdirectory(filename.data()));
    if (subdir) {
        *out_entry_type = FileSys::DirectoryEntryType::Directory;
        return ResultSuccess;
    }
    return FileSys::ResultPathNotFound;
}

void LoopProcess(Core::System& system) {
    auto server_manager = std::make_unique<ServerManager>(system);

    const auto FileSystemProxyFactory = [&] { return std::make_shared<FSP_SRV>(system); };

    server_manager->RegisterNamedService("fsp-ldr", std::make_shared<FSP_LDR>(system));
    server_manager->RegisterNamedService("fsp:pr", std::make_shared<FSP_PR>(system));
    server_manager->RegisterNamedService("fsp-srv", std::move(FileSystemProxyFactory));
    ServerManager::RunServer(std::move(server_manager));
}

} // namespace Service::FileSystem
