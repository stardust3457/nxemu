// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>

#include "yuzu_common/common_types.h"
#include <nxemu-module-spec/system_loader.h>

namespace FileSys {

class VfsDirectory;
class VfsFile;
class VfsFilesystem;

// Declarations for Vfs* pointer types

using VirtualDir = std::shared_ptr<VfsDirectory>;
using VirtualFile = std::shared_ptr<VfsFile>;
using VirtualFilesystem = std::shared_ptr<VfsFilesystem>;

struct FileTimeStampRaw {
    u64 created{};
    u64 accessed{};
    u64 modified{};
    u64 padding{};
};

} // namespace FileSys

class VirtualDirectoryPtr :
    public IVirtualDirectory
{
public:
    VirtualDirectoryPtr();
    VirtualDirectoryPtr(FileSys::VirtualDir & directory);
    VirtualDirectoryPtr(VirtualDirectoryPtr && other) noexcept;
    ~VirtualDirectoryPtr();

    VirtualDirectoryPtr& operator=(VirtualDirectoryPtr && other) noexcept;

    FileSys::VfsDirectory * operator->() const;
    FileSys::VfsDirectory & operator*() const;
    operator bool() const;

    // IVirtualDirectory
    IVirtualDirectory * GetDirectoryRelative(const char * path) const override;
    IVirtualDirectory * GetSubdirectory(const char * name) const override;
    IVirtualFile * CreateFile(const char * name) const override;
    IVirtualFile * GetFile(const char * name) const override;
    IVirtualFile * GetFileRelative(const char * relative_path) const override;
    IVirtualFile * OpenFile(const char * path, VirtualFileOpenMode perms) override;
    void Release() override;

private:
    VirtualDirectoryPtr(const VirtualDirectoryPtr &) = delete;
    VirtualDirectoryPtr & operator=(const VirtualDirectoryPtr &) = delete;

    FileSys::VirtualDir m_directory;
};

class VirtualFilePtr :
    public IVirtualFile
{
public:
    VirtualFilePtr();
    VirtualFilePtr(FileSys::VirtualFile& file);
    VirtualFilePtr(VirtualFilePtr&& other) noexcept;
    ~VirtualFilePtr();

    VirtualFilePtr& operator=(VirtualFilePtr&& other) noexcept;

    FileSys::VfsFile* operator->() const;
    FileSys::VfsFile& operator*() const;
    operator bool() const;

    // IVirtualFile
    uint64_t GetSize() const override;
    bool Resize(uint64_t size) override;
    uint64_t ReadBytes(uint8_t * data, uint64_t datalen, uint64_t offset) override;
    uint64_t WriteBytes(const uint8_t * data, uint64_t datalen, uint64_t offset) override;
    IVirtualDirectory * ExtractRomFS() override;
    IVirtualFile * Duplicate() override;
    void Release() override;

private:
    VirtualFilePtr(const VirtualFilePtr&) = delete;
    VirtualFilePtr& operator=(const VirtualFilePtr&) = delete;

    FileSys::VirtualFile m_file;
};
