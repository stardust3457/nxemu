// SPDX-FileCopyrightText: Copyright 2024 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "core/file_sys/nca_metadata.h"
#include "core/file_sys/savedata_factory.h"
#include "core/file_sys/vfs/vfs_types.h"
#include "core/hle/result.h"

namespace Service::FileSystem {

class SaveDataController {
public:
    explicit SaveDataController(Systemloader & loader_, std::shared_ptr<FileSys::SaveDataFactory> factory_);
    ~SaveDataController();

    Result CreateSaveData(FileSys::VirtualDir* out_save_data, SaveDataSpaceId space, const SaveDataAttribute& attribute);
    Result OpenSaveData(FileSys::VirtualDir* out_save_data, SaveDataSpaceId space, const SaveDataAttribute& attribute);
    Result OpenSaveDataSpace(FileSys::VirtualDir* out_save_data_space, SaveDataSpaceId space);

    FileSys::SaveDataSize ReadSaveDataSize(SaveDataType type, u64 title_id, u128 user_id);
    void WriteSaveDataSize(SaveDataType type, u64 title_id, u128 user_id,FileSys::SaveDataSize new_value);
    void SetAutoCreate(bool state);

private:
    Systemloader & loader;
    const std::shared_ptr<FileSys::SaveDataFactory> factory;
};

} // namespace Service::FileSystem

class SaveDataControllerPtr :
    public ISaveDataController
{
public:
    SaveDataControllerPtr();
    SaveDataControllerPtr(std::shared_ptr<Service::FileSystem::SaveDataController> saveDataController);
    SaveDataControllerPtr(SaveDataControllerPtr&& other) noexcept;
    ~SaveDataControllerPtr();

    SaveDataControllerPtr & operator=(SaveDataControllerPtr && other) noexcept;

    Service::FileSystem::SaveDataController * operator->() const;
    Service::FileSystem::SaveDataController & operator*() const;
    operator bool() const;

    // ISaveDataController
    uint32_t CreateSaveData(IVirtualDirectory ** out_save_data, SaveDataSpaceId space, const SaveDataAttribute & attribute) override;
    void Release() override;

private:
    SaveDataControllerPtr(const SaveDataControllerPtr &) = delete;
    SaveDataControllerPtr & operator=(const SaveDataControllerPtr &) = delete;

    std::shared_ptr<Service::FileSystem::SaveDataController> m_saveDataController;
};
