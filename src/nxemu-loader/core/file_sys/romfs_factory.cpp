// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <memory>
#include "yuzu_common/yuzu_assert.h"
#include "yuzu_common/common_types.h"
#include "yuzu_common/logging/log.h"
#include "core/file_sys/common_funcs.h"
#include "core/file_sys/content_archive.h"
#include "core/file_sys/filesystem.h"
#include "core/file_sys/nca_metadata.h"
#include "core/file_sys/patch_manager.h"
#include "core/file_sys/registered_cache.h"
#include "core/file_sys/romfs_factory.h"
#include "core/loader/loader.h"

namespace FileSys {

RomFSFactory::RomFSFactory(FileSys::VirtualFile file_, bool updatable_, ContentProvider & provider, FileSystemController& controller) :
    content_provider{provider}, 
    filesystem_controller{controller},
    file(file_),
    updatable(updatable_) 
{
}

RomFSFactory::~RomFSFactory() = default;

void RomFSFactory::SetPackedUpdate(VirtualFile update_raw_file) {
    packed_update_raw = std::move(update_raw_file);
}

VirtualFile RomFSFactory::OpenCurrentProcess(uint64_t current_process_title_id) const {
    if (!updatable) {
        return file;
    }

    const auto type = LoaderContentRecordType::Program;
    const auto nca = content_provider.GetEntryNCA(current_process_title_id, type);
    const FileSys::PatchManager patch_manager{current_process_title_id, filesystem_controller,
                                     content_provider};
    return patch_manager.PatchRomFS(nca.get(), file, LoaderContentRecordType::Program, packed_update_raw);
}

VirtualFile RomFSFactory::OpenPatchedRomFS(uint64_t title_id, LoaderContentRecordType type) const {
    UNIMPLEMENTED();
    return nullptr;
}

VirtualFile RomFSFactory::OpenPatchedRomFSWithProgramIndex(uint64_t title_id, u8 program_index,
                                                           LoaderContentRecordType type) const {
    const auto res_title_id = GetBaseTitleIDWithProgramIndex(title_id, program_index);

    return OpenPatchedRomFS(res_title_id, type);
}

VirtualFile RomFSFactory::Open(uint64_t title_id, StorageId storage, LoaderContentRecordType type) const {
    const std::shared_ptr<NCA> res = GetEntry(title_id, storage, type);
    if (res == nullptr) {
        return nullptr;
    }

    return res->RomFS();
}

std::shared_ptr<NCA> RomFSFactory::GetEntry(uint64_t title_id, StorageId storage, LoaderContentRecordType type) const 
{
    UNIMPLEMENTED();
    return nullptr;
}

} // namespace FileSys

RomFsControllerImpl::RomFsControllerImpl(std::shared_ptr<FileSys::RomFSFactory> factory_, uint64_t programId_) :
    m_factory(factory_),
    m_programId(programId_)
{
}

RomFsControllerImpl::~RomFsControllerImpl()
{
}

IVirtualFile * RomFsControllerImpl::OpenRomFSCurrentProcess()
{
    FileSys::VirtualFile fs = m_factory->OpenCurrentProcess(m_programId);
    return std::make_unique<VirtualFileImpl>(fs).release();
}

void RomFsControllerImpl::Release()
{
    delete this;
}
