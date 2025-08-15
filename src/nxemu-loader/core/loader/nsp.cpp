// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <vector>

#include "yuzu_common/common_types.h"
#include "core/core.h"
#include "core/file_sys/content_archive.h"
#include "core/file_sys/control_metadata.h"
#include "core/file_sys/filesystem.h"
#include "core/file_sys/nca_metadata.h"
#include "core/file_sys/patch_manager.h"
#include "core/file_sys/registered_cache.h"
#include "core/file_sys/romfs_factory.h"
#include "core/file_sys/submission_package.h"
#include "core/loader/deconstructed_rom_directory.h"
#include "core/loader/nca.h"
#include "core/loader/nsp.h"
#include "system_loader.h"

namespace Loader {

AppLoader_NSP::AppLoader_NSP(FileSys::VirtualFile file_,
                             const FileSys::FileSystemController& fsc,
                             const FileSys::ContentProvider& content_provider, u64 program_id,
                             std::size_t program_index)
    : AppLoader(file_), nsp(std::make_unique<FileSys::NSP>(file_, program_id, program_index)) {

    if (nsp->GetStatus() != LoaderResultStatus::Success) {
        return;
    }

    if (nsp->IsExtractedType()) {
        secondary_loader = std::make_unique<AppLoader_DeconstructedRomDirectory>(
            nsp->GetExeFS(), false, file->GetName() == "hbl.nsp");
    } else {
        const auto control_nca =
            nsp->GetNCA(nsp->GetProgramTitleID(), LoaderContentRecordType::Control);
        if (control_nca == nullptr || control_nca->GetStatus() != LoaderResultStatus::Success) {
            return;
        }

        std::tie(nacp_file, icon_file) = [this, &content_provider, &control_nca, &fsc] {
            const FileSys::PatchManager pm{nsp->GetProgramTitleID(), fsc, content_provider};
            return pm.ParseControlNCA(*control_nca);
        }();

        secondary_loader = std::make_unique<AppLoader_NCA>(
            nsp->GetNCAFile(nsp->GetProgramTitleID(), LoaderContentRecordType::Program));
    }
}

AppLoader_NSP::~AppLoader_NSP() = default;

FileType AppLoader_NSP::IdentifyType(const FileSys::VirtualFile& nsp_file) {
    const FileSys::NSP nsp(nsp_file);

    if (nsp.GetStatus() == LoaderResultStatus::Success) {
        // Extracted Type case
        if (nsp.IsExtractedType() && nsp.GetExeFS() != nullptr &&
            FileSys::IsDirectoryExeFS(nsp.GetExeFS())) {
            return FileType::NSP;
        }

        // Non-Extracted Type case
        const auto program_id = nsp.GetProgramTitleID();
        if (!nsp.IsExtractedType() &&
            nsp.GetNCA(program_id, LoaderContentRecordType::Program) != nullptr &&
            AppLoader_NCA::IdentifyType(
                nsp.GetNCAFile(program_id, LoaderContentRecordType::Program)) == FileType::NCA) {
            return FileType::NSP;
        }
    }
    return FileType::Error;
}

AppLoader_NSP::LoadResult AppLoader_NSP::Load(Systemloader & loader) {
    if (is_loaded) {
        return {LoaderResultStatus::ErrorAlreadyLoaded, {}};
    }

    const auto title_id = nsp->GetProgramTitleID();

    if (!nsp->IsExtractedType() && title_id == 0) {
        return {LoaderResultStatus::ErrorNSPMissingProgramNCA, {}};
    }

    const auto nsp_status = nsp->GetStatus();
    if (nsp_status != LoaderResultStatus::Success) {
        return {nsp_status, {}};
    }

    const auto nsp_program_status = nsp->GetProgramStatus();
    if (nsp_program_status != LoaderResultStatus::Success) {
        return {nsp_program_status, {}};
    }

    if (!nsp->IsExtractedType() &&
        nsp->GetNCA(title_id, LoaderContentRecordType::Program) == nullptr) {
        return {LoaderResultStatus::ErrorNSPMissingProgramNCA, {}};
    }

    const auto result = secondary_loader->Load(loader);
    if (result.first != LoaderResultStatus::Success) {
        return result;
    }

    if (nsp->IsExtractedType()) {
        UNIMPLEMENTED();
    }

    FileSys::VirtualFile update_raw;
    if (ReadUpdateRaw(update_raw) == LoaderResultStatus::Success && update_raw != nullptr) {
        loader.GetFileSystemController().SetPackedUpdate(result.second->process_id,
                                                         std::move(update_raw));
    }

    is_loaded = true;
    return result;
}

LoaderResultStatus AppLoader_NSP::VerifyIntegrity(std::function<bool(size_t, size_t)> progress_callback) {
    // Extracted-type NSPs can't be verified.
    if (nsp->IsExtractedType()) {
        return LoaderResultStatus::ErrorIntegrityVerificationNotImplemented;
    }

    // Get list of all NCAs.
    const auto ncas = nsp->GetNCAsCollapsed();

    size_t total_size = 0;
    size_t processed_size = 0;

    // Loop over NCAs, collecting the total size to verify.
    for (const auto& nca : ncas) {
        total_size += nca->GetBaseFile()->GetSize();
    }

    // Loop over NCAs again, verifying each.
    for (const auto& nca : ncas) {
        AppLoader_NCA loader_nca(nca->GetBaseFile());

        const auto NcaProgressCallback = [&](size_t nca_processed_size, size_t nca_total_size) {
            return progress_callback(processed_size + nca_processed_size, total_size);
        };

        const auto verification_result = loader_nca.VerifyIntegrity(NcaProgressCallback);
        if (verification_result != LoaderResultStatus::Success) {
            return verification_result;
        }

        processed_size += nca->GetBaseFile()->GetSize();
    }

    return LoaderResultStatus::Success;
}

LoaderResultStatus AppLoader_NSP::ReadRomFS(FileSys::VirtualFile& out_file) {
    return secondary_loader->ReadRomFS(out_file);
}

LoaderResultStatus AppLoader_NSP::ReadUpdateRaw(FileSys::VirtualFile& out_file) {
    if (nsp->IsExtractedType()) {
        return LoaderResultStatus::ErrorNoPackedUpdate;
    }

    const auto read = nsp->GetNCAFile(FileSys::GetUpdateTitleID(nsp->GetProgramTitleID()),
                                      LoaderContentRecordType::Program);

    if (read == nullptr) {
        return LoaderResultStatus::ErrorNoPackedUpdate;
    }

    const auto nca_test = std::make_shared<FileSys::NCA>(read);
    if (nca_test->GetStatus() != LoaderResultStatus::ErrorMissingBKTRBaseRomFS) {
        return nca_test->GetStatus();
    }

    out_file = read;
    return LoaderResultStatus::Success;
}

LoaderResultStatus AppLoader_NSP::ReadProgramId(u64& out_program_id) {
    out_program_id = nsp->GetProgramTitleID();
    if (out_program_id == 0) {
        return LoaderResultStatus::ErrorNotInitialized;
    }
    return LoaderResultStatus::Success;
}

LoaderResultStatus AppLoader_NSP::ReadProgramIds(std::vector<u64>& out_program_ids) {
    out_program_ids = nsp->GetProgramTitleIDs();
    return LoaderResultStatus::Success;
}

LoaderResultStatus AppLoader_NSP::ReadIcon(std::vector<u8>& buffer) {
    if (icon_file == nullptr) {
        return LoaderResultStatus::ErrorNoControl;
    }

    buffer = icon_file->ReadAllBytes();
    return LoaderResultStatus::Success;
}

LoaderResultStatus AppLoader_NSP::ReadTitle(std::string& title) {
    if (nacp_file == nullptr) {
        return LoaderResultStatus::ErrorNoControl;
    }

    title = nacp_file->GetApplicationName();
    return LoaderResultStatus::Success;
}

LoaderResultStatus AppLoader_NSP::ReadControlData(FileSys::NACP& nacp) {
    if (nacp_file == nullptr) {
        return LoaderResultStatus::ErrorNoControl;
    }

    nacp = *nacp_file;
    return LoaderResultStatus::Success;
}

LoaderResultStatus AppLoader_NSP::ReadManualRomFS(FileSys::VirtualFile& out_file) {
    UNIMPLEMENTED();
    return LoaderResultStatus::ErrorNoRomFS;
}

LoaderResultStatus AppLoader_NSP::ReadBanner(std::vector<u8>& buffer) {
    return secondary_loader->ReadBanner(buffer);
}

LoaderResultStatus AppLoader_NSP::ReadLogo(std::vector<u8>& buffer) {
    return secondary_loader->ReadLogo(buffer);
}

LoaderResultStatus AppLoader_NSP::ReadNSOModules(Modules& modules) {
    return secondary_loader->ReadNSOModules(modules);
}

} // namespace Loader
