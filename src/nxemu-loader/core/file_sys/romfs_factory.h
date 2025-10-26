// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>

#include "yuzu_common/common_types.h"
#include "core/file_sys/vfs/vfs_types.h"
#include "core/hle/result.h"

namespace Loader {
class AppLoader;
} // namespace Loader

namespace FileSys {
class FileSystemController;
}

namespace FileSys {

class ContentProvider;
class NCA;

/// File system interface to the RomFS archive
class RomFSFactory {
public:
    explicit RomFSFactory(FileSys::VirtualFile file_, bool updatable_, ContentProvider & provider,
                          FileSys::FileSystemController& controller);
    ~RomFSFactory();

    void SetPackedUpdate(VirtualFile packed_update_raw);
    [[nodiscard]] VirtualFile OpenCurrentProcess(uint64_t current_process_title_id) const;
    [[nodiscard]] VirtualFile OpenPatchedRomFS(uint64_t title_id, LoaderContentRecordType type) const;
    [[nodiscard]] VirtualFile OpenPatchedRomFSWithProgramIndex(uint64_t title_id, u8 program_index,
                                                               LoaderContentRecordType type) const;
    [[nodiscard]] VirtualFile Open(uint64_t title_id, StorageId storage, LoaderContentRecordType type) const;
    [[nodiscard]] std::shared_ptr<NCA> GetEntry(uint64_t title_id, StorageId storage,
                                                LoaderContentRecordType type) const;

private:
    VirtualFile file;
    VirtualFile packed_update_raw;

    VirtualFile base;

    bool updatable;

    ContentProvider& content_provider;
    FileSys::FileSystemController& filesystem_controller;
};

} // namespace FileSys

class RomFSFactoryPtr :
    public IRomFsController
{
public:
    RomFSFactoryPtr();
    RomFSFactoryPtr(std::shared_ptr<FileSys::RomFSFactory> romFSFactory);
    RomFSFactoryPtr(RomFSFactoryPtr && other) noexcept;
    ~RomFSFactoryPtr();

    RomFSFactoryPtr & operator=(RomFSFactoryPtr && other) noexcept;

    FileSys::RomFSFactory * operator->() const;
    FileSys::RomFSFactory & operator*() const;
    operator bool() const;

    // IRomFsController
    IVirtualFile * OpenCurrentProcess(uint64_t currentProcessTitleId) const;
    void Release() override;

private:
    RomFSFactoryPtr(const RomFSFactoryPtr &) = delete;
    RomFSFactoryPtr & operator=(const RomFSFactoryPtr &) = delete;

    std::shared_ptr<FileSys::RomFSFactory> m_romFSFactory;
};
