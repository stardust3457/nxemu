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
    uint64_t created{};
    uint64_t accessed{};
    uint64_t modified{};
    uint64_t padding{};
};

} // namespace FileSys

class VirtualDirectoryImpl :
    public IVirtualDirectory
{
public:
    VirtualDirectoryImpl();
    VirtualDirectoryImpl(FileSys::VirtualDir & directory);
    VirtualDirectoryImpl(VirtualDirectoryImpl && other) noexcept;
    ~VirtualDirectoryImpl();

    VirtualDirectoryImpl & operator=(VirtualDirectoryImpl && other) noexcept;

    FileSys::VfsDirectory * operator->() const;
    FileSys::VfsDirectory & operator*() const;
    operator bool() const;

    // IVirtualDirectory
    IVirtualDirectory * CreateSubdirectory(const char * path) const override;
    IVirtualDirectory * GetDirectoryRelative(const char * path) const override;
    IVirtualDirectory * GetSubdirectory(const char * name) const override;
    IVirtualDirectory * Duplicate() override;
    IVirtualFile * CreateFile(const char * name) const override;
    IVirtualFile * GetFile(const char * name) const override;
    IVirtualFile * GetFileRelative(const char * relative_path) const override;
    IVirtualFile * OpenFile(const char * path, VirtualFileOpenMode perms) override;
    void Release() override;

private:
    VirtualDirectoryImpl(const VirtualDirectoryImpl &) = delete;
    VirtualDirectoryImpl & operator=(const VirtualDirectoryImpl &) = delete;

    FileSys::VirtualDir m_directory;
};

class VirtualFileImpl :
    public IVirtualFile
{
public:
    VirtualFileImpl();
    VirtualFileImpl(FileSys::VirtualFile & file);
    VirtualFileImpl(VirtualFileImpl && other) noexcept;
    ~VirtualFileImpl();

    VirtualFileImpl & operator=(VirtualFileImpl && other) noexcept;

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
    VirtualFileImpl(const VirtualFileImpl &) = delete;
    VirtualFileImpl & operator=(const VirtualFileImpl &) = delete;

    FileSys::VirtualFile m_file;
};
