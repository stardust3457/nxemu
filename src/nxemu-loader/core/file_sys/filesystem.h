// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <map>
#include <memory>
#include <mutex>
#include <yuzu_common/common_types.h>
#include <nxemu-module-spec/system_loader.h>
#include "core/file_sys/vfs/vfs_types.h"

class Systemloader;

namespace FileSys {
class RomFSFactory;
class SaveDataFactory;
class SDMCFactory;
class BISFactory;
class VfsFilesystem;
class RegisteredCache;

using ProcessId = u64;
using ProgramId = u64; 

class FileSystemController :
    public IFileSystemController
{
public:
    explicit FileSystemController(Systemloader & loader_);
    ~FileSystemController();

    bool RegisterProcess(ProcessId process_id, ProgramId program_id, std::shared_ptr<FileSys::RomFSFactory>&& romfs_factory);

    FileSys::RegisteredCache * SystemNANDContents() const;

    FileSys::VirtualDir GetSDMCModificationLoadRoot(u64 title_id) const;
    FileSys::VirtualDir GetModificationLoadRoot(u64 title_id) const;
    FileSys::VirtualDir GetModificationDumpRoot(u64 title_id) const;

    void CreateFactories(FileSys::VfsFilesystem & vfs, bool overwrite = true);

    //IFileSystemController
    IFileSysRegisteredCache * GetSystemNANDContents() const override;
    ISaveDataController * OpenSaveDataController() const override;
    uint64_t GetFreeSpaceSize(StorageId id) const override;
    uint64_t GetTotalSpaceSize(StorageId id) const override;
    bool OpenProcess(uint64_t * programId, ISaveDataFactory ** saveDataFactory, IRomFsController ** romFsController, uint64_t processId) override;
    bool OpenSDMC(IVirtualDirectory ** out_sdmc) const override;

private:
    std::shared_ptr<FileSys::SaveDataFactory> CreateSaveDataFactory(ProgramId program_id);

    Systemloader & loader;

    struct Registration {
        ProgramId program_id;
        std::shared_ptr<FileSys::RomFSFactory> romfs_factory;
        std::shared_ptr<FileSys::SaveDataFactory> save_data_factory;
    };
    std::mutex registration_lock;
    std::map<ProcessId, Registration> registrations;

    std::unique_ptr<FileSys::SDMCFactory> sdmc_factory;
    std::unique_ptr<FileSys::BISFactory> bis_factory;
};

} // namespace FileSys
