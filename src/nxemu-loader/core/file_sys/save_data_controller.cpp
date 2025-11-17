// SPDX-FileCopyrightText: Copyright 2024 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "core/core.h"
#include "core/file_sys/control_metadata.h"
#include "core/file_sys/errors.h"
#include "core/file_sys/patch_manager.h"
#include "core/file_sys/save_data_controller.h"
#include "core/loader/loader.h"
#include "system_loader.h"

namespace Service::FileSystem {

namespace {

// A default size for normal/journal save data size if application control metadata cannot be found.
// This should be large enough to satisfy even the most extreme requirements (~4.2GB)
constexpr uint64_t SufficientSaveDataSize = 0xF0000000;

FileSys::SaveDataSize GetDefaultSaveDataSize(Systemloader & loader, uint64_t program_id) {
    const FileSys::PatchManager pm{program_id, loader.GetFileSystemController(),
                                   loader.GetContentProvider()};
    const auto metadata = pm.GetControlMetadata();
    const auto& nacp = metadata.first;

    if (nacp != nullptr) {
        return {nacp->GetDefaultNormalSaveSize(), nacp->GetDefaultJournalSaveSize()};
    }

    return {SufficientSaveDataSize, SufficientSaveDataSize};
}

} // namespace

SaveDataController::SaveDataController(Systemloader & loader_,
                                       std::shared_ptr<FileSys::SaveDataFactory> factory_)
    : loader{loader_}, factory{std::move(factory_)} {}
SaveDataController::~SaveDataController() = default;

Result SaveDataController::CreateSaveData(FileSys::VirtualDir* out_save_data,
                                          SaveDataSpaceId space,
                                          const SaveDataAttribute& attribute) {
    LOG_TRACE(Service_FS, "Creating Save Data for space_id={:01X}, save_struct=[title_id={:016X}, user_id={:016X}{:016X}, save_id={:016X}, type={:02X}, rank={}, index={}]", space,
              attribute.program_id, attribute.user_id[1], attribute.user_id[0], attribute.system_save_data_id, static_cast<u8>(attribute.type), static_cast<u8>(attribute.rank), attribute.index);

    auto save_data = factory->Create(space, attribute);
    if (save_data == nullptr) {
        return FileSys::ResultTargetNotFound;
    }

    *out_save_data = save_data;
    return ResultSuccess;
}

Result SaveDataController::OpenSaveData(FileSys::VirtualDir * out_save_data, SaveDataSpaceId space, const SaveDataAttribute & attribute)
{
    auto save_data = factory->Open(space, attribute);
    if (save_data == nullptr) {
        return FileSys::ResultTargetNotFound;
    }

    *out_save_data = save_data;
    return ResultSuccess;
}

Result SaveDataController::OpenSaveDataSpace(FileSys::VirtualDir* out_save_data_space, SaveDataSpaceId space) 
{
    auto save_data_space = factory->GetSaveDataSpaceDirectory(space);
    if (save_data_space == nullptr) {
        return FileSys::ResultTargetNotFound;
    }

    *out_save_data_space = save_data_space;
    return ResultSuccess;
}

FileSys::SaveDataSize SaveDataController::ReadSaveDataSize(SaveDataType type, uint64_t title_id, u128 user_id) 
{
    const auto value = factory->ReadSaveDataSize(type, title_id, user_id);

    if (value.normal == 0 && value.journal == 0) {
        const auto size = GetDefaultSaveDataSize(loader, title_id);
        factory->WriteSaveDataSize(type, title_id, user_id, size);
        return size;
    }

    return value;
}

void SaveDataController::WriteSaveDataSize(SaveDataType type, uint64_t title_id, u128 user_id,
                                           FileSys::SaveDataSize new_value) {
    factory->WriteSaveDataSize(type, title_id, user_id, new_value);
}

void SaveDataController::SetAutoCreate(bool state) {
    factory->SetAutoCreate(state);
}

} // namespace Service::FileSystem

SaveDataControllerPtr::SaveDataControllerPtr(std::shared_ptr<Service::FileSystem::SaveDataController> saveDataController) :
    m_saveDataController(saveDataController)
{
}

SaveDataControllerPtr::~SaveDataControllerPtr()
{
}

uint32_t SaveDataControllerPtr::CreateSaveData(IVirtualDirectory ** out_save_data, SaveDataSpaceId space, const SaveDataAttribute & attribute)
{
    FileSys::VirtualDir out_dir;
    Result result = m_saveDataController->CreateSaveData(&out_dir, space, attribute);
    *out_save_data = std::make_unique<VirtualDirectoryPtr>(out_dir).release();
    return result.raw;
}

void SaveDataControllerPtr::Release()
{
    delete this;
}
